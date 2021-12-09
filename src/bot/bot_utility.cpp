#include "bot_utility.h"

namespace bot_utility {

void replace(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty()) return;
    std::size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void get_correct_encoded_url_str(std::string& str)
{
    bot_utility::replace(str, " ", "%20");
    bot_utility::replace(str, "\n", "%0A");
    bot_utility::replace(str, "+", "%2B");
}

const char* base64::base64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz"
                                      "0123456789+/";

bool base64::encode(const std::string& in, std::string* out) {
    int i = 0, j = 0;
    std::size_t enc_len = 0;
    unsigned char a3[3];
    unsigned char a4[4];

    out->resize(encoded_length(in));

    std::size_t input_len = in.size();
    std::string::const_iterator input = in.begin();

    while (input_len--) {
        a3[i++] = *(input++);
        if(i == 3) {
            a3_to_a4(a4, a3);
            for(i = 0; i < 4; i++) {
                (*out)[enc_len++] = base64_alphabet[a4[i]];
            }
            i = 0;
        }
    }

    if(i) {
        for (j = i; j < 3; j++) {
            a3[j] = '\0';
        }
        a3_to_a4(a4, a3);
        for(j = 0; j < i + 1; j++) {
            (*out)[enc_len++] = base64_alphabet[a4[j]];
        }
        while ((i++ < 3)) {
            (*out)[enc_len++] = '=';
        }
    }
    return (enc_len == out->size());
}

bool base64::encode(const char* input, std::size_t input_length, char* out, std::size_t out_length) {
    int i = 0, j = 0;
    char *out_begin = out;
    unsigned char a3[3];
    unsigned char a4[4];

    std::size_t encoded_length_ = encoded_length(input_length);

    if(out_length < encoded_length_) return false;

    while (input_length--) {
        a3[i++] = *input++;
        if(i == 3) {
            a3_to_a4(a4, a3);

            for(i = 0; i < 4; i++) {
                *out++ = base64_alphabet[a4[i]];
            }
            i = 0;
        }
    }

    if(i) {
        for (j = i; j < 3; j++) {
            a3[j] = '\0';
        }

        a3_to_a4(a4, a3);

        for(j = 0; j < i + 1; j++) {
            *out++ = base64_alphabet[a4[j]];
        }

        while ((i++ < 3)) {
            *out++ = '=';
        }
    }
    return (out == (out_begin + encoded_length_));
}

bool base64::decode(const std::string& in, std::string* out) {
    int i = 0, j = 0;
    std::size_t dec_len = 0;
    unsigned char a3[3];
    unsigned char a4[4];

    std::size_t input_len = in.size();
    std::string::const_iterator input = in.begin();

    out->resize(decoded_length(in));

    while (input_len--) {
        if(*input == '=') {
            break;
        }

        a4[i++] = *(input++);
        if(i == 4) {
            for(i = 0; i <4; i++) {
                a4[i] = b64_lookup(a4[i]);
            }
            a4_to_a3(a3,a4);

            for (i = 0; i < 3; i++) {
                (*out)[dec_len++] = a3[i];
            }
            i = 0;
        }
    }

    if(i) {
        for(j = i; j < 4; j++) {
            a4[j] = '\0';
        }

        for(j = 0; j < 4; j++) {
            a4[j] = b64_lookup(a4[j]);
        }

        a4_to_a3(a3,a4);

        for(j = 0; j < i - 1; j++) {
            (*out)[dec_len++] = a3[j];
        }
    }
    return (dec_len == out->size());
}

bool base64::decode(const char* input, std::size_t input_length, char* out, std::size_t out_length) {
    int i = 0, j = 0;
    char *out_begin = out;
    unsigned char a3[3];
    unsigned char a4[4];

    std::size_t decoded_length_ = decoded_length(input, input_length);

    if(out_length < decoded_length_) return false;

    while (input_length--) {
        if(*input == '=') {
            break;
        }

        a4[i++] = *(input++);
        if(i == 4) {
            for(i = 0; i < 4; i++) {
                a4[i] = b64_lookup(a4[i]);
            }

            a4_to_a3(a3,a4);

            for(i = 0; i < 3; i++) {
                *out++ = a3[i];
            }
            i = 0;
        }
    }

    if(i) {
        for (j = i; j < 4; j++) {
            a4[j] = '\0';
        }

        for (j = 0; j < 4; j++) {
            a4[j] = b64_lookup(a4[j]);
        }

        a4_to_a3(a3,a4);

        for(j = 0; j < i - 1; j++) {
            *out++ = a3[j];
        }
    }

    return (out == (out_begin + decoded_length_));
}

std::size_t base64::decoded_length(const char* in, std::size_t in_length) {
    int numEq = 0;

    const char *in_end = in + in_length;
    while(*--in_end == '=') ++numEq;

    return ((6 * in_length) / 8) - numEq;
}

std::size_t base64::decoded_length(const std::string& in) {
    int numEq = 0;
    std::size_t n = in.size();

    for(std::string::const_reverse_iterator it = in.rbegin(); *it == '='; ++it) {
      ++numEq;
    }

    return ((6 * n) / 8) - numEq;
}

std::size_t base64::encoded_length(std::size_t length) {
    return (length + 2 - ((length + 2) % 3)) / 3 * 4;
}

std::size_t base64::encoded_length(const std::string& in) {
    return encoded_length(in.length());
}

void base64::strip_padding(std::string *in) {
    while(!in->empty() && *(in->rbegin()) == '=') in->resize(in->size() - 1);
}

void base64::a3_to_a4(unsigned char* a4, unsigned char* a3) {
    a4[0] = (a3[0] & 0xfc) >> 2;
    a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
    a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
    a4[3] = (a3[2] & 0x3f);
}

void base64::a4_to_a3(unsigned char* a3, unsigned char* a4) {
    a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
    a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
    a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

unsigned char base64::b64_lookup(unsigned char c) {
    if(c >='A' && c <='Z') return c - 'A';
    if(c >='a' && c <='z') return c - 71;
    if(c >='0' && c <='9') return c + 4;
    if(c == '+') return 62;
    if(c == '/') return 63;
    return 255;
}

}
