//===----------------------------------------------------------------------===////
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===////

#ifndef ATOMIC_SUPPORT_H
#define ATOMIC_SUPPORT_H

#include "__config"
#include "memory" // for __libcpp_relaxed_load

#if defined(__clang__) && __has_builtin(__atomic_load_n)             \
                       && __has_builtin(__atomic_store_n)            \
                       && __has_builtin(__atomic_add_fetch)          \
                       && __has_builtin(__atomic_compare_exchange_n) \
                       && defined(__ATOMIC_RELAXED)                  \
                       && defined(__ATOMIC_CONSUME)                  \
                       && defined(__ATOMIC_ACQUIRE)                  \
                       && defined(__ATOMIC_RELEASE)                  \
                       && defined(__ATOMIC_ACQ_REL)                  \
                       && defined(__ATOMIC_SEQ_CST)
#   define _LIBCPP_HAS_ATOMIC_BUILTINS
#elif !defined(__clang__) && defined(_GNUC_VER) && _GNUC_VER >= 407
#   define _LIBCPP_HAS_ATOMIC_BUILTINS
#endif

#if !defined(_LIBCPP_HAS_ATOMIC_BUILTINS) && !defined(_LIBCPP_HAS_NO_THREADS)
# if defined(_MSC_VER) && !defined(__clang__)
    _LIBCPP_WARNING("Building libc++ without __atomic builtins is unsupported")
# else
#   warning Building libc++ without __atomic builtins is unsupported
# endif
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

namespace {

#if defined(_LIBCPP_HAS_ATOMIC_BUILTINS) && !defined(_LIBCPP_HAS_NO_THREADS)

enum __libcpp_atomic_order {
    _AO_Relaxed = __ATOMIC_RELAXED,
    _AO_Consume = __ATOMIC_CONSUME,
    _AO_Acquire = __ATOMIC_ACQUIRE,
    _AO_Release = __ATOMIC_RELEASE,
    _AO_Acq_Rel = __ATOMIC_ACQ_REL,
    _AO_Seq     = __ATOMIC_SEQ_CST
};

template <class _ValueType, class _FromType>
inline _LIBCPP_INLINE_VISIBILITY
void __libcpp_atomic_store(_ValueType* __dest, _FromType __val,
                           int __order = _AO_Seq)
{
    __atomic_store_n(__dest, __val, __order);
}

template <class _ValueType, class _FromType>
inline _LIBCPP_INLINE_VISIBILITY
void __libcpp_relaxed_store(_ValueType* __dest, _FromType __val)
{
    __atomic_store_n(__dest, __val, _AO_Relaxed);
}

template <class _ValueType>
inline _LIBCPP_INLINE_VISIBILITY
_ValueType __libcpp_atomic_load(_ValueType const* __val,
                                int __order = _AO_Seq)
{
    return __atomic_load_n(__val, __order);
}

template <class _ValueType, class _AddType>
inline _LIBCPP_INLINE_VISIBILITY
_ValueType __libcpp_atomic_add(_ValueType* __val, _AddType __a,
                               int __order = _AO_Seq)
{
    return __atomic_add_fetch(__val, __a, __order);
}

template <class _ValueType>
inline _LIBCPP_INLINE_VISIBILITY
bool __libcpp_atomic_compare_exchange(_ValueType* __val,
    _ValueType* __expected, _ValueType __after,
    int __success_order = _AO_Seq,
    int __fail_order = _AO_Seq)
{
    return __atomic_compare_exchange_n(__val, __expected, __after, true,
                                       __success_order, __fail_order);
}

#elif defined(_LIBCPP_SGX_HAS_CXX_ATOMIC)

// The comment below (Not supporting this function) may not apply any more.
// It seems that SGX Windows needs __libcpp_relaxed_store.
// However, a placeholder __libcpp_relaxed_store() for when atomic builtins are not available was added in rev 248313.

// Not supporting this function. Investigate if we really need it. Otherwise, use libc++ atomic operations
inline _LIBCPP_INLINE_VISIBILITY
void __libcpp_relaxed_store(volatile unsigned long* __dest, volatile unsigned long __val)
{
    __atomic_store_explicit(__dest, __val, memory_order_relaxed);
}

inline _LIBCPP_INLINE_VISIBILITY
long __libcpp_atomic_load(long const* __val,
    int __order = memory_order_seq_cst)
{
    return __atomic_load_explicit(__val, __order);
}

inline _LIBCPP_INLINE_VISIBILITY
bool __libcpp_atomic_compare_exchange(long* __val,
    long* __expected, long __after,
    int __success_order = memory_order_seq_cst,
    int __fail_order = memory_order_seq_cst)
{
    return __atomic_compare_exchange_strong_explicit(__val, __expected, __after, __success_order, __fail_order);
}


#else // !(defined(_LIBCPP_HAS_ATOMIC_BUILTINS) && !defined(_LIBCPP_HAS_NO_THREADS)) && !defined(_LIBCPP_SGX_HAS_CXX_ATOMIC)

enum __libcpp_atomic_order {
    _AO_Relaxed,
    _AO_Consume,
    _AO_Acquire,
    _AO_Release,
    _AO_Acq_Rel,
    _AO_Seq
};

template <class _ValueType, class _FromType>
inline _LIBCPP_INLINE_VISIBILITY
void __libcpp_atomic_store(_ValueType* __dest, _FromType __val,
                           int = 0)
{
    *__dest = __val;
}

template <class _ValueType, class _FromType>
inline _LIBCPP_INLINE_VISIBILITY
void __libcpp_relaxed_store(_ValueType* __dest, _FromType __val)
{
    *__dest = __val;
}

template <class _ValueType>
inline _LIBCPP_INLINE_VISIBILITY
_ValueType __libcpp_atomic_load(_ValueType const* __val,
                                int = 0)
{
    return *__val;
}

template <class _ValueType, class _AddType>
inline _LIBCPP_INLINE_VISIBILITY
_ValueType __libcpp_atomic_add(_ValueType* __val, _AddType __a,
                               int = 0)
{
    return *__val += __a;
}

template <class _ValueType>
inline _LIBCPP_INLINE_VISIBILITY
bool __libcpp_atomic_compare_exchange(_ValueType* __val,
    _ValueType* __expected, _ValueType __after,
    int = 0, int = 0)
{
    if (*__val == *__expected) {
        *__val = __after;
        return true;
    }
    *__expected = *__val;
    return false;
}

#endif // _LIBCPP_HAS_NO_THREADS

} // end namespace

_LIBCPP_END_NAMESPACE_STD

#endif // ATOMIC_SUPPORT_H