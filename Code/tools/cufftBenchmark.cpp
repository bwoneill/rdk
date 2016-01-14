//cufftBenchmark.cpp

#include "tools/login.C"
#include "tools/cudaFitTest.cpp"

void cufftBenchmark(){
	login();
	cudaFitTest();
	exit();
}