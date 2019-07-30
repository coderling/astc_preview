using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

public static class ASTCWrap
{
    [DllImport("astc_wrap_lib")]
    public static extern int Add(int a, int b);

    [DllImport("astc_wrap_lib")]
    public unsafe static extern byte* GetUnsignedCharArr();

    [DllImport("astc_wrap_lib")]
    public unsafe static extern IntPtr GetCustomStruct();

    [DllImport("astc_wrap_lib")]
    public unsafe static extern IntPtr pack_and_unpack_image(string file_path, int compress_mode, int decode_mode, int sw_alpha, int quality);

    public delegate void ErrorCall(string msg);

    [DllImport("astc_wrap_lib")]
    public static extern void SetUnityError(ErrorCall call);

    public unsafe struct astc_codec_image
    {
        public byte*** imagedata8;
        public UInt16*** imagedata16;
        public int xsize;
        public int ysize;
        public int zsize;
        public int padding;
    };
}
