/*
 * Farm+ff_farm.cpp
 *
 * MPI + FastFlow hybrid Farm Skeleton with Emitter, Worker, and Collector roles
 *
 * Description:
 *   - This program implements a three-stage farm using MPI for inter-process
 *     communication and FastFlow for intra-process parallelism:
 *
 *     1. Emitter (rank 0):
 *        - Generates 'total_tasks' work items in batches of 'batch_tasks'.
 *        - Packs each task into a byte buffer and sends batches to each Worker
 *          via non-blocking 'MPI_Isend' with double-buffering to overlap send
 *          setup and data preparation.
 *        - Receives acknowledgements ('ACK_TAG') from Workers when ready
 *          to accept the next batch, and finally sends 'EOS_TAG' to signal end.
 *
 *     2. Worker (ranks 1...size-2):
 *        - Posts two non-blocking 'MPI_Irecv' calls (double-buffered) to
 * receive incoming task batches from the Emitter.
 *        - Upon receipt, immediately sends an empty 'ACK_TAG' message back to
 *          the Emitter to overlap communication and computation.
 *        - Processes the batch using a FastFlow farm ('ff_farm'), where:
 *            - An internal Emitter node generates tasks for local Worker nodes.
 *            - Each Worker node applies 'active_delay()' to simulate
 * CPU-intensive work and copies results into an output buffer.
 *        - Sends results to the Collector (rank 'size-1') via non-blocking
 *          'MPI_Isend', using double-buffering to overlap sending and next
 * receive.
 *
 *     3. Collector (rank size-1):
 *        - Posts two non-blocking 'MPI_Irecv' calls to collect result batches
 *          from any Worker.
 *        - Uses 'MPI_Waitany' to handle incoming batches, accumulating
 * 'total_tasks' and tracking counts per Worker.
 *        - After receiving all tasks, prints per-Worker counts and elapsed
 * time.
 *
 *   - The program requires at least 3 MPI processes and initializes MPI with
 *     'MPI_THREAD_MULTIPLE' to support concurrent MPI calls from FastFlow
 * threads (MPI_THREAD_FUNNELED would be enough indeed).
 *
 * Usage:
 *   mpirun -n P ./Farm+ff_farm total_tasks task_size batch_tasks
 * threads_per_worker [psuccess]
 *
 */

#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <ff/ff.hpp>
#include <iostream>
#include <mpi.h>
#include <random>
#include <vector>

constexpr int TASK_TAG = 1;
constexpr int ACK_TAG = 2;
constexpr int RESULT_TAG = 3;
constexpr int EOS_TAG = 4;

constexpr int EXEC_TIME = 1000;
constexpr std::uint64_t MT_PRIME_SEED = 18446744073709551557ULL;

// global variables ------------------------------------
int total_tasks; // number of tasks
int task_size;   // bytes per task
int batch_tasks; // batch size (E->W and W->C have the same batch_tasks)
int th_x_worker; // number of threads used by FastFlow within a single Worker
float psuccess = 0.5;  // probability of success in the geometric distribution
double t_start, t_end; // to measure the elapsed time
// -----------------------------------------------------

// per-Worker data
struct Buf
{
    char* data;
    MPI_Request req;
    bool in_use;
};

#define CHECK_ERROR(err)                                                       \
    do                                                                         \
    {                                                                          \
        if (err != MPI_SUCCESS)                                                \
        {                                                                      \
            char errstr[MPI_MAX_ERROR_STRING];                                 \
            int errlen = 0;                                                    \
            MPI_Error_string(err, errstr, &errlen);                            \
            std::cerr << "MPI error code " << err << " ("                      \
                      << std::string(errstr, errlen) << ")"                    \
                      << " line: " << __LINE__ << "\n";                        \
            MPI_Abort(MPI_COMM_WORLD, err);                                    \
            std::abort();                                                      \
        }                                                                      \
    } while (0)

// Utility function: it simulates CPU intensive work for usecs microseconds
static inline float active_delay(int usecs)
{
    float x = 1.25f;
    auto start = std::chrono::high_resolution_clock::now();
    auto end = false;
    while (!end)
    {
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto usec =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed)
                .count();
        x *= sin(x) / atan(x) * tanh(x) * sqrt(x);
        if (usec >= usecs)
            end = true;
    }
    return x;
}

