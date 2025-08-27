/*
Josue Hernández Gonzalez - 24770
Universidad del Valle de Guatemala
Programación de microprocesadores
Laboratorio 5 - Actividad 
NOTA: La mayoria de codigo se reutilizo del lab3, solamente se modifico para el uso de hilos.
*/

#include <iostream>
#include <pthread.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

// Estructura para pasar datos a cada hilo
struct ThreadData {
    vector<string> instrucciones;
    int thread_id;
    vector<string> resultados;
    vector<bool> validez;
};

// Función para mostrar un número en binario
void mostrarBinario(int numero, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        if (numero & (1 << i)) {
            cout << "1";
        } else {
            cout << "0";
        }
    }
}

// Función para manejar la paridad
bool manejarParidad(int instruccion, int thread_id) {
    int bitParidad = (instruccion >> 7) & 1;
    
    int conteo1s = 0;
    for (int i = 0; i <= 6; i++) {
        if ((instruccion >> i) & 1) {
            conteo1s++;
        }
    }    
    
    bool paridadCorrecta = false;
    if (bitParidad == 0) {
        if (conteo1s % 2 == 0) {
            paridadCorrecta = true;
        } 
    } else {
        if (conteo1s % 2 == 1) {
            paridadCorrecta = true;}
    }
    
    if (!paridadCorrecta) {
        cout << "  [Hilo " << thread_id << "] INSTRUCCIÓN RECHAZADA por no cumplir paridad." << endl;
    }
    
    return paridadCorrecta;
}

// Función para manejar OPCODE
int manejarOpcode(int instruccion, int opA, int opB, int thread_id) {
    int opcode = (instruccion >> 5) & 3;
    

    mostrarBinario(opcode, 2);
    cout << " -> ";
    
    int resultado;
    string nombreOp;
    
    switch (opcode) {
        case 0:
            resultado = opA + opB;
            nombreOp = "SUMA";
            break;
        case 1:
            resultado = opA & opB;
            nombreOp = "AND";
            break;
        case 2:
            resultado = opA | opB;
            nombreOp = "OR";
            break;
        case 3:
            resultado = opA ^ opB;
            nombreOp = "XOR";
            break;
        default:
            cout << "ERROR: OPCODE inválido" << endl;
            return 0;
    }
    

    
    return resultado;
}

// Función para manejar MOD
void manejarMod(int instruccion, int& opA, int& opB, int& resultado, int thread_id) {
    int mod = (instruccion >> 3) & 3;
    
    mostrarBinario(mod, 2);
    cout << " -> ";
    
    switch (mod) {
        case 0:
            break;
        case 1:
            resultado = ~resultado;
            break;
        case 2:
            opA = opA * 2;
            break;
        case 3:
            int temp = opA;
            opA = opB;
            opB = temp;
            break;
    }
}

// Función para manejar BANDERA
void manejarBandera(int instruccion, int& opA, int& opB, int thread_id) {
    int bandera = (instruccion >> 2) & 1;
    
    if (bandera == 0) {
    } else {
        opA = (~opA) & 0xFF;
        opB = (~opB) & 0xFF;
    }
}

// Función para manejar OPERANDOS
void manejarOperandos(int instruccion, int& opA, int& opB, int thread_id) {
    int operandos = instruccion & 3;
    
    mostrarBinario(operandos, 2);
    
    int valor;
    switch (operandos) {
        case 0: valor = 1; break;
        case 1: valor = 2; break;
        case 2: valor = 3; break;
        case 3: valor = 4; break;
        default: valor = 1; break;
    }
    
    
    opA = valor;
    opB = valor;
}

