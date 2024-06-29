#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_ID "RIFF"
#define FORMAT "WAVE"
#define SUBCHUNK1_ID "fmt "
#define SUBCHUNK2_ID "data"

typedef struct {
    char chunk_id[4];
    int chunk_size;
    char format[4];
    char subchunk1_id[4];
    int subchunk1_size;
    short audio_format;
    short num_channels;
    int sample_rate;
    int byte_rate;
    short block_align;
    short bits_per_sample;
    char subchunk2_id[4];
    int subchunk2_size;
} WavHeader;

void reverse_audio(FILE *input, FILE *output, WavHeader header) {
    int data_size = header.subchunk2_size;
    short *audio_data = (short *)malloc(data_size);
    
    fread(audio_data, 1, data_size, input);
    
    int num_samples = data_size / sizeof(short);
    for (int i = 0; i < num_samples / 2; i++) {
        short temp = audio_data[i];
        audio_data[i] = audio_data[num_samples - 1 - i];
        audio_data[num_samples - 1 - i] = temp;
    }
    
    fwrite(audio_data, 1, data_size, output);
    free(audio_data);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file.wav> <output_file.wav>\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        printf("Error opening input file.\n");
        return 1;
    }

    FILE *output_file = fopen(argv[2], "wb");
    if (!output_file) {
        printf("Error opening output file.\n");
        fclose(input_file);
        return 1;
    }

    WavHeader header;
    fread(&header, sizeof(WavHeader), 1, input_file);

    if (strncmp(header.chunk_id, CHUNK_ID, 4) != 0 ||
        strncmp(header.format, FORMAT, 4) != 0 ||
        strncmp(header.subchunk1_id, SUBCHUNK1_ID, 4) != 0 ||
        strncmp(header.subchunk2_id, SUBCHUNK2_ID, 4) != 0) {
        printf("Invalid WAV file format.\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    fwrite(&header, sizeof(WavHeader), 1, output_file);
    reverse_audio(input_file, output_file, header);

    fclose(input_file);
    fclose(output_file);
    printf("Audio reversed successfully.\n");
    return 0;
}