// rank 0
void Emitter(int size)
{
    int error;
    int num_workers = size - 2;
    int tasks_sent = 0;
    int eos_sent = 0;

    std::mt19937_64 rng(MT_PRIME_SEED);
    std::geometric_distribution dist(psuccess);

    // allocate and copy data into the send buffer until the batch
    // is filled or there are no more tasks, then sends the batch
    // to Worker with id rank
    auto prepare_and_send_batch = [&](Buf& buf, int rank) {
        int count = 0;
        for (int i = 0; i < batch_tasks && tasks_sent < total_tasks;
             i++, tasks_sent++)
        {
            // copy garbage into the buffer
            char* datatask = new char[task_size];
            std::memcpy(buf.data + i * task_size, datatask, task_size);

            // pertask workload
            int v = dist(rng);
            std::memcpy(buf.data + i * task_size, &v, sizeof(int));

            ++count;
            delete[] datatask;
        }
        int error = MPI_Isend(buf.data, count * task_size, MPI_BYTE, rank,
                              TASK_TAG, MPI_COMM_WORLD, &buf.req);
        CHECK_ERROR(error);
        buf.in_use = true;
    };

    // pre-allocate two buffers for each Worker
    std::vector<std::array<Buf, 2>> bufs(num_workers);
    for (int i = 0; i < num_workers; i++)
    {
        bufs[i][0].data = new char[task_size * batch_tasks];
        bufs[i][1].data = new char[task_size * batch_tasks];
        bufs[i][0].in_use = bufs[i][1].in_use = false;
        assert(bufs[i][0].data && bufs[i][1].data);
    }

    // keep track of which buffer to use at each round
    std::vector<int> whichbuffer(num_workers, 0);

    // initially all worker are ready
    // prepare and send the batch
    for (int i = 1; i <= num_workers; ++i)
    {
        prepare_and_send_batch(bufs[i - 1][0], i);
        whichbuffer[i - 1] ^= 1; // switch buffer
    }

    while (eos_sent < num_workers)
    {
        MPI_Status st;
        error = MPI_Recv(nullptr, 0, MPI_BYTE, MPI_ANY_SOURCE, ACK_TAG,
                         MPI_COMM_WORLD, &st);
        CHECK_ERROR(error);
        int ready_rank = st.MPI_SOURCE;

        if (tasks_sent < total_tasks)
        {
            int buf_id = whichbuffer[ready_rank - 1];
            auto& buf = bufs[ready_rank - 1][buf_id];

            // if the buffer is still in use, wait for send completion
            if (buf.in_use)
            {
                error = MPI_Wait(&buf.req, MPI_STATUS_IGNORE);
                CHECK_ERROR(error);
                buf.in_use = false;
            }
            prepare_and_send_batch(buf, ready_rank);
            whichbuffer[ready_rank - 1] ^= 1;
        }
        else
        {
            // sending EOS to stop the Worker
            error = MPI_Send(nullptr, 0, MPI_BYTE, ready_rank, EOS_TAG,
                             MPI_COMM_WORLD);
            CHECK_ERROR(error);
            eos_sent++;
        }
    }
    // cleanup
    for (int i = 0; i < num_workers; i++)
    {
        if (bufs[i][0].in_use)
        {
            MPI_Wait(&bufs[i][0].req, MPI_STATUS_IGNORE);
            delete[] bufs[i][0].data;
        }
        if (bufs[i][1].in_use)
        {
            MPI_Wait(&bufs[i][1].req, MPI_STATUS_IGNORE);
            delete[] bufs[i][1].data;
        }
    }
}

