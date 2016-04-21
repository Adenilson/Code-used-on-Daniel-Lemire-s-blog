// gcc -march=native -O3 -o simdunion simdunion.c
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

#include <x86intrin.h>
static bool is_sorted(__m128i x) {
    return ( (uint32_t)_mm_extract_epi32(x,0)<=(uint32_t)_mm_extract_epi32(x,1))
           && ( (uint32_t)_mm_extract_epi32(x,1)<=(uint32_t)_mm_extract_epi32(x,2))
           && ( (uint32_t)_mm_extract_epi32(x,2)<=(uint32_t)_mm_extract_epi32(x,3));
}

// Inoue and Taura, SIMD- and Cache-Friendly Algorithm for Sorting an Array of Structures
static void sse_merge(__m128i *vA, __m128i *vB, // input 1 & 2
                      __m128i *vMin, __m128i *vMax) { // output
    __m128i vTmp; // temporary register

//vA = 0 1 6 7 (hypothetical)
//vB = 2 3 4 5 (hypothetical)

    vTmp = _mm_min_epu32(*vA, *vB);
// vTmp = 0 1 4 5
    *vMax = _mm_max_epu32(*vA, *vB);
// vMax = 2 3 6 7
    vTmp = _mm_alignr_epi8(vTmp, vTmp, 4);
// vTmp = 1 4 5 0 (rotate the min values)
    *vMin = _mm_min_epu32(vTmp, *vMax);
// vMin = 1 3 5 0
    *vMax = _mm_max_epu32(vTmp, *vMax);
// vMax = 2 4 6 7
    vTmp = _mm_alignr_epi8(*vMin, *vMin, 4);
// vTmp = 3 5 0 1 (rotate min)
    *vMin = _mm_min_epu32(vTmp, *vMax);
// vMin = 2 4 0 1
    *vMax = _mm_max_epu32(vTmp, *vMax);
// vMax = 3 5 6 7
    vTmp = _mm_alignr_epi8(*vMin, *vMin, 4);
// rottate min vTmp = 4 0 1 2
    *vMin = _mm_min_epu32(vTmp, *vMax);
// vMin = 3 0 1 2
    *vMax = _mm_max_epu32(vTmp, *vMax);
// vMax = 4 5 6 7
    *vMin = _mm_alignr_epi8(*vMin, *vMin, 4);
// result: 0 1 2 3
    assert(is_sorted(*vMin));
    assert(is_sorted(*vMax));
    assert((uint32_t)_mm_extract_epi32(*vMin,3)<=(uint32_t)_mm_extract_epi32(*vMax,0));

}

static void print32(__m128i x) {
    printf("%u %u %u %u \n", _mm_extract_epi32(x,0),_mm_extract_epi32(x,1),_mm_extract_epi32(x,2),_mm_extract_epi32(x,3));
}


static uint32_t sse_unite(uint32_t * __restrict__ array1, uint32_t length1 ,
                          uint32_t *  __restrict__ array2, uint32_t length2, uint32_t *  __restrict__  output) {
    __m128i vA, vB, V, vMin,vMax;
    assert(length1 % 4 == 0); // because I am lazy
    assert(length2 % 4 == 0); // because I am lazy
    assert(length1 != 0); // because I am lazy
    assert(length2 != 0); // because I am lazy
    uint32_t len1 = length1 / 4;
    uint32_t len2 = length2 / 4;
    uint32_t pos1 = 0;
    uint32_t pos2 = 0;
    uint32_t outpos = 0;

    // we start the machine
    vA = _mm_lddqu_si128((const __m128i* )array1 + pos1);
    pos1++;
    vB = _mm_lddqu_si128((const __m128i* )array2 + pos2);
    pos2++;
    uint32_t curA, curB;
    curA = (pos1 < len1) ? array1[4*pos1] : 0;
    curB = (pos2 < len2) ? array2[4*pos2] : 0;
    sse_merge(&vA,&vB,&vMin,&vMax);
    _mm_storeu_si128((__m128i* )output + outpos,vMin);
    outpos++;
    while((pos1 < len1) && (pos2<len2)) {
        // code is not optimal, too much branching?
        if(curA <= curB) {
          V = _mm_lddqu_si128((const __m128i* )array1 + pos1);
          pos1++;
          curA = (pos1 < len1) ? array1[4*pos1] : 0;
        } else {
          V = _mm_lddqu_si128((const __m128i* )array2 + pos2);
          pos2++;
          curB = (pos2 < len2) ? array2[4*pos2] : 0;
        }
        sse_merge(&V,&vMax,&vMin,&vMax);
        _mm_storeu_si128((__m128i* )output + outpos,vMin);
        outpos++;
    }
    //
    while(pos1<len1) {
      V = _mm_lddqu_si128((const __m128i* )array1 + pos1);
      pos1++;
      sse_merge(&V,&vMax,&vMin,&vMax);
      _mm_storeu_si128((__m128i* )output + outpos,vMin);
      outpos++;
    }
    while(pos2<len2) {
      V = _mm_lddqu_si128((const __m128i* )array2 + pos2);
      pos2++;
      sse_merge(&V,&vMax,&vMin,&vMax);
      _mm_storeu_si128((__m128i* )output + outpos,vMin);
      outpos++;
    }
    _mm_storeu_si128((__m128i* )output + outpos,vMax);
    outpos++;
    return outpos * 4;
}

