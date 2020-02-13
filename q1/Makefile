float: 
	sed -i "s/#define HIGH_PRECISION.*/#define HIGH_PRECISION float/g" main.cpp;
	g++ main.cpp -o "main_float.o";

double: 
	sed -i "s/#define HIGH_PRECISION.*/#define HIGH_PRECISION double/g" main.cpp;
	g++ main.cpp -o "main_double.o";

clean:
	rm -rf *.o