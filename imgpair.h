#ifndef LIBIMGPAIR_H
#define LIBIMGPAIR_H

#include <stdint.h>
#include <stddef.h>

#define IMGP_SUCCESS 0
#define IMGP_FAILURE -1
/* An IMGP Value can be IMGP_SUCCESS/FAILURE to indicate status,
   or just a normal integrer */
typedef int    IMGPVALUE;

extern const char *imgp_failure_reason (void);

/* Write data_len byte from data to pixel lsb starting from bit_offset.
   Return the next offset if success. Otherwise IMGP_FAILURE */
IMGPVALUE imgp_write_lsb (uint8_t *pixels, int w, int h, int ch,
    size_t bit_offset, const uint8_t *data, size_t data_len);

/* Read decoded pixels starting from bit_offset and store it into data_out.
   Return the next offset if success. Otherwise IMGP_FAILURE */
IMGPVALUE imgp_read_lsb (uint8_t *pixels, int w, int h, int ch,
    size_t bit_offset, uint8_t *data_out, size_t data_len);

/* Decode pixels from offset and store data into fd using write () syscall.
   write () uses chunk buffer to save memory. */
IMGPVALUE imgp_decode_pixels (int fd, uint8_t *pixels, int w, int h, int ch,
    size_t offset, size_t data_len);

/* Write file content (pathname) into pixels starting from bitoffset.
   Return the next offset if success. Otherwise IMGP_FAILURE */
IMGPVALUE imgp_write_file (const char *pathname, uint8_t *pixels, int w,
    int h, int ch, size_t offset);

#endif /*LIBIMGPAIR_H */
