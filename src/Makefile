CC = gcc
FC = gfortran
RM = rm
INCLUDE = -I/opt/AMDAPP/include/ -I/opt/acml5.3.1/gfortran64/include/ -I/opt/clBLAS/include
# LIBS = -L/opt/AMDAPP/lib/x86_64/ -lOpenCL
LIBS = -L/opt/acml5.3.1/gfortran64/lib/ -L/opt/AMDAPP/lib/x86_64 -L/opt/clBLAS/lib -Llib/x86_64-linux-gnu/ -lOpenCL -lclBLAS -lacml -lm 
CFLAGS = -g -O0 -std=c99
TARGET = test
OBJS = test.o clerr.o jobQueue.o ringQueue.o clState.o subMatrix.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

clean:
	$(RM) $(TARGET) $(OBJS)
