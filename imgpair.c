#include "imgpair.h"
#include "mapfile.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>

#define CHUNK_THRESHOLD    1024

static const char *_imgp_error_reason = NULL;

static IMGPVALUE imgp_errpuc (const char *reason)
{
  _imgp_error_reason = reason;
  return IMGP_FAILURE;
}

const char *imgp_failure_reason (void)
{
  return _imgp_error_reason;
}

IMGPVALUE imgp_write_lsb (uint8_t *pixels, int w, int h, int ch,
    size_t bit_offset, const uint8_t *data, size_t data_len)
{
  register size_t i;
  register int b;
  register size_t bp;
  size_t pixel_size = w * h * ch;

  for (i = 0; i < data_len; i++) {
    for (b = 7; b >= 0; b--) {
      bp = 0;
      bp = bit_offset + (i * 8) + (7 - b);
      if (bp >= pixel_size)
	return imgp_errpuc ("write overflow");
      pixels[bp] = (pixels[bp] & 0xfe) | (data[i] >> b & 1u);
    }
  }

  return bp;
}

IMGPVALUE imgp_read_lsb (uint8_t *pixels, int w, int h, int ch,
    size_t bit_offset, uint8_t *data_out, size_t data_len)
{
  register size_t i;
  register int b;
  register size_t bp;
  size_t pixel_size = w * h * ch;

  for (i = 0; i < data_len; i++) {
    for (b = 7; b >= 0; b--) {
      bp = 0;
      bp = bit_offset + (i * 8) + (7 - b);
      if (bp >= pixel_size)
	return imgp_errpuc ("read overflow");
      data_out[i] |= ((pixels[bp] & 1u) << b);
    }
  }

  return bp;
}

IMGPVALUE imgp_decode_pixels (int fd, uint8_t *pixels, int w, int h, int ch,
    size_t offset, size_t data_len)
{
  uint8_t *data;
  data = (uint8_t *) malloc (data_len);

  while (1) {
    register int chunk;
    memset (data, 0, data_len);
    chunk = (data_len - offset > CHUNK_THRESHOLD) ? CHUNK_THRESHOLD - offset
            : data_len - offset;
    if (chunk <= 0)
      break;
    if (imgp_read_lsb (pixels, w, h, ch, offset, data, chunk) == IMGP_FAILURE)
      return IMGP_FAILURE;

    write (fd, data, chunk);
    offset += chunk;
  }
  free (data);

  return IMGP_SUCCESS;
}

IMGPVALUE imgp_write_file (const char *pathname, uint8_t *pixels, int w,
    int h, int ch, size_t offset)
{
  struct mapfile *map;
  IMGPVALUE ret;

  map = mapfile_open (pathname, MAP_PRIVATE, O_RDONLY, PROT_READ, 0);

  if (!map) {
    imgp_errpuc (strerror (errno));
    goto out;
  }

  if ((ret = imgp_write_lsb (pixels, w, h, ch, offset, (const uint8_t *)map->ptr,
	    map->sb.st_size)) == IMGP_FAILURE) {
    goto out;
  }

  mapfile_close (map);
  return ret;

out:
  if (map)
    mapfile_close (map);
  return IMGP_FAILURE;
}
