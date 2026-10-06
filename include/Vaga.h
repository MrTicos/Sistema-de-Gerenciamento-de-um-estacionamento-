#ifndef VAGA_H
#define VAGA_H

#include <string>

// Classe que representa uma vaga física dentro do estacionamento.
class Vaga {
private:
    // Atributos privados para proteger o estado da vaga.
    int numero;
    std::string tipo;
    bool ocupada;
    std::string placaVeiculo;

public:
    // Construtor: cria a vaga definindo seu número e tipo.
    Vaga(int numero, const std::string& tipo);

    // Métodos de leitura para consultar os dados da vaga.
    int getNumero() const;
    std::string getTipo() const;
    
    // Retorna true se a vaga tiver um veículo, ou false se estiver livre.
    bool estaOcupada() const;
    
    // Retorna a placa do veículo que está na vaga (ou vazio se estiver livre).
    std::string getPlacaVeiculo() const;

    // Métodos de ação que alteram o estado da vaga:
    
    // Marca a vaga como ocupada e registra a placa do veículo nela.
    void ocupar(const std::string& placa);
    
    // Marca a vaga como livre e apaga o registro da placa.
    void liberar();
};

#endif