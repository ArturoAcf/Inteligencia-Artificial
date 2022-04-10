#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

using namespace std;

// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
	if(!hayPlan){
		actual.fila=sensores.posF;
		actual.columna=sensores.posC;
		actual.orientacion=sensores.sentido;
		destino.fila=sensores.destinoF;
		destino.columna=sensores.destinoC;
		hayPlan=pathFinding(sensores.nivel, actual, destino, plan);
	}

	Action sigAccion;
	if(hayPlan && plan.size()>0){
		sigAccion=plan.front();
		plan.erase(plan.begin());
	}

	return sigAccion;
}


// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan){
	switch (level){
		case 0: cout << "Busqueda en profundad\n";
            return pathFinding_Profundidad(origen,destino,plan);
						break;
		case 1: cout << "Busqueda en Anchura\n";
            return pathFinding_Anchura(origen,destino,plan);
						break;
		case 2: cout << "Busqueda Costo Uniforme\n";
            return pathFinding_CostoUniforme(origen,destino,plan);
						break;
		case 3: cout << "Busqueda A*\n";
            return pathFinding_A_Estrella(origen,destino,plan);
						break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M')
		return true;
	else
	  return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}




struct nodo{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}

// Nombre: Arturo Alonso Carbonero
// Grupo: 2ºA - A2
// NOTAS
// -Al realizar pruebas por mi cuenta nunca se ha "colgado" tal y como debe
// suceder según las indicaciones del guion, pero como desconozco qué puede
// probocar dicho comportamiento, no se si la realización de la P2E es correcta en
// relación a ese aspecto.
// Contempla Anchura, costo uniforme y A* (Además de lo proporcionado).
/////////////////////
//BÚSQUEDA EN ANCHURA
/////////////////////
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
	cout << "Calculando plan utilizando búsqueda en Anchura\n";

	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> cola;											// Lista de Abiertos

  nodo inic;
	// Inicialización del nodo inic
	inic.st = origen;
	inic.secuencia.empty();

	cola.push(inic);

  while (!cola.empty() and (inic.st.fila!=destino.fila or inic.st.columna != destino.columna)){

		generados.insert(inic.st);
		// NODOS HIJOS:
		//Generar descendiente de girar a la derecha
		nodo hijoTurnR = inic;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = inic;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = inic;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				cola.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la cola
		// Como puede comprobarse, este fragmento del código difiere del fragmento del algoritmo anterior.
		inic=cola.front();
		while(!cola.empty() && generados.find(inic.st)!=generados.end()){
			cola.pop();
			inic=cola.front();
		}
	}

  cout << "Terminada la busqueda\n";

	if (inic.st.fila == destino.fila and inic.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = inic.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}

	return false;
}

/////////////////////////
//BÚSQUEDA COSTO UNIFORME
/////////////////////////

//Estado que va a incluir el coste para pasar a la siguiente casilla
struct estadoCoste{
	estado st;
	int coste;
};

//Copio la Implementación del strcut ComparaEstados proporcionado por la práctica y
//lo adapto a este nuevo tipo de estado con coste.
struct comparaEstadosC{
	bool operator()(const estadoCoste &esA, const estadoCoste &esB) const{
		if ((esA.st.fila > esB.st.fila) or (esA.st.fila == esB.st.fila and esA.st.columna > esB.st.columna) or
	      (esA.st.fila == esB.st.fila and esA.st.columna == esB.st.columna and esA.st.orientacion > esB.st.orientacion))
			return true;
		else
			return false;
	}
};

//Tipo de nodo que emplearé en costo uniforme
struct nodoConCoste{
	estadoCoste st;
	list<Action> secuencia;
	bool zapatillas; // Variable para almacenar si en un nodo hemos cogido las zapatillas
	bool bikini; // Variable para almacenar si en un nodo hemos cogido el bikini
	char letra_casilla; // Variable para almacenar la letra de la casilla
};

//Clase para ordenar los nodos en la cola con prioridad
class compararNodos{
	public:
		bool operator()(const nodoConCoste &nodoA, const nodoConCoste &nodoB){
			return (nodoA.st.coste > nodoB.st.coste);
		}
};

