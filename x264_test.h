#include "stdint.h"
typedef struct
{
    int     i_poc; 		//Picture order count, in presentation order(In one GOP)
    int     i_type;
    int     i_qpplus1;
    int     i_frame;    	/* Presentation frame number , (in Absoulte order)*/
    int     i_frame_num; 	/* Coded frame number ,in decoding(encoding) order */
    int     b_kept_as_ref;
    float   f_qp_avg;

    /* YUV buffer */
    int     i_plane;
    int     i_stride[4]; // Why 4??
    int     i_lines[4];
    int     i_stride_lowres;
    int     i_lines_lowres;
    uint8_t *plane[4];
    uint8_t *filtered[4]; /* plane[0], H, V, HV */
    uint8_t *lowres[4]; /* half-size copy of input frame: Orig, H, V, HV */
    uint16_t *integral;
    void    *buffer[12];
} x264_frame_t;
typedef struct
{
    int     i_csp; 		// Color space
    int     i_plane;	// ??	
    int     i_stride[4];//
    uint8_t *plane[4];	// Data
} x264_image_t;

typedef struct
{
    int     i_type;
    int     i_qpplus1;
    int64_t i_pts;
    x264_image_t img;
} x264_picture_t;