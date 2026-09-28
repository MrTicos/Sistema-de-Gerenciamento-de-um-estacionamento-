#include "Vaga.h"
using namespace std;

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

void Vaga::ocupar(const string& placa) {
    ocupada = true;
    placaVeiculo = placa;
}

void Vaga::liberar() {
    ocupada = false;
    placaVeiculo = "";
}
