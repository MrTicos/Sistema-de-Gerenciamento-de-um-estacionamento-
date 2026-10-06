#ifndef VAGA_H
#define VAGA_H

#include <string>

/**
 * Uma vaga do estacionamento: tem número, tipo e sabe se está ocupada (e por qual placa).
 * Os atributos são privados: só se altera o estado pelos métodos ocupar() e liberar().
 */
class Vaga {
private:
    int numero;                  ///< Número da vaga (começa em 1).
    std::string tipo;            ///< Tipo da vaga ("Carro" ou "Moto").
    bool ocupada;                ///< true se há veículo na vaga.
    std::string placaVeiculo;    ///< Placa de quem ocupa (vazia se a vaga está livre).

public:
    /**
     * Cria uma vaga livre.
     * @param numero Número da vaga.
     * @param tipo   Tipo da vaga ("Carro" ou "Moto").
     */
    Vaga(int numero, const std::string& tipo);

    /// @return O número da vaga.
    int getNumero() const;
    /// @return O tipo da vaga.
    std::string getTipo() const;
    /// @return true se a vaga está ocupada.
    bool estaOcupada() const;
    /// @return A placa do veículo que ocupa a vaga (vazia se livre).
    std::string getPlacaVeiculo() const;

    /**
     * Marca a vaga como ocupada.
     * @param placa Placa do veículo que está estacionando.
     */
    void ocupar(const std::string& placa);

    /// Marca a vaga como livre e esquece a placa.
    void liberar();
};

#endif
