#ifndef VEICULO_H
#define VEICULO_H

#include <string>

/**
 * Classe base ABSTRATA de todos os veículos do estacionamento.
 *
 * Guarda os dados comuns (placa, modelo, cor, taxa por hora) e declara, como
 * métodos virtuais puros, o que cada tipo de veículo precisa responder por conta
 * própria. Quem usa um Veiculo (por ponteiro ou referência) não precisa saber se
 * ele é Carro, Moto ou Caminhonete: isso é o polimorfismo.
 */
class Veiculo {
protected:
    std::string placa;      ///< Placa do veículo (identifica o veículo no sistema).
    std::string modelo;     ///< Modelo, por exemplo "Civic".
    std::string cor;        ///< Cor do veículo.
    double taxaHora;        ///< Valor cobrado por hora, em reais.

public:
    /**
     * Cria um veículo.
     * @param placa    Placa do veículo.
     * @param modelo   Modelo do veículo.
     * @param cor      Cor do veículo.
     * @param taxaHora Valor cobrado por hora (R$).
     */
    Veiculo(const std::string& placa, const std::string& modelo,
            const std::string& cor, double taxaHora);

    /// Destrutor virtual: garante que o objeto certo seja destruído ao apagar via Veiculo*.
    virtual ~Veiculo() = default;

    /// @return A placa do veículo.
    std::string getPlaca() const;
    /// @return O modelo do veículo.
    std::string getModelo() const;
    /// @return A cor do veículo.
    std::string getCor() const;

    /// @return O nome do tipo ("Carro", "Moto" ou "Caminhonete"), definido por cada subclasse.
    virtual std::string getTipo() const = 0;

    /**
     * Calcula o valor a pagar pela permanência. Cada subclasse tem a sua regra.
     * @param minutos Tempo total de permanência, em minutos.
     * @return Valor a pagar, em reais.
     */
    virtual double calcularTarifa(double minutos) const = 0;

    /**
     * Informa em qual grupo de vagas o veículo estaciona.
     * O Estacionamento usa este método para escolher entre vagas de moto e
     * vagas de carro, sem precisar comparar o nome do tipo.
     * @return true se estaciona em vaga de moto; false se estaciona em vaga de carro.
     */
    virtual bool podeUsarVagaMoto() const = 0;
};

#endif
