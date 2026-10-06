#include "Moto.h"
using namespace std;

// Construtor: recebe os dados e os repassa imediatamente para o construtor da classe pai (Veiculo).
Moto::Moto(const string& placa, const string& modelo, const string& cor, double taxaHora)
    : Veiculo(placa, modelo, cor, taxaHora) {}

// Retorna o texto de identificação do veículo.
string Moto::getTipo() const {
    return "Moto";
}

// Implementa o cálculo da tarifa de estacionamento, baseando no tempo de permanência em minutos e na taxa por hora do veículo.
double Moto::calcularTarifa(double minutos) const {
    return minutos * (taxaHora / 60.0);
}

// Retorna verdadeiro, pois uma moto PODE usar a vaga de moto.
bool Moto::podeUsarVagaMoto() const {
    return true;
}