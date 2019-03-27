file(GLOB sources *.cpp)

set(name demo_server)
add_executable(${name} ${sources})

target_link_libraries(${name} ${COMMON_LIBS})
