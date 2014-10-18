#ifndef KADBG_H__
#define KADBG_H__ 1
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <asm/unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void kaDump( void*, int);

#ifdef __cplusplus
}
#endif

#define dbgMsg(str) fprintf( stderr, "tid:%d %s %s() %d %s\n", syscall(__NR_gettid), __FILE__, __func__, __LINE__, str)
#define IS_FAILED(expr) ((expr) ? false : ( fprintf( stderr, "%s: %d %s: `%s' failed.\n", __FILE__, __LINE__, __func__, __STRING(expr)), true))

#endif // KADBG_H__
