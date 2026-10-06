#ifndef JANELAPRINCIPAL_H
#define JANELAPRINCIPAL_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTableWidget>

class Estacionamento;

// Janela única do sistema: formulário à esquerda, vagas e veículos estacionados à direita.
// Toda a regra de negócio fica em Estacionamento; a janela só lê os campos e mostra os resultados.
class JanelaPrincipal : public QMainWindow {
    Q_OBJECT

public:
    explicit JanelaPrincipal(Estacionamento& estacionamento, QWidget* pai = nullptr);

private slots:
    void registrarEntrada();           // chamado pelo botão "Registrar entrada"
    void registrarSaida();             // chamado pelo botão "Registrar saída"
    void usarPlacaDaLinha(int linha);  // clicar numa linha da tabela preenche a placa

private:
    void atualizar();                  // redesenha o texto de vagas e a tabela
    void mostrarTicket(const QString& titulo, const QString& texto);

    Estacionamento& estacionamento;    // referência: a lógica é compartilhada, não copiada
    QLineEdit* campoPlaca;
    QLineEdit* campoModelo;
    QLineEdit* campoCor;
    QComboBox* comboTipo;
    QLabel* textoVagas;
    QTableWidget* tabela;
};

#endif
