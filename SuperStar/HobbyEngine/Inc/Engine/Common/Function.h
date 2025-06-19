#pragma once

#include <functional>

namespace Core::Common
{
    template <typename TResult, typename... TArgs>
    class FunctionObject
    {
    public:
        FunctionObject(std::function<TResult(TArgs...)>&& in_f) : _f(std::move(in_f)) {}

        /// <summary>
        /// 右辺値参照用
        /// </summary>
        TResult CallByMove(TArgs&&... args) { return this->_f(std::forward<TArgs>(args)...); }
        /// <summary>
        /// 参照用
        /// </summary>
        TResult CallByRef(TArgs&... args) { return this->_f(args...); }

    private:
        std::function<TResult(TArgs...)> _f;
    };
}  // namespace Core::Common
