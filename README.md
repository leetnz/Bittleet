# Bittleet
[![Bittleet CI](https://github.com/leetnz/Bittleet/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/leetnz/Bittleet/actions/workflows/c-cpp.yml)

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

# feature/rollpitch branch

- [ ] Bug - On recovery, bittle will use an old stale angle measurement once it has recovered 
    - need to look at how we want to manage angles after recovery 
        - perhaps just let bittle chill for a little while when recovered?
    - this is likely to occur randomly during gait too
        - will want to figure out how to decay trust in stale measurements?
- [ ] Add a scheduler - Bittle should not be operating at an arbitrary update rate - it should be updated at a known frequency
- [ ] Update multiple joints in sync - this seems like an obvious win for update rates.


# TODO

- [ ] Document Installation Dependencies
- [ ] Implement `PixelStatusReporter` to use the neopixels to communicate Bittle status 
- [ ] Make Skill commands their own Command category - will consume a little extra RAM
- [ ] Make serial output a `StatusReporter`
    - [ ] Add Command Skills as a type of status report
- [ ] Move EEPROM specific functionality out of OpenCat/Bittle.ino
    - [ ] Create an interface `SkillLoader` and structure `Skill` (this becomes effectively the motion data type)
    - [ ] Create `EepromConfig` which implements `SkillLoader`
    - [ ] Create `ProgmemSkillLoader` which implements `SkillLoader`
- [ ] Move IMU specific functionality out of `Bittle.ino`
- [ ] Bittle DOF is 9 under normal circumstances. Consider making DOF matrixes only 9 
    - will reclaim 10 bytes of RAM per servo, so 70 bytes doing this.

## Wishlist
- [ ] Add arduino integration to github actions
    - https://github.com/arduino/arduino-cli-example
    - Would like to see binary size/RAM - set some thresholds to fail when this goes over a threshold
- [ ] Implement a binary protocol for executing skills, getting petoi status
- [ ] Bittle navigation integration with ROS would be nice, we would need twist messages... 
    - Might want to consider how we can determine that data...
- [ ] Make `Comms` implement `SkillLoader` so we can load skills over Serial


