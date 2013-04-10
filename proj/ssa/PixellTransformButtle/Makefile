

all:
	gcc -c -o buttle_macro.o1 buttle_macro.c -O1
	gcc -c -o buttle_macro.o2 buttle_macro.c -O2
	gcc -c -o buttle_macro.o3 buttle_macro.c -O3
	g++ -c -std=c++11 -o buttle_c11.o1 buttle_c11.cpp -O1
	g++ -c -std=c++11 -o buttle_c11.o2 buttle_c11.cpp -O2
	g++ -c -std=c++11 -o buttle_c11.o3 buttle_c11.cpp -O3
	g++ -o buttle_O1 -std=c++11 buttle_main.cpp buttle_c11.o1 buttle_macro.o1 -O3
	g++ -o buttle_O2 -std=c++11 buttle_main.cpp buttle_c11.o2 buttle_macro.o2 -O3
	g++ -o buttle_O3 -std=c++11 buttle_main.cpp buttle_c11.o3 buttle_macro.o3 -O3

run:
	./buttle_O1
	./buttle_O2
	./buttle_O3

clean:
	rm -f buttle_macro.o? buttle_c11.o? buttle_O?


