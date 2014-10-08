#ifndef __DEBUG_H
#define __DEBUG_H
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#ifdef DEBUG
#define debug(str) fprintf( stderr, "%s %d %s\n", __FILE__, __LINE__, str)
#define return if ( ( fprintf(stderr,"return@%s#%d\n", __FILE__, __LINE__), false)) {} else return
#else
#define debug(str)
#define return return
#endif // DEBUG

#define TEST(expr) (((expr)) ? ( fprintf( stderr, "TEST: %s: %d %s: `%s'.\n", __FILE__, __LINE__, __func__, __STRING(expr))), true : false)
#endif // __DEBUG_H
