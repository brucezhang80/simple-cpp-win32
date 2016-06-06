#ifndef MEM_LEAK_INC_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define MEM_LEAK_INC_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#ifndef		NDEBUG
#	define	MEMORY_LEAK_CHECK
#	include	"_third/MemLeakDetect/MemLeakDetect.h"
static		CMemLeakDetect s_memLeakDetect;
#	include	"_third/MemLeakDetect/MemLeakDetect.cpp"
#endif

#endif
