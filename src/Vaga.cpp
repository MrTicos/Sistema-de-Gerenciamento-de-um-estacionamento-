#include "Vaga.h"
using namespace std;

// Construtor: inicializa o número e tipo recebidos. Por padrão, toda vaga nasce livre (ocupada = false) e sem placa.
Vaga::Vaga(int numero, const string& tipo)
    : numero(numero), tipo(tipo), ocupada(false), placaVeiculo("") {}

// Retorna o número de identificação da vaga.
int Vaga::getNumero() const {
    return numero;
}

// Retorna o tipo de veículo que essa vaga aceita (ex: Moto, Carro).
string Vaga::getTipo() const {
    return tipo;
}

// Informa o status atual da vaga (true = ocupada, false = livre).
bool Vaga::estaOcupada() const {
    return ocupada;
}

// Retorna a placa do veículo estacionado no momento.
string Vaga::getPlacaVeiculo() const {
    return placaVeiculo;
}

// Altera o estado da vaga para ocupada e guarda a placa do cliente.
void Vaga::ocupar(const string& placa) {
    ocupada = true;
    placaVeiculo = placa;
}

// "Limpa" a vaga: altera o estado para livre e remove a placa do veículo que acabou de sair.
void Vaga::liberar() {
    ocupada = false;
    placaVeiculo = "";
}