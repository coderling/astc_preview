#include "Test.h"
#include "decode_wrap.h"

int sub(int a, int b)
{
    return a - b;
}

int main(int argc, char** argv)
{
    //astcenc -c example.png example.astc 6x6 -medium
    char* c = "-c";
    char* path = "C:\\dev\\TestEffect\\Assets\\glass.tga";
    char* out_path = "C:\\dev\\TestEffect\\Assets\\glass.astc";
    char* block = "12x12";
    char* qua = "-medium";
    char* arg[6];
    char* exe = "astc_warp.exe";
    arg[0] = exe;
    arg[1] = c;
    arg[2] = path;
    arg[3] = out_path;
    arg[4] = block;
    arg[5] = qua;
//    astc_main(6, arg);
    astc_codec_image* img = pack_and_unpack_image("C:\\dev\\TestEffect\\Assets\\glass.tga", 1, 0, 4);
	int store_result = -1;
	const char *format_string = "";

	store_result = astc_codec_store_image(img, "C:\\dev\\TestEffect\\Assets\\ASTC\\glass1.tga", 8, &format_string);
    return 0;
}