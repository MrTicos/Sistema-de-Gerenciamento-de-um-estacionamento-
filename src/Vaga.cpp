#include "Vaga.h"
using namespace std;

// Toda vaga nasce livre.
Vaga::Vaga(int numero, const string& tipo)
    : numero(numero), tipo(tipo), ocupada(false), placaVeiculo("") {}

int Vaga::getNumero() const {
    return numero;
}

string Vaga::getTipo() const {
    return tipo;
}

bool Vaga::estaOcupada() const {
    return ocupada;
}

string Vaga::getPlacaVeiculo() const {
    return placaVeiculo;
}

// Guarda a placa de quem estacionou e marca a vaga como ocupada.
void Vaga::ocupar(const string& placa) {
    ocupada = true;
    placaVeiculo = placa;
}

// Esquece a placa e marca a vaga como livre.
void Vaga::liberar() {
    ocupada = false;
    placaVeiculo = "";
}
