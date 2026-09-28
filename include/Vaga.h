#ifndef VAGA_H
#define VAGA_H

#include <string>

class Vaga {
private:
    int numero;
    std::string tipo;
    bool ocupada;
    std::string placaVeiculo;

public:
    Vaga(int numero, const std::string& tipo);

    int getNumero() const;
    std::string getTipo() const;
    bool estaOcupada() const;
    std::string getPlacaVeiculo() const;

    void ocupar(const std::string& placa);
    void liberar();
};

#endif