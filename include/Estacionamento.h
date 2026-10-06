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

// Estrutura para retornar o resultado de operações do sistema (sucesso/falha e mensagem explicativa).
struct Resultado {
    bool ok;
    string mensagem;
};

// Estrutura para agrupar os dados completos de consulta de um veículo.
struct InfoVeiculo {
    DadosVeiculo dados;
    bool estacionado;
    string horarioEntrada;
};

// Estrutura com o balanço de vagas ocupadas e disponíveis por categoria.
struct ResumoVagas {
    int livresCarro;
    int totalCarro;
    int livresMoto;
    int totalMoto;
};

// Classe principal que orquestra a lógica do estacionamento, integrando vagas, veículos e banco de dados.
class Estacionamento {
private:
    Banco& banco;              // Referência para a camada de persistência.
    vector<Vaga> vagasCarro;   // Lista de vagas para carros e caminhonetes.
    vector<Vaga> vagasMoto;    // Lista de vagas para motos.
    double taxaCarro;          // Valor/hora padrão para carros e caminhonetes.
    double taxaMoto;           // Valor/hora padrão para motos.

    // Busca a primeira vaga não ocupada correspondente ao tipo de veículo.
    Vaga* encontrarVagaLivre(const string& tipo);

    // Fábrica de objetos (Factory): instancia a subclasse correta de Veiculo usando polimorfismo.
    unique_ptr<Veiculo> criarVeiculo(const DadosVeiculo& dados);

public:
    // Construtor: recebe a conexão do banco e inicializa a quantidade inicial de vagas.
    Estacionamento(Banco& banco, int quantidadeCarro, int quantidadeMoto);

    // Operações principais de fluxo de veículos:
    Resultado registrarEntrada(const string& placa, const string& modelo,
                               const string& cor, const string& tipo,
                               Ticket* ticket = nullptr);
    Resultado registrarSaida(const string& placa, Ticket* ticket = nullptr);

    // Gestão e cadastro de veículos:
    Resultado editarVeiculo(const string& placa, const string& novoModelo, const string& novaCor);
    Resultado removerVeiculo(const string& placa);

    // Recompõe o estado das vagas em memória com base nos veículos atualmente estacionados no banco.
    void restaurarVagas();

    // Consultas e relatórios:
    bool consultarVeiculo(const string& placa, InfoVeiculo& info);
    vector<VeiculoEstacionado> listarVeiculos();
    ResumoVagas resumoVagas() const;
    vector<RegistroSaida> historico();
    double faturamentoDoDia(const string& data);

    // Configurações do sistema:
    Resultado configurarVagas(int quantidadeCarro, int quantidadeMoto);
    Resultado configurarTaxas(double novaTaxaCarro, double novaTaxaMoto);

    // Getters de consulta rápida:
    double getTaxaCarro() const;
    double getTaxaMoto() const;
    int getTotalVagasCarro() const;
    int getTotalVagasMoto() const;
    const vector<Vaga>& getVagasCarro() const;
    const vector<Vaga>& getVagasMoto() const;
};

#endif