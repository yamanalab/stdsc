file(GLOB sources *.cpp)

set(name demo_client)
add_executable(${name} ${sources})

target_link_libraries(${name} ${COMMON_LIBS})
