// g++ -msse4.2 -O3 -o unarydecoding unarydecoding.cpp

/**

The idea here is to decode unary numbers. E.g.,

0 is written as 1
1 is written as 01
2 is written as 001
and so on.

How fast can you go?

**/

#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <nmmintrin.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vector>
#include <iostream>
#include <cassert>

class WallClockTimer
{
public:
    struct timeval t1, t2;
public:
    WallClockTimer() : t1(), t2() {
        gettimeofday(&t1,0);
        t2 = t1;
    }
    void reset() {
        gettimeofday(&t1,0);
        t2 = t1;
    }
    int elapsed() {
        return (t2.tv_sec * 1000 + t2.tv_usec / 1000) - (t1.tv_sec * 1000 + t1.tv_usec / 1000);
    }
    int split() {
        gettimeofday(&t2,0);
        return elapsed();
    }
};

using namespace std;

int bitscanunary_naive(long *bitmap, int bitmapsize, int *out) {
    int pos = 0;
    int val = 0, newval = 0;
    for(int k = 0; k < bitmapsize; ++k) {
        unsigned long bitset = bitmap[k];
        for(int bit = 0; bit < sizeof(bitset) * 8 ; ++bit) {
            if((bitset & (1UL<<bit)) != 0) {
                newval = k * 64+ bit;
                out[pos++] = newval - val;
                val = newval;
            }
        }
    }
    return pos;
}

int bitscanunary_popcnt(long *bitmap, int bitmapsize, int *out) {
    int pos = 0;
    int val = 0, newval = 0;
    for(int k = 0; k < bitmapsize; ++k) {
        long bitset = bitmap[k];
        while (bitset != 0) {
            long t = bitset & -bitset;
            newval = k * 64 +  __builtin_popcountl (t-1);//_mm_popcnt_u64 (t-1);
            out[pos++] = newval - val;
            val = newval;
            bitset ^= t;
        }
    }
    return pos;
}

