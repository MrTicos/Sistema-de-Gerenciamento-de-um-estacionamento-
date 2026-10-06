#include "Caminhonete.h"
using namespace std;

// Construtor: recebe os dados e os repassa imediatamente para o construtor da classe pai (Veiculo).
Caminhonete::Caminhonete(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : Veiculo(placa, modelo, cor, taxaHora) {}

// Retorna o texto de identificação do veículo.
string Caminhonete::getTipo() const {
    return "Caminhonete";
}

// Implementa o cálculo da tarifa de estacionamento, baseando no tempo de permanência em minutos e na taxa por hora do veículo.
double Caminhonete::calcularTarifa(double minutos) const {
    return minutos * (taxaHora / 60.0);
}

// Retorna falso, pois uma caminhonete não pode usar a vaga de moto.
bool Caminhonete::podeUsarVagaMoto() const {
    return false;
}