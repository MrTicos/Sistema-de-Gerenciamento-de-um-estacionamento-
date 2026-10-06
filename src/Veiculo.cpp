#include "Veiculo.h"
using namespace std;

// Construtor que inicializa os atributos do veículo com os valores recebidos.
Veiculo::Veiculo(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : placa(placa), modelo(modelo), cor(cor), taxaHora(taxaHora) {}

// Métodos de leitura que retornam as informações do veículo.
    string Veiculo::getPlaca() const {
    return placa;
}

string Veiculo::getModelo() const {
    return modelo;
}

string Veiculo::getCor() const {
    return cor;
}