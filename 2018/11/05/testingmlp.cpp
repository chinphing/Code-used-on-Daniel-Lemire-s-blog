#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
static uint64_t murmur64(uint64_t h) {
    h ^= h >> 33;
    h *= UINT64_C(0xff51afd7ed558ccd);
    h ^= h >> 33;
    h *= UINT64_C(0xc4ceb9fe1a85ec53);
    h ^= h >> 33;
    return h;
}

template <size_t howmanylanes>
uint64_t access(uint64_t * bigarray, size_t length, size_t howmanyhits) {
    size_t howmanyhits_perlane = howmanyhits / howmanylanes;
    uint64_t lanesums[howmanylanes];
    for(size_t i = 0; i < howmanylanes; i++) lanesums[i] = 0;
    for(size_t counter = 0; counter < howmanyhits_perlane; counter++) {
        for(size_t i = 0; i < howmanylanes; i++) {
            size_t laneindexes = murmur64(lanesums[i] + i) & (length - 1);
            lanesums[i] += bigarray[laneindexes];
        }

    }
    uint64_t sum = 0;
    for(size_t i = 0; i < howmanylanes; i++) sum += lanesums[i];
    //std::cout << howmanylanes << " : " << sum << std::endl;
    return sum;
}
template <size_t howmanylanes>
float time_me(uint64_t * bigarray, size_t length, size_t howmanyhits, size_t repeat) {
    clock_t begin_time, end_time;
    float mintime = 99999999999;
        uint64_t bogus = 0;

    while(repeat-- > 0) {

    begin_time = clock();
    bogus += access<howmanylanes>(bigarray,length,howmanyhits);
    end_time = clock();
    float tv = float( end_time - begin_time ) /  CLOCKS_PER_SEC;
    if(tv < mintime) mintime = tv;
    }
    if(bogus == 0x010101) {
        printf("ping!");
    }
    std::cout << howmanylanes << " " << mintime << std::endl;
    return mintime; 

}

int measure(size_t length) {
        uint64_t * bigarray = (uint64_t * ) malloc(sizeof(uint64_t) * length);
    for(size_t i = 0; i < length; i++) bigarray[i] = 3 * i - i + 1;
    float time_measure[15];
    size_t howmanyhits = 1*4*5*6*7*8*9*11*13;
    int repeat = 5;

    time_measure[1] = time_me<1>(bigarray,length,howmanyhits,repeat);
    time_measure[2] = time_me<2>(bigarray,length,howmanyhits,repeat);
    time_measure[3] = time_me<3>(bigarray,length,howmanyhits,repeat);
    time_measure[4] = time_me<4>(bigarray,length,howmanyhits,repeat);
        time_measure[5] = time_me<5>(bigarray,length,howmanyhits,repeat);
            time_measure[6] = time_me<6>(bigarray,length,howmanyhits,repeat);
                time_measure[7] = time_me<7>(bigarray,length,howmanyhits,repeat);
                    time_measure[8] = time_me<8>(bigarray,length,howmanyhits,repeat);
                        time_measure[9] = time_me<9>(bigarray,length,howmanyhits,repeat);
        time_measure[10] = time_me<10>(bigarray,length,howmanyhits,repeat);
time_measure[11] = time_me<11>(bigarray,length,howmanyhits,repeat);
time_measure[12] = time_me<12>(bigarray,length,howmanyhits,repeat);
time_measure[13] = time_me<13>(bigarray,length,howmanyhits,repeat);
time_measure[14] = time_me<14>(bigarray,length,howmanyhits,repeat);

    for(size_t i = 2; i <= 14; i++) {
        float ratio = (time_measure[i-1] - time_measure[i]) /time_measure[i-1] ;
        printf("%f \n", ratio);

        if(ratio< 0.05) // if a new lane does not add at least 5% of performance...
        {
            std::cout << "Maybe you have about " << i - 1 << " parallel paths? " << std::endl;
            return i - 1;
            break;
        }
     }
     return 10000;
}

int main() {
    size_t length = 1 << 25;
    for(size_t i = 0; i < 3; i++) measure(length);

}