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



