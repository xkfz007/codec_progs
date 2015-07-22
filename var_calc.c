#ifdef _VAR_CALC
#include <stdio.h>
double pixel_var_wxh(int *pix1, int i_stride_pix1, int i_width,int i_height) 
{
    int x,y;
    double mea_val,var_val;
    mea_val=0;
    var_val=0;
    for( y = 0; y < i_height; y++ )
    {
        for( x = 0; x < i_width; x++ )
        {
            double pixel=pix1[x];
            mea_val+=pixel;
            var_val+=pixel*pixel;
        }
        pix1 += i_stride_pix1;
    }

    mea_val=mea_val/(i_height*i_width);
    var_val=var_val/(i_height*i_width)-mea_val*mea_val;

    return var_val;
}
int main(){
    int matrix[]={
35,     1,      6,      26 ,      19 ,      24, 
    3 ,   32,       7,        21,       23,       25, 
    31 ,  9,        2,        22,       27,       20, 
    8 ,   289,       33,       17,       10 ,      15, 
    30,   5,        34,       12,       14 ,      16, 
    4,    36,       29,       13,       18 ,      11, 
    };
    int stride=6;
    int width=6,height=6;
    double var;
    var=pixel_var_wxh(matrix,stride,width,height);
    printf("%f \n",var);
}
#endif
