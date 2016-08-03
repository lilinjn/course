cmake_minimum_required(VERSION 3.4)
project(dec)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")

set(BUNNY "${CMAKE_CURRENT_SOURCE_DIR}/bunny.obj")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/usr/include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/usr/lib")
link_libraries("-framework Accelerate")
link_libraries("-lspqr -lumfpack -lcholmod -lmetis -lcolamd -lccolamd -lcamd -lamd -ltbb -lm -lsuitesparseconfig")
link_libraries("-framework OpenGL -framework GLUT")


macro (app name)
    project($name)
    file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
    add_executable(${name} ${SRC})

    file(COPY ${BUNNY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endmacro()

add_subdirectory(Connection)
add_subdirectory(Elasticity)
add_subdirectory(Fairing)
add_subdirectory(Flatten)
add_subdirectory(Geodesics)
add_subdirectory(Hot2)
add_subdirectory(Stripe)