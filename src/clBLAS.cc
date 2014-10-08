#include "clBLAS.hpp"
#include "clerr.h"

#define CL_KERNEL_SOURCE_FILENAME "clSubBLAS.cl"

#define N (256)
#define N2 (N * N)

class Job {
public:
  unsigned int row, col;
  std::vector<cl::Event> eq;
  std::vector<cl::Buffer*> bq;

  Job() {};
  ~Job() {
    std::vector<cl::Buffer*>::iterator i;
    for ( i = bq.begin(); i != bq.end(); i = bq.erase(i)) delete *i;
  };
};

clBLAS::clBLAS()
{
  cl_int error = CL_SUCCESS;
  std::string clSource;

  try {
    cl::Platform::get( &platforms);
    if ( platforms.size() == 0) {
      std::cout << "Any Platforms is Not Found." << std::endl;
    }

    cl_context_properties properties[] = {
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)(platforms[0])(),
      0
    };

    // GPU�Τߤˤ�����ϡ�CL_DEVICE_TYPE_GPU�����
    context = new cl::Context( CL_DEVICE_TYPE_DEFAULT, properties);
    devices = context->getInfo<CL_CONTEXT_DEVICES>();

    // �����ͥ륽�������ɤ߹���
    std::ifstream ifs(CL_KERNEL_SOURCE_FILENAME);
    std::string clSource(std::istreambuf_iterator<char>(ifs),
			 (std::istreambuf_iterator<char>()));
    
    // �����ͥ륽������clBLAS���󥹥��󥹤��ȹ�
    cl::Program::Sources source( 1, std::make_pair( clSource.c_str(),
						    clSource.size()));
    cl::Program program = cl::Program( *context, source);
    program.build( devices);
    kernel_sgemm = new cl::Kernel( program, "clSubSGEMM", &error);
    kernel_dgemm = new cl::Kernel( program, "clSubDGEMM", &error);

    // Queue�κ���
    queue = new cl::CommandQueue( *context, devices.at(0),
				  CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
				  &error);
  } catch( cl::Error err) {
    std::cerr << "ERROR:" << err.what() << "(" << cl_errNo2Str( err.err()) << ":" << err.err() << ")" << std::endl;
  }
}

clBLAS::~clBLAS()
{
  delete queue;
  delete kernel_sgemm;
  delete kernel_dgemm;
  delete context;
}

