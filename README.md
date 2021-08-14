# Dock

CutefishOS application dock.

## Dependencies

```shell
sudo pacman -S gcc cmake qt5-base qt5-quickcontrols2 kwindowsystem
```

You also need [`fishui`](https://github.com/cutefishos/fishui) and [`libcutefish`](https://github.com/cutefishos/libcutefish).

## Build and Install

```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
make
sudo make install
```

## License

This project has been licensed by GPLv3.
