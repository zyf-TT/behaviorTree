 # 设置最低 CMake 版本  
cmake_minimum_required(VERSION 3.10)  

# 设置项目名称  
project(BehaviorTreeExample)  

# 设置 C 语言标准  
set(CMAKE_C_STANDARD 11)  
set(CMAKE_C_STANDARD_REQUIRED True)  

# 查找源文件  
set(SOURCES  
    main.c  
    BehaviorTree.c  
)  

# 添加可执行文件  
add_executable(BehaviorTreeExample ${SOURCES})  

# 如果你有额外的库或者包括其他目录，请在这里添加  
# target_include_directories(BehaviorTreeExample PRIVATE include)