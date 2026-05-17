#include "uart_debug.h"
#include "main.h"
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

extern UART_HandleTypeDef huart1;

#define DEBUG_BUFFER_SIZE 128

// Forward declarations
static void uint_to_str(char *buf, uint32_t val, uint8_t width, uint8_t pad_zero);
static void float_to_str(char *buf, float val, uint8_t frac_digits);

static void uart_send_str(const char *str, uint16_t len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str, len, 100);
}

void uprint(const char *fmt, ...)
{
    char buffer[DEBUG_BUFFER_SIZE];
    char out[DEBUG_BUFFER_SIZE];
    char *dst = out;
    uint16_t out_len = 0;
    va_list args;

    va_start(args, fmt);
    const char *p = fmt;

    while (*p && out_len < DEBUG_BUFFER_SIZE - 1)
    {
        if (*p != '%')
        {
            *dst++ = *p++;
            out_len++;
            continue;
        }

        p++;
        if (*p == '%')  // escaped percent
        {
            *dst++ = *p++;
            out_len++;
            continue;
        }

        // Parse width / precision
        uint8_t width = 0;
        uint8_t pad_zero = 0;
        if (*p == '0')
        {
            pad_zero = 1;
            p++;
        }
        while (*p >= '0' && *p <= '9')
        {
            width = width * 10 + (*p - '0');
            p++;
        }

        uint8_t frac_digits = 2;  // default for %f
        if (*p == '.')
        {
            p++;
            frac_digits = 0;
            while (*p >= '0' && *p <= '9')
            {
                frac_digits = frac_digits * 10 + (*p - '0');
                p++;
            }
        }

        char token[DEBUG_BUFFER_SIZE];
        token[0] = '\0';

        switch (*p)
        {
        case 'u':
        {
            uint32_t v = va_arg(args, uint32_t);
            uint_to_str(token, v, width, pad_zero);
            break;
        }
        case 'd':
        case 'i':
        {
            int32_t v = va_arg(args, int32_t);
            if (v < 0)
            {
                token[0] = '-';
                uint_to_str(token + 1, (uint32_t)(-v), width ? width - 1 : 0, pad_zero);
            }
            else
            {
                uint_to_str(token, (uint32_t)v, width, pad_zero);
            }
            break;
        }
        case 'f':
        {
            double v = va_arg(args, double);
            float_to_str(token, (float)v, frac_digits);
            break;
        }
        case 'c':
        {
            char v = (char)va_arg(args, int);
            token[0] = v;
            token[1] = '\0';
            break;
        }
        case 's':
        {
            const char *v = va_arg(args, const char *);
            strncpy(token, v ? v : "(null)", DEBUG_BUFFER_SIZE - 1);
            break;
        }
        case 'l':
        {
            p++;
            if (*p == 'u')
            {
                uint64_t v = va_arg(args, uint64_t);
                uint_to_str(token, (uint32_t)v, width, pad_zero);
            }
            else if (*p == 'd')
            {
                int64_t v = va_arg(args, int64_t);
                if (v < 0)
                {
                    token[0] = '-';
                    uint_to_str(token + 1, (uint32_t)(-v), width ? width - 1 : 0, pad_zero);
                }
                else
                {
                    uint_to_str(token, (uint32_t)v, width, pad_zero);
                }
            }
            break;
        }
        case 'x':
        {
            uint32_t v = va_arg(args, uint32_t);
            uint_to_str(token, v, width, pad_zero);
            // Lowercase hex without prefix
            char *t = token;
            while (*t) { *t = (*t >= 'A' && *t <= 'Z') ? (*t + 32) : *t; t++; }
            break;
        }
        case 'p':
        {
            uint32_t v = va_arg(args, uint32_t);
            token[0] = '0';
            token[1] = 'x';
            uint_to_str(token + 2, v, width, pad_zero);
            break;
        }
        default:
            break;
        }

        uint8_t tlen = (uint8_t)strlen(token);
        if (out_len + tlen < DEBUG_BUFFER_SIZE - 1)
        {
            memcpy(dst, token, tlen);
            dst += tlen;
            out_len += tlen;
        }

        p++;
    }
    va_end(args);

    *dst = '\0';
    uart_send_str(out, out_len);
}

