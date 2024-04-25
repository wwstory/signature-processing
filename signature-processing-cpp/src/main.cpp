#include "base64.h"
#include "signature_processing.hpp"
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, const char **argv) {
    if (argc <= 0) {
        printf("please input image path!");
        return -1;
    }
    std::string img_path = argv[1];
    std::cout << "---------- image path: " << img_path << " ----------" << std::endl;

    /* read image */
    std::ifstream file(img_path, std::ios::binary | std::ios::in);
    file.seekg(0, std::ios::end);
    int file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> img;
    img.resize(file_size);
    file.read((char *)img.data(), img.size());
    file.close();
    // fmt::print("{}", img);

    /* base64 */
    /* encode */
    std::vector<unsigned char> image_base64(base64::cal_encode_size(img.size()));
    const int size_out = base64::base64_encode(image_base64.data(), img.data(), img.size());
    std::string base64_str(image_base64.cbegin(), image_base64.cend());
    printf("image_base64 size: %d\n", size_out);
    // fmt::print("{}\n", base64_str);
    /* decode */
    std::vector<unsigned char> image(base64::cal_decode_size(base64_str.length()));
    const int size_out2 = base64::base64_decode(image.data(), base64_str.data(), base64_str.length());
    printf("image size: %d\n", size_out2);

    std::cout << "--- test lib start ---" << std::endl;
    unsigned char image_out[4096 * 1024]{}; // 4 mb
    long ret = process_sign(image_out, image.data(), image.size(), IMAGE_TYPE_BMP);
    std::cout << "--- test lib end ---" << std::endl;
    printf("%d\n", ret);
    if (ret != -1) {
        std::string base64_str2(image_out, image_out + ret);
        std::cout << base64_str2 << std::endl;

        std::vector<unsigned char> image_bmp(base64::cal_decode_size(ret));
        const int len_bmp = base64::base64_decode(image_bmp.data(), base64_str2.data(), base64_str2.length());
        printf("bmp image size: %d\n", len_bmp);
        size_t position = img_path.find_last_of("/\\");
        std::string filename = img_path.substr(position + 1, img_path.length() - position - 1);
        std::ofstream out_file(filename + ".bmp", std::ios::binary | std::ios::out);
        out_file.write((char *)image_bmp.data(), len_bmp);
        out_file.close();
    }

    return 0;
}
