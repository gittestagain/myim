#ifndef ___IPC_DEF_H
#define ___IPC_DEF_H

//head
#ifdef  WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <windows.h>
#else //WIN32
#include <unistd.h>
#endif//WIN32

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include<iostream>
using namespace std;

//type
namespace ipc {
	typedef unsigned char       byte;
	typedef char*               str;
	typedef const char*         cstr;

	typedef unsigned char       u8_t;
	typedef signed char         i8_t;
	typedef unsigned short      u16_t;
	typedef short               i16_t;
	typedef unsigned int        u32_t;
	typedef int                 i32_t;
	typedef unsigned long long  u64_t;
	typedef long long           i64_t;

	typedef u32_t               msec_t;
	typedef u64_t               usec_t;
	typedef byte                md5_bin_t[16];
	typedef char                md5_hex_t[33];
	typedef void              (*foo_t)(void* d);

	//val
#ifdef  WIN32
#define ___IPC_PATH_SPLIT       '\\'
#define ___IPC_MAX_PATH         MAX_PATH
#define ___IPC_INFINITE         INFINITE
#else //WIN32
#define ___IPC_PATH_SPLIT       '/'
#define ___IPC_INFINITE         0xFFFFFFFF
#define ___IPC_MAX_PATH         260
#endif//WIN32
#define ___IPC_HALF_U16         0x7fff 
#define ___IPC_HALF_U32         0x7fffffff

	//net
	typedef u32_t  ipv4_t;
	typedef u16_t  port_t;
#ifdef  WIN32
	typedef unsigned int      sock_t;
#define ___IPC_INVALID_SOCK    0xffffffff
#else //WIN32
	typedef int               sock_t;
#define ___IPC_INVALID_SOCK    -1
#endif//WIN32

	//expt
#ifdef  ___IPC_EXCEPTION_ON
#ifdef  WIN32
#define ___IPC_EXP_TRY          __try
#define ___IPC_FIN_TRY          __try
#define ___IPC_FINALLY          __finally
#define ___IPC_CATCH_ALL        __except(EXCEPTION_EXECUTE_HANDLER)
#else //WIN32
#define ___IPC_EXP_TRY          try
#define ___IPC_FIN_TRY
#define ___IPC_FINALLY
#define ___IPC_CATCH_ALL        catch(...)
#endif//WIN32
#else //___IPC_EXCEPTION_ON
#define ___IPC_EXP_TRY
#define ___IPC_FIN_TRY
#define ___IPC_FINALLY
#define ___IPC_CATCH_ALL
#endif//___IPC_EXCEPTION_ON

	//func
#ifdef  WIN32
#define snprintf            _snprintf
#define vsnprintf           _vsnprintf
#define atoll               _atoi64
#define getpid              _getpid
#endif//WIN32
#define memcast(type, mem, idx)   *((type*)((byte*)(mem) + (idx)))
#define membpos(type, meb) (u32_t)(&(((type*)(0))->meb))
#define membsiz(type, meb) sizeof(((type*)0)->meb)
#define cstrlen(z) (sizeof(z) - 1)

}//namespace wuk

#endif //___IPC_DEF_H
