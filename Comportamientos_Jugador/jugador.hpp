#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado{
  ubicacion jugador;
  ubicacion sonambulo;
  bool J_bikini, J_zapatillas;
  bool S_bikini, S_zapatillas;
  int coste;
  int coste_total = 0;

  bool operator== (const estado &x) const{
    return (jugador.f == x.jugador.f and jugador.c == x.jugador.c 
    and sonambulo.f == x.sonambulo.f and sonambulo.c == x.sonambulo.c);

  }
};

struct node{
  estado st;
  list<Action> secuencia;

  bool operator== (const node &n) const{
    return (st == n.st);
  }

  bool operator< (const node &n) const {
    if (st.sonambulo.f < n.st.sonambulo.f)
      return true;

    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c < n.st.sonambulo.c)
      return true;

    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula < n.st.sonambulo.brujula)
      return true;
    
    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.S_bikini < n.st.S_bikini)
      return true;

    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.S_bikini == n.st.S_bikini
            and st.S_zapatillas < n.st.S_zapatillas)
      return true;

    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.S_bikini == n.st.S_bikini
            and st.S_zapatillas == n.st.S_zapatillas and st.jugador.f < n.st.jugador.f)
      return true;

    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.S_bikini == n.st.S_bikini
            and st.S_zapatillas == n.st.S_zapatillas and st.jugador.f == n.st.jugador.f 
            and st.jugador.c < n.st.jugador.c)
      return true;

    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.S_bikini == n.st.S_bikini
            and st.S_zapatillas == n.st.S_zapatillas and st.jugador.f == n.st.jugador.f 
            and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula)
      return true;

    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.S_bikini == n.st.S_bikini
            and st.S_zapatillas == n.st.S_zapatillas and st.jugador.f == n.st.jugador.f 
            and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula
            and st.J_bikini < n.st.J_bikini)
      return true;

    else if (st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.S_bikini == n.st.S_bikini
            and st.S_zapatillas == n.st.S_zapatillas and st.jugador.f == n.st.jugador.f 
            and st.jugador.c == n.st.jugador.c and st.jugador.brujula == n.st.jugador.brujula
            and st.J_bikini == n.st.J_bikini and st.J_zapatillas < n.st.J_zapatillas)
      return true;

    else 
      return false;
  }
};


struct ComparaCoste{ // Función de comparación para la cola con prioridad (costo Uniforme)
    bool operator() (const node &a, const node &b) const{
        if (a.st.coste_total > b.st.coste_total )
            return true;
        else
            return false;
    }
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      hayPlan = false;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hayPlan = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    void VisualizaPlan(const estado &st, const list<Action> &plan);
    void PintaPlan(list<Action> plan);


  private:
    // Declarar Variables de Estado
    list<Action> plan;    // Almacena el plan en ejecucion
    bool hayPlan;         // TRUE: esta siguiendo un plan
    estado c_state;      // Estado nvl 0
    ubicacion goal;       // Posicion a la que se debe llegar
    vector<vector<unsigned char>> matrizConPlan;

    estado apply(const Action &a, const estado &st, const vector<vector<unsigned char>> &mapa);

    bool CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa);
    bool SonambuloALaVista(ubicacion &jugador, ubicacion &sonambulo);

    void getCoste(const Action &a, estado &st, const vector<vector<unsigned char>> &mapa);

    void AnularMatriz(vector<vector<unsigned char>> &matriz);

    ubicacion NextCasilla(const ubicacion &pos);
    
    list<Action> AnchuraSoloJugador(const estado &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa);
    list<Action> AnchuraJugadorYSonambulo(const estado &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa);
    list<Action> CostoUniformeJugador(const estado &inicio, const ubicacion &final, 
										const vector<vector<unsigned char>> &mapa);
    list<Action> AlgoritmoA(const estado &inicio, const ubicacion &final, 
										const vector<vector<unsigned char>> &mapa);



};

#endif
