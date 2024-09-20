#pragma once
#include <type_traits>
#include <boost/type_traits/detected.hpp>
#include <boost/type_traits/detected_or.hpp>
//apparently this shit can be done with concepts
template <template <class...> class Op, class... Types>
struct dispatcher;

template <template <class...> class Op, class T>
struct dispatcher<Op, T> : boost::detected_t<Op, T> {};

template <template <class...> class Op, class T, class... Types>
struct dispatcher<Op, T, Types...>
    : boost::detected_or_t<
    typename dispatcher<Op, Types...>::type, Op, T> {};


// Helper to convert a signature to a function pointer
template <class Signature> struct function_ptr;

template <class R, class... Args> struct function_ptr<R(Args...)> {
    using type = R(*)(Args...);
};

// Macro to simplify creation of the dispatcher
// NOTE: This macro isn't smart enough to handle creating an overloaded
//       dispatcher because both dispatchers will try to use the same
//       integral_constant type alias name. If you want to overload, do it
//       manually or make a smarter macro that can somehow put the signature in
//       the integral_constant type alias name.
#define virtual_static_method(name, signature, ...)                            \
    template <class VSM_T>                                                     \
    using vsm_##name##_type = std::integral_constant<                          \
        function_ptr<signature>::type, &VSM_T::name>;                          \
                                                                               \
    template <class... VSM_Args>                                               \
    static auto name(VSM_Args&&... args)                                       \
    {                                                                          \
        return dispatcher<vsm_##name##_type, __VA_ARGS__>::value(              \
            std::forward<VSM_Args>(args)...);                                  \
    }