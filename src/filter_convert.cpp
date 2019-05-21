#include <iostream>

class FilterConvert: public Filter {
private:
    struct SwsContext *input_scaler_context = NULL;
    int input_width;
    int input_height;
    AVPixelFormat input_pixel_format;

    int output_width;
    int output_height;
    AVPixelFormat output_pixel_format;

protected:
    virtual const AVFrame* filterConverted(const AVFrame* frame) = 0;

public:
    FilterConvert(AVPixelFormat output_pixel_format, int output_width, int output_height) : Filter() {
        this->output_pixel_format = output_pixel_format;
        this->output_width = output_width;
        this->output_height = output_height;
    }

    ~FilterConvert() {
        if (this->input_scaler_context != NULL) sws_freeContext(this->input_scaler_context);
    }

    const AVFrame* filter(const AVFrame* frame) final {
        if (frame->height == 0) {
            //Frame has no content. Do not update window.
            return frame;
        }
        
        //Change scaler?
        if (this->input_scaler_context == NULL || frame->format != this->input_pixel_format || frame->width != this->input_width || frame->height != this->input_height) {
            this->input_scaler_context = sws_getContext(frame->width, frame->height, (AVPixelFormat)frame->format, this->output_width, this->output_height, this->output_pixel_format, SWS_BILINEAR, NULL, NULL, NULL);
            this->input_width = frame->width;
            this->input_height = frame->height;
            this->input_pixel_format = (AVPixelFormat)frame->format;
        }

        //Convert frame
        AVFrame* frameFormatted = av_frame_alloc();
        frameFormatted->width = this->output_width;
        frameFormatted->height = this->output_height;
        frameFormatted->format = this->output_pixel_format;
        frameFormatted->pts = frame->pts;
        av_frame_get_buffer(frameFormatted, 32);
        sws_scale(this->input_scaler_context, (uint8_t const * const *) frame->data, frame->linesize, 0, frame->height, frameFormatted->data, frameFormatted->linesize);
        av_frame_unref((AVFrame*)frame);

        //Filter converted frame
        return this->filterConverted(frameFormatted);
    }

    AVPixelFormat getPixelFormat() {
        return this->output_pixel_format;
    }

    int getWidth() {
        return this->output_width;
    }

    int getHeight() {
        return this->output_height;
    }
};
