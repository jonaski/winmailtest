include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${GLIBCONFIG_INCLUDE_DIRS})

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5AxContainer_INCLUDE_DIRS})

include_directories("/home/jonas/mxe-static/usr/x86_64-w64-mingw32.static/qt5/include/QtCore")
include_directories("/home/jonas/mxe-static/usr/x86_64-w64-mingw32.static/qt5/include/QtGui")
include_directories("/home/jonas/mxe-static/usr/x86_64-w64-mingw32.static/qt5/include/QtWidgets")
include_directories("/home/jonas/mxe-static/usr/x86_64-w64-mingw32.static/qt5/include/ActiveQt")
#include_directories("/home/jonas/mxe-static/usr/x86_64-w64-mingw32.static/qt5/include/QtCore/")

link_directories(${GLIB_LIBRARY_DIRS})

set(SOURCES mainwindow.cpp logging.cpp iconloader.cpp aboutdialog.cpp outlook.cpp)
set(HEADERS mainwindow.h aboutdialog.h)
set(UI mainwindow.ui aboutdialog.ui)
set(RESOURCES ../data/data.qrc ../data/icons.qrc)

qt5_wrap_cpp(MOC ${HEADERS})
qt5_wrap_ui(UIC ${UI})
qt5_add_resources(QRC ${RESOURCES})

add_library(winmailtest_lib STATIC ${SOURCES} ${MOC} ${UIC} ${QRC})
target_link_libraries(winmailtest_lib ${GLIB_LIBRARIES} ${QT_LIBRARIES})

target_link_libraries(winmailtest_lib odbccp32 regex shlwapi ole32)

set(WINMAILTEST-WIN32-RESOURCES ${CMAKE_SOURCE_DIR}/data/windres.rc)
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT ENABLE_WIN32_CONSOLE)
  set(WINMAILTEST-WIN32-FLAG WIN32)
endif (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT ENABLE_WIN32_CONSOLE)

set(EXECUTABLE_OUTPUT_PATH ..)
add_executable(winmailtest ${WINMAILTEST-WIN32-FLAG} ${WINMAILTEST-WIN32-RESOURCES} main.cpp)
target_link_libraries(winmailtest winmailtest_lib)
