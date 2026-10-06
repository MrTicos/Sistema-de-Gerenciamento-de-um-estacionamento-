#ifndef CAMINHONETE_H
#define CAMINHONETE_H

#include "Veiculo.h"

using namespace std;

// Classe que representa uma caminhonete, herdada da classe base Veiculo.
class Caminhonete : public Veiculo {
public:
    // Construtor: recebe os dados da caminhonete e os repassa para a classe pai inicializar.
    Caminhonete(const string& placa, const string& modelo, const string& cor, double taxaHora);

    // Implementações dos métodos obrigatórios da classe base Veiculo:
    
    // Sobrescreve o método do pai para retornar o texto específico "Caminhonete".
    string getTipo() const override;
    
    // Implementa a regra específica de cálculo do valor do estacionamento para caminhonetes.
    double calcularTarifa(double minutos) const override;
    
    // Retorna falso, pois uma caminhonete não pode usar a vaga de moto.
    bool podeUsarVagaMoto() const override;
};

#endif