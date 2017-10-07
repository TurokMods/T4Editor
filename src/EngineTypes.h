#ifndef BCTYPES_H
#define BCTYPES_H

#define GLEW_STATIC
#include <GL/glew.h>
#ifdef __APPLE__
//#include <OpenGL/gl3.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <stdio.h>

#include <assert.h>

#define BEARCLAW_VERSION_MAJOR 0
#define BEARCLAW_VERSION_MINOR 1

#define _USE_32BIT_TIME_
#define _USE_32BIT_INDICES_
//#define _USE_32BIT_REF_COUNTS_
#define _USE_64BIT_UIDS_

#define MakeCString(s) new char[s]
#define FreeCString(s) delete [] s

typedef uintptr_t           Ptr;

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef signed char         s8;
typedef signed short        s16;
typedef signed int          s32;
typedef signed long long    s64;

typedef float               f32;
typedef double              f64;

typedef char                Byte;
typedef unsigned char       uByte;

typedef const char*         Literal;
typedef char*               CString;
typedef f32                 Scalar;
typedef bool                Flag;

#ifdef _USE_32BIT_TIME_
typedef f32                 Time;
#else
typedef f64                 Time;
#endif

#ifdef __gnu_linux__
    #define INT32_MAX    __INT32_MAX__
#endif

#ifdef _USE_32BIT_INDICES_
    typedef i32         Index;
    #define INDEX_NULL  INT32_MAX
#else
    typedef i16         Index;
    #define INDEX_NULL  INT16_MAX
#endif

#ifdef _USE_32BIT_REF_COUNTS_
    typedef i32         RefCount;
#else
    typedef i16         RefCount;
#endif

#ifdef _USE_64BIT_UIDS_
    typedef u64          UID;
#else
    typedef u32          UID;
#endif

#define LOG(...)     printf(__VA_ARGS__)
#define ERROR(...)   fprintf(stderr,__VA_ARGS__)
#define WARNING(...) fprintf(stderr,__VA_ARGS__)

#endif
