cmake_minimum_required(VERSION 3.0)

project(GStreamerAppTest)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)


#------------------------------------------------------------------------------
# Deps
#------------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER-APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GSTREAMER-MPEGTS REQUIRED gstreamer-mpegts-1.0)
pkg_check_modules(GSTREAMER-BASE REQUIRED gstreamer-base-1.0)
pkg_check_modules(GSTREAMER-AUDIO REQUIRED gstreamer-audio-1.0)

include_directories(
    ${GLIB_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${GSTREAMER-BASE_INCLUDE_DIRS}
    ${GSTREAMER-AUDIO_INCLUDE_DIRS}
    ${GSTREAMER-APP_INCLUDE_DIRS}
)

link_directories(
    ${GLIB_LIBRARY_DIRS}
    ${GSTREAMER_LIBRARY_DIRS}
    ${GSTREAMER-BASE_LIBRARY_DIRS}
    ${GSTREAMER-AUDIO_LIBRARY_DIRS}
    ${GSTREAMER-APP_LIBRARY_DIRS}
)

set(LINK_LIBS
    ${GSTREAMER_LIBRARIES}
    ${GSTREAMER-APP_LIBRARIES}
    ${GSTREAMER-AUDIO_LIBRARIES}
    ${GSTREAMER-BASE_LIBRARIES}
    pthread
    ${GLIB_LIBRARIES}
    ${GLIB_GIO_LIBRARIES}
    ${GLIB_GOBJECT_LIBRARIES}
)

#------------------------------------------------------------------------------
# Sources
#------------------------------------------------------------------------------

set(APPSINK_SRC
    src/AppSink/main.cpp
)


#------------------------------------------------------------------------------
# Binaries
#------------------------------------------------------------------------------

add_executable(GStreamerAppTest_APPSINK
    ${APPSINK_SRC}
)

target_link_libraries(GStreamerAppTest_APPSINK
    ${LINK_LIBS}
)

