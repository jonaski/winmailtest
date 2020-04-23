set(SOURCES mainwindow.cpp logging.cpp iconloader.cpp aboutdialog.cpp)
if(BUILD_WITH_QT5)
  list(APPEND SOURCES qt5/outlook.cpp)
endif()
set(HEADERS mainwindow.h aboutdialog.h)
set(UI mainwindow.ui aboutdialog.ui)
set(RESOURCES ../data/data.qrc ../data/icons.qrc)

if(BUILD_WITH_QT5)
  qt5_wrap_cpp(MOC ${HEADERS})
  qt5_wrap_ui(UIC ${UI})
  qt5_add_resources(QRC ${RESOURCES})
elseif(BUILD_WITH_QT6)
  qt6_wrap_cpp(MOC ${HEADERS})
  qt6_wrap_ui(UIC ${UI})
  qt6_add_resources(QRC ${RESOURCES})
endif()

add_library(winmailtest_lib STATIC ${SOURCES} ${MOC} ${UIC} ${QRC})

target_include_directories(winmailtest_lib PUBLIC
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GLIB_INCLUDE_DIRS}
  ${GLIBCONFIG_INCLUDE_DIRS}
)

target_link_directories(winmailtest_lib PUBLIC
  ${GLIB_LIBRARY_DIRS}
)

target_link_libraries(winmailtest_lib PUBLIC
  ${GLIB_LIBRARIES}
  ${QtCore_LIBRARIES}
  ${QtGui_LIBRARIES}
  ${QtWidgets_LIBRARIES}
  ${QtAxContainer_LIBRARIES}
  odbccp32
  regex
  shlwapi
  ole32
)

set(WINMAILTEST-WIN32-RESOURCES ${CMAKE_SOURCE_DIR}/data/windres.rc)
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT ENABLE_WIN32_CONSOLE)
  set(WINMAILTEST-WIN32-FLAG WIN32)
endif (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT ENABLE_WIN32_CONSOLE)

set(EXECUTABLE_OUTPUT_PATH ..)
add_executable(winmailtest ${WINMAILTEST-WIN32-FLAG} ${WINMAILTEST-WIN32-RESOURCES} main.cpp)
target_link_libraries(winmailtest PUBLIC winmailtest_lib)
