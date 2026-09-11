# AISMV_Hardware_Interfaces

Firmware and drivers for the hardware side of AISMV, a machine-vision automatic irrigation system: camera interface, motors, obstacle avoidance, soil-moisture arm, water pump, and IoT control. This repository is the embedded interfaces, not the vision model.

## Install

PlatformIO project (`platformio.ini`). Clone and build with the board environment defined there.

```sh
git clone https://github.com/Dhi13man/AISMV_Hardware_Interfaces.git
cd AISMV_Hardware_Interfaces
pio run
```

## Use

Sources are under `src/`. The interfaces in this tree:

1. Machine vision interface
2. Motor controlling interface
3. Obstacle avoiding interface
4. Soil moisture sensing and arm controlling interface
5. Water pump control interface
6. IoT communication and control interface

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT. See [LICENSE](LICENSE).
