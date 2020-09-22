#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>

double mysecond();

int main(int argc, char* argv[]){
	#define SIZE 9
	#define REPEAT 10
	int thread_num[SIZE] = {1,2,4,8,16,20,24,28,32};
	double measure_time[SIZE][REPEAT];
	double measure_time_mean[SIZE];
	double measure_time_dev[SIZE];
	
	#define N 1000000
	
	typedef struct {
		double val;
		int loc;
		char pad[128];
	} tvals;
	
	for(int k=0; k<SIZE; k++){
		measure_time_mean[k] = 0;
		measure_time_dev[k] = 0;
	
		for(int m=0; m<REPEAT; m++){
			omp_set_num_threads(thread_num[k]);
			
			// Initialize x array
			double x[N];
			srand(time(0));
			for(int i=0; i < N;i++){
				x[i] = ((double)(rand()) / RAND_MAX)*((double)(rand()) / RAND_MAX)*((double)(rand()) / RAND_MAX)*1000;
			}
		
			// Compute maxloc and maxval                                                          
			tvals maxinfo[thread_num[k]], minfo;
    
			double t1 = mysecond();
			#pragma omp parallel shared(maxinfo)
			{
				int id = omp_get_thread_num();
				maxinfo[id].val = -1.0e30;
				#pragma omp parallel for
				for (int i=0; i < N; i++){
						if (x[i] > maxinfo[id].val) {
							maxinfo[id].loc = x[i];
							maxinfo[id].val = i;
						}
				}
			}
			
			minfo.loc = maxinfo[0].loc;
			minfo.val = maxinfo[0].val;
			for(int i=1; i<thread_num[k]; i++){
				if(maxinfo[i].val>minfo.val){
					minfo.val = maxinfo[i].val;
					minfo.loc = maxinfo[i].loc;
				}
			}
			double t2 = mysecond();
		
		
			// Print out
			printf("maxloc=%d, maxval=%lf\n", minfo.loc, minfo.val);
			
			measure_time[k][m] = t2-t1;
  	}
  	
  	// Data processing
  	for(int m=0; m<REPEAT; m++){
  		measure_time_mean[k] += measure_time[k][m];
  	}
  	measure_time_mean[k] /= REPEAT;
  	
  	for(int m=0; m<REPEAT; m++){
  		measure_time_dev[k] += pow(measure_time[k][m]-measure_time_mean[k], 2);
  	}
  	measure_time_dev[k] = sqrt(measure_time_dev[k])/(REPEAT-1);
  	printf("thread=%d, exec_time_mean=%11.8f s, standard_derivation=%11.8f s\n", thread_num[k], measure_time_mean[k], measure_time_dev[k]);
  	
  }
  
  
	
	return 0;
}

// function with timer                                                             
double mysecond(){
  struct timeval tp;
  struct timezone tzp;
  int i;

  i = gettimeofday(&tp,&tzp);
  return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}
