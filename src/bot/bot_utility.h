#ifndef BOT_UTILITY_H
#define BOT_UTILITY_H

#include <string>

namespace bot_utility {

void replace(std::string& str, const std::string& from, const std::string& to);
void get_correct_encoded_url_str(std::string& str);

// Class copied from: https://github.com/tkislan/base64
class base64 {
    static const char* base64_alphabet;
    static void a3_to_a4(unsigned char* a4, unsigned char* a3);
    static void a4_to_a3(unsigned char* a3, unsigned char* a4);
    static unsigned char b64_lookup(unsigned char c);

public:
    static bool encode(const std::string& in, std::string* out);
    static bool encode(const char* input, size_t input_length, char* out, size_t out_length);
    static bool decode(const std::string &in, std::string *out);
    static bool decode(const char* input, size_t input_length, char* out, size_t out_length);
    static std::size_t decoded_length(const char* in, size_t in_length);
    static std::size_t decoded_length(const std::string& in);
    static std::size_t encoded_length(size_t length);
    static std::size_t encoded_length(const std::string& in);
    static void strip_padding(std::string* in);
};

}

#endif // BOT_UTILITY_H
