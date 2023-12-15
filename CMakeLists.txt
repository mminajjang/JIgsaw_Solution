cmake_minimum_required(VERSION 3.8)
project(jigsaw_cpp)

set(CMAKE_CXX_STANDARD 17)
find_package(catkin REQUIRED COMPONENTS)
find_package(OpenCV REQUIRED)
include_directories(include/ src/ ${catkin_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

add_library(${PROJECT_NAME}_lib src/helper.cpp src/match.cpp src/puzzle.cpp)
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} main.cpp src/helper.cpp src/match.cpp src/puzzle.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})

add_executable(${PROJECT_NAME}_edit editing.cpp)
target_link_libraries(${PROJECT_NAME}_edit ${PROJECT_NAME}_lib ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})