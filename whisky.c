#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include <sys/ioctl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

void cleanup(int)
{
    printf("\033[?25h");
    fflush(stdout);
    system("rm -rf audio.mp3");
    system("rm -rf frames/*");
    exit(0);
}


int main(int argc, char *argv[])
{
    signal(SIGINT, cleanup);

    if(argc != 3){
        printf("Usage: %s <video name> <fps>\n",argv[0]);
        return 1;
    }
    
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    char command1[100];
    char command2[100];
    sprintf(command1, "ffmpeg -i %s -vf \"scale=%d:%d,fps=%s\" frames/frame_%%06d.png", argv[1],w.ws_col,w.ws_row - 1,argv[2]);
    sprintf(command2, "ffmpeg -i %s -vn -ar 44100 -ac 2 audio.mp3", argv[1]);
    
    system(command1);
    system(command2);

    if(system(command1) != 0){
        printf("Failed to extract frames. FFmpeg might not be installed.\n");
        return 1;
    }
    
    ma_engine engine;
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        printf("Failed to init audio engine\n");
        return 1;
    }
    
    
    ma_sound sound;
    if (ma_sound_init_from_file(&engine, "audio.mp3", 0, NULL, NULL, &sound) != MA_SUCCESS) {
        printf("Failed to load audio.\n");
        return 1;
    }
    ma_sound_start(&sound);
    
    double fps = atoi(argv[2]);
    
    char brightness_ramp[] = " .,-~:;=!*#$@";
    int ramp_length = strlen(brightness_ramp); 
    
    char str[32];
    int frame_counter = 0;
    
    ma_uint32 sample_rate;
    ma_sound_get_data_format(&sound, NULL, NULL, &sample_rate, NULL, 0);
    
    int width, height, channels;

    for(;;){
        printf("\033[H");
        frame_counter++;
        
        double expected_frame_time = (frame_counter - 1) / fps;
        ma_uint64 pcm_pos;
        double audio_time;
        
        for(;;){
            ma_sound_get_cursor_in_pcm_frames(&sound, &pcm_pos);
            audio_time = (double)pcm_pos / sample_rate;
            
            if(audio_time >= expected_frame_time) break;
            usleep(1000);
        }
        
        int target_frame = (int)(audio_time * fps) + 1;
        if(target_frame > frame_counter){
            frame_counter = target_frame;
        }
        
        snprintf(str, sizeof(str), "frames/frame_%06d.png", frame_counter);
        unsigned char *pixels = stbi_load(str, &width, &height, &channels, 3);
        
        unsigned char *p = pixels;
        
        
        if(pixels == NULL){
            printf("Error Loading image.......\n");
            printf("%s\n", stbi_failure_reason());
            break;
        }
        
        printf("\033[?25l");
        fflush(stdout);
        
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                unsigned char r = *p++;
                unsigned char g = *p++;
                unsigned char b = *p++;
                
                float avg = 0.2126*r + 0.7152*g + 0.0722*b;
                int ramp_index = (int)(ramp_length * (avg/255.0f));
                
                printf("\033[38;2;%d;%d;%d;48;2;%d;%d;%dm",r,g,b,0,0,0);
                putchar(brightness_ramp[ramp_index]);
            }  
            putchar('\n');
        }
        
        stbi_image_free(pixels);
    }
    printf("\033[?25h");
    fflush(stdout);

    ma_sound_uninit(&sound);
    ma_engine_uninit(&engine);
    system("rm -rf audio.mp3");
    system("rm -rf frames/*");
    return 0;
}