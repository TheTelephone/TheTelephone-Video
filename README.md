TheTelephone-Video
===

TheTelephone-Video complements TheTelephone to achieve the implementation of _software-based **video** telephony simulator_ for laboratory experiments.
Similar to TheTelephone, the concept is that the capturing, processing, and presentation of the video stream is conducted by one computer alone.
This approach reduces the complexity of the setup as it avoids setting up and verifying a computer network infrastructure (e.g., end-to-end delay).

TheTelephone-Video opens a locally connected webcam (e.g., via Video2Linux) and for each frame applies the configured processing (e.g., adding coding degradations, simulating packet loss or delay), and present the result directly on the screen.
Frame-wise processing is done here by passing a frame through a chain of filters while each filter can modify the frame's content.
While running TheTelephone-Video can switch from one filter chain to another.

__NOTE:__ This software is only a rough prototypical implementation. It may or may not work as desired.

It uses:
* [FFMPEG](https://ffmpeg.org/)
* [SDL2](https://www.libsdl.org/)

Build procedure (Debian 10)
---
1. `sudo apt install build-essential git cmake libavdevice-dev libavcodec-dev libavformat-dev libavfilter-dev libavswscale-dev`
2. `git clone https://github.com/TheTelephone/TheTelephone-Video`
3. `cd TheTelephone-Video && cmake . && cmake --build .`

License
---
This project is licensed as [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt) or later.
