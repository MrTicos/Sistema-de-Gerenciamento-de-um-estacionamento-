#include "Veiculo.h"
using namespace std;

Veiculo::Veiculo(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : placa(placa), modelo(modelo), cor(cor), taxaHora(taxaHora) {}

string Veiculo::getPlaca() const {
    return placa;
}

string Veiculo::getModelo() const {
    return modelo;
}

string Veiculo::getCor() const {
    return cor;
}