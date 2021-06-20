#pragma once
#include <dots/testing/gtest/gtest.h>

namespace dots::testing::details
{
    template <typename T, typename = void>
    struct expectation_signature {};

    template <typename F>
    struct expectation_signature<::testing::internal::TypedExpectation<F>> { using type = F; };

    template <typename T>
    using expectation_signature_t = typename expectation_signature<T>::type;

    template <typename T, typename = void>
    struct is_compatible_action : std::false_type {};

    template <typename T, typename... Args>
    struct is_compatible_action<T, void(Args...)> : std::disjunction<std::is_invocable<T>, std::is_invocable<T, Args...>> {};

    template <typename T, typename Signature>
    using is_compatible_action_t = typename is_compatible_action<T, Signature>::type;

    template <typename T, typename Signature>
    constexpr bool is_compatible_action_v = is_compatible_action_t<T, Signature>::value;

    template <typename DefaultExpectationFactory, typename ArgHead, typename... ArgTail>
    auto& expect_consecutive_call_sequence_recursive(DefaultExpectationFactory defaultExpectationFactory, ArgHead&& argHead, ArgTail&&... argTail)
    {
        auto& expectation = defaultExpectationFactory(std::forward<decltype(argHead)>(argHead));
        using expectation_t = std::decay_t<decltype(expectation)>;

        if constexpr (sizeof...(argTail) > 0)
        {
            auto argTailTuple = std::make_tuple(std::forward<decltype(argTail)>(argTail)...);
            using arg_tail_head_t = std::tuple_element_t<0, decltype(argTailTuple)>;

            if constexpr (is_compatible_action_v<arg_tail_head_t, expectation_signature_t<expectation_t>>)
            {
                auto action = std::forward<arg_tail_head_t>(std::get<0>(argTailTuple));
                return expectation.WillOnce(::testing::DoAll(
                    [defaultExpectationFactory{ std::move(defaultExpectationFactory) }, argTailTuple{ std::move(argTailTuple) }](auto&&...)
                    {
                        if constexpr (sizeof...(argTail) > 1)
                        {
                            std::apply([defaultExpectationFactory{ std::move(defaultExpectationFactory) }](auto&&/* argTailHead*/, auto&&... argTailTail) -> auto&
                            {
                                return expect_consecutive_call_sequence_recursive(defaultExpectationFactory, std::forward<decltype(argTailTail)>(argTailTail)...);
                            }, argTailTuple);
                        }
                    },
                    std::move(action)
                ));
            }
            else
            {
                return expectation.WillOnce(::testing::DoAll([defaultExpectationFactory{ std::move(defaultExpectationFactory) }, argTailTuple{ std::move(argTailTuple) }](auto&&...)
                {
                    std::apply([defaultExpectationFactory{ std::move(defaultExpectationFactory) }](auto&&... argTailTail) -> auto&
                    {
                        return expect_consecutive_call_sequence_recursive(defaultExpectationFactory, std::forward<decltype(argTailTail)>(argTailTail)...);
                    }, argTailTuple);
                }));
            }
        }
        else
        {
            return expectation;
        }
    }

    template <typename DefaultExpectationFactory, typename ArgHead, typename... ArgTail>
    auto& expect_named_call_sequence_recursive(DefaultExpectationFactory& defaultExpectationFactory, ArgHead&& argHead, ArgTail&&... argTail)
    {
        auto& expectation = defaultExpectationFactory(std::forward<decltype(argHead)>(argHead));
        using expectation_t = std::decay_t<decltype(expectation)>;

        if constexpr (sizeof...(argTail) > 0)
        {
            auto argTailRefs = std::forward_as_tuple(std::forward<decltype(argTail)>(argTail)...);
            using arg_tail_head_t = std::tuple_element_t<0, decltype(argTailRefs)>;

            if constexpr (is_compatible_action_v<arg_tail_head_t, expectation_signature_t<expectation_t>>)
            {
                expectation.WillOnce(::testing::DoAll(std::forward<arg_tail_head_t>(std::get<0>(argTailRefs))));

                if constexpr (sizeof...(argTail) > 1)
                {
                    return std::apply([&defaultExpectationFactory](auto&&/* argTailHead*/, auto&&... argTailTail) -> auto&
                    {
                        return expect_named_call_sequence_recursive(defaultExpectationFactory, std::forward<decltype(argTailTail)>(argTailTail)...);
                    }, argTailRefs);
                }
                else
                {
                    return expectation;
                }
            }
            else
            {
                (void)expectation;
                return expect_named_call_sequence_recursive(defaultExpectationFactory, std::forward<decltype(argTail)>(argTail)...);
            }
        }
        else
        {
            return expectation;
        }
    }


    template <typename DefaultExpectationFactory, typename ArgHead, typename... ArgTail>
    auto& expect_consecutive_call_sequence(DefaultExpectationFactory defaultExpectationFactory, ArgHead&& argHead, ArgTail&&... argTail)
    {
        if constexpr (std::is_invocable_v<decltype(argHead)>)
        {
            auto& expectation = expect_consecutive_call_sequence_recursive(defaultExpectationFactory, std::forward<decltype(argTail)>(argTail)...);
            std::invoke(std::forward<decltype(argHead)>(argHead));

            return expectation;
        }
        else
        {
            return expect_consecutive_call_sequence_recursive(defaultExpectationFactory, std::forward<decltype(argHead)>(argHead), std::forward<decltype(argTail)>(argTail)...);
        }
    }

    template <typename DefaultExpectationFactory, typename ArgHead, typename... ArgTail>
    auto& expect_named_call_sequence(DefaultExpectationFactory defaultExpectationFactory, ArgHead&& argHead, ArgTail&&... argTail)
    {
        if constexpr (std::is_invocable_v<decltype(argHead)>)
        {
            auto& expectation = expect_named_call_sequence_recursive(defaultExpectationFactory, std::forward<decltype(argTail)>(argTail)...);
            std::invoke(std::forward<decltype(argHead)>(argHead));

            return expectation;
        }
        else
        {
            return expect_named_call_sequence_recursive(defaultExpectationFactory, std::forward<decltype(argHead)>(argHead), std::forward<decltype(argTail)>(argTail)...);
        }
    }
}

#define DOTS_EXPECT_CONSECUTIVE_CALL_SEQUENCE(defaultExpectationFactory_, ...)                                                         \
[&](auto defaultExpectationFactory, auto&&... args) -> auto&                                                                           \
{                                                                                                                                      \
    return dots::testing::details::expect_consecutive_call_sequence(defaultExpectationFactory, std::forward<decltype(args)>(args)...); \
}                                                                                                                                      \
(defaultExpectationFactory_, __VA_ARGS__)                                                                                              \

#define DOTS_EXPECT_NAMED_CALL_SEQUENCE(defaultExpectationFactory_, sequence_, ...)                                                    \
[&](const ::testing::Sequence& sequence, auto&&... args) -> auto&                                                                      \
{                                                                                                                                      \
    auto default_call_sequence_factory = [&](auto&& arg) -> auto&                                                                      \
    {                                                                                                                                  \
        return defaultExpectationFactory_(std::forward<decltype(arg)>(arg)).InSequence(sequence);                                      \
    };                                                                                                                                 \
                                                                                                                                       \
    return dots::testing::details::expect_named_call_sequence(default_call_sequence_factory, std::forward<decltype(args)>(args)...);   \
}                                                                                                                                      \
(sequence_, __VA_ARGS__)
