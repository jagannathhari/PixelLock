#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#define shuffle_array_row(arr, total_rows, total_cols, seed)                   \
    {                                                                          \
        srand(seed);                                                           \
        int j;                                                                 \
        typeof(arr[0]) temp;                                                   \
        for (unsigned int i = 0; i < total_rows; i++) {                        \
            j = rand() % total_rows;                                           \
            unsigned int x = total_cols * i;                                   \
            unsigned int y = total_cols * j;                                   \
            for (unsigned int k = 0; k < total_cols; k++) {                    \
                temp = arr[x + k];                                             \
                arr[x + k] = arr[y + k];                                       \
                arr[y + k] = temp;                                             \
            }                                                                  \
        }                                                                      \
    }

#define unshuffle_array_row(arr, total_rows, total_cols, seed, result)         \
    {                                                                          \
        int *dummy = malloc(sizeof(*dummy) * total_rows);                      \
        for (int i = 0; i < total_rows; i++)                                   \
            dummy[i] = i;                                                      \
        shuffle_array_row(dummy, total_rows, 1, seed);                         \
        srand(seed);                                                           \
        int j;                                                                 \
        for (unsigned int i = 0; i < total_rows; i++) {                        \
            j = rand() % total_rows;                                           \
            unsigned int x = total_cols * i;                                   \
            unsigned int y = total_cols * dummy[i];                            \
            for (unsigned int k = 0; k < total_cols; k++) {                    \
                result[y + k] = arr[x + k];                                    \
            }                                                                  \
        }                                                                      \
        free(dummy);                                                           \
    }

void ofuscate_image(char *input_image_path, char *ouput_image_path, long seed) {
    int width, height, channels;
    unsigned char *image =
        stbi_load(input_image_path, &width, &height, &channels, 0);
    if (!image) {
        puts("Unable to open image\n");
        return;
    }
    shuffle_array_row(image, height, width * channels, seed);
    stbi_write_png(ouput_image_path, width, height, channels, image,
                   width * channels);
    stbi_image_free(image);
}

void deofuscate_image(char *input_image_path, char *ouput_image_path,
                      long seed) {
    int width, height, channels;
    unsigned char *image =
        stbi_load(input_image_path, &width, &height, &channels, 0);
    if (!image) {
        puts("Unable to open image\n");
        return;
    }
    unsigned char *new_image =
        malloc(sizeof(*new_image) * width * height * channels);
    unshuffle_array_row(image, height, width * channels, seed, new_image);

    stbi_write_png(ouput_image_path, width, height, channels, new_image,
                   width * channels);
    stbi_image_free(image);
    free(new_image);
}

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("uses: ofuscate o|d input_image output_image number_key\n");
        return 0;
    }
    if (strlen(argv[1]) != 1)
        return 0;
    switch (argv[1][0]) {
    case 'o':
        ofuscate_image(argv[2], argv[3], atoi(argv[4]));
        break;
    case 'd':
        deofuscate_image(argv[2], argv[3], atoi(argv[4]));
        break;
    default:
        printf("only d and o is valid options\n");
        break;
    }
    return 0;
}
