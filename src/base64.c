#include "base64.h"

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char decoding_table[] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
                                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
                                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63, 
                                52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0, 
                                 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
                                15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0, 
                                 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
                                41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

int base64_encode(
    unsigned char *encoded_data,
    const unsigned char *data,
    const unsigned int size_in
) {
    if (size_in <= 0) return -1;

    int mod_table[] = {0, 2, 1};
    int size_out = 4 * ((size_in + 2) / 3);

    int i = 0, j = 0;
    for (i = 0, j = 0; i < size_in;) {
        uint32_t octet_a = i < size_in ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < size_in ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < size_in ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < mod_table[size_in % 3]; i++)
        encoded_data[size_out - 1 - i] = '=';

    return size_out;
}


int base64_decode(
    unsigned char *decoded_data,
    const char *data,
    const unsigned int size_in
) {
    if (size_in <= 0 || size_in % 4 != 0) return -1;

    int size_out = size_in / 4 * 3;
    if (data[size_in - 1] == '=') size_out--;
    if (data[size_in - 2] == '=') size_out--;

    int i = 0, j = 0;
    for (i = 0, j = 0; i < size_in;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
                            + (sextet_b << 2 * 6)
                            + (sextet_c << 1 * 6)
                            + (sextet_d << 0 * 6);

        if (j < size_out) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < size_out) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < size_out) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return size_out;
}


int cal_encode_size(const int size_in) {
    return 4 * ((size_in + 2) / 3);
}

int cal_decode_size(const int size_in) {
    if (size_in <= 0 || size_in % 4 != 0) return -1;
    return size_in / 4 * 3;
}