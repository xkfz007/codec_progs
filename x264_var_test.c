#ifdef _X264_VAR_TEST

typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef __int64  int64_t;
typedef unsigned __int64 uint64_t;
typedef uint8_t  pixel;
static uint64_t x264_pixel_var_8x8( pixel *pix, int i_stride ) 
{                                             
    uint32_t sum = 0, sqr = 0;                
	int h=16,w=16;
    for( int y = 0; y < h; y++ )              
    {                                         
        for( int x = 0; x < w; x++ )          
        {                                     
            sum += pix[x];                    
            sqr += pix[x] * pix[x];           
        }                                     
        pix += i_stride;                      
    }                                         
    return sum + ((uint64_t)sqr << 32);       
}
int main(){
	pixel pix[8*8]={
64,2,3,61,60,6,7,57
,9,55,54,12,13,51,50,16
,17,47,46,20,21,43,42,24
,40,26,27,37,36,30,31,33
,32,34,35,29,28,38,39,25
,41,23,22,44,45,19,18,48
,49,15,14,52,53,11,10,56
,8,58,59,5,4,62,63,1
	};
	uint64_t var=x264_pixel_var_8x8(pix,8);
	uint32_t sum=var;
	uint32_t ssd=var>>32;

	return 0;
	
}
#endif


#if 0
#include <stdio.h>
typedef signed char int8_t;
typedef unsigned char   uint8_t;
static const uint8_t exp2_lut[64] = {
      1,   4,   7,  10,  13,  16,  19,  22,  25,  28,  31,  34,  37,  40,  44,  47,
     50,  53,  57,  60,  64,  67,  71,  74,  78,  81,  85,  89,  93,  96, 100, 104,
    108, 112, 116, 120, 124, 128, 132, 137, 141, 145, 150, 154, 159, 163, 168, 172,
    177, 182, 186, 191, 196, 201, 206, 211, 216, 221, 226, 232, 237, 242, 248, 253,
};

static  int x264_exp2fix8_pre( float x )
{
    int i, f;
    x += 8;
    if( x <= 0 ) return 0;
    if( x >= 16 ) return 0xffff;
    i = x;
    f = (x-i)*64;
    return (exp2_lut[f]+256) << i >> 8;
}


const uint8_t x264_exp2_lut[64] =
{
      0,   3,   6,   8,  11,  14,  17,  20,  23,  26,  29,  32,  36,  39,  42,  45,
     48,  52,  55,  58,  62,  65,  69,  72,  76,  80,  83,  87,  91,  94,  98, 102,
    106, 110, 114, 118, 122, 126, 130, 135, 139, 143, 147, 152, 156, 161, 165, 170,
    175, 179, 184, 189, 194, 198, 203, 208, 214, 219, 224, 229, 234, 240, 245, 250
};
/* Not a general-purpose function; multiplies input by -1/6 to convert
 * qp to qscale. */
static  int x264_exp2fix8( float x )
{
    int i = x*(-64.f/6.f) + 512.5f;
    if( i < 0 ) return 0;
    if( i > 1023 ) return 0xffff;
    return (x264_exp2_lut[i&63]+256) << (i>>6) >> 8;
}
int main(){
	float x=1.3f;
	int a,b;
	a=x264_exp2fix8_pre(x*1.0f/6.0f);
	b=x264_exp2fix8(x);
	printf("a=%d b=%d\n",a,b);
}
#endif


#endif