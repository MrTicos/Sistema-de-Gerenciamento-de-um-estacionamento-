#include "Ticket.h"
#include <iostream>
#include <iomanip>
using namespace std;

Ticket::Ticket(const string& placa, const string& tipo, const string& modelo,
               const string& cor, int numeroVaga, const string& entrada)
    : placa(placa), tipo(tipo), modelo(modelo), cor(cor), numeroVaga(numeroVaga),
      entrada(entrada), saida(""), minutos(0), valor(0) {
}

void Ticket::definirSaida(const string& saida, double minutos, double valor) {
    this->saida = saida;
    this->minutos = minutos;
    this->valor = valor;
}

void Ticket::imprimirEntrada() const {
    cout << "\n==============================\n";
    cout << "        TICKET DE ENTRADA     \n";
    cout << "==============================\n";
    cout << "Placa: " << placa << "\n";
    cout << "Tipo: " << tipo << "\n";
    cout << "Modelo: " << modelo << "\n";
    cout << "Cor: " << cor << "\n";
    cout << "Vaga: " << numeroVaga << "\n";
    cout << "Entrada: " << entrada << "\n";
    cout << "==============================\n";
}

void Ticket::imprimirSaida() const {
    cout << "\n==============================\n";
    cout << "         TICKET DE SAIDA      \n";
    cout << "==============================\n";
    cout << "Placa: " << placa << "\n";
    cout << "Tipo: " << tipo << "\n";
    cout << "Modelo: " << modelo << "\n";
    cout << "Cor: " << cor << "\n";
    cout << "Vaga: " << numeroVaga << "\n";
    cout << "Entrada: " << entrada << "\n";
    cout << "Saida: " << saida << "\n";
    cout << "Tempo: " << minutos << " minutos\n";
    cout << fixed << setprecision(2);
    cout << "Valor pago: R$ " << valor << "\n";
    cout << "==============================\n";
}
