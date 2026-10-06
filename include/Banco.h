#ifndef BANCO_H
#define BANCO_H

#include <string>
#include <vector>

/// Uma saída já registrada (linha do histórico).
struct RegistroSaida {
    int id;
    std::string placa;
    std::string horarioEntrada;
    std::string horarioSaida;
    double valorPago;
};

/// Dados cadastrais de um veículo.
struct DadosVeiculo {
    std::string placa;
    std::string modelo;
    std::string cor;
    std::string tipo;    ///< "Carro", "Moto" ou "Caminhonete".
};

/// Um veículo que está no estacionamento agora.
struct VeiculoEstacionado {
    DadosVeiculo veiculo;
    int vaga;                      ///< Número da vaga (0 se o banco não souber).
    std::string horarioEntrada;
};

/**
 * Único ponto de acesso ao banco de dados SQLite. Implementa o CRUD.
 *
 * Nenhuma outra classe conhece o SQLite: o ponteiro da conexão fica escondido
 * (void*) para que quem inclui este arquivo não precise incluir sqlite3.h.
 * Todos os comandos SQL usam parâmetros ("?"), nunca texto concatenado
 * (evita SQL injection).
 */
class Banco {
private:
    void* banco;                                   ///< Conexão SQLite (sqlite3*), escondida.
    bool executar(const std::string& sql);         ///< Executa SQL sem parâmetros.
    bool historicoPrecisaMigrar();                 ///< Detecta tabela de histórico em formato antigo.
    bool migrarHistorico();                        ///< Converte o formato antigo para o atual.

public:
    /**
     * Abre (ou cria) o arquivo do banco.
     * @param nomeArquivo Caminho do arquivo .db.
     */
    Banco(const std::string& nomeArquivo);

    /// Fecha a conexão.
    ~Banco();

    /**
     * Cria as tabelas se ainda não existirem e atualiza bancos de versões antigas.
     * @return true se o banco ficou pronto para uso.
     */
    bool criarTabelas();

    // ----- CREATE -----

    /**
     * Cadastra um veículo novo.
     * @param veiculo Dados do veículo.
     * @return true se cadastrou (false, por exemplo, se a placa já existe).
     */
    bool cadastrarVeiculo(const DadosVeiculo& veiculo);

    /**
     * Registra a entrada de um veículo no estacionamento.
     * @param placa          Placa do veículo.
     * @param horarioEntrada Horário (AAAA-MM-DD HH:MM:SS).
     * @param vaga           Número da vaga ocupada (guardado para restaurar a ocupação ao reabrir o programa).
     * @return true se gravou.
     */
    bool registrarEntrada(const std::string& placa, const std::string& horarioEntrada, int vaga);

    // ----- READ -----

    /**
     * Busca o cadastro de um veículo pela placa.
     * @param placa   Placa procurada.
     * @param veiculo Preenchido com os dados se encontrar.
     * @return true se encontrou.
     */
    bool buscarVeiculo(const std::string& placa, DadosVeiculo& veiculo);

    /// @return Os veículos que estão no estacionamento agora (ordenados por placa).
    std::vector<VeiculoEstacionado> listarVeiculosEstacionados();

    /**
     * @param placa Placa do veículo.
     * @return true se o veículo está no estacionamento agora (entrou e ainda não saiu).
     */
    bool veiculoEstaEstacionado(const std::string& placa);

    /**
     * @param placa Placa do veículo estacionado.
     * @return Horário de entrada da permanência em andamento (vazio se não houver).
     */
    std::string buscarEntrada(const std::string& placa);

    /// @return Todas as saídas registradas, da mais recente para a mais antiga.
    std::vector<RegistroSaida> listarHistorico();

    /**
     * Soma o que foi pago nas saídas de um dia.
     * @param data Dia no formato AAAA-MM-DD.
     * @return Total faturado no dia, em reais (0 se não houve saídas).
     */
    double calcularFaturamentoDoDia(const std::string& data);

    // ----- UPDATE -----

    /**
     * Atualiza modelo e cor de um veículo cadastrado.
     * @param placa  Placa do veículo (não muda).
     * @param modelo Novo modelo.
     * @param cor    Nova cor.
     * @return true se atualizou.
     */
    bool atualizarVeiculo(const std::string& placa, const std::string& modelo, const std::string& cor);

    /**
     * Registra a saída de um veículo (fecha a permanência em andamento).
     * @param placa        Placa do veículo.
     * @param horarioSaida Horário de saída.
     * @param valorPago    Valor cobrado, em reais.
     * @return true se havia uma permanência em andamento e ela foi fechada.
     */
    bool registrarSaida(const std::string& placa, const std::string& horarioSaida, double valorPago);

    // ----- DELETE -----

    /**
     * Remove o cadastro de um veículo que NÃO está estacionado.
     * O histórico de saídas é preservado (guarda a placa como texto).
     * @param placa Placa do veículo.
     * @return true se removeu.
     */
    bool removerVeiculo(const std::string& placa);
};

#endif
