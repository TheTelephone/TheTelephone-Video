#include <libavutil/frame.h>

class Filter {
public:
    Filter() {
    }

    virtual const AVFrame* filter(const AVFrame* frame) = 0;

protected:
    bool isFrameEmpty(const AVFrame* frame) {
        return frame->height == 0;
    }
};
