![header](https://capsule-render.vercel.app/api?type=soft&color=006EDB&fontColor=DEEAF7&height=200&section=header&text=PINLAB&desc=Two-Way%20Real-Time%20Voice%20Communication&descAlignY=80&fontSize=90)
# PINLAB_Project2 
- Two-Way Real-Time Voice Communication on Raspberry PI

---

## Navigation
1. [Description](#Description)
2. [Getting started](#Getting-Started)
3. [Architecture](#Architecture)

---

## Description
Two-way real-Time voice communication with two Raspberry PI boards
- Managing sound I/O using ALSA and libasound2-dev library
- Using multithread
- TCP communication between server and client

---

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 
See deployment for notes on how to deploy the project on a live system.

### Installing & Setting
- Installing the ALSA library used for sound I/O in Linux
    - alsa-utils: Required for sound settings
    - libasound2-dev: Required for sound-related program development
```console
sudo apt-get install alsa-utils
sudo apt-get install libasound2-dev
```

- Configuring sound device in the ALSA library
```console
aplay -l
plughw : {card_number},{device_number}
arecord –l
plughw : {card_number},{device_number}

alsamixer
```

### Running the tests
```console
gcc {file_name.cpp} -lasound -Wno-write-strings -o {exe_name}
```

### docker: [kimakuma8/ubuntu:project3](https://hub.docker.com/layers/kimakuma8/ubuntu/project3/images/sha256-a7c68cba54a68254646067b6d37e700e0ff1d643d7900beafe8b2a5fcd9ea4f2?context=repo)

---

## Architecture
### Test Scenario
![image](https://user-images.githubusercontent.com/76460405/202168003-e979e001-38b4-47fa-be5f-8709350d8306.png)

### Test Bed
![image](https://user-images.githubusercontent.com/76460405/202168300-1d980c54-5547-483f-ba12-6907fcb7ad9a.png)

---

## Stacks
<img src="https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=C&logoColor=white"> <img src="https://img.shields.io/badge/Raspberry Pi-A22846?style=for-the-badge&logo=Raspberry Pi&logoColor=white"> <img src="https://img.shields.io/badge/Docker-2496ED?style=for-the-badge&logo=Docker&logoColor=white">
