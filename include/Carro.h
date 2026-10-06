#ifndef CARRO_H
#define CARRO_H

#include "Veiculo.h"

using namespace std;

// Classe que representa um carro, herdada da classe base Veiculo.
class Carro : public Veiculo {
public:
    // Construtor: recebe os dados do carro e os repassa para a classe pai inicializar.
    Carro(const string& placa, const string& modelo, const string& cor, double taxaHora);

    // Implementações dos métodos obrigatórios da classe base Veiculo:
    
    // Sobrescreve o método do pai para retornar o texto específico "Carro".
    string getTipo() const override;
    
    // Implementa a regra específica de cálculo do valor do estacionamento para carros.
    double calcularTarifa(double minutos) const override;
    
    // Retorna falso, pois um carro não pode usar a vaga de moto.
    bool podeUsarVagaMoto() const override;
};

#endif