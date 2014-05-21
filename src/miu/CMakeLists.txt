configure_file(config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

file(GLOB entries *)
foreach(entry ${entries})
    if(IS_DIRECTORY ${entry} AND EXISTS ${entry}/CMakeLists.txt)
        add_subdirectory(${entry})
    endif()
endforeach()

