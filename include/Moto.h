#ifndef MOTO_H
#define MOTO_H

#include "Veiculo.h"

using namespace std;

// Classe que representa uma moto, herdada da classe base Veiculo.
class Moto : public Veiculo {
public:
    // Construtor: recebe os dados da moto e os repassa para a classe pai inicializar.
    Moto(const string& placa, const string& modelo, const string& cor, double taxaHora);

    // Implementações dos métodos obrigatórios da classe base Veiculo:
    
    // Sobrescreve o método do pai para retornar o texto específico "Moto".
    string getTipo() const override;
    
    // Implementa a regra específica de cálculo do valor do estacionamento para motos.
    double calcularTarifa(double minutos) const override;
    
    // Retorna verdadeiro, pois uma moto obviamente PODE usar a vaga de moto.
    bool podeUsarVagaMoto() const override;
};

#endif