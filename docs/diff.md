# How to diff a paired image with unpaired image

## Getting Started

To do this, you will need a ImageMagick software to use the `compare` commands.
You can install ImageMagick with this command:

```bash
apt install imagemagick
```

Confirm if it installed with the `compare` command:

```bash
Copyright: (C) 1999-2021 ImageMagick Studio LLC
...
```

## Diff image

To compare the UNPAIRED with PAIRED image, you can run this command:

```bash
compare -compose src <UNPAIRED> <PAIRED> <DIFFOUTPUT>
```

For example i have this file:

```
./unpaired.png
./paired.png
```

Then, i can run the `compare` command to start diff
between that 2 images:

```bash
compare -compose src ./unpaired.png ./paired.png ./diff.png
```

Done! Now you can open the file `diff.png` using a image viewer software to see
the differences between `unpaired.png` and `./paired.png` image!.
I use `risretto` for that:

```bash
risretto diff.png
```
