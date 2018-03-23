// make sure _DEBUG is defined if NDEBUG is not defined
#ifndef	NDEBUG
#define _DEBUG 1
#endif

// defines for RVL target
#define RVL
#define RVL_SDK 1
#define TRK_INTEGRATION
#define RVL_OS 1
#define EPPC
#define GEKKO
#define	HOLLYWOOD_REV	1
#define	BROADWAY_REV	1
#define	IOP_REV	1
#define __STDC_WANT_LIB_EXT1__ 1
#define _MSL_C_HAS_CPP_EXTENSIONS 1
#define _MSL_C99 1

#define FT2_BUILD_LIBRARY 1
//#define _MSL_NO_THROW_SPECS 1
/*
 * Enabling C++ extensions if compiling as C++
 * sw on 09102002
 */

#ifdef __cplusplus
#pragma cpp_extensions on
#endif

#include <ansi_prefix.PPCEABI.Gamecube.h>

