extern "C" {
    #include <libswscale/swscale.h>

    #include <libavdevice/avdevice.h>
}

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

/**
Uses FFMPEG to read frames from a camera.
*/
class Camera {

private:
    AVFormatContext *format_context;
    AVCodecContext *decoder_context;
    AVInputFormat *input_format; //TODO necessary?
    AVCodec * decoder_codec; //TODO necessary?
    int video_stream_index = -1;

public:
    Camera(std::string driver, std::string cameraPath) {
        avdevice_register_all();

        this->format_context = avformat_alloc_context();
        this->input_format = av_find_input_format(driver.c_str());

        //Open webcam and open stream
        if (avformat_open_input(&format_context, cameraPath.c_str(), input_format, NULL) != 0) {
            throw std::runtime_error("No camera found.");
        }
        if (avformat_find_stream_info(format_context, NULL) < 0) {
            throw std::runtime_error("Could not get stream information.");
        }

        for (int i = 0; i < format_context->nb_streams; i++) {
            if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                this->video_stream_index = i;
                break;
            }
        }
        if (this->video_stream_index == -1) {
            throw std::runtime_error("No camera does not provide a video stream.");
        }

        this->decoder_codec = avcodec_find_decoder(format_context->streams[video_stream_index]->codecpar->codec_id);
        this->decoder_context = avcodec_alloc_context3(decoder_codec);

        avcodec_parameters_to_context(this->decoder_context, this->format_context->streams[video_stream_index]->codecpar);
        if (avcodec_open2(this->decoder_context, this->decoder_codec, NULL) < 0) {
            throw std::runtime_error("Could not get codec.");
        }
    }

    ~Camera() {
        avcodec_close(this->decoder_context);
        av_free(this->decoder_context);
    }

    int getWidth() {
        return this->decoder_context->width;
    }

    int getHeight() {
        return this->decoder_context->height;
    }

    AVPixelFormat getPixelFormatOutput() {
        return this->decoder_context->pix_fmt;
    }

    /**
    Read frames from camera.
    For each frame, `callback(frame)` is executed.
    Frames are allocated, while need to be freed by the sink.
    */
    int readFrame(std::function<void(const AVFrame*)> callback) {
        int result;
        AVPacket* packet = av_packet_alloc();

        //Get data
        result = av_read_frame(this->format_context, packet);

        //Send data to decoder (the selected stream only)
        do {
            result = avcodec_send_packet(this->decoder_context, packet);
            if (result < 0) {
                av_packet_unref(packet);
                return result == AVERROR(EAGAIN) ? 0 : result;
            }
        } while (packet->stream_index != this->video_stream_index);
        av_packet_unref(packet);

        //Get frame
        while (!result) {
            AVFrame* frame = av_frame_alloc();
            result = avcodec_receive_frame(this->decoder_context, frame);
            if (!result) {
                callback(frame);
            }
        }

        return result == AVERROR(EAGAIN) ? 0 : result;
    }
};
