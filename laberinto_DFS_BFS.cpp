#include <iostream> // para utilizar las funciones de entrada y salida
#include <vector>   // para poder utilizar vectores en la generacion de la matriz
#include <stack>    // para poder utilizar pilas en la funcion generacion de laberintos (DFS)
#include <queue> // Para usar std::queue en BFS
#include <utility> // Para usar std::pair
#include <algorithm> // Para usar std::shuffle
#include <random> // Para usar std::random_device, std::mt19937, std::uniform_int_distribution


using namespace std;  // evita introducir prefijos std:: a cada a los nombres definidos dentro de namespaces de la libreria standard de C++

// Definición de tipo para coordenadas (fila, columna)
// alias de tipo para crear un nombre mas corto y legible para tipo de dato de un par de enteros que representa las cordenadas de un punto en el tablero.
// "using" es una palabra clave de C++ que se utiliza para crear un alias de tipo
// "pair" es una plantilla de clase de la libreria estandar de C++ que permite agrupar dos valores.
using cordenadas = pair<int, int>;

vector<vector<char>> definir_matriz() {
    int numero_Filas_Internas = 25; // Dimensiones del laberinto sin el borde
    int numero_Columnas_Internas = 25;

    // Las dimensiones deben ser impares para que el algoritmo de salto 2 funcione bien
    // Si no son pares, ajustamos para que siempre haya un camino
    if (numero_Filas_Internas % 2 == 0) numero_Filas_Internas++;
    if (numero_Columnas_Internas % 2 == 0) numero_Columnas_Internas++;

    // Creamos la matriz con un borde de 1 celda por cada lado
    int total_Filas_Tablero = numero_Filas_Internas + 2;
    int total_Columnas_Tablero = numero_Columnas_Internas + 2;

    // inicializamos la matriz del tablero con vector de vectores
    vector<vector<char>> tablero(total_Filas_Tablero, vector<char>(total_Columnas_Tablero, '#'));

    // NOTA: La 'E' y la 'S' las colocaremos DESPUÉS de generar el laberinto
    // para asegurarnos de que el perímetro se mantenga cerrado durante la generación.

    return tablero;
}

void mostrar_matriz(vector<vector<char>> matriz) {
    for (auto& fila_Actual : matriz) {                          // itera cada fila de la matriz
        for (char& columna_Actual : fila_Actual) {           // itera cada elemento(columna) de la fila
            cout << columna_Actual << " ";                     // imprime los elementos de la fila y los separa con un espacio en blanco
        }
        cout << endl;
    }
}

