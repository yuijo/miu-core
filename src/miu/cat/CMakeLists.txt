set(name miu-cat)

file(GLOB sources *.cpp)

find_package(Boost COMPONENTS program_options system thread)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(${name} ${sources})
target_link_libraries(${name} pthread zmq msgpack ${Boost_LIBRARIES})

set_target_properties(${name} PROPERTIES
    VERSION ${MIU_CORE_VERSION}
    SOVERSION ${MIU_CORE_VERSION})

install(TARGETS ${name} DESTINATION bin)

