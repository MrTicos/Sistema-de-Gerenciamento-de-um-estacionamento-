#ifndef BANCO_H
#define BANCO_H

#include <string>
#include <vector>

using namespace std;

struct RegistroSaida {
    int id;
    string placa;
    string horarioEntrada;
    string horarioSaida;
    double valorPago;
};

struct DadosVeiculo {
    string placa;
    string modelo;
    string cor;
    string tipo;
};

struct VeiculoEstacionado {
    DadosVeiculo veiculo;
    int vaga;
    string horarioEntrada;
};

class Banco {
private:
    void* banco;
    bool executar(const string& sql);

public:
    Banco(const string& nomeArquivo);
    ~Banco();

    bool criarTabelas();

    bool cadastrarVeiculo(const DadosVeiculo& veiculo);
    bool buscarVeiculo(const string& placa, DadosVeiculo& veiculo);
    vector<VeiculoEstacionado> listarVeiculosEstacionados();

    bool atualizarVeiculo(const std::string& placa, const std::string& modelo, const std::string& cor);
    bool removerVeiculo(const std::string& placa);

    bool registrarEntrada(const string& placa, const string& horarioEntrada);
    bool registrarSaida(const string& placa, const string& horarioSaida, double valorPago);
    bool veiculoEstaEstacionado(const string& placa);
    string buscarEntrada(const string& placa);
    vector<RegistroSaida> listarHistorico();
    double calcularFaturamentoDoDia(const string& data);
};

#endif