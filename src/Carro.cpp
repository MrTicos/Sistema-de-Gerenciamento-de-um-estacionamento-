#include "Carro.h"
using namespace std;

Carro::Carro(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : Veiculo(placa, modelo, cor, taxaHora) {}

string Carro::getTipo() const {
    return "Carro";
}

double Carro::calcularTarifa(double minutos) const {
    return minutos * (taxaHora / 60.0);
}

bool Carro::podeUsarVagaMoto() const {
    return false;
}