// first byte on each row is the number of 1s, then we have a list
// of their position. We end on the number of trailing zeroes.
unsigned char hugearray[256][10] = {{0,8},
    {1,0,7},
    {1,1,6},
    {2,0,1,6},
    {1,2,5},
    {2,0,2,5},
    {2,1,2,5},
    {3,0,1,2,5},
    {1,3,4},
    {2,0,3,4},
    {2,1,3,4},
    {3,0,1,3,4},
    {2,2,3,4},
    {3,0,2,3,4},
    {3,1,2,3,4},
    {4,0,1,2,3,4},
    {1,4,3},
    {2,0,4,3},
    {2,1,4,3},
    {3,0,1,4,3},
    {2,2,4,3},
    {3,0,2,4,3},
    {3,1,2,4,3},
    {4,0,1,2,4,3},
    {2,3,4,3},
    {3,0,3,4,3},
    {3,1,3,4,3},
    {4,0,1,3,4,3},
    {3,2,3,4,3},
    {4,0,2,3,4,3},
    {4,1,2,3,4,3},
    {5,0,1,2,3,4,3},
    {1,5,2},
    {2,0,5,2},
    {2,1,5,2},
    {3,0,1,5,2},
    {2,2,5,2},
    {3,0,2,5,2},
    {3,1,2,5,2},
    {4,0,1,2,5,2},
    {2,3,5,2},
    {3,0,3,5,2},
    {3,1,3,5,2},
    {4,0,1,3,5,2},
    {3,2,3,5,2},
    {4,0,2,3,5,2},
    {4,1,2,3,5,2},
    {5,0,1,2,3,5,2},
    {2,4,5,2},
    {3,0,4,5,2},
    {3,1,4,5,2},
    {4,0,1,4,5,2},
    {3,2,4,5,2},
    {4,0,2,4,5,2},
    {4,1,2,4,5,2},
    {5,0,1,2,4,5,2},
    {3,3,4,5,2},
    {4,0,3,4,5,2},
    {4,1,3,4,5,2},
    {5,0,1,3,4,5,2},
    {4,2,3,4,5,2},
    {5,0,2,3,4,5,2},
    {5,1,2,3,4,5,2},
    {6,0,1,2,3,4,5,2},
    {1,6,1},
    {2,0,6,1},
    {2,1,6,1},
    {3,0,1,6,1},
    {2,2,6,1},
    {3,0,2,6,1},
    {3,1,2,6,1},
    {4,0,1,2,6,1},
    {2,3,6,1},
    {3,0,3,6,1},
    {3,1,3,6,1},
    {4,0,1,3,6,1},
    {3,2,3,6,1},
    {4,0,2,3,6,1},
    {4,1,2,3,6,1},
    {5,0,1,2,3,6,1},
    {2,4,6,1},
    {3,0,4,6,1},
    {3,1,4,6,1},
    {4,0,1,4,6,1},
    {3,2,4,6,1},
    {4,0,2,4,6,1},
    {4,1,2,4,6,1},
    {5,0,1,2,4,6,1},
    {3,3,4,6,1},
    {4,0,3,4,6,1},
    {4,1,3,4,6,1},
    {5,0,1,3,4,6,1},
    {4,2,3,4,6,1},
    {5,0,2,3,4,6,1},
    {5,1,2,3,4,6,1},
    {6,0,1,2,3,4,6,1},
    {2,5,6,1},
    {3,0,5,6,1},
    {3,1,5,6,1},
    {4,0,1,5,6,1},
    {3,2,5,6,1},
    {4,0,2,5,6,1},
    {4,1,2,5,6,1},
    {5,0,1,2,5,6,1},
    {3,3,5,6,1},
    {4,0,3,5,6,1},
    {4,1,3,5,6,1},
    {5,0,1,3,5,6,1},
    {4,2,3,5,6,1},
    {5,0,2,3,5,6,1},
    {5,1,2,3,5,6,1},
    {6,0,1,2,3,5,6,1},
    {3,4,5,6,1},
    {4,0,4,5,6,1},
    {4,1,4,5,6,1},
    {5,0,1,4,5,6,1},
    {4,2,4,5,6,1},
    {5,0,2,4,5,6,1},
    {5,1,2,4,5,6,1},
    {6,0,1,2,4,5,6,1},
    {4,3,4,5,6,1},
    {5,0,3,4,5,6,1},
    {5,1,3,4,5,6,1},
    {6,0,1,3,4,5,6,1},
    {5,2,3,4,5,6,1},
    {6,0,2,3,4,5,6,1},
    {6,1,2,3,4,5,6,1},
    {7,0,1,2,3,4,5,6,1},
    {1,7,0},
    {2,0,7,0},
    {2,1,7,0},
    {3,0,1,7,0},
    {2,2,7,0},
    {3,0,2,7,0},
    {3,1,2,7,0},
    {4,0,1,2,7,0},
    {2,3,7,0},
    {3,0,3,7,0},
    {3,1,3,7,0},
    {4,0,1,3,7,0},
    {3,2,3,7,0},
    {4,0,2,3,7,0},
    {4,1,2,3,7,0},
    {5,0,1,2,3,7,0},
    {2,4,7,0},
    {3,0,4,7,0},
    {3,1,4,7,0},
    {4,0,1,4,7,0},
    {3,2,4,7,0},
    {4,0,2,4,7,0},
    {4,1,2,4,7,0},
    {5,0,1,2,4,7,0},
    {3,3,4,7,0},
    {4,0,3,4,7,0},
    {4,1,3,4,7,0},
    {5,0,1,3,4,7,0},
    {4,2,3,4,7,0},
    {5,0,2,3,4,7,0},
    {5,1,2,3,4,7,0},
    {6,0,1,2,3,4,7,0},
    {2,5,7,0},
    {3,0,5,7,0},
    {3,1,5,7,0},
    {4,0,1,5,7,0},
    {3,2,5,7,0},
    {4,0,2,5,7,0},
    {4,1,2,5,7,0},
    {5,0,1,2,5,7,0},
    {3,3,5,7,0},
    {4,0,3,5,7,0},
    {4,1,3,5,7,0},
    {5,0,1,3,5,7,0},
    {4,2,3,5,7,0},
    {5,0,2,3,5,7,0},
    {5,1,2,3,5,7,0},
    {6,0,1,2,3,5,7,0},
    {3,4,5,7,0},
    {4,0,4,5,7,0},
    {4,1,4,5,7,0},
    {5,0,1,4,5,7,0},
    {4,2,4,5,7,0},
    {5,0,2,4,5,7,0},
    {5,1,2,4,5,7,0},
    {6,0,1,2,4,5,7,0},
    {4,3,4,5,7,0},
    {5,0,3,4,5,7,0},
    {5,1,3,4,5,7,0},
    {6,0,1,3,4,5,7,0},
    {5,2,3,4,5,7,0},
    {6,0,2,3,4,5,7,0},
    {6,1,2,3,4,5,7,0},
    {7,0,1,2,3,4,5,7,0},
    {2,6,7,0},
    {3,0,6,7,0},
    {3,1,6,7,0},
    {4,0,1,6,7,0},
    {3,2,6,7,0},
    {4,0,2,6,7,0},
    {4,1,2,6,7,0},
    {5,0,1,2,6,7,0},
    {3,3,6,7,0},
    {4,0,3,6,7,0},
    {4,1,3,6,7,0},
    {5,0,1,3,6,7,0},
    {4,2,3,6,7,0},
    {5,0,2,3,6,7,0},
    {5,1,2,3,6,7,0},
    {6,0,1,2,3,6,7,0},
    {3,4,6,7,0},
    {4,0,4,6,7,0},
    {4,1,4,6,7,0},
    {5,0,1,4,6,7,0},
    {4,2,4,6,7,0},
    {5,0,2,4,6,7,0},
    {5,1,2,4,6,7,0},
    {6,0,1,2,4,6,7,0},
    {4,3,4,6,7,0},
    {5,0,3,4,6,7,0},
    {5,1,3,4,6,7,0},
    {6,0,1,3,4,6,7,0},
    {5,2,3,4,6,7,0},
    {6,0,2,3,4,6,7,0},
    {6,1,2,3,4,6,7,0},
    {7,0,1,2,3,4,6,7,0},
    {3,5,6,7,0},
    {4,0,5,6,7,0},
    {4,1,5,6,7,0},
    {5,0,1,5,6,7,0},
    {4,2,5,6,7,0},
    {5,0,2,5,6,7,0},
    {5,1,2,5,6,7,0},
    {6,0,1,2,5,6,7,0},
    {4,3,5,6,7,0},
    {5,0,3,5,6,7,0},
    {5,1,3,5,6,7,0},
    {6,0,1,3,5,6,7,0},
    {5,2,3,5,6,7,0},
    {6,0,2,3,5,6,7,0},
    {6,1,2,3,5,6,7,0},
    {7,0,1,2,3,5,6,7,0},
    {4,4,5,6,7,0},
    {5,0,4,5,6,7,0},
    {5,1,4,5,6,7,0},
    {6,0,1,4,5,6,7,0},
    {5,2,4,5,6,7,0},
    {6,0,2,4,5,6,7,0},
    {6,1,2,4,5,6,7,0},
    {7,0,1,2,4,5,6,7,0},
    {5,3,4,5,6,7,0},
    {6,0,3,4,5,6,7,0},
    {6,1,3,4,5,6,7,0},
    {7,0,1,3,4,5,6,7,0},
    {6,2,3,4,5,6,7,0},
    {7,0,2,3,4,5,6,7,0},
    {7,1,2,3,4,5,6,7,0},
    {8,0,1,2,3,4,5,6,7,0}
};


