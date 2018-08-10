

cmake_minimum_required(VERSION 3.4.1)

add_library(duktape
            SHARED
            IMPORTED)

set_target_properties(duktape
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libduktape.so)

add_library(event
            SHARED
            IMPORTED)

set_target_properties(event
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libevent.so)

add_library(kk-object
            SHARED
            IMPORTED)

set_target_properties(kk-object
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libkk-object.so)

add_library(kk-element
            SHARED
            src/main/jni/src/kk-document.cc
            src/main/jni/src/kk-element.cc
            src/main/jni/src/kk-element-style.cc
            src/main/jni/src/kk-document-binary.cc
            )

include_directories(
            src/main/jni/include
            src/main/jni/kk-duktape/include
            src/main/jni/kk-event/include
            src/main/jni/kk-object/include
            )

set_target_properties(kk-element PROPERTIES
    CXX_STANDARD 11
)

target_link_libraries(
    kk-element
    kk-object
    duktape
    event
    log
    GLESv2
    EGL)
    
