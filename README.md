<h1 align=center>
  <img src="src/build/resources/assets/logo_1080.png" width=192 alt="CMake Template">
  <br/>
  CMake Template
</h1>

[![Build](https://github.com/pugur523/cmake_template/actions/workflows/build.yml/badge.svg)](https://github.com/pugur523/cmake_template/actions/workflows/build.yml)
[![Issues](https://img.shields.io/github/issues/pugur523/cmake_template.svg)](https://github.com/pugur523/cmake_template/issues)
[![License](https://img.shields.io/badge/License-Apache%20License%20Version%202.0-red)](LICENSE)
[![C](https://img.shields.io/badge/C-blue?logo=c)](https://www.c-language.org/)
[![C++](https://img.shields.io/badge/C++-blue?logo=cplusplus)](https://isocpp.org/)
[![LLVM](https://img.shields.io/badge/LLVM-21-orange?logo=llvm)](https://llvm.org/docs/index.html)
[![CMake](https://img.shields.io/badge/CMake-4.0.2+-green?logo=cmake)](https://cmake.org/)
<br/>
[![日本語の説明はこちら](https://img.shields.io/badge/日本語の説明はこちら-blue)](README_ja.md)

This is a template that integrates all the features required for CMake-based projects.
It leverages LLVM tools to provide a lightweight, highly optimized C/C++ build system and comprehensive analysis features.

<details open>
  <summary>
    <h2>
      Table of Contents
    </h2>
  </summary>

- [Features](#features)
- [Built-in Libraries](#built-in-libraries)
- [Getting Started](#getting-started)
  - [Install Dependencies](#install-dependencies)
  - [Run Build Script](#run-build-script)
- [How to Customize](#how-to-customize)
  - [Project Configuration](#project-configuration)
  - [How to Manage Third-party Libraries](#how-to-manage-third-party-libraries)
  - [How to Add a Module](#how-to-add-a-module)
  - [Customize Build Arguments](#customize-build-arguments)
  - [Profile Performance of Your Program](#profile-performance-of-your-program)
- [Distributing Build Artifacts](#distributing-build-artifacts)
- [Projects Using This Template](#projects-using-this-template)

</details>

## Features

- **Fast and sophisticated builds**: Optimized and fast builds using tools like clang, lld, ninja, and optimization analysis with llvm-opt-viewer
- **Test integration**: Powerful test suite with Google Test
- **Profiling**: Detailed performance analysis with llvm-xray, llvm-cov, address sanitizer, etc.
- **Cross-platform**: Supports Windows, Linux, and MacOS
- **Ready to start development immediately**: Minimal external libraries and a simple core module are preconfigured
- **Rich development tools**: Includes configs for clang-tidy, clangd, clang-format, cpplint, and VSCode settings

## Built-in Libraries

The following libraries are included as submodules by default so you can start development right away.

|                        Library Name                         | Overview                                               |
| :---------------------------------------------------------: | :----------------------------------------------------- |
| [**Google Benchmark**](https://github.com/google/benchmark) | Optimal benchmark framework for performance profiling  |
|   [**Google Test**](https://github.com/google/googletest)   | Powerful test suite framework                          |
|                [**zlib**](https://zlib.net/)                | Ideal for compression/decompression of log files, etc. |
|      [**toml11**](https://github.com/ToruNiina/toml11)      | TOML parser/reader, ideal for configs                  |

If you do not need these libraries, please refer to [How to Manage Libraries](#how-to-manage-libraries) and remove them from your project.<br/>

## Getting Started

Click the **Use this template** button on this repository to create a new repository.
Then, clone the created repository with submodules using the following command:

```shell
$ git clone --recursive https://github.com/<your_username>/<repository_name>.git && cd <repository_name>
```

For detailed setup and build instructions, see `//.github/workflows/build.yml` and `//src/build/docker/*.dockerfile`, but the basic steps are also described here.

### Install Dependencies

> The dependencies required for building are listed below. The required dependencies and their versions differ by platform, so please check and install accordingly.
> Step-by-step guides for Windows, Ubuntu, ArchLinux, and MacOS are provided below.

**Common**

|                      Name                       | Version  |
| :---------------------------------------------: | :------- |
| [**Python**](https://www.python.org/downloads/) | >= 3.11  |
|         [**CMake**](https://cmake.org/)         | >= 4.0.2 |


**LLVM related**

  * **LLVM tools version**:

  | Platform | Version   |
  | :------: | :-------- |
  |  Linux   | >= 21.0.0 |
  | Windows  | >= 20.1.4 |
  |  MacOS   | >= 19     |


  * **Tools**

  | LLVM Tool Name |  Platforms   |
  | :------------: | :----------: |
  |     clang      |     All      |
  |    clang++     |     All      |
  |    clang-cl    |   Windows    |
  |     clangd     |     All      |
  |   clang-tidy   |     All      |
  |      lld       |     All      |
  |    llvm-ar     |     All      |
  |  llvm-ranlib   |     All      |
  |    llvm-nm     |     All      |
  |  llvm-objcopy  |     All      |
  |  llvm-objdump  |     All      |
  |   llvm-strip   |     All      |
  |  llvm-config   | Linux, MacOS |
  |    llvm-cov    |     All      |
  |   llvm-xray    | Linux, MacOS |
  |   libunwind    | Linux, MacOS |
  |     libc++     | Linux, MacOS |
  |   libc++abi    | Linux, MacOS |


**MinGW related (cross compilation for Windows)**

|                           Name                           | Version     |
| :------------------------------------------------------: | :---------- |
| [**LLVM-MinGW**](https://github.com/mstorsjo/llvm-mingw) | >= 20250528 |
|           [**Wine**](https://www.winehq.org/)            | >= 6.0.3    |


**Both Windows and MinGW**

|                       Name                        | Version |
| :-----------------------------------------------: | :------ |
| [**NSIS**](https://nsis.sourceforge.io/Main_Page) | >= 3.08 |


**Windows only**

|                                Name                                 | Version    |
| :-----------------------------------------------------------------: | :--------- |
|      [**WiX Toolset**](https://www.firegiant.com/wixtoolset/)       | >= 6.0.1   |
| [**Visual Studio 2022**](https://visualstudio.microsoft.com/ja/vs/) | >= 17.13.3 |

After installing all the dependencies above, make sure they are in your PATH.

<details open>
  <summary>
    <h3>
      Setup on Windows
    </h3>
  </summary>

1. Install Chocolatey

    Download and install Chocolatey from the [official page](https://chocolatey.org/).

2. Use Chocolatey to install CMake, LLVM, python, NSIS, WiX Toolset, and ninja

    ```shell
    $ choco install cmake --version=4.0.2 --installargs 'ADD_CMAKE_TO_PATH=System' -y
    $ choco install llvm --version=20.1.4 -y
    $ choco install python3 nsis wixtoolset ninja -y
    ```

3. Install Visual Studio

    Download and install from the [Visual Studio download page](https://visualstudio.microsoft.com/ja/downloads/?cid=learn-onpage-download-install-visual-studio-page-cta), and make sure **x64 Native Tools Command Prompt for VS 2022** is available.

</details>

<details open>
  <summary>
    <h3>
      Setup on Ubuntu
    </h3>
  </summary>

1. Install basic packages

    ```shell
    $ sudo apt-get update && sudo apt-get install -y \
        wget \
        curl \
        gnupg \
        lsb-release \
        software-properties-common \
        ca-certificates \
        git \
        python3.13 \
        ninja-build
    ```

2. Install CMake

    ```shell
    $ CMAKE_VERSION="4.0.2"
    $ CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh"
    $ wget -nv "${CMAKE_URL}" -O cmake-installer.sh
    $ chmod +x cmake-installer.sh
    $ sudo ./cmake-installer.sh --skip-license --prefix=/usr/local
    $ rm cmake-installer.sh
    ```

3. Install LLVM

    ```shell
    $ LLVM_VERSION="21"
    $ wget -O llvm.sh https://apt.llvm.org/llvm.sh
    $ chmod +x llvm.sh
    $ sudo ./llvm.sh ${LLVM_VERSION} all
    $ rm llvm.sh
    $ sudo apt-get install -y libunwind-${LLVM_VERSION}-dev
    $ sudo ln -s /usr/bin/clang-${LLVM_VERSION} /usr/bin/clang 
    $ sudo ln -s /usr/bin/clang++-${LLVM_VERSION} /usr/bin/clang++ 
    $ sudo ln -s /usr/bin/clang-tidy-${LLVM_VERSION} /usr/bin/clang-tidy 
    $ sudo ln -s /usr/bin/clang-format-${LLVM_VERSION} /usr/bin/clang-format 
    $ sudo ln -s /usr/bin/llvm-config-${LLVM_VERSION} /usr/bin/llvm-config 
    $ sudo ln -s /usr/bin/llvm-cov-${LLVM_VERSION} /usr/bin/llvm-cov 
    $ sudo ln -s /usr/bin/llvm-profdata-${LLVM_VERSION} /usr/bin/llvm-profdata 
    $ sudo ln -s /usr/bin/llvm-xray-${LLVM_VERSION} /usr/bin/llvm-xray 
    ```

4. Install LLVM-MinGW / NSIS / Wine (only if cross-compiling for Windows with MinGW)

    ```shell
    $ sudo dpkg --add-architecture i386
    $ sudo apt-get update && sudo apt-get install -y nsis wine wine32
    $ source ./src/build/scripts/install_llvm_mingw.sh
    $ echo "LLVM_MINGW_DIR=${LLVM_MINGW_DIR}" >> $HOME/.bashrc
    ```

</details>

<details open>
  <summary>
    <h3>
      Setup on ArchLinux
    </h3>
  </summary>

1. Install basic packages

    ```shell
    $ sudo pacman -Syu --noconfirm \
    $ sudo pacman -S --noconfirm \
        base-devel \
        git \
        wget \
        curl \
        gnupg \
        python \
        python-pip \
        ninja \
        nasm \
        yasm \
        pkgconf \
        openssl \
        llvm \
        clang \
        clang-tools-extra \
        lld \
        llvm-libs \
        cmake
    ```

2. Install libc++-with-libunwind (via AUR)

    ```shell
    $ yay -S --noconfirm libc++-with-libunwind
    ```

</details>

<details open>
  <summary>
    <h3>
      Setup on MacOS
    </h3>
  </summary>

1. Install LLVM / CMake

    ```shell
    $ LLVM_VERSION="19"
    $ PYTHON_VERSION="3.13"
    $ brew update
    $ brew install --formula cmake llvm@${LLVM_VERSION} lld@${LLVM_VERSION} python@${PYTHON_VERSION}

    $ LLVM_PREFIX=$(brew --prefix llvm@${LLVM_VERSION})
    $ echo "PATH=${LLVM_PREFIX}/bin:$PATH" >> ~/.bashrc
    $ export PATH="${LLVM_PREFIX}/bin:$PATH"

    $ LLD_PREFIX=$(brew --prefix lld@${LLVM_VERSION})
    $ echo "PATH=${LLD_PREFIX}/bin:$PATH" >> ~/.bashrc
    $ export PATH="${LLD_PREFIX}/bin:$PATH"
    ```

</details>

<details open>
  <summary>
    <h3>
      Common steps for all environments
    </h3>
  </summary>

1. Install required Python modules

    ```shell
    $ python3 -m pip install --upgrade pip setuptools wheel
    $ python3 -m pip install -r ./src/build/scripts/requirements.txt
    ```

</details>

### Run Build Script

```shell
$ python3 ./src/build/scripts/build.py
```
Running the above command will perform a debug build for the same platform/CPU architecture as your build environment.<br/>
On Windows, you need to build in the **x64 Native Tools Command Prompt for VS 2022** because `vcvars64.bat` must be used to set up environment variables for x64.
The build target will be generated in `//out/build/<platform>/<arch>/debug`, and the install target in `//out/install/<platform>/<arch>/debug`.<br/>
If submodules are not set up correctly, you will get an error, so please initialize all submodules with `git submodule update --init --recursive` and try again.

## How to Customize

### Project Configuration

* Edit each item in `//src/project_config.toml`. You may refer to the example below.

<details open>
  <summary>
    <h3>
      Example project_config.toml
    </h3>
  </summary>

```toml
[project]
name = "CMakeTemplate"
main_executable_name = "cmake_template"
version = "1.2.3"
description = "a all-in-one cmake template"
homepage = "https://github.com/pugur523/cmake_template"
c_version = "17"
cxx_version = "17"
author = "pugur"
author_email = "pugurmc@gmail.com"
```

</details>

### How to Manage Third-party Libraries

* To add a library that is available as a git repository and supports CMake

Add the library as a submodule under `//src/third_party/`, and add a setup macro for the library in `//src/build/cmake/libraries.cmake`. The macro should handle build options, include directories, link directories, link libraries, and add_subdirectory(). Call the added `setup_<library_name>` macro from `//src/CMakeLists.txt`. You can refer to the setup code for the default libraries. For each library, define variables like `<LIBNAME>_INCLUDE_DIRS` and add them to project variables such as `${PROJECT_INCLUDE_DIRECTORIES}` in `//src/CMakeLists.txt`.

> It is recommended to add libraries as git submodules and use add_subdirectory, rather than using pre-built binary packages or CMake's `ExternalProject_Add` / `find_package`. By unifying the build environment for all libraries, you can benefit from better optimization, easier patching, and immediate access to source code.

* To add a library that is not available as a git repository or does not support CMake

If possible, write a dedicated setup script in `//src/build/scripts` and call it from `//src/build/scripts/build.py`. If not, you may install it locally, but you will need to modify CI elements such as `//.github/workflows/build.yml` for all platforms, so this is not recommended.

* To remove a library

Reverse the above steps and remove all related parts for that library.

### How to Add a Module

The [core](src/core) module is included by default. To add a new module, create a directory under `//src/`, copy the [CMakeLists.txt](src/core/CMakeLists.txt) from core, change `MODULE_NAME` and `MODULE_OBJECTS_NAME`, and set the list of source files in `SOURCES` as needed. By changing the arguments passed to `setup_module`, you can set include directories, link directories, compile options, link options, and link libraries for the module. You can also specify whether to build the library as static or shared with `"-D ENABLE_BUILD_SHARED=<true|false>"` and `"-D BUILD_<UPPER_MODULE_NAME>_SHARED=<true|false>"`. For example, to build core as a static library, use `"-D BUILD_CORE_SHARED=false"`.

### Customize Build Arguments

```shell
$ python3 ./src/build/scripts/build.py -h
```
The above command will output help for arguments. It doesn't contain some aliases such as `--debug`, `--release`. Here are some common usage examples:

<details open>
  <summary>
    Perform a release build for the same platform/CPU architecture as your build environment
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --release
```
or
```shell
python3 ./src/build/scripts/build.py \
    --build_mode=release
```

</details>

<details open>
  <summary>
    On Linux, build both debug and release types, and build for both Linux natively and Windows via MinGW cross-compilation
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --all \
    --target_platform=linux,windows
```

</details>

<details open>
  <summary>
    Perform a debug build with LLVM-XRay enabled instead of Asan
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --debug \
    -- "-D ENABLE_SANITIZERS=false,-D ENABLE_XRAY=true
```
or
```shell
python3 ./src/build/scripts/build.py \
    --debug \
    --extra_args="-D ENABLE_SANITIZERS=false,-D ENABLE_XRAY=true
```
> Arguments after `--` will be passed directly to the `--extra_args`

</details>

<details open>
  <summary>
    On Linux, build debug type and obtain various optimization-related information during the build
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --all \
    -- "-D ENABLE_SANITIZERS=false,-D ENABLE_XRAY=true,-D ENABLE_COVERAGE=true,-D ENABLE_OPTIMIZATION_REPORT=true,-D ENABLE_LTO=true"
```

</details>

<details open>
  <summary>
    Only build, do not install or package
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --no-install \
    --no-package
```

</details>

<details open>
  <summary>
    After building, run the app and tests and profile with llvm-xray and llvm-cov
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    -- "-D ENABLE_XRAY=true,-D ENABLE_SANITIZERS=false,-D ENABLE_COVERAGE=true,-D ENABLE_RUN_APP_POST_BUILD=true,-D ENABLE_RUN_TESTING_POST_BUILD=true"
```

</details>

### Profile Performance of Your Program

As shown in the [Customize Build Arguments](#customize-build-arguments) section, if you specify the appropriate build arguments, the following will be generated in `//out/build/<platform>/<arch>/<build_type>`:

* `coverage/<project_name>/html/index.html`
* `opt_report/index.html`
* `xray/<project_name>/xray_trace.<project_name>.json`

You can view `index.html` in your browser with a GUI, and open `xray_trace.<project_name>.json` in Chromium-based browsers via [about:tracing](chrome://tracing/). For details, refer to the official documentation for llvm-cov, llvm-opt-viewer, llvm-xray, and [Chromium's trace event profiling tool documentation](https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/).

## Distributing Build Artifacts
The build artifacts are placed in the `//out/install/<platform>/<arch>/<build_type>` directory.
Additionally, installers packaged from these artifacts are generated in the `//out/build/<platform>/<arch>/<build_type>/package` directory.

To distribute your application, release the installer files such as `.exe`, `.zip`, `.tar.gz`, `.deb`, or `.dmg` found in the latter directory.
Users can download the installer appropriate for their environment and install the build artifacts using the standard method for each format.

## Projects Using This Template
|                   Project Name                   | Description                       |
| :----------------------------------------------: | :-------------------------------- |
| [femtolog](https://github.com/pugur523/femtolog) | An ultra fast c++ logging library |
