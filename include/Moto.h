#ifndef MOTO_H
#define MOTO_H

#include "Veiculo.h"

/**
 * Moto. Usa vaga de moto e tem uma tolerância gratuita de curta permanência.
 */
class Moto : public Veiculo {
public:
    /// Permanência (em minutos) até a qual a moto não paga nada.
    static constexpr double TOLERANCIA_MINUTOS = 15.0;

    /// @copydoc Veiculo::Veiculo
    Moto(const std::string& placa, const std::string& modelo,
         const std::string& cor, double taxaHora);

    /// @return "Moto".
    std::string getTipo() const override;

    /**
     * Tarifa da moto: até TOLERANCIA_MINUTOS não paga; acima disso paga o tempo
     * total, proporcional: minutos x (taxaHora / 60).
     * @param minutos Tempo de permanência, em minutos.
     * @return Valor a pagar, em reais.
     */
    double calcularTarifa(double minutos) const override;

    /// @return true: moto usa vaga de moto.
    bool podeUsarVagaMoto() const override;
};

#endif
