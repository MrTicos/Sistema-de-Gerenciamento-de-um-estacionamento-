#include "Estacionamento.h"
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
using namespace std;


void limparEntrada() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int lerInteiro(const string& mensagem) {
    int valor;

    while (true) {
        cout << mensagem;

        if (cin >> valor) {
            limparEntrada();
            return valor;
        }

        cout << "Digite um numero valido.\n";
        cin.clear();
        limparEntrada();
    }
}

double lerDouble(const string& mensagem) {
    double valor;

    while (true) {
        cout << mensagem;

        if (cin >> valor) {
            limparEntrada();
            return valor;
        }

        cout << "Digite um valor valido.\n";
        cin.clear();
        limparEntrada();
    }
}

string lerTexto(const string& mensagem) {
    string texto;
    cout << mensagem;
    getline(cin, texto);
    return texto;
}

string escolherTipo() {
    while (true) {
        cout << "1 - Carro\n";
        cout << "2 - Moto\n";
        cout << "3 - Caminhonete\n";

        int opcao = lerInteiro("Tipo: ");

        if (opcao == 1) return "Carro";
        if (opcao == 2) return "Moto";
        if (opcao == 3) return "Caminhonete";

        cout << "Opcao invalida.\n";
    }
}


void mostrarResultado(const Resultado& resultado) {
    cout << resultado.mensagem << "\n";
}

void mostrarVagas(const Estacionamento& estacionamento) {
    ResumoVagas resumo = estacionamento.resumoVagas();

    cout << "\n--- VAGAS ---\n";
    cout << "Carro/Caminhonete: " << resumo.livresCarro << "/" << resumo.totalCarro << " livres\n";
    cout << "Moto: " << resumo.livresMoto << "/" << resumo.totalMoto << " livres\n";
}

void mostrarVeiculo(Estacionamento& estacionamento, const string& placa) {
    InfoVeiculo info;

    if (!estacionamento.consultarVeiculo(placa, info)) {
        cout << "Veiculo nao encontrado.\n";
        return;
    }

    cout << "\n--- VEICULO ---\n";
    cout << "Placa: " << info.dados.placa << "\n";
    cout << "Tipo: " << info.dados.tipo << "\n";
    cout << "Modelo: " << info.dados.modelo << "\n";
    cout << "Cor: " << info.dados.cor << "\n";

    if (info.estacionado) {
        cout << "Situacao: Estacionado\n";
        cout << "Entrada: " << info.horarioEntrada << "\n";
    } else {
        cout << "Situacao: Nao esta no estacionamento\n";
    }
}

void mostrarVeiculosEstacionados(Estacionamento& estacionamento) {
    vector<VeiculoEstacionado> veiculos = estacionamento.listarVeiculos();

    if (veiculos.empty()) {
        cout << "Nenhum veiculo esta atualmente no estacionamento.\n";
        return;
    }

    cout << "\n--- VEICULOS NO ESTACIONAMENTO ---\n";
    for (const VeiculoEstacionado& item : veiculos) {
        cout << "Vaga: " << item.vaga
             << " | Placa: " << item.veiculo.placa
             << " | Tipo: " << item.veiculo.tipo
             << " | Modelo: " << item.veiculo.modelo
             << " | Cor: " << item.veiculo.cor
             << " | Entrada: " << item.horarioEntrada << "\n";
    }
}

void mostrarHistorico(Estacionamento& estacionamento) {
    vector<RegistroSaida> registros = estacionamento.historico();

    if (registros.empty()) {
        cout << "Nenhuma saida registrada.\n";
        return;
    }

    cout << "\n--- HISTORICO DE SAIDAS ---\n";
    for (const RegistroSaida& registro : registros) {
        cout << "Placa: " << registro.placa
             << " | Entrada: " << registro.horarioEntrada
             << " | Saida: " << registro.horarioSaida
             << " | Valor: R$ " << fixed << setprecision(2)
             << registro.valorPago << "\n";
    }
}

void mostrarFaturamento(Estacionamento& estacionamento, const string& data) {
    cout << "Faturamento de " << data << ": R$ "
         << fixed << setprecision(2) << estacionamento.faturamentoDoDia(data) << "\n";
}


void registrarEntrada(Estacionamento& estacionamento) {
    string placa = lerTexto("Placa: ");
    InfoVeiculo existente;
    Ticket ticket("", "", "", "", 0, "");   

    if (estacionamento.consultarVeiculo(placa, existente)) {
        cout << "\nVeiculo ja cadastrado:\n";
        cout << "Modelo: " << existente.dados.modelo << "\n";
        cout << "Cor: " << existente.dados.cor << "\n";
        cout << "Tipo: " << existente.dados.tipo << "\n";

        string resposta = lerTexto("Deseja registrar a entrada? (s/n): ");
        if (resposta != "s" && resposta != "S") {
            return;
        }

        Resultado r = estacionamento.registrarEntrada(existente.dados.placa, existente.dados.modelo,
                                                      existente.dados.cor, existente.dados.tipo, &ticket);
        mostrarResultado(r);
        if (r.ok) ticket.imprimirEntrada();
        return;
    }

    string modelo = lerTexto("Modelo: ");
    string cor = lerTexto("Cor: ");
    string tipo = escolherTipo();

    Resultado r = estacionamento.registrarEntrada(placa, modelo, cor, tipo, &ticket);
    mostrarResultado(r);
    if (r.ok) ticket.imprimirEntrada();
}

void registrarSaida(Estacionamento& estacionamento) {
    Ticket ticket("", "", "", "", 0, "");
    Resultado r = estacionamento.registrarSaida(lerTexto("Placa: "), &ticket);
    mostrarResultado(r);
    if (r.ok) ticket.imprimirSaida();
}

void configurarVagas(Estacionamento& estacionamento) {
    cout << "\n--- VAGAS ---\n";
    cout << "Carro/Caminhonete: " << estacionamento.getTotalVagasCarro() << "\n";
    cout << "Moto: " << estacionamento.getTotalVagasMoto() << "\n\n";

    int vagasCarro = lerInteiro("Nova quantidade de vagas para carro/caminhonete: ");
    int vagasMoto = lerInteiro("Nova quantidade de vagas para moto: ");

    mostrarResultado(estacionamento.configurarVagas(vagasCarro, vagasMoto));
}

void configurarTaxas(Estacionamento& estacionamento) {
    cout << "\n--- TARIFAS ---\n";
    cout << "Carro/Caminhonete: R$ " << estacionamento.getTaxaCarro() << " por hora\n";
    cout << "Moto: R$ " << estacionamento.getTaxaMoto() << " por hora\n\n";

    double taxaCarro = lerDouble("Nova taxa para carro/caminhonete por hora: R$ ");
    double taxaMoto = lerDouble("Nova taxa para moto por hora: R$ ");

    mostrarResultado(estacionamento.configurarTaxas(taxaCarro, taxaMoto));
}

void menuConfiguracoes(Estacionamento& estacionamento) {
    int opcao;

    do {
        cout << "\n==============================\n";
        cout << "        CONFIGURACOES         \n";
        cout << "==============================\n";
        cout << "1 - Alterar quantidade de vagas\n";
        cout << "2 - Alterar tarifas\n";
        cout << "0 - Voltar\n";

        opcao = lerInteiro("Escolha uma opcao: ");
        cout << "\n";

        switch (opcao) {
            case 1:
                configurarVagas(estacionamento);
                break;
            case 2:
                configurarTaxas(estacionamento);
                break;
            case 0:
                break;
            default:
                cout << "Opcao invalida.\n";
        }
    } while (opcao != 0);
}

void gerenciarVeiculo(Estacionamento& estacionamento) {
    string placa, modelo, cor;
    int opcao;

    cout << "1 - Editar veiculo\n2 - Remover veiculo\n0 - Voltar\nOpcao: ";
    cin >> opcao;
    if (opcao == 0) return;

    cout << "Placa: ";
    cin >> placa;

    if (opcao == 1) {
        cout << "Novo modelo: ";
        cin >> modelo;
        cout << "Nova cor: ";
        cin >> cor;
        mostrarResultado(estacionamento.editarVeiculo(placa, modelo, cor));
    } else if (opcao == 2) {
        mostrarResultado(estacionamento.removerVeiculo(placa));
    }
}

void mostrarMenu() {
    cout << "\n==============================\n";
    cout << "       ESTACIONAMENTO         \n";
    cout << "==============================\n";
    cout << "1 - Registrar entrada\n";
    cout << "2 - Registrar saida\n";
    cout << "3 - Consultar vagas\n";
    cout << "4 - Consultar veiculo\n";
    cout << "5 - Listar veiculos no estacionamento\n";
    cout << "6 - Historico de saidas\n";
    cout << "7 - Faturamento do dia\n";
    cout << "8 - Gerenciar veiculo\n";
    cout << "9 - Configuracoes\n";
    cout << "0 - Sair\n";
}

int main() {
    Banco banco("estacionamento.db");

    if (!banco.criarTabelas()) {
        cout << "Nao foi possivel preparar o banco de dados.\n";
        return 1;
    }

    Estacionamento estacionamento(banco, 20, 10);

    int opcao;

    do {
        mostrarMenu();
        opcao = lerInteiro("Escolha uma opcao: ");
        cout << "\n";

        switch (opcao) {
            case 1: registrarEntrada(estacionamento); break;
            case 2: registrarSaida(estacionamento); break;
            case 3: mostrarVagas(estacionamento); break;
            case 4: mostrarVeiculo(estacionamento, lerTexto("Placa: ")); break;
            case 5: mostrarVeiculosEstacionados(estacionamento); break;
            case 6: mostrarHistorico(estacionamento); break;
            case 7: mostrarFaturamento(estacionamento, lerTexto("Data (AAAA-MM-DD): ")); break;
            case 8: gerenciarVeiculo(estacionamento); break;
            case 9: menuConfiguracoes(estacionamento); break;
            case 0: cout << "Encerrando o sistema.\n"; break;
            default: cout << "Opcao invalida.\n";
        }
    } while (opcao != 0);

    return 0;
}
