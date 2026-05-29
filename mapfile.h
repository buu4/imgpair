#ifndef _MAPFILE_H
#define _MAPFILE_H

#include <sys/mman.h>
#include <sys/stat.h>

struct mapfile
{
  int fd;
  void *ptr;
  struct stat sb;
};

extern struct mapfile *mapfile_open (const char *pathname, int mapflags, int fdflags,
    int prot, off_t offset);
extern void mapfile_close (struct mapfile *map);

#endif /* _MAPFILE_H */
