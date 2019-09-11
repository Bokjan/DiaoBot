# DiaoBot
Group Bot (群机器人) framework for Wework (企业微信). Linux only (tested on tlinux 2.2). 

# Prerequisites
- CMake 3.1 +
- C++ compiler with C++11 support
- `libcurl`
- `RapidJSON`

# Build
Run `build.sh`

# Architecture
Framework contains `libdiaobot` (library) and `diaobotd` (executable). Your logic should be implemented in several dynamic-link libraries, and `diaobotd` will load them. See `Sample` for hints. 

# Run
`diaobotd`, `libdiaobot.so` and a config file (for example, `sample.conf` in `Dist`) are the minimum requirements. To run your code, add your own `.so`s into config file. 