void uprintln(const char *fmt, ...)
{
    char buffer[DEBUG_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    const char *p = fmt;
    char *dst = buffer;
    uint16_t len = 0;

    while (*p && len < DEBUG_BUFFER_SIZE - 1)
    {
        if (*p != '%')
        {
            *dst++ = *p++;
            len++;
            continue;
        }

        p++;
        if (*p == '%')
        {
            *dst++ = *p++;
            len++;
            continue;
        }

        uint8_t width = 0;
        uint8_t pad_zero = 0;
        if (*p == '0')
        {
            pad_zero = 1;
            p++;
        }
        while (*p >= '0' && *p <= '9')
        {
            width = width * 10 + (*p - '0');
            p++;
        }

        uint8_t frac_digits = 2;
        if (*p == '.')
        {
            p++;
            frac_digits = 0;
            while (*p >= '0' && *p <= '9')
            {
                frac_digits = frac_digits * 10 + (*p - '0');
                p++;
            }
        }

        char token[DEBUG_BUFFER_SIZE];
        token[0] = '\0';

        switch (*p)
        {
        case 'u':
        {
            uint32_t v = va_arg(args, uint32_t);
            uint_to_str(token, v, width, pad_zero);
            break;
        }
        case 'd':
        case 'i':
        {
            int32_t v = va_arg(args, int32_t);
            if (v < 0)
            {
                token[0] = '-';
                uint_to_str(token + 1, (uint32_t)(-v), width ? width - 1 : 0, pad_zero);
            }
            else
            {
                uint_to_str(token, (uint32_t)v, width, pad_zero);
            }
            break;
        }
        case 'f':
        {
            double v = va_arg(args, double);
            float_to_str(token, (float)v, frac_digits);
            break;
        }
        case 'c':
        {
            char v = (char)va_arg(args, int);
            token[0] = v;
            token[1] = '\0';
            break;
        }
        case 's':
        {
            const char *v = va_arg(args, const char *);
            strncpy(token, v ? v : "(null)", DEBUG_BUFFER_SIZE - 1);
            break;
        }
        case 'l':
        {
            p++;
            if (*p == 'u')
            {
                uint64_t v = va_arg(args, uint64_t);
                uint_to_str(token, (uint32_t)v, width, pad_zero);
            }
            else if (*p == 'd')
            {
                int64_t v = va_arg(args, int64_t);
                if (v < 0)
                {
                    token[0] = '-';
                    uint_to_str(token + 1, (uint32_t)(-v), width ? width - 1 : 0, pad_zero);
                }
                else
                {
                    uint_to_str(token, (uint32_t)v, width, pad_zero);
                }
            }
            break;
        }
        case 'x':
        {
            uint32_t v = va_arg(args, uint32_t);
            uint_to_str(token, v, width, pad_zero);
            char *t = token;
            while (*t) { *t = (*t >= 'A' && *t <= 'Z') ? (*t + 32) : *t; t++; }
            break;
        }
        case 'p':
        {
            uint32_t v = va_arg(args, uint32_t);
            token[0] = '0';
            token[1] = 'x';
            uint_to_str(token + 2, v, width, pad_zero);
            break;
        }
        default:
            break;
        }

        uint8_t tlen = (uint8_t)strlen(token);
        if (len + tlen < DEBUG_BUFFER_SIZE - 1)
        {
            memcpy(dst, token, tlen);
            dst += tlen;
            len += tlen;
        }

        p++;
    }
    va_end(args);

    // Append \r\n
    if (len < DEBUG_BUFFER_SIZE - 2)
    {
        *dst++ = '\r';
        *dst++ = '\n';
        len += 2;
    }
    *dst = '\0';
    uart_send_str(buffer, len);
}

// --- Internal helpers ---

static void uint_to_str(char *buf, uint32_t val, uint8_t width, uint8_t pad_zero)
{
    char tmp[12];
    uint8_t i = 0;

    if (val == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (val > 0)
        {
            tmp[i++] = (char)('0' + (val % 10));
            val /= 10;
        }
    }

    uint8_t digits = i;
    uint8_t pad = (width > digits) ? (width - digits) : 0;

    uint8_t j = 0;
    if (!pad_zero)
    {
        while (j < pad)
        {
            buf[j++] = ' ';
        }
    }
    else
    {
        while (j < pad)
        {
            buf[j++] = '0';
        }
    }

    for (uint8_t k = 0; k < digits; k++)
    {
        buf[j++] = tmp[i - 1 - k];
    }
    buf[j] = '\0';
}

static void float_to_str(char *buf, float val, uint8_t frac_digits)
{
    if (frac_digits > 6) frac_digits = 6;

    int8_t sign = 0;
    if (val < 0)
    {
        sign = 1;
        val = -val;
    }

    uint32_t int_part = (uint32_t)val;
    float frac = val - (float)int_part;

    // Compute fractional digits
    char frac_buf[8];
    frac_buf[frac_digits] = '\0';
    uint32_t frac_pow = 1;
    for (uint8_t i = 0; i < frac_digits; i++) frac_pow *= 10;

    uint32_t frac_val = (uint32_t)(frac * (float)frac_pow + 0.5f);

    // Build fractional part string
    for (int8_t i = (int8_t)frac_digits - 1; i >= 0; i--)
    {
        frac_buf[i] = (char)('0' + (frac_val % 10));
        frac_val /= 10;
    }

    // Convert integer part
    char int_buf[12];
    uint8_t int_len = 0;
    if (int_part == 0)
    {
        int_buf[int_len++] = '0';
    }
    else
    {
        while (int_part > 0)
        {
            int_buf[int_len++] = (char)('0' + (int_part % 10));
            int_part /= 10;
        }
    }

    uint8_t out_pos = 0;
    if (sign)
    {
        buf[out_pos++] = '-';
    }

    for (int8_t i = int_len - 1; i >= 0; i--)
    {
        buf[out_pos++] = int_buf[i];
    }

    if (frac_digits > 0)
    {
        buf[out_pos++] = '.';
        for (uint8_t i = 0; i < frac_digits; i++)
        {
            buf[out_pos++] = frac_buf[i];
        }
    }

    buf[out_pos] = '\0';
}
