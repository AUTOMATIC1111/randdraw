Requirements
============

 * imagemagick's convert program
 * ffmpeg (optional, for --webm option)

Program: randdraw
=================

Draws a picture using shapes as described in this post: https://warosu.org/g/thread/S62885489

```
Usage:
  randdraw [OPTION...] <input> <output>

  -p, --program arg        program; a list of comma separated pairs, each
                           takes form of <shape>:<iterations>; for example
                           lines:10000,squares:20000 means do 10000 iterations
                           drawing lines, then 20000 iterations drawing squares;
                           possible shapes are: squares, large-squares, lines,
                           circles (default: lines:100000)
  -d, --dump arg           dump a picture to disk every N iterations; if you
                           use this option, you should either also use --webm,
                           or output filename should include %d in it --
                           multiple pictures will be created, and %d will be
                           replaced by a number
  -f, --input arg          input image
  -m, --measure            measure time taken
  -o, --output arg         output image
  -c, --colors arg         limit number of colors to use when drawing
      --webm               produce a video; use together with --dump
      --colordist arg      color distance function (for -c option only!); one
                           of: Manhattan, Euclidean, CIE2000
      --import-colors arg  read all colors from the image specified in this
                           argument (instead of from input image)
      --allow-worse arg    also commit edits to image that make it look less
                           like input image - if the decrease in likeness is
                           not too large; the numeric argument specifies the
                           threshold for throwing edits away;  a value of 0.5 is
                           a good start
      --help               Print help
```

For example, if you run it as:

`randdraw source.png destination.png -p 'large-squares:10000,lines:100000'`

It will first do 10000 iterations to paint small squares, then
100000 iterations drawing lines.

This command will create multiple images:

`randdraw source.png destination-%02d.png -d 2000 -p 'large-squares:10000,lines:100000'`

Program: extract-colors
=======================

Get a list of important colors from an image. Colors will be written
in #RRGGBB format to stdout, and additionally an image will be created
with colors in it.

```
Usage:
  extract-colors [OPTION...] <input> [<output>]

  -f, --input arg          input image
  -o, --output arg         output image with desired colors in it
  -c, --count arg          how many colors to produce; if omitted, detected
                           automatically
  -m, --colormap arg       apply the produced palette and write result to
                           specified colormap file
      --method arg         method for color extraction; must be one of:
                           kmeans, random
  -q, --quiet              do not output colors to stdout
      --colordist arg      color distance function; one of: Manhattan,
                           Euclidean, CIE2000
      --import-colors arg  ignore colors in input image; instead read all
                           colors from the one specified in this argument
      --help               Print help
```