void demo() {
    uint32_t array1[]= {0,1,6,7};
    uint32_t array2[]= {2,3,4,5};
    __m128i vA = _mm_lddqu_si128((const __m128i* )array1);
    __m128i vB = _mm_lddqu_si128((const __m128i* )array2);
    __m128i vMin;
    __m128i vMax;
    sse_merge(&vA,&vB,&vMin,&vMax);
    uint32_t * output = malloc(1024);
    _mm_storeu_si128((__m128i* )output,vMin);
    _mm_storeu_si128((__m128i* )output+1,vMax);
    for(int k = 0 ; k < 8; ++k) {
        printf("%u ", output[k]);
    }
    free(output);
    printf("\n");
}



void bigdemo(uint32_t size) {
    assert(size % 4 == 0);
    uint32_t * array1= malloc(size * sizeof(uint32_t));
    uint32_t len1 = size;
    uint32_t * array2= malloc(size * sizeof(uint32_t));
    uint32_t len2 = size;
    uint32_t last;
    last = 0;
    for(int k = 0; k < len1; ++k) {
        last = last + (rand()&0x0F);
        array1[k] =last;
    }
    last = 0;
    for(int k = 0; k < len1; ++k) {
        last = last + (rand()&0x0F);
        array2[k] = last;
    }
    uint32_t * output = malloc(2 * size * sizeof(uint32_t));
    uint32_t len = sse_unite(array1, len1,array2, len2,output);
    for(int k = 0 ; k < len; ++k) {
        if(k>0) {
            if(output[k]<output[k-1]) {
                printf("bug at k =%d out of %d ",k,len);
                abort();
            }
        }
    }
    free(output);
    free(array1);
    free(array2);
}


void skewedbigdemo(uint32_t size) {
    assert(size % 4 == 0);
    uint32_t * array1= malloc(size * sizeof(uint32_t));
    uint32_t len1 = size;
    uint32_t * array2= malloc(2 * size * sizeof(uint32_t));
    uint32_t len2 = 2 * size;
    uint32_t last;
    last = 0;
    for(int k = 0; k < len1; ++k) {
        last = last + (rand()&0x0F);
        array1[k] =last;
    }
    last = 0;
    for(int k = 0; k < len1; ++k) {
        last = last + (rand()&0x0F);
        array2[k] = last;
    }
    uint32_t * output = malloc((len1 + len2) * sizeof(uint32_t));
    uint32_t len = sse_unite(array1, len1,array2, len2,output);
    for(int k = 0 ; k < len; ++k) {
        if(k>0) {
            if(output[k]<output[k-1]) {
                printf("bug at k =%d out of %d ",k,len);
                abort();
            }
        }
    }
    free(output);
    free(array1);
    free(array2);
}

int main() {
    for(int k = 4; k <1024; k+=4) {
        printf("k=%d\n",k);
        for(int j = 0; j< 1000; ++j) {
          bigdemo(k);
        }
        for(int j = 0; j< 1000; ++j) {
          skewedbigdemo(k);
        }
    }
}
