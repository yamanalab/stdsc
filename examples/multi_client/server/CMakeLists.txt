file(GLOB sources *.cpp)

set(name multi_client_server)
add_executable(${name} ${sources})

target_link_libraries(${name} ${COMMON_LIBS})
