![Valentine](docs/valentine_screenshot.png)

[![](https://github.com/Tote-Bag-Labs/valentine/actions/workflows/cmake_ctest.yml/badge.svg)](https://github.com/Tote-Bag-Labs/valentine/actions/workflows/cmake_ctest.yml)


V A L E N T I N E
==================
Valentine is a compressor and distortion processor. It was inspired by the hyper compressed
and crushed textures in the seminal Justice record, †. Using it is easy: turning input up makes the signal
louder, more compressed, and more saturated. Turning crush up adds digital distortion.

The real fun is in processing signal with some ambience, be it a room or a reverb. With the right input gain
and release settings, you can introduce pumping and breathing artifacts.

Getting Valentine
=================
Download the latest precompiled binaries [here](https://github.com/Tote-Bag-Labs/valentine/releases/latest).

AU and VST3 on MacOs and VST3 on Windows are currently supported.

Using Valentine
===============

Signal Path
----------
Valentine is a non-linear processor. As such, knowing the signal path makes it easier
to get your desired result. Here it is:

**Crush** -> **Input** -> Compression -> **Saturation** -> Soft Clip -> **Output** -> **Mix**

Parameters
----------
- **Crush**: increases the amount of bit crushing. Downsamples the signal to 27.5kHz for any setting greater than 0%.
- **Input**: sets the gain before compresison. Use this to increase the amount of compression and distortion affecting the signal.
- **Ratio**: sets the compression ratio. "Infinity" is more like 1000:1. Increasing this also increases the threshold and decreases the knee.
- **Attack**: sets compression attack time.
- **Release**: sets compression release time.
- **Saturation**: increases the amount of saturation.
- **Output**: sets the wet signal's output gain before mix.
- **Mix**:  increases the amount of wet signal in the processor's output.
- **Bypass**: bypasses all processing.

**[Secret Parameter]**
- **Nice**: raises the compressor's threshold when "on". Find this in your DAW's automation section or wherever the parameter's can be exposed
in that DAW's format. I removed this from the UI because it was hard to reason about / communicate the purpose of visually but forgot to
actually remove the parameter. Then some friends said "what happened to Nice?". So I'm letting you know about it now. This may be removed in the 1.0.0 release


Building Valentine
==================

Xcode
-----

Currently, `Xcode 14.2` is supported for building Valentine.

Here's how to create an Xcode project for building the plugin.

```
git clone https://github.com/Tote-Bag-Labs/valentine.git
cd valentine
git submodule update --init --recursive
cmake -B builds -G Xcode
```

Other (for now)
---------------

If you'd rather just build it from the command line, run
`cmake --build Builds --config Release`


Contributing to Valentine
=========================

I'm currently focusing on getting this project off the ground, and as such have limited capacity to review PRs.
There are exceptions:

- I would like to offer Valentine on Linux, but haven't the set up to see why it's not currently working.

- Valentine has 0 unit test coverage. I would love to change that.


TODO
====

- Get it working on Linux

- Write tests

- Code cleanup

- Decide on final parameter set and ranges

- Algorithm tweaks

- Write an instruction manual

- Finalize UI
    - Colors
    - Text size/positioning
    - Component size/placement
    - Logos

- Windows installer signing
