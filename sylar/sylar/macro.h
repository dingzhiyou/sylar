#ifndef  __SYLAR__MACRO_H__
#define __SYLAR__MACRO_H__

#include<string.h>
#include<assert.h>
#include "util.h"
#include"log.h"

#if defined __GUNC__ || defined __llvm__
#define SYLAR_LICKLY(x)  __builtin_expect(!!(x),1)
#define SYLAR_UNLICKLY(x)  __builtin_expect(!!(x),0)

#else 
# define SYLAR_LICKLY(x) (x)
# define SYLAR_UNLICKLY(x) (x)
#endif

#define SYLAR_ASSERT(x) \
	if(!(x)){ \
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSECPTION : " #x \
		<<"\nbacktrace\n" \
		<<sylar::BacktraceToString(100,2); \
		assert(x);\
	}

#define SYLAR_ASSERT2(x,w) \
	if(!(x)){ \
		SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSECPTION : " << #x \
		<<"\n" << w \
		<<"\nbacktrace\n" \
		<<sylar::BacktraceToString(100,2); \
		assert(x);\
	}






#endif
