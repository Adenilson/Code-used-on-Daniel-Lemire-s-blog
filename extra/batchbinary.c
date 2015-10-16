/**
gcc -O3 batchbinary.c -o batchbinary && ./batchbinary
*/
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>

size_t branchy_search(int* source, size_t n, int target) {
    size_t lo = 0;
    size_t hi = n;
    while (lo < hi) {
        size_t m = (lo + hi) / 2;
        if (target < source[m]) {
            hi = m;
        } else if (target > source[m]) {
            lo = m+1;
        } else {
            return m;
        }
    }
    return hi;
}


size_t branchfree_search(int* source, size_t n, int target) {
    int * base = source;
    while(n>1) {
        size_t half = n >> 1;
        base = (base[half] < target) ? &base[half] : base;
        n -= half;
    }
    return ((base < source+n)?(*base < target):0) + base - source;
}

void branchfree_search2(int* source, size_t n, int target1, int target2, size_t * index1, size_t * index2) {
    int * base1 = source;
    int * base2 = source;
    while(n>1) {
        size_t half = n >> 1;
        base1 = (base1[half] < target1) ? &base1[half] : base1;
        base2 = (base2[half] < target2) ? &base2[half] : base2;
        n -= half;
    }
    *index1 = ((base1 < source+n)?(*base1 < target1):0) + base1 - source;
    *index2 = ((base2 < source+n)?(*base2 < target2):0) + base2 - source;
}


size_t branchfree_search_prefetch(int* source, size_t n, int target) {
    int * base = source;
    while(n>1) {
        size_t half = n >> 1;
        __builtin_prefetch(base+(half>>1),0,0);
        __builtin_prefetch(base+half+(half>>1),0,0);
        base = (base[half] < target) ? &base[half] : base;
        n -= half;
    }
    return ((base < source+n)?(*base < target):0) + base - source;
}


int compare (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

int demo(size_t N, size_t Nq) {
  int * queries = (int*)malloc(N*sizeof(int));
  int * source = (int*)malloc(N*sizeof(int));
  size_t bogus = 0;
  size_t bogus1 = 0;
  size_t bogus2 = 0;
  printf("===============\n");

  printf("array size (N)=%zu,  number of queries (Nq)=%zu...\n",N,Nq);
  printf("preparing data...\n");
  for(size_t i = 0; i < N; ++i) {
      source[i] = rand();
  }
  int maxval = source[N-1];
  qsort (source, N, sizeof(int), compare);
  for(size_t i = 0; i < Nq; ++i) {
      queries[i] = rand()%(maxval+1);
  }
  printf("beginning tests...\n");
  printf("\n");

  for(size_t ti = 0; ti < 3; ++ti) {
      struct timeval t1, t2, t3, t4, t5;

      gettimeofday(&t1, 0);
      for(size_t k = 0; k < Nq; ++k)
          bogus += branchfree_search(source,N,queries[k]);
      gettimeofday(&t2, 0);
      for(size_t k = 0; k < Nq; ++k)
          bogus += branchy_search(source,N,queries[k]);
      gettimeofday(&t3, 0);
      for(size_t k = 0; k < Nq; ++k)
          bogus += branchfree_search_prefetch(source,N,queries[k]);
      gettimeofday(&t4, 0);
      for(size_t k = 0; k+1 < Nq; k+=2)
          branchfree_search2(source,N,queries[2*k],queries[2*k+1],&bogus1,&bogus2);
      gettimeofday(&t5, 0);

      printf("branchless time=%llu  \n",((t2.tv_sec - t1.tv_sec) * 1000ULL * 1000ULL) + t2.tv_usec-t1.tv_usec);
      printf("branchy time=%llu  \n",((t3.tv_sec - t2.tv_sec) * 1000ULL * 1000ULL) + t3.tv_usec-t2.tv_usec);
      printf("branchless time with prefetch=%llu \n",((t4.tv_sec - t3.tv_sec) * 1000ULL * 1000ULL) + t4.tv_usec-t2.tv_usec);
      printf("branchless interleaved time=%llu  \n",((t5.tv_sec - t4.tv_sec) * 1000ULL * 1000ULL) + t5.tv_usec-t4.tv_usec);

      printf("\n");
  }
  free(source);
  free(queries);
  return (int) bogus+bogus1+bogus2;

}

int main() {
  demo(1024,1024 * 1024);
  demo(1024 * 1024,1024 * 1024);
  demo(32 * 1024 * 1024,1024 * 1024);

}
