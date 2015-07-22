#ifdef _X264_SATD4X4_TEST
#include <stdio.h>

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

#define BITS_PER_SUM  (8*sizeof(VMF_S16))

// in: a pseudo-simd number of the form x+(y<<16)
// return: abs(x)+(abs(y)<<16)
inline VMF_U32 abs2(VMF_U32 a) {
  VMF_U32 s = ((a >> (BITS_PER_SUM - 1)) & (((VMF_U32)1 << BITS_PER_SUM) + 1)) * ((VMF_U16) - 1);
  return (a + s) ^ s;
}

VMF_S32 satd_4x4(const pixel* pix1, VMF_S32 const stride_pix1, const pixel* pix2, VMF_S32 const stride_pix2) {
  VMF_U32 tmp[4][2];
  VMF_U32 a0, a1, a2, a3, b0, b1;
  VMF_U32 sum = 0;

  for(VMF_S32 i = 0; i < 4; i++, pix1 += stride_pix1, pix2 += stride_pix2) {
    a0 = pix1[0] - pix2[0];
    a1 = pix1[1] - pix2[1];
    b0 = (a0 + a1) + ((a0 - a1) << BITS_PER_SUM);
    a2 = pix1[2] - pix2[2];
    a3 = pix1[3] - pix2[3];
    b1 = (a2 + a3) + ((a2 - a3) << BITS_PER_SUM);
    tmp[i][0] = b0 + b1;
    tmp[i][1] = b0 - b1;
  }

  for(VMF_S32 i = 0; i < 2; i++) {
    HADAMARD4(a0, a1, a2, a3, tmp[0][i], tmp[1][i], tmp[2][i], tmp[3][i]);
    a0 = abs2(a0) + abs2(a1) + abs2(a2) + abs2(a3);
    sum += ((VMF_U16)a0) + (a0 >> BITS_PER_SUM);
  }

  return (VMF_S32)(sum >> 1);
}
int main(){
  pixel src[4*4]={
    219, 211, 211, 214,
    211, 204, 202, 208,
    210, 203, 202, 212,
    205, 211, 212, 215,};
    pixel pred[4*4]={
      216, 212, 210, 210,
      211, 209, 209, 209,
      209, 208, 208, 208,
      205, 206, 206, 207,
    };
    int satd=satd_4x4(src,4,pred,4);
    printf("satd=%d\n",satd);
}
#endif