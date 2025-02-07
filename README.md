# Content-Based-Image-Retrieval
Project for CS5330 Computer Vision and Content Recognition

### CMakeLists.txt
cmake_minimum_required(VERSION 3.10)  
project(project2)

set(CMAKE_CXX_STANDARD 17)  
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)  
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(main main.cpp csv_util/csv_util.cpp functions.cpp faceDetect.cpp)  
target_link_libraries(main ${OpenCV_LIBS})

add_executable(image_proc image_proc.cpp csv_util/csv_util.cpp functions.cpp faceDetect.cpp)  
target_link_libraries(image_proc ${OpenCV_LIBS})

find_package(Eigen3 REQUIRED)  
find_package(onnxruntime REQUIRED)  
include_directories(${OpenCV_INCLUDE_DIRS})  
include_directories(${EIGEN3_INCLUDE_DIR})  
include_directories(<dir_containing_onnxruntime>/onnxruntime)
link_directories(<dir_containing_onnxruntime>/onnxruntime/1.20.1_2/lib)
target_link_libraries(main ${OpenCV_LIBS} Eigen3::Eigen <dir_containing_onnxruntime>/onnxruntime/1.20.1_2/lib/libonnxruntime.1.20.1.dylib)
target_link_libraries(image_proc ${OpenCV_LIBS} Eigen3::Eigen <dir_containing_onnxruntime>/onnxruntime/1.20.1_2/lib/libonnxruntime.1.20.1.dylib)