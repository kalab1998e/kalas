#ifndef KALAS_H__
#define KALAS_H__

#define CHKCLERR(expr) (((expr) == CL_SUCCESS) ? true :     \
  ( fprintf( stderr, "%s: %d %s: `%s' failed.",             \
             __FILE__, __LINE__, __func__, __STRING(expr)), \
    goto FUNCEXIT)

typedef enum { KALAS_FLOAT = 4, KALAS_DOUBLE = 8} typeKind;

#endif // KALAS_H__
