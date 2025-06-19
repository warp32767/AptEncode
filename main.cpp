#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include "WavAptEncoder.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"


int main(int argc, char** argv) {
    bool dual_image_mode = false;
    const char* channel_A_image_path = nullptr;
    const char* channel_B_image_path = nullptr;
    const char* output_file = nullptr;

    // Argument check
    if (argc < 3) {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "  Single image mode: " << argv[0] << " <input_image> <output_wav>" << std::endl;
        std::cerr << "  Dual image mode:   " << argv[0] << " -d <input_image_A> <input_image_B> <output_wav>" << std::endl;
        return 1;
    }

    // Determine the mode based on the first argument
    if (std::string(argv[1]) == "-d") {
        dual_image_mode = true;
        // Check for enough arguments for dual mode
        if (argc < 5) {
            std::cerr << "Error: Not enough arguments for dual image mode." << std::endl;
            std::cerr << "Usage: " << argv[0] << " -d <input_image_A> <input_image_B> <output_wav>" << std::endl;
            return 1;
        }
        // Check for too many arguments in dual mode
        if (argc > 5) {
            std::cerr << "Error: Too many arguments for dual image mode." << std::endl;
            std::cerr << "Usage: " << argv[0] << " -d <input_image_A> <input_image_B> <output_wav>" << std::endl;
            return 1;
        }
        channel_A_image_path = argv[2];
        channel_B_image_path = argv[3];
        output_file = argv[4];
    } else {
        // Single image mode
        // Check for too many arguments in single mode
        if (argc > 3) {
            std::cerr << "Error: Too many arguments for single image mode." << std::endl;
            std::cerr << "Usage: " << argv[0] << " <input_image> <output_wav>" << std::endl;
            return 1;
        }
        channel_A_image_path = argv[1];
        channel_B_image_path = argv[1];
        output_file = argv[2];
    }

    const int SAMPLE_RATE = 20800;
    const int MAX_OUTPUT_LINES = 1300;

    // Image loading and grayscale conversion for Channel A
    int img_A_width, img_A_height, img_A_channels;
    unsigned char* img_A_data_orig = stbi_load(channel_A_image_path, &img_A_width, &img_A_height, &img_A_channels, 0);
    if (!img_A_data_orig) {
        std::cerr << "Error: Could not load image A from " << channel_A_image_path << std::endl;
        return 1;
    }

    unsigned char* img_A_grayscale = nullptr;
    if (img_A_channels == 3 || img_A_channels == 4) {
        img_A_grayscale = new unsigned char[static_cast<size_t>(img_A_width) * img_A_height];
        for (int i = 0; i < img_A_width * img_A_height; ++i) {
            unsigned char r = img_A_data_orig[i * img_A_channels];
            unsigned char g = img_A_data_orig[i * img_A_channels + 1];
            unsigned char b = img_A_data_orig[i * img_A_channels + 2];
            img_A_grayscale[i] = static_cast<unsigned char>(std::round(r * 0.299 + g * 0.587 + b * 0.114));
        }
        stbi_image_free(img_A_data_orig);
    } else if (img_A_channels == 1) {
        img_A_grayscale = img_A_data_orig;
    } else {
        std::cerr << "Error: Unsupported number of channels for image A: " << img_A_channels << std::endl;
        stbi_image_free(img_A_data_orig);
        return 1;
    }

    // Image loading and grayscale conversion for Channel B
    int img_B_width, img_B_height, img_B_channels;
    unsigned char* img_B_data_orig = stbi_load(channel_B_image_path, &img_B_width, &img_B_height, &img_B_channels, 0);
    if (!img_B_data_orig) {
        std::cerr << "Error: Could not load image B from " << channel_B_image_path << std::endl;
        if (img_A_channels != 1) {
            delete[] img_A_grayscale;
        } else {
            stbi_image_free(img_A_grayscale);
        }
        return 1;
    }

    unsigned char* img_B_grayscale = nullptr;
    if (img_B_channels == 3 || img_B_channels == 4) {
        img_B_grayscale = new unsigned char[static_cast<size_t>(img_B_width) * img_B_height];
        for (int i = 0; i < img_B_width * img_B_height; ++i) {
            unsigned char r = img_B_data_orig[i * img_B_channels];
            unsigned char g = img_B_data_orig[i * img_B_channels + 1];
            unsigned char b = img_B_data_orig[i * img_B_channels + 2];
            img_B_grayscale[i] = static_cast<unsigned char>(std::round(r * 0.299 + g * 0.587 + b * 0.114));
        }
        stbi_image_free(img_B_data_orig);
    } else if (img_B_channels == 1) {
        img_B_grayscale = img_B_data_orig;
    } else {
        std::cerr << "Error: Unsupported number of channels for image B: " << img_B_channels << std::endl;
        stbi_image_free(img_B_data_orig);
        if (img_A_channels != 1) {
            delete[] img_A_grayscale;
        } else {
            stbi_image_free(img_A_grayscale);
        }
        return 1;
    }

    int effective_output_height = std::min({img_A_height, img_B_height, MAX_OUTPUT_LINES});

    if (effective_output_height == 0) {
        std::cerr << "Error: Effective output height is zero." << std::endl;
        if (img_A_channels != 1) { delete[] img_A_grayscale; } else { stbi_image_free(img_A_grayscale); }
        if (img_B_channels != 1) { delete[] img_B_grayscale; } else { stbi_image_free(img_B_grayscale); }
        return 1;
    }

    unsigned char* resized_img_A_data = new unsigned char[static_cast<size_t>(APT_IMAGE_WIDTH) * effective_output_height];
    unsigned char* resized_img_B_data = new unsigned char[static_cast<size_t>(APT_IMAGE_WIDTH) * effective_output_height];

    stbir_resize_uint8(img_A_grayscale, img_A_width, img_A_height, 0,
                       resized_img_A_data, APT_IMAGE_WIDTH, effective_output_height, 0,
                       1);

    stbir_resize_uint8(img_B_grayscale, img_B_width, img_B_height, 0,
                       resized_img_B_data, APT_IMAGE_WIDTH, effective_output_height, 0,
                       1);

    if (img_A_channels != 1) {
        delete[] img_A_grayscale;
    } else {
        stbi_image_free(img_A_grayscale);
    }
    if (img_B_channels != 1) {
        delete[] img_B_grayscale;
    } else {
        stbi_image_free(img_B_grayscale);
    }

    int num_lines = effective_output_height;
    int samples_per_line = static_cast<int>(SAMPLE_RATE * APT_LINE_DURATION);
    std::vector<int16_t> audio_data(static_cast<size_t>(samples_per_line) * num_lines);

    const int CHANNEL_A = 0;
    const int CHANNEL_B = 1;

    for (int line_idx = 0; line_idx < num_lines; line_idx++) {
        std::vector<float> apt_line(APT_PIXEL_PER_LINE, 0.0f);
        int current_offset = 0;

        generateSyncA(apt_line, current_offset);
        current_offset += APT_SYNC_A_WIDTH;

        generateSpaceMinuteMarker(apt_line, current_offset, line_idx, true);
        current_offset += APT_SPACE_A_WIDTH;

        generateImageData(apt_line, current_offset, resized_img_A_data, APT_IMAGE_WIDTH, num_lines, line_idx, CHANNEL_A);
        current_offset += APT_IMAGE_WIDTH;

        generateTelemetry(apt_line, current_offset, line_idx);
        current_offset += APT_TELEMETRY_WIDTH;

        generateSyncB(apt_line, current_offset);
        current_offset += APT_SYNC_B_WIDTH;

        generateSpaceMinuteMarker(apt_line, current_offset, line_idx, false);
        current_offset += APT_SPACE_B_WIDTH;

        generateImageData(apt_line, current_offset, resized_img_B_data, APT_IMAGE_WIDTH, num_lines, line_idx, CHANNEL_B);
        current_offset += APT_IMAGE_WIDTH;

        generateTelemetry(apt_line, current_offset, line_idx);

        for (int i = 0; i < samples_per_line; i++) {
            float apt_pixel_float_pos = static_cast<float>(i) * APT_PIXEL_PER_LINE / samples_per_line;

            int idx1 = static_cast<int>(std::floor(apt_pixel_float_pos));
            int idx2 = static_cast<int>(std::ceil(apt_pixel_float_pos));
            float frac = apt_pixel_float_pos - idx1;

            idx1 = std::min(APT_PIXEL_PER_LINE - 1, std::max(0, idx1));
            idx2 = std::min(APT_PIXEL_PER_LINE - 1, std::max(0, idx2));

            float baseband_value;
            if (idx1 == idx2) {
                baseband_value = apt_line[idx1];
            } else {
                baseband_value = apt_line[idx1] * (1.0f - frac) + apt_line[idx2] * frac;
            }

            float t = static_cast<float>(i) / SAMPLE_RATE;
            float carrier = std::sin(2.0f * M_PI * APT_SUBCARRIER_FREQ * t);

            float modulated_signal = (1.0f + APT_MAX_MODULATION * baseband_value) * carrier;
            audio_data[static_cast<size_t>(line_idx) * samples_per_line + i] = static_cast<int16_t>(modulated_signal * 16383.0f);
        }

        if (line_idx % 10 == 0) {
            std::cout << "Processing line " << line_idx << " of " << num_lines << std::endl;
        }
    }

    writeWavFile(output_file, audio_data, SAMPLE_RATE);

    delete[] resized_img_A_data;
    delete[] resized_img_B_data;

    std::cout << "APT encoding complete. Output written to: " << output_file << std::endl;
    return 0;
}