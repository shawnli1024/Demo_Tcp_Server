# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 指定生成 MathFunctions 链接库
add_library (binParser ${DIR_LIB_SRCS})

SET(CMAKE_C_COMPILER g++)
if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

