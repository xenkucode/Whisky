# Whisky
---
Convert video files into ASCII art playback in your terminal, written in C.

![demo](demo.gif)

## Limitations
---
- Current version only works on Linux.
- Terminal cannot be resized during playback.
## Installation

Make sure you have [FFmpeg](https://ffmpeg.org/) installed.

Recommended terminal emulator: [kitty](https://sw.kovidgoyal.net/kitty/binary/) 

---
```bash
git clone https://github.com/xenkucode/Whisky.git
cd Whisky
make
```

## Usage
---
```bash
./whisky <path-to-video> <FPS>
```

### Example
---
```bash
./whisky sample.mp4 30
```
## How It Works
---
1. Frames are extracted from the video and resized to fit the terminal using FFmpeg.
2. Pixel data (RGB values) for each frame is read using the stb_image library.
3. Brightness is calculated per pixel and mapped to a corresponding ASCII character from a brightness ramp.
4. Characters are printed to the terminal using ANSI color escape sequences.
