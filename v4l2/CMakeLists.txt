cmake_minimum_required(VERSION 3.12.0)
project(main)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} 
  main.cc
  video_control.cc
)

target_link_libraries(${PROJECT_NAME}
	${OpenCV_LIBS}
	yaml-cpp
)
