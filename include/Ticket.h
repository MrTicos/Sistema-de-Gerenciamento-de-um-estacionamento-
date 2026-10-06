#ifndef TICKET_H
#define TICKET_H

#include <string>
using namespace std;

class Ticket {
private:
    string placa;
    string tipo;
    string modelo;
    string cor;
    int numeroVaga;
    string entrada;
    string saida;
    double minutos;
    double valor;

public:
    Ticket(const string& placa, const string& tipo, const string& modelo,
           const string& cor, int numeroVaga, const string& entrada);

    void definirSaida(const string& saida, double minutos, double valor);

    string textoEntrada() const;
    string textoSaida() const;

    void imprimirEntrada() const;
    void imprimirSaida() const;
};

#endif
