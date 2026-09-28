#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

#include "Banco.h"
#include "Vaga.h"
#include "Veiculo.h"
#include <memory>
#include <string>
#include <vector>
 
using namespace std;


class Estacionamento {
private:
    Banco& banco;
    vector<Vaga> vagasCarro;
    vector<Vaga> vagasMoto;
    double taxaCarro;
    double taxaMoto;

    Vaga* encontrarVagaLivre(const string& tipo);
    unique_ptr<Veiculo> criarVeiculo(const DadosVeiculo& dados);

public:
    Estacionamento(Banco& banco, int quantidadeCarro, int quantidadeMoto);

    bool registrarEntrada(const string& placa, const string& modelo,
                          const string& cor, const string& tipo);
    bool registrarSaida(const string& placa);

    void consultarVeiculo(const string& placa);
    void listarVeiculos();
    void mostrarVagas() const;
    void mostrarHistorico();
    void mostrarFaturamentoDoDia(const string& data);

    void configurarVagas(int quantidadeCarro, int quantidadeMoto);
    void configurarTaxas(double novaTaxaCarro, double novaTaxaMoto);

    double getTaxaCarro() const;
    double getTaxaMoto() const;
    int getTotalVagasCarro() const;
    int getTotalVagasMoto() const;
};

#endif