#include <stdexcept>

#include <SDL.h>
#include <SDL_thread.h>

class Display: public FilterConvert {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    int previous_pts = 0;

public:
    Display(int width, int height): FilterConvert(AV_PIX_FMT_YUV420P, width, height) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) throw std::runtime_error("Could not initialize SDL.");

        this->window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->getWidth(), this->getHeight(), 0);
        //SDL_WINDOW_FULLwindow_DESKTOP

        if (!this->window) throw std::runtime_error("Could not create SDL (window).");

        this->renderer = SDL_CreateRenderer(this->window, -1, 0);
        if (!this->renderer) throw std::runtime_error("Could not create SDL (renderer).");

        // Allocate a place to put our YUV image on that window
        this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, this->getWidth(), this->getHeight());
        if (!texture) throw std::runtime_error("Could not not create SDL (texture).");

        //Render initial empty.
        SDL_RenderClear(this->renderer);
        SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
        SDL_RenderPresent(this->renderer);
    }

    ~Display() {
        SDL_DestroyWindow(this->window);
        SDL_DestroyRenderer(this->renderer);
        SDL_DestroyTexture(this->texture);
        SDL_Quit();
    }

    const AVFrame* filterConverted(const AVFrame* frame) {
        if (frame->pts < this->previous_pts) {
            std::cerr << "PTS is non-monotonic.\n";
        }
        this->previous_pts = frame->pts;

        SDL_UpdateYUVTexture(this->texture, NULL, frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);

        SDL_RenderClear(this->renderer);
        SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
        SDL_RenderPresent(this->renderer);

        return frame;
    }

    bool isFullscreen() {
        return !(SDL_GetWindowFlags(this->window) & SDL_WINDOW_FULLSCREEN);
    }

    void setFullscreen(bool is_fullscreen) {
        SDL_SetWindowFullscreen(this->window, is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    }
};
