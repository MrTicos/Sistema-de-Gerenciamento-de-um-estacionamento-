#ifndef CARRO_H
#define CARRO_H

#include "Veiculo.h"

/**
 * Carro de passeio. Usa vaga de carro e paga a tarifa proporcional ao tempo,
 * sem tolerância e sem adicional.
 */
class Carro : public Veiculo {
public:
    /// @copydoc Veiculo::Veiculo
    Carro(const std::string& placa, const std::string& modelo,
          const std::string& cor, double taxaHora);

    /// @return "Carro".
    std::string getTipo() const override;

    /**
     * Tarifa do carro: minutos x (taxaHora / 60).
     * @param minutos Tempo de permanência, em minutos.
     * @return Valor a pagar, em reais.
     */
    double calcularTarifa(double minutos) const override;

    /// @return false: carro usa vaga de carro.
    bool podeUsarVagaMoto() const override;
};

#endif
