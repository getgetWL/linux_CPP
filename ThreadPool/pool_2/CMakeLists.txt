# 可执行文件名
set(BIN_NAME pool_2)

# 头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 指定可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#生成可执行文件
add_executable(${BIN_NAME} ${MAIN_SRC})

# 指定要链接的动态库
target_link_libraries(${BIN_NAME} 
    pthread
)



