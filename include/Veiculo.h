#ifndef VEICULO_H
#define VEICULO_H

#include <string>
using namespace std;

class Veiculo {
    
protected:
    string placa;
    string modelo;
    string cor;
    double taxaHora;

public:
    Veiculo(const string& placa, const string& modelo, const string& cor, double taxaHora);
    virtual ~Veiculo() = default;

    string getPlaca() const;
    string getModelo() const;
    string getCor() const;

    virtual string getTipo() const = 0;
    virtual double calcularTarifa(double minutos) const = 0;
    virtual bool podeUsarVagaMoto() const = 0;
};

#endif