int bitscanunary_table(long *bitmap, int bitmapsize, int *out) {
    int *initout = out;
    int val = 0, newval = 0;
    unsigned char * b8 = (unsigned char *) bitmap;
    for (int k = 0; k < bitmapsize*sizeof(long); ++k) {
        unsigned char* codes = hugearray[b8[k]];
        for(int offset = 0; offset<codes[0]; ++offset) {
            newval = 8 * k + codes[offset +1];
            out[offset] = newval - val;
            val = newval;
        }
        out += codes[0];
    }
    return out-initout;
}

int bitscanunary_ctzl(long *bitmap, int bitmapsize, int *out) {
    int pos = 0;
    int val = 0, newval = 0;
    for(int k = 0; k < bitmapsize; ++k) {
        unsigned long bitset = bitmap[k];
        while (bitset != 0) {
            long t = bitset & -bitset;
            int r = __builtin_ctzl(bitset);
            newval = k * 64 +  r;
            out[pos++] = newval - val;
            val = newval;
            bitset ^= t;
        }
    }
    return pos;
}

int main() {
    assert(sizeof(long)==8);
    assert(sizeof(int)==4);
    WallClockTimer timer;
    int repeat = 100;
    int N = 10000;
    cout<<"# We report bits-per-integer speed-of-naive speed-of-popcnt speed-of-table speed-of-tzcnt where speeds are in millions of integers per second "<<endl;
    for(int sb = 1; sb<=64; sb*=2) {
        int setbitsmax = sb*N;
        vector<long> bitmap(N);
        for (int k = 0; k < setbitsmax; ++k) {
            int bit = rand() % (N*64);
            bitmap[bit/64] |= (1L<<(bit%64));
        }
        int bitcount = 0;
        for(int k = 0; k <N; ++k) {
            bitcount += __builtin_popcountl(bitmap[k]);
        }
        double bitsperinteger = N*sizeof(long)*8.0/bitcount;
        vector<int> outputnaive(bitcount);
        vector<int> outputpopcnt(bitcount);
        vector<int> outputtable(bitcount);
        vector<int> outputctz(bitcount);

        cout<<"# Stored "<<bitcount<<" unary numbers in  ";
        cout<< N*sizeof(long)<<" bytes " ;
        cout<<" ("<<bitsperinteger<<" bits per number)"<<endl;
        timer.reset();
        int c0 = 0;
        for(int t1=0; t1<repeat; ++t1)
            c0 = bitscanunary_naive(bitmap.data(),N,outputnaive.data());
        int tinaive = timer.split();
        //cout<<"Decoded "<<c0<<" values with naive"<<endl;
        timer.reset();
        int c1 = 0;
        for(int t1=0; t1<repeat; ++t1)
            c1 = bitscanunary_popcnt(bitmap.data(),N,outputpopcnt.data());
        assert(c1 == c0);
        int tipopcnt = timer.split();
        //cout<<"Decoded "<<c1<<" values with pop"<<endl;
        timer.reset();
        int c2 = 0;
        for(int t1=0; t1<repeat; ++t1)
            c2 = bitscanunary_table(bitmap.data(),N,outputtable.data());
        assert(c2 == c0);
        int titable = timer.split();
        timer.reset();
        int c3 = 0;
        for(int t1=0; t1<repeat; ++t1)
            c3 = bitscanunary_ctzl(bitmap.data(),N,outputctz.data());
        assert(c3 == c0);
        int tictz = timer.split();
        //cout<<"Decoded "<<c2<<" values with naive"<<endl;
        assert (outputnaive == outputpopcnt);
        assert (outputnaive == outputtable);
        assert (outputnaive == outputctz);
        
        cout << bitsperinteger<<" " ;
        cout << bitcount * 0.001 /tinaive <<" ";
        cout << bitcount * 0.001 /tipopcnt <<" ";
        cout << bitcount * 0.001 /titable <<" ";
        cout << bitcount * 0.001 /tictz <<" ";
        cout << endl ;
    }

    return 0;
}
