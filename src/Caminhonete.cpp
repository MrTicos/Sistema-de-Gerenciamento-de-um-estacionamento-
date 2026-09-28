#include "Caminhonete.h"
using namespace std;

Caminhonete::Caminhonete(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : Veiculo(placa, modelo, cor, taxaHora) {}

string Caminhonete::getTipo() const {
    return "Caminhonete";
}

double Caminhonete::calcularTarifa(double minutos) const {
    return minutos * (taxaHora / 60.0);
}

bool Caminhonete::podeUsarVagaMoto() const {
    return false;
}