class FilterConvertCodec: public FilterConvert {
private:
    std::function<bool()> packetLost = NULL;

public:
    FilterConvertCodec(AVPixelFormat output_pixel_format, int output_width, int output_height): FilterConvert(output_pixel_format, output_width, output_height) {
    }

    void setPacketLost(std::function<bool()> packetLost) {
        this->packetLost = packetLost;
    }

protected:
    bool wasPacketLost() {
        if (this->packetLost == NULL) return false;
        return this->packetLost();
    }
};
