cmake_minimum_required(VERSION 3.20)
project(StreamingRecognition)

set(CMAKE_CXX_STANDARD 20)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(StreamingRecognition main.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

