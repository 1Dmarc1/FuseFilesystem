cmake_minimum_required(VERSION 3.25)
project(FuseFileSystem_2 C)

set(CMAKE_C_STANDARD 23)

# Find and include Fuse package
find_package(PkgConfig REQUIRED)
pkg_search_module(FUSE REQUIRED fuse)


add_compile_options(-D_FILE_OFFSET_BITS=64)

add_executable(FuseFileSystem_2 ../main.c ../Bitmap.c ../Directory.c ../FileSystem.c ../INode.c ../Util.c)

# Link against Fuse library using pkg-config
target_link_libraries(FuseFileSystem_2 PRIVATE ${FUSE_LIBRARIES})
target_include_directories(FuseFileSystem_2 PRIVATE ${FUSE_INCLUDE_DIRS})
