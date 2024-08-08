#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 4096

void xor_encrypt(char *data, int data_len, char *key, int key_len) {
    for (int i = 0; i < data_len; i++) {
        data[i] ^= key[i % key_len];
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <input_file> <output_file> <key>\n", argv[0]);
        return 1;
    }

    char *input_file = argv[1];
    char *output_file = argv[2];
    char *key = argv[3];
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

    while (ReadFile(h_input, buffer, BUFFER_SIZE, &bytes_read, NULL) && bytes_read > 0) {
        xor_encrypt(buffer, bytes_read, key, key_len);
        if (!WriteFile(h_output, buffer, bytes_read, &bytes_written, NULL) || bytes_written != bytes_read) {
            printf("Error writing to output file\n");
            CloseHandle(h_input);
            CloseHandle(h_output);
            return 1;
        }
    }

    CloseHandle(h_input);
    CloseHandle(h_output);
    printf("File encrypted successfully\n");
    return 0;
}
