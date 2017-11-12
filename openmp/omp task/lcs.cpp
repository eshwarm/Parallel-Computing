#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <string.h>
#include <fstream>
using namespace std;
int max(int a, int b);
  
 /* Returns length of LCS for X[0..m-1], Y[0..n-1] */

int lcs( const char *X, const char *Y, int m, int n , int num_threads)
{
	int i, j, return_val;
	
	//Allocaate memory to two D array
	int ** arr = new int*[m + 1];
	for (int i = 0; i <= m; i++)
		arr[i] = new int[n + 1];
	
	//Initialise array row and column with 0
	for (int i = 1; i <= m; i++)
		arr[i][0] = 0;
	for (int j = 0; j <= n; j++)
		arr[0][j] = 0;

	int total = m + n; //diagonal count
	#pragma omp parallel  default(none) firstprivate(num_threads,m,n,X,Y) shared(arr,total ,cout)
	{
	  for (int current = 1; current <= (total - 1);) {			
		int j = omp_get_thread_num() + 1;   //column index
		int i = current - j + 1;      //row index
		for (; j <= current; j += num_threads, i = i - num_threads) {
			if (i <= m && j <= n) {
				if (X[i] == Y[j]) {
					arr[i][j] = arr[i - 1][j - 1] + 1;
				}else {
					arr[i][j] = max(arr[i-1][j], arr[i][j-1]);	//get max of up and left
				}
			}
		}
		current++;
	#pragma omp barrier
	  }
	}
	return_val = arr[m][n];
	//Free memory
	for(int i = 0; i <= m; i++){
	  delete [] arr[i];	
	}
		

return return_val;
}


/* Utility function to get max of 2 integers */
int max(int a, int b)
{
    return (a > b)? a : b;
}
  
int main(int argc, char* argv[])
{
	int num_threads = atoi(argv[1]); //no of threads	
	std::ifstream file1("Text1.txt");
	std::ifstream file2("Text2.txt");
	std::string str1;
	std::string  str2;
	std::string file_contents1;
	std::string  file_contents2;
	//Read contents of file1
	while (std::getline(file1, str1)){
	  file_contents1 += str1;
	}
	//Read contents of file2
	while (std::getline(file2, str2)){
	  file_contents2 += str2;
	}

	const char* X = file_contents1.data(); //convert string to const char
	const char* Y = file_contents2.data(); //convert string to const char
	int val;
	int m = strlen(X); //get string length
	int n = strlen(Y); //get string length
	std::cout<< m <<" "<<n<<std::endl;
	omp_set_num_threads(num_threads);
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	val = lcs( X, Y, m, n , num_threads);
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cerr<< elapsed_seconds.count()<<std::endl; 
	std::cout<<val<<endl;
    return 0;
}