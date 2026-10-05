#include "Estacionamento.h"
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

void registrarEntrada(Estacionamento& estacionamento, Banco& banco) {
    string placa = lerTexto("Placa: ");
    DadosVeiculo existente;

    if (banco.buscarVeiculo(placa, existente)) {
        cout << "\nVeiculo ja cadastrado:\n";
        cout << "Modelo: " << existente.modelo << "\n";
        cout << "Cor: " << existente.cor << "\n";
        cout << "Tipo: " << existente.tipo << "\n";

        string resposta = lerTexto("Deseja registrar a entrada? (s/n): ");
        if (resposta == "s" || resposta == "S") {
            estacionamento.registrarEntrada(existente.placa, existente.modelo,
                                             existente.cor, existente.tipo);
        }
        return;
    }

    string modelo = lerTexto("Modelo: ");
    string cor = lerTexto("Cor: ");
    string tipo = escolherTipo();

    estacionamento.registrarEntrada(placa, modelo, cor, tipo);
}

void configurarVagas(Estacionamento& estacionamento) {
    cout << "\n--- VAGAS ---\n";
    cout << "Carro/Caminhonete: " << estacionamento.getTotalVagasCarro() << "\n";
    cout << "Moto: " << estacionamento.getTotalVagasMoto() << "\n\n";

    int vagasCarro = lerInteiro("Nova quantidade de vagas para carro/caminhonete: ");
    int vagasMoto = lerInteiro("Nova quantidade de vagas para moto: ");

    estacionamento.configurarVagas(vagasCarro, vagasMoto);
}

void configurarTaxas(Estacionamento& estacionamento) {
    cout << "\n--- TARIFAS ---\n";
    cout << "Carro/Caminhonete: R$ " << estacionamento.getTaxaCarro() << " por hora\n";
    cout << "Moto: R$ " << estacionamento.getTaxaMoto() << " por hora\n\n";

    double taxaCarro = lerDouble("Nova taxa para carro/caminhonete por hora: R$ ");
    double taxaMoto = lerDouble("Nova taxa para moto por hora: R$ ");

    estacionamento.configurarTaxas(taxaCarro, taxaMoto);
    cout << "Tarifas atualizadas.\n";
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
            case 1:
                registrarEntrada(estacionamento, banco);
                break;

            case 2:
                estacionamento.registrarSaida(lerTexto("Placa: "));
                break;

            case 3:
                estacionamento.mostrarVagas();
                break;

            case 4:
                estacionamento.consultarVeiculo(lerTexto("Placa: "));
                break;

            case 5:
                estacionamento.listarVeiculos();
                break;

            case 6:
                estacionamento.mostrarHistorico();
                break;

            case 7:
                estacionamento.mostrarFaturamentoDoDia(
                    lerTexto("Data (AAAA-MM-DD): "));
                break;

            case 8: {
                string placa, modelo, cor;
                int opcao;
                cout << "1 - Editar veiculo\n2 - Remover veiculo\n0 - Voltar\nOpcao: ";
                cin >> opcao;
                if (opcao == 0) break;
                cout << "Placa: ";
                cin >> placa;
                if (opcao == 1) {
                    cout << "Novo modelo: ";
                    cin >> modelo;
                    cout << "Nova cor: ";
                    cin >> cor;
                    estacionamento.editarVeiculo(placa, modelo, cor);
                } else if (opcao == 2) {
                    estacionamento.removerVeiculo(placa);
                }
                break;
            }

            case 9:
                menuConfiguracoes(estacionamento);
                break;

            case 0:
                cout << "Encerrando o sistema.\n";
                break;

            default:
                cout << "Opcao invalida.\n";
        }
    } while (opcao != 0);

    return 0;
}
