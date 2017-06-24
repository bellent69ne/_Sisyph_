cmake_minimum_required(VERSION 3.8)

SET(CMAKE_C_COMPILER /usr/bin/clang)
SET(CMAKE_CXX_COMPILER /usr/bin/clang)

SET(CLANG_COMPILER_FLAGS "-Wall -pedantic")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_COMPILER_FLAGS}")

project(sisyph)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"/usr/include/cryptopp"
)

link_directories(/usr/lib)


SET(
	SOURCE_FILES 
	"${PROJECT_SOURCE_DIR}/src/main.cpp"
	"${PROJECT_SOURCE_DIR}/src/cbctwofish.cpp"
	"${PROJECT_SOURCE_DIR}/src/walker.cpp"
	"${PROJECT_SOURCE_DIR}/src/shredder.cpp"
)

add_executable(sisyph ${SOURCE_FILES})

set_property(TARGET sisyph PROPERTY CXX_STANDARD 14)

target_link_libraries(sisyph
	stdc++	
	boost_system
	boost_filesystem
	cryptopp
	pthread
)

