#ifndef LIBIMGPAIR_H
#define LIBIMGPAIR_H

#include <stdint.h>
#include <stddef.h>

#define PNGPAIL_SUCCESS 0
#define PNGPAIL_FAILURE -1
typedef int    PNGPAILSTATUS;

extern const char *imgp_failure_reason (void);

PNGPAILSTATUS imgp_lsb_write (uint8_t *pixels, size_t pixel_size, size_t bit_offset,
    const uint8_t *data, size_t data_len);

PNGPAILSTATUS imgp_lsb_read (uint8_t *pixels, size_t pixel_size, size_t bit_offset,
    uint8_t *data_out, size_t data_len);

PNGPAILSTATUS imgp_write_file (const char *pathname, uint8_t *pixels, int w,
    int h, int ch, size_t offset);

#endif /*LIBIMGPAIR_H */
