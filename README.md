# AptEncode

**AptEncode** is a cross-platform command-line tool that converts standard images into **NOAA APT-format WAV audio files**, simulating the analog signal used by real weather satellites. It works on **Linux, Windows and macOS**, and supports a wide range of image formats out of the box.

![APT dual image mode Sample](placeholder)

---

## 🌟 Features

- 🖼️ Accepts **JPEG, PNG, BMP, and other standard formats**
- 📏 **Automatically scales** images to NOAA APT spec (909 pixels wide)
- 🎧 Outputs **WAV files compatible with all APT decoders**
- 🛠️ Works on **Linux, Windows, and macOS**

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