// Procesar una instrucción individual
pair<string, bool> procesarInstruccion(string cadenaDeBits, int thread_id) {
    
    // Validar longitud
    if (cadenaDeBits.length() != 8) {
        cout << "  [Hilo " << thread_id << "] ERROR: La instrucción debe tener 8 bits." << endl;
        return make_pair("ERROR", false);
    }
    
    int instruccion = stoi(cadenaDeBits, 0, 2);
    
    // 1. Manejar paridad
    if (!manejarParidad(instruccion, thread_id)) {
        return make_pair("PARIDAD_ERROR", false);
    }
    
    // 2. Manejar operandos
    int op1, op2;
    manejarOperandos(instruccion, op1, op2, thread_id);
    
    // 3. Manejar bandera
    manejarBandera(instruccion, op1, op2, thread_id);
    
    // 4. Manejar MOD (pre-operación)
    int mod = (instruccion >> 3) & 3;
    int resultado = 0;
    if (mod == 2 || mod == 3) {
        manejarMod(instruccion, op1, op2, resultado, thread_id);
    }
    
    // 5. Manejar OPCODE
    resultado = manejarOpcode(instruccion, op1, op2, thread_id);
    
    // 6. Manejar MOD (post-operación)
    if (mod == 1) {
        manejarMod(instruccion, op1, op2, resultado, thread_id);
    }
    
    // 7. Generar resultado final
    cout << "  [Hilo " << thread_id << "] ------ RESULTADO ------" << endl;
    cout << "  [Hilo " << thread_id << "] Resultado binario: ";
    
    // Convertir resultado a string binario
    string resultadoBinario = "";
    int resultadoFinal = resultado & 0xFF;
    for (int i = 7; i >= 0; i--) {
        if (resultadoFinal & (1 << i)) {
            resultadoBinario += "1";
        } else {
            resultadoBinario += "0";
        }
    }
    
    cout << resultadoBinario << " (decimal: " << resultadoFinal << ")" << endl;
    
    return make_pair(resultadoBinario, true);
}

// Función que ejecutará cada hilo
void* procesarInstruccionesEnHilo(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    cout << "Hilo " << data->thread_id << " iniciado. Procesará " 
         << data->instrucciones.size() << " instrucciones." << endl;
    
    // Procesar cada instrucción asignada a este hilo
    for (size_t i = 0; i < data->instrucciones.size(); i++) {
        pair<string, bool> resultado = procesarInstruccion(data->instrucciones[i], data->thread_id);
        data->resultados.push_back(resultado.first);
        data->validez.push_back(resultado.second);
    }
    
    cout << "Hilo " << data->thread_id << " terminó su procesamiento." << endl;
    
    pthread_exit(NULL);
}

// Función para obtener entrada válida
int obtenerEntradaValida(const string& mensaje) {
    int valor;
    cout << mensaje;
    while (!(cin >> valor) || valor < 1) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Entrada no válida, debe ser número válido: ";
    }
    return valor;
}

int main() {
    string entradaUser;
    
    cout << "  DECODIFICADOR PARALELO DE INSTRUCCIONES     " << endl;
    cout << endl;
    
    cout << "Ingrese instrucciones binarias de 8 bits separadas por espacio: ";
    cin.ignore(); // Limpiar buffer
    getline(cin, entradaUser);
    
    // Dividir instrucciones
    vector<string> instrucciones;
    stringstream ss(entradaUser);
    string instruccion;
    
    while (ss >> instruccion) {
        instrucciones.push_back(instruccion);
    }
    
    if (instrucciones.empty()) {
        cout << "No se ingresaron instrucciones válidas." << endl;
        return 1;
    }
    
    int cantidad_instrucciones = instrucciones.size();
    cout << endl;
    cout << "Total de instrucciones ingresadas: " << cantidad_instrucciones << endl;
    cout << "Creando " << cantidad_instrucciones << " hilos para procesamiento paralelo..." << endl;
    cout << endl;
    
    // Crear arrays para hilos y datos
    pthread_t* hilos = new pthread_t[cantidad_instrucciones];
    ThreadData* thread_data = new ThreadData[cantidad_instrucciones];
    
    // Crear y ejecutar hilos (un hilo por instrucción)
    for (int i = 0; i < cantidad_instrucciones; i++) {
        thread_data[i].thread_id = i + 1;
        thread_data[i].instrucciones.push_back(instrucciones[i]);
        
        int resultado = pthread_create(&hilos[i], NULL, procesarInstruccionesEnHilo, &thread_data[i]);
        if (resultado != 0) {
            cout << "Error al crear hilo " << (i + 1) << endl;
            return 1;
        }
    }
    
    // Esperar que terminen todos los hilos
    for (int i = 0; i < cantidad_instrucciones; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    // Mostrar resumen final
    cout << endl;
    cout << "================= RESUMEN FINAL =================" << endl;
    for (int i = 0; i < cantidad_instrucciones; i++) {
        cout << "Hilo " << thread_data[i].thread_id << " - Instrucción: " << instrucciones[i];
        
        if (!thread_data[i].resultados.empty() && thread_data[i].validez[0]) {
            cout << " Resultado: " << thread_data[i].resultados[0] << " ✓" << endl;
        } else {
            cout << " ERROR " << endl;
        }
    }
    cout << "================================================" << endl;
    
    // Limpiar memoria
    delete[] hilos;
    delete[] thread_data;
    
    return 0;
}