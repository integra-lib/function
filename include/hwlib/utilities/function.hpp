#pragma once
#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>

namespace hwlib::utilities
{

template<typename UnusedType>
class Function;

/// @brief Lightweight analogue of std::function
///
/// Stores any callable matching the signature. The callable is placed on the
/// heap, so this component is unusable where dynamic allocation is forbidden.
/// Move-only: copying is deleted because the held callable is owned uniquely.
template<typename ReturnType, typename... ArgumentTypes>
class Function<ReturnType(ArgumentTypes...)>
{
public:
    Function() noexcept = default;

    Function(std::nullptr_t) noexcept {} // NOLINT (hicpp-explicit-conversions)

    template<typename FunctionType>
    Function(FunctionType f) // NOLINT (hicpp-explicit-conversions)
        : m_invoker{std::make_unique<FreeFunctionHolder<FunctionType>>(std::move(f))}
    {}

    ~Function() = default;

    Function(const Function&)            = delete;
    Function& operator=(const Function&) = delete;

    Function(Function&&) noexcept            = default;
    Function& operator=(Function&&) noexcept = default;

    template<typename FunctionType>
    Function& operator=(FunctionType f)
    {
        m_invoker = std::make_unique<FreeFunctionHolder<FunctionType>>(std::move(f));
        return *this;
    }

    ReturnType operator()(ArgumentTypes... args)
    {
        assert(m_invoker);
        return m_invoker->Invoke(args...);
    }

    [[nodiscard]] explicit operator bool() const
    {
        return m_invoker != nullptr;
    }

private:
    class IFunctionHolderBase
    {
    public:
        IFunctionHolderBase()                                          = default;
        IFunctionHolderBase(const IFunctionHolderBase&)                = default;
        IFunctionHolderBase(IFunctionHolderBase&&) noexcept            = default;
        IFunctionHolderBase& operator=(const IFunctionHolderBase&)     = default;
        IFunctionHolderBase& operator=(IFunctionHolderBase&&) noexcept = default;

        virtual ~IFunctionHolderBase() = default;

        virtual ReturnType Invoke(ArgumentTypes... args) = 0;
    };

    template<typename FunctionType>
    class FreeFunctionHolder : public IFunctionHolderBase
    {
    public:
        explicit FreeFunctionHolder(FunctionType func)
            : m_function{std::move(func)}
        {}

        ReturnType Invoke(ArgumentTypes... args) override
        {
            return m_function(args...);
        }

    private:
        FunctionType m_function;
    };

    std::unique_ptr<IFunctionHolderBase> m_invoker{};
};

} // namespace hwlib::utilities
