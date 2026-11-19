#include "mem.hpp"
#include "types.hpp"

extern "C" {

void *memset(void *dst, int c, size_t n) {
    auto *d = static_cast<byte *>(dst);
    const byte cr = static_cast<byte>(c); // only low byte is used
    for (size_t i = 0; i < n; ++i) {
        d[i] = cr;
    }
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
    auto *d = static_cast<byte *>(dst);
    auto *s = static_cast<const byte *>(src);
    for (size_t i = 0; i < n; ++i) {
        d[i] = s[i];
    }
    return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
    // memcpy copies forwards, which is safe when dst is below src
    if (dst < src) {
        memcpy(dst, src, n);
        return dst;
    }

    auto *d = static_cast<byte *>(dst);
    auto *s = static_cast<const byte *>(src);
    for (size_t i = n; i-- > 0;) {
        d[i] = s[i];
    }
    return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
    auto *pa = static_cast<const byte *>(a);
    auto *pb = static_cast<const byte *>(b);
    for (size_t i = 0; i < n; ++i) {
        if (pa[i] != pb[i]) return pa[i] > pb[i] ? 1 : -1;
    }
    return 0;
}

}
