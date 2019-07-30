
#pragma once

#include "astc_func.h"

#ifdef WRAP_EXPORT
#define WRAP_API __declspec(dllexport)
#define EXTERN_C extern "C"
#else
#define WRAP_API 
#define EXTERN_C
#endif


typedef void (__stdcall *UnityError)(char* msg);

// export interface
EXTERN_C WRAP_API int Add(int a, int b);
EXTERN_C WRAP_API unsigned char* GetUnsignedCharArr();
EXTERN_C WRAP_API astc_codec_image* GetCustomStruct();
//file_path: 文件路径 compress_mode：压缩模式4x4... decode_mode: 
EXTERN_C WRAP_API astc_codec_image* pack_and_unpack_image(const char* file_path, int compress_mode, int decode_mode, int sw_alpha, int quality);
EXTERN_C WRAP_API void SetUnityError(UnityError error);
// end export interface
