cmake_minimum_required (VERSION 3.8)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set (CMAKE_CXX_STANDARD 17)

project (TwilightEditor)

set (TE_VERSION_MAIN 3)
set (TE_VERSION_SUB 2)

configure_file (
	"${projectDir}include/config.h.in"
	"${projectDir}include/config.h"
	)

include_directories("${PROJECT_BINARY_DIR}/include")

file(GLOB TE_SOURCES "${projectDir}source/*.cpp")
file(GLOB TE_HEADERS "${projectDir}include/*.h")


set (TE_INCLUDE_DIRS "")
foreach (_headerFile ${TE_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND TE_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES TE_INCLUDE_DIRS)


add_executable (TwilightEditor ${TE_SOURCES})
target_include_directories(TwilightEditor PRIVATE ${TE_INCLUDE_DIRS})

if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	add_definitions (-DOS_STRING="LINUX")
	add_definitions (-DLINUX)
    target_link_libraries(TwilightEditor stdc++fs)
else ()
	add_definitions (-DOS_STRING="WIN32")
	add_definitions (-DWIN32)
endif()