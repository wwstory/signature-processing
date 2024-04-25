#include <stdint.h>

#ifndef _BASE64_H
#ifdef __cplusplus
namespace base64
{
extern "C" {
#endif

int base64_encode(
    unsigned char *encoded_data,
    const unsigned char *data,
    const unsigned int size_in);

int base64_decode(
    unsigned char *decoded_data,
    const char *data,
    const unsigned int size_in);

int cal_encode_size(const int size_in);
int cal_decode_size(const int size_in);

#ifdef __cplusplus
}
} // namespace base64
#endif
#endif // !_BASE64_H