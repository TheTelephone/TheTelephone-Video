#include <list>

class FilterDelay: public Filter {
private:
    std::list<const AVFrame*> buffer;

public:
    FilterDelay(unsigned int delayFrame) {
        for(unsigned int i = 0; i < delayFrame; i++) {
            AVFrame* frame = av_frame_alloc();
            this->buffer.push_back(frame);
        }
    }

    const AVFrame* filter(const AVFrame* frame) {
        this->buffer.push_back(frame);
        auto frameReturn = this->buffer.front();
        this->buffer.pop_front();
        return frameReturn;
    }
};
