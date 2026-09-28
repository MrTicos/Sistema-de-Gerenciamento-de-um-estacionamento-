#include "Moto.h"
using namespace std;

Moto::Moto(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : Veiculo(placa, modelo, cor, taxaHora) {}

string Moto::getTipo() const {
    return "Moto";
}

double Moto::calcularTarifa(double minutos) const {
    return minutos * (taxaHora / 60.0);
}

bool Moto::podeUsarVagaMoto() const {
    return true;
}