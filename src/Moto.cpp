#include "Moto.h"
using namespace std;

// Repassa os dados para o construtor da classe base (Veiculo).
Moto::Moto(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : Veiculo(placa, modelo, cor, taxaHora) {}

string Moto::getTipo() const {
    return "Moto";
}

// Regra da moto: até 15 minutos é grátis (tolerância).
// Passou disso, paga o tempo TOTAL proporcional.
// Exemplos a R$ 5,00/hora: 10 min = R$ 0,00; 30 min = R$ 2,50.
double Moto::calcularTarifa(double minutos) const {
    if (minutos <= TOLERANCIA_MINUTOS) {
        return 0.0;
    }
    return minutos * (taxaHora / 60.0);
}

// Moto estaciona em vaga de moto.
bool Moto::podeUsarVagaMoto() const {
    return true;
}
