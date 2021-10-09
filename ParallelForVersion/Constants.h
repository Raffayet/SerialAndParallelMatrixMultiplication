#include <string>
#include <tbb/tbb.h>

using namespace tbb;
using namespace std;

#define DIM1 1000      // dimenzija prve
#define DIM2 1000      // dimenzija druge
#define DIM12 1000    // dimenzija prve / druge     // zajednicka dimenzija (dve matrice moraju imati bar jednu istu dimenziju da bi se pomnozile
#define PATH_A "inputMatrixA.txt"                   //ulazni fajl za matricu A
#define PATH_B "inputMatrixB.txt"                   //ulazni fajl za matricu B

using namespace std;

void init_matrix(int* matrix, int m, int n);
void read_matrix(int* matrix, string path, int dimenzija);
void write_matrix(int* matrix, string path, int dimenzija);
void matrix_multiplication_serial(int* C, int* A, int* B);
void print_matrix(int* matrix, int m, int n, string name);


