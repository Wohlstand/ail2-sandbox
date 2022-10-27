cmake_minimum_required(VERSION 3.5)

project(MIDIFORM LANGUAGES C)

include(CheckCCompilerFlag)

if(NOT WIN32 AND NOT NINTENDO_SWITCH AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX))
    check_c_compiler_flag("-no-pie" HAS_NO_PIE)
endif()

function(pge_set_nopie _target)
    set_target_properties(${_target} PROPERTIES
        POSITION_INDEPENDENT_CODE False
    )
    if(HAS_NO_PIE AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX))
        set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS " -no-pie")
    endif()
endfunction()

add_executable(midiform
    midiform.c
)
pge_set_nopie(midiform)
