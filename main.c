#include "mapfile.h"
#include "imgpair.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "error.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

/* This current running program name */
char *progname = "imgpair";

struct cmdlineopt
{
  char *image;
  char *file;
  char *output;
  size_t bit_offset;
};

static struct cmdlineopt  global_opt;

const char _version[] =
"imgpair 0.1\n"
"Copyright (C) 2026 Bayu Setiawan. License GPLv2\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n";

/* XXX - Some options may not work/undefined as a milestone */
const char _help[] =
"Usage: %s [options] [file] [-img img]\n"
"  Pair file content into image file. If no file provided read from image.\n"
"  If --output is - or its not specified when reading, output to stdout.\n"
"\n"
"  -i, --img IMG        Image files to be read/write.\n"
"  -f, --format FMT     Image format, if not specified read header.\n"
"  -o, --output FILE    Set output to FILE. if not specified defaults to IMG,\n"
"                       if read from IMG: output data to FILE.\n"
"  --offset NUM         Set offset to be start of data written to IMG.\n"
"  --version            Show program version\n"
"  --help               Show this message.\n"
"\n";

static void parse_args (int argc, char **argv)
{
  int i;

  global_opt = (struct cmdlineopt) {
    .image = NULL,
    .file = NULL,
    .bit_offset = 0,
    .output = NULL,
  };

  progname = argv[0];

  if (argc == 1)
    goto out;

  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {

      if (strcmp (argv[i], "--help") == 0) {
        printf (_help, progname);
        exit (0);
      }
      else if (strcmp (argv[i], "--version") == 0) {
        printf (_version);
        exit (0);
      }
      else if ((argv[i][1] == 'i' && argv[i][2] == '\0') ||
                  strcmp (argv[i], "--img") == 0) {
	if (i + 2 > argc)
	  break;
	global_opt.image = argv[++i];
	continue;
      }
      else if ((argv[i][1] == 'o' && argv[i][2] == '\0') ||
                   strcmp (argv[i], "--output") == 0) {
	if (i + 2 > argc) {
          report_error ("%s: argument required", argv[i]);
          goto out;
        }
	global_opt.output = argv[++i];
	continue;
      }
      else if (strcmp (argv[i], "--offset") == 0) {
        char *endptr;
        long value;

	if (i + 2 > argc) {
          report_error ("%s: argument required", argv[i]);
          goto out;
        }

        errno = 0;
        value = strtol(argv[++i], &endptr, 0);

        if (errno != 0) {
          report_error (strerror (errno));
          exit (1);
        }
        else if (endptr == argv[i]) {
          report_error ("%s: no value provided", argv[i - 1]);
          continue;
        }
        else if (value < 0) {
          report_error ("%s: value can't be negative", argv[i - 1]);
          exit (1);
        }

	global_opt.bit_offset = value;
	continue;
      }

      report_error ("unknown options: %s", argv[i]);
      goto out;
    }

    if (!global_opt.file)  /* Allow only 1 file */
      global_opt.file = argv[i];
  }

  if (global_opt.image == NULL) {
    report_error ("no image provided");
    goto out;
  }

  /* Default value for options */
  if (global_opt.output == NULL)
    {
      /* Output to stdout if this read from image */
      if (global_opt.file == NULL) {
        global_opt.output = "-";
        return;
      }

      global_opt.output = global_opt.image;

      char c;
      char y = '\0';
      printf ("Write output to '%s'?: ", global_opt.output);
      while ((c = fgetc (stdin)) != EOF) {
        if (c == '\n') {
          if (y == 'y' ||  y == 'Y')
            break;
          else {
	    printf ("no.\n");
            exit (1);
          }
        }
        else {
          y = c;
        }
      }

      if (c == EOF) {
        printf ("no.\n");
        exit (1);
      }
    }

  return;

out:
  fprintf (stderr, _help, progname);
  exit (1);
}

int main (int argc, char **argv)
{
  int w, h, ch;
  uint8_t *pixels;

  parse_args (argc, argv);

  pixels = stbi_load (global_opt.image, &w, &h, &ch, 0);

  if (pixels == NULL) {
    report_error ("%s: %s", global_opt.image, stbi_failure_reason ());
    return 1;
  }

  /* READ */
  if (global_opt.file == NULL) {
    struct mapfile *map;
    int fd;

    if (global_opt.output == NULL || (global_opt.output[0] == '-' &&
            global_opt.output[1] == '\0')) {
      fd = 1;
      map = NULL;
    }
    else {
      map = mapfile_open (global_opt.output, MAP_PRIVATE, O_RDONLY,
                PROT_READ, 0);

      if (!map) {
        report_error ("%s: %s", global_opt.output, strerror (errno));
        goto out;
      }

      fd = map->fd;
    }

    if (imgp_decode_pixels (fd, pixels, w, h, ch,
            global_opt.bit_offset, 1024) == IMGP_FAILURE) {
      report_error ("%s: %s", global_opt.file, imgp_failure_reason ());
      mapfile_close (map);
      goto out;
    }

    if (map)
      mapfile_close (map);
    if (fd == 1)
      putchar ('\n');
    goto cleanup;
  }
  /* WRITE */
  else {
    if (imgp_write_file (global_opt.file, pixels, w, h, ch,
            global_opt.bit_offset) == IMGP_FAILURE) {
      report_error ("%s: %s", global_opt.file, imgp_failure_reason ());
      goto out;
    }

    if (stbi_write_png (global_opt.output, w, h, ch, pixels, w * ch) < 0) {
      report_error ("can't write to output: %s", stbi_failure_reason ());
      goto out;
    }

    goto cleanup;
  }

out:
  stbi_image_free (pixels);
  return 1;

cleanup:
  stbi_image_free (pixels);
  return 0;
}
