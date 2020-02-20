#include <iostream>
#include <chrono>
#include <stdlib.h>

#define HIGH_PRECISION double

using Clock = std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::cout;
using std::endl;

HIGH_PRECISION factorial(HIGH_PRECISION num)
{
	HIGH_PRECISION res = 1;
	for (int i = num; i > 0; --i)
	{
		res *= i;
	}
	return res;
}

HIGH_PRECISION pow(HIGH_PRECISION base, HIGH_PRECISION exponent)
{
	HIGH_PRECISION res = 1;
	for (int i = 0; i < exponent; ++i)
	{
		res *= base;
	}
	return res; 
}

HIGH_PRECISION f(HIGH_PRECISION x, unsigned int count)
{
	HIGH_PRECISION result = x; 
	HIGH_PRECISION pow_res = 0; 
	HIGH_PRECISION factorial_res = 0; 
	
	for (int i = 0; i < count; ++i)
	{
		pow_res = pow(x, i);
		factorial_res = factorial(i);
		result += pow_res/factorial_res;
	}
	return result;
}

int main(int argc, char const *argv[])
{
	
	time_point<Clock> start = Clock::now();

	if(argc != 3)
	{
		printf("ERROR: to caculate f(x)=x+e^x a value for x and the number of iterations is needed\n");
		return -1;
	}

	HIGH_PRECISION x = atof(argv[1]);
	HIGH_PRECISION termCount = atof(argv[2]);
	HIGH_PRECISION res = f(x, termCount);

	time_point<Clock> end = Clock::now();
    nanoseconds diff = duration_cast<nanoseconds>(end - start);

    cout << "F[" << x << "] = " << res << endl;
    cout << diff.count() << "ns" << endl;

	return 0;
}