//Cálculo del coste del avanze de casilla
int costeDelAvance(const char cas, bool zapas, bool bik){
	int valorDeCoste=1;

	if(cas=='A'){
		if(bik==true){
			valorDeCoste=10;
		}else{
			valorDeCoste=100;
		}
	}else if(cas=='B'){
		if(zapas==true){
			valorDeCoste=5;
		}else{
			valorDeCoste=50;
		}
	}else if(cas=='T'){
		valorDeCoste=2;
	}else if(cas=='S'){
		valorDeCoste=1;
	}

	return valorDeCoste;
}

//Implementación de costo uniforme
bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan usando costo unifome\n";
	plan.clear();

	set<estadoCoste,comparaEstadosC> cerrados; //Lista de Cerrados
	set<estadoCoste,comparaEstadosC> abiertos_aux; //Lista de abiertos auxiliar par suplir la falta de "find" en las priority queue
	priority_queue<nodoConCoste, vector<nodoConCoste>, compararNodos> abiertos; //Lista de abiertos

	nodoConCoste inic;
	//Inicialización del nodo inic
	inic.st.st=origen;
	inic.letra_casilla=mapaResultado[inic.st.st.fila][inic.st.st.columna];
	inic.zapatillas=zapatillas;
	inic.bikini=bikini;
	inic.st.coste=0;
	inic.secuencia.empty();

	abiertos.push(inic);
	abiertos_aux.insert(inic.st);

	while(!abiertos.empty() && (inic.st.st.fila!=destino.fila || inic.st.st.columna!=destino.columna)){
		//Sacar el nodo de abiertos
		abiertos.pop();
		abiertos_aux.erase(abiertos_aux.find(inic.st));

		//Pasarlo a Cerrados
		cerrados.insert(inic.st);

		//Actualización de valores
		inic.letra_casilla=mapaResultado[inic.st.st.fila][inic.st.st.columna];

		if(inic.letra_casilla=='K'){
			inic.bikini=true;
		}else if(inic.letra_casilla=='D'){
			inic.zapatillas=true;
		}

		//Generar descendiente avanzzar de frente
		nodoConCoste hijoForward=inic;
		if(!HayObstaculoDelante(hijoForward.st.st)){
			if(cerrados.find(hijoForward.st)==cerrados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				hijoForward.st.coste+=costeDelAvance(inic.letra_casilla, inic.zapatillas, inic.bikini);

				//Si el descendiente que inetnta buscar ya existe comprobamos si el valor de su casilla
				//es mayor o no que el del actual. Si se cumple, introducimos el nuevo descendiente.
				auto itr=abiertos_aux.find(hijoForward.st);
				if(itr==abiertos_aux.end()){
					abiertos.push(hijoForward);
					abiertos_aux.insert(hijoForward.st);
				}else if(itr->coste > hijoForward.st.coste){
					abiertos_aux.erase(itr);
					abiertos_aux.insert(hijoForward.st);
				}
			}
		}

		//Generar descendiente girar derecha
		nodoConCoste hijoTurnR=inic;
		hijoTurnR.st.st.orientacion=(hijoTurnR.st.st.orientacion+1)%4;
		if(cerrados.find(hijoTurnR.st)==cerrados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.st.coste+=costeDelAvance(inic.letra_casilla, inic.zapatillas, inic.bikini);

			auto itr=abiertos_aux.find(hijoTurnR.st);
			if(itr==abiertos_aux.end()){
				abiertos.push(hijoTurnR);
				abiertos_aux.insert(hijoTurnR.st);
			}else if(itr->coste > hijoTurnR.st.coste){
					abiertos_aux.erase(itr);
					abiertos_aux.insert(hijoTurnR.st);
				}
			}

		//Generar descendiente girar izquierda
		nodoConCoste hijoTurnL=inic;
		hijoTurnL.st.st.orientacion=(hijoTurnL.st.st.orientacion+3)%4;
		if(cerrados.find(hijoTurnL.st)==cerrados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.st.coste+=costeDelAvance(inic.letra_casilla, inic.zapatillas, inic.bikini);

			auto itr=abiertos_aux.find(hijoTurnL.st);
			if (itr==abiertos_aux.end()){
				abiertos.push(hijoTurnL);
				abiertos_aux.insert(hijoTurnL.st);
			}else if(itr->coste > hijoTurnL.st.coste){
					abiertos_aux.erase(itr);
					abiertos_aux.insert(hijoTurnL.st);
				}
			}

		//Siguiente valor de la cola con prioridad
		if(!abiertos.empty()){
			inic=abiertos.top();
		}
	}

		cout << "Terminada la busqueda\n";

	if (inic.st.st.fila == destino.fila and inic.st.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = inic.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);

		// Mostramos el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;

	}else{
		cout << "No se ha encontrado un plan\n";
	}

	return false;
}

