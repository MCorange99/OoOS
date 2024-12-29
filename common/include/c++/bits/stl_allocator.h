#ifndef __ALLOCATOR_BASE
#define __ALLOCATOR_BASE
#ifndef _GCC_STDINT_H
#include "stdint.h"
#endif
#include "stddef.h"
#include "bits/ptr_traits.hpp"
#include "type_traits"
#define __visible __attribute__((__externally_visible__))
#define __force_inline [[gnu::always_inline]]
namespace std
{
    using size_t = decltype(sizeof(int));
    enum class align_val_t : decltype(alignof(int)) {};
    struct __allocator_traits_base
    {
        template<typename T, typename U, typename = void> struct __rebind : __replace_first_arg<T, U> {};
        template<typename T, typename U> struct __rebind<T, U, __void_t<typename T::template rebind<U>::other>> { using type = typename T::template rebind<U>::other; };
    protected:
        template<typename T> using __pointer = typename T::pointer;
        template<typename T> using __c_pointer = typename T::const_pointer;
        template<typename T> using __v_pointer = typename T::void_pointer;
        template<typename T> using __cv_pointer = typename T::const_void_pointer;
        template<typename T> using __pocca = typename T::propagate_on_container_copy_assignment;
        template<typename T> using __pocma = typename T::propagate_on_container_move_assignment;
        template<typename T> using __pocs = typename T::propagate_on_container_swap;
        template<typename T> using __equal = typename T::is_always_equal;
    };
    template<typename AT, typename U> using __alloc_rebind = typename __allocator_traits_base::template __rebind<AT, U>::type;
    template<typename T>
    struct __base_allocator
    {
        constexpr static std::size_t __align_val { alignof(T) };
        constexpr static std::size_t __size_val { sizeof(T) };
        constexpr __base_allocator() noexcept = default;
        constexpr __base_allocator(__base_allocator<T> const&) noexcept = default;
        constexpr ~__base_allocator() = default;
        template<class U> constexpr __base_allocator(__base_allocator<U> const&) noexcept {}
        [[nodiscard]] __force_inline constexpr T* __allocate(std::size_t n) const { return static_cast<T*>(::operator new(n * __size_val, static_cast<std::align_val_t>(__align_val))); }
        __force_inline constexpr void __deallocate(T* ptr, std::size_t n) const { ::operator delete(ptr, n * __size_val, static_cast<std::align_val_t>(__align_val)); }
    };
    namespace __detail
    {
        [[nodiscard]] void* __aligned_reallocate(void* ptr, size_t n, size_t align);
        template<typename T> concept __non_array = !std::is_array_v<T>;
        template<typename ... Args> concept __zero_size = sizeof...(Args) == 0;
        template<typename T, typename ... Args> concept __dynamic_constructible = std::constructible_from<T, Args...> && (__non_array<T> || __zero_size<Args...>);
    }
#pragma region non-standard memory functions
    template<typename T> [[nodiscard]] __force_inline constexpr T* resize(T* array, size_t ncount) { return reinterpret_cast<T*>(__detail::__aligned_reallocate(array, ncount * sizeof(T), alignof(T))); }
#pragma endregion
}
#endif