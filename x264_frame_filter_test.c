#ifdef _X264_FRAME_FILTER_TEST
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "x264_test.h"

void *x264_malloc( int i_size )
{
  uint8_t * buf;
  uint8_t * align_buf;
  buf = (uint8_t *) malloc( i_size + 15 + sizeof( void ** ) +
    sizeof( int ) );
  align_buf = buf + 15 + sizeof( void ** ) + sizeof( int );
  align_buf -= (long) align_buf & 15;
  *( (void **) ( align_buf - sizeof( void ** ) ) ) = buf;
  *( (int *) ( align_buf - sizeof( void ** ) - sizeof( int ) ) ) = i_size;
  return align_buf;
}
void x264_free( void *p )
{
  if( p )
  {
    free( *( ( ( void **) p ) - 1 ) );
  }
}
/* Read the Y ,U ,V data individually */
int read_frame_yuv( x264_picture_t *p_pic, FILE* handle, int i_frame, int i_width, int i_height )
{
  static int prev_frame = -1;
  FILE *f = (FILE *)handle;

  if( i_frame != prev_frame+1 )
    if( fseek( f, (uint64_t)i_frame * i_width * i_height * 3 / 2, SEEK_SET ) )
      return -1;

  if( fread( p_pic->img.plane[0], 1, i_width * i_height, f ) <= 0
    || fread( p_pic->img.plane[1], 1, i_width * i_height / 4, f ) <= 0
    || fread( p_pic->img.plane[2], 1, i_width * i_height / 4, f ) <= 0 )
    return -1;

  prev_frame = i_frame;

  return 0;
}
void x264_picture_alloc( x264_picture_t *pic, int i_width, int i_height )
{
  pic->i_type = 0;//X264_TYPE_AUTO;
  pic->i_qpplus1 = 0;
  pic->img.i_plane = 3;
  pic->img.plane[0] = x264_malloc( 3 * i_width * i_height / 2 );
  pic->img.plane[1] = pic->img.plane[0] + i_width * i_height;
  pic->img.plane[2] = pic->img.plane[1] + i_width * i_height / 4;
  pic->img.i_stride[0] = i_width;
  pic->img.i_stride[1] = i_width / 2;
  pic->img.i_stride[2] = i_width / 2;
}
void x264_picture_clean( x264_picture_t *pic )
{
  x264_free( pic->img.plane[0] );
  /* just to be safe */
  memset( pic, 0, sizeof( x264_picture_t ) );
}
void x264_frame_init(x264_frame_t  *frame,int i_width,int i_height)
{
  //  = x264_malloc( sizeof( x264_frame_t ) );
  int i, j;

  int i_mb_count = ((i_width+15)>>4)*((i_height+15)>>4);
  int i_stride;
  int i_lines;

  memset( frame, 0, sizeof(x264_frame_t) );

  /* allocate frame data (+64 for extra data for me) */
  i_stride = ( ( i_width  + 15 )&0xfffff0 )+ 64;
  i_lines  = ( ( i_height + 15 )&0xfffff0 );

  frame->i_plane = 3;
  for( i = 0; i < 3; i++ )
  {
    int i_divh = 1;
    int i_divw = 1;
    if( i > 0 )
    {
      i_divh = i_divw = 2;
    }
    frame->i_stride[i] = i_stride / i_divw;
    frame->i_lines[i] = i_lines / i_divh;
    frame->buffer[i] = x264_malloc( frame->i_stride[i] *
      ( frame->i_lines[i] + 64 / i_divh ) );

    frame->plane[i] = ((uint8_t*)frame->buffer[i]) +
      frame->i_stride[i] * 32 / i_divh + 32 / i_divw;
  }
  frame->i_stride[3] = 0;
  frame->i_lines[3] = 0;
  frame->buffer[3] = NULL;
  frame->plane[3] = NULL;

  frame->filtered[0] = frame->plane[0];
  for( i = 0; i < 3; i++ )
  {
    frame->buffer[4+i] = x264_malloc( frame->i_stride[0] *
      ( frame->i_lines[0] + 64 ) );

    frame->filtered[i+1] = ((uint8_t*)frame->buffer[4+i]) +
      frame->i_stride[0] * 32 + 32;
  }

  // if( h->frames.b_have_lowres )
  {
    frame->i_stride_lowres = frame->i_stride[0]/2 + 32;
    frame->i_lines_lowres = frame->i_lines[0]/2;
    for( i = 0; i < 4; i++ )
    {
      frame->buffer[7+i] = x264_malloc( frame->i_stride_lowres *
        ( frame->i_lines[0]/2 + 64 ) );
      frame->lowres[i] = ((uint8_t*)frame->buffer[7+i]) +
        frame->i_stride_lowres * 32 + 32;
    }
  }

  //  if( h->param.analyse.i_me_method == X264_ME_ESA )
  {
    frame->buffer[11] = x264_malloc( frame->i_stride[0] * (frame->i_lines[0] + 64) * sizeof(uint16_t) );
    frame->integral = (uint16_t*)frame->buffer[11] + frame->i_stride[0] * 32 + 32;
  }
}
void x264_frame_delete( x264_frame_t *frame )
{
  int i;
  for( i = 0; i < frame->i_plane; i++ )
    x264_free( frame->buffer[i] );
  for( i = 4; i < 12; i++ ) /* filtered planes */
    x264_free( frame->buffer[i] );
}
static void plane_copy( uint8_t *dst, int i_dst,
  uint8_t *src, int i_src, int w, int h)
{
  for( ; h > 0; h-- ) {
    memcpy( dst, src, w );
    dst += i_dst;
    src += i_src;
  }
}
static void i420_to_i420( x264_frame_t *frm, x264_image_t *img,
  int i_width, int i_height )
{
  plane_copy( frm->plane[0], frm->i_stride[0],
    img->plane[0], img->i_stride[0],
    i_width, i_height );
  plane_copy( frm->plane[1], frm->i_stride[1],
    img->plane[1], img->i_stride[1],
    i_width / 2, i_height / 2 );
  plane_copy( frm->plane[2], frm->i_stride[2],
    img->plane[2], img->i_stride[2],
    i_width / 2, i_height / 2 );
}

