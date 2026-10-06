#include "Estacionamento.h"
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
using namespace std;

// Limpa os caracteres restantes no buffer de entrada (cin).
void limparEntrada() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Lê um valor inteiro do terminal de forma segura, tratando entradas inválidas (ex: letras).
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

// Lê um valor decimal (double) do terminal com validação contra erros de digitação.
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

// Lê uma linha inteira de texto digitada pelo usuário.
string lerTexto(const string& mensagem) {
    string texto;
    cout << mensagem;
    getline(cin, texto);
    return texto;
}

// Apresenta o menu para seleção da categoria do veículo (Carro, Moto ou Caminhonete).
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

// Exibe na tela a mensagem retornada pelas operações do estacionamento.
void mostrarResultado(const Resultado& resultado) {
    cout << resultado.mensagem << "\n";
}

// Exibe o painel com a quantidade de vagas livres e totais por categoria.
void mostrarVagas(const Estacionamento& estacionamento) {
    ResumoVagas resumo = estacionamento.resumoVagas();

    cout << "\n--- VAGAS ---\n";
    cout << "Carro/Caminhonete: " << resumo.livresCarro << "/" << resumo.totalCarro << " livres\n";
    cout << "Moto: " << resumo.livresMoto << "/" << resumo.totalMoto << " livres\n";
}

// Exibe os detalhes cadastrais e a situação atual de um veículo específico.
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

// Lista no terminal todos os veículos que estão atualmente ocupando vagas.
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

// Exibe o histórico de todos os veículos que já saíram do estacionamento.
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

// Consulta e exibe o faturamento total acumulado em uma data informada.
void mostrarFaturamento(Estacionamento& estacionamento, const string& data) {
    cout << "Faturamento de " << data << ": R$ "
         << fixed << setprecision(2) << estacionamento.faturamentoDoDia(data) << "\n";
}

// Fluxo interativo para registrar a entrada de um veículo e gerar o ticket impresso.
void registrarEntrada(Estacionamento& estacionamento) {
    string placa = lerTexto("Placa: ");
    InfoVeiculo existente;
    Ticket ticket("", "", "", "", 0, "");  

    // Se o veículo já possui cadastro prévio no banco de dados:
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

    // Se é a primeira vez do veículo no sistema:
    string modelo = lerTexto("Modelo: ");
    string cor = lerTexto("Cor: ");
    string tipo = escolherTipo();

    Resultado r = estacionamento.registrarEntrada(placa, modelo, cor, tipo, &ticket);
    mostrarResultado(r);
    if (r.ok) ticket.imprimirEntrada();
}

// Fluxo interativo para registrar a saída de um veículo e imprimir o comprovante de pagamento.
void registrarSaida(Estacionamento& estacionamento) {
    Ticket ticket("", "", "", "", 0, "");
    Resultado r = estacionamento.registrarSaida(lerTexto("Placa: "), &ticket);
    mostrarResultado(r);
    if (r.ok) ticket.imprimirSaida();
}

// Submenu para alterar a quantidade operacional de vagas do estacionamento.
void configurarVagas(Estacionamento& estacionamento) {
    cout << "\n--- VAGAS ---\n";
    cout << "Carro/Caminhonete: " << estacionamento.getTotalVagasCarro() << "\n";
    cout << "Moto: " << estacionamento.getTotalVagasMoto() << "\n\n";

    int vagasCarro = lerInteiro("Nova quantidade de vagas para carro/caminhonete: ");
    int vagasMoto = lerInteiro("Nova quantidade de vagas para moto: ");

    mostrarResultado(estacionamento.configurarVagas(vagasCarro, vagasMoto));
}

// Submenu para reajustar os valores das tarifas/hora por categoria.
void configurarTaxas(Estacionamento& estacionamento) {
    cout << "\n--- TARIFAS ---\n";
    cout << "Carro/Caminhonete: R$ " << estacionamento.getTaxaCarro() << " por hora\n";
    cout << "Moto: R$ " << estacionamento.getTaxaMoto() << " por hora\n\n";

    double taxaCarro = lerDouble("Nova taxa para carro/caminhonete por hora: R$ ");
    double taxaMoto = lerDouble("Nova taxa para moto por hora: R$ ");

    mostrarResultado(estacionamento.configurarTaxas(taxaCarro, taxaMoto));
}

// Menu secundário contendo as opções de configuração do sistema.
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

// Opção para alterar dados cadastrais (modelo/cor) ou excluir um veículo do banco.
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

// Imprime as opções do menu principal.
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

// Ponto de entrada do programa C++.
int main() {
    // Inicializa a conexão com o SQLite.
    Banco banco("estacionamento.db");

    // Tenta preparar/criar as tabelas necessárias no arquivo de banco de dados.
    if (!banco.criarTabelas()) {
        cout << "Nao foi possivel preparar o banco de dados.\n";
        return 1;
    }

    // Instancia o controlador principal do estacionamento (inicia com 20 vagas para carro e 10 para moto).
    Estacionamento estacionamento(banco, 20, 10);

    int opcao;

    // Loop principal da aplicação.
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
