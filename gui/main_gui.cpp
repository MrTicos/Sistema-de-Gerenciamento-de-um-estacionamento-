#include "Banco.h"
#include "Estacionamento.h"
#include "JanelaPrincipal.h"
#include <QApplication>
#include <QMessageBox>
#include <QString>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");   

    QString arquivoBanco = qEnvironmentVariable("ESTACIONAMENTO_DB", "estacionamento.db");

    Banco banco(arquivoBanco.toStdString());
    if (!banco.criarTabelas()) {
        QMessageBox::critical(nullptr, "Erro", "Nao foi possivel preparar o banco de dados.");
        return 1;
    }

    Estacionamento estacionamento(banco, 20, 10);

    JanelaPrincipal janela(estacionamento);
    janela.show();

    return app.exec();
}