// Esta funcion recibe el tablero por referencia para poder modificarlo directamente
void generador_laberinto(vector<vector<char>>& tablero) {
    int Filas = tablero.size();    // Obtengo la cantidad de filas totales del tablero (incluyendo el borde)
    int Columnas = tablero[0].size(); // Obtengo la cantidad de columnas totales del tablero (incluyendo el borde)

    // inicializo un vector de vectores de tipo booleano del tamaño del tablero y asigno false(no visitado) a todas sus celdas.
    // para llevar un registro de las ubicaciones visitadas en el tablero
    vector<vector<bool>> visitado(Filas, vector<bool>(Columnas, false));

    //crear una estructura de datos tipo pila, que almaceno elementos de tipo cordenadas, o sea almacena un par de dos enteros que representan 
    // cordenadas de celdas en el laberinto
    stack<cordenadas> pila;

    // La celda de inicio para la generación debe estar dentro del perímetro
    // Y debe ser una coordenada impar (para que el algoritmo funcione con salto 2)
    cordenadas celda_inicio_generacion = { 1, 1 }; // Empezar en (1,1) para evitar el borde

    vector<cordenadas> direcciones = { {-2, 0}, {2, 0} ,{0, -2} , {0, 2} };

    // declaro un objeto llamado rd de tipo random_device
    // se usa para generar una semilla inicial impredecible a otros generadores de numeros aleatorios como mt19937
    // y por sobre todo evita que se repitan patrones predecibles en ejecuciones.

    /* Si necesitas generar millones de números aleatorios, usar random_device para cada uno sería ineficiente.
    En su lugar, se usa random_device una sola vez al inicio del programa para obtener una semilla verdaderamente aleatoria,
    y luego se pasa esa semilla a un generador como mt19937, que es muy rápido generando números a partir de esa semilla.*/
    random_device rd;    // para generar una semilla inicial verdaderamente aleatoria y utilizar en conjunto con mt19937
    mt19937 gen(rd());   // esta linea crea y declara un objeto llamado gen, que es generador de numeros seudo aleatorios generados por un algoritmo matematico determinista. Si 
                         // su semilla inicial para generar aleatoriedad puede repetirse y generar el mismo resultado, por eso utiliza la semilla de rendom device para asegurar 
                         // realmente que la aleatoriedad no se repita y siempre sea diferente dando un resultado diferente.

    // Marcar la celda de inicio de generación como visitada y convertirla en camino
    visitado[celda_inicio_generacion.first][celda_inicio_generacion.second] = true;
    tablero[celda_inicio_generacion.first][celda_inicio_generacion.second] = ' ';
    pila.push(celda_inicio_generacion);

    while (!pila.empty()) {  // mientras haya celdas para explorar para extender el laberinto
        //obtengo las cordenadas de la parte superior de la pila y almaceno en la variable "celda_actual"
        /*Mientras haya celdas (coordenadas) por procesar en nuestra pila, mira la que se añadió más recientemente y trabaja con esas coordenadas.*/
        cordenadas celda_actual = pila.top();

        /*Esta línea está creando un contenedor vacío listo para guardar las posiciones de
        las celdas adyacentes que son accesibles y que todavía no hemos explorado*/
        vector<cordenadas> vecinos_validos_no_visitados;
        //elige aleatoriamente una direccion posible para ser explorada en el vector direcciones
        shuffle(direcciones.begin(), direcciones.end(), gen);  // baraja las direcciones entre el primero y la ultima posicion utilizando el generador de numeros aleatorios gen.
                                                               // esto permite que el patron del laberinto excarvado no siga el mismo patron predecible

        /*Este bucle for está diseñado para iterar a través de un conjunto predefinido de direcciones
        y calcular las coordenadas de una celda vecina basándose en la celda_actual.*/
        //Este bucle busca veninos_validos_no_visitados y agrega al vector vecinos validos no visitados.
        for (cordenadas direccion : direcciones) {
            cordenadas vecino = {
                celda_actual.first + direccion.first,      //calcula la cordenada x(fila) del vecino potencial
                celda_actual.second + direccion.second    ////calcula la cordenada y(columna) del vecino potencial
            };

            // Asegurarse de que el vecino esté DENTRO del área jugable (no en el borde)
            // y dentro de los límites generales del tablero
            if (vecino.first > 0 && vecino.first < Filas - 1 && // Mayor que 0, menor que Filas-1 (se excluye el borde exterior de las filas)
                vecino.second > 0 && vecino.second < Columnas - 1) { // Mayor que 0, menor que Columnas-1 (se excluye el borde exterior de las columnas)
                if (!visitado[vecino.first][vecino.second]) {    //si la celda nueva no fue visitada
                    vecinos_validos_no_visitados.push_back(vecino); //añade la celda no visitada al vector vecinos_validos_no_visitados.
                }
            }
        }

        // este bloque de codigo if se ejecuta solo si hay vecinos validos y no visitados disponibles para la celda actual.
        if (!vecinos_validos_no_visitados.empty()) {   //si vecinos validos no visitados no esta vacio elige uno de los vecinos al azar
            // si hay vecinos validos no visitados, elige uno de ellos al azar
            // uniform int distribution hace que cada elemento tenga la misma probabilidad de ser elegiso, si tengo 4 elementos en el vector, cada uno de ellos tienen el 25% de ser elegidos, o sea distribuye en forma igualitaria.
            uniform_int_distribution<> dist(0, vecinos_validos_no_visitados.size() - 1); //creo un objeto de la clase uniform llamado dist para elegir aleatoriamente vecinos validos no visitados.
            cordenadas vecino_elegido = vecinos_validos_no_visitados[dist(gen)];

            // Calcular la celda intermedia (la "pared" entre la celda actual y el vecino elegido)
            cordenadas celda_intermedia = {
                celda_actual.first + (vecino_elegido.first - celda_actual.first) / 2,      // sumo la cordenada x de la celda actual + la diferencia de distantcia entre ambas celdas.
                celda_actual.second + (vecino_elegido.second - celda_actual.second) / 2    // lo mismo para la columna.
            };

            //Marcar el vecino visitado como elegido
            visitado[vecino_elegido.first][vecino_elegido.second] = true;

            //Convertir la celda intermedia y el vecino elegido en pasajes
            tablero[celda_intermedia.first][celda_intermedia.second] = ' ';
            tablero[vecino_elegido.first][vecino_elegido.second] = ' ';

            //añadir el vecino elegido a la pila para explorarlo en la proxima iteracion(profundizar)
            pila.push(vecino_elegido);
        }
        // si no encuentra vecinos validos no visitados llego a un callejon sin salida
        // luego retrocede sacando la celda actual de la pila para continuar explorando desde la celda anterior.
        else {
            pila.pop();
        }
    }
}

