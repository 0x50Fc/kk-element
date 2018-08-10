

cmake_minimum_required(VERSION 3.4.1)

add_library(kk-element
            STATIC
            src/kk-document.cc
            src/kk-element.cc
            src/kk-element-style.cc
            src/kk-document-binary.cc
            )

include_directories(
            ./include
            ../kk-duktape/include
            ../kk-event/include
            ../kk-object/include
            )

set_target_properties(kk-element PROPERTIES
    CXX_STANDARD 11
)
