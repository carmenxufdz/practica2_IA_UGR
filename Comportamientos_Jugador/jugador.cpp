#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <map>

///////////////////////////////
// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;
	if(sensores.nivel != 4)
	{
		// Incluir aquí el comportamiento del agente jugador
		if(!hayPlan){
			//Invocar al metodo de busqueda
			cout << "Calculando un nuevo plan\n";
			c_state.jugador.f = sensores.posF;
			c_state.jugador.c = sensores.posC;
			c_state.jugador.brujula = sensores.sentido;

			c_state.sonambulo.f = sensores.SONposF;
			c_state.sonambulo.c = sensores.SONposC;
			c_state.sonambulo.brujula = sensores.SONsentido;

			c_state.J_bikini = false;
			c_state.J_zapatillas = false;
			c_state.S_bikini = false;
			c_state.S_zapatillas = false;

			goal.f = sensores.destinoF;
			goal.c = sensores.destinoC;

			switch(sensores.nivel)
			{
				case 0:
					plan = AnchuraSoloJugador(c_state, goal, mapaResultado);
					break;
				case 1:
					plan = AnchuraJugadorYSonambulo(c_state, goal, mapaResultado);
					break;
				case 2:
					plan = CostoUniformeJugador(c_state, goal, mapaResultado);
					break;
				case 3:
					plan = AlgoritmoA(c_state, goal, mapaResultado);
					break;
			}
			
			if(plan.size() > 0){
				VisualizaPlan(c_state,plan);
				hayPlan = true;
			}
		}
		
		if(hayPlan and plan.size()>0){
			cout << "Ejecutando la siguiente accion del plan\n";
			accion = plan.front();
			plan.pop_front();
		}

		if(plan.size()==0){
			cout << "Se completo el plan\n";
			hayPlan = false;
		}
	}
	else
	{
		// Solucion nivel 4
	}
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// FUNCIONES AUXILIARES ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/** Devuelve si una ubicacion en el mapa es transitable para el agente */
bool ComportamientoJugador::CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa)
{
	return (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M');
}

/** Devuelve la ubicacion siguiente segun el avance del agente */
ubicacion ComportamientoJugador::NextCasilla(const ubicacion &pos)
{
	ubicacion next = pos;
	switch(pos.brujula)
	{
		case norte:
			next.f = pos.f-1;
			break;
		case noreste:
			next.f = pos.f-1;
			next.c = pos.c+1;
			break;
		case este:
			next.c = pos.c+1;
			break;
		case sureste:
			next.f = pos.f+1;
			next.c = pos.c+1;
			break;
		case sur:
			next.f = pos.f+1;
			break;
		case suroeste:
			next.f = pos.f+1;
			next.c = pos.c-1;
			break;
		case oeste:
			next.c = pos.c-1;
			break;
		case noroeste:
			next.f = pos.f-1;
			next.c = pos.c-1;
			break;
	}
	return next;
}

/** Devuelve el estado que se genera si el agente puede avanzar.
 *  Si no puede avanzar, se devuelve como salida el mismo estado de entrada.
 */
estado ComportamientoJugador::apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa)
{
	estado st_result = st;
	ubicacion sig_ubicacion;
	switch(a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(st.jugador);
			if(CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.sonambulo.f and sig_ubicacion.c == st.sonambulo.c))
				st_result.jugador = sig_ubicacion;
			break;
		case actTURN_L:
			st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 6)%8);
			break;
		case actTURN_R:
			st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 2)%8);
			break;
		case actSON_FORWARD:
			sig_ubicacion = NextCasilla(st.sonambulo);
			if(CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion.f == st.jugador.f and sig_ubicacion.c == st.jugador.c))
				st_result.sonambulo = sig_ubicacion;
			break;
		case actSON_TURN_SL:
			st_result.sonambulo.brujula = static_cast<Orientacion>((st_result.sonambulo.brujula + 7)%8);
			break;
		case actSON_TURN_SR:
			st_result.sonambulo.brujula = static_cast<Orientacion>((st_result.sonambulo.brujula + 1)%8);
			break;
	}
	return st_result;
}

