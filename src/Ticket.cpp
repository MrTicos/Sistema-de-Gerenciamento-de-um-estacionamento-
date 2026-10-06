#include "Ticket.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

// Construtor: registra os dados do veículo/vaga/entrada e define os campos de saída com valores padrão zerados/vazios.
Ticket::Ticket(const string& placa, const string& tipo, const string& modelo,
               const string& cor, int numeroVaga, const string& entrada)
    : placa(placa), tipo(tipo), modelo(modelo), cor(cor), numeroVaga(numeroVaga),
      entrada(entrada), saida(""), minutos(0), valor(0) {
}

// Atualiza o ticket com as informações de fechamento do estacionamento.
void Ticket::definirSaida(const string& saida, double minutos, double valor) {
    this->saida = saida;
    this->minutos = minutos;
    this->valor = valor;
}

// Monta e retorna uma string formatada com os dados do comprovante de entrada.
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

// Monta e retorna uma string formatada com os dados do comprovante de saída e cobrança (2 casas decimais).
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

// Imprime o comprovante de entrada na tela do terminal.
void Ticket::imprimirEntrada() const {
    cout << "\n" << textoEntrada();
}

// Imprime o comprovante de saída na tela do terminal.
void Ticket::imprimirSaida() const {
    cout << "\n" << textoSaida();
}
