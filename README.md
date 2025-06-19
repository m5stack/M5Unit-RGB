# M5Unit - RGB

## Overview

Library for RGB using [M5UnitUnified](https://github.com/m5stack/M5UnitUnified).  
M5UnitUnified is a library for unified handling of various M5 units products.

### SKU:U003
Unit RGB is an LED light unit. It integrates 3 programmable LED lights, allowing you to customize their color and brightness through programming. It supports daisy-chaining multiple Unit RGBs for expansion. You can use it in STEM classrooms to create interesting applications, such as traffic lights, or use it in other projects as status indicators.

### SKU:A035 A035-B A035-C A035-D A035-E
Unit RGB LED is a programmable light strip that uses SK6812 LEDs. This light strip supports digital addressing, meaning you can individually control the color and brightness of each LED on the strip. It uses a single-bus programming interface, allowing for the expansion of the light strip. Note that as the number of connected LEDs increases, so does the power consumption. Therefore, when using a Unit RGB LED strip with a large number of LEDs, an additional power supply is required.

### SKU:A045
Unit HEX is a hexagonal RGB LED light board. It embeds a total of 37 RGB LED beads and provides input and output ports, which means you can connect multiple RGB LEDs in series.

### SKU:A045-B
Unit NeoHEX is a hexagonal RGB LED light panel embedded with 37 RGB LED full-color beads. It uses a single-bus control and supports cascading multiple light panels. It provides two sets of input interfaces with different socket angles, which can be used for independent power supply + signal control, ensuring sufficient power supply when cascading multiple light panels. The external structure comes with mounting ears for easy embedding and fixing. It comes with a milky white diffuser, making the lighting effect softer, suitable for various lighting DIY exhibits.

### SKU:A093
Unit RGB LED Strip is a programmable light strip that uses SK6812 LED beads. This strip supports digital addressing, meaning you can individually control the color and brightness of each LED on the strip. It uses a single-bus programming interface, allowing for strip expansion. The strip is processed with a waterproof epoxy coating, and the transparent protective silicone layer provides IP65 waterproof protection, making it suitable for various environments. The product uses flexible FPC (soft strip board) / PCB (hard strip board) as the base for soldering LEDs, with 5050 RGB LED beads. The back is equipped with 3M strong double-sided tape for easy installation. It operates at a 5V DC voltage, and various lighting effects can be achieved through controller programming.

### SKU:U163
Unit Neco is an RGB light board unit with a "cat ear design," featuring 35 WS2812C-2020 RGB LEDs. It comes with two 4-pin Grove ports for connecting to M5 hosts and expanding more units. Additionally, it includes a touch button for interacting with the host and switching between different lighting effects. It is suitable for home decoration, party atmospheres, stage performances, and more.

### SKU:U193
Unit Puzzle is a colorful light control unit composed of an 8x8 array of 64 WS2812E RGB LEDs. Each LED is separated by a grid structure to prevent interference between adjacent lights, ensuring clearer and purer light effects. The product features two Grove interfaces, one for input signal and the other for output, along with a connector for linking multiple Unit Puzzle units to expand the light display area. The output Grove interface also includes a reserved IO port, allowing connection to other M5 devices, such as the Unit Angle sensor with a Port B interface, enabling more interactive control functions. The Unit Puzzle is compact and easy to install, with an M2 hole at the bottom for secure mounting. The product measures only 24 x 24 x 15mm. It is suitable for applications such as smart lighting, decorative displays, and interactive installations.


### SKU:C008-B
M5AtomMatrix  has a 5*5 RGB Led matrix on the panel. This library can manage Led matrix.


## Related Link
See also examples using conventional methods here.

- [Unit RGB - Document & Datasheet](https://docs.m5stack.com/en/unit/rgb)
- [Unit RGB LED - Document & Datasheet](https://docs.m5stack.com/en/unit/neopixel)
- [Unit Hex - Document & Datasheet](https://docs.m5stack.com/en/unit/hex)
- [Unit NeoHex - Document & Datasheet](https://docs.m5stack.com/en/unit/neohex)
- [Unit RGB LED Strip - Document & Datasheet](https://docs.m5stack.com/en/unit/rgb_led_strip)
- [Unit Neco - Document & Datasheet](https://docs.m5stack.com/en/unit/Neco%20Unit)
- [Unit Puzzle - Document & Datasheet](https://docs.m5stack.com/en/unit/Unit-Puzzle)
- [M5AtomMatrix - Document & Datasheet](https://docs.m5stack.com/en/core/ATOM%20Matrix)


### Required Libraries:
- [M5UnitUnified](https://github.com/m5stack/M5UnitUnified)
- [M5Utility](https://github.com/m5stack/M5Utility)
- [M5HAL](https://github.com/m5stack/M5HAL)

## License

- [M5Unit-RGB - MIT](LICENSE)

### Support via [PbHub](https://docs.m5stack.com/en/unit/pbhub_1.1)

|Unit|Support|
|---|---|
|UnitRGB|OK|
|UnitRGBLED|OK|
|UnitHex|OK|
|UnitNeoHex|OK|
|UnitRGBLEDStrip|OK|
|UnitNeco|LED only OK, Button function NG||
|UnitPuzzle|OK|
|UnitAtomMatrixRGB|NG|

See also [M5Unit-HUB](https://github.com/m5stack/M5Unit-HUB)

#### Limitations via PbHub
- Each channel can control up to **MAX 74 LEDs each**
- **Button status of UnitNeco** cannot be retrieved

## Examples
See also [examples/UnitUnified](examples/UnitUnified)

### Doxygen document
[GitHub Pages](https://m5stack.github.io/M5Unit-RGB/)

If you want to generate documents on your local machine, execute the following command

```
bash docs/doxy.sh
```

It will output it under docs/html  
If you want to output Git commit hashes to html, do it for the git cloned folder.

#### Required
- [Doxyegn](https://www.doxygen.nl/)
- [pcregrep](https://formulae.brew.sh/formula/pcre2)
- [Git](https://git-scm.com/) (Output commit hash to html)
