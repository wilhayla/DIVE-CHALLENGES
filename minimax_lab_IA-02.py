import math
import time
import random

class Jugar_Gato_Raton:
    def __init__(self):
        self.tablero = [["." for _ in range(5)] for _ in range(5)]
        self.gato = "G"
        self.raton = "R"
        self.pos_raton = [0, 0]  # [fila, columna]
        self.pos_gato = [4, 0]   # [fila, columna]
        
        # --- NUEVOS CONTADORES DE MOVIMIENTOS ---
        self.movimientos_gato_realizados = 0
        self.movimientos_raton_realizados = 0
        self.limite_movimientos = 10 # Definir el límite máximo de movimientos

        self.tablero[self.pos_raton[0]][self.pos_raton[1]] = self.raton
        self.tablero[self.pos_gato[0]][self.pos_gato[1]] = self.gato

    def imprimir_tablero(self):
        for fila in self.tablero:
            print(" ".join(fila))
        print("-" * 10) # Separador para mejor visualización

    # --- Métodos auxiliares para Minimax ---

    def _obtener_movimientos_posibles(self, personaje_pos, tipo_personaje):
        """
        Retorna una lista de nuevas posiciones válidas a las que el personaje puede moverse.
        No modifica el estado actual del juego.
        """
        movimientos = []  # lista que almacenara todas las nuevas posicioines validas
        fila_actual, col_actual = personaje_pos   # desempueta la lista en dos variables separadas
        
        # Define los cambios direccionables posibles para el movimiento (arriba, abajo, izquierda, derecha)
        direcciones = {
            "w": (-1, 0),
            "s": (1, 0),
            "a": (0, -1),
            "d": (0, 1)
        }
        # bucle que itera a traves de cada cambio direccional definido en el diccionario
        for dr, dc in direcciones.values():
            # para cada direccion esta linea calcula las posibles nuevas coordenadas sumando el cambio direccional (dr, dc)
            # a la fila actual y a la columna actual.
            nueva_fila, nueva_col = fila_actual + dr, col_actual + dc

            # Validar la nueva posición dentro de los límites del tablero
            if 0 <= nueva_fila < 5 and 0 <= nueva_col < 5:
                # El ratón no puede moverse a la posición del gato si el gato ya está allí.
                # (El gato sí puede moverse a la posición del ratón para capturarlo)
                if tipo_personaje == self.raton and [nueva_fila, nueva_col] == self.pos_gato:
                    continue
                
                movimientos.append([nueva_fila, nueva_col])
        return movimientos

    def _evaluar_estado(self, pos_gato_eval, pos_raton_eval):
        """
        Evalúa qué tan buena es una posición para el GATO.
        Una puntuación más alta significa mejor para el gato.
        """
        # Si el gato ha atrapado al ratón, es la mejor situación para el gato
        if pos_gato_eval == pos_raton_eval:
            return 1000  # Puntuacion mas alta de victoria para el gato. Resultado mas deseable.
        
        # si el gato no ha atrapado al raton
        # Calcular la distancia de Manhattan entre el gato y el ratón
        distancia_x = abs(pos_gato_eval[0] - pos_raton_eval[0])
        distancia_y = abs(pos_gato_eval[1] - pos_raton_eval[1])
        distancia_manhattan = distancia_x + distancia_y

        # El objetivo del gato es minimizar la distancia, así que una distancia menor
        # resulta en una puntuación más alta para el gato.
        # El 500 es un valor base para que la distancia reduzca desde ahí.
        puntuacion_distancia = 500 - distancia_manhattan

        # si el ratón está cerca de un borde, podría ser bueno para el gato (menos escapes)
        # Borde superior (fila 0) o inferior (fila 4)
        if pos_raton_eval[0] == 0 or pos_raton_eval[0] == 4:
            puntuacion_distancia += 10 # Pequeño bonus si el ratón está en un borde horizontal
        # Borde izquierdo (col 0) o derecho (col 4)
        if pos_raton_eval[1] == 0 or pos_raton_eval[1] == 4:
            puntuacion_distancia += 10 # Pequeño bonus si el ratón está en un borde vertical

        return puntuacion_distancia

    def _minimax(self, pos_gato_actual, pos_raton_actual, profundidad, es_turno_maximizador):
        """
        Implementa el algoritmo Minimax para encontrar la mejor jugada.
        La función tiene como objetivo encontrar el movimiento óptimo para el jugador actual (ya sea el gato o el ratón) explorando los posibles estados futuros 
        del juego hasta una cierta profundidad. Asumo que el gato es el "maximizador" (intentando maximizar su puntuación, 
        lo que para mi significa atrapar al ratón) y que el ratón es el "minimizador" (intentando minimizar la puntuación del gato, 
        lo que para mi significa evitar ser atrapado).
        """
        # Caso base: Si la profundidad es 0 o el juego ha terminado (gato atrapa ratón)
        # Condicion de parada. Aqui se detiene la recurcion y la funcion minimax devolvera un valor si:
        if profundidad == 0 or pos_gato_actual == pos_raton_actual:
            return self._evaluar_estado(pos_gato_actual, pos_raton_actual)

        if es_turno_maximizador:  # Turno del Gato (Maximizador)
            max_eval = -math.inf
            movimientos_gato = self._obtener_movimientos_posibles(pos_gato_actual, self.gato)
            for nuevo_pos_gato in movimientos_gato:
                # Llamada recursiva para el turno del ratón (minimizador)
                evaluacion = self._minimax(nuevo_pos_gato, pos_raton_actual, profundidad - 1, False)
                max_eval = max(max_eval, evaluacion)
            return max_eval
        else:  # Turno del Ratón (Minimizador)
            min_eval = math.inf
            movimientos_raton = self._obtener_movimientos_posibles(pos_raton_actual, self.raton)
            for nuevo_pos_raton in movimientos_raton:
                # Llamada recursiva para el turno del gato (maximizador)
                evaluacion = self._minimax(pos_gato_actual, nuevo_pos_raton, profundidad - 1, True)
                min_eval = min(min_eval, evaluacion)
            return min_eval

    def _obtener_mejor_movimiento_ia(self, pos_gato_actual, pos_raton_actual, profundidad, es_turno_gato_ia):
        """
        Calcula el mejor movimiento para el IA (gato o ratón) usando Minimax.
        Retorna la mejor nueva posición.
        """
        mejor_movimiento = None
        
        if es_turno_gato_ia: # Gato es la IA (Maximiza)
            mejor_valor = -math.inf
            movimientos_posibles = self._obtener_movimientos_posibles(pos_gato_actual, self.gato)
            for movimiento in movimientos_posibles:
                # Evaluar el estado después de este movimiento, asumiendo que el ratón jugará óptimamente
                valor_actual = self._minimax(movimiento, pos_raton_actual, profundidad - 1, False)
                if valor_actual > mejor_valor:
                    mejor_valor = valor_actual
                    mejor_movimiento = movimiento
        else: # Ratón es la IA (Minimiza)
            mejor_valor = math.inf
            movimientos_posibles = self._obtener_movimientos_posibles(pos_raton_actual, self.raton)
            for movimiento in movimientos_posibles:
                # Evaluar el estado después de este movimiento, asumiendo que el gato jugará óptimamente
                valor_actual = self._minimax(pos_gato_actual, movimiento, profundidad - 1, True)
                if valor_actual < mejor_valor: # El ratón minimiza, busca el valor más bajo
                    mejor_valor = valor_actual
                    mejor_movimiento = movimiento

        return mejor_movimiento

    # --- Método de movimiento ahora solo para movimientos automáticos ---
    def mover_personaje(self, personaje_simbolo, nueva_posicion):
        """
        Realiza el movimiento de un personaje a una nueva posición.
        """
        # Determinar qué personaje se está moviendo y obtener su posición actual
        if personaje_simbolo == self.raton:
            pos_actual = self.pos_raton
            # ---- VERIFICAR LIMITE DE MOVIMIENTOS PARA EL RATON ---
            if self.movimientos_raton_realizados >= self.limite_movimientos:
                print(f"El RATON ha alcanzado su límite de {self.limite_movimientos} movimientos.")
                return False # indica que el movimiento no se realizó
        elif personaje_simbolo == self.gato:
            pos_actual = self.pos_gato
            # ---- VERIFICAR LIMITE DE MOVIMIENTOS PARA EL GATO ---
            if self.movimientos_gato_realizados >= self.limite_movimientos:
                print(f"El GATO ha alcanzado su límite de {self.limite_movimientos} movimientos.")
                return False # indica que el movimiento no se realizó
            
        else:
            print("Personaje no válido.")
            return False
        
        fila_actual, col_actual = pos_actual[0], pos_actual[1]
        nueva_fila, nueva_col = nueva_posicion[0], nueva_posicion[1]

        # Actualizar el tablero
        self.tablero[fila_actual][col_actual] = "." # Borrar posición antigua
        self.tablero[nueva_fila][nueva_col] = personaje_simbolo # Poner en nueva posición

        # Actualizar la posición almacenada del personaje
        if personaje_simbolo == self.raton:
            self.pos_raton = [nueva_fila, nueva_col]
            self.movimientos_raton_realizados += 1 # Incrementar contador
            print(f"Movimientos del Ratón: {self.movimientos_raton_realizados}/{self.limite_movimientos}")
        else: # self.gato
            self.pos_gato = [nueva_fila, nueva_col]
            self.movimientos_gato_realizados += 1 # Incrementar contador
            print(f"Movimientos del Gato: {self.movimientos_gato_realizados}/{self.limite_movimientos}")
            
        print(f"{personaje_simbolo} movido a ({nueva_fila},{nueva_col}).")
        return True  # indica que el movimiento se realizó exitosamente
    
    # --- Método principal para jugar con IA ---
    def jugar_auto(self, profundidad_ia):
        """
        Inicia el juego con ambos personajes controlados por IA.
        profundidad_ia: Cuántos movimientos futuros considerará la IA.
        """
        # Elegir aleatoriamente que personaje juega primero
        turno = random.choice([self.gato, self.raton])
        print(f"¡El primer turno es para el {turno}!")

        while self.pos_gato != self.pos_raton:
            self.imprimir_tablero()
            time.sleep(0.5) # Pausa para ver el movimiento

            # --- Revisar límite de movimientos antes de cada turno ---
            # condifion de empate general que termina inmediatamente si ambos jugadores han agotado sus movimientos.
            if self.movimientos_gato_realizados >= self.limite_movimientos and self.movimientos_raton_realizados >= self.limite_movimientos:
                print("¡Ambos jugadores han alcanzado su límite de movimientos! Empate.")
                break
            
            if turno == self.gato:
                print("--- Turno del Gato (IA) ---")
                if self.movimientos_gato_realizados >= self.limite_movimientos:
                    print("El Gato ya no puede moverse.")
                    turno = self.raton # Cambiar turno si el gato no puede moverse
                    continue

                print("El Gato (IA) está pensando...")
                # Obtener el mejor movimiento para el gato
                mejor_mov = self._obtener_mejor_movimiento_ia(self.pos_gato, self.pos_raton, profundidad_ia, True)
                if mejor_mov:
                    self.mover_personaje(self.gato, mejor_mov)
                else:
                    print("El Gato no tiene movimientos válidos. ¡Fin del juego!")
                    break
                turno = self.raton # Cambiar turno al ratón

            elif turno == self.raton:
                print("--- Turno del Ratón (IA) ---")
                if self.movimientos_raton_realizados >= self.limite_movimientos:
                    print("El Ratón ya no puede moverse.")
                    turno = self.gato # Cambiar turno si el ratón no puede moverse
                    continue

                print("El Ratón (IA) está pensando...")
                # Obtener el mejor movimiento para el ratón
                mejor_mov = self._obtener_mejor_movimiento_ia(self.pos_gato, self.pos_raton, profundidad_ia, False)
                if mejor_mov:
                    self.mover_personaje(self.raton, mejor_mov)
                else:
                    print("El Ratón no tiene movimientos válidos. ¡Fin del juego!")
                    break
                turno = self.gato # Cambiar turno al gato
            
            # Verificar si el gato atrapó al ratón después de cada movimiento
            if self.pos_gato == self.pos_raton:
                self.imprimir_tablero()
                print("¡Juego terminado! El Gato atrapó al Ratón.")
                break
        
        print("Juego Terminado.")
    
# --- Iniciar el juego ---

juego = Jugar_Gato_Raton()

print("Bienvenido al juego Gato y Ratón con IA!")
print("Ambos jugadores serán controlados por la Inteligencia Artificial.")
print("El juego comenzará automáticamente.")

# La profundidad de la IA se establece directamente
profundidad_ia_elegida = 5
juego.jugar_auto(profundidad_ia=profundidad_ia_elegida)