// ranks form 1 to size-2
void Worker(int rank, int size)
{
    // ------------------ FastFlow part --------------------------
    // We define a farm building block Emitter -> Workers
    // Emitter and Workers use the following static variables

    static int ntasks_in_batch;
    static std::vector<char>* recv = nullptr;
    static std::vector<char>* send = nullptr;

    struct myFarm : ff::ff_farm
    {
        struct Emitter : ff::ff_node_t<long>
        {
            long* svc(long*)
            {
                for (long i = 1; i <= ntasks_in_batch; ++i)
                    ff_send_out((long*)i);
                return this->EOS;
            }
        } emitter;
        struct Worker : ff::ff_node_t<long>
        {
            long* svc(long* task)
            {
                long i = reinterpret_cast<long>(task);
                i--;
                // simulate work
                int workload;
                std::memcpy(&workload, recv->data() + i * task_size,
                            sizeof(workload));
                if (workload)
                    active_delay(workload * EXEC_TIME);

                // copying the result in the send buffer
                std::memcpy(send->data() + i * task_size,
                            recv->data() + i * task_size, task_size);
                return this->GO_ON;
            }
        };
        myFarm(int nw)
        {
            this->add_emitter(&emitter);
            std::vector<ff_node*> W;
            for (int i = 0; i < nw; ++i)
                W.push_back(new Worker);
            this->add_workers(W);
            this->cleanup_workers();
            this->set_scheduling_ondemand();
        }
        ~myFarm() { this->wait(); }
        int run(std::vector<char>* r, std::vector<char>* s, int ntasks)
        {
            ntasks_in_batch = ntasks;
            recv = r;
            send = s;
            this->skipfirstpop(true);
            return this->run_then_freeze();
        }
        int wait() { return this->wait_freezing(); }
    } localfarm(th_x_worker);
    // ------------------ FastFlow part --------------------------

    int error;
    const int emitter = 0;
    const int collector = size - 1;
    const int buff_size = task_size * batch_tasks;

    // it sends an empty message to the Emitter to notify that I'm just
    // received a task and I'm ready for the next one (so to overlap
    // computation and message transfer)
    auto send_ready = [emitter]() {
        MPI_Request req;
        int error = MPI_Isend(nullptr, 0, MPI_BYTE, emitter, ACK_TAG,
                              MPI_COMM_WORLD, &req);
        CHECK_ERROR(error);
        MPI_Request_free(&req);
    };

    // double buffering in input and output
    std::vector<char> recv_buf[2] = {std::vector<char>(buff_size),
                                     std::vector<char>(buff_size)};
    std::vector<char> send_buf[2] = {std::vector<char>(buff_size),
                                     std::vector<char>(buff_size)};

    MPI_Request recv_reqs[2];
    error = MPI_Irecv(recv_buf[0].data(), buff_size, MPI_BYTE, emitter,
                      MPI_ANY_TAG, MPI_COMM_WORLD, &recv_reqs[0]);
    CHECK_ERROR(error);
    error = MPI_Irecv(recv_buf[1].data(), buff_size, MPI_BYTE, emitter,
                      MPI_ANY_TAG, MPI_COMM_WORLD, &recv_reqs[1]);
    CHECK_ERROR(error);

    MPI_Request send_reqs[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};
    int curr = 0; // current buffer index
    int prev = 1; // index of the previously used buffer

    while (true)
    {
        int idx;
        MPI_Status st;
        error = MPI_Waitany(2, recv_reqs, &idx, &st);
        CHECK_ERROR(error);
        if (st.MPI_TAG == EOS_TAG)
        { // End-Of-Stream
            int other = 1 - idx;
            MPI_Cancel(&recv_reqs[other]);
            MPI_Wait(&recv_reqs[other], MPI_STATUS_IGNORE);
            break;
        }
        // sending ready message to the Emitter
        send_ready();

        int recv_count;
        MPI_Get_count(&st, MPI_BYTE, &recv_count);
        int tasks_in_batch = recv_count / task_size;

        // do I have to wait for send completion?
        if (send_reqs[curr] != MPI_REQUEST_NULL)
        {
            error = MPI_Wait(&send_reqs[curr], MPI_STATUS_IGNORE);
            CHECK_ERROR(error);
        }

        // TODO (optimization): use a ff_farm in which the Emitter receives the
        // batch from the MPI Emitter, the Workers compute one or more items of
        // the batch and send results to the local Collector, which sends the
        // output batch to the MPI Collector.
        localfarm.run(&recv_buf[idx], &send_buf[curr], tasks_in_batch);
        localfarm.wait();

        // send results to collector
        error = MPI_Isend(send_buf[curr].data(), tasks_in_batch * task_size,
                          MPI_BYTE, collector, RESULT_TAG, MPI_COMM_WORLD,
                          &send_reqs[curr]);
        CHECK_ERROR(error);

        // swap the send buffer indexes
        std::swap(curr, prev);

        // repost receive
        error = MPI_Irecv(recv_buf[idx].data(), buff_size, MPI_BYTE, emitter,
                          MPI_ANY_TAG, MPI_COMM_WORLD, &recv_reqs[idx]);
        CHECK_ERROR(error);
    }
    // Before exiting, make sure any in-flight send completes
    if (send_reqs[0] != MPI_REQUEST_NULL)
    {
        error = MPI_Wait(&send_reqs[0], MPI_STATUS_IGNORE);
        CHECK_ERROR(error);
    }
    if (send_reqs[1] != MPI_REQUEST_NULL)
    {
        MPI_Wait(&send_reqs[1], MPI_STATUS_IGNORE);
        CHECK_ERROR(error);
    }
}