// Función para encontrar el camino más corto usando BFS
vector<cordenadas> busqueda_camino_BFS(vector<vector<char>>& tablero, cordenadas celda_inicial, cordenadas celda_final) {
    int filas = tablero.size();
    int columnas = tablero[0].size();

    // inicializo el tablero con un vector de booleanos asignando a todas sus celda false(no visitadas)
    vector<vector<bool>> visitado(filas, vector<bool>(columnas, false));

    // queue de pares: {celda_actual, camino_hasta_celda}
    queue<pair<cordenadas, vector<cordenadas>>> q;

    // inicializo un vector con las direcciones posibles para explorar (arriba, abajo, izquierda, derecha)
    vector<cordenadas> direcciones = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    // Inicializar BFS
    visitado[celda_inicial.first][celda_inicial.second] = true;  // marco la celda actual origen como visitado
    q.push({ celda_inicial, {celda_inicial} });   // agrego la celda inicial a la cola

    //exploracion del laberinto bucle principal
    while (!q.empty()) {   // mientras la cola no esta vacia
        /*el algoritmo seguirá explorando celdas mientras haya celdas "pendientes" por visitar en la cola.
        Cuando la cola esté vacía, significa que se encontro la celda final y se devolvio el camino o
        no hay mas celdas alcanzables.*/
        cordenadas celda_actual = q.front().first;  //saco la primera celda de la cola que es un pair de cordenadas de la celda origen y lo almaceno en una variable (celda_actual)
        vector<cordenadas> camino_actual = q.front().second; // esta linea obtiene el camino que llevo a la celda_actual hasta este punto y lo almaceno en la variable camino_actual
        q.pop();   // esta linea elimina la linea que acabamos de procesar en la cola, retira el elemento que esta al frente de la cola.

        // Si hemos llegado a la celda final, retornamos el camino
        if (celda_actual.first == celda_final.first && celda_actual.second == celda_final.second) {  // verifica si la cordenada x, y de la celda actual es la misma que las cordenadas x,y de la celda final
            return camino_actual;   // retorna el camino almacenado hasta llegar al punto final.
        }

        // Si no ha llegado a destino verifica las celdas vecinas(arriba, abajo, izquierda, derecha) de la celda actual
        for (cordenadas dir : direcciones) {
            int nueva_fila = celda_actual.first + dir.first;
            int nueva_columna = celda_actual.second + dir.second;

            // validacion de los vecinos
            // Verificar límites y si es un camino y no ha sido visitado
            if (nueva_fila >= 0 && nueva_fila < filas &&
                nueva_columna >= 0 && nueva_columna < columnas &&
                tablero[nueva_fila][nueva_columna] != '#' &&    // accede al caracter que se encuentra en la posicion del vecino y verifica si no es una pared
                !visitado[nueva_fila][nueva_columna]) {         // accede al estado visitado de la celda vecina, por lo que verifica si la celda no ha sido visitada previamente. 

                visitado[nueva_fila][nueva_columna] = true;     // si el vecino es valido lo marca como visitado
                vector<cordenadas> nuevo_camino = camino_actual;   // crea un nuevo camino copiando el camino actual agregando a esta nueva celda de tipo vector.
                nuevo_camino.push_back({ nueva_fila, nueva_columna });  // agrega esta nueva celda al nuevo camino de tipo vector.
                q.push({ {nueva_fila, nueva_columna}, nuevo_camino });  // agrego un nuevo elemento a la cola con su nueva celda y su nuevo camino.
            }
        }
    }

    // Si no se encuentra un camino, retorna un vector vacío
    return {};
}

