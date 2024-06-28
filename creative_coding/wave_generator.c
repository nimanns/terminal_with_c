#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define NUM_SAMPLES 44100 * 3
#define FREQUENCY_C4 261.63
#define FREQUENCY_G4 392.00

void write_wav_header(FILE *file) {
    uint32_t num_samples = NUM_SAMPLES;
    uint32_t sample_rate = SAMPLE_RATE;
    uint32_t byte_rate = SAMPLE_RATE * sizeof(short);
    uint16_t block_align = sizeof(short) * 1;
    uint16_t bits_per_sample = sizeof(short) * 8;
    uint32_t data_size = NUM_SAMPLES * sizeof(short);

    fwrite("RIFF", 1, 4, file);
    fwrite(&data_size, sizeof(data_size), 1, file);
    fwrite("WAVE", 1, 4, file);
    fwrite("fmt ", 1, 4, file);
    fwrite("\x10\0\0\0", sizeof(uint32_t), 1, file);
    fwrite("\x01\0", sizeof(uint16_t), 1, file);
    fwrite("\x01\0", sizeof(uint16_t), 1, file);
    fwrite(&sample_rate, sizeof(sample_rate), 1, file);
    fwrite(&byte_rate, sizeof(byte_rate), 1, file);
    fwrite(&block_align, sizeof(block_align), 1, file);
    fwrite(&bits_per_sample, sizeof(bits_per_sample), 1, file);
    fwrite("data", 1, 4, file);
    fwrite(&data_size, sizeof(data_size), 1, file);
}

int main() {
    FILE *file;
    const char *filename = "generated_wave_file.wav";
    short sample;
    double t, dt;
    double freq;

    file = fopen(filename, "wb");
    if (!file) {
        printf("Error opening file\n");
        return 1;
    }

    write_wav_header(file);

    dt = 1.0 / SAMPLE_RATE;
    t = 0.0;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        if (i < NUM_SAMPLES / 3 || i >= NUM_SAMPLES * 2 / 3) {
            freq = FREQUENCY_C4;
        } else {
            freq = FREQUENCY_G4;
        }

        sample = (short)(32767.0 * sin(2.0 * M_PI * freq * t));
        fwrite(&sample, sizeof(sample), 1, file);

        t += dt;
    }

    fclose(file);
    printf("WAV file created: %s\n", filename);
    return 0;
}
