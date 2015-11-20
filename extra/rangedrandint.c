/***
* This tests the generation of ranged 16-bit random integers.
* gcc -O2 -o rangedrandint rangedrandint.c
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


int fogapproach(uint16_t range) {
    printf("fog range %d \n",range);

    int counter[1<<16];
    for(int k = 0; k < 1<<16; ++k) {
        counter[k] = 0;
    }
    int threshold = (int) ((uint32_t)((1<<16)/range*range) -1);
    if(threshold+1 < (1<<16)-range+ (range & (~(range-1)))){
      printf("threshold = %d ",threshold);
      printf("crazy = %d ",(1<<16)-range+ (range & (~(range-1))));
      return -1;
    }

    for(int k = 0; k < 1<<16; ++k) {
        uint32_t r = k;
        uint16_t c =(r* range) >> 16;
        int leftover = (r* range) & 0xFFFF;
        if(leftover <= threshold) {
            counter[c]++;
        } else {

             if(leftover <= (1<<16)-range)
              return -1;

        }
    }

    int minc = UINT16_MAX+1;
    int maxc = 0;
    int fair = counter[0];
    for(int k = 0; k < range; ++k) {
        if(counter[k]<minc) minc = counter[k];
        if(counter[k]>maxc) maxc = counter[k];
    }
    printf("%d ** %d +++  %d -- %d +++ %d \n",-minc+maxc,minc,UINT16_MAX/range,(UINT16_MAX+range-1)/range,maxc);
    if(minc == maxc) printf("******fair\n");
    else {printf("======unfair \n"); return -1;}
    return 0;
}

int reversefogapproach(uint16_t range) {
    printf("reverse fog range %d \n",range);

    int counter[1<<16];
    for(int k = 0; k < 1<<16; ++k) {
        counter[k] = 0;
    }
    int threshold = (1<<16) % range;

    if((range &(range-1))!=0) {
        int fthreshold = ((1<<16)-1)%range + 1;
        if(threshold != fthreshold) {
          printf("bug %d %d %d ",range,threshold,fthreshold);
          abort();
        }
    }
    for(int k = 0; k < 1<<16; ++k) {
        uint32_t r = k;
        uint16_t c =(r* range) >> 16;
        int leftover = (r* range) & 0xFFFF;
        if(leftover >= threshold) {
            counter[c]++;
        } else {
        }
    }

    int minc = UINT16_MAX+1;
    int maxc = 0;
    int fair = counter[0];
    for(int k = 0; k < range; ++k) {
        if(counter[k]<minc) minc = counter[k];
        if(counter[k]>maxc) maxc = counter[k];
    }
    printf("%d ** %d +++  %d -- %d +++ %d \n",-minc+maxc,minc,UINT16_MAX/range,(UINT16_MAX+range-1)/range,maxc);
    if(minc == maxc) printf("******fair\n");
    else {printf("======unfair \n"); return -1;}
    return 0;
}

int fasterreversefogapproach(uint16_t range) {
    printf("faster reverse fog range %d \n",range);

    int counter[1<<16];
    for(int k = 0; k < 1<<16; ++k) {
        counter[k] = 0;
    }
    int threshold = (1<<16) % range;
    int cheap = range- (range & (~(range-1)));
    if(cheap < threshold ) {
      printf("bug");
      abort();
    }
    for(int k = 0; k < 1<<16; ++k) {
        uint32_t r = k;
        uint16_t c =(r* range) >> 16;
        int leftover = (r* range) & 0xFFFF;
        if(leftover >= threshold) {
            counter[c]++;
        } else {
        }
    }

    int minc = UINT16_MAX+1;
    int maxc = 0;
    int fair = counter[0];
    for(int k = 0; k < range; ++k) {
        if(counter[k]<minc) minc = counter[k];
        if(counter[k]>maxc) maxc = counter[k];
    }
    printf("%d ** %d +++  %d -- %d +++ %d \n",-minc+maxc,minc,UINT16_MAX/range,(UINT16_MAX+range-1)/range,maxc);
    if(minc == maxc) printf("******fair\n");
    else {printf("======unfair \n"); return -1;}
    return 0;
}


int fasterthanfogapproach(uint16_t range) {
    printf("fasterthanfog range %d \n",range);

    int counter[1<<16];
    for(int k = 0; k < 1<<16; ++k) {
        counter[k] = 0;
    }
    int fastthreshold = (1<<16)-range+ (range & (~(range-1)));
    int threshold = (int) ((uint32_t)((1<<16)/range*range) -1);
    if(threshold+1 < fastthreshold){
      printf("threshold = %d ",threshold);
      printf("crazy = %d ",fastthreshold);
      return -1;
    }

    for(int k = 0; k < 1<<16; ++k) {
        uint32_t r = k;
        uint16_t c =(r* range) >> 16;
        int leftover = (r* range) & 0xFFFF;
        if(leftover < fastthreshold) {
            counter[c]++;
        } else {
          if(leftover <= threshold)
            counter[c]++;
        }
    }

    int minc = UINT16_MAX+1;
    int maxc = 0;
    int fair = counter[0];
    for(int k = 0; k < range; ++k) {
        if(counter[k]<minc) minc = counter[k];
        if(counter[k]>maxc) maxc = counter[k];
    }
    printf("%d ** %d +++  %d -- %d +++ %d \n",-minc+maxc,minc,UINT16_MAX/range,(UINT16_MAX+range-1)/range,maxc);
    if(minc == maxc) printf("******fair\n");
    else {printf("======unfair \n"); return -1;}
    return 0;
}



int classicapproach(uint16_t range) {
    printf("classic range %d \n",range);

    int counter[1<<16];
    for(int k = 0; k < 1<<16; ++k) {
        counter[k] = 0;
    }
    int threshold = (int) ((uint32_t)((1<<8)/range*range) -1);
    int failure = 0;
    for(int k = 0; k < 1<<16; ++k) {
        uint32_t r = k;
        uint16_t c = r%range;
        if(r+ range -1 - c < (1<<16)) {
            counter[c]++;
        } else {
            ++failure;

        }
    }
    printf("failure rate  %f \n",failure * 1.0 / (1<<16));
    int minc = UINT16_MAX+1;
    int maxc = 0;
    int fair = counter[0];
    for(int k = 0; k < range; ++k) {
        if(counter[k]<minc) minc = counter[k];
        if(counter[k]>maxc) maxc = counter[k];
    }
    printf("%d ** %d +++  %d -- %d +++ %d \n",-minc+maxc,minc,UINT16_MAX/range,(UINT16_MAX+range-1)/range,maxc);
    if(minc == maxc) printf("******fair\n");
    else {printf("======unfair \n"); return -1;}
    return 0;
}



int main() {

    for(int k = 1; k <60000; k++) {
        int r = fogapproach(k);
        if(r<0) return r;
        r = reversefogapproach(k);
        if(r<0) return r;
        r = fasterreversefogapproach(k);
        if(r<0) return r;

        r = fasterthanfogapproach(k);
        if(r<0) return r;

        r = classicapproach(k);
        if(r<0) return r;
        printf("\n\n");



    }
    return 0;
}
