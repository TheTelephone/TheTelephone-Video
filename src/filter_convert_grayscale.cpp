class FilterConvertGrayscale: public FilterConvert {

public:
    FilterConvertGrayscale(int output_width, int output_height): FilterConvert(AV_PIX_FMT_GRAY8A, output_width, output_height) {
    }

    const AVFrame* filterConverted(const AVFrame* frame) {
        return frame;
    }
};
