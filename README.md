# psplib

A simple 2D graphics and input library for PSP.

The goal of this was to learn more about the PSP GU and not have any GPL/LGPL dependencies like gLib2D.
The code is referenced from the psp sdk, sdk samples, OSLib source and libvita2d.
Some of the code is things I wrote myself for gLib2DMod.

For audio, [OSLib](https://github.com/hexundev/oslibaudio) supports WAV, ADPCM and other formats.

This was tested with the Minimalist PSP SDK 0.15.0.

## Features
- Basic input functions
- Loading PNG textures with lodepng
- Setting views and projections
- Drawing sprites, rects or vertex arrays

## Samples
To do

## TODO
### Graphics
- Use VPU
- Texture padding to po2 at load
- Textures movable to VRAM and VRAM management
- ~~Swizzling~~ (used swizzle_fast from SDK samples)
- ~~Texture formarts other than 8-bit RGBA and dithering~~ Defaulting to 16bit for everything
- Look at other other vertex and primitive formats for rendering
- Drawing untextured rects with color_vertex
- More drawing functions

### Input
- Analog press/release 
- ~~Get d-pad as float values~~

## License
MIT
