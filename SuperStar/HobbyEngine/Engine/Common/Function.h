#pragma once

#include <functional>

namespace Core::Common
{
    template <typename TResult, typename... TArgs>
    class FunctionObject
    {
    public:
        FunctionObject(std::function<TResult(TArgs...)>&& in_f) : _f(std::move(in_f)) {}

        TResult Call(TArgs&&... args) { return this->_f(std::forward<TArgs>(args)...); }

        /*
                std::array<std::string, 1 + sizeof...(TArgs)> signatureNames()
                {
                    return {typeid(TResult).name(), typeid(TArgs).name()...};
                }
        */
    private:
        std::function<TResult(TArgs...)> _f;
    };
}  // namespace Core::Common
