/*
Josue Hernández Gonzalez - 24770
Universidad del Valle de Guatemala
Programación de microprocesadores
Laboratorio 5 - Actividad 2
*/

#include <iostream>
#include <pthread.h>
#include <random>
#include <vector>
#include <iomanip>

using namespace std;

// Estructura para pasar parámetros a los hilos
struct ThreadData {
    int* arreglo;           
    int indice_inicio;      
    int indice_fin;   
    int thread_id;         
    long suma_parcial;   
};

// Función que ejecutará cada hilo para calcular suma parcial
void* calcularSumaParcial(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    cout << "Hilo " << data->thread_id << " procesara de "<< data->indice_inicio << " al " << data->indice_fin << endl;
    
    data->suma_parcial = 0;
    
    // Calcular suma del segmento asignado
    for (int i = data->indice_inicio; i <= data->indice_fin; i++) {
        data->suma_parcial += data->arreglo[i];
    }
    
    cout << "Hilo " << data->thread_id << ". Suma parcial: " << data->suma_parcial << endl;
    pthread_exit(NULL);
}

// llenar arreglo con valores aleatorios ddel 1 a 100
void llenarArregloAleatorio(int* arreglo, int tamaño) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 100);
    
    for (int i = 0; i < tamaño; i++) {
        arreglo[i] = dis(gen);
    }
}

// imprimir arrglo
void imprimirArreglo(int* arreglo, int tamaño) {
    if (tamaño <= 50) {
        cout << "Arreglo: ";
        for (int i = 0; i < tamaño; i++) {
            cout << setw(4) << arreglo[i] << " ";
        }
        cout << endl;
    } else {
        cout << "Arreglo de " << tamaño << " elementos generado (muy grande para mostrar)" << endl;
    }
}

// Función para validar si es numero
int obtenerEntradaValida(const string& mensaje) {
    int valor;
    cout << mensaje;
    while (!(cin >> valor) || valor < 1) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Entrada no valida, debe ser numero valido ";
    }
    return valor;
}


int main() {
    int tamaño_arreglo, cantidad_hilos;
    
    cout << " --------------------- Suma segmentada con hilos --------------------- " << endl;
    cout << endl;
    
    // Obtener parámetros del usuario con validación
    do {
        tamaño_arreglo = obtenerEntradaValida("Ingrese el tamaño del arreglo: ");
        cantidad_hilos = obtenerEntradaValida("Ingrese la cantidad de hilos: ");
        
        if (tamaño_arreglo < cantidad_hilos) {
            cout << "Error: La cantidad de elementos debe ser mayor o igual a la cantidad de hilos." << endl;
            cout << "Entrada no valida, debe ser numero valido" << endl;
        }   
    } while (tamaño_arreglo < cantidad_hilos);

    
    // Crear y llenar arreglo
    int* arreglo = new int[tamaño_arreglo];
    llenarArregloAleatorio(arreglo, tamaño_arreglo);
    
    cout << endl;
    cout << "----------------------- arreglo original -----------------------" << endl;
    imprimirArreglo(arreglo, tamaño_arreglo);
    cout << endl;
    
    // Crear arrays para hilos y datos
    pthread_t* hilos = new pthread_t[cantidad_hilos];
    ThreadData* thread_data = new ThreadData[cantidad_hilos];
    
    // Calcular distribución de elementos
    int elementos_por_hilo = tamaño_arreglo / cantidad_hilos;
    int elementos_residuo = tamaño_arreglo % cantidad_hilos;
    
    cout << "----------------------- division de trabajo de hilos -----------------------" << endl;
    cout << "Elementos por hilo: " << elementos_por_hilo << endl;
    cout << "Elementos residuo: " << elementos_residuo << endl;
    cout << endl;
    
    // Crear y poner a trabaar a lo hilos
    int indice = 0;
    
    for (int i = 0; i < cantidad_hilos; i++) {
        thread_data[i].arreglo = arreglo;
        thread_data[i].thread_id = i + 1;
        thread_data[i].indice_inicio = indice;
        
        // suma por cada hilo calcuulo
        int elementos_este_hilo = elementos_por_hilo;
        if (i < elementos_residuo) {
            elementos_este_hilo++;
        }
        
        thread_data[i].indice_fin = indice + elementos_este_hilo - 1;
        indice += elementos_este_hilo;
        
        // crear y ejecutar hilo
        int resultado = pthread_create(&hilos[i], NULL, calcularSumaParcial, &thread_data[i]);
    }
    
    // Esperar que terminen todos los hilos
    long suma_total = 0;
    for (int i = 0; i < cantidad_hilos; i++) {
        pthread_join(hilos[i], NULL);
        suma_total += thread_data[i].suma_parcial;
    }

    cout << endl;
    cout << "Suma total: " << suma_total << endl;
    
    return 0;
}