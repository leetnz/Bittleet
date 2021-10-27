# Bittleet
[![Bittleet CI](https://github.com/leetnz/Bittleet/actions/workflows/actions.yml/badge.svg)](https://github.com/leetnz/Bittleet/actions/workflows/c-cpp.yml)

A fork of [Petoi OpenCat](https://github.com/PetoiCamp/OpenCat) but only focusses on Bittle.

The motivation of `Bittleet` is to break the original `OpenCat` firmware into manageable chunks so that functionality can be extended without breaking other functionality.

This is a work in progress.

## Running Tests

Requires `g++` and `make`. Run tests with:

```
make test
```

## Interfacing with Bittleet

* [Bittleet Serial Protocol](https://github.com/leetnz/Bittleet/wiki/Bittleet-Communication-Protocol)


# TODO

- [ ] Document Installation Dependencies
- [ ] Implement `PixelStatusReporter` to use the neopixels to communicate Bittle status 
- [ ] Make Skill commands their own Command category - will consume a little extra RAM
- [ ] Make serial output a `StatusReporter`
    - [ ] Add Command Skills as a type of status report
- [ ] Move IMU specific functionality out of `Bittle.ino`
- [ ] Motion behavior reads from internal EEPROM for calibration
    - [ ] Need to benchmark EEPROM reads
    - [ ] Ideally, we just store these params in RAM/ROM

## Wishlist
- [ ] Add arduino integration to github actions
    - https://github.com/arduino/arduino-cli-example
    - Would like to see binary size/RAM - set some thresholds to fail when this goes over a threshold
- [ ] Implement a binary protocol for executing skills, getting petoi status
- [ ] Bittle navigation integration with ROS would be nice, we would need twist messages... 
    - Might want to consider how we can determine that data...
- [ ] Make `Comms` implement `SkillLoader` so we can load skills over Serial


