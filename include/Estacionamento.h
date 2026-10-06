#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

#include "Banco.h"
#include "Ticket.h"
#include "Vaga.h"
#include "Veiculo.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;


struct Resultado {
    bool ok;
    string mensagem;
};


struct InfoVeiculo {
    DadosVeiculo dados;
    bool estacionado;
    string horarioEntrada;
};


struct ResumoVagas {
    int livresCarro;
    int totalCarro;
    int livresMoto;
    int totalMoto;
};

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

    
    Resultado registrarEntrada(const string& placa, const string& modelo,
                               const string& cor, const string& tipo,
                               Ticket* ticket = nullptr);
    Resultado registrarSaida(const string& placa, Ticket* ticket = nullptr);

    Resultado editarVeiculo(const string& placa, const string& novoModelo, const string& novaCor);
    Resultado removerVeiculo(const string& placa);

    void restaurarVagas();

    bool consultarVeiculo(const string& placa, InfoVeiculo& info);
    vector<VeiculoEstacionado> listarVeiculos();
    ResumoVagas resumoVagas() const;
    vector<RegistroSaida> historico();
    double faturamentoDoDia(const string& data);

    Resultado configurarVagas(int quantidadeCarro, int quantidadeMoto);
    Resultado configurarTaxas(double novaTaxaCarro, double novaTaxaMoto);

    double getTaxaCarro() const;
    double getTaxaMoto() const;
    int getTotalVagasCarro() const;
    int getTotalVagasMoto() const;
    const vector<Vaga>& getVagasCarro() const;
    const vector<Vaga>& getVagasMoto() const;
};

#endif
