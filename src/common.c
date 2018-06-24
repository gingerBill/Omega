#if defined(_WIN32) || defined(_WIN64)
	#if _MSC_VER < 1300
	typedef unsigned char     uint8_t;
	typedef   signed char      int8_t;
	typedef unsigned short   uint16_t;
	typedef   signed short    int16_t;
	typedef unsigned int     uint32_t;
	typedef   signed int      int32_t;
	#else
	typedef unsigned __int8   uint8_t;
	typedef   signed __int8    int8_t;
	typedef unsigned __int16 uint16_t;
	typedef   signed __int16  int16_t;
	typedef unsigned __int32 uint32_t;
	typedef   signed __int32  int32_t;
	#endif
	typedef unsigned __int64 uint64_t;
	typedef   signed __int64  int64_t;

#else
	#include <stdint.h>
#endif
typedef uint8_t   u8;
typedef  int8_t   i8;
typedef uint16_t u16;
typedef  int16_t i16;
typedef uint32_t u32;
typedef  int32_t i32;
typedef uint64_t u64;
typedef  int64_t i64;



#if defined(_WIN64)
	typedef signed   __int64  intptr_t;
	typedef unsigned __int64 uintptr_t;
	typedef unsigned __int64    size_t;
	typedef __int64          ptrdiff_t;
#elif defined(_WIN32)
	// NOTE(bill); To mark types changing their size, e.g. intptr
	#if !defined(_W64)
		#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
			#define _W64 __w64
		#else
			#define _W64
		#endif
	#endif

	typedef _W64   signed int  intptr_t;
	typedef _W64 unsigned int uintptr_t;
	typedef _W64 unsigned int    size_t;
	typedef _W64          int ptrdiff_t;
#endif

typedef uintptr_t uintptr;
typedef  intptr_t  intptr;

typedef size_t    usize;
typedef ptrdiff_t isize;

// NOTE(bill): Get true and false
#if !defined(__cplusplus)
	#if (defined(_MSC_VER) && _MSC_VER < 1800) || (!defined(_MSC_VER) && !defined(__STDC_VERSION__))
		#ifndef true
		#define true  (0 == 0)
		#endif
		#ifndef false
		#define false (0 != 0)
		#endif
		typedef uint8_t bool;
	#else
		#include <stdbool.h>
	#endif
#endif

// NOTE(bill): Because a signed sizeof is more useful
#ifndef size_of
#define size_of(x) (isize)(sizeof(x))
#endif

#ifndef count_of
#define count_of(x) ((size_of(x)/size_of(0[x])) / ((isize)(!(size_of(x) % size_of(0[x])))))
#endif

#ifndef offset_of
#define offset_of(Type, element) ((isize)&(((Type *)0)->element))
#endif

#if defined(__cplusplus)
#ifndef align_of
	#if __cplusplus >= 201103L
		#define align_of(Type) (isize)alignof(Type)
	#else
extern "C++" {
		// NOTE(bill): Fucking Templates!
		template <typename T> struct Alignment_Trick { char c; T member; };
		#define align_of(Type) offset_of(Alignment_Trick<Type>, member)
}
	#endif
#endif
#else
	#ifndef align_of
	#define align_of(Type) offset_of(struct { char c; Type member; }, member)
	#endif
#endif


#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900

#define snprintf  c99_snprintf
#define vsnprintf c99_vsnprintf

__inline int c99_vsnprintf(char *out_buf, isize size, char const *format, va_list ap) {
    int count = -1;

    if (size != 0) {
        count = _vsnprintf_s(out_buf, size, _TRUNCATE, format, ap);
    }
    if (count == -1) {
        count = _vscprintf(format, ap);
    }

    return count;
}

__inline int c99_snprintf(char *out_buf, isize size, char const *format, ...) {
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(out_buf, size, format, ap);
    va_end(ap);

    return count;
}

#endif


#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

