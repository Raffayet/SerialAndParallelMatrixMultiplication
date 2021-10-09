#include <iostream>
#include <random>
#include <time.h>
#include <string>
#include <fstream>
#include <tbb/tick_count.h>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

using namespace std;
using namespace tbb;

#include "Constants.h"

const int matrix_A_size = DIM1 * DIM12;
const int matrix_B_size = DIM12 * DIM2;
const int matrix_C_size = DIM1 * DIM2;

void individual_multiplication(int* C, int* A, int* B, int i, int j) {  //koristi se za podverziju pojedinacnog izvrsavanja i izvrsavanja prema broju jezgara

    for (int k = 0; k < DIM12; k++) {
        C[DIM2 * i + j] += (A[DIM12 * i + k] * B[DIM2 * k + j]);
    }
}

void multiplication_by_row(int* C, int* A, int* B, int i) {     // koristi se u podverziji kada svaki zadatak racuna jednu vrstu

    for (int j = 0; j < DIM2; j++) {
        C[DIM2 * i + j] = 0;
        for (int k = 0; k < DIM12; k++) {

            C[DIM2 * i + j] += (A[DIM12 * i + k] * B[DIM2 * k + j]);
        }
    }
}

void matrix_multiplication_parallel(int* C, int* A, int* B, string kompleksnost) {

    if (kompleksnost == "POJEDINACNO") {
        
        task_group tg;

        for (int i = 0; i < DIM1; i++) {
            for (int j = 0; j < DIM2; j++) {
                C[DIM2 * i + j] = 0;
                tg.run([=] {individual_multiplication(C, A, B, i, j); });
            }  
        }
        tg.wait();
    }

    else if (kompleksnost == "PO_VRSTI") {

        task_group tg;

        for (int i = 0; i < DIM1; i++) {
           
            tg.run([=] {multiplication_by_row(C, A, B, i); });
           
        }
        tg.wait();
    }

    else if (kompleksnost == "PREMA_BROJU_JEZGARA") {

        task_group tg;

        for (int i = 0; i < DIM1; i++) {
            for (int j = 0; j < NUMBER_OF_CORES; j++) {
                C[DIM2 * i + j] = 0;
                tg.run([=] {individual_multiplication(C, A, B, i, j); });
            }
        }
        tg.wait();
    }
}

void matrix_multiplication_serial(int* C, int* A, int* B) {         //serijski algoritam
    for (int i = 0; i < DIM1; i++) {
        for (int j = 0; j < DIM2; j++) {
            C[DIM2 * i + j] = 0;
            for (int k = 0; k < DIM12; k++) {

                C[DIM2 * i + j] += (A[DIM12 * i + k] * B[DIM2 * k + j]);
            }
        }
    }
}

void init_matrix(int* matrix, int m, int n) {               //inicijalizacija matrice
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

void write_matrix(int* matrix, string path, int dimenzija) {     //upisivanje random vrednosti elemenata matrice u fajl na jednodimenzionalan nacin

    ofstream f(path);

    for (int i = 0; i < DIM12 * dimenzija; i++)
    {
        matrix[i] = rand() % 10;
        f << matrix[i];
    }
}

int main()
{

    //srand(time(NULL));            // koristi se ukoliko zelimo da svaki put generisemo nove vrednosti

    const int matrix_A_size = DIM1 * DIM12;
    const int matrix_B_size = DIM12 * DIM2;
    const int matrix_C_size = DIM1 * DIM2;

    int* matrix_A = (int*)malloc(sizeof(int) * matrix_A_size);
    int* matrix_B = (int*)malloc(sizeof(int) * matrix_B_size);
    int* matrix_C = (int*)malloc(sizeof(int) * matrix_C_size);

    init_matrix(matrix_A, DIM1, DIM12);
    init_matrix(matrix_B, DIM12, DIM2);

    write_matrix(matrix_A, PATH_A, DIM1);
    write_matrix(matrix_B, PATH_B, DIM2);

    read_matrix(matrix_A, PATH_A, DIM1);
    read_matrix(matrix_B, PATH_B, DIM2);

    cout << "Task group version initiated..." << endl;
    cout << endl;
    tick_count startTime = tick_count::now();

    matrix_multiplication_parallel(matrix_C, matrix_A, matrix_B, KOMPLEKSNOST);

    tick_count endTime = tick_count::now();
    cout << "Task group version time: \t\t\t" << (endTime - startTime).seconds() << " seconds\n";
    cout << endl;

    //print_matrix(matrix_A, DIM1, DIM12, "Matrix A");              //print koristiti samo za proveru rezultatne matrice
    cout << endl;
    //print_matrix(matrix_B, DIM12, DIM2, "Matrix B");
    cout << endl;
    //print_matrix(matrix_C, DIM1, DIM2, "Matrix C (result)");
    cout << endl;

    free(matrix_A);
    free(matrix_B);
    free(matrix_C);
}