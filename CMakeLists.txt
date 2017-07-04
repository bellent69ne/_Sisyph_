cmake_minimum_required(VERSION 3.8)

SET(CMAKE_C_COMPILER /usr/bin/clang)
SET(CMAKE_CXX_COMPILER /usr/bin/clang)

SET(CLANG_COMPILER_FLAGS "-Wall -pedantic")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_COMPILER_FLAGS}")

project(sisyph)

include_directories(
	"${PROJECT_SOURCE_DIR}/polymorphic_design/include"
	"/usr/include/cryptopp"
)

link_directories(/usr/lib)


SET(
	SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/polymorphic_design/src/main.cpp"
	"${PROJECT_SOURCE_DIR}/polymorphic_design/src/symmetric.cpp"
	"${PROJECT_SOURCE_DIR}/polymorphic_design/src/blockCipher.cpp"
	"${PROJECT_SOURCE_DIR}/polymorphic_design/src/rc6.cpp"
	"${PROJECT_SOURCE_DIR}/polymorphic_design/src/twofish.cpp"
	"${PROJECT_SOURCE_DIR}/polymorphic_design/src/shredder.cpp"
	"${PROJECT_SOURCE_DIR}/polymorphic_design/src/walker.cpp"
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
