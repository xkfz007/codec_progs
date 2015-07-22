#include <stdio.h>
#include <stdlib.h>
#ifdef _X264_SATD8X8_TEST
typedef int VMF_S32;
typedef unsigned char pixel;
typedef unsigned int VMF_U32;
typedef short VMF_S16;
typedef unsigned short VMF_U16;

#define BITS_PER_SUM  (8*sizeof(VMF_S16))

#define HADAMARD4(d0, d1, d2, d3, s0, s1, s2, s3) { \
  VMF_U32 const t0 = s0 + s1; \
  VMF_U32 const t1 = s0 - s1; \
  VMF_U32 const t2 = s2 + s3; \
  VMF_U32 const t3 = s2 - s3; \
  d0 = t0 + t2; \
  d2 = t0 - t2; \
  d1 = t1 + t3; \
  d3 = t1 - t3; \
}
VMF_U32 abs2(VMF_U32 a) {
  VMF_U32 s = ((a >> (BITS_PER_SUM - 1)) & (((VMF_U32)1 << BITS_PER_SUM) + 1)) * ((VMF_U16) - 1);
  return (a + s) ^ s;
}

// x264's SWAR version of satd 8x4, performs two 4x4 SATDs at once
VMF_S32 satd_8x4(const pixel* pix1, VMF_S32 const stride_pix1, const pixel* pix2, VMF_S32 const stride_pix2) {
  VMF_U32 tmp[4][4];
  VMF_U32 a0, a1, a2, a3;
  VMF_U32 sum = 0;

  for(VMF_S32 i = 0; i < 4; i++, pix1 += stride_pix1, pix2 += stride_pix2) {
    a0 = (pix1[0] - pix2[0]) + ((VMF_U32)(pix1[4] - pix2[4]) << BITS_PER_SUM);
    a1 = (pix1[1] - pix2[1]) + ((VMF_U32)(pix1[5] - pix2[5]) << BITS_PER_SUM);
    a2 = (pix1[2] - pix2[2]) + ((VMF_U32)(pix1[6] - pix2[6]) << BITS_PER_SUM);
    a3 = (pix1[3] - pix2[3]) + ((VMF_U32)(pix1[7] - pix2[7]) << BITS_PER_SUM);
    HADAMARD4(tmp[i][0], tmp[i][1], tmp[i][2], tmp[i][3], a0, a1, a2, a3);
  }

  for(VMF_S32 i = 0; i < 4; i++) {
    HADAMARD4(a0, a1, a2, a3, tmp[0][i], tmp[1][i], tmp[2][i], tmp[3][i]);
    sum += abs2(a0) + abs2(a1) + abs2(a2) + abs2(a3);
  }

  return (((VMF_U16)sum) + (sum >> BITS_PER_SUM)) >> 1;
}
typedef unsigned short sum_t;
typedef unsigned int sum2_t;
static int x264_pixel_satd_8x4( pixel *pix1, intptr_t i_pix1, pixel *pix2, intptr_t i_pix2 )
{
  sum2_t tmp[4][4];
  sum2_t a0, a1, a2, a3;
  sum2_t sum = 0;
  for( int i = 0; i < 4; i++, pix1 += i_pix1, pix2 += i_pix2 )
  {
    a0 = (pix1[0] - pix2[0]) + ((sum2_t)(pix1[4] - pix2[4]) << BITS_PER_SUM);
    a1 = (pix1[1] - pix2[1]) + ((sum2_t)(pix1[5] - pix2[5]) << BITS_PER_SUM);
    a2 = (pix1[2] - pix2[2]) + ((sum2_t)(pix1[6] - pix2[6]) << BITS_PER_SUM);
    a3 = (pix1[3] - pix2[3]) + ((sum2_t)(pix1[7] - pix2[7]) << BITS_PER_SUM);
    HADAMARD4( tmp[i][0], tmp[i][1], tmp[i][2], tmp[i][3], a0,a1,a2,a3 );
  }
  for( int i = 0; i < 4; i++ )
  {
    HADAMARD4( a0, a1, a2, a3, tmp[0][i], tmp[1][i], tmp[2][i], tmp[3][i] );
    sum += abs2(a0) + abs2(a1) + abs2(a2) + abs2(a3);
  }
  return (((sum_t)sum) + (sum>>BITS_PER_SUM)) >> 1;
}
// calculate satd in blocks of 8x4
static VMF_S32 as265_pixel_satd_8x8_c(pixel *pix1, VMF_S32 const stride_pix1,pixel *pix2, VMF_S32 const stride_pix2) { 
  VMF_S32 satd = 0; 
  for(VMF_S32 row = 0; row < 8; row += 4) { 
    for (VMF_S32 col = 0; col < 8; col += 8) { 
      satd += satd_8x4(pix1 + row * stride_pix1 + col, stride_pix1, 
        pix2 + row * stride_pix2 + col, stride_pix2); 
    } 
  } 

  // int satd= satd_8x4( pix1, stride_pix1, pix2, stride_pix2 )
  //        + satd_8x4( pix1+4*stride_pix1, stride_pix1, pix2+4*stride_pix2, stride_pix2 );
  return satd; 
}

