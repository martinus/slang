#ifndef SLANG_CORE_COMMON_H
#define SLANG_CORE_COMMON_H

#include "../../slang.h"

#include <assert.h>

#include <stdint.h>

#include "slang-signal.h"

#define VARIADIC_TEMPLATE

namespace Slang
{
    
	typedef int32_t Int32;
	typedef uint32_t UInt32;

	typedef int64_t Int64;
	typedef uint64_t UInt64;

    // Define 
    typedef SlangUInt UInt;
    typedef SlangInt Int;

    static const UInt kMaxUInt = ~UInt(0);
    static const Int kMaxInt = Int(kMaxUInt >> 1);

//	typedef unsigned short Word;

	typedef intptr_t PtrInt;

    // TODO(JS): It looks like Index is actually 64 bit on 64 bit targets(!)
    // Previous discussions landed on Index being int32_t.

    // Type used for indexing, in arrays/views etc. Signed.
    typedef Int Index;
    typedef UInt UIndex;
    typedef Int Count;
    typedef UInt UCount;

    static const Index kMaxIndex = kMaxInt;

    typedef uint8_t Byte;

    // TODO(JS):
    // Perhaps these should be named Utf8, Utf16 and UnicodePoint/Rune/etc? For now, just keep it simple
    //
    typedef char Char8;
    // 16 bit character. Note much like in utf8, a character may or may not represent a code point (it can be part of a code point).  
    typedef uint16_t Char16;

    // Can always hold a unicode code point.
    typedef uint32_t Char32;

	template <typename T>
	inline T&& _Move(T & obj)
	{
		return static_cast<T&&>(obj);
	}

	template <typename T>
	inline void Swap(T & v0, T & v1)
	{
		T tmp = _Move(v0);
		v0 = _Move(v1);
		v1 = _Move(tmp);
	}

    // Make these interfaces have more convenient names
    typedef ISlangCastable ICastable;
    typedef ISlangClonable IClonable;

    // Convenience function for using clonable
    template <typename T>
    SLANG_FORCE_INLINE T* clone(IClonable* clonable) { return (T*)clonable->clone(T::getTypeGuid()); }

    template <typename T>
    inline bool isBitSet(T value, T bitToTest)
    {
        static_assert(sizeof(T) <= sizeof(uint32_t), "Only support up to 32 bit enums");
        return (T)((uint32_t)value & (uint32_t)bitToTest) == bitToTest;
    }
}

// TODO: Shouldn't these be SLANG_ prefixed?
#ifdef _MSC_VER
#define UNREACHABLE_RETURN(x)
#else
#define UNREACHABLE_RETURN(x) return x;
#endif

//
// Use `SLANG_ASSUME(myBoolExpression);` to inform the compiler that the condition is true.
// Do not rely on side effects of the condition being performed.
//
#if defined(__cpp_assume)
#    define SLANG_ASSUME(X) [[assume(X)]]
#elif SLANG_GCC
#    define SLANG_ASSUME(X) do{if(!(X)) __builtin_unreachable();} while(0)
#elif SLANG_CLANG
#    define SLANG_ASSUME(X) __builtin_assume(X)
#elif SLANG_VC
#    define SLANG_ASSUME(X) __assume(X)
#else
     [[noreturn]] inline void invokeUndefinedBehaviour() {}
#    define SLANG_ASSUME(X) do{if(!(X)) invokeUndefinedBehaviour();} while(0)
#endif

//
// Assertions abort in debug builds, but inform the compiler of true
// assumptions in release builds
//
#ifdef _DEBUG
#define SLANG_ASSERT(VALUE) do{if(!(VALUE)) SLANG_ASSERT_FAILURE(#VALUE);} while(0)
#else
#define SLANG_ASSERT(VALUE) SLANG_ASSUME(VALUE)
#endif

#define SLANG_RELEASE_ASSERT(VALUE) if(VALUE) {} else SLANG_ASSERT_FAILURE(#VALUE)

template<typename T> void slang_use_obj(T&) {}

#define SLANG_UNREFERENCED_PARAMETER(P) slang_use_obj(P)
#define SLANG_UNREFERENCED_VARIABLE(P) slang_use_obj(P)
#endif

#if defined(SLANG_RT_DYNAMIC)
#if defined(_MSC_VER)
#    ifdef SLANG_RT_DYNAMIC_EXPORT
#        define SLANG_RT_API SLANG_DLL_EXPORT
#    else
#        define SLANG_RT_API __declspec(dllimport)
#    endif
#else
// TODO: need to consider compiler capabilities
//#     ifdef SLANG_RT_DYNAMIC_EXPORT
#    define SLANG_RT_API SLANG_DLL_EXPORT
//#     endif
#endif
#endif

#if defined(_MSC_VER)
#   define SLANG_ATTR_PRINTF(string_index, varargs_index)
#else
#   define SLANG_ATTR_PRINTF(string_index, varargs_index) __attribute__((format(printf, string_index, varargs_index)))
#endif

#ifndef SLANG_RT_API
#define SLANG_RT_API
#endif
