#include <stdint.h>
#include <stdlib.h>

#ifndef _SIGNATUREPROCESSING_H
#define _SIGNATUREPROCESSING_H
#ifdef __cplusplus
extern "C" {
#endif

#define IMAGE_TYPE_BMP 0
#define IMAGE_TYPE_JPG 1

/// @brief
/// @param image_out
/// @param image_base64
/// @return if return positive number, this is size of image out; if return negative, it is error code.
long process_sign(unsigned char *image_out, const unsigned char *image_base64, const long size_src, const int out_image_type);

#ifdef __cplusplus
}
#endif
#endif // !_SIGNATUREPROCESSING_H