float: 
	sed -i "s/#define HIGH_PRECISION.*/#define HIGH_PRECISION float/g" main.cpp;
	g++ -g -std=c++11 main.cpp -o "main_float.o";

double: 
	sed -i "s/#define HIGH_PRECISION.*/#define HIGH_PRECISION double/g" main.cpp;
	g++ -g -std=c++11 main.cpp -o "main_double.o";

clean:
	rm -rf *.o
