cmake_minimum_required(VERSION 3.16)

project(ext2boot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    swal
    GIT_REPOSITORY https://github.com/disba1ancer/swal.git
    GIT_TAG master
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(swal)
add_executable(ext2boot
    src/Device.cpp
    src/Device.h
    src/Ext2Driver.cpp
    src/Ext2Driver.h
    src/Ext2INode.cpp
    src/Ext2INode.h
    src/byte_parser.h
    src/enum_bit.h
    src/main.cpp
    src/types.h
)
target_compile_definitions(ext2boot PRIVATE $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0xA00 UNICODE=1>)
target_link_libraries(ext2boot PRIVATE swal::swal)

include(GNUInstallDirs)
install(TARGETS ext2boot
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
