project(winmailtest)
cmake_minimum_required(VERSION 3.0)

include(FindPkgConfig)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND COMPILE_OPTIONS
  $<$<COMPILE_LANGUAGE:C>:-std=c99>
  $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
  -Wall
  -Wextra
  -Wpedantic
  -Wunused
  -Wshadow
  -Wundef
  -Wuninitialized
  -Wredundant-decls
  -Wcast-align
  -Winit-self
  -Wmissing-include-dirs
  -Wmissing-declarations
  -Wstrict-overflow=2
  -Wunused-parameter
  -Wformat=2
  -Wdisabled-optimization
  $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
  $<$<COMPILE_LANGUAGE:CXX>:-Wno-old-style-cast>
  $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
)

option(BUILD_WERROR "Build with -Werror" OFF)
if(BUILD_WERROR)
  list(APPEND COMPILE_OPTIONS -Werror)
endif(BUILD_WERROR)

add_compile_options(${COMPILE_OPTIONS})

if (NOT WIN32)
  message(FATAL_ERROR "This program only works on Windows.")
endif()

pkg_check_modules(GLIB glib-2.0)

option(QT_VERSION_MAJOR "Qt version to use (5 or 6)")
option(BUILD_WITH_QT5 "Build with Qt 5" OFF)
option(BUILD_WITH_QT6 "Build with Qt 6" OFF)

if(WITH_QT6)
  set(BUILD_WITH_QT6 ON)
endif()
if(QT_MAJOR_VERSION)
  set(QT_VERSION_MAJOR ${QT_MAJOR_VERSION})
endif()

if(QT_VERSION_MAJOR)
  set(QT_DEFAULT_MAJOR_VERSION ${QT_VERSION_MAJOR})
endif()
set(QT_COMPONENTS Core Gui Widgets AxContainer)

set(QT_MIN_VERSION 5.8)

if(BUILD_WITH_QT6 OR QT_VERSION_MAJOR EQUAL 6)
  set(QT_VERSION_MAJOR 6 CACHE STRING "" FORCE)
  set(BUILD_WITH_QT6 ON CACHE BOOL "" FORCE)
elseif(BUILD_WITH_QT5 OR QT_VERSION_MAJOR EQUAL 5)
  set(QT_VERSION_MAJOR 5 CACHE STRING "" FORCE)
  set(BUILD_WITH_QT5 ON CACHE BOOL "" FORCE)
else()
  # Automatically detect Qt version.
  find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
  if(QT_FOUND AND QT_VERSION_MAJOR EQUAL 6)
    set(BUILD_WITH_QT6 ON CACHE BOOL "" FORCE)
    set(QT_VERSION_MAJOR 6 CACHE STRING "" FORCE)
  elseif(QT_FOUND AND QT_VERSION_MAJOR EQUAL 5)
    set(BUILD_WITH_QT5 ON CACHE BOOL "" FORCE)
    set(QT_VERSION_MAJOR 5 CACHE STRING "" FORCE)
  else()
    message(FATAL_ERROR "Missing Qt.")
  endif()
endif()

if(QT_VERSION_MAJOR)
  set(QT_DEFAULT_MAJOR_VERSION ${QT_VERSION_MAJOR})
endif()

find_package(Qt${QT_VERSION_MAJOR} ${QT_MIN_VERSION} REQUIRED COMPONENTS ${QT_COMPONENTS})

set(QtCore_LIBRARIES Qt${QT_VERSION_MAJOR}::Core)
set(QtGui_LIBRARIES Qt${QT_VERSION_MAJOR}::Gui)
set(QtWidgets_LIBRARIES Qt${QT_VERSION_MAJOR}::Widgets)
set(QtAxContainer_LIBRARIES Qt${QT_VERSION_MAJOR}::AxContainer)

option(ENABLE_WIN32_CONSOLE "Show the windows console even outside Debug mode" OFF)

add_definitions(-DUNICODE)

add_subdirectory(src)
