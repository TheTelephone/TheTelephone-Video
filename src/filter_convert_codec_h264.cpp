#include <stdexcept>
#include <iostream>

class FilterConvertCodecH264: public FilterConvertCodec {
private:
    AVCodecContext *encoder_context;
    AVCodecContext *decoder_context;

public:
    FilterConvertCodecH264(int output_width, int output_height): FilterConvertCodec(AV_PIX_FMT_YUV420P, output_width, output_height) {
        //Encoder
        AVCodec *encoder_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (!encoder_codec) {
            throw std::runtime_error("Codec not found.");
        }
        this->encoder_context = avcodec_alloc_context3(encoder_codec);
        if (!this->encoder_context) {
            throw std::runtime_error("Could not allocate codec context.");
        }

        this->encoder_context->bit_rate = 1000;
        this->encoder_context->width = output_width;
        this->encoder_context->height = output_height;
        this->encoder_context->time_base = (AVRational){1, 25}; // frames per second
        this->encoder_context->gop_size = 5;
        this->encoder_context->max_b_frames = 1;
        this->encoder_context->pix_fmt = this->getPixelFormat();

        av_opt_set(this->encoder_context->priv_data, "profile", "baseline", AV_OPT_SEARCH_CHILDREN);
        av_opt_set_int(this->encoder_context->priv_data, "rc-lookahead", 0, 0);
        av_opt_set(this->encoder_context->priv_data, "preset", "ultrafast", 0);

        if (avcodec_open2(this->encoder_context, encoder_codec, NULL) < 0) {
            throw std::runtime_error("Could not initialize codec context.");
        }

        //Decoder
        AVCodec *decoder_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        if (!decoder_codec) {
            throw std::runtime_error("Codec not found.");
        }
        this->decoder_context = avcodec_alloc_context3(decoder_codec);
        if (!this->decoder_context) {
            throw std::runtime_error("Could not allocate video codec context.");
        }
        if (avcodec_open2(this->decoder_context, decoder_codec, NULL) < 0) {
            throw std::runtime_error("Could not initialize codec context.");
        }
    }

    ~FilterConvertCodecH264() {
        avcodec_close(this->encoder_context);
        av_free(this->encoder_context);
        avcodec_close(this->decoder_context);
        av_free(this->decoder_context);

    }

    const AVFrame* filterConverted(const AVFrame* frame) {
        AVFrame* frameFiltered = av_frame_alloc();

        //Encode
        int result;
        result = avcodec_send_frame(this->encoder_context, frame);
        av_frame_unref((AVFrame*)frame);

        if (result == AVERROR(EAGAIN)) {
            return frameFiltered;
        }

        if (result < 0) {
            //TODO REMOVE
            char a[512];
            av_strerror(result, a, 512);
            std::cout << a << "\n";

            std::cout << "error sending a frame for encoding " << result << "\n";
            return frameFiltered;
        }

        AVPacket* packet = av_packet_alloc();
        int i = 0;
        while (true) {
            //Get packet
            result = avcodec_receive_packet(this->encoder_context, packet);
            if (result == AVERROR(EAGAIN) || result == AVERROR(AVERROR_EOF)) {
                break;
            }
            if (result < 0) {
                std::cout << "error during encoding\n";
                break;
            }
            //Process packet
            if (result == 0 && this->wasPacketLost() == false) {
                result = avcodec_send_packet(this->decoder_context, packet);
            }
        }

        //Decode
        av_packet_unref(packet);

        result = avcodec_receive_frame(this->decoder_context, frameFiltered);
        if (result < 0) {
            //TODO can we ignore result < 0?
        }

        return frameFiltered;
    }
};
