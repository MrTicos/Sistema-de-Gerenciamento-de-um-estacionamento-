#ifndef CAMINHONETE_H
#define CAMINHONETE_H

#include "Veiculo.h"

/**
 * Caminhonete. Usa vaga de carro (a vaga é grande), mas paga um adicional por
 * ocupar mais espaço.
 */
class Caminhonete : public Veiculo {
public:
    /// Adicional cobrado sobre a tarifa normal (0.20 = 20%).
    static constexpr double ADICIONAL = 0.20;

    /// @copydoc Veiculo::Veiculo
    Caminhonete(const std::string& placa, const std::string& modelo,
                const std::string& cor, double taxaHora);

    /// @return "Caminhonete".
    std::string getTipo() const override;

    /**
     * Tarifa da caminhonete: minutos x (taxaHora / 60) x (1 + ADICIONAL).
     * @param minutos Tempo de permanência, em minutos.
     * @return Valor a pagar, em reais.
     */
    double calcularTarifa(double minutos) const override;

    /// @return false: caminhonete usa vaga de carro.
    bool podeUsarVagaMoto() const override;
};

#endif
