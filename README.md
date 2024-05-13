# Wake on LAN telegram bot

## Building

You need to install libraries and build tools such as CMake.
On Debian-based Linux distributions you can do it as follows:

```sh
sudo apt-get install libcurl4-openssl-dev libjson-c-dev cmake binutils make
```

To build the library run following commands:

```sh
git submodule update --init --recursive
mkdir -p build && cd build
cmake ../
make
```

## usage:
```sh
./woltgbot -t TG_BOT_TOKEN -u TG_USER_IDS,SPLITED_BY,COMMA
```