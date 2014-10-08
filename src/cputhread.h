#ifdef __CPU_THREAD
#define __CPU_THREAD

typedef struct {
} CpuTHreadState;

bool CpuThreadTest( void);
bool CpuThreadNew( CpuThreadState *env);
bool CpuThreadDelete( CpuThreadState *env);
bool CpuThreadDispatch( CpuThreadState *env);

#endif // __CPU_THREAD
