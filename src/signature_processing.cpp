#include "signature_processing.hpp"
#include "base64.h"
#include "cvt_mat2bmp.hpp"
#include <cmath>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

cv::Mat cvt_bin_img(const cv::Mat img) {
    cv::Mat img_smoothed;
    int sigma = 3;
    int ksize = (sigma * 5) | 1;
    cv::GaussianBlur(img, img_smoothed, cv::Size(ksize, ksize), sigma, sigma);
    cv::Mat img_bin;
    cv::threshold(img_smoothed, img_bin, 100, 255, cv::THRESH_BINARY_INV);
    return img_bin;
}

cv::Rect find_rect_box(const cv::Mat img, int threshold_area = -1, const float rate_area = 0.0002f) {
    if (threshold_area == -1) {
        threshold_area = img.rows * img.cols * rate_area;
    }

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    std::vector<cv::Point> select_contour;
    double area{};
    for (int i = 0; i < contours.size(); i++) {
        area = cv::contourArea(contours[i]);
        if (area < threshold_area) {
            // TODO: clear dirty
            // cv::drawContours(img, contours[i], 0, 0, cv::FILLED);
        } else {
            select_contour.insert(select_contour.end(), contours[i].begin(), contours[i].end());
        }
    }
    cv::Rect box = cv::boundingRect(select_contour);
    return box;
}

cv::Mat crop_image(const cv::Mat img, const cv::Rect box) {
#ifdef DEBUG
    cv::rectangle(img, box, cv::Scalar(255, 0, 0));
    cv::namedWindow("crop", cv::WINDOW_AUTOSIZE);
    cv::imshow("crop", img);
    cv::waitKey(0);
#endif // DEBUG
    cv::Mat img_crop(img, box);
    return img_crop;
}

cv::Mat resize_image(const cv::Mat img,
                     const std::tuple<size_t, size_t> limit_height = std::tuple<size_t, size_t>{80, 800}) {
#ifdef DEBUG
    std::cout << img.cols << "," << img.rows << std::endl;
#endif // DEBUG

    size_t min_height = std::get<0>(limit_height);
    size_t max_height = std::get<1>(limit_height);
    if (min_height < img.rows && img.rows < max_height) {
        return img;
    } else if (img.rows < min_height) {
        return cv::Mat{};
    }

    cv::Size new_size{};
    new_size.height = max_height;
    new_size.width = img.cols * max_height / img.rows;

#ifdef DEBUG
    std::cout << __FILE__ << __LINE__ << " | "
              << "resize: " << img.cols << "," << img.rows << " -> " << new_size.width << "," << new_size.height
              << std::endl;
#endif // DEBUG

    cv::Mat img_resize{};
    cv::resize(img, img_resize, new_size);
    return img_resize;
}

cv::Mat resize_image_with_size(const cv::Mat img,
                               const std::tuple<size_t, size_t> limit_height = std::tuple<size_t, size_t>{80, 800},
                               const unsigned int limit_size = 80 * 1024) {
    int h = img.rows;
    int w = img.cols;

    size_t min_height = std::get<0>(limit_height);
    size_t max_height = std::get<1>(limit_height);

    if (h * w < limit_size) {
        return img;
    }

    const float R = 1.0 * w / h;
    int x = sqrt(limit_size / R);
    if (x < min_height) {
        return cv::Mat{};
    }
    x = x < max_height ? x : max_height;

    cv::Size new_size{};
    new_size.height = x;
    new_size.width = R * x;

#ifdef DEBUG
    std::cout << __FILE__ << __LINE__ << " | "
              << "resize: " << img.cols << "," << img.rows << " -> " << new_size.width << "," << new_size.height
              << std::endl;
#endif // DEBUG

    cv::Mat img_resize{};
    cv::resize(img, img_resize, new_size);
    return img_resize;
}

std::vector<uchar> encode_jpg(const cv::Mat img) {
    std::vector<uchar> buf;
    const int COUNT_TRY = 3;
    const int SIZE_LIMIT = 80 * 1024;
    int count = 0;
    do {
        std::vector<int> param;
        param.push_back(cv::IMWRITE_JPEG_QUALITY);
        param.push_back(100 - count * 20);
        cv::imencode(".jpg", img, buf, param);
#ifdef DEBUG
        std::cout << __FILE__ << __LINE__ << " | "
                  << "out jpg size: " << buf.size() << std::endl;
#endif // DEBUG
    } while (buf.size() >= SIZE_LIMIT && ++count < COUNT_TRY);

    return buf.size() < SIZE_LIMIT ? buf : std::vector<uchar>{};
}

std::vector<uchar> encode_bmp(const cv::Mat img) {
    std::vector<uchar> buf;
    const int COUNT_TRY = 3;
    const int SIZE_LIMIT = 80 * 1024;

    std::vector<int> param;
    cv::imencode(".bmp", img, buf, param);
#ifdef DEBUG
    std::cout << __FILE__ << " " << __LINE__ << " : "
              << "out bmp size: " << buf.size() << std::endl;
#endif // DEBUG

    return buf.size() < SIZE_LIMIT ? buf : std::vector<uchar>{};
}

long process_sign(unsigned char *image_out, const unsigned char *image_base64, const long size_src,
                  const int out_image_type) {
    cv::Mat img = cv::imdecode(cv::Mat(1, size_src, CV_8UC1, (char *)image_base64), cv::IMREAD_GRAYSCALE);
    if (!img.data) {
        printf("No image data. \n");
        return -1;
    }
    cv::Mat img_bin = cvt_bin_img(img);
    cv::Rect box = find_rect_box(img_bin);
    cv::Mat img_crop = crop_image(img_bin, box);
    cv::Mat img_inv;
    cv::bitwise_not(img_crop, img_inv);

    cv::Mat img_out;
    if (out_image_type == IMAGE_TYPE_BMP)
        img_out = resize_image_with_size(img_inv, std::tuple<size_t, size_t>{80, 800}, (80 - 5) * 1024);
    else
        img_out = resize_image(img_inv);
    if (img_out.empty()) {
        printf("Height of image less 80px. \n");
        return -1;
    }

#ifdef DEBUG
    cv::namedWindow("Display", cv::WINDOW_AUTOSIZE);
    cv::imwrite("out.jpg", img_out);
    // cv::imwrite("out.bmp", img_out);
    cv::imshow("Display", img_out);
    cv::waitKey(0);
#endif // DEBUG

    std::vector<uchar> buf;
    if (out_image_type == IMAGE_TYPE_JPG)
        buf = encode_jpg(img_out);
    else {
        // buf = encode_bmp(img_out);   // it's gray image.
        
        size_t len_data_bmp = cvt_mat2bmp(nullptr, 0, img_out);
        // unsigned char data_bmp[len_data_bmp]{};
        unsigned char data_bmp[len_data_bmp]; // todo init
        if (cvt_mat2bmp(data_bmp, len_data_bmp, img_out) == 0) {
            buf = std::vector<uchar>(data_bmp, data_bmp + len_data_bmp); // copy to vector
        }
    }
    if (buf.size() == 0) {
        return -1;
    }
    long size_out = base64::base64_encode(image_out, buf.data(), buf.size());

    if (size_out <= 0) {
        return -1;
    }
    return size_out;
}
