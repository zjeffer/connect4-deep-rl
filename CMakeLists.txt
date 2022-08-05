cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

######### MAIN PROJECT #########
project(connect4 VERSION 1.0)

###### LibTorch ######
set(CMAKE_PREFIX_PATH /usr/include/libtorch/share/cmake/)
find_package(Torch REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_LIBS})
include_directories(${TORCH_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

###### g3log #########
set(CMAKE_PREFIX_PATH /usr/local/lib/cmake/g3log)
find_package(g3log CONFIG REQUIRED)
message(
"g3log package:
    found: ${G3LOG_FOUND}
    include dir: ${G3LOG_INCLUDE_DIRS}
    libraries: ${G3LOG_LIBRARIES}"
)
include_directories(${G3LOG_INCLUDE_DIRS})


###### main files ######
file (GLOB PROJECT_SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${PROJECT_SOURCE_DIR}/src/connect4/*.cpp
    ${PROJECT_SOURCE_DIR}/src/connect4/*.hpp
    ${PROJECT_SOURCE_DIR}/src/logger/*.cpp
    ${PROJECT_SOURCE_DIR}/src/logger/*.hpp
    ${PROJECT_SOURCE_DIR}/src/neuralNetwork/*.cpp
    ${PROJECT_SOURCE_DIR}/src/neuralNetwork/*.hpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# link libraries (opencv, torch, g3log, qt)
target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES} g3log)

# fix libgomp error
add_library(libgomp SHARED IMPORTED)
set_property(TARGET libgomp PROPERTY IMPORTED_LOCATION "/usr/lib/libgomp.so")
target_link_libraries(${PROJECT_NAME} libgomp)

# set c++ standard
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
