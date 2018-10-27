#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <android/log.h>
#include <malloc.h>
extern "C"{
#include "jpeglib.h"
}

typedef uint8_t BYTE;

void writeImage(BYTE *data, const char *path, int w, int h);

void writeImage(BYTE *data, const char *path, int w, int h) {
    //压缩
    //--------------------------初始化
    struct jpeg_compress_struct jpeg_struct;
    struct jpeg_error_mgr err;
    //预防发生错误
    jpeg_struct.err = jpeg_std_error(&err);

    jpeg_create_compress(&jpeg_struct);
    //设置输出路径
    FILE *file = fopen(path, "wb");
    jpeg_stdio_dest(&jpeg_struct, file);
    //设置宽高
    jpeg_struct.image_height = h;
    jpeg_struct.image_width = w;
    //skia 阉割的地方
    //采用最优哈夫曼编码
    jpeg_struct.arith_code = FALSE;
    jpeg_struct.optimize_coding = TRUE;
    jpeg_struct.in_color_space = JCS_RGB;
    jpeg_struct.input_components = 3;
    //其他默认设置
    jpeg_set_defaults(&jpeg_struct);
    jpeg_set_quality(&jpeg_struct, 20, TRUE);

    //------------------------开始压缩
    jpeg_start_compress(&jpeg_struct, TRUE);
    //-------------------------写入数据
    JSAMPROW row_pointer[1];
    //一行rgb数量
    int row_stride = jpeg_struct.image_width * 3;
    while (jpeg_struct.next_scanline < jpeg_struct.image_height) {
        //计算一行的rgb索引
        row_pointer[0] = &data[jpeg_struct.next_scanline * row_stride];
        jpeg_write_scanlines(&jpeg_struct, row_pointer, 1);
    }

    //关闭
    jpeg_finish_compress(&jpeg_struct);
    jpeg_destroy_compress(&jpeg_struct);
    fclose(file);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_create_whc_wxpicturecompress_MainActivity_nativeCompress(JNIEnv *env, jobject instance,
                                                                  jobject inputBitmap,
                                                                  jstring absolutePath_) {
    const char *path = env->GetStringUTFChars(absolutePath_, 0);

    BYTE *pixels;
    AndroidBitmapInfo bitmapInfo;
    AndroidBitmap_getInfo(env, inputBitmap, &bitmapInfo);
    AndroidBitmap_lockPixels(env, inputBitmap, reinterpret_cast<void **>(&pixels));
    int h = bitmapInfo.height;
    int w = bitmapInfo.width;
    int i = 0;
    int j = 0;
    int color;
    BYTE *data = NULL, *tmpData = NULL;
    //r g b
    data = static_cast<BYTE *>(malloc(h * w * 3));
    tmpData = data;
    BYTE r, g, b;
    for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
            color = *((int *) pixels);
            r = ((color & 0x00FF0000) >> 16);
            g = ((color & 0x0000FF00) >> 8);
            b = (color & 0x000000FF);
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;
            pixels += 4;
        }
    }

    AndroidBitmap_unlockPixels(env, inputBitmap);
    //jpeg压缩
    writeImage(tmpData, path, w, h);

    env->ReleaseStringUTFChars(absolutePath_, path);
}