#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

float clamp(float value, float min, float max) {
    return fminf(fmaxf(value, min), max);
}

void apply_delay(FILE *input, FILE *output, WavHeader header, float delay_seconds, float decay) {
    int delay_samples = (int)(delay_seconds * header.sample_rate);
    int num_samples = header.subchunk2_size / sizeof(short);
    short *audio_data = (short *)malloc(header.subchunk2_size);
    float *delayed_data = (float *)calloc(num_samples + delay_samples, sizeof(float));
    
    fread(audio_data, 1, header.subchunk2_size, input);
    
    for (int i = 0; i < num_samples; i++) {
        delayed_data[i] += audio_data[i] / 32768.0f;
        if (i + delay_samples < num_samples + delay_samples) {
            delayed_data[i + delay_samples] += (audio_data[i] / 32768.0f) * decay;
        }
    }
    
    for (int i = 0; i < num_samples + delay_samples; i++) {
        delayed_data[i] = clamp(delayed_data[i], -1.0f, 1.0f);
        delayed_data[i] = tanh(delayed_data[i]);
    }
    
    float max_amplitude = 0;
    for (int i = 0; i < num_samples + delay_samples; i++) {
        if (fabsf(delayed_data[i]) > max_amplitude) {
            max_amplitude = fabsf(delayed_data[i]);
        }
    }
    
    float normalization_factor = 0.95f / max_amplitude;
    for (int i = 0; i < num_samples + delay_samples; i++) {
        short sample = (short)(delayed_data[i] * normalization_factor * 32767.0f);
        fwrite(&sample, sizeof(short), 1, output);
    }
    
    header.subchunk2_size = (num_samples + delay_samples) * sizeof(short);
    header.chunk_size = 36 + header.subchunk2_size;
    
    fseek(output, 0, SEEK_SET);
    fwrite(&header, sizeof(WavHeader), 1, output);
    
    free(audio_data);
    free(delayed_data);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Hint: %s <input_file.wav> <output_file.wav> <delay_seconds> <decay>\n", argv[0]);
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

    float delay_seconds = atof(argv[3]);
    float decay = atof(argv[4]);

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

    apply_delay(input_file, output_file, header, delay_seconds, decay);

    fclose(input_file);
    fclose(output_file);
    printf("Delay effect applied successfully.\n");
    return 0;
}