void clBLAS::clSGEMM( const char ta, const char tb,
	     const unsigned int l,
	     const unsigned int m,
	     const unsigned int n,
	     const float alpha, const float *a, const unsigned int LDA,
	     const float *b, const unsigned int LDB, const float beta,
	     float *c, const unsigned int LDC)
{
  cl_int error = CL_SUCCESS;
  unsigned int bsl, bsm, bsn; // �������Υ֥�å���
  unsigned int row, col, bRow, bCol, k, en; 
  unsigned int idxA, idxB, idxC;
  float ***ba, ***bb, bc[N2], tc[N2];
  std::vector<Job *> JQ;
  std::vector<Job *>::iterator JQItr;
  Job *job;
  cl::Event ev;
  
  try {
    // ��������ν����
    for ( row = 0; row < l; row++) {
      for ( col = 0; col < n; col++) {
	idxC = row * LDC + col;
	c[ idxC] = 0.0f;
      }
    }

    // �������Υ֥�å����η׻�
    bsl = ( l - 1) / N + 1;
    bsm = ( m - 1) / N + 1;
    bsn = ( n - 1) / N + 1;

    // ����a�Υ֥�å�ʬ�� ����ü����������Ƥ��ʤ���
    ba = new float**[bsl];
    for ( bRow = 0; bRow < bsl; bRow++) {
      ba[bRow] = new float*[bsm];
      for ( bCol = 0; bCol < bsm; bCol++) {
	ba[bRow][bCol] = new float[ N2];
	
	idxA = bRow * N * LDA + bCol * N;
	for ( row = 0; row < N; row++) {
	  for ( col = 0; col < N; col++) {
	    ba[ bRow][ bCol][ row * N + col] = a[ idxA + row * LDA + col];
	  }
	}
      }
    }

    // ����b�Υ֥�å�ʬ�� ����ü����������Ƥ��ʤ���
    bb = new float**[bsm];
    for ( bRow = 0; bRow < bsm; bRow++) {
      bb[bRow] = new float*[bsn];
      for ( bCol = 0; bCol < bsn; bCol++) {
	bb[bRow][bCol] = new float[ N2];

	idxB = bRow * N * LDB + bCol * N;
	for ( row = 0; row < N; row++) {
	  for ( col = 0; col < N; col++) {
	    bb[ bRow][ bCol][ row * N + col] = b[ idxB + row * LDB + col];
	  }
	}
      }
    }
    
    // �֥�å���������ν������
    for ( idxC = 0; idxC < N2; idxC++) tc[ idxC] = 0.0f;
    
    // ž�����绻�����ȹ���׻����¹Ԥ��ƹԤ���褦�����塼���󥰽������롥
    for ( bRow = 0; bRow < bsl; bRow++) {
      for ( bCol = 0; bCol < bsn; bCol++) {
	job = new Job;

	// JQ�ˤ˥��������
	// ��ʬ����(bRow,bCol)�ν񤭹��ߥХåե����Ѱ�
	cl::Buffer *clc = new cl::Buffer( *context, CL_MEM_READ_WRITE,
					  sizeof( cl_float) * N2, NULL, &error);
	queue->enqueueWriteBuffer( *clc, CL_TRUE, 0, sizeof( cl_float) * N2,
				      tc, NULL, NULL);
	job->bq.push_back( clc);

	for ( k = 0; k < bsm; k++) {
	  cl::Buffer *cla
	    = new cl::Buffer( *context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			      sizeof( cl_float) * N2, ba[ bRow][ k], &error);
	  cl::Buffer *clb
	    = new cl::Buffer( *context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			      sizeof( cl_float) * N2, bb[ k][ bCol], &error);
	  job->bq.push_back( cla);
	  job->bq.push_back( clb);
	  
	  // �����ͥ�˰�������
	  kernel_sgemm->setArg( 0, *cla);
	  kernel_sgemm->setArg( 1, *clb);
	  kernel_sgemm->setArg( 2, *clc);
	  
	  // �¹ԥ��塼�ؤ������ȼ¹�
	  queue->enqueueNDRangeKernel( *kernel_sgemm,
					  cl::NDRange( 0),
					  cl::NDRange( N2),
					  cl::NDRange( N),
					  NULL, &ev);
	  job->eq.push_back( ev);
	}

	job->row = bRow;
	job->col = bCol;
	JQ.push_back( job);

	// �׻���λ����֤򸫤Ĥ��Ʒ׻���̤β�����롥
	for( JQItr = JQ.begin(); JQItr != JQ.end();) {
	  std::vector<cl::Event>::iterator EQItr;

	  // ��������֤ο�Ľ��Ĵ�٤�
	  for ( EQItr = (*JQItr)->eq.begin(); EQItr != (*JQItr)->eq.end();) {
	    cl_uint res;
	    (*EQItr).getInfo( CL_EVENT_COMMAND_EXECUTION_STATUS, &res);
	    if ( res != CL_COMPLETE) EQItr++;
	    else EQItr = (*JQItr)->eq.erase( EQItr);
	  }

	  // ��������֤���λ���Ƥ�������ʬ������������
	  if ( (*JQItr)->eq.size() == 0) {
	    queue->enqueueReadBuffer( *((*JQItr)->bq.at(0)), CL_TRUE, 0,
				      sizeof(cl_float) * N2,
				      bc, NULL, NULL);

	    idxC = (*JQItr)->row * N * LDC + (*JQItr)->col * N;
	    for ( row = 0; row < N; row++) {
	      for ( col = 0; col < N; col++) {
		c[ idxC + row * LDC + col] = bc[ row * N + col];
	      }
	    }
	    delete *JQItr;
	    JQItr = JQ.erase( JQItr);
	  } else JQItr++;
	}
      }
      
      // ̤����η׻���̤������롥
      for( JQItr = JQ.begin(); JQItr != JQ.end(); JQItr = JQ.erase( JQItr)) {
	std::vector<cl::Event>::iterator EQItr;
	
	// ��������֤ν�λ���Ԥ�
	for ( EQItr = (*JQItr)->eq.begin(); EQItr != (*JQItr)->eq.end();) {
	  cl_uint res;
	  (*EQItr).getInfo( CL_EVENT_COMMAND_EXECUTION_STATUS, &res);
	  if ( res == CL_COMPLETE) EQItr = (*JQItr)->eq.erase( EQItr);
	}
	
	// ��������֤���ʬ������������
	queue->enqueueReadBuffer( *((*JQItr)->bq.at(0)), CL_TRUE, 0,
				  sizeof(cl_float) * N2,
				  bc, NULL, NULL);
	idxC = (*JQItr)->row * N * LDC + (*JQItr)->col * N;
	for ( row = 0; row < N; row++) {
	  for ( col = 0; col < N; col++) {
	    c[ idxC + row * LDC + col] = bc[ row * N + col];
	  }
	}
	delete *JQItr;
      }
    }

    // ���Ѥ��������ΰ������Ū����
    for ( bRow = 0; bRow < bsm; bRow++) {
      for ( bCol = 0; bCol < bsn; bCol++) {
	delete[] bb[bRow][bCol];
      }
      delete[] bb[bRow];
    }
    delete[] bb;
    
    for ( bRow = 0; bRow < bsl; bRow++) {
      for ( bCol = 0; bCol < bsm; bCol++) {
	delete [] ba[bRow][bCol];
      }
      delete[] ba[bRow];
    }
    delete[] ba;
    
  } catch( cl::Error err) {
    std::cerr << "ERROR:" << err.what() << "(" << cl_errNo2Str( err.err()) << ":" << err.err() << ")" << std::endl;
  }
}

