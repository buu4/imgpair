#include "mapfile.h"
#include "imgpair.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "error.h"

#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

/* This current running program name */
char *progname = "imgpair";

/* PNG file to be modified */
static char *image_target;

/* File to be stored to image_target */
static char *filename;
static char *output;


/* XXX - Some options may not work/undefined as a milestone */
const char _help[] =
"Usage: %s [options] [file] [-img img]\n"
"  Pair file content into image file. If no file provided read from image.\n"
"\n"
"  -i, --img IMG        Image files to be read/write.\n"
"  -f, --format FMT     Image format, if not specified read header.\n"
"  -o, --output FILE    Set output to FILE. if not specified defaults to IMG.\n"
"  --help               Show this message.\n"
"\n";

static int takechar (char *s, int n)
{
  if (s[n])
    return s[n];
  return 0;
}

static void parse_args (int argc, char **argv)
{
  int i;

  image_target = NULL;
  filename = NULL;
  output = NULL;

  progname = argv[0];

  if (argc == 1)
    goto out;

  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {

      if (strcmp (argv[i], "--help") == 0) {
        printf (_help, progname);
        exit (0);
      }
      else if (takechar (argv[i], 1) == 'i' || strcmp (argv[i], "--img") == 0) {
	if (i + 2 > argc)
	  break;
	image_target = argv[++i];
	continue;
      }
      else if (takechar (argv[i], 1) == 'o' || strcmp (argv[i], "--output") == 0) {
	if (i + 2 > argc)
	  break;
	output = argv[++i];
	continue;
      }

      report_error ("unknown options: %s", argv[i]);
      exit (1);
    }

    if (!filename)  /* Allow only 1 file */
      filename = argv[i];
  }

  if (image_target == NULL) {
    report_error ("no image provided");
    goto out;
  }

  /* Default value for options */
  if (output == NULL)
    output = image_target;

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
    report_error ("%s: %s", filename, strerror (errno));
    goto out;
  }

  err = imgpair_lsb_write (pixels, w * h * ch, 0, (const uint8_t *)map->ptr, map->sb.st_size);

  if (err == PNGPAIL_FAILURE) {
    report_error ("%s: %s", filename, imgpair_failure_reason ());
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

  pixels = stbi_load (image_target, &w, &h, &ch, 0);

  if (pixels == NULL) {
    report_error ("%s: %s", image_target, stbi_failure_reason ());
    return 1;
  }

  if (filename == NULL) {
    goto cleanup;
  }

  /* Write to file */
  merge_file_to_pixels (filename, pixels, w, h, ch);

  if (stbi_write_png (output, w, h, ch, pixels, w * ch) < 0)
    report_error ("can't write to output: %s", stbi_failure_reason ());

cleanup:
  stbi_image_free (pixels);
  return 0;
}
