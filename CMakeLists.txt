project(winmailtest)
cmake_minimum_required(VERSION 3.0)

include(FindPkgConfig)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -U__STRICT_ANSI__ -Wall -Wextra -Wpedantic -Wshadow -Wuninitialized -Woverloaded-virtual -fpermissive")

if (NOT WIN32)
  message(FATAL_ERROR "This program only works on Windows.")
endif()

pkg_check_modules(GLIB glib-2.0)

set(QT_MIN_VERSION 5.9)
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Widgets AxContainer)
set(QT_LIBRARIES Qt5::Core Qt5::Widgets Qt5::AxContainer)

option(ENABLE_WIN32_CONSOLE "Show the windows console even outside Debug mode" OFF)

add_subdirectory(src)
