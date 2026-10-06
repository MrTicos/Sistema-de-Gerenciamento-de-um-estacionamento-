#ifndef JANELAPRINCIPAL_H
#define JANELAPRINCIPAL_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTableWidget>

class Estacionamento;


class JanelaPrincipal : public QMainWindow {
    Q_OBJECT

public:
    explicit JanelaPrincipal(Estacionamento& estacionamento, QWidget* pai = nullptr);

private slots:
    void registrarEntrada();          
    void registrarSaida();             
    void usarPlacaDaLinha(int linha);  

private:
    void atualizar();                 
    void mostrarTicket(const QString& titulo, const QString& texto);

    Estacionamento& estacionamento;    
    QLineEdit* campoPlaca;
    QLineEdit* campoModelo;
    QLineEdit* campoCor;
    QComboBox* comboTipo;
    QLabel* textoVagas;
    QTableWidget* tabela;
};

#endif
