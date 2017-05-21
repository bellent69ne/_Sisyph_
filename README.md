# _Sisyph_
/***************************Compilation****************************/
clang -std=c++14 -Wall -pedantic -g3 -ggdb -O0 -DDEBUG -I/usr/include/cryptopp main.cpp walker.cpp cryptographer.cpp -o sisyph -lstdc++ -lboost_filesystem -lboost_system -lcryptopp -lpthread

