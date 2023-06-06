# Configurable Task Manager

![C++](https://img.shields.io/badge/C%2B%2B-11%2F14%2F17%2F20%2F23-blue)
![License](https://camo.githubusercontent.com/890acbdcb87868b382af9a4b1fac507b9659d9bf/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f6c6963656e73652d4d49542d626c75652e737667)
![Linux Build](https://github.com/franneck94/CppProjectTemplate/workflows/Ubuntu%20CI%20Test/badge.svg)
[![codecov](https://codecov.io/gh/franneck94/CppProjectTemplate/branch/master/graph/badge.svg)](https://codecov.io/gh/franneck94/CppProjectTemplate)

* Main goal of this project is to provide a program that is capable of\
reading tasks from config file\
and execute those task periodically using most recent c++ features.\
The project export the data it creates into shared memory (currently implemented using boost/ManageSharedMemory)\
and allows python to access the shared memory by exprting via pybind11.\
It is on going tasks. please checkout out pull request for next action items.

This is a C++ projects based on cmake. What you get:


- Utils Library, whichexecutable and test code separated in distinct folders
- Use of modern CMake for building and compiling
- External libraries installed and managed by
  - CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) OR
  - [Conan](https://conan.io/) Package Manager OR
  - [VCPKG](https://github.com/microsoft/vcpkg) Package Manager
- Unit testing using [Catch2](https://github.com/catchorg/Catch2)
- Google testing using [GoogleTest](https://github.com/google/googletest.git)
- Yaml parser  using [yaml-cpp] (https://github.com/jbeder/yaml-cpp.git)
- General purpose libraries: [JSON](https://github.com/nlohmann/json), [spdlog](https://github.com/gabime/spdlog), [cxxopts](https://github.com/jarro2783/cxxopts) and [fmt](https://github.com/fmtlib/fmt)
- Continuous integration testing with Github Actions and [pre-commit](https://pre-commit.com/)
- Code coverage reports, including automatic upload to [Codecov](https://codecov.io)
- Code documentation with [Doxygen](https://doxygen.nl/) and [Github Pages](https://franneck94.github.io/CppProjectTemplate/)

## Structure

``` text
├── app
│   ├── CMakeLists.txt
│   ├── InteractiveTask.h
│   ├── main.cc
│   ├── NetFunctionFactory.h
│   ├── TasksFromConfig.cpp
│   ├── TasksFromConfig.h
│   ├── test.json
│   └── utils
│       ├── CMakeLists.txt
│       ├── DB.cpp
│       ├── DB.h
│       ├── global_settings.h
│       ├── PeriodicTask.cpp
│       ├── PeriodicTask.h
│       ├── PriorityQ.h
│       ├── PriorityQueueElement.h
│       ├── SharedMemoryWrapper.cpp
│       ├── SharedMemoryWrapper.h
│       ├── SimpleBoostSharedMemory.cpp
│       ├── SimpleBoostSharedMemory.h
│       ├── Storage.h
│       ├── thread_safe_queue.h
│       ├── ThreadUtil.cpp
│       └── ThreadUtil.h
├── FormatterAnalyzer.md
├── LICENSE
├── Makefile
├── python
│   └── access_data.py
├── README.md
├── src
│   ├── CMakeLists.txt
│   ├── my_lib
│   │   ├── CMakeLists.txt
│   │   ├── my_lib.cc
│   │   └── my_lib.h
│   └── yaml_api
│       ├── CMakeLists.txt
│       ├── text.yaml
│       ├── yaml_api.cpp
│       └── yaml_api.h
├── tests
│   ├── CMakeLists.txt
│   ├── gtest_database.cpp
│   ├── gtest_priority_queue.cpp
│   ├── gtest_scheduled_task.cpp
│   ├── gtest_simple_boost_shared_memory.cpp
│   ├── gtest_yaml_api.cpp
│   ├── main.cc
│   └── text.yaml
├── tools
│   ├── iwyu_tool.py
│   ├── run-clang-format.py
│   └── run-clang-tidy.py
├── VCPKG_Install.md
└── vcpkg.json

```

Library code goes into [src/](src/), main program code in [app/](app) and tests go in [tests/](tests/).

## Software Requirements

- CMake 3.16+
- GNU Makefile
- Doxygen
- Conan or VCPKG
- MSVC 2017 (or higher), G++9 (or higher), Clang++9 (or higher)
- Code Coverage (only on GNU|Clang): lcov, gcovr

## Building

First, clone this repo and do the preliminary work:

```shell
git clone --recursive https://github.com/franneck94/CppProjectTemplate
make prepare
```

- App Executable

```shell
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target main
cd app
./main
```

- Unit testing

```shell
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug --target unit_tests
cd bin
./unit_tests
```

- Documentation

```shell
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug --target docs
```

- Code Coverage (Unix only)

```shell
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
cmake --build . --config Debug --target coverage
```

For more info about CMake see [here](./CMakeGuide.md).