void clBLAS::clDGEMM( const char ta, const char tb,
	     const unsigned int l,
	     const unsigned int m,
	     const unsigned int n,
	     const double alpha, const double *a, const unsigned int LDA,
	     const double *b, const unsigned int LDB, const double beta,
	     double *c, const unsigned int LDC)
{
  cl_int error = CL_SUCCESS;
  unsigned int bsl, bsm, bsn; // �������Υ֥�å���
  unsigned int row, col, bRow, bCol, k, en; 
  unsigned int idxA, idxB, idxC;
  double ***ba, ***bb, bc[N2], tc[N2];
  std::vector<Job *> JQ;
  std::vector<Job *>::iterator JQItr;
  Job *job;
  cl::Event ev;
  
  try {
    // ��������ν����
    for ( row = 0; row < l; row++) {
      for ( col = 0; col < n; col++) {
	idxC = row * LDC + col;
	c[ idxC] = 0.0f;
      }
    }

    // �������Υ֥�å����η׻�
    bsl = ( l - 1) / N + 1;
    bsm = ( m - 1) / N + 1;
    bsn = ( n - 1) / N + 1;

    // ����a�Υ֥�å�ʬ�� ����ü����������Ƥ��ʤ���
    ba = new double**[bsl];
    for ( bRow = 0; bRow < bsl; bRow++) {
      ba[bRow] = new double*[bsm];
      for ( bCol = 0; bCol < bsm; bCol++) {
	ba[bRow][bCol] = new double[ N2];
	
	idxA = bRow * N * LDA + bCol * N;
	for ( row = 0; row < N; row++) {
	  for ( col = 0; col < N; col++) {
	    ba[ bRow][ bCol][ row * N + col] = a[ idxA + row * LDA + col];
	  }
	}
      }
    }

    // ����b�Υ֥�å�ʬ�� ����ü����������Ƥ��ʤ���
    bb = new double**[bsm];
    for ( bRow = 0; bRow < bsm; bRow++) {
      bb[bRow] = new double*[bsn];
      for ( bCol = 0; bCol < bsn; bCol++) {
	bb[bRow][bCol] = new double[ N2];

	idxB = bRow * N * LDB + bCol * N;
	for ( row = 0; row < N; row++) {
	  for ( col = 0; col < N; col++) {
	    bb[ bRow][ bCol][ row * N + col] = b[ idxB + row * LDB + col];
	  }
	}
      }
    }
    
    // �֥�å���������ν������
    for ( idxC = 0; idxC < N2; idxC++) tc[ idxC] = 0.0f;
    
    // ž�����绻�����ȹ���׻����¹Ԥ��ƹԤ���褦�����塼���󥰽������롥
    for ( bRow = 0; bRow < bsl; bRow++) {
      for ( bCol = 0; bCol < bsn; bCol++) {
	job = new Job;

	// JQ�ˤ˥��������
	// ��ʬ����(bRow,bCol)�ν񤭹��ߥХåե����Ѱ�
	cl::Buffer *clc = new cl::Buffer( *context, CL_MEM_READ_WRITE,
					  sizeof( cl_double) * N2, NULL,
					  &error);
	queue->enqueueWriteBuffer( *clc, CL_TRUE, 0, sizeof( cl_double) * N2,
				      tc, NULL, NULL);
	job->bq.push_back( clc);

	for ( k = 0; k < bsm; k++) {
	  cl::Buffer *cla
	    = new cl::Buffer( *context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			      sizeof( cl_double) * N2, ba[ bRow][ k], &error);
	  cl::Buffer *clb
	    = new cl::Buffer( *context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			      sizeof( cl_double) * N2, bb[ k][ bCol], &error);
	  job->bq.push_back( cla);
	  job->bq.push_back( clb);
	  
	  // �����ͥ�˰�������
	  kernel_dgemm->setArg( 0, *cla);
	  kernel_dgemm->setArg( 1, *clb);
	  kernel_dgemm->setArg( 2, *clc);
	  
	  // �¹ԥ��塼�ؤ������ȼ¹�
	  queue->enqueueNDRangeKernel( *kernel_dgemm,
					  cl::NDRange( 0),
					  cl::NDRange( N2),
					  cl::NDRange( N),
					  NULL, &ev);
	  job->eq.push_back( ev);
	}

	job->row = bRow;
	job->col = bCol;
	JQ.push_back( job);

	// �׻���λ����֤򸫤Ĥ��Ʒ׻���̤β�����롥
	for( JQItr = JQ.begin(); JQItr != JQ.end();) {
	  std::vector<cl::Event>::iterator EQItr;

	  // ��������֤ο�Ľ��Ĵ�٤�
	  for ( EQItr = (*JQItr)->eq.begin(); EQItr != (*JQItr)->eq.end();) {
	    cl_uint res;
	    (*EQItr).getInfo( CL_EVENT_COMMAND_EXECUTION_STATUS, &res);
	    if ( res != CL_COMPLETE) EQItr++;
	    else EQItr = (*JQItr)->eq.erase( EQItr);
	  }

	  // ��������֤���λ���Ƥ�������ʬ������������
	  if ( (*JQItr)->eq.size() == 0) {
	    queue->enqueueReadBuffer( *((*JQItr)->bq.at(0)), CL_TRUE, 0,
				      sizeof(cl_double) * N2,
				      bc, NULL, NULL);

	    idxC = (*JQItr)->row * N * LDC + (*JQItr)->col * N;
	    for ( row = 0; row < N; row++) {
	      for ( col = 0; col < N; col++) {
		c[ idxC + row * LDC + col] = bc[ row * N + col];
	      }
	    }
	    delete *JQItr;
	    JQItr = JQ.erase( JQItr);
	  } else JQItr++;
	}
      }
      
      // ̤����η׻���̤������롥
      for( JQItr = JQ.begin(); JQItr != JQ.end(); JQItr = JQ.erase( JQItr)) {
	std::vector<cl::Event>::iterator EQItr;
	
	// ��������֤ν�λ���Ԥ�
	for ( EQItr = (*JQItr)->eq.begin(); EQItr != (*JQItr)->eq.end();) {
	  cl_uint res;
	  (*EQItr).getInfo( CL_EVENT_COMMAND_EXECUTION_STATUS, &res);
	  if ( res == CL_COMPLETE) EQItr = (*JQItr)->eq.erase( EQItr);
	}
	
	// ��������֤���ʬ������������
	queue->enqueueReadBuffer( *((*JQItr)->bq.at(0)), CL_TRUE, 0,
				  sizeof(cl_double) * N2,
				  bc, NULL, NULL);
	idxC = (*JQItr)->row * N * LDC + (*JQItr)->col * N;
	for ( row = 0; row < N; row++) {
	  for ( col = 0; col < N; col++) {
	    c[ idxC + row * LDC + col] = bc[ row * N + col];
	  }
	}
	delete *JQItr;
      }
    }

    // ���Ѥ��������ΰ������Ū����
    for ( bRow = 0; bRow < bsm; bRow++) {
      for ( bCol = 0; bCol < bsn; bCol++) {
	delete[] bb[bRow][bCol];
      }
      delete[] bb[bRow];
    }
    delete[] bb;
    
    for ( bRow = 0; bRow < bsl; bRow++) {
      for ( bCol = 0; bCol < bsm; bCol++) {
	delete [] ba[bRow][bCol];
      }
      delete[] ba[bRow];
    }
    delete[] ba;
    
  } catch( cl::Error err) {
    std::cerr << "ERROR:" << err.what() << "(" << cl_errNo2Str( err.err()) << ":" << err.err() << ")" << std::endl;
  }
}

