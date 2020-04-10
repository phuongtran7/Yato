# Yato

This is a simple `TOML` to `YAML` converter. Currently, only support converting [Ditto](https://github.com/phuongtran7/Ditto_ActiveMQ)'s configuration file into `YAML` format. This is done due to the verbosity of current Ditto's `TOML` config. The more datarefs the more redundant text there is.

## Dependencies
1. Stand-alone ASIO ([https://think-async.com/Asio/AsioStandalone.html](https://think-async.com/Asio/AsioStandalone.html)).
2. {fmt} ([https://github.com/fmtlib/fmt](https://github.com/fmtlib/fmt)).
3. FlatBuffers ([https://github.com/google/flatbuffers](https://github.com/google/flatbuffers))

## Building
1. Install all dependencies with Microsoft's [vcpkg](https://github.com/Microsoft/vcpkg).
    * `vcpkg install cpptoml`
    * `vcpkg install yaml-cpp`
    * `vcpkg install fmt`
2. Clone the project: `git clone https://github.com/phuongtran7/Yato`.
3. Build the project:
```
cd Yato
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake ..
cmake --build .
```

## Usage
1. Copy the `.toml` file next to the binary.
2. Execute it.