// rank size-1
void Collector(int num_workers)
{
    int error;
    std::vector<int> tasks_per_worker(num_workers, 0);

    const int buff_size = task_size * batch_tasks;
    char* buffers[2] = {new char[buff_size], new char[buff_size]};
    assert(buffers[0] && buffers[1]);

    // posting both receives
    MPI_Request requests[2];
    error = MPI_Irecv(buffers[0], buff_size, MPI_BYTE, MPI_ANY_SOURCE,
                      RESULT_TAG, MPI_COMM_WORLD, requests);
    CHECK_ERROR(error);
    error = MPI_Irecv(buffers[1], buff_size, MPI_BYTE, MPI_ANY_SOURCE,
                      RESULT_TAG, MPI_COMM_WORLD, requests + 1);
    CHECK_ERROR(error);

    int total_received = 0;

    MPI_Status st;
    while (true)
    {
        int idx;
        error = MPI_Waitany(2, requests, &idx, &st);
        CHECK_ERROR(error);

        error = MPI_Irecv(buffers[idx], buff_size, MPI_BYTE, MPI_ANY_SOURCE,
                          RESULT_TAG, MPI_COMM_WORLD, requests + idx);
        CHECK_ERROR(error);

        int source = st.MPI_SOURCE;
        int got = 0;
        MPI_Get_count(&st, MPI_BYTE, &got);
        got = got / task_size;
        total_received += got;
        tasks_per_worker[source - 1] += got;

        if (total_received >= total_tasks)
            break;
    }
    double t_end = MPI_Wtime();

    std::printf("Collector received %d tasks\n", total_received);
    for (int i = 0; i < num_workers; ++i)
    {
        std::printf("%d: %d\n", i + 1, tasks_per_worker[i]);
    }
    std::printf("Elapsed time %f sec\n", t_end - t_start);

    delete[] buffers[0];
    delete[] buffers[1];
}

void usage(char* argv[])
{
    std::printf("usage: %s total_tasks bytes_x_task batch_size thread_x_worker "
                "[prob=%f]\n",
                argv[0], psuccess);
}

int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        usage(argv);
        return -1;
    }

    total_tasks = std::stoll(argv[1]);
    task_size = std::atoi(argv[2]);
    batch_tasks = std::atoi(argv[3]);
    if (task_size < 4)
        task_size = 4;
    th_x_worker = std::atoi(argv[4]);

    if (argc == 6)
    {
        auto p = std::atof(argv[4]);
        if (p >= 1 || p <= 0)
        {
            std::printf("Invalid success probability, it should be in (0,1)\n");
            return -1;
        }
        psuccess = p;
    }

    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE)
    {
        std::printf("MPI does not provide required threading support\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        std::abort();
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 3)
    {
        if (rank == 0)
        {
            std::printf("Too few processes, at least three are required\n");
            usage(argv);
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
        return -1;
    }

    MPI_Barrier(MPI_COMM_WORLD); // needed to measure exec time properly
    t_start = MPI_Wtime();

    if (rank == 0)
        Emitter(size);
    else
    {
        if (rank == (size - 1))
            Collector(size - 2);
        else
            Worker(rank, size);
    }

    MPI_Finalize();

    return 0;
}
