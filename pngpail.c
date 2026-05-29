#include "mapfile.h"
#include "error.h"

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/* This current running program name */
char *progname = "pngpail";

/* PNG file to be modified */
static char *pngfile = NULL;

/* File to be stored to pngfile */
static char *filename = NULL;
static char *output = NULL;

/* write data_len byte from data to pixel lsb starting from bit_offset */
PNGPAILSTATUS pngpail_lsb_write (uint8_t *pixels, size_t pixel_size, size_t bit_offset,
    const uint8_t *data, size_t data_len)
{
  register size_t i;
  register int b;

  for (i = 0; i < data_len; i++) {
    for (b = 7; b >= 0; b--) {
      register int bp = bit_offset + (i * 8) + (7 - b);
      if (bp >= pixel_size)
	return pngpail_errpuc ("write overflow");
      pixels[bp] = (pixels[bp] & 0xfe) | (data[i] >> b & 1u);
    }
  }

  return PNGPAIL_SUCCESS;
}

/* read steganed pixels starting from bit_offset and store it into data_out */
PNGPAILSTATUS pngpail_lsb_read (uint8_t *pixels, size_t pixel_size, size_t bit_offset,
    uint8_t *data_out, size_t data_len)
{
  register size_t i;
  register int b;

  for (i = 0; i < data_len; i++) {
    for (b = 7; b >= 0; b--) {
      register int bp = bit_offset + (i * 8) + (7 - b);
      if (bp >= pixel_size)
	return pngpail_errpuc ("read overflow");
      data_out[i] |= ((pixels[bp] & 1u) << b);

    }
  }

  return PNGPAIL_SUCCESS;
}

const char _help[] =
"Usage: %s [-options] [file] [-png png]\n"
"  Pair file content into image file, If no file provided read from image.\n"
"  -png png   PNG file\n\n";

static void parse_args (int argc, char **argv)
{
  int i;

  progname = argv[0];

  if (argc == 1)
    goto out;

  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {
      if (strcmp (argv[i], "-png") == 0) {
	if (i + 2 > argc)
	  break;
	pngfile = argv[++i];
	continue;
      }
    }

    if (!filename)  /* Allow only 1 file */
      filename = argv[i];
  }

  if (pngfile == NULL) {
    error ("no png file provided");
    goto out;
  }

  output = pngfile;

  return;

out:
  fprintf (stderr, _help, progname);
  exit (1);
}

static void merge_file_to_pixels (const char *filename, uint8_t *pixels,
    int w, int h, int ch)
{
  struct mapfile *map;
  int err;

  map = mapfile_open (filename, MAP_PRIVATE, O_RDONLY, PROT_READ, 0);

  if (!map) {
    error ("%s: %s", filename, strerror (errno));
    goto out;
  }

  err = pngpail_lsb_write (pixels, w * h * ch, 0, (const uint8_t *)map->ptr, map->sb.st_size);

  if (err == PNGPAIL_FAILURE) {
    error ("%s: %s", filename, pngpail_failure_reason ());
    goto out;
  }

  mapfile_close (map);
  return;

out:
  mapfile_close (map);
  stbi_image_free (pixels);
  exit (1);
}

int main (int argc, char **argv)
{
  int w, h, ch;
  uint8_t *pixels;

  parse_args (argc, argv);

  pixels = stbi_load (pngfile, &w, &h, &ch, 0);

  if (pixels == NULL) {
    error ("can't load image file: %s", stbi_failure_reason ());
    return 1;
  }

  if (filename == NULL)  /* XXX - Should make pngpail_lsb_read */
    goto cleanup;

  merge_file_to_pixels (filename, pixels, w, h, ch);

  if (stbi_write_png (output, w, h, ch, pixels, w * ch) < 0)
    error ("can't write image file: %s", stbi_failure_reason ());

cleanup:
  stbi_image_free (pixels);
  return 0;
}
