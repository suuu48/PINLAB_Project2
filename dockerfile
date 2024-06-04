apt update // for install
apt install build-essential // for make
apt install gcc

// mkdir project // there is fin.c in project dir
// docker ps [fin.c]
// docker ps [portaudio]

cd portaudio
./configure && make
make install
cp /usr/local/lib/libportaudio.a /project

apt install alsa
apt install libasound2-dev
apt install libjack0
apt install libjack-dev

apt update

gcc fin.c libportaudio.a -lrt -lm -lasound -ljack -lpthread -o fin
// ./fin
