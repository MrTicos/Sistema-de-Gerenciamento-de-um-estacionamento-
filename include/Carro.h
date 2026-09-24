#ifndef CARRO_H
#define CARRO_H

#include "Veiculo.h"
using namespace std;

class Carro : public Veiculo {
public:
    Carro(const string& placa, const string& modelo, const string& cor, double taxaHora);

    string getTipo() const override;
    double calcularTarifa(double minutos) const override;
    bool podeUsarVagaMoto() const override;
};

#endif