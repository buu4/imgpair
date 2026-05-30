#include "imgpair.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static const char *_imgpair_error_reason = NULL;

static PNGPAILSTATUS imgpair_errpuc (const char *reason)
{
  _imgpair_error_reason = reason;
  return PNGPAIL_FAILURE;
}

const char *imgpair_failure_reason (void)
{
  return _imgpair_error_reason;
}

/* write data_len byte from data to pixel lsb starting from bit_offset */
PNGPAILSTATUS imgpair_lsb_write (uint8_t *pixels, size_t pixel_size, size_t bit_offset,
    const uint8_t *data, size_t data_len)
{
  register size_t i;
  register int b;

  for (i = 0; i < data_len; i++) {
    for (b = 7; b >= 0; b--) {
      register int bp = bit_offset + (i * 8) + (7 - b);
      if (bp >= pixel_size)
	return imgpair_errpuc ("write overflow");
      pixels[bp] = (pixels[bp] & 0xfe) | (data[i] >> b & 1u);
    }
  }

  return PNGPAIL_SUCCESS;
}

/* read steganed pixels starting from bit_offset and store it into data_out */
PNGPAILSTATUS imgpair_lsb_read (uint8_t *pixels, size_t pixel_size, size_t bit_offset,
    uint8_t *data_out, size_t data_len)
{
  register size_t i;
  register int b;

  for (i = 0; i < data_len; i++) {
    for (b = 7; b >= 0; b--) {
      register int bp = bit_offset + (i * 8) + (7 - b);
      if (bp >= pixel_size)
	return imgpair_errpuc ("read overflow");
      data_out[i] |= ((pixels[bp] & 1u) << b);

    }
  }

  return PNGPAIL_SUCCESS;
}
