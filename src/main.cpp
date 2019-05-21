#include <functional>
#include <iostream>
#include <csignal>
#include <cstdlib>

#include "camera.cpp"
#include "filter.cpp"
#include "filter_delay.cpp"
#include "filter_convert.cpp"
#include "filter_convert_grayscale.cpp"
#include "filter_convert_codec.cpp"
#include "filter_convert_codec_h264.cpp"
#include "display.cpp"

int main(int argc, char *argv[]) {
    Camera camera("video4linux2", "/dev/video0");

    //Create filters
    FilterDelay filterDelay(20);
    FilterConvertGrayscale filterGrayscale(camera.getWidth(), camera.getHeight());
    FilterConvertCodecH264 filterH264(camera.getWidth(), camera.getHeight());
    Display display(camera.getWidth(), camera.getHeight());

    //Setup filter chain.
    unsigned int filterIndex = 0;

    auto filterChain = [&](const AVFrame* f0) -> void {
        const AVFrame* f;

        switch(filterIndex) {
        case 1:
            f = filterGrayscale.filter(f0);
            break;
        case 2:
            f = filterDelay.filter(f0);
            break;
        case 3:
            f = filterH264.filter(f0);
            break;
        default:
            f = f0;
            break;
        }
        display.filter(f);
        av_frame_unref((AVFrame*)f);
    };

    //Event loop
    SDL_Event event;

    while (true) {
        SDL_PollEvent(&event);
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_f:
                display.setFullscreen(!display.isFullscreen());
                break;

                case SDLK_k:
                filterH264.setPacketLost([]() -> bool {
                    return false;
                });

                case SDLK_l:
                filterH264.setPacketLost([]() -> bool {
                    bool i =  rand() % 100 < 50;
                    return i;
                });
                break;

                case SDLK_SPACE:
                filterIndex = (filterIndex + 1) % 4;
                std::cout << "Next filter index: " << filterIndex << "\n";
                break;

                case SDLK_ESCAPE:
                exit(0);
                break;
            }
        }

        camera.readFrame(filterChain);
    }
}
