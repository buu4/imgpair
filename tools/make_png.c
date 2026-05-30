/* ./make_png file.png */

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

#include <stdlib.h>

int main (int argc, char **argv)
{
  if (argc == 1)
    return 1;

  int w, h, ch;
  w = h = 256;
  ch = 3;

  size_t pixel_size = w * h * ch;
  unsigned char *pixels;

  pixels = (unsigned char *) malloc (pixel_size);

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int index = (y * w + x) * ch;
      pixels[index] = x;
      pixels[index + 1] = y;
      pixels[index + 2] = 128;
    }
  }

  stbi_write_png (argv[1], w, h, ch, pixels, w * ch);
  return 0;
}
