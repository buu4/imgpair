#include "imgpair.h"
#include "mapfile.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>

static const char *_imgp_error_reason = NULL;

static IMGPSTATUS imgp_errpuc (const char *reason)
{
  _imgp_error_reason = reason;
  return IMGP_FAILURE;
}

const char *imgp_failure_reason (void)
{
  return _imgp_error_reason;
}

/* write data_len byte from data to pixel lsb starting from bit_offset */
IMGPSTATUS imgp_lsb_write (uint8_t *pixels, size_t pixel_size, size_t bit_offset,
    const uint8_t *data, size_t data_len)
{
  register size_t i;
  register int b;

  for (i = 0; i < data_len; i++) {
    for (b = 7; b >= 0; b--) {
      register int bp = bit_offset + (i * 8) + (7 - b);
      if (bp >= pixel_size)
	return imgp_errpuc ("write overflow");
      pixels[bp] = (pixels[bp] & 0xfe) | (data[i] >> b & 1u);
    }
  }

  return IMGP_SUCCESS;
}

/* read steganed pixels starting from bit_offset and store it into data_out */
IMGPSTATUS imgp_lsb_read (uint8_t *pixels, size_t pixel_size, size_t bit_offset,
    uint8_t *data_out, size_t data_len)
{
  register size_t i;
  register int b;

  for (i = 0; i < data_len; i++) {
    for (b = 7; b >= 0; b--) {
      register int bp = bit_offset + (i * 8) + (7 - b);
      if (bp >= pixel_size)
	return imgp_errpuc ("read overflow");
      data_out[i] |= ((pixels[bp] & 1u) << b);

    }
  }

  return IMGP_SUCCESS;
}

IMGPSTATUS imgp_write_file (const char *pathname, uint8_t *pixels, int w,
    int h, int ch, size_t offset)
{
  struct mapfile *map;

  map = mapfile_open (pathname, MAP_PRIVATE, O_RDONLY, PROT_READ, 0);

  if (!map) {
    imgp_errpuc (strerror (errno));
    goto out;
  }

  if (imgp_lsb_write (pixels, w * h * ch, offset, (const uint8_t *)map->ptr,
	    map->sb.st_size) == IMGP_FAILURE) {
    imgp_errpuc (imgp_failure_reason ());
    goto out;
  }

  mapfile_close (map);
  return IMGP_SUCCESS;

out:
  if (map)
    mapfile_close (map);
  return IMGP_FAILURE;
}
