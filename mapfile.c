#include "mapfile.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

struct mapfile *mapfile_open (const char *pathname, int mapflags, int fdflags,
    int prot, off_t offset)
{
  struct mapfile *map;
  map = (struct mapfile *) malloc (sizeof (struct mapfile));

  if (!map)
    return NULL;

  map->fd = open (pathname, fdflags);

  if (fstat (map->fd, &map->sb) == -1)
    goto out;

  map->ptr = mmap (NULL, map->sb.st_size, prot, mapflags, map->fd, offset);

  if (map->ptr == MAP_FAILED)
    goto out;

  return map;

out:
  mapfile_close (map);
  return map;
}

void mapfile_close (struct mapfile *map)
{
  if (map->fd > 0)
    close (map->fd);
  if (map->ptr != MAP_FAILED)
    munmap (map->ptr, map->sb.st_size);
  free (map);
  map = NULL;
}
