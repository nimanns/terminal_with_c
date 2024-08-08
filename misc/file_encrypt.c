#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 4096

void xor_encrypt(char *data, int data_len, char *key, int key_len) {
    for (int i = 0; i < data_len; i++) {
        data[i] ^= key[i % key_len];
    }
}

int process_file(char *input_file, char *output_file, char *key, int encrypt) {
    int key_len = strlen(key);

    HANDLE h_input = CreateFile(input_file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h_input == INVALID_HANDLE_VALUE) {
        printf("Error opening input file\n");
        return 1;
    }

    HANDLE h_output = CreateFile(output_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h_output == INVALID_HANDLE_VALUE) {
        printf("Error creating output file\n");
        CloseHandle(h_input);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    DWORD bytes_read, bytes_written;
    LARGE_INTEGER file_size, progress;
    DWORD start_time, end_time;

    GetFileSizeEx(h_input, &file_size);
    progress.QuadPart = 0;
    start_time = GetTickCount();

    while (ReadFile(h_input, buffer, BUFFER_SIZE, &bytes_read, NULL) && bytes_read > 0) {
        if (encrypt) {
            xor_encrypt(buffer, bytes_read, key, key_len);
        }
        if (!WriteFile(h_output, buffer, bytes_read, &bytes_written, NULL) || bytes_written != bytes_read) {
            printf("Error writing to output file\n");
            CloseHandle(h_input);
            CloseHandle(h_output);
            return 1;
        }
        progress.QuadPart += bytes_read;
        printf("\rProgress: %.2f%%", (double)progress.QuadPart / file_size.QuadPart * 100);
    }

    end_time = GetTickCount();
    CloseHandle(h_input);
    CloseHandle(h_output);

    double elapsed_time = (end_time - start_time) / 1000.0;
    double speed = file_size.QuadPart / (1024.0 * 1024.0) / elapsed_time;

    printf("\nFile %s successfully\n", encrypt ? "encrypted" : "decrypted");
    printf("Time elapsed: %.2f seconds\n", elapsed_time);
    printf("Processing speed: %.2f MB/s\n", speed);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <e|d> <input_file> <output_file> <key>\n", argv[0]);
        return 1;
    }

    int encrypt = argv[1][0] == 'e';
    return process_file(argv[2], argv[3], argv[4], encrypt);
}
