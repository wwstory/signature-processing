#include "cvt_buf_bmp.h"
#include <memory.h>
#include <stdio.h>

// ref: https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
// ref: https://www.cnblogs.com/wainiwann/p/7086844.html
const int depth_per_pixel = 1;
const int file_header_size = 14;
const int info_header_size = 40;
const int color_palette_size = 8;
static unsigned char color_palette_header[] = {0, 0, 0, 0, 0xff, 0xff, 0xff, 0};

size_t generate_bitmap_image(unsigned char *data_out, size_t len_data_out, unsigned char *image, int width, int height,
                             int num_line_byte) {
    unsigned char padding[3] = {0, 0, 0};
    int padding_size = (4 - num_line_byte % 4) % 4; // must %4 at last

    int file_size = file_header_size + info_header_size + color_palette_size + (num_line_byte + padding_size) * height;
    if (len_data_out < file_size || data_out == NULL) {
#ifdef DEBUG
    printf("%s %d: size of dest data: %d < %d\n", __FILE__, __LINE__, len_data_out, file_size);
#endif // !DEBUG
        return file_size;
    }

    unsigned char *file_header = create_bitmap_file_header(height, width, num_line_byte, padding_size);
    unsigned char *info_header = create_bitmap_info_header(height, width);

    size_t offset = 0;
    memcpy(data_out, file_header, file_header_size);
    offset += file_header_size;
    memcpy(data_out + offset, info_header, info_header_size);
    offset += info_header_size;
    memcpy(data_out + offset, color_palette_header, color_palette_size);
    offset += color_palette_size;
#ifdef DEBUG
    FILE *fp_image = fopen("out_1bit.bmp", "wb");
    fwrite(file_header, 1, file_header_size, fp_image);
    fwrite(info_header, 1, info_header_size, fp_image);
    fwrite(color_palette_header, 1, color_palette_size, fp_image);
#endif // !DEBUG

    int i;
    for (i = 0; i < height; i++) {
        memcpy(data_out + offset, image + (i * num_line_byte), num_line_byte);
        offset += num_line_byte;
        memcpy(data_out + offset, padding, padding_size);
        offset += padding_size;
#ifdef DEBUG
        fwrite(image + (i * num_line_byte), 1, num_line_byte, fp_image);
        fwrite(padding, 1, padding_size, fp_image);
#endif // !DEBUG
    }

#ifdef DEBUG
    printf("--------------\n");
    int x = 0, y = 0;
    for (y = 0; y < file_size / 16; y++) {
        for (x = 0; x < 16; x++) {
            printf("%.2X ", data_out[y * 16 + x]);
        }
        printf("\n");
    }
    for (x = file_size / 16 * 16; x < file_size; x++) {
        printf("%.2X ", data_out[x]);
    }
    printf("\n");
    printf("--------------\n");
#endif // !DEBUG

#ifdef DEBUG
    fclose(fp_image);
#endif // !DEBUG
    return 0;
}

unsigned char *create_bitmap_file_header(int height, int width, int num_line_byte, int padding_size) {
    int file_size = file_header_size + info_header_size + color_palette_size + (num_line_byte + padding_size) * height;

    static unsigned char fileHeader[] = {
        0, 0,       /// signature
        0, 0, 0, 0, /// image file size in bytes
        0, 0, 0, 0, /// reserved
        0, 0, 0, 0, /// start of pixel array
    };

    fileHeader[0] = (unsigned char)('B');
    fileHeader[1] = (unsigned char)('M');
    fileHeader[2] = (unsigned char)(file_size);
    fileHeader[3] = (unsigned char)(file_size >> 8);
    fileHeader[4] = (unsigned char)(file_size >> 16);
    fileHeader[5] = (unsigned char)(file_size >> 24);
    fileHeader[10] = (unsigned char)(file_header_size + info_header_size + color_palette_size);

    return fileHeader;
}

unsigned char *create_bitmap_info_header(int height, int width) {
    static unsigned char info_header[] = {
        0, 0, 0, 0, /// header size
        0, 0, 0, 0, /// image width
        0, 0, 0, 0, /// image height
        0, 0,       /// number of color planes
        0, 0,       /// bits per pixel
        0, 0, 0, 0, /// compression
        0, 0, 0, 0, /// image size
        0, 0, 0, 0, /// horizontal resolution
        0, 0, 0, 0, /// vertical resolution
        0, 0, 0, 0, /// colors in color table
        0, 0, 0, 0, /// important color count
    };

    info_header[0] = (unsigned char)(info_header_size);
    info_header[4] = (unsigned char)(width);
    info_header[5] = (unsigned char)(width >> 8);
    info_header[6] = (unsigned char)(width >> 16);
    info_header[7] = (unsigned char)(width >> 24);
    info_header[8] = (unsigned char)(height);
    info_header[9] = (unsigned char)(height >> 8);
    info_header[10] = (unsigned char)(height >> 16);
    info_header[11] = (unsigned char)(height >> 24);
    info_header[12] = (unsigned char)(1);
    info_header[14] = (unsigned char)(depth_per_pixel);
    // info_header[20] = (unsigned char)(?);

    return info_header;
}

size_t cvt_8bit_to_1bit(unsigned char *data, size_t len_data, size_t *line_byte_num, const unsigned char *img,
                        unsigned int width, unsigned int height) {
    *line_byte_num = width / 8 + (width % 8 + 7) / 8;
    size_t line_size = *line_byte_num * 8;
    size_t bit_size = line_size * height;
    if (len_data < bit_size || data == NULL) {

#ifdef DEBUG
        printf("%s %d: size of dest data: %d < %d\n", __FILE__, __LINE__, len_data, bit_size);
#endif // !DEBUG
        return bit_size;
    }

    unsigned char *p = data;
    int offset, v;
    unsigned char temp;
    int row = 0, col = 0, j = 0;
    for (row = height - 1; row >= 0; row--) {
        p[*line_byte_num - 1] = 0;
        for (col = 0; col < width; col++) {
            offset = col % 8;
            v = img[row * width + col];
            temp = 1;
            temp = temp << (8 - offset - 1);
            if (v == 255) {
#ifdef DEBUG
                printf("*");
#endif // !DEBUG
                *(p + col / 8) |= temp;
            } else {
#ifdef DEBUG
                printf(".");
#endif // !DEBUG
                temp = ~temp;
                *(p + col / 8) &= temp;
            }
        }
        p = p + *line_byte_num;
#ifdef DEBUG
        printf("\n");
#endif // !DEBUG
    }
    return 0;
}
