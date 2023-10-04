cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(PROJECT_NAME ComparerPlayground)
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

if(MSVC)
    add_compile_definitions(
                            _USE_MATH_DEFINES
                            _CRT_SECURE_NO_DEPRECATE
                            _CRT_SECURE_NO_WARNINGS
                            NOMINMAX)
    add_compile_options(/GF /Zc:wchar_t /W4)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Z7")
endif()

include(FetchContent)

set(BUILD_opencv_videoio OFF)
set(BUILD_opencv_flann OFF)
set(BUILD_ZLIB OFF)
set(BUILD_opencv_js_bindings_generator OFF)
set(WITH_PROTOBUF OFF)
set(BUILD_opencv_java_bindings_generator OFF)
set(BUILD_opencv_dnn OFF)
set(BUILD_opencv_ml OFF)
set(BUILD_opencv_apps OFF)
set(BUILD_opencv_features2d ON)
set(BUILD_opencv_objc_bindings_generator OFF)
set(BUILD_opencv_objdetect OFF)
set(BUILD_ITT OFF)
set(BUILD_opencv_python_bindings_generator OFF)
set(BUILD_opencv_photo OFF)
set(BUILD_JAVA OFF)
set(BUILD_JASPER OFF)
set(WITH_OPENEXR OFF)
set(WITH_OPENCLAMDFFT OFF)
set(WITH_OPENCLAMDBLAS OFF)
set(BUILD_opencv_python_tests OFF)
set(BUILD_PERF_TESTS OFF)
set(BUILD_JPEG OFF)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_opencv_stitching OFF)
set(BUILD_PNG ON)
set(BUILD_opencv_videoio OFF)
set(BUILD_opencv_flann OFF)
set(BUILD_ZLIB OFF)
set(BUILD_opencv_js_bindings_generator OFF)
set(WITH_PROTOBUF OFF)
set(BUILD_opencv_java_bindings_generator OFF)
set(BUILD_opencv_dnn OFF)
set(BUILD_opencv_ml OFF)
set(BUILD_opencv_apps OFF)
set(BUILD_opencv_objc_bindings_generator OFF)
set(BUILD_opencv_objdetect OFF)
set(BUILD_ITT OFF)
set(BUILD_opencv_python_bindings_generator OFF)
set(BUILD_opencv_photo OFF)
set(BUILD_JAVA OFF)
set(BUILD_JASPER OFF)
set(WITH_OPENEXR OFF)
set(BUILD_IPP_IW ON)
set(WITH_OPENCLAMDFFT OFF)
set(WITH_OPENCLAMDBLAS OFF)
set(BUILD_opencv_python_tests OFF)
set(BUILD_PERF_TESTS OFF)
set(BUILD_JPEG OFF)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_opencv_stitching OFF)
set(BUILD_PNG ON)
set(BUILD_opencv_imgcodecs ON)
set(BUILD_PROTOBUF OFF)
set(BUILD_opencv_video OFF)
set(BUILD_opencv_calib3d OFF)
set(BUILD_TIFF OFF)
set(BUILD_TESTS OFF)
set(BUILD_opencv_gapi OFF)
set(BUILD_OPENJPEG ON)
set(WITH_OPENCL OFF)
set(BUILD_opencv_ts OFF)
set(BUILD_PACKAGE OFF)
set(BUILD_WEBP ON)
set(ENABLE_LIBJPEG_TURBO_SIMD OFF)
set(OPENCV_ENABLE_NONFREE ON)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG        v2.9
)

FetchContent_Declare(
    opencv
    GIT_REPOSITORY https://github.com/opencv/opencv.git
    GIT_TAG        4.7.0
)

FetchContent_MakeAvailable(argparse opencv)

set(Source_Files
    "sources/main.cpp"
    "sources/DifferencesFinder.cpp"
    "sources/BlobDetect.cpp"
    "sources/SSIMImageComparator.cpp"
    "sources/CPSImageComparator.cpp"
    "sources/MSEImageComparator.cpp"
    "sources/PSNRImageComparator.cpp"
    "sources/HistogramImageComparator.cpp"
    "sources/PixelWiseImageComparator.cpp"
    "sources/CVBlobsImageComparator.cpp"
    "sources/WhiteLineImageComparator.cpp"
    "sources/Utils.cpp"
)
source_group("Source Files" FILES ${Source_Files})

add_executable(${PROJECT_NAME} ${Source_Files})
target_link_libraries(${PROJECT_NAME} PRIVATE argparse opencv_core opencv_imgcodecs opencv_highgui opencv_imgproc opencv_features2d)
target_include_directories(${PROJECT_NAME} PRIVATE "include")
target_include_directories(${PROJECT_NAME} PRIVATE
        ${OPENCV_CONFIG_FILE_INCLUDE_DIR}
        ${OPENCV_MODULE_opencv_core_LOCATION}/include
        ${OPENCV_MODULE_opencv_imgcodecs_LOCATION}/include
        ${OPENCV_MODULE_opencv_highgui_LOCATION}/include
        ${OPENCV_MODULE_opencv_imgproc_LOCATION}/include
        ${OPENCV_MODULE_opencv_features2d_LOCATION}/include
)
