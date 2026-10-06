#ifndef TICKET_H
#define TICKET_H

#include <string>
using namespace std;

// Classe responsável por gerar e formatar os recibos (tickets) de entrada e saída do veículo.
class Ticket {
private:
    // Dados do veículo e do momento da entrada.
    string placa;
    string tipo;
    string modelo;
    string cor;
    int numeroVaga;
    string entrada;
    
    // Dados preenchidos apenas no encerramento da permanência.
    string saida;
    double minutos;
    double valor;

public:
    // Construtor: inicializa o ticket com os dados do veículo e da entrada.
    Ticket(const string& placa, const string& tipo, const string& modelo,
           const string& cor, int numeroVaga, const string& entrada);

    // Registra as informações do encerramento (horário de saída, tempo total e valor).
    void definirSaida(const string& saida, double minutos, double valor);

    // Métodos para montagem e formatação do texto dos recibos.
    string textoEntrada() const;
    string textoSaida() const;

    // Métodos para exibição dos recibos no terminal.
    void imprimirEntrada() const;
    void imprimirSaida() const;
};

#endif
