#ifndef STAGE_HPP
#define STAGE_HPP

#include <functional>

namespace spm
{

template <typename Out, typename... Args>
class stage
{
public:
    stage(std::function<Out(Args...)>&& func) : m_func(std::move(func)) {}

    Out operator()(Args&&... args)
    {
        return m_func(std::forward<Args>(args)...);
    }

    ~stage() {}

private:
    std::function<Out(Args...)> m_func;
};

} // namespace spm

#endif
