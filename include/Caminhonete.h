#ifndef CAMINHONETE_H
#define CAMINHONETE_H

#include "Veiculo.h"

using namespace std;

class Caminhonete : public Veiculo {
public:
    Caminhonete(const string& placa, const string& modelo, const string& cor, double taxaHora);

    string getTipo() const override;
    double calcularTarifa(double minutos) const override;
    bool podeUsarVagaMoto() const override;
};

#endif