bool ComportamientoJugador::SonambuloALaVista(ubicacion &jugador, ubicacion &sonambulo)
{

	switch(jugador.brujula){
		case norte:

			if(jugador.c == sonambulo.c){
				if((jugador.f-1) == sonambulo.f)	// posicion 2
					return true;
				if((jugador.f-2) == sonambulo.f)	// posicion 6
					return true;
				if((jugador.f-3) == sonambulo.f)	// posicion 12
					return true;
			}

			if(jugador.c == (sonambulo.c-1)){
				if((jugador.f-1) == sonambulo.f)	// posicion 1
					return true;
				if((jugador.f-2) == sonambulo.f)	// posicion 5
					return true;
				if((jugador.f-3) == sonambulo.f)	// posicion 11
					return true;
			}

			if(jugador.c == (sonambulo.c-2)){
				if((jugador.f-2) == sonambulo.f)	// posicion 4
					return true;
				if((jugador.f-3) == sonambulo.f)	// posicion 10
					return true;
			}

			if(jugador.c == (sonambulo.c-3)){
				if((jugador.f-3) == sonambulo.f)	// posicion 9
					return true;
			}

			if(jugador.c == (sonambulo.c+1)){
				if((jugador.f-1) == sonambulo.f)	// posicion 3
					return true;
				if((jugador.f-2) == sonambulo.f)	// posicion 7
					return true;
				if((jugador.f-3) == sonambulo.f)	// posicion 13
					return true;
			}

			if(jugador.c == (sonambulo.c+2)){
				if((jugador.f-2) == sonambulo.f)	// posicion 8
					return true;
				if((jugador.f-3) == sonambulo.f)	// posicion 14
					return true;
			}

			if(jugador.c == (sonambulo.c+3)){
				if((jugador.f-3) == sonambulo.f)	// posicion 15
					return true;
			}

		break;

		case noreste:
			
			if(jugador.f == sonambulo.f){
				if((jugador.c+1) == sonambulo.c)	// posicion 3
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 8
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 15
					return true;
			}

			if(jugador.f == (sonambulo.f-1)){
				if(jugador.c == sonambulo.c)		// posicion 1
					return true;
				if((jugador.c+1) == sonambulo.c)	// posicion 2
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 7
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 14
					return true;
			}

			if(jugador.f == (sonambulo.f-2)){
				if(jugador.c == sonambulo.c)		// posicion 4
					return true;
				if((jugador.c+1) == sonambulo.c)	// posicion 5
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 6
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 13
					return true;
			}

			if(jugador.f == (sonambulo.f-3)){
				if(jugador.c == sonambulo.c)		// posicion 9
					return true;
				if((jugador.c+1) == sonambulo.c)	// posicion 10
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 11
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 12
					return true;
			}

		break;

		case este:

			if(jugador.f == sonambulo.f){
				if((jugador.c+1) == sonambulo.c)	// posicion 2
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 6
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 12
					return true;
			}

			if(jugador.f == (sonambulo.f-1)){
				if((jugador.c+1) == sonambulo.c)	// posicion 1
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 5
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 11
					return true;
			}

			if(jugador.f == (sonambulo.f-2)){
				if((jugador.c+2) == sonambulo.c)	// posicion 4
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 10
					return true;
			}

			if(jugador.f == (sonambulo.f-3)){
				if((jugador.c+3) == sonambulo.c)	// posicion 9
					return true;
			}

			if(jugador.f == (sonambulo.f+1)){
				if((jugador.c+1) == sonambulo.c)	// posicion 3
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 7
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 13
					return true;
			}

			if(jugador.f == (sonambulo.f+2)){
				if((jugador.c+2) == sonambulo.c)	// posicion 8
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 14
					return true;
			}

			if(jugador.f == (sonambulo.f+3)){
				if((jugador.c+3) == sonambulo.c)	// posicion 15
					return true;
			}
			
		break;

		case sureste:
			
			if(jugador.f == sonambulo.f){
				if((jugador.c+1) == sonambulo.c)	// posicion 3
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 8
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 15
					return true;
			}

			if(jugador.f == (sonambulo.f+1)){
				if(jugador.c == sonambulo.c)		// posicion 1
					return true;
				if((jugador.c+1) == sonambulo.c)	// posicion 2
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 7
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 14
					return true;
			}

			if(jugador.f == (sonambulo.f+2)){
				if(jugador.c == sonambulo.c)		// posicion 4
					return true;
				if((jugador.c+1) == sonambulo.c)	// posicion 5
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 6
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 13
					return true;
			}

			if(jugador.f == (sonambulo.f+3)){
				if(jugador.c == sonambulo.c)		// posicion 9
					return true;
				if((jugador.c+1) == sonambulo.c)	// posicion 10
					return true;
				if((jugador.c+2) == sonambulo.c)	// posicion 11
					return true;
				if((jugador.c+3) == sonambulo.c)	// posicion 12
					return true;
			}

		break;

		case sur:

			if(jugador.c == sonambulo.c){
				if((jugador.f+1) == sonambulo.f)	// posicion 2
					return true;
				if((jugador.f+2) == sonambulo.f)	// posicion 6
					return true;
				if((jugador.f+3) == sonambulo.f)	// posicion 12
					return true;
			}

			if(jugador.c == (sonambulo.c-1)){
				if((jugador.f+1) == sonambulo.f)	// posicion 3
					return true;
				if((jugador.f+2) == sonambulo.f)	// posicion 7
					return true;
				if((jugador.f+3) == sonambulo.f)	// posicion 13
					return true;
			}

			if(jugador.c == (sonambulo.c-2)){
				if((jugador.f+2) == sonambulo.f)	// posicion 8
					return true;
				if((jugador.f+3) == sonambulo.f)	// posicion 14
					return true;
			}

			if(jugador.c == (sonambulo.c-3)){
				if((jugador.f+3) == sonambulo.f)	// posicion 15
					return true;
			}

			if(jugador.c == (sonambulo.c+1)){
				if((jugador.f+1) == sonambulo.f)	// posicion 1
					return true;
				if((jugador.f+2) == sonambulo.f)	// posicion 5
					return true;
				if((jugador.f+3) == sonambulo.f)	// posicion 11
					return true;
			}

			if(jugador.c == (sonambulo.c+2)){
				if((jugador.f+2) == sonambulo.f)	// posicion 4
					return true;
				if((jugador.f+3) == sonambulo.f)	// posicion 10
					return true;
			}

			if(jugador.c == (sonambulo.c+3)){
				if((jugador.f+3) == sonambulo.f)	// posicion 9
					return true;
			}
			
		break;

		case suroeste:
			
			if(jugador.f == sonambulo.f){
				if((jugador.c-1) == sonambulo.c)	// posicion 3
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 8
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 15
					return true;
			}

			if(jugador.f == (sonambulo.f+1)){
				if(jugador.c == sonambulo.c)		// posicion 1
					return true;
				if((jugador.c-1) == sonambulo.c)	// posicion 2
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 7
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 14
					return true;
			}

			if(jugador.f == (sonambulo.f+2)){
				if(jugador.c == sonambulo.c)		// posicion 4
					return true;
				if((jugador.c-1) == sonambulo.c)	// posicion 5
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 6
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 13
					return true;
			}

			if(jugador.f == (sonambulo.f+3)){
				if(jugador.c == sonambulo.c)		// posicion 9
					return true;
				if((jugador.c-1) == sonambulo.c)	// posicion 10
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 11
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 12
					return true;
			}

		break;

		case oeste:

			if(jugador.f == sonambulo.f){
				if((jugador.c-1) == sonambulo.c)	// posicion 2
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 6
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 12
					return true;
			}

			if(jugador.f == (sonambulo.f-1)){
				if((jugador.c-1) == sonambulo.c)	// posicion 3
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 7
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 13
					return true;
			}

			if(jugador.f == (sonambulo.f-2)){
				if((jugador.c-2) == sonambulo.c)	// posicion 8
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 14
					return true;
			}

			if(jugador.f == (sonambulo.f-3)){
				if((jugador.c-3) == sonambulo.c)	// posicion 15
					return true;
			}

			if(jugador.f == (sonambulo.f+1)){
				if((jugador.c-1) == sonambulo.c)	// posicion 
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 5
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 11
					return true;
			}

			if(jugador.f == (sonambulo.f+2)){
				if((jugador.c-2) == sonambulo.c)	// posicion 4
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 10
					return true;
			}

			if(jugador.f == (sonambulo.f+3)){
				if((jugador.c-3) == sonambulo.c)	// posicion 9
					return true;
			}
			
		break;
		
		case noroeste:
			
			if(jugador.f == sonambulo.f){
				if((jugador.c-1) == sonambulo.c)	// posicion 3
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 8
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 15
					return true;
			}

			if(jugador.f == (sonambulo.f-1)){
				if(jugador.c == sonambulo.c)		// posicion 1
					return true;
				if((jugador.c-1) == sonambulo.c)	// posicion 2
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 7
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 14
					return true;
			}

			if(jugador.f == (sonambulo.f-2)){
				if(jugador.c == sonambulo.c)		// posicion 4
					return true;
				if((jugador.c-1) == sonambulo.c)	// posicion 5
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 6
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 13
					return true;
			}

			if(jugador.f == (sonambulo.f-3)){
				if(jugador.c == sonambulo.c)		// posicion 9
					return true;
				if((jugador.c-1) == sonambulo.c)	// posicion 10
					return true;
				if((jugador.c-2) == sonambulo.c)	// posicion 11
					return true;
				if((jugador.c-3) == sonambulo.c)	// posicion 12
					return true;
			}

		break;				

	}

	return false;
}

