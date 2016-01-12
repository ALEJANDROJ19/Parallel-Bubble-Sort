#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <tareador.h>

// N and MIN must be powers of 2
long N;
long MIN_SORT_SIZE;

#define BLOCK_SIZE 128L

#define T int


void basicbombolla(long n, T data[n]){
  int swaped = 1;
  for(int i=1; i<n && swaped >= 1; i++){
    //tareador_start_task("row_bombolla_compute");
    swaped = 0;
    for(int j=0; j<n-i; j++){
      T dsig = data[j+1], dact = data[j];
      //Sorted less to more
      if(dact > dsig){
	data[j+1] = dact;
	data[j] = dsig;
	swaped++;
      }
    }
    //tareador_end_task("row_bombolla_compute");
  }
}

void basicmerge(long n, T left[n], T right[n], T result[n*2], long start, long length){
  int li = start, ri = start, l = n+start;
  
  result += start;
  while(length != 0){

    if(ri >= l){
      *result = left[li];
      li++;
    } else if(li >= l) {
      *result = right[ri];
      ri++;
    } else if(right[ri] < left[li]) {
      *result = right[ri];
      ri++;
    } else {
      *result = left[li];
      li++;
    }
    length--;
    result++;
  }
}

void merge(long n, T left[n], T right[n], T result[n*2], long start, long length) {
    basicmerge(n, left, right, result, start, length);
}

void bombolla(long n, T data[n], T tmp[n]) {
	if (n >= MIN_SORT_SIZE*4L) {
                // Recursive decomposition
		  tareador_start_task("bombolla [0]");
                bombolla(n/4L, &data[0], &tmp[0]);
		  tareador_end_task("bombolla [0]");
		  tareador_start_task("bombolla [1]");
                bombolla(n/4L, &data[n/4L], &tmp[n/4L]);
		  tareador_end_task("bombolla [1]");
		  tareador_start_task("bombolla [2]");
                bombolla(n/4L, &data[n/2L], &tmp[n/2L]);
		  tareador_end_task("bombolla [2]");
		  tareador_start_task("bombolla [3]");
                bombolla(n/4L, &data[3L*n/4L], &tmp[3L*n/4L]);
		  tareador_end_task("bombolla [3]");
		  
		  tareador_start_task("merge [0]");
                merge(n/4L, &data[0], &data[n/4L], &tmp[0], 0, n/2L);
		  tareador_end_task("merge [0]");
		  tareador_start_task("merge [1]");
                merge(n/4L, &data[n/2L], &data[3L*n/4L], &tmp[n/2L], 0, n/2L);
		  tareador_end_task("merge [1]");

		  tareador_start_task("merge");
		merge(n/2L, &tmp[0], &tmp[n/2L], &data[0], 0, n);
		  tareador_end_task("merge");

	} else {
		// Base case
		basicbombolla(n, data);
	}
}

static void initialize(long length, T data[length]) {
   long i;
   for (i = 0; i < length; i++) {
      if (i==0) {
         data[i] = rand();
	//data[i] = N;
      } else {
         data[i] = ((data[i-1]+1) * i * 104723L) % N;
	 //data[i] = N-i;
      }
   }
}

static void clear(long length, T data[length]) {
   long i;
   for (i = 0; i < length; i++) {
      data[i] = 0;
   }
}

void check_sorted(long n, T data[n]) 
{
   int unsorted=0;
   for (int i=1; i<n; i++){
      if (data[i-1] > data[i]){ 
	unsorted++;
	printf("ant: %d, act: %d, post: %d, i: %d\n", data[i-1], data[i], data[i+1],i);
   	}
   }
   if (unsorted > 0)
      printf ("\nERROR: data is NOT properly sorted. There are %d unordered positions\n\n",unsorted);
   else {
//      printf ("data IS ordered; ");
   }
}

int main(int argc, char **argv) {

        if (argc < 3) {
                fprintf(stderr, "Usage: %s <vector size in K> <sort size in K>\n", argv[0]);
                return 1;
        }

	N = atol(argv[1]) * BLOCK_SIZE;
	MIN_SORT_SIZE = atol(argv[2]) * BLOCK_SIZE;

	T *data = malloc(N*sizeof(T));
	T *tmp = malloc(N*sizeof(T));
	
	//tareador_ON();
	
	//tareador_start_task("init_main");
	
	//Initialize the data vector with random natual values
	initialize(N, data);
	//Clear the tmp vector.
	clear(N, tmp);
	//tareador_end_task("init_main");
	
	tareador_ON();
   	bombolla(N, data, tmp);
	tareador_OFF();

   	check_sorted (N, data);

	//tareador_OFF();
    	fprintf(stdout, "Bombolla program finished\n");
	return 0;
}
