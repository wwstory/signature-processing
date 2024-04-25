cmake_minimum_required(VERSION 3.11..3.21)
project(signature_processing
        VERSION 0.1
        DESCRIPTION "process sign")

# set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native")
set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_library(${PROJECT_NAME} SHARED 
    src/signature_processing.cpp
    src/base64.c
    src/cvt_buf_bmp.c
    src/cvt_mat2bmp.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
)

# MACRO
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

# --- test_main ---
add_executable(test_${PROJECT_NAME} 
    src/main.cpp
    src/base64.c
)
target_include_directories(test_${PROJECT_NAME} PUBLIC include)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fmt)

# include(FetchContent)
# FetchContent_Declare(fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#   GIT_TAG master
# )
# FetchContent_MakeAvailable(fmt)

target_link_libraries(test_${PROJECT_NAME}  
    ${PROJECT_NAME}
)
