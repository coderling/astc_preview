#include "decode_wrap.h"
#ifdef DEBUG_PRINT_DIAGNOSTICS
	#include <stdio.h>
#endif

//for test use
int Add(int a, int b)
{
    return MIN(a, b);
}

 unsigned char* GetUnsignedCharArr()
{
    unsigned char* output = new unsigned char[10];
    for (size_t i = 0; i < 10; i++)
    {
        output[i] = i; 
    }

    return output;
}

astc_codec_image* GetCustomStruct()
{
    astc_codec_image* img = allocate_image(8, 1024, 720, 1, 0);

	int x, y, z;

	int exsize = img->xsize + 2 * img->padding;
	int eysize = img->ysize + 2 * img->padding;
	int ezsize = (img->zsize == 1) ? 1 : img->zsize + 2 * img->padding;

	if (img->imagedata8)
	{
		for (z = 0; z < ezsize; z++)
			for (y = 0; y < eysize; y++)
				for (x = 0; x < exsize; x++)
				{
					img->imagedata8[z][y][4 * x] = 0xFF;
					img->imagedata8[z][y][4 * x + 1] = 0;
					img->imagedata8[z][y][4 * x + 2] = 0;
					img->imagedata8[z][y][4 * x + 3] = 0xFF;
				}
	}
    return img;
}

static UnityError logFunc = nullptr;
void SetUnityError(UnityError error)
{
    logFunc = error;
}

enum ECompressMode
{
    BLOCK_4x4 = 1,
    BLOCK_5x5 = 2,
    BLOCK_6x6 = 3,
    BLOCK_8x8 = 4,
    BLOCK_10x10 = 5,
    BLOCK_12x12 = 6,
};

enum ECompressQuality
{
    QUALITY_VERYFAST = 0,
    QUALITY_FAST,
    QUALITY_MEDIUM,
    QUALITY_THOROUGH,
    QUALITY_EXHAUSTIVE,
};

