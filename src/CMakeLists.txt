find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(TheTelephone_Video main.cpp)
target_link_libraries(TheTelephone_Video avcodec avdevice avfilter avformat avutil swscale ${SDL2_LIBRARIES})
