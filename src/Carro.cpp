#include "Carro.h"
using namespace std;

// Repassa os dados para o construtor da classe base (Veiculo).
Carro::Carro(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : Veiculo(placa, modelo, cor, taxaHora) {}

string Carro::getTipo() const {
    return "Carro";
}

// Regra do carro: proporcional ao tempo, sem tolerância e sem adicional.
// Exemplo: 120 minutos a R$ 10,00/hora = R$ 20,00.
double Carro::calcularTarifa(double minutos) const {
    return minutos * (taxaHora / 60.0);
}

// Carro estaciona em vaga de carro.
bool Carro::podeUsarVagaMoto() const {
    return false;
}
