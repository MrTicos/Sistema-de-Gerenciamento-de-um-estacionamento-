#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

#include "Banco.h"
#include "Ticket.h"
#include "Vaga.h"
#include "Veiculo.h"
#include <memory>
#include <string>
#include <vector>

/**
 * Resultado de uma operação: diz se deu certo e traz a mensagem para o usuário.
 * A lógica NÃO imprime nada: quem chama (terminal ou janela) decide como exibir.
 */
struct Resultado {
    bool ok;                  ///< true se a operação deu certo.
    std::string mensagem;     ///< Texto para mostrar ao usuário.
};

/// Dados completos de um veículo consultado (cadastro + situação atual).
struct InfoVeiculo {
    DadosVeiculo dados;
    bool estacionado;               ///< true se está no estacionamento agora.
    std::string horarioEntrada;     ///< Preenchido só se estacionado.
};

/// Contagem de vagas livres e totais.
struct ResumoVagas {
    int livresCarro;
    int totalCarro;
    int livresMoto;
    int totalMoto;
};

/**
 * Coração do sistema: concentra TODAS as regras (entrada, saída, vagas, tarifas).
 * Não sabe se existe terminal ou janela: devolve Resultado e dados.
 *
 * Relações: tem várias Vaga (composição), usa o Banco por referência
 * e cria objetos Veiculo (Carro, Moto ou Caminhonete) quando precisa deles.
 */
class Estacionamento {
private:
    Banco& banco;                      ///< Referência: o Banco é compartilhado, não copiado.
    std::vector<Vaga> vagasCarro;      ///< Vagas de carro e caminhonete.
    std::vector<Vaga> vagasMoto;       ///< Vagas de moto.
    double taxaCarro;                  ///< Taxa por hora de carro e caminhonete (R$).
    double taxaMoto;                   ///< Taxa por hora de moto (R$).

    /**
     * Fábrica: transforma o tipo guardado no banco (texto) no objeto certo.
     * É o ÚNICO lugar que compara o nome do tipo; depois disso tudo é polimórfico.
     * @param dados Dados cadastrais do veículo.
     * @return Objeto Carro, Moto ou Caminhonete (devolvido como Veiculo).
     */
    std::unique_ptr<Veiculo> criarVeiculo(const DadosVeiculo& dados) const;

    /**
     * Escolhe o grupo de vagas perguntando ao PRÓPRIO veículo (podeUsarVagaMoto()).
     * @param veiculo O veículo que vai estacionar.
     * @return O vetor de vagas de moto ou de carro.
     */
    std::vector<Vaga>& vagasPara(const Veiculo& veiculo);
    const std::vector<Vaga>& vagasPara(const Veiculo& veiculo) const;

    /**
     * Procura a primeira vaga livre do grupo adequado ao veículo.
     * @param veiculo O veículo que vai estacionar.
     * @return Ponteiro para a vaga livre, ou nullptr se estiver lotado.
     */
    Vaga* encontrarVagaLivre(const Veiculo& veiculo);

public:
    /**
     * Cria o estacionamento e já restaura a ocupação das vagas a partir do banco.
     * @param banco           Banco de dados (já com as tabelas criadas).
     * @param quantidadeCarro Número de vagas de carro/caminhonete.
     * @param quantidadeMoto  Número de vagas de moto.
     */
    Estacionamento(Banco& banco, int quantidadeCarro, int quantidadeMoto);

    /**
     * Registra a entrada de um veículo. Se a placa já tem cadastro, os dados do
     * cadastro são usados (modelo, cor e tipo informados são ignorados).
     * @param placa  Placa do veículo.
     * @param modelo Modelo (obrigatório só para veículo novo).
     * @param cor    Cor (obrigatória só para veículo novo).
     * @param tipo   "Carro", "Moto" ou "Caminhonete" (só para veículo novo).
     * @param ticket Se não for nulo, é preenchido com o comprovante de entrada.
     * @return Resultado com sucesso/falha e a mensagem.
     */
    Resultado registrarEntrada(const std::string& placa, const std::string& modelo,
                               const std::string& cor, const std::string& tipo,
                               Ticket* ticket = nullptr);

    /**
     * Registra a saída, calcula o valor (cada tipo de veículo com a sua regra) e libera a vaga.
     * @param placa  Placa do veículo.
     * @param ticket Se não for nulo, é preenchido com o comprovante de saída.
     * @return Resultado com sucesso/falha e a mensagem.
     */
    Resultado registrarSaida(const std::string& placa, Ticket* ticket = nullptr);

    /**
     * Altera modelo e cor de um veículo cadastrado.
     * @return Resultado com sucesso/falha e a mensagem.
     */
    Resultado editarVeiculo(const std::string& placa, const std::string& novoModelo,
                            const std::string& novaCor);

    /**
     * Remove o cadastro de um veículo que não está estacionado (o histórico é mantido).
     * @return Resultado com sucesso/falha e a mensagem.
     */
    Resultado removerVeiculo(const std::string& placa);

    /**
     * Marca como ocupadas as vagas dos veículos que o banco diz estarem estacionados.
     * Usa o número de vaga gravado no banco; se não houver (banco antigo) ou estiver
     * ocupado, usa a primeira vaga livre. É chamado pelo construtor.
     */
    void restaurarVagas();

    /**
     * Consulta um veículo pela placa.
     * @param placa Placa procurada.
     * @param info  Preenchido se encontrar.
     * @return true se o veículo está cadastrado.
     */
    bool consultarVeiculo(const std::string& placa, InfoVeiculo& info);

    /// @return Os veículos que estão no estacionamento agora, com o número da vaga preenchido.
    std::vector<VeiculoEstacionado> listarVeiculos();

    /// @return Vagas livres e totais por grupo.
    ResumoVagas resumoVagas() const;

    /// @return Histórico de saídas (da mais recente para a mais antiga).
    std::vector<RegistroSaida> historico();

    /**
     * @param data Dia no formato AAAA-MM-DD.
     * @return Total faturado naquele dia, em reais.
     */
    double faturamentoDoDia(const std::string& data);

    /**
     * Altera a quantidade de vagas. Tudo ou nada: se alguma vaga que seria removida
     * estiver ocupada, nada é alterado.
     * @return Resultado com sucesso/falha e a mensagem.
     */
    Resultado configurarVagas(int quantidadeCarro, int quantidadeMoto);

    /**
     * Altera as taxas por hora.
     * @param novaTaxaCarro Taxa de carro e caminhonete (R$/hora).
     * @param novaTaxaMoto  Taxa de moto (R$/hora).
     * @return Resultado com sucesso/falha e a mensagem.
     */
    Resultado configurarTaxas(double novaTaxaCarro, double novaTaxaMoto);

    double getTaxaCarro() const;                       ///< @return Taxa por hora de carro/caminhonete.
    double getTaxaMoto() const;                        ///< @return Taxa por hora de moto.
    int getTotalVagasCarro() const;                    ///< @return Quantidade de vagas de carro.
    int getTotalVagasMoto() const;                     ///< @return Quantidade de vagas de moto.
    const std::vector<Vaga>& getVagasCarro() const;    ///< @return Vagas de carro (somente leitura).
    const std::vector<Vaga>& getVagasMoto() const;     ///< @return Vagas de moto (somente leitura).
};

#endif
