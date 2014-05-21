cmake_minimum_required(VERSION 2.6)

project(miu-core)

set(MIU_CORE_VERSION_MAJOR 0)
set(MIU_CORE_VERSION_MINOR 0)
set(MIU_CORE_VERSION_PATCH 1)
set(MIU_CORE_VERSION ${MIU_CORE_VERSION_MAJOR}.${MIU_CORE_VERSION_MAJOR}.${MIU_CORE_VERSION_PATCH})

install(DIRECTORY bin/ DESTINATION bin USE_SOURCE_PERMISSIONS)

include_directories(src ${CMAKE_BINARY_DIR}/src)
add_subdirectory(src/miu)

include(CPack)

