#ifndef BANCO_H
#define BANCO_H

#include <string>
#include <vector>

using namespace std;

// Estruturas de dados (DTOs) usadas para agrupar e transportar informações do banco.
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

// Classe responsável por gerenciar a conexão e as operações com o banco de dados SQLite.
class Banco {
private:
    // Ponteiro genérico para a conexão com o SQLite (oculta a dependência da biblioteca no .h).
    void* banco;
    
    // Método interno para executar comandos SQL simples (sem retorno de dados), como CREATE TABLE.
    bool executar(const string& sql);

public:
    // Construtor: recebe o nome do arquivo do banco e tenta abrir a conexão.
    Banco(const string& nomeArquivo);
    
    // Destrutor: garante que a conexão com o banco seja fechada ao encerrar o programa.
    ~Banco();

    // Cria as tabelas necessárias (veiculos e estacionamentos) caso não existam.
    bool criarTabelas();

    // Operações de CRUD (Criar, Ler, Atualizar, Apagar) para os Veículos:
    bool cadastrarVeiculo(const DadosVeiculo& veiculo);
    bool buscarVeiculo(const string& placa, DadosVeiculo& veiculo);
    vector<VeiculoEstacionado> listarVeiculosEstacionados();
    bool atualizarVeiculo(const std::string& placa, const std::string& modelo, const std::string& cor);
    bool removerVeiculo(const std::string& placa);

    // Operações de fluxo do Estacionamento:
    bool registrarEntrada(const string& placa, const string& horarioEntrada);
    bool registrarSaida(const string& placa, const string& horarioSaida, double valorPago);
    
    // Retorna verdadeiro se o veículo não tiver um horário de saída registrado.
    bool veiculoEstaEstacionado(const string& placa);
    
    // Retorna o horário em que o veículo entrou no estacionamento.
    string buscarEntrada(const string& placa);
    
    // Lista todas as movimentações de veículos que já saíram do estacionamento.
    vector<RegistroSaida> listarHistorico();
    
    // Soma o valor de todos os estacionamentos encerrados na data informada.
    double calcularFaturamentoDoDia(const string& data);
};

#endif