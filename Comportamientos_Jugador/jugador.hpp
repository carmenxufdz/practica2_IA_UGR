#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado{
  ubicacion jugador;
  ubicacion sonambulo;
  bool J_bikini, J_zapatillas;
  bool S_bikini, S_zapatillas;

  bool operator== (const estado &x) const{
    return (jugador == x.jugador 
    and sonambulo == x.sonambulo);

  }
};

struct node{
  estado st;
  list<Action> secuencia;
  int coste;

  bool operator== (const node &n) const{
    return (st == n.st);
  }

  bool operator > (const node &n) const{
    if(coste > n.coste)
      return true;
    else
      return false;
  }

  bool operator< (const node &n) const {
    if(coste < n.coste)
      return true;
    else if (coste == n.coste and st.sonambulo.f < n.st.sonambulo.f)
      return true;
    else if (coste == n.coste and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c < n.st.sonambulo.c)
      return true;
    else if (coste == n.coste and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula < n.st.sonambulo.brujula)
      return true;
    else if (coste == n.coste and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.jugador.f < n.st.jugador.f)
      return true;
    else if (coste == n.coste and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.jugador.f == n.st.jugador.f 
            and st.jugador.c < n.st.jugador.c)
      return true;
    else if (coste == n.coste and st.sonambulo.f == n.st.sonambulo.f and st.sonambulo.c == n.st.sonambulo.c 
            and st.sonambulo.brujula == n.st.sonambulo.brujula and st.jugador.f == n.st.jugador.f 
            and st.jugador.c == n.st.jugador.c and st.jugador.brujula < n.st.jugador.brujula)
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

    int getCosteGiroJugador(estado &st);
    int getCosteJugador(estado &st);
    int getCosteGiroSonambulo(estado &st);
    int getCosteSonambulo(estado &st);

    void AnularMatriz(vector<vector<unsigned char>> &matriz);

    ubicacion NextCasilla(const ubicacion &pos);
    
    list<Action> AnchuraSoloJugador(const estado &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa);
    list<Action> AnchuraJugadorYSonambulo(const estado &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa);
    list<Action> CostoUniformeJugador(const estado &inicio, const ubicacion &final, 
										const vector<vector<unsigned char>> &mapa);



};

#endif
