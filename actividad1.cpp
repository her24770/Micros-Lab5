/*
    Josue Hernández Gonzalez - 24770
    Universidad del Valle de Guatemala
    Programación de microprocesadores
    Laboratorio 5 - Actividad 1
*/

#include <iostream>
#include <pthread.h>
#include <random>
#include <algorithm>
#include <vector>
#include <iomanip>

using namespace std;

// Estructura para os hilos
struct ThreadData {
    int** matriz;   
    int tamaño;     
    int fila_inicio;    
    int fila_fin;        
    int thread_id;       
};

// Función que ejecutará cada hilo
void* ordenarFilas(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    cout << "Hilo " << data->thread_id << " manejará filas " 
         << data->fila_inicio << " a " << data->fila_fin << endl;
    
    // Procesar cada fila asignada a este hilo
    for (int i = data->fila_inicio; i <= data->fila_fin; i++) {
        // Crear un vector temporal para ordenar la fila
        vector<int> fila_temp(data->tamaño);
        
        // Copiar la fila a un vector
        for (int j = 0; j < data->tamaño; j++) {
            fila_temp[j] = data->matriz[i][j];
        }
    
        sort(fila_temp.begin(), fila_temp.end());

        for (int j = 0; j < data->tamaño; j++) {
            data->matriz[i][j] = fila_temp[j];
        }
    }

    cout << "Hilo " << data->thread_id << " terminado" << endl;
    
    pthread_exit(NULL);
}

// funcion crear matriz con punteros
int** crearMatriz(int tamaño) {
    int** matriz = new int*[tamaño];
    for (int i = 0; i < tamaño; i++) {
        matriz[i] = new int[tamaño];
    }
    return matriz;
}


// funcion para llenar con dato aleaotrios
void llenarMatrizAleatoria(int** matriz, int tamaño) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 100);
    
    for (int i = 0; i < tamaño; i++) {
        for (int j = 0; j < tamaño; j++) {
            matriz[i][j] = dis(gen);
        }
    }
}

//funcion para imprimir matriz
void imprimirMatriz(int** matriz, int tamaño) {
    for (int i = 0; i < tamaño; i++) {
        cout << endl;
        cout << "Fila " << i << ": ";
        cout << endl;
        for (int j = 0; j < tamaño; j++) {
            cout << setw(4) << matriz[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    //tamaño de matriz y cantidad de hilos
    const int dimension_matriz = 20;
    const int cantidad_hilos = 10;
    

    // Crear y llenar matriz
    int** matriz = crearMatriz(dimension_matriz);
    llenarMatrizAleatoria(matriz, dimension_matriz);

    cout << "------------------------- MATRIZ ORIGINAL -------------------" << endl;
    imprimirMatriz(matriz, dimension_matriz);
    
    // manejar id de hilos y sus datos
    pthread_t hilos[cantidad_hilos];
    ThreadData thread_data[cantidad_hilos];
    
    // calculo para dividir la tarea a los hilos
    int filas_hilo = dimension_matriz / cantidad_hilos;
    int filas_residuo = dimension_matriz % cantidad_hilos;


    // POner a trabajar a cada hilo
    for (int i = 0; i < cantidad_hilos; i++) {
        thread_data[i].matriz = matriz;
        thread_data[i].tamaño = dimension_matriz;
        thread_data[i].thread_id = i;
        
        // Calcular rango de filas para este hilo
        thread_data[i].fila_inicio = i * filas_hilo;
        thread_data[i].fila_fin = (i + 1) * filas_hilo - 1;
        
        if (i == cantidad_hilos - 1) {
            thread_data[i].fila_fin += filas_residuo;
        }
        
        // Crear el hilo
        int resultado = pthread_create(&hilos[i], NULL, ordenarFilas, &thread_data[i]);
    }
    
    // Esperar que cada hilo ordene us s¿ filas
    for (int i = 0; i < cantidad_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    // mostrar reultado final
    cout <<"------------------------- MATRIZ ORDENADA -------------------" << endl;
    imprimirMatriz(matriz, dimension_matriz);

    return 0;
}