static VMF_S32 as265_pixel_sad_8x8_c( const pixel* pix1, VMF_S32 const i_stride_pix1, 
  const pixel* pix2, VMF_S32 const i_stride_pix2 ) { 
    VMF_S32 i_sum = 0; 
    for( VMF_S32 y = 0; y < 8; y++ ) { 
      for( VMF_S32 x = 0; x < 8; x++ ) { 
        i_sum += abs( pix1[x] - pix2[x] ); 
      } 
      pix1 += i_stride_pix1; 
      pix2 += i_stride_pix2; 
    } 
    return i_sum; 
}

static int x264_pixel_satd_8x8( pixel *pix1, intptr_t i_pix1, pixel *pix2, intptr_t i_pix2 )
{
  int sum = x264_pixel_satd_8x4( pix1, i_pix1, pix2, i_pix2 )
    + x264_pixel_satd_8x4( pix1+4*i_pix1, i_pix1, pix2+4*i_pix2, i_pix2 );
  return sum;
}
int main2(){
  pixel src[8*8]={
    220,219,216,215,213,217,219,213,
    221,214,206,205,203,208,209,212,
    216,208,207,204,202,204,204,209,
    214,203,202,201,199,202,202,215,
    213,203,198,199,194,202,209,213,
    210,212,209,203,202,209,214,210,
    204,206,207,210,211,210,212,214,
    204,205,213,212,212,213,216,216,
  };
  pixel pred1[8*8]={
    216,218,221,222,220,220,218,216,
    216,218,221,222,220,220,218,216,
    216,218,221,222,220,220,218,216,
    216,218,221,222,220,220,218,216,
    216,218,221,222,220,220,218,216,
    216,218,221,222,220,220,218,216,
    216,218,221,222,220,220,218,216,
    216,218,221,222,220,220,218,216,

  };
  pixel pred2[8*8]={
    218,217,215,213,212,212,212,209,
    218,216,214,213,211,212,212,208,
    214,213,212,211,210,210,210,208,
    213,212,211,210,209,209,209,207,
    211,210,209,209,208,208,208,207,
    209,208,208,207,207,207,207,206,
    205,205,205,205,205,206,206,206,
    204,204,204,205,205,205,205,205,

  };
  int satd1=as265_pixel_satd_8x8_c(src,8,pred1,8);
  int satd2=as265_pixel_satd_8x8_c(src,8,pred2,8);

  int satd1_x=x264_pixel_satd_8x8(src,8,pred1,8);
  int satd2_x=x264_pixel_satd_8x8(src,8,pred2,8);

  int sad1=as265_pixel_sad_8x8_c(src,8,pred1,8);
  int sad2=as265_pixel_sad_8x8_c(src,8,pred2,8);

  printf("satd1=%d satd2=%d\n",satd1,satd2);
  printf("satd1_x=%d satd2_x=%d\n",satd1_x,satd2_x);
  printf("sad1=%d sad2=%d\n",sad1,sad2);

  return 0;

}
#endif