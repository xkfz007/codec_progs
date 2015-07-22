/*
this program is used to compare the SSIM calculation codes of JM and that of x264
*/
#ifdef _SSIM_COMPARE
#include <cstdio>
#include <cstdlib>
typedef double VMF_DOUBLE;
typedef unsigned char VMF_U8;
typedef VMF_U8 pixel;
typedef unsigned int VMF_U32;
typedef int VMF_S32;
typedef unsigned int uint32_t;
typedef int int32_t;
#define BIT_DEPTH 8
#define MIN(a,b) (a<b?a:b)

//original SSIM calculation
static VMF_DOUBLE Calculate_SSIM(pixel* pic_r, VMF_U32 stride_r,
								 pixel* pic_i, VMF_U32 stride_i,
								 VMF_U32 width,VMF_U32 height) 
{

	static const VMF_DOUBLE K1 = 0.01f, K2 = 0.03f;
	static const VMF_S32 win_width=8,win_height=8;
	VMF_DOUBLE max_pix_value_sqd;
	VMF_DOUBLE C1, C2;

	VMF_DOUBLE win_pixels = (VMF_DOUBLE) (win_width * win_height);
	VMF_DOUBLE win_pixels_bias = win_pixels;

	VMF_DOUBLE mb_ssim, mean_i, mean_r;
	VMF_DOUBLE var_i, var_r, cov_ir;
	VMF_S32 imean_i, imean_r, ivar_i, ivar_r, icov_ir;
	VMF_DOUBLE cur_distortion = 0.0;
	VMF_S32 i, j, n, m, win_cnt = 0;
	VMF_S32 overlapSize = 8;
	pixel* p_i,*p_r;

	max_pix_value_sqd = ((1<<BIT_DEPTH)-1)*((1<<BIT_DEPTH)-1);
	C1 = K1 * K1 * max_pix_value_sqd;
	C2 = K2 * K2 * max_pix_value_sqd;

	for(VMF_U32 j = 0; j <= height-win_height; j+=overlapSize) {
		for(VMF_U32 i = 0; i <= width-win_width; i+=overlapSize) {

			imean_i = 0;
			imean_r = 0; 
			ivar_i  = 0;
			ivar_r  = 0;
			icov_ir = 0;

			p_i=pic_i+i;
			p_r=pic_r+i;

			for ( n = 0;n < 0 + win_height;n ++)
			{
				for (m = 0;m < 0 + win_width;m ++)
				{
					pixel px_i=*(p_i+m);
					pixel px_r=*(p_r+m);
					imean_i   += px_i;
					imean_r   += px_r;
					ivar_i    += px_i*px_i;
					ivar_r    += px_r*px_r;
					icov_ir += px_i * px_r;
				}
				p_i+=stride_i;
				p_r+=stride_r;
			}

			mean_i = (VMF_DOUBLE) imean_i / win_pixels;
			mean_r = (VMF_DOUBLE) imean_r / win_pixels;

			var_i    = ((VMF_DOUBLE) ivar_i - ((VMF_DOUBLE) imean_i) * mean_i) / win_pixels_bias;
			var_r    = ((VMF_DOUBLE) ivar_r - ((VMF_DOUBLE) imean_r) * mean_r) / win_pixels_bias;
			cov_ir = ((VMF_DOUBLE) icov_ir - ((VMF_DOUBLE) imean_i) * mean_r) / win_pixels_bias;

			mb_ssim  = (VMF_DOUBLE) ((2.0 * mean_i * mean_r + C1) * (2.0 * cov_ir + C2));
			mb_ssim /= (VMF_DOUBLE) (mean_i * mean_i + mean_r * mean_r + C1) * (var_i + var_r + C2);

			cur_distortion += mb_ssim;
			win_cnt++;

		}
		pic_i += overlapSize*stride_i;
		pic_r += overlapSize*stride_r;
	}
	cur_distortion /= (VMF_DOUBLE) win_cnt;

	if (cur_distortion >= 1.0 && cur_distortion < 1.01) // avoid VMF_DOUBLE accuracy problem at very low QP(e.g.2)
		cur_distortion = 1.0;

	return cur_distortion;

}	
/* structural similarity metric */
void ssim_4x4x2_core(const pixel *pix1, intptr_t stride1, const pixel *pix2, intptr_t stride2, int sums[2][4])
{
    for (int z = 0; z < 2; z++)
    {
        uint32_t s1 = 0, s2 = 0, ss = 0, s12 = 0;
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                int a = pix1[x + y * stride1];
                int b = pix2[x + y * stride2];
                s1 += a;
                s2 += b;
                ss += a * a;
                ss += b * b;
                s12 += a * b;
            }
        }

        sums[z][0] = s1;
        sums[z][1] = s2;
        sums[z][2] = ss;
        sums[z][3] = s12;
        pix1 += 4;
        pix2 += 4;
    }
}

float ssim_end_1(int s1, int s2, int ss, int s12)
{
/* Maximum value for 10-bit is: ss*64 = (2^10-1)^2*16*4*64 = 4286582784, which will overflow in some cases.
 * s1*s1, s2*s2, and s1*s2 also obtain this value for edge cases: ((2^10-1)*16*4)^2 = 4286582784.
 * Maximum value for 9-bit is: ss*64 = (2^9-1)^2*16*4*64 = 1069551616, which will not overflow. */

#define PIXEL_MAX ((1 << BIT_DEPTH) - 1)

#define type int
    static const int ssim_c1 = (int)(.01 * .01 * PIXEL_MAX * PIXEL_MAX * 64 + .5);
    static const int ssim_c2 = (int)(.03 * .03 * PIXEL_MAX * PIXEL_MAX * 64 * 63 + .5);
    type fs1 = (type)s1;
    type fs2 = (type)s2;
    type fss = (type)ss;
    type fs12 = (type)s12;
    type vars = (type)(fss * 64 - fs1 * fs1 - fs2 * fs2);
    type covar = (type)(fs12 * 64 - fs1 * fs2);
    return (float)(2 * fs1 * fs2 + ssim_c1) * (float)(2 * covar + ssim_c2)
           / ((float)(fs1 * fs1 + fs2 * fs2 + ssim_c1) * (float)(vars + ssim_c2));
#undef type
#undef PIXEL_MAX
}

float ssim_end_4(int sum0[5][4], int sum1[5][4], int width)
{
    float ssim = 0.0;

    for (int i = 0; i < width; i++)
    {
        ssim += ssim_end_1(sum0[i][0] + sum0[i + 1][0] + sum1[i][0] + sum1[i + 1][0],
                           sum0[i][1] + sum0[i + 1][1] + sum1[i][1] + sum1[i + 1][1],
                           sum0[i][2] + sum0[i + 1][2] + sum1[i][2] + sum1[i + 1][2],
                           sum0[i][3] + sum0[i + 1][3] + sum1[i][3] + sum1[i + 1][3]);
    }

    return ssim;
}
/* Function to calculate SSIM for each row */
static float calculate_ssim_x264(pixel *pix1, intptr_t stride1, pixel *pix2, intptr_t stride2, int width, int height, void *buf, int32_t* cnt)
{
    int z = 0;
    float ssim = 0.0;

    int(*sum0)[4] = (int(*)[4])buf;
    int(*sum1)[4] = sum0 + (width >> 2) + 3;
    width >>= 2;
    height >>= 2;
//	printf("width=%d height=%d ....\n",width,height);

    for (int y = 1; y < height; y++)
    {
        for (; z <= y; z++)
        {
            void* swap = sum0;
            sum0 = sum1;
            sum1 = (int(*)[4])swap;
            for (int x = 0; x < width; x += 2)
            {
                ssim_4x4x2_core(&pix1[(4 * x + (z * stride1))], stride1, &pix2[(4 * x + (z * stride2))], stride2, &sum0[x]);
            }
        }

        for (int x = 0; x < width - 1; x += 4)
        {
            ssim += ssim_end_4(sum0 + x, sum1 + x, MIN(4, width - x - 1));
        }
    }

    *cnt = (height - 1) * (width - 1);
    return ssim;
}

int main(){
	FILE* fp_i,*fp_r;
	//fp_i=fopen("e:/sequences/BlowingBubbles_416x240_50.yuv","rb");
	fp_i=fopen("e:/sequences/Mobisode2_416x240_30.yuv","rb");
	//fp_r=fopen("BlowingBubbles_416x240_50_rec.yuv","rb");
	fp_r=fopen("F:/encoder_test_output/x265_output/rec.yuv","rb");
	int frame_to_be_encoded=100;
	const int width=416,height=240;
	/*
	pixel* py_i=(pixel*)malloc(sizeof(pixel)*width*height);
	pixel* pu_i=(pixel*)malloc(sizeof(pixel)*width*height/4);
	pixel* pv_i=(pixel*)malloc(sizeof(pixel)*width*height/4);
	pixel* py_r=(pixel*)malloc(sizeof(pixel)*width*height);
	pixel* pu_r=(pixel*)malloc(sizeof(pixel)*width*height/4);
	pixel* pv_r=(pixel*)malloc(sizeof(pixel)*width*height/4);
	*/
	pixel py_i[width*height];
	pixel pu_i[width/2*height/2];
	pixel pv_i[width/2*height/2];
	pixel py_r[width*height];
	pixel pu_r[width/2*height/2];
	pixel pv_r[width/2*height/2];
	int m_ssimBuf[8*(width/4+3)];
	int dump;
	for(int i=0;i<frame_to_be_encoded;i++)
	{
		fread(py_i,sizeof(pixel),width*height,fp_i);
		fread(pu_i,sizeof(pixel),width*height/4,fp_i);
		fread(pv_i,sizeof(pixel),width*height/4,fp_i);

		fread(py_r,sizeof(pixel),width*height,fp_r);
		fread(pu_r,sizeof(pixel),width*height/4,fp_r);
		fread(pv_r,sizeof(pixel),width*height/4,fp_r);

		double ssimy1=Calculate_SSIM(py_r,width,py_i,width,width,height);
		double ssimu=Calculate_SSIM(pu_r,width/2,pu_i,width/2,width/2,height/2);
		double ssimv=Calculate_SSIM(pv_r,width/2,pv_i,width/2,width/2,height/2);
		//double ssimy2=calculateSSIM(py_r,width,py_i,width,width,height,m_ssimBuf,&dump);
		//ssimy2/=dump;
		//printf("SSIMY=%.4f %.4f \n",ssimy1,ssimy2);
		//printf("SSIMY=%.4f ",ssimy1);
		//printf("SSIMU=%.4f ",ssimu);
		//printf("SSIMV=%.4f \n",ssimv);
		printf(" [Y %.4lf U %.4lf V %.4lf ]\n", ssimy1, ssimu, ssimv);

	}
	/*
	free(py_i);
	free(pu_i);
	free(pv_i);
	free(py_r);
	free(pu_r);
	free(pv_r);
	*/
	fclose(fp_i);
	fclose(fp_r);

}
#endif