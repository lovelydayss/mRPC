// FFmpegDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
}

int main() {
    std::cout << "Hello World!\n";

    // std::cout << avcodec_configuration() << std::endl;
    // avcodec_register_all();

    char* info = (char*) malloc(40000);
    memset(info, 0, 40000);

    avcodec_register_all();

    AVCodec* c_temp = av_codec_next(NULL);

    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            strcat(info, "[Decode]");
        } else {
            strcat(info, "[Encode]");
        }
        switch (c_temp->type) {
        case AVMEDIA_TYPE_VIDEO:
            strcat(info, "[Video]");
            break;
        case AVMEDIA_TYPE_AUDIO:
            strcat(info, "[Audeo]");
            break;
        default:
            strcat(info, "[Other]");
            break;
        }
        sprintf(info, "%s %10s\n", info, c_temp->name);
        c_temp = c_temp->next;
    }
    puts(info);
    free(info);
    return 0;
}
