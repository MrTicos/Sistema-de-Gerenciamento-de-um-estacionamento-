#ifndef VEICULO_H
#define VEICULO_H

#include <string>
using namespace std;

// Classe base para representar os veículos do sistema de estacionamento.
class Veiculo {

// Atributos protegidos para armazenar informações básicas do veículo.    
protected:
    string placa;
    string modelo;
    string cor;
    double taxaHora;

public:
    // Construtor que recebe e inicializa os atributos do veículo.
    Veiculo(const string& placa, const string& modelo, const string& cor, double taxaHora);
    // Destrutor virtual para permitir a destruição correta de objetos derivados.
    virtual ~Veiculo() = default;

    // Métodos de leitura. Retornam as informações do veículo.
    string getPlaca() const;
    string getModelo() const;
    string getCor() const;

    // Retorna um texto dizendo qual é o tipo do veículo.
    virtual string getTipo() const = 0;
    // Calcula a tarifa a ser paga com base no tempo de permanência.
    virtual double calcularTarifa(double minutos) const = 0;
    // Método para verificar se o veículo pode usar a vaga de moto.
    virtual bool podeUsarVagaMoto() const = 0;
};

#endif
