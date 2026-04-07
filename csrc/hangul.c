#include "hangul.h"

#include <stdlib.h>
#include <stdint.h>

static int utf8_decode_one(const unsigned char *s, uint32_t *out_cp, size_t *out_len) {
    if (!s || !*s) return 0;

    if ((s[0] & 0x80) == 0x00) {
        *out_cp = s[0];
        *out_len = 1;
        return 1;
    }

    if ((s[0] & 0xE0) == 0xC0) {
        if ((s[1] & 0xC0) != 0x80) return 0;
        *out_cp = ((uint32_t)(s[0] & 0x1F) << 6) |
                  ((uint32_t)(s[1] & 0x3F));
        *out_len = 2;
        return 1;
    }

    if ((s[0] & 0xF0) == 0xE0) {
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return 0;
        *out_cp = ((uint32_t)(s[0] & 0x0F) << 12) |
                  ((uint32_t)(s[1] & 0x3F) << 6) |
                  ((uint32_t)(s[2] & 0x3F));
        *out_len = 3;
        return 1;
    }

    if ((s[0] & 0xF8) == 0xF0) {
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) {
            return 0;
        }
        *out_cp = ((uint32_t)(s[0] & 0x07) << 18) |
                  ((uint32_t)(s[1] & 0x3F) << 12) |
                  ((uint32_t)(s[2] & 0x3F) << 6) |
                  ((uint32_t)(s[3] & 0x3F));
        *out_len = 4;
        return 1;
    }

    return 0;
}

static int is_hangul_syllable(uint32_t cp) {
    return (cp >= 0xAC00 && cp <= 0xD7A3);
}

static int compatibility_consonant_to_cho(uint32_t cp, uint8_t *out) {
    switch (cp) {
        case 0x3131: *out = 0x01; return 1;
        case 0x3132: *out = 0x02; return 1;
        case 0x3134: *out = 0x03; return 1;
        case 0x3137: *out = 0x04; return 1;
        case 0x3138: *out = 0x05; return 1;
        case 0x3139: *out = 0x06; return 1;
        case 0x3141: *out = 0x07; return 1;
        case 0x3142: *out = 0x08; return 1;
        case 0x3143: *out = 0x09; return 1;
        case 0x3145: *out = 0x0A; return 1;
        case 0x3146: *out = 0x0B; return 1;
        case 0x3147: *out = 0x0C; return 1;
        case 0x3148: *out = 0x0D; return 1;
        case 0x3149: *out = 0x0E; return 1;
        case 0x314A: *out = 0x0F; return 1;
        case 0x314B: *out = 0x10; return 1;
        case 0x314C: *out = 0x11; return 1;
        case 0x314D: *out = 0x12; return 1;
        case 0x314E: *out = 0x13; return 1;
        default: return 0;
    }
}

static int compatibility_vowel_to_jung(uint32_t cp, uint8_t *out) {
    switch (cp) {
        case 0x314F: *out = 0x01; return 1;
        case 0x3150: *out = 0x02; return 1;
        case 0x3151: *out = 0x03; return 1;
        case 0x3152: *out = 0x04; return 1;
        case 0x3153: *out = 0x05; return 1;
        case 0x3154: *out = 0x06; return 1;
        case 0x3155: *out = 0x07; return 1;
        case 0x3156: *out = 0x08; return 1;
        case 0x3157: *out = 0x09; return 1;
        case 0x3158: *out = 0x0A; return 1;
        case 0x3159: *out = 0x0B; return 1;
        case 0x315A: *out = 0x0C; return 1;
        case 0x315B: *out = 0x0D; return 1;
        case 0x315C: *out = 0x0E; return 1;
        case 0x315D: *out = 0x0F; return 1;
        case 0x315E: *out = 0x10; return 1;
        case 0x315F: *out = 0x11; return 1;
        case 0x3160: *out = 0x12; return 1;
        case 0x3161: *out = 0x13; return 1;
        case 0x3162: *out = 0x14; return 1;
        case 0x3163: *out = 0x15; return 1;
        default: return 0;
    }
}

static int encode_one(uint32_t cp, uint8_t out[3]) {
    uint8_t code;

    if (cp == 0x20) {
        out[0] = 0x00;
        out[1] = 0x00;
        out[2] = 0x00;
        return 1;
    }

    if (compatibility_consonant_to_cho(cp, &code)) {
        out[0] = code;
        out[1] = 0x00;
        out[2] = 0x00;
        return 1;
    }

    if (compatibility_vowel_to_jung(cp, &code)) {
        out[0] = 0x00;
        out[1] = code;
        out[2] = 0x00;
        return 1;
    }

    if (is_hangul_syllable(cp)) {
        uint32_t sindex = cp - 0xAC00;
        out[0] = (uint8_t)(sindex / (21 * 28)) + 1;
        out[1] = (uint8_t)((sindex % (21 * 28)) / 28) + 1;
        out[2] = (uint8_t)(sindex % 28);
        return 1;
    }

    return 0;
}

HangulResult h(const char *s) {
    HangulResult r;
    r.ok = 0;
    r.char_count = 0;
    r.data = NULL;

    if (!s) return r;

    {
        const unsigned char *p = (const unsigned char *)s;
        size_t count = 0;

        while (*p) {
            uint32_t cp = 0;
            size_t len = 0;
            uint8_t tmp[3];

            if (!utf8_decode_one(p, &cp, &len)) {
                return r;
            }

            if (!encode_one(cp, tmp)) {
                return r;
            }

            count++;
            p += len;
        }

        if (count == 0) {
            r.ok = 1;
            return r;
        }

        r.data = (uint8_t *)malloc(count * 3);
        if (!r.data) {
            return r;
        }

        p = (const unsigned char *)s;
        {
            size_t i = 0;
            while (*p) {
                uint32_t cp = 0;
                size_t len = 0;
                uint8_t tmp[3];

                if (!utf8_decode_one(p, &cp, &len)) {
                    free(r.data);
                    r.data = NULL;
                    return r;
                }

                if (!encode_one(cp, tmp)) {
                    free(r.data);
                    r.data = NULL;
                    return r;
                }

                r.data[i * 3 + 0] = tmp[0];
                r.data[i * 3 + 1] = tmp[1];
                r.data[i * 3 + 2] = tmp[2];

                i++;
                p += len;
            }

            r.ok = 1;
            r.char_count = count;
            return r;
        }
    }
}

void free_hangul_result(HangulResult *r) {
    if (!r) return;
    free(r->data);
    r->data = NULL;
    r->ok = 0;
    r->char_count = 0;
}