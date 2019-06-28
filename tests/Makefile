INCLUDES=\
	-I. \
	-I../include \
	-I../libs/rapidjson/include \
	-I../libs/Catch2/single_include

main: tests.cpp
	g++ tests.cpp -o test_build -std=c++14 -Wall $(INCLUDES) 
		
run: main
	./test_build -s -d yes
