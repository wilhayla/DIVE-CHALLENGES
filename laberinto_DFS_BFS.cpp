#include <iostream> // para utilizar las funciones de entrada y salida
#include <vector>   // para poder utilizar vectores en la generacion de la matriz
#include <stack>    // para poder utilizar pilas en la funcion generacion de laberintos (DFS)
#include <queue> // Para usar std::queue en BFS
#include <utility> // Para usar std::pair
#include <algorithm> // Para usar std::shuffle
#include <random> // Para usar std::random_device, std::mt19937, std::uniform_int_distribution


using namespace std;  

using cordenadas = pair<int, int>;

// Funcion para definir la matriz del laberinto
vector<vector<char>> definir_matriz() {
    int numero_Filas_Internas = 25; 
    int numero_Columnas_Internas = 25;
    
    if (numero_Filas_Internas % 2 == 0) numero_Filas_Internas++;
    if (numero_Columnas_Internas % 2 == 0) numero_Columnas_Internas++;

    int total_Filas_Tablero = numero_Filas_Internas + 2;
    int total_Columnas_Tablero = numero_Columnas_Internas + 2;

    vector<vector<char>> tablero(total_Filas_Tablero, vector<char>(total_Columnas_Tablero, '#'));
    
    return tablero;
}
// Funcion para imrpimir 
void mostrar_matriz(vector<vector<char>> matriz) {
    for (auto& fila_Actual : matriz) {                          
        for (char& columna_Actual : fila_Actual) {           
            cout << columna_Actual << " ";                     
        }
        cout << endl;
    }
}
// Funcion para generar un laberinto aleatorio
void generador_laberinto(vector<vector<char>>& tablero) {
    int Filas = tablero.size();    
    int Columnas = tablero[0].size(); 
    
    vector<vector<bool>> visitado(Filas, vector<bool>(Columnas, false));
    stack<cordenadas> pila;
    vector<cordenadas> direcciones = { {-2, 0}, {2, 0} ,{0, -2} , {0, 2} };

    random_device rd;    
    mt19937 gen(rd());   
                    
    visitado[celda_inicio_generacion.first][celda_inicio_generacion.second] = true;
    tablero[celda_inicio_generacion.first][celda_inicio_generacion.second] = ' ';
    pila.push(celda_inicio_generacion);

    while (!pila.empty()) {  
    
        cordenadas celda_actual = pila.top();

        vector<cordenadas> vecinos_validos_no_visitados;
        
        shuffle(direcciones.begin(), direcciones.end(), gen); 
                                                               
        for (cordenadas direccion : direcciones) {
            cordenadas vecino = {
                celda_actual.first + direccion.first,      
                celda_actual.second + direccion.second    
            };

            if (vecino.first > 0 && vecino.first < Filas - 1 && 
                vecino.second > 0 && vecino.second < Columnas - 1) { 
                if (!visitado[vecino.first][vecino.second]) {    
                    vecinos_validos_no_visitados.push_back(vecino); 
                }
            }
        }

        if (!vecinos_validos_no_visitados.empty()) {   
            
            uniform_int_distribution<> dist(0, vecinos_validos_no_visitados.size() - 1); 
            cordenadas vecino_elegido = vecinos_validos_no_visitados[dist(gen)];

            cordenadas celda_intermedia = {
                celda_actual.first + (vecino_elegido.first - celda_actual.first) / 2,      
                celda_actual.second + (vecino_elegido.second - celda_actual.second) / 2    
            };

            visitado[vecino_elegido.first][vecino_elegido.second] = true;
            tablero[celda_intermedia.first][celda_intermedia.second] = ' ';
            tablero[vecino_elegido.first][vecino_elegido.second] = ' ';

            pila.push(vecino_elegido);
        }
        else {
            pila.pop();
        }
    }
}

// Función para encontrar el camino más corto usando BFS
vector<cordenadas> busqueda_camino_BFS(vector<vector<char>>& tablero, cordenadas celda_inicial, cordenadas celda_final) {
    int filas = tablero.size();
    int columnas = tablero[0].size();
    
    vector<vector<bool>> visitado(filas, vector<bool>(columnas, false));

    queue<pair<cordenadas, vector<cordenadas>>> q;

    vector<cordenadas> direcciones = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    visitado[celda_inicial.first][celda_inicial.second] = true;  
    q.push({ celda_inicial, {celda_inicial} });   

    //exploracion del laberinto bucle principal
    while (!q.empty()) {   
        
        cordenadas celda_actual = q.front().first;  
        vector<cordenadas> camino_actual = q.front().second; 
        q.pop();   

        if (celda_actual.first == celda_final.first && celda_actual.second == celda_final.second) {  
            return camino_actual;   
        }

        for (cordenadas dir : direcciones) {
            int nueva_fila = celda_actual.first + dir.first;
            int nueva_columna = celda_actual.second + dir.second;

            if (nueva_fila >= 0 && nueva_fila < filas &&
                nueva_columna >= 0 && nueva_columna < columnas &&
                tablero[nueva_fila][nueva_columna] != '#' &&    
                !visitado[nueva_fila][nueva_columna]) {          

                visitado[nueva_fila][nueva_columna] = true;     
                vector<cordenadas> nuevo_camino = camino_actual;   
                nuevo_camino.push_back({ nueva_fila, nueva_columna });  
                q.push({ {nueva_fila, nueva_columna}, nuevo_camino });  
            }
        }
    }

    return {};
}

int main() {
    vector<vector<char>> matriz = definir_matriz();
    
    generador_laberinto(matriz);

    cordenadas celda_entrada = { 0, 1 };  
    cordenadas celda_salida = { matriz.size() - 1, matriz[0].size() - 2 };   
                                                                    
    matriz[celda_entrada.first][celda_entrada.second] = 'E'; 
    matriz[celda_entrada.first + 1][celda_entrada.second] = ' '; 

    matriz[celda_salida.first][celda_salida.second] = 'S'; 
    matriz[celda_salida.first - 1][celda_salida.second] = ' '; 

    cout << "Laberinto generado:" << endl;
    mostrar_matriz(matriz);

    cout << "\nBuscando el camino mas corto con BFS..." << endl;
    vector<cordenadas> camino_mas_corto = busqueda_camino_BFS(matriz, celda_entrada, celda_salida);

    if (!camino_mas_corto.empty()) {    
        cout << "Camino mas corto encontrado (longitud: " << camino_mas_corto.size() << " pasos):" << endl;  
        vector<vector<char>> matriz_con_camino = matriz;   
        for (const auto& p : camino_mas_corto) {           
            if (matriz_con_camino[p.first][p.second] != 'E' && matriz_con_camino[p.first][p.second] != 'S') {   
                matriz_con_camino[p.first][p.second] = '.'; 
            }
        }
        mostrar_matriz(matriz_con_camino);
    }
    else {
        cout << "No se encontro un camino desde la entrada a la salida." << endl;
    }
    return 0;
}
