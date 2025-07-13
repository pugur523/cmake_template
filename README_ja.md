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
[![English Readme](https://img.shields.io/badge/English%20Readme-blue)](README.md)

CMakeベースのプロジェクトに必要な機能をすべて統合したテンプレートです。
LLVM系統のツールを活用し、軽量で高度に最適化されたC/C++ビルドシステムと包括的な分析機能を提供します。

<details open>
  <summary>
    <h2>
      Table of Contents
    </h2>
  </summary>

- [このテンプレートの特長](#このテンプレートの特長)
- [組み込み済みのライブラリ](#組み込み済みのライブラリ)
- [はじめる](#はじめる)
  - [依存関係のインストール](#依存関係のインストール)
  - [ビルドスクリプトの実行](#ビルドスクリプトの実行)
- [カスタマイズ方法](#カスタマイズ方法)
  - [プロジェクト設定](#プロジェクト設定)
  - [外部ライブラリ管理](#外部ライブラリ管理)
  - [モジュール追加方法](#モジュール追加方法)
  - [ビルド引数のカスタマイズ](#ビルド引数のカスタマイズ)
  - [パフォーマンスプロファイリング](#パフォーマンスプロファイリング)
- [ビルド成果物の配布](#ビルド成果物の配布)

</details>

## このテンプレートの特長

- **高速で洗練されたビルド**: clang, lld, ninjaなど各種ツールによる最適化高速ビルドと、llvm-opt-viewerによる最適化分析
- **テスト統合**: Google Test による強力なテストスイート
- **プロファイリング**: llvm-xray、llvm-cov、address sanitizerなどによる詳細なパフォーマンス分析
- **クロスプラットフォーム**: Windows、Linux、MacOS 対応
- **即座に開発を始められる**: 必要最低限の外部ライブラリと簡易coreモジュールを事前構成済み
- **充実した開発補助ツール**: clang-tidy, clangd, clang-format, cpplintなどのコード補助ツールのconfigとvscode向け設定ファイルを同梱


## 組み込み済みのライブラリ

すぐに開発を始められるように、デフォルトで以下のライブラリがサブモジュールとして組み込まれています。

|                        Library Name                         | Overview                                                         |
| :---------------------------------------------------------: | :--------------------------------------------------------------- |
| [**Google Benchmark**](https://github.com/google/benchmark) | パフォーマンスプロファイリングに最適なベンチマークフレームワーク |
|   [**Google Test**](https://github.com/google/googletest)   | 強力なテストスイートフレームワーク                               |
|                [**zlib**](https://zlib.net/)                | ログファイルなどの圧縮/解凍に最適                                |
|      [**toml11**](https://github.com/ToruNiina/toml11)      | configなどに最適なTOMLパーサー/リーダー                          |

これらのライブラリが必要ない場合には[外部ライブラリ管理](#外部ライブラリ管理)を参照のもと、プロジェクトから削除してください。<br/>

## はじめる

このリポジトリの**Use this template**ボタンをクリックし、新しいリポジトリを作成してください。
次に、作成したリポジトリを以下のコマンドでサブモジュールごとクローンします。

```shell
$ git clone --recursive https://github.com/<your_username>/<repository_name>.git && cd <repository_name>
```

環境構築とビルド手順は、`//.github/workflows/build.yml`や`//src/build/docker/*.dockerfile`に詳しく記載されていますが、ここでも基本的な手順を説明します。

### 依存関係のインストール

> ビルドに必要な依存関係を以下に記します。プラットフォームごとに必要な依存関係とそのバージョンが異なるので、確認してインストールしてください。
> Windows, Ubuntu, ArchLinux, MacOSでの環境構築方法についてはStep-by-Stepなガイドを後述しています。


**全プラットフォーム共通**

|                    ツール名                     | バージョン |
| :---------------------------------------------: | :--------- |
| [**Python**](https://www.python.org/downloads/) | >= 3.11    |
|         [**CMake**](https://cmake.org/)         | >= 4.0.2   |


**LLVM関連**

  * **LLVM関連のツールのバージョン**:

  | プラットフォーム | バージョン |
  | :--------------: | :--------- |
  |      Linux       | >= 21.0.0  |
  |     Windows      | >= 20.1.4  |
  |      MacOS       | >= 19      |


  * **ツール**

  | LLVMツール名 | プラットフォーム |
  | :----------: | :--------------: |
  |    clang     |       All        |
  |   clang++    |       All        |
  |   clang-cl   |     Windows      |
  |    clangd    |       All        |
  |  clang-tidy  |       All        |
  |     lld      |       All        |
  |   llvm-ar    |       All        |
  | llvm-ranlib  |       All        |
  |   llvm-nm    |       All        |
  | llvm-objcopy |       All        |
  | llvm-objdump |       All        |
  |  llvm-strip  |       All        |
  | llvm-config  |   Linux, MacOS   |
  |   llvm-cov   |       All        |
  |  llvm-xray   |   Linux, MacOS   |
  |  libunwind   |   Linux, MacOS   |
  |    libc++    |   Linux, MacOS   |
  |  libc++abi   |   Linux, MacOS   |


**MinGW関連 (Windows向けクロスコンパイルに必要)**

|                         ツール名                         | バージョン  |
| :------------------------------------------------------: | :---------- |
| [**LLVM-MinGW**](https://github.com/mstorsjo/llvm-mingw) | >= 20250528 |
|           [**Wine**](https://www.winehq.org/)            | >= 6.0.3    |


**WindowsとMinGWで共通**

|                     ツール名                      | バージョン |
| :-----------------------------------------------: | :--------- |
| [**NSIS**](https://nsis.sourceforge.io/Main_Page) | >= 3.08    |


**Windowsのみ**

|                              ツール名                               | バージョン |
| :-----------------------------------------------------------------: | :--------- |
|      [**WiX Toolset**](https://www.firegiant.com/wixtoolset/)       | >= 6.0.1   |
| [**Visual Studio 2022**](https://visualstudio.microsoft.com/ja/vs/) | >= 17.13.3 |

上記の依存関係を全てインストールした上で、PATHを通してください。

<details open>
  <summary>
    <h3>
      Windowsでのセットアップ方法
    </h3>
  </summary>

1. Chocolateyのインストール

    [公式ページ](https://chocolatey.org/)からChocolateyをダウンロード、インストールし、使用可能な状態にします。

2. Chocolateyを使用してCMake, LLVM, python, NSIS, WiX Toolset, ninjaをインストール

    ```shell
    $ choco install cmake --version=4.0.2 --installargs 'ADD_CMAKE_TO_PATH=System' -y
    $ choco install llvm --version=20.1.4 -y
    $ choco install python3 nsis wixtoolset ninja -y
    ```

3. Visual Studioのインストール

    [Visual Studioのダウンロードページ](https://visualstudio.microsoft.com/ja/downloads/?cid=learn-onpage-download-install-visual-studio-page-cta)からダウンロード/インストールし、**x64 Native Tools Command Prompt for VS 2022**が使用可能になったことを確認します。

</details>

<details open>
  <summary>
    <h3>
      Ubuntuでのセットアップ方法
    </h3>
  </summary>


1. 基本的なパッケージのインストール

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

2. CMakeのインストール

    ```shell
    $ CMAKE_VERSION="4.0.2"
    $ CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh"
    $ wget -nv "${CMAKE_URL}" -O cmake-installer.sh
    $ chmod +x cmake-installer.sh
    $ sudo ./cmake-installer.sh --skip-license --prefix=/usr/local
    $ rm cmake-installer.sh
    ```

3. LLVMのインストール

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

4. LLVM-MinGW / NSIS / Wineのインストール (MinGWでWindows向けクロスコンパイルを行う場合のみ)

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
      ArchLinuxでのセットアップ方法
    </h3>
  </summary>

1. 基本的なパッケージのインストール

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

2. libc++-with-libunwindのインストール (via AUR)

    ```shell
    $ yay -S --noconfirm libc++-with-libunwind
    ```

</details>

<details open>
  <summary>
    <h3>
      MacOSでのセットアップ方法
    </h3>
  </summary>

1. LLVM / CMakeのインストール

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
      全環境で共通の部分
    </h3>
  </summary>


1. 依存Pythonモジュールのインストール

    ```shell
    $ python3 -m pip install --upgrade pip setuptools wheel
    $ python3 -m pip install -r ./src/build/scripts/requirements.txt
    ```

</details>

### ビルドスクリプトの実行

```shell
$ python3 ./src/build/scripts/build.py
```
上記のコマンドを実行するとビルド環境と同じプラットフォーム / CPUアーキテクチャ向けのdebug buildが行われます。<br/>
Windows上では`vcvars64.bat`によりx64用の環境変数を設定する必要があるため、これを実行するか、**x64 Native Tools Command Prompt for VS 2022**上でビルドを行う必要があります。
build targetは`//out/build/<platform>/<arch>/debug`、install targetは`//out/install/<platform>/<arch>/debug`に生成されます。<br/>
このとき、サブモジュールが正しくセットアップされていない場合はエラーが出るので`git submodule update --init --recursive`で全てのサブモジュールを初期化してから再実行してください。

## カスタマイズ方法

### プロジェクト設定

* `//src/project_config.toml`の各項目を書き換えます。以下の例を参考に設定するとよいかもしれません。
<details open>
  <summary>
    <h3>
      project_config.tomlの例
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

### 外部ライブラリ管理

* git repositoryが入手可能であり、かつそれがCMakeに対応しているライブラリを追加する

サブモジュールとして`//src/third_party/`下にそのライブラリを追加し、`//src/build/cmake/libraries.cmake`にそのライブラリに関するセットアップマクロを追記します。マクロ内に必要な記述は、ビルド関連のオプションのセットアップ、include directoryの追加、link directoryの追加、link librariesの追加、add_subdirectory()のみで完結するようにしてあります。追記した`setup_<library_name>`マクロを`//src/CMakeLists.txt`から呼び出すようにしてください。デフォルトで組み込み済みのライブラリのsetupコードと同様に行えば問題ないはずです。ライブラリごとに`<LIBNAME>_INCLUDE_DIRS`などの変数を設け、それらを`//src/CMakeLists.txt`にて`${PROJECT_INCLUDE_DIRECTORIES}`など、それぞれプロジェクト用変数に追加するようにするとよいです。

> pre-built binary packageやCMake標準の`ExternalProject_Add` / `find_package`なども用いらず、gitサブモジュールを`add_subdirectory`でサブディレクトリとしてプロジェクトに追加することを推奨しています。ビルドシステムの機能に頼らずライブラリのビルド環境も含めてすべて統一することで、より洗練された最適化の恩恵を得られたり、独自パッチの適用がしやすくなったり、すぐにライブラリのソースコードを確認できたりなど、様々なメリットがあると考えています。


* git repositoryが入手可能でない、またはそれがCMakeに対応していないライブラリを追加する

可能なら自前で`//src/build/scripts`の中にそのライブラリ専用のsetup scriptを書いて配置し、`//src/build/scripts/build.py`から呼び出すようにしてください。それが叶わないのであればローカルに直接インストールしてもよいかもしれませんが、`//.github/workflows/build.yml`などのCI要素を全プラットフォーム分修正しなくてはならないため、あまり推奨はできません。


* ライブラリを消去する

上記の工程を逆から辿り、そのライブラリに関連する部分をすべて消去してください。

### モジュール追加方法

[core](src/core)モジュールがデフォルトで組み込まれているので、これと同様に`//src/`内に新たに追加したいモジュールのディレクトリを作成し、coreの[CMakeLists.txt](src/core/CMakeLists.txt)から`MODULE_NAME`と`MODULE_OBJECTS_NAME`をそれぞれ変更し、必要なソースファイルのリストを`SOURCES`に設定しなおしたものを新たなモジュールのディレクトリに追加してください。`setup_module`に渡す引数を変更することでそのモジュールに設定するインクルードディレクトリ、リンクディレクトリ、コンパイルオプション、リンクオプション、リンクライブラリをそれぞれ設定可能です。また、ビルド時にそのライブラリをstatic / sharedのどちらとしてビルドするかは`"-D ENABLE_BUILD_SHARED=<true|false>"`と`"-D BUILD_<UPPER_MODULE_NAME>_SHARED=<true|false>"`で指定可能です。例として、coreをstaticライブラリとしてビルドしたい場合は`"-D BUILD_CORE_SHARED=false"`により実現できます。

### ビルド引数のカスタマイズ

```shell
$ python3 ./src/build/scripts/build.py -h
```
上記のコマンドにより引数に関するヘルプメッセージが出力されますが、ここでは一般的な使用例をいくつか示しておきます。また、このヘルプメッセージに`--debug`や`--release`などの一部のエイリアスは含まれていません。

<details open>
  <summary>
    ビルド環境と同じプラットフォーム / CPUアーキテクチャ向けのrelease buildを行う
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --release
```
または
```shell
python3 ./src/build/scripts/build.py \
    --build_mode=release
```

</details>

<details open>
  <summary>
    Linux上のdebug / release両方のビルドタイプで、LinuxネイティブビルドとMinGWクロスコンパイルによるWindows向けビルドを行う
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --all \
    --target_platforms=linux,windows
```

</details>

<details open>
  <summary>
    debugビルドで、AsanではなくLLVM-XRayを有効化したビルドを行う
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --debug \
    -- "-D ENABLE_SANITIZERS=false,-D ENABLE_XRAY=true"
```
または
```shell
python3 ./src/build/scripts/build.py \
    --debug \
    --extra_args="-D ENABLE_SANITIZERS=false,-D ENABLE_XRAY=true"
```
> `--`以降に指定された引数は`extra_args`に直接渡されます

</details>

<details open>
  <summary>
    Linux上のdebugビルドで、ビルドと同時に色々な最適化関連の情報を得る
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    --all \
    -- "-D ENABLE_SANITIZERS=false,-D ENABLE_XRAY=true,-D ENABLE_COVERAGE=true,-D ENABLE_OPTIMIZATION_REPORT=true,-D ENABLE_LTO=true"
```

</details>

<details open>
  <summary>
    ビルドのみを行い、installとpackageを行わない
  </summary>

```shell
$ python3 ./src/build/scripts/build.py \
    --no-install \
    --no-package
```

</details>

<details open>
  <summary>
    ビルド後にappとテストを実行してllvm-xray, llvm-covによるプロファイリングを行う
  </summary>

```shell
python3 ./src/build/scripts/build.py \
    -- "-D ENABLE_XRAY=true,-D ENABLE_SANITIZERS=false,-D ENABLE_COVERAGE=true,-D ENABLE_RUN_APP_POST_BUILD=true,-D ENABLE_RUN_TESTING_POST_BUILD=true"
```

</details>

### パフォーマンスプロファイリング

[ビルド引数のカスタマイズ](#ビルド引数のカスタマイズ)セクションにして例示したように、適切にビルド時の引数指定を行うと`//out/build/<platform>/<arch>/<build_type>`の中にそれぞれ

* `coverage/<project_name>/html/index.html`
* `opt_report/index.html`
* `xray/<project_name>/xray_trace.<project_name>.json`

が生成されます。`index.html`はブラウザなどでそのファイルを開くことでGUIつきで閲覧することができ、`xray_trace.<project_name>.json`は、Chromiumベースのブラウザで[about:tracing](chrome://tracing/)から開くことができます。細かい仕様についてはLLVMが公開しているllvm-cov、llvm-opt-viewer、llvm-xrayのドキュメントや、Chromiumが公開している[trace event profiling toolのドキュメント](https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/)を確認してください。

## ビルド成果物の配布

ビルドが成功すると、その成果物は`//out/install/<platform>/<arch>/<build_type>`ディレクトリに配置されます。
また、これらをパッケージ化したインストーラーは`//out/build/<platform>/<arch>/<build_type>/package`ディレクトリ内に生成されます。

配布したい場合は、後者のディレクトリにある`.exe`, `.zip`, `.tar.gz`, `.deb`, `.dmg`などの拡張子のインストーラーファイルをリリースしてください。
ユーザーは自らの環境に適したインストーラーをダウンロードし、各形式に応じた一般的な方法でこれらに含まれるビルド成果物をインストールすることができます。
