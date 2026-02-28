#include"yscanf3.h"
#ifndef YIN3_HPP
#define YIN3_HPP
namespace ystd {

class yin_t {
public:
    inline yin_t& operator>>(int &x) {
        yread_int_ok(&x);
        return *this;
    }
    inline yin_t& operator>>(unsigned &x) {
        yread_uint_ok(&x);
        return *this;
    }
    inline yin_t& operator>>(long long &x) {
        yread_ll_ok(&x);
        return *this;
    }
    inline yin_t& operator>>(unsigned long long &x) {
        yread_ull_ok(&x);
        return *this;
    }
    inline yin_t& operator>>(double &x) {
        yread_double_ok(&x);
        return *this;
    }
    inline yin_t& operator>>(char &x) {
        int c = yget();
        while (c != EOF && isspace(c))
            c = yget();
        x = (char)c;
        return *this;
    }
    inline yin_t& operator>>(char *s) {
        yread_str_ok(s);
        return *this;
    }
    inline bool getline(char *s, int maxlen) {
        return yread_line_ok(s, maxlen);
    }
    inline bool good() const {
        return !yeof;
    }

};

inline yin_t yin;

} // namespace ystd
#endif /* YIN3_HPP */
