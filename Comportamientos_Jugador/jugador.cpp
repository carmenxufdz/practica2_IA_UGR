#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

/** Devuelve si una ubicacion en el mapa es transitable para el agente */
bool CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa)
{
	return (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M');
}

/** Devuelve la ubicacion siguiente segun el avance del agente */
ubicacion NextCasilla(const ubicacion &pos)
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

/** Devuelce el estado que se genera si el agente puede avanzar.
 *  Si no puede avanzar, se devuelve como salida el mismo estado de entrada.
 */
stateN0 apply(const Action &a, const stateN0 &st, const vector<vector<unsigned char>> &mapa)
{
	stateN0 st_result = st;
	ubicacion sig_ubicacion;
	switch(a){
		case actFORWARD:
			sig_ubicacion = NextCasilla(st.jugador);
			if(CasillaTransitable(sig_ubicacion, mapa) and !(sig_ubicacion == st.sonambulo))
				st_result.jugador = sig_ubicacion;
			break;
		case actTURN_L:
			st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 6)%8);
			break;
		case actTURN_R:
			st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula + 2)%8);
			break;
	}
	return st_result;
}

/** Encuentra si el elemento item esta en la lista */
bool Find(const stateN0 &item, const list<nodeN0> &lista)
{
	auto it = lista.begin();
	while(it != lista.end() and !(it->st == item))
		it++;
	return (!(it == lista.end()));
}

/** Primera aproximacion a la implementacion de la busqueda en anchura */
list<Action> AnchuraSoloJugador_V2(const stateN0 &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa)
{
	nodeN0 current_node;
	list<nodeN0> frontier;
	list<nodeN0> explored;
	list<Action> plan;
	current_node.st = inicio;
	frontier.push_back(current_node);
	bool SolutionFound = (current_node.st.jugador.f == final.f and
						current_node.st.jugador.c == final.c);

	while(!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.push_back(current_node);

		// Generar hijo actFORWARD
		nodeN0 child_forward = current_node;
		child_forward.st = apply(actFORWARD, current_node.st, mapa);
		if(child_forward.st.jugador.f == final.f and child_forward.st.jugador.c == final.c)
		{
			child_forward.secuencia.push_back(actFORWARD);
			current_node = child_forward;
			SolutionFound = true;
		}
		else if(!Find(child_forward.st,frontier) and !Find(child_forward.st, explored))
		{
			child_forward.secuencia.push_back(actFORWARD);
			frontier.push_back(child_forward);
		}

		if(!SolutionFound){
			// Generar hijo actTURN_L
			nodeN0 child_turnl = current_node;
			child_turnl.st = apply(actTURN_L, current_node.st, mapa);
			if(!Find(child_turnl.st,frontier) and !Find(child_turnl.st, explored))
			{
				child_turnl.secuencia.push_back(actTURN_L);
				frontier.push_back(child_turnl);
			}
			// Generar hijo actTURN_R
			nodeN0 child_turnr = current_node;
			child_turnr.st = apply(actTURN_R, current_node.st, mapa);
			if(!Find(child_turnr.st,frontier) and !Find(child_turnr.st, explored))
			{
				child_turnr.secuencia.push_back(actTURN_R);
				frontier.push_back(child_turnr);
			}
		}

		if(!SolutionFound and !frontier.empty())
			current_node = frontier.front();
	} 

	if(SolutionFound)
			plan = current_node.secuencia;

	return plan;
}

/** Pone a cero todos los elementos de una matriz */
void AnularMatriz(vector<vector<unsigned char>> &matriz)
{
	for(int i=0; i < matriz.size(); i++)
		for(int j=0; j < matriz[0].size(); j++)
			matriz[i][j] = 0;
}

/** Permite pintar sobre el mapa del simulador el plan aprtiendo desde el estado st */
void ComportamientoJugador::VisualizaPlan(const stateN0 &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	stateN0 cst = st;
	
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

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;

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
		goal.f = sensores.destinoF;
		goal.c = sensores.destinoC;

		plan = AnchuraSoloJugador_V2(c_state, goal, mapaResultado);
		VisualizaPlan(c_state,plan);
		hayPlan = true;
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

	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}