/** Pone a cero todos los elementos de una matriz */
void ComportamientoJugador::AnularMatriz(vector<vector<unsigned char>> &matriz)
{
	for(int i=0; i < matriz.size(); i++)
		for(int j=0; j < matriz[0].size(); j++)
			matriz[i][j] = 0;
}

// Devuelve el coste de avanzar en una casilla en el mapa
void ComportamientoJugador::getCoste(const Action &a, estado &st, const vector<vector<unsigned char>> &mapa){
	
	unsigned char casilla ;
	switch(a){
		case actFORWARD:
			casilla = mapa[st.jugador.f][st.jugador.c];
			switch (casilla){
				case 'A':
					if(!st.J_bikini)
						st.coste = 100;
					else
						st.coste = 10;
				break;

				case 'B':
					if(!st.J_zapatillas)
						st.coste = 50;
					else
						st.coste = 15;
				break;

				case 'T':
					st.coste = 2;
				break;

				default:
					st.coste = 1;
				break;
			}	
		break;
		case actTURN_L:
			casilla = mapa[st.jugador.f][st.jugador.c];
			switch (casilla){
				case 'A':
					if(!st.J_bikini)
						st.coste = 25;
					else
						st.coste = 5;
				break;

				case 'B':
					if(!st.J_zapatillas)
						st.coste = 5;
					else
						st.coste = 1;
				break;

				case 'T':
					st.coste = 2;
				break;

				default:
					st.coste = 1;
				break;
			}	
		break;
		
		case actTURN_R:
			casilla = mapa[st.jugador.f][st.jugador.c];
			switch (casilla){
				case 'A':
					if(!st.J_bikini)
						st.coste = 25;
					else
						st.coste = 5;
				break;

				case 'B':
					if(!st.J_zapatillas)
						st.coste = 5;
					else
						st.coste = 1;
				break;

				case 'T':
					st.coste = 2;
				break;

				default:
					st.coste = 1;
				break;
			}		
		break;

		case actSON_FORWARD:
			casilla = mapa[st.sonambulo.f][st.sonambulo.c];
			switch (casilla){
				case 'A':
					if(!st.J_bikini)
						st.coste = 100;
					else
						st.coste = 10;
				break;

				case 'B':
					if(!st.J_zapatillas)
						st.coste = 50;
					else
						st.coste = 15;
				break;

				case 'T':
					st.coste = 2;
				break;

				default:
					st.coste = 1;
				break;
			}		
		break;	
		
		case actSON_TURN_SL:
			casilla = mapa[st.sonambulo.f][st.sonambulo.c];
			switch (casilla){
				case 'A':
					if(!st.J_bikini)
						st.coste = 7;
					else
						st.coste = 2;
				break;

				case 'B':
					if(!st.J_zapatillas)
						st.coste = 3;
					else
						st.coste = 1;
				break;

				default:
					st.coste = 1;
				break;
			}		
		break;	
		
		case actSON_TURN_SR:
			casilla = mapa[st.sonambulo.f][st.sonambulo.c];
			switch (casilla){
				case 'A':
					if(!st.J_bikini)
						st.coste = 7;
					else
						st.coste = 2;
				break;

				case 'B':
					if(!st.J_zapatillas)
						st.coste = 3;
					else
						st.coste = 1;
				break;

				default:
					st.coste = 1;
				break;
			}		
		break;	
		
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// NIVEL 0 ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

list<Action> ComportamientoJugador::AnchuraSoloJugador(const estado &inicio, const ubicacion &final,
									const vector<vector<unsigned char>> &mapa)
{
	node current_node;
	list<node> frontier;
	set<node> explored;
	list<Action> plan;
	current_node.st = inicio;
	frontier.push_back(current_node);
	bool SolutionFound = (current_node.st.jugador.f == final.f and
						current_node.st.jugador.c == final.c);

	while(!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);

		// Generar hijo actFORWARD
		node child_forward = current_node;
		child_forward.st = apply(actFORWARD, current_node.st, mapa);
		if(child_forward.st.jugador.f == final.f and child_forward.st.jugador.c == final.c)
		{
			child_forward.secuencia.push_back(actFORWARD);
			current_node = child_forward;
			SolutionFound = true;
		}
		else if(explored.find(child_forward) == explored.end())
		{
			child_forward.secuencia.push_back(actFORWARD);
			frontier.push_back(child_forward);
		}

		if(!SolutionFound){
			// Generar hijo actTURN_L
			node child_turnl = current_node;
			child_turnl.st = apply(actTURN_L, current_node.st, mapa);
			if(explored.find(child_turnl) == explored.end())
			{
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push_back(child_turnl);
			}
			// Generar hijo actTURN_R
			node child_turnr = current_node;
			child_turnr.st = apply(actTURN_R, current_node.st, mapa);
			if(explored.find(child_turnr) == explored.end())
			{
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push_back(child_turnr);
			}
		}

		if(!SolutionFound and !frontier.empty()){
			current_node = frontier.front();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if(!frontier.empty())
					current_node = frontier.front();
			}
		}
	} 

	if(SolutionFound)
			plan = current_node.secuencia;

	return plan;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// NIVEL 1 ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

list<Action> ComportamientoJugador::AnchuraJugadorYSonambulo(const estado &inicio, const ubicacion &final,
										const vector<vector<unsigned char>> &mapa)
{
	node current_node;
    list<node> frontier;
    set<node> explored;
    list<Action> plan;
    current_node.st = inicio;
    frontier.push_back(current_node);
    bool SolutionFound = (current_node.st.sonambulo.f == final.f 
						and current_node.st.sonambulo.c == final.c);

 	while (!frontier.empty() and !SolutionFound)
	{
        frontier.pop_front();
        explored.insert(current_node);

        
		if(!SonambuloALaVista(current_node.st.jugador, current_node.st.sonambulo)){
			// Generar hijo actFORWARD
			node child_forward = current_node;
			child_forward.st = apply(actFORWARD, current_node.st, mapa);

			if(explored.find(child_forward) == explored.end())
			{
				// Añadir hijo a la lista de nodos por explorar
				child_forward.secuencia.push_back(actFORWARD);
				frontier.push_back(child_forward);
			}

			if(!SolutionFound)
			{
				// Generar hijo actTURN_L
				node child_turnl = current_node;
				child_turnl.st = apply(actTURN_L, current_node.st, mapa);
				if(explored.find(child_turnl) == explored.end())
				{
					child_turnl.secuencia.push_back(actTURN_L);
					frontier.push_back(child_turnl);
				}
				// Generar hijo actTURN_R
				node child_turnr = current_node;
				child_turnr.st = apply(actTURN_R, current_node.st, mapa);
				if(explored.find(child_turnr) == explored.end())
				{
					child_turnr.secuencia.push_back(actTURN_R);
					frontier.push_back(child_turnr);
				}
			}
		}
		else
		{
			// Generar hijo actSON_FORWARD
			node sonambulo_forward = current_node;
			sonambulo_forward.st = apply(actSON_FORWARD, current_node.st, mapa);
			if(sonambulo_forward.st.sonambulo.f == final.f 
				and sonambulo_forward.st.sonambulo.c == final.c)
			{
				sonambulo_forward.secuencia.push_back(actSON_FORWARD);
				current_node = sonambulo_forward;
				SolutionFound = true;
			}    
			else if(explored.find(sonambulo_forward) == explored.end())
			{
				// Añadir hijo a la lista de nodos por explorar
				sonambulo_forward.secuencia.push_back(actSON_FORWARD);
				frontier.push_back(sonambulo_forward);
			}

			if(!SolutionFound){

				// Generar hijo actSON_TURN_SL
				node sonambulo_turnl = current_node;
				sonambulo_turnl.st = apply(actSON_TURN_SL, current_node.st, mapa);

				if(explored.find(sonambulo_turnl) == explored.end())
				{
					// Añadir hijo a la lista de nodos por explorar
					sonambulo_turnl.secuencia.push_back(actSON_TURN_SL);
					frontier.push_back(sonambulo_turnl);
				}

				// Generar hijo actSON_TURN_SL
				node sonambulo_turnr = current_node;
				sonambulo_turnr.st = apply(actSON_TURN_SR, current_node.st, mapa);

				if(explored.find(sonambulo_turnr) == explored.end())
				{
					// Añadir hijo a la lista de nodos por explorar
					sonambulo_turnr.secuencia.push_back(actSON_TURN_SR);
					frontier.push_back(sonambulo_turnr);
				}
			}
		}

		if(!SolutionFound and !frontier.empty()){
			current_node = frontier.front();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if(!frontier.empty())
					current_node = frontier.front();
			}
		}
	}
			
	if(SolutionFound)
		plan = current_node.secuencia;

	return plan;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// NIVEL 2 ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

list<Action> ComportamientoJugador::CostoUniformeJugador(const estado &inicio, const ubicacion &final, 
										const vector<vector<unsigned char>> &mapa) {
	list<Action> plan;
	set<node> explored;
	priority_queue<node, vector<node>, ComparaCoste> frontier;
	node current_node;
	current_node.st = inicio;
	getCoste(actFORWARD, current_node.st, mapa);
	current_node.st.coste_total = current_node.st.coste;
	bool SolutionFound = (current_node.st.jugador.f == final.f 
						and current_node.st.jugador.c == final.c);

	current_node.secuencia.empty();

	frontier.push(current_node);

	while(!frontier.empty() and !SolutionFound)
	{	

		if((mapa[current_node.st.jugador.f][current_node.st.jugador.c] == 'K') and (current_node.st.J_bikini == false)){
			current_node.st.J_bikini = true;
			current_node.st.J_zapatillas = false;
		}
		if((mapa[current_node.st.jugador.f][current_node.st.jugador.c] == 'D') and (current_node.st.J_zapatillas == false)){
			current_node.st.J_bikini = false;
			current_node.st.J_zapatillas = true;
		}		
		
		frontier.pop();
		explored.insert(current_node);
		
		if(current_node.st.jugador.f == final.f and current_node.st.jugador.c == final.c){
			SolutionFound = true;
			break;
		}

		// Generar hijo actFORWARD
		node child_forward = current_node;
		getCoste(actFORWARD, child_forward.st, mapa);
		child_forward.st = apply(actFORWARD, child_forward.st, mapa);

		if(explored.find(child_forward) == explored.end())
		{
			child_forward.secuencia.push_back(actFORWARD);	
			child_forward.st.coste_total = current_node.st.coste_total + child_forward.st.coste;
			frontier.push(child_forward);
		}

		// Generar hijo actTURN_L
		node child_turnl = current_node;
		child_turnl.st = apply(actTURN_L, current_node.st, mapa);
		if(explored.find(child_turnl) == explored.end())
		{
			child_turnl.secuencia.push_back(actTURN_L);
			getCoste(actTURN_L, child_turnl.st, mapa);
			child_turnl.st.coste_total = current_node.st.coste_total + child_turnl.st.coste;
			frontier.push(child_turnl);
		}
		// Generar hijo actTURN_R
		node child_turnr = current_node;
		child_turnr.st = apply(actTURN_R, current_node.st, mapa);
		if(explored.find(child_turnr) == explored.end())
		{
			child_turnr.secuencia.push_back(actTURN_R);
			getCoste(actTURN_R, child_turnr.st, mapa);
			child_turnr.st.coste_total = current_node.st.coste_total + child_turnr.st.coste;
			frontier.push(child_turnr);
		}

		if(!frontier.empty() and !SolutionFound){
			current_node = frontier.top();	
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop();
				if(!frontier.empty())
					current_node = frontier.top();
			}
		}
		
	}

	if(SolutionFound){
		cout << "Cargando el plan\n";
		plan = current_node.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
	}

	return plan;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// NIVEL 3 ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
list<Action> ComportamientoJugador::AlgoritmoA(const estado &inicio, const ubicacion &final, 
										const vector<vector<unsigned char>> &mapa)
{
	list<Action> plan;
	set<node> explored;
	priority_queue<node, vector<node>, ComparaCoste> frontier;
	node current_node;
	current_node.st = inicio;
	getCoste(actFORWARD, current_node.st, mapa);
	current_node.st.coste_total = current_node.st.coste;
	bool SolutionFound = (current_node.st.sonambulo.f == final.f 
						and current_node.st.sonambulo.c == final.c);

	current_node.secuencia.empty();

	frontier.push(current_node);

	while(!frontier.empty() and !SolutionFound)
	{
		
		if((mapa[current_node.st.jugador.f][current_node.st.jugador.c] == 'K') and (current_node.st.J_bikini == false)){
			current_node.st.J_bikini = true;
			current_node.st.J_zapatillas = false;
		}
		if((mapa[current_node.st.jugador.f][current_node.st.jugador.c] == 'D') and (current_node.st.J_zapatillas == false)){
			current_node.st.J_bikini = false;
			current_node.st.J_zapatillas = true;
		}			

		if((mapa[current_node.st.sonambulo.f][current_node.st.sonambulo.c] == 'K') and (current_node.st.S_bikini == false)){
			current_node.st.S_bikini = true;
			current_node.st.S_zapatillas = false;
		}
		if((mapa[current_node.st.jugador.f][current_node.st.jugador.c] == 'D') and (current_node.st.S_zapatillas == false)){
			current_node.st.S_bikini = false;
			current_node.st.S_zapatillas = true;
		}	

		frontier.pop();
		explored.insert(current_node);

		if(current_node.st.sonambulo.f == final.f and current_node.st.sonambulo.c == final.c){
			SolutionFound = true;
			break;
		}


		if(!SonambuloALaVista(current_node.st.jugador, current_node.st.sonambulo))
		{
			// Generar hijo actFORWARD
			node child_forward = current_node;
			getCoste(actFORWARD, child_forward.st, mapa);
			child_forward.st = apply(actFORWARD, child_forward.st, mapa);
			if(explored.find(child_forward) == explored.end())
			{
				child_forward.secuencia.push_back(actFORWARD);
				child_forward.st.coste_total = current_node.st.coste_total + child_forward.st.coste;
				frontier.push(child_forward);
			}

			// Generar hijo actTURN_L
			node child_turnl = current_node;
			child_turnl.st = apply(actTURN_L, current_node.st, mapa);
			if(explored.find(child_turnl) == explored.end())
			{
				child_turnl.secuencia.push_back(actTURN_L);
				getCoste(actTURN_L, child_turnl.st, mapa);
				child_turnl.st.coste_total = current_node.st.coste_total + child_turnl.st.coste;
				frontier.push(child_turnl);
			}
			// Generar hijo actTURN_R
			node child_turnr = current_node;
			child_turnr.st = apply(actTURN_R, current_node.st, mapa);
			if(explored.find(child_turnr) == explored.end())
			{
				child_turnr.secuencia.push_back(actTURN_R);
				getCoste(actTURN_R, child_turnr.st, mapa);
				child_turnr.st.coste_total = current_node.st.coste_total + child_turnr.st.coste;
				frontier.push(child_turnr);
			}
		}
		else
		{
			// Generar hijo actSON_FORWARD
			node sonambulo_forward = current_node;
			getCoste(actSON_FORWARD, sonambulo_forward.st, mapa);
			sonambulo_forward.st = apply(actSON_FORWARD, current_node.st, mapa);
			if(explored.find(sonambulo_forward) == explored.end())
			{
				// Añadir hijo a la lista de nodos por explorar
				sonambulo_forward.secuencia.push_back(actSON_FORWARD);
				sonambulo_forward.st.coste_total = current_node.st.coste_total + sonambulo_forward.st.coste;
				frontier.push(sonambulo_forward);
			}

			// Generar hijo actSON_TURN_SL
			node sonambulo_turnl = current_node;
			sonambulo_turnl.st = apply(actSON_TURN_SL, current_node.st, mapa);
			if(explored.find(sonambulo_turnl) == explored.end())
			{
				// Añadir hijo a la lista de nodos por explorar
				getCoste(actSON_TURN_SL, sonambulo_turnl.st, mapa);
				sonambulo_turnl.secuencia.push_back(actSON_TURN_SL);
				sonambulo_turnl.st.coste_total = current_node.st.coste_total + sonambulo_turnl.st.coste;
				frontier.push(sonambulo_turnl);
			}

			// Generar hijo actSON_TURN_SL
			node sonambulo_turnr = current_node;
			sonambulo_turnr.st = apply(actSON_TURN_SR, current_node.st, mapa);

			if(explored.find(sonambulo_turnr) == explored.end())
			{
				// Añadir hijo a la lista de nodos por explorar
				getCoste(actSON_TURN_SR, sonambulo_turnr.st, mapa);
				sonambulo_turnr.secuencia.push_back(actSON_TURN_SR);
				sonambulo_turnr.st.coste_total = current_node.st.coste_total + sonambulo_turnr.st.coste;
				frontier.push(sonambulo_turnr);
			}
		}

		if(!frontier.empty() and !SolutionFound){
			current_node = frontier.top();	
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop();
				if(!frontier.empty())
					current_node = frontier.top();
			}
		}
		
	}

	if(SolutionFound){
		cout << "Cargando el plan\n";
		plan = current_node.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
	}

	return plan;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// VISUALIZA //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/** Permite pintar sobre el mapa del simulador el plan aprtiendo desde el estado st */
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;
	
	auto it = plan.begin();
	while(it != plan.end()){
		switch(*it){
			case actFORWARD:
				cst.jugador = NextCasilla(cst.jugador);
				mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
				break;
			case actTURN_L:
				cst.jugador.brujula = (Orientacion((cst.jugador.brujula+6)%8));
				break;
			case actTURN_R:
				cst.jugador.brujula = (Orientacion((cst.jugador.brujula+2)%8));
				break;
			case actSON_FORWARD:
				cst.sonambulo = NextCasilla(cst.sonambulo);
				mapaConPlan[cst.sonambulo.f][cst.sonambulo.c] = 2;
				break;
			case actSON_TURN_SL:
				cst.sonambulo.brujula = (Orientacion((cst.sonambulo.brujula+7)%8));
				break;
			case actSON_TURN_SR:
				cst.sonambulo.brujula = (Orientacion((cst.sonambulo.brujula+1)%8));
				break;
		}
		it++;
	}
}

// Sacar por la consola la secuencia del plan obtenido
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