astc_codec_image* pack_and_unpack_image(const char* file_path, int compress_mode, int decode_mode, int sw_alpha, int quality)
{
    prepare_angular_tables();
    build_quantization_mode_table();
    int result;
    astc_codec_image* input_image = astc_codec_load_image(file_path, 0, &result);
    if(result < 0)
    {
        return nullptr;
    }
    int xdim_2d = 0, ydim_2d = 0, zdim_2d = 1;
    ECompressMode mode = (ECompressMode)compress_mode;
    switch (mode)
    {
        case BLOCK_4x4: 
        {
            xdim_2d = ydim_2d = 4;
        }
        break;
        case BLOCK_5x5:
        {
            xdim_2d = ydim_2d = 5;
        }    
        case BLOCK_6x6:
        {
            xdim_2d = ydim_2d = 6;
        }   
        break; 
        case BLOCK_8x8:
        {
            xdim_2d = ydim_2d = 8;
        }   
        break; 
        case BLOCK_10x10:
        {
            xdim_2d = ydim_2d = 10;
        }   
        break; 
        case BLOCK_12x12:
        {
            xdim_2d = ydim_2d = 12;
        }   
        break; 
        default:
            return nullptr;
    }

    astc_decode_mode inner_decode_mode = (astc_decode_mode)decode_mode;    

    //default weight params
	error_weighting_params ewp;
	ewp.rgb_power = 1.0f;
	ewp.alpha_power = 1.0f;
	ewp.rgb_base_weight = 1.0f;
	ewp.alpha_base_weight = 1.0f;
	ewp.rgb_mean_weight = 0.0f;
	ewp.rgb_stdev_weight = 0.0f;
	ewp.alpha_mean_weight = 0.0f;
	ewp.alpha_stdev_weight = 0.0f;

	ewp.rgb_mean_and_stdev_mixing = 0.0f;
	ewp.mean_stdev_radius = 0;
	ewp.enable_rgb_scale_with_alpha = 0;
	ewp.alpha_radius = 0;

	ewp.block_artifact_suppression = 0.0f;
	ewp.rgba_weights[0] = 1.0f;
	ewp.rgba_weights[1] = 1.0f;
	ewp.rgba_weights[2] = 1.0f;
	ewp.rgba_weights[3] = 1.0f;
	ewp.ra_normal_angular_scale = 0;

    ewp.max_refinement_iters = 0;
    ewp.block_mode_cutoff = 0;
    ewp.texel_avg_error_limit = 0;

    int padding = MAX(ewp.mean_stdev_radius, ewp.alpha_radius);

	swizzlepattern swz_encode = { 0, 1, 2, 3 }; 
    //只能是4或5表示是否开启Alpha， 默认开启
    if(sw_alpha != 4 || sw_alpha != 5)
    {
        sw_alpha = 5;
    }

    swz_encode.a = (uint8_t)sw_alpha;
	swizzlepattern swz_decode = { 0, 1, 2, 3 };

    float oplimit = 0.0f;
    float mincorrel = 0.0f;
    int bmc = 0;
    int maxiters = 0;
    int plimit = 0;
    int pcdiv = 0;

    ECompressQuality e_quality = (ECompressQuality)quality;

    switch (e_quality)
    {
    case QUALITY_VERYFAST:
        {
            plimit = 2;
            oplimit = 1.0f;
            bmc = 25;
            mincorrel = 0.5;
            maxiters = 1;
			
            switch (ydim_2d)
			{
			case 4:
				pcdiv = 240;
				break;
			case 5:
				pcdiv = 56;
				break;
			case 6:
				pcdiv = 64;
				break;
			case 8:
				pcdiv = 47;
				break;
			case 10:
				pcdiv = 36;
				break;
			case 12:
				pcdiv = 30;
				break;
			default:
				pcdiv = 30;
				break;
			}
        }
        break;
    
    case QUALITY_FAST:
        {
            plimit = 4;
            oplimit = 1.0f;
            bmc = 50;
            mincorrel = 0.5;
            maxiters = 1;
			
            switch (ydim_2d)
			{
			case 4:
				pcdiv = 60;
				break;
			case 5:
				pcdiv = 27;
				break;
			case 6:
				pcdiv = 30;
				break;
			case 8:
				pcdiv = 24;
				break;
			case 10:
				pcdiv = 16;
				break;
			case 12:
				pcdiv = 20;
				break;
			default:
				pcdiv = 20;
				break;
			}
        }
        break;
    case QUALITY_MEDIUM:
        {
            plimit = 25;
            oplimit = 1.2f;
            bmc = 75;
            mincorrel = 0.75;
            maxiters = 2;
			
            switch (ydim_2d)
			{
			case 4:
				pcdiv = 25;
				break;
			case 5:
				pcdiv = 15;
				break;
			case 6:
				pcdiv = 15;
				break;
			case 8:
				pcdiv = 10;
				break;
			case 10:
				pcdiv = 8;
				break;
			case 12:
				pcdiv = 6;
				break;
			default:
				pcdiv = 6;
				break;
			}
        }
        break;
    case QUALITY_THOROUGH:
        {
            plimit = 100;
            oplimit = 2.5f;
            bmc = 95;
            mincorrel = 0.95;
            maxiters = 4;
			
            switch (ydim_2d)
			{
			case 4:
				pcdiv = 12;
				break;
			case 5:
				pcdiv = 7;
				break;
			case 6:
				pcdiv = 7;
				break;
			case 8:
				pcdiv = 5;
				break;
			case 10:
				pcdiv = 4;
				break;
			case 12:
				pcdiv = 3;
				break;
			default:
				pcdiv = 3;
				break;
			}
        }
        break;
    case QUALITY_EXHAUSTIVE:
        {
            plimit = PARTITION_COUNT;
            oplimit = 1000.0f;
            bmc = 100;
            mincorrel = 0.99;
            maxiters = 4;
			
            switch (ydim_2d)
			{
			case 4:
				pcdiv = 3;
				break;
			case 5:
				pcdiv = 1;
				break;
			case 6:
				pcdiv = 1;
				break;
			case 8:
				pcdiv = 1;
				break;
			case 10:
				pcdiv = 1;
				break;
			case 12:
				pcdiv = 1;
				break;
			default:
				pcdiv = 1;
				break;
			}
        }
        break;
    default:
        printf("ERROR: error quality\n");
        break;
    }

    if(plimit < 1)
    {
        plimit = 1;
    }
    else if(plimit > PARTITION_COUNT)
    {
        plimit = PARTITION_COUNT;
    }

    ewp.partition_search_limit = plimit;
    ewp.partition_1_to_2_limit = oplimit;
    ewp.lowest_correlation_cutoff = mincorrel;
    ewp.block_mode_cutoff = bmc;
    ewp.max_refinement_iters = maxiters;

    printf("Tips: plimit: %d oplimit %f mincorrel %f bmc %d maxiters %d\n", plimit, oplimit, mincorrel, bmc, maxiters);

    expand_block_artifact_suppression(xdim_2d, ydim_2d, zdim_2d, &ewp);
    if (padding > 0 || ewp.rgb_mean_weight != 0.0f || ewp.rgb_stdev_weight != 0.0f || ewp.alpha_mean_weight != 0.0f || ewp.alpha_stdev_weight != 0.0f)
    {
        compute_averages_and_variances(input_image, ewp.rgb_power, ewp.alpha_power, ewp.mean_stdev_radius, ewp.alpha_radius, swz_encode);
    }

    int bitness = get_output_filename_enforced_bitness(file_path);
    //仅考虑8bit
    if(bitness == -1 || bitness != 8)
    {
        if(inner_decode_mode == DECODE_HDR)
        {
            logFunc("fail decode mode ....");
            return nullptr;
        }

        bitness = 8;
    }
    else if(bitness != 8)
    {
        bitness = 8;
    }

	int thread_count = get_number_of_cpus();
    astc_codec_image* output_image = pack_and_unpack_astc_image(input_image, xdim_2d, ydim_2d, zdim_2d, &ewp, inner_decode_mode, swz_encode, swz_decode, bitness, thread_count);
    
    //unpack array
    if(output_image != nullptr)
    {
		if(logFunc != nullptr)
			logFunc("sucess ....");
        return output_image; 
    }
	if(logFunc != nullptr)
		logFunc("sucess ....");
    return nullptr;
}
