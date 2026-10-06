#ifndef TICKET_H
#define TICKET_H

#include <string>

/**
 * Comprovante de entrada e de saída de um veículo.
 * Guarda os dados e monta o texto do comprovante; quem decide onde mostrar
 * (terminal ou janela) é quem chama.
 */
class Ticket {
private:
    std::string placa;
    std::string tipo;
    std::string modelo;
    std::string cor;
    int numeroVaga;
    std::string entrada;     ///< Horário de entrada (AAAA-MM-DD HH:MM:SS).
    std::string saida;       ///< Horário de saída (vazio até definirSaida()).
    double minutos;          ///< Tempo de permanência.
    double valor;            ///< Valor pago, em reais.

public:
    /**
     * Cria o ticket no momento da entrada.
     * @param placa      Placa do veículo.
     * @param tipo       Tipo do veículo.
     * @param modelo     Modelo do veículo.
     * @param cor        Cor do veículo.
     * @param numeroVaga Número da vaga ocupada.
     * @param entrada    Horário de entrada.
     */
    Ticket(const std::string& placa, const std::string& tipo, const std::string& modelo,
           const std::string& cor, int numeroVaga, const std::string& entrada);

    /**
     * Completa o ticket com os dados da saída.
     * @param saida   Horário de saída.
     * @param minutos Tempo de permanência, em minutos.
     * @param valor   Valor a pagar, em reais.
     */
    void definirSaida(const std::string& saida, double minutos, double valor);

    /// @return O texto do comprovante de entrada.
    std::string textoEntrada() const;
    /// @return O texto do comprovante de saída.
    std::string textoSaida() const;

    /// Imprime o comprovante de entrada no terminal.
    void imprimirEntrada() const;
    /// Imprime o comprovante de saída no terminal.
    void imprimirSaida() const;
};

#endif
