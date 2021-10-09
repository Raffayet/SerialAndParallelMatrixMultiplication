#include <iostream>
#include <random>
#include <time.h>
#include <string>
#include <fstream>
#include <tbb/tick_count.h>
#include <tbb/tbb.h>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"

#include "Constants.h"

using namespace std;
using namespace tbb;

const int matrix_A_size = DIM1 * DIM12;
const int matrix_B_size = DIM12 * DIM2;
const int matrix_C_size = DIM1 * DIM2;

class Multiply {                    //klasa u kojoj se nalazi telo parallel for petlje

private:

    int* matrix_A;
    int* matrix_B;
    int* matrix_C;

public:

    Multiply() {

        matrix_A = (int*)malloc(sizeof(int) * matrix_A_size);
        matrix_B = (int*)malloc(sizeof(int) * matrix_B_size);
        matrix_C = (int*)malloc(sizeof(int) * matrix_C_size);

        init_matrix(matrix_A, DIM1, DIM12);
        init_matrix(matrix_B, DIM12, DIM2);

        write_matrix(matrix_A, PATH_A, DIM1);
        write_matrix(matrix_B, PATH_B, DIM2);

        read_matrix(matrix_A, PATH_A, DIM1);
        read_matrix(matrix_B, PATH_B, DIM2);
    }

    int* getMatrixA() {

        return matrix_A;
    }

    int* getMatrixB() {

        return matrix_B;
    }

    int* getMatrixC() {

        return matrix_C;
    }

    void operator()(const blocked_range2d<size_t>& r) const {
        for (size_t i = r.rows().begin(); i != r.rows().end(); ++i) {

            for (int j = r.cols().begin(); j != r.cols().end(); j++) {
                matrix_C[DIM2 * i + j] = 0;
                for (int k = 0; k < DIM12; k++) {

                    matrix_C[DIM2 * i + j] += (matrix_A[DIM12 * i + k] * matrix_B[DIM2 * k + j]);
                }
            }

        }
    }
};

void matrix_multiplication_parallel(Multiply mult) {

   parallel_for(blocked_range2d<size_t>(0, DIM1, 0, DIM2), mult);
        
}

void matrix_multiplication_serial(int* C, int* A, int* B) {     //serijski algoritam
    for (int i = 0; i < DIM1; i++) {
        for (int j = 0; j < DIM2; j++) {
            C[DIM2 * i + j] = 0;
            for (int k = 0; k < DIM12; k++) {

                C[DIM2 * i + j] += (A[DIM12 * i + k] * B[DIM2 * k + j]);
            }
        }
    }
}

void init_matrix(int* matrix, int m, int n) {       // inicijalizacija matrice 
    int data_size = m * n;
    for (int i = 0; i < data_size; ++i) {
        matrix[i] = i;
    }
}

void print_matrix(int* matrix, int m, int n, string name) {
    cout << "[" << name << "]" << endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i * n + j] << "\t";
        }
        cout << endl;
    }
}

void read_matrix(int* matrix, string path, int dimenzija) {     // cita matricu iz fajla zapisanu na jednodimenzionalan nacin u dvodimenzionalnu matricu

    ifstream f(path);
    string stream;
    for (int i = 0; i < DIM12 * dimenzija; i++) {

        f >> stream;
    }

    for (int j = 0; j < stream.size(); j++) {

        to_string(matrix[j]) = stream[j];
    }
}

void write_matrix(int* matrix, string path, int dimenzija) {        //upisivanje random vrednosti elemenata matrice u fajl na jednodimenzionalan nacin

    ofstream f(path);

    for (int i = 0; i < DIM12 * dimenzija; i++)
    {
        matrix[i] = rand() % 10;
        f << matrix[i];
    }
}

int main()
{

    //srand(time(NULL));          // koristi se ukoliko zelimo da svaki put generisemo nove random vrednosti

    Multiply mult;

    cout << "Parallel for version initiated..." << endl;
    cout << endl;
    tick_count startTime = tick_count::now();

    matrix_multiplication_parallel(mult);

    tick_count endTime = tick_count::now();
    cout << "Parallel for version time: \t\t\t" << (endTime - startTime).seconds() << " seconds\n";
    cout << endl;

    //print_matrix(mult.getMatrixA(), DIM1, DIM12, "Matrix A");         //print koristiti samo za proveru rezultatne matrice
    cout << endl;
    //print_matrix(mult.getMatrixB(), DIM12, DIM2, "Matrix B");
    cout << endl;
    //print_matrix(mult.getMatrixC(), DIM1, DIM2, "Matrix C (result)");
    cout << endl;

    free(mult.getMatrixA());
    free(mult.getMatrixB());
    free(mult.getMatrixC());
}