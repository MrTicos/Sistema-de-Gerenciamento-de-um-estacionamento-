#include "JanelaPrincipal.h"
#include "Estacionamento.h"
#include <QFontDatabase>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
// std::string (núcleo) -> QString (Qt)
QString texto(const std::string& s) {
    return QString::fromUtf8(s.c_str());
}
}

JanelaPrincipal::JanelaPrincipal(Estacionamento& estacionamento, QWidget* pai)
    : QMainWindow(pai), estacionamento(estacionamento) {
    setWindowTitle("Estacionamento");
    resize(900, 460);

    // ----- Lado esquerdo: formulário e botões -----
    auto* grupoForm = new QGroupBox("Veículo");
    grupoForm->setFixedWidth(300);
    auto* layoutForm = new QVBoxLayout(grupoForm);

    campoPlaca = new QLineEdit;
    campoPlaca->setPlaceholderText("Ex.: ABC1D23");
    campoModelo = new QLineEdit;
    campoCor = new QLineEdit;
    comboTipo = new QComboBox;
    comboTipo->addItems({"Carro", "Moto", "Caminhonete"});   // mesmos nomes usados no banco

    auto* form = new QFormLayout;
    form->addRow("Placa:", campoPlaca);
    form->addRow("Modelo:", campoModelo);
    form->addRow("Cor:", campoCor);
    form->addRow("Tipo:", comboTipo);
    layoutForm->addLayout(form);

    auto* botaoEntrada = new QPushButton("Registrar entrada");
    auto* botaoSaida = new QPushButton("Registrar saída");
    layoutForm->addWidget(botaoEntrada);
    layoutForm->addWidget(botaoSaida);
    layoutForm->addStretch();

    auto* aviso = new QLabel("Veículo já cadastrado? Digite só a placa:\nmodelo, cor e tipo são reaproveitados.");
    aviso->setWordWrap(true);
    layoutForm->addWidget(aviso);

    // ----- Lado direito: vagas livres e tabela -----
    textoVagas = new QLabel;
    QFont fonte = textoVagas->font();
    fonte.setBold(true);
    textoVagas->setFont(fonte);

    tabela = new QTableWidget;
    tabela->setColumnCount(6);
    tabela->setHorizontalHeaderLabels({"Vaga", "Placa", "Tipo", "Modelo", "Cor", "Entrada"});
    tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabela->verticalHeader()->setVisible(false);
    tabela->setWordWrap(false);
    tabela->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Vaga e Entrada ocupam só o espaço do conteúdo (a data não quebra em duas linhas).
    tabela->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tabela->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    auto* layoutDireita = new QVBoxLayout;
    layoutDireita->addWidget(textoVagas);
    layoutDireita->addWidget(new QLabel("Veículos no estacionamento (clique numa linha para usar a placa):"));
    layoutDireita->addWidget(tabela);

    auto* central = new QWidget;
    auto* layoutPrincipal = new QHBoxLayout(central);
    layoutPrincipal->addWidget(grupoForm);
    layoutPrincipal->addLayout(layoutDireita, 1);
    setCentralWidget(central);

    // Liga os botões aos métodos (sinais e slots do Qt).
    connect(botaoEntrada, &QPushButton::clicked, this, &JanelaPrincipal::registrarEntrada);
    connect(botaoSaida, &QPushButton::clicked, this, &JanelaPrincipal::registrarSaida);
    connect(tabela, &QTableWidget::cellClicked, this, [this](int linha, int) { usarPlacaDaLinha(linha); });

    atualizar();
}

// Mostra as vagas livres e preenche a tabela com quem está no pátio.
void JanelaPrincipal::atualizar() {
    ResumoVagas vagas = estacionamento.resumoVagas();
    textoVagas->setText(QString("Vagas livres  |  Carro/Caminhonete: %1 de %2  |  Moto: %3 de %4")
                            .arg(vagas.livresCarro).arg(vagas.totalCarro)
                            .arg(vagas.livresMoto).arg(vagas.totalMoto));

    std::vector<VeiculoEstacionado> veiculos = estacionamento.listarVeiculos();
    tabela->setRowCount(static_cast<int>(veiculos.size()));

    for (size_t i = 0; i < veiculos.size(); i++) {
        const VeiculoEstacionado& v = veiculos[i];
        int linha = static_cast<int>(i);
        tabela->setItem(linha, 0, new QTableWidgetItem(QString::number(v.vaga)));
        tabela->setItem(linha, 1, new QTableWidgetItem(texto(v.veiculo.placa)));
        tabela->setItem(linha, 2, new QTableWidgetItem(texto(v.veiculo.tipo)));
        tabela->setItem(linha, 3, new QTableWidgetItem(texto(v.veiculo.modelo)));
        tabela->setItem(linha, 4, new QTableWidgetItem(texto(v.veiculo.cor)));
        tabela->setItem(linha, 5, new QTableWidgetItem(texto(v.horarioEntrada)));
    }
}

void JanelaPrincipal::registrarEntrada() {
    // A placa é guardada sem espaços nas pontas e em maiúsculas.
    std::string placa = campoPlaca->text().trimmed().toUpper().toStdString();
    Ticket ticket("", "", "", "", 0, "");   // a lógica preenche este ticket

    Resultado resultado = estacionamento.registrarEntrada(
        placa, campoModelo->text().trimmed().toStdString(),
        campoCor->text().trimmed().toStdString(),
        comboTipo->currentText().toStdString(), &ticket);

    if (!resultado.ok) {
        QMessageBox::warning(this, "Não foi possível registrar a entrada", texto(resultado.mensagem));
        return;
    }

    // Limpa o formulário para o próximo veículo.
    campoPlaca->clear();
    campoModelo->clear();
    campoCor->clear();
    comboTipo->setCurrentIndex(0);

    atualizar();
    mostrarTicket("Entrada registrada", texto(ticket.textoEntrada()));
}

void JanelaPrincipal::registrarSaida() {
    std::string placa = campoPlaca->text().trimmed().toUpper().toStdString();
    Ticket ticket("", "", "", "", 0, "");

    Resultado resultado = estacionamento.registrarSaida(placa, &ticket);

    if (!resultado.ok) {
        QMessageBox::warning(this, "Não foi possível registrar a saída", texto(resultado.mensagem));
        return;
    }

    campoPlaca->clear();
    atualizar();
    mostrarTicket("Saída registrada", texto(ticket.textoSaida()));
}

void JanelaPrincipal::usarPlacaDaLinha(int linha) {
    campoPlaca->setText(tabela->item(linha, 1)->text());
}

// Mostra o ticket numa caixa de mensagem, com fonte de largura fixa (parece um comprovante).
void JanelaPrincipal::mostrarTicket(const QString& titulo, const QString& conteudo) {
    QMessageBox caixa(QMessageBox::Information, titulo, conteudo, QMessageBox::Ok, this);
    caixa.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    caixa.exec();
}
