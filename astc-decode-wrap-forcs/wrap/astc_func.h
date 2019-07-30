
#include "astc_codec_internals.h"

astc_codec_image *load_astc_file(const char *filename, int bitness, astc_decode_mode decode_mode, swizzlepattern swz_decode);

astc_codec_image *pack_and_unpack_astc_image(const astc_codec_image * input_image,
											 int xdim,
											 int ydim,
											 int zdim,
											 const error_weighting_params * ewp, astc_decode_mode decode_mode, swizzlepattern swz_encode, swizzlepattern swz_decode, int bitness, int threadcount);

unsigned get_number_of_cpus(void);