#include "WavAptEncoder.h"
#include <cmath>
#include <fstream>
#include <algorithm>
#include <iostream>


void generateSyncA(std::vector<float>& line, int offset) {
    for (int i = 0; i < APT_SYNC_A_WIDTH; i++) {
        if((i==4) || (i==5) || (i==8) || (i==9) || (i==12) || (i==13) || (i==16)) {
            line[offset + i] = SYNC_HIGH_LEVEL;
        } else if((i==17) || (i==20) || (i==21) || (i==24) || (i==25) || (i==28) || (i==29)) {
            line[offset + i] = SYNC_HIGH_LEVEL;
        } else {
            line[offset + i] = SYNC_LOW_LEVEL;
        }
    }
}

void generateSpaceMinuteMarker(std::vector<float>& line, int offset, int line_index, bool isChannelA) {
    int minute_flag = ((line_index % APT_LINES_PER_MINUTE == 0) || (line_index % APT_LINES_PER_MINUTE == 1)) ? 1 : 0;

    float marker_value;
    if (isChannelA) {
        marker_value = (minute_flag == 1) ? MARKER_HIGH_LEVEL : MARKER_LOW_LEVEL;
    } else {
        marker_value = (minute_flag == 1) ? MARKER_LOW_LEVEL : MARKER_HIGH_LEVEL;
    }

    std::fill(line.begin() + offset, line.begin() + offset + APT_SPACE_A_WIDTH, marker_value);
}

void generateTelemetry(std::vector<float>& line, int offset, int line_index) {
    int frame_index_in_cycle = (line_index % APT_FRAME_LINES);
    float telemetry_value = 0.0f;

    if (frame_index_in_cycle >= 0 && frame_index_in_cycle <= 7) {
        telemetry_value = static_cast<float>(APT_TEL_WEDGE1) / 255.0f;
    } else if (frame_index_in_cycle >= 8 && frame_index_in_cycle <= 15) {
        telemetry_value = static_cast<float>(APT_TEL_WEDGE2) / 255.0f;
    } else if (frame_index_in_cycle >= 16 && frame_index_in_cycle <= 23) {
        telemetry_value = static_cast<float>(APT_TEL_WEDGE3) / 255.0f;
    } else if (frame_index_in_cycle >= 24 && frame_index_in_cycle <= 31) {
        telemetry_value = static_cast<float>(APT_TEL_WEDGE4) / 255.0f;
    } else if (frame_index_in_cycle >= 32 && frame_index_in_cycle <= 39) {
        telemetry_value = static_cast<float>(APT_TEL_WEDGE5) / 255.0f;
    } else if (frame_index_in_cycle >= 40 && frame_index_in_cycle <= 47) {
        telemetry_value = static_cast<float>(APT_TEL_WEDGE6) / 255.0f;
    } else if (frame_index_in_cycle >= 48 && frame_index_in_cycle <= 55) {
        telemetry_value = static_cast<float>(APT_TEL_WEDGE7) / 255.0f;
    } else if (frame_index_in_cycle >= 56 && frame_index_in_cycle <= 63) {
        telemetry_value = static_cast<float>(APT_TEL_WEDGE8) / 255.0f;
    } else if (frame_index_in_cycle >= 64 && frame_index_in_cycle <= 71) {
        telemetry_value = static_cast<float>(APT_TEL_ZEROMODREF) / 255.0f;
    } else if (frame_index_in_cycle >= 72 && frame_index_in_cycle <= 79) {
        telemetry_value = static_cast<float>(APT_TEL_TEMP1) / 255.0f;
    } else if (frame_index_in_cycle >= 80 && frame_index_in_cycle <= 87) {
        telemetry_value = static_cast<float>(APT_TEL_TEMP2) / 255.0f;
    } else if (frame_index_in_cycle >= 88 && frame_index_in_cycle <= 95) {
        telemetry_value = static_cast<float>(APT_TEL_TEMP3) / 255.0f;
    } else if (frame_index_in_cycle >= 96 && frame_index_in_cycle <= 103) {
        telemetry_value = static_cast<float>(APT_TEL_TEMP4) / 255.0f;
    } else if (frame_index_in_cycle >= 104 && frame_index_in_cycle <= 111) {
        telemetry_value = static_cast<float>(APT_TEL_PATCHTEMP) / 255.0f;
    } else if (frame_index_in_cycle >= 112 && frame_index_in_cycle <= 119) {
        telemetry_value = static_cast<float>(APT_TEL_BACKSCAN) / 255.0f;
    } else if (frame_index_in_cycle >= 120 && frame_index_in_cycle <= 127) {
        telemetry_value = static_cast<float>(APT_TEL_CHANNELID) / 255.0f;
    }
    std::fill(line.begin() + offset, line.begin() + offset + APT_TELEMETRY_WIDTH, telemetry_value);
}

void generateSyncB(std::vector<float>& line, int offset) {
    for (int i = 0; i < APT_SYNC_B_WIDTH; i++) {
        if((i==4) || (i==5) || (i==6) || (i==9) || (i==10) || (i==11) || (i==14)) {
            line[offset + i] = SYNC_HIGH_LEVEL;
        } else if((i==15) || (i==16) || (i==19) || (i==20) || (i==21) || (i==24) || (i==25)) {
            line[offset + i] = SYNC_HIGH_LEVEL;
        } else if((i==26) || (i==29) || (i==30) || (i==31) || (i==34) || (i==35) || (i==36)) {
            line[offset + i] = SYNC_HIGH_LEVEL;
        } else {
            line[offset + i] = SYNC_LOW_LEVEL;
        }
    }
}

void generateImageData(std::vector<float>& line, int offset, const unsigned char* image_data,
                       int img_width, int img_height, int line_index, int channel) {
    for (int i = 0; i < APT_IMAGE_WIDTH; i++) {
        int img_x = i;
        int img_y = std::min(img_height - 1, std::max(0, line_index));

        float pixel_value = static_cast<float>(image_data[static_cast<size_t>(img_y) * img_width + img_x]) / 255.0f;

        line[offset + i] = pixel_value;
    }
}

void writeWavFile(const std::string& filename, const std::vector<int16_t>& audio_data, int sample_rate) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open output file for writing: " << filename << std::endl;
        return;
    }

    WavHeader header;
    header.sample_rate = static_cast<uint32_t>(sample_rate);
    header.byte_rate = static_cast<uint32_t>(sample_rate * header.num_channels * header.bit_depth / 8);
    header.data_bytes = static_cast<uint32_t>(audio_data.size() * sizeof(int16_t));
    header.wav_size = header.data_bytes + sizeof(WavHeader) - 8;

    file.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));
    file.write(reinterpret_cast<const char*>(audio_data.data()), header.data_bytes);
    file.close();
}