#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#pragma comment(lib, "winmm.lib")

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <wav_file_path>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    char command[256];
    MCIERROR error;

    snprintf(command, sizeof(command), "play %s", filename);
    
    error = mciSendString(command, NULL, 0, NULL);
    
    if (error) {
        char errorText[256];
        mciGetErrorString(error, errorText, sizeof(errorText));
        printf("Error playing WAV file: %s\n", errorText);
        return 1;
    }
    
    printf("Playing %s. Press Enter to stop...\n", filename);
    getchar();
    
    mciSendString("stop all", NULL, 0, NULL);
    
    return 0;
}
