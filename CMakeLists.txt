cmake_minimum_required(VERSION 3.10)
project(project2)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add the main executable
add_executable(main main.cpp csv_util/csv_util.cpp functions.cpp faceDetect.cpp)
target_link_libraries(main ${OpenCV_LIBS})

# Add an executable for filters_test.cpp
add_executable(image_proc image_proc.cpp csv_util/csv_util.cpp functions.cpp faceDetect.cpp)
target_link_libraries(image_proc ${OpenCV_LIBS})

# For Depth Anything
find_package(Eigen3 REQUIRED)
find_package(onnxruntime REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(/usr/local/include/onnxruntime)
link_directories(/usr/local/lib)
target_link_libraries(main ${OpenCV_LIBS} Eigen3::Eigen /usr/local/lib/libonnxruntime.dylib)
target_link_libraries(image_proc ${OpenCV_LIBS} Eigen3::Eigen /usr/local/lib/libonnxruntime.dylib)