#include "cvt_mat2bmp.hpp"
#include "cvt_buf_bmp.h"

size_t cvt_mat2bmp(unsigned char *data_bmp, size_t len_data_out, const cv::Mat img) {
    size_t line_byte_num{0};
    size_t ret = -1;
    size_t len_data = cvt_8bit_to_1bit(nullptr, 0, &line_byte_num, nullptr, img.cols, img.rows);
    size_t len_data_bmp = generate_bitmap_image(nullptr, 0, nullptr, img.cols, img.rows, line_byte_num);
    if (len_data_out < len_data_bmp || data_bmp == nullptr) {
#ifdef DEBUG
        printf("%s %d: size of dest data: %d < %d\n", __FILE__, __LINE__, len_data_out, len_data_bmp);
#endif // !DEBUG
        return len_data_bmp;
    }

    // unsigned char data[len_data]{};
    unsigned char data[len_data]; // todo init
    ret = cvt_8bit_to_1bit(data, len_data, &line_byte_num, (const unsigned char *)img.data, img.cols, img.rows);
    if(ret != 0) return 0;
    // unsigned char data_bmp[len_data_bmp]{};
    ret = generate_bitmap_image(data_bmp, len_data_bmp, data, img.cols, img.rows, line_byte_num);

    return ret;
}