void x264_frame_copy_picture( x264_frame_t *dst, x264_picture_t *src,int i_width,int i_height )
{
  dst->i_type     = src->i_type;
  dst->i_qpplus1  = src->i_qpplus1;
  i420_to_i420(dst,&(src->img),i_width,i_height);
}
static void plane_expand_border( uint8_t *pix, int i_stride, int i_height, int i_pad )
{
#define PPIXEL(x, y) ( pix + (x) + (y)*i_stride )
  const int i_width = i_stride - 2*i_pad;
  int y;

  for( y = 0; y < i_height; y++ )
  {
    /* left band */
    memset( PPIXEL(-i_pad, y), PPIXEL(0, y)[0], i_pad );
    /* right band */
    memset( PPIXEL(i_width, y), PPIXEL(i_width-1, y)[0], i_pad );
  }
  /* upper band */
  for( y = 0; y < i_pad; y++ )
    memcpy( PPIXEL(-i_pad, -y-1), PPIXEL(-i_pad, 0), i_stride );
  /* lower band */
  for( y = 0; y < i_pad; y++ )
    memcpy( PPIXEL(-i_pad, i_height+y), PPIXEL(-i_pad, i_height-1), i_stride );
#undef PPIXEL
}
void x264_frame_expand_border( x264_frame_t *frame )
{
  int i;
  for( i = 0; i < frame->i_plane; i++ )
  {
    int i_pad = i ? 16 : 32; //??
    plane_expand_border( frame->plane[i], frame->i_stride[i], frame->i_lines[i], i_pad );
  }
}
void x264_frame_expand_border_filtered( x264_frame_t *frame )
{
  /* during filtering, 8 extra pixels were filtered on each edge. 
  we want to expand border from the last filtered pixel */
  int i;
  for( i = 1; i < 4; i++ )
    plane_expand_border( frame->filtered[i] - 8*frame->i_stride[0] - 8, frame->i_stride[0], frame->i_lines[0]+2*8, 24 );
}

typedef void (*pf_mc_t)(uint8_t *src, int i_src_stride, uint8_t *dst, int i_dst_stride, int i_width, int i_height );
static int x264_tapfilter( uint8_t *pix, int i_pix_next )
{
  return pix[-2*i_pix_next] - 5*pix[-1*i_pix_next] + 20*(pix[0] + pix[1*i_pix_next]) - 5*pix[ 2*i_pix_next] + pix[ 3*i_pix_next];
}
static int x264_tapfilter1( uint8_t *pix )
{
  return pix[-2] - 5*pix[-1] + 20*(pix[0] + pix[1]) - 5*pix[ 2] + pix[ 3];
}
/*Half pixel interpolation horizontally*/
static void mc_hh( uint8_t *src, int i_src_stride, uint8_t *dst, int i_dst_stride, int i_width, int i_height )
{
  int x, y;

  for( y = 0; y < i_height; y++ )
  {
    for( x = 0; x < i_width; x++ )
    {
      dst[x] = /*x264_mc_clip1*/( ( x264_tapfilter1( &src[x] ) + 16 ) >> 5 );
    }
    src += i_src_stride;
    dst += i_dst_stride;
  }
}
/*Half pixel interpolation vertically*/
static void mc_hv( uint8_t *src, int i_src_stride, uint8_t *dst, int i_dst_stride, int i_width, int i_height )
{
  int x, y;

  for( y = 0; y < i_height; y++ )
  {
    for( x = 0; x < i_width; x++ )
    {
      dst[x] = ( ( x264_tapfilter( &src[x], i_src_stride ) + 16 ) >> 5 );
    }
    src += i_src_stride;
    dst += i_dst_stride;
  }
}
/*Interpolation diagonally*/
static void mc_hc( uint8_t *src, int i_src_stride, uint8_t *dst, int i_dst_stride, int i_width, int i_height )
{
  uint8_t *out;
  uint8_t *pix;
  int x, y;

  for( x = 0; x < i_width; x++ )
  {
    int tap[6];

    pix = &src[x];
    out = &dst[x];
    /*Don't need border check ??*/
    tap[0] = x264_tapfilter1( &pix[-2*i_src_stride] );
    tap[1] = x264_tapfilter1( &pix[-1*i_src_stride] );
    tap[2] = x264_tapfilter1( &pix[ 0*i_src_stride] );
    tap[3] = x264_tapfilter1( &pix[ 1*i_src_stride] );
    tap[4] = x264_tapfilter1( &pix[ 2*i_src_stride] );

    for( y = 0; y < i_height; y++ )
    {
      tap[5] = x264_tapfilter1( &pix[ 3*i_src_stride] );

      *out = ( ( tap[0] - 5*tap[1] + 20 * tap[2] + 20 * tap[3] -5*tap[4] + tap[5] + 512 ) >> 10 );

      /* Next line */
      pix += i_src_stride;
      out += i_dst_stride;
      tap[0] = tap[1];
      tap[1] = tap[2];
      tap[2] = tap[3];
      tap[3] = tap[4];
      tap[4] = tap[5];
    }
  }
}
void x264_frame_filter( x264_frame_t *frame )
{
  const int x_inc = 16, y_inc = 16;
  const int stride = frame->i_stride[0];
  int x, y;

  pf_mc_t int_h = mc_hh;  //Horizontal
  pf_mc_t int_v = mc_hv;  //Vertical
  pf_mc_t int_hv = mc_hc; //Diagonal

  {
    for( y = -8; y < frame->i_lines[0]+8; y += y_inc )
    {
      uint8_t *p_in = frame->plane[0] + y * stride - 8;
      uint8_t *p_h  = frame->filtered[1] + y * stride - 8;
      uint8_t *p_v  = frame->filtered[2] + y * stride - 8;
      uint8_t *p_hv = frame->filtered[3] + y * stride - 8;
      for( x = -8; x < stride - 64 + 8; x += x_inc )
      {
        int_h(  p_in, stride, p_h,  stride, x_inc, y_inc );
        int_v(  p_in, stride, p_v,  stride, x_inc, y_inc );
        int_hv( p_in, stride, p_hv, stride, x_inc, y_inc );

        p_h += x_inc;
        p_v += x_inc;
        p_hv += x_inc;
        p_in += x_inc;
      }
    }
  }

  /* generate integral image:
  * each entry in frame->integral is the sum of all luma samples above and
  * to the left of its location (inclusive).
  * this allows us to calculate the DC of any rectangle by looking only
  * at the corner entries.
  * individual entries will overflow 16 bits, but that's ok:
  * we only need the differences between entries, and those will be correct
  * as long as we don't try to evaluate a rectangle bigger than 16x16.
  * likewise, we don't really have to init the edges to 0, leaving garbage
  * there wouldn't affect the results.*/
  // Important
  if( frame->integral )
  {
    memset( frame->integral - 32 * stride - 32, 0, stride * sizeof(uint16_t) );
    for( y = -31; y < frame->i_lines[0] + 32; y++ )
    {
      uint8_t  *ref  = frame->plane[0] + y * stride - 32;
      uint16_t *line = frame->integral + y * stride - 32;
      uint16_t v = line[0] = 0;
      for( x = 1; x < stride; x++ )
        line[x] = v += ref[x] + line[x-stride] - line[x-stride-1];
    }
  }
}
int get_frame_total_yuv( FILE* handle, int i_width, int i_height )
{
  FILE *f = (FILE *)handle;
  int i_frame_total = 0;

  if( !fseek( f, 0, SEEK_END ) )
  {
    uint64_t i_size = ftell( f );
    fseek( f, 0, SEEK_SET );
    i_frame_total = (int)(i_size / ( i_width * i_height * 3 / 2 ));
  }

  return i_frame_total;
}
int main11(){
  char *psz_filename="E:/sequences/akiyo_176x144_25.yuv";
  FILE* p_handle;
  int i_width,i_height;
  int i_frame_total;
  int i;
  x264_picture_t pic;
  x264_frame_t  frame;

  i_width=176;
  i_height=144;
  if ((p_handle = fopen(psz_filename, "rb")) == NULL)
    return -1;
  i_frame_total=get_frame_total_yuv(p_handle,i_width,i_height);
  x264_picture_alloc(&pic,i_width,i_height);
  x264_frame_init(&frame,i_width,i_height);
  for(i=0;i<i_frame_total;i++)
  {
    read_frame_yuv(&pic,p_handle,i,i_width,i_height);
    x264_frame_copy_picture(&frame,&pic,i_width,i_height);
    x264_frame_filter(&frame);
  }


  x264_picture_clean(&pic);
  x264_frame_delete(&frame);
  fclose(p_handle);
  return 0;
}
#endif