#include "Caminhonete.h"
using namespace std;

// Repassa os dados para o construtor da classe base (Veiculo).
Caminhonete::Caminhonete(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : Veiculo(placa, modelo, cor, taxaHora) {}

string Caminhonete::getTipo() const {
    return "Caminhonete";
}

// Regra da caminhonete: tarifa normal + 20% (ocupa mais espaço).
// Exemplo: 60 minutos a R$ 10,00/hora = R$ 12,00.
double Caminhonete::calcularTarifa(double minutos) const {
    return minutos * (taxaHora / 60.0) * (1.0 + ADICIONAL);
}

// Caminhonete estaciona em vaga de carro.
bool Caminhonete::podeUsarVagaMoto() const {
    return false;
}