/////////////
//Búsqueda A*
/////////////

//Cálculo de la distancia al destino
int distanciaDes(const estado &origen, const estado &destino){
	return abs(origen.fila-destino.fila)+abs(origen.columna-destino.columna);
}

//Estado
struct estadoEstrella{
	estado st;
	int func; //Función a minimizar
};

//Copio la Implementación del strcut ComparaEstados proporcionado por la práctica y
//lo adapto a este nuevo tipo de estado para A*
struct ComparaEstadosEstrella{
	bool operator()(const estadoEstrella &estA, const estadoEstrella &estB) const{
		if((estA.st.fila > estB.st.fila) or (estA.st.fila == estB.st.fila and estA.st.columna > estB.st.columna) or
	      (estA.st.fila == estB.st.fila and estA.st.columna == estB.st.columna and estA.st.orientacion > estB.st.orientacion))
			return true;
		else
			return false;
	}
};

//Tipo de nodo para A*
struct nodoAest{
	estadoEstrella st;
	list<Action> secuencia;
	bool zapatillas;
	bool bikini;
	char letra_casilla;
	int h; //Distancia total del nodo al destino
	int g; //Cste de avanzar al destino
};

//Comprar nodos
class CompararNodoEstrella{
	public:
		bool operator()(const nodoAest &nodoA, const nodoAest &nodoB){
			if(nodoA.st.func>nodoB.st.func){
				return true;
			}else if(nodoA.st.func==nodoB.st.func && nodoA.g>nodoB.g){
				return true;
			}else return false;
		}
};

//Coste del avance, igual que en el algortimo anterior
int costeDelAvanceAest(const char cas, bool zapas, bool bik){
	int valorDeCoste=1;

	if(cas=='A'){
		if(bik==true){
			valorDeCoste=10;
		}else{
			valorDeCoste=100;
		}
	}else if(cas=='B'){
		if(zapas==true){
			valorDeCoste=5;
		}else{
			valorDeCoste=50;
		}
	}else if(cas=='T'){
		valorDeCoste=2;
	}else if(cas=='S'){
		valorDeCoste=1;
	}

	return valorDeCoste;
}

//Algoritmo de A*
bool ComportamientoJugador::pathFinding_A_Estrella(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan usando A*\n";

	set<estadoEstrella,ComparaEstadosEstrella> cerrados; //Lista de Cerrados
	set<estadoEstrella,ComparaEstadosEstrella> abiertos_aux; //Lista de abiertos auxiliar
	priority_queue<nodoAest, vector<nodoAest>, CompararNodoEstrella> abiertos; //Lista de abiertos

	nodoAest inic;
	//Inicialización del nodo inic
	inic.st.st=origen;
	inic.st.func=0;
	inic.secuencia.empty();
	inic.zapatillas=zapatillas;
	inic.bikini=bikini;
	inic.letra_casilla=mapaResultado[inic.st.st.fila][inic.st.st.columna];
	inic.g=0;
	inic.h=0;

	//Similar Implementación a costo uniforme, ya que únicamente difiere de este
	//en que tiene en cuenta la función f, no solo el coste:
	abiertos.push(inic);
	abiertos_aux.insert(inic.st);

	while(!abiertos.empty() && (inic.st.st.fila!=destino.fila || inic.st.st.columna!=destino.columna)){
		//Sacar el nodo de abiertos
		abiertos.pop();
		abiertos_aux.erase(abiertos_aux.find(inic.st));

		//Pasarlo a cerrados
		cerrados.insert(inic.st);

		//Actualización de valores
		inic.letra_casilla=mapaResultado[inic.st.st.fila][inic.st.st.columna];

		if(inic.letra_casilla=='K'){
			inic.bikini=true;
		}else if(inic.letra_casilla=='D'){
			inic.zapatillas=true;
		}

		//Generar descendiente avanzar de frente
		nodoAest hijoForward=inic;
		if(!HayObstaculoDelante(hijoForward.st.st)){
			if(cerrados.find(hijoForward.st)==cerrados.end()){
				hijoForward.secuencia.push_back(actFORWARD);

				hijoForward.g+=costeDelAvanceAest(inic.letra_casilla, inic.zapatillas, inic.bikini);
				hijoForward.h=distanciaDes(origen, destino);
				hijoForward.st.func=hijoForward.h+hijoForward.g;

				auto itr=abiertos_aux.find(hijoForward.st);
				if(itr==abiertos_aux.end()){
					abiertos.push(hijoForward);
					abiertos_aux.insert(hijoForward.st);
				}else if(itr->func>hijoForward.st.func){
					abiertos_aux.erase(itr);
					abiertos_aux.insert(hijoForward.st);
				}
			}
		}

		//Generar descendiente girar derecha
		nodoAest hijoTurnR=inic;
		hijoTurnR.st.st.orientacion=(hijoTurnR.st.st.orientacion+1)%4;
			if(cerrados.find(hijoTurnR.st)==cerrados.end()){
				hijoTurnR.secuencia.push_back(actTURN_R);

				hijoTurnR.g+=costeDelAvanceAest(inic.letra_casilla, inic.zapatillas, inic.bikini);
				hijoTurnR.h=distanciaDes(origen, destino);
				hijoTurnR.st.func=hijoTurnR.h+hijoTurnR.g;

				auto itr=abiertos_aux.find(hijoTurnR.st);
				if (itr==abiertos_aux.end()){
					abiertos.push(hijoTurnR);
					abiertos_aux.insert(hijoTurnR.st);
				}else if(itr->func > hijoTurnR.st.func){
						abiertos_aux.erase(itr);
						abiertos_aux.insert(hijoTurnR.st);
				}
			}

			//Generar descendiente girar izquierda
			nodoAest hijoTurnL=inic;
			hijoTurnL.st.st.orientacion=(hijoTurnL.st.st.orientacion+1)%4;
				if(cerrados.find(hijoTurnL.st)==cerrados.end()){
					hijoTurnL.secuencia.push_back(actTURN_R);

					hijoTurnL.g+=costeDelAvanceAest(inic.letra_casilla, inic.zapatillas, inic.bikini);
					hijoTurnL.h=distanciaDes(origen, destino);
					hijoTurnL.st.func=hijoTurnL.h+hijoTurnL.g;

					auto itr=abiertos_aux.find(hijoTurnL.st);
					if (itr==abiertos_aux.end()){
						abiertos.push(hijoTurnL);
						abiertos_aux.insert(hijoTurnL.st);
					}else if(itr->func > hijoTurnL.st.func){
							abiertos_aux.erase(itr);
							abiertos_aux.insert(hijoTurnL.st);
					}
				}

				//Siguiente valor de la cola con prioridad
				if(!cerrados.empty()){
					inic=abiertos.top();
				}
			}

			cout << "Terminada la busqueda\n";

		if (inic.st.st.fila == destino.fila and inic.st.st.columna == destino.columna){
			cout << "Cargando el plan\n";
			plan = inic.secuencia;
			cout << "Longitud del plan: " << plan.size() << endl;
			PintaPlan(plan);

			// Mostramos el plan en el mapa
			VisualizaPlan(origen, plan);
			return true;

		}else{
			cout << "No se ha encontrado un plan\n";
		}

		return false;
}


// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
