
#include <stdint.h>

#ifndef _CVT_BUF_BMP
#ifdef __cplusplus
extern "C" {
#endif
typedef unsigned long size_t;

size_t generate_bitmap_image(unsigned char *data_out, size_t len_data_out, unsigned char *image, int width,
                             int height, int num_line_byte);
unsigned char *create_bitmap_file_header(int height, int width, int pitch, int padding_size);
unsigned char *create_bitmap_info_header(int height, int width);
size_t cvt_8bit_to_1bit(unsigned char *data, size_t len_data, size_t *line_byte_num, const unsigned char *img,
                        unsigned int width, unsigned int height);

#ifdef __cplusplus
}
#endif
#endif // !_CVT_BUF_BMP