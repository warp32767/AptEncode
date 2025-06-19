# AptEncode

**AptEncode** is a cross-platform command-line tool that converts standard images into **NOAA APT-format WAV audio files**, simulating the analog signal used by real weather satellites. It works on **Linux, Windows and macOS**, and supports a wide range of image formats out of the box.

![APT dual image mode Sample](https://raw.githubusercontent.com/warp32767/AptEncode/refs/heads/main/Samples/raw_sync.png)

---

## 🌟 Features

- 🖼️ Accepts **JPEG, PNG, BMP, and other standard formats**
- 📏 **Automatically scales** input images to proper width and handles images with different resolutions 
- 🎧 Outputs **WAV files compatible with all APT decoders**
- 🛠️ Works on **Linux, Windows and macOS**

---

## 🔧 Installation & Build

### Requirements

- C++20 compatible compiler

### Windows

- Open with your IDE of choice and hit build or something (I used CLion)

### Linux / macOS
- I don't use linux so if this snippet doesn't work i have no idea
```bash
g++ main.cpp WavAptEncoder.cpp -o AptEncode
```

---

## 🔎 Usage

### Single image mode (same image for both channels)
```bash
AptEncode <input> <output.wav>
```

### Dual image mode (separate images for each channel)
```bash
AptEncode <input_a> <input_b> <output.wav>
