cmake_minimum_required(VERSION 3.9)
project(NGP C)

set(EXEC myTpz) # put whatever name you want for the executable 

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_executable(${EXEC}
               "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/src/glTpz.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/src/tpzMap.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/src/predicates.c"
               # you can add other source file here !
               )

target_include_directories(${EXEC} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_target_properties(${EXEC} PROPERTIES
                      C_STANDARD 99
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(BOV_BUILD_EXAMPLES OFF)
add_subdirectory(deps/BOV)
target_link_libraries(${EXEC} bov)

add_custom_target(run COMMAND ${EXEC} -b DEPENDS ${EXEC} COMMENT "./bin/${EXEC}")
add_custom_target(debug COMMAND valgrind --leak-check=full --show-leak-kinds=all -s --track-origins=yes ./bin/myTpz -d DEPENDS ${EXEC} COMMENT "./bin/${EXEC} -n")

