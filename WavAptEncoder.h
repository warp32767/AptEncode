#ifndef WAV_APT_ENCODER_H
#define WAV_APT_ENCODER_H

#include <cstdint>
#include <vector>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct WavHeader {
    char riff_header[4] = {'R', 'I', 'F', 'F'};
    uint32_t wav_size;
    char wave_header[4] = {'W', 'A', 'V', 'E'};
    char fmt_header[4] = {'f', 'm', 't', ' '};
    uint32_t fmt_chunk_size = 16;
    uint16_t audio_format = 1;
    uint16_t num_channels = 1;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t sample_alignment = 2;
    uint16_t bit_depth = 16;
    char data_header[4] = {'d', 'a', 't', 'a'};
    uint32_t data_bytes;
};

// NOAA APT Constants
constexpr int APT_PIXEL_PER_LINE = 2080;
constexpr int APT_SYNC_A_WIDTH = 39;
constexpr int APT_SPACE_A_WIDTH = 47;
constexpr int APT_IMAGE_WIDTH = 909;
constexpr int APT_TELEMETRY_WIDTH = 45;
constexpr int APT_SYNC_B_WIDTH = 39;
constexpr int APT_SPACE_B_WIDTH = 47;

constexpr float APT_SUBCARRIER_FREQ = 2400.0f;
constexpr float APT_LINE_DURATION = 0.5f;
constexpr int APT_LINES_PER_MINUTE = 120;
constexpr int APT_FRAME_LINES = 128;

constexpr float APT_MAX_MODULATION = 0.87f;

// Sync and telemetry levels
constexpr float SYNC_HIGH_LEVEL = 244.0f / 255.0f;
constexpr float SYNC_LOW_LEVEL = 11.0f / 255.0f;
constexpr float MARKER_LOW_LEVEL = 0.0f / 255.0f;
constexpr float MARKER_HIGH_LEVEL = 255.0f / 255.0f;

// Telemetry values
enum AptTelemetryData {
    APT_TEL_WEDGE1 = 31,
    APT_TEL_WEDGE2 = 63,
    APT_TEL_WEDGE3 = 95,
    APT_TEL_WEDGE4 = 127,
    APT_TEL_WEDGE5 = 159,
    APT_TEL_WEDGE6 = 191,
    APT_TEL_WEDGE7 = 223,
    APT_TEL_WEDGE8 = 255,
    APT_TEL_ZEROMODREF = 0,
    APT_TEL_TEMP1 = 105,
    APT_TEL_TEMP2 = 105,
    APT_TEL_TEMP3 = 105,
    APT_TEL_TEMP4 = 105,
    APT_TEL_PATCHTEMP = 120,
    APT_TEL_BACKSCAN = 150,
    APT_TEL_CHANNELID = 31
};

void generateSyncA(std::vector<float>& line, int offset);
void generateSpaceMinuteMarker(std::vector<float>& line, int offset, int line_index, bool isChannelA);
void generateTelemetry(std::vector<float>& line, int offset, int line_index);
void generateSyncB(std::vector<float>& line, int offset);
void generateImageData(std::vector<float>& line, int offset, const unsigned char* image_data,
                       int img_width, int img_height, int line_index, int channel);
void writeWavFile(const std::string& filename, const std::vector<int16_t>& audio_data,
                  int sample_rate);

#endif // WAV_APT_ENCODER_H