INCLUDES=\
	-I. \
	-I../include \
	-I../../libs/rapidjson/include \
	-I../../libs/Catch2/single_include

catch_main.o: catch_main.cpp
	g++ catch_main.cpp -c -std=c++14 -Wall $(INCLUDES) 

main: tests.cpp catch_main.o
	g++ catch_main.o tests.cpp -o test_build -std=c++14 -Wall $(INCLUDES) 
		
run: main
	./test_build -s -d yes
