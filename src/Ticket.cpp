#include "Ticket.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

// Cria o ticket no momento da entrada; saída, tempo e valor começam zerados.
Ticket::Ticket(const string& placa, const string& tipo, const string& modelo,
               const string& cor, int numeroVaga, const string& entrada)
    : placa(placa), tipo(tipo), modelo(modelo), cor(cor), numeroVaga(numeroVaga),
      entrada(entrada), saida(""), minutos(0), valor(0) {
}

// Completa o ticket com os dados da saída.
void Ticket::definirSaida(const string& saida, double minutos, double valor) {
    this->saida = saida;
    this->minutos = minutos;
    this->valor = valor;
}

// Monta o comprovante de entrada como texto (sem imprimir nada).
string Ticket::textoEntrada() const {
    ostringstream texto;
    texto << "==============================\n";
    texto << "        TICKET DE ENTRADA     \n";
    texto << "==============================\n";
    texto << "Placa: " << placa << "\n";
    texto << "Tipo: " << tipo << "\n";
    texto << "Modelo: " << modelo << "\n";
    texto << "Cor: " << cor << "\n";
    texto << "Vaga: " << numeroVaga << "\n";
    texto << "Entrada: " << entrada << "\n";
    texto << "==============================\n";
    return texto.str();
}

// Monta o comprovante de saída como texto (sem imprimir nada).
string Ticket::textoSaida() const {
    ostringstream texto;
    texto << "==============================\n";
    texto << "         TICKET DE SAIDA      \n";
    texto << "==============================\n";
    texto << "Placa: " << placa << "\n";
    texto << "Tipo: " << tipo << "\n";
    texto << "Modelo: " << modelo << "\n";
    texto << "Cor: " << cor << "\n";
    texto << "Vaga: " << numeroVaga << "\n";
    texto << "Entrada: " << entrada << "\n";
    texto << "Saida: " << saida << "\n";
    texto << "Tempo: " << minutos << " minutos\n";
    texto << fixed << setprecision(2);
    texto << "Valor pago: R$ " << valor << "\n";
    texto << "==============================\n";
    return texto.str();
}

// Versão para o terminal: só imprime o texto já montado.
void Ticket::imprimirEntrada() const {
    cout << "\n" << textoEntrada();
}

void Ticket::imprimirSaida() const {
    cout << "\n" << textoSaida();
}