int main() {
    vector<vector<char>> matriz = definir_matriz();

    // Generar el laberinto
    generador_laberinto(matriz);

    // --- Colocar la entrada y salida en el perímetro ---
    cordenadas celda_entrada = { 0, 1 };  // celda de entrada fija en la posicion 0, 1 de la matriz
    cordenadas celda_salida = { matriz.size() - 1, matriz[0].size() - 2 };   // el primer elemento optiene la cantidad de filas menos 1, el segundo elemento optiene 
                                                                             // la cantidad de columnas de la primera fila asumiento que todas las filas tienen
                                                                             // la misma cantidad de columnas

    matriz[celda_entrada.first][celda_entrada.second] = 'E'; // (0,1) sería el punto de entrada en el borde superior
    matriz[celda_entrada.first + 1][celda_entrada.second] = ' '; // Accede a la fila inmediatamente inferior y a la misma columna que E marcando el camino de inicio.

    matriz[celda_salida.first][celda_salida.second] = 'S'; // (Filas-1, Columnas-2)
    matriz[celda_salida.first - 1][celda_salida.second] = ' '; // accede a la penultima fila del tablero y a la columna de S, marcando el punto final del camino.

    cout << "Laberinto generado:" << endl;
    mostrar_matriz(matriz);

    cout << "\nBuscando el camino mas corto con BFS..." << endl;
    vector<cordenadas> camino_mas_corto = busqueda_camino_BFS(matriz, celda_entrada, celda_salida);

    if (!camino_mas_corto.empty()) {    // si el vector de caminos no esta vacio.
        cout << "Camino mas corto encontrado (longitud: " << camino_mas_corto.size() << " pasos):" << endl;   // imprimir la cantidad de pasos que dio para llegar a la salida, 
                                                                                                              // determinando el tamaño del vector camino mas corto
        // Marcar el camino en el laberinto para visualizarlo
        vector<vector<char>> matriz_con_camino = matriz;   // creo una copia del tablero y lo guardo en una variable llamado matriz_con_camino
        for (const auto& p : camino_mas_corto) {           // bucle para iterar las cordenadas de los caminos almacenados en camino mas corto y lo guardo en una variable temporal p(tipo pari de cordenadas x, y)
            if (matriz_con_camino[p.first][p.second] != 'E' && matriz_con_camino[p.first][p.second] != 'S') {   // condicion, si el cada una de las cordenadas del camino es distinto E o S
                matriz_con_camino[p.first][p.second] = '.'; // Marca el camino con '.'
            }
        }
        mostrar_matriz(matriz_con_camino);
    }
    else {
        cout << "No se encontro un camino desde la entrada a la salida." << endl;
    }

    return 0;
}