#include "Estacionamento.h"
#include "Carro.h"
#include "Moto.h"
#include "Caminhonete.h"
#include "Ticket.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

namespace {
string horarioAtual() {
    auto agora = chrono::system_clock::now();
    time_t tempo = chrono::system_clock::to_time_t(agora);
    tm* tmAtual = localtime(&tempo);

    ostringstream saida;
    saida << put_time(tmAtual, "%Y-%m-%d %H:%M:%S");
    return saida.str();
}

long long converterParaMinutos(const string& horario) {
    tm tmHorario = {};
    istringstream entrada(horario);
    entrada >> get_time(&tmHorario, "%Y-%m-%d %H:%M:%S");
    time_t tempo = mktime(&tmHorario);
    return static_cast<long long>(tempo / 60);
}
}

Estacionamento::Estacionamento(Banco& banco, int quantidadeCarro, int quantidadeMoto)
    : banco(banco), taxaCarro(10.0), taxaMoto(5.0) {
    configurarVagas(quantidadeCarro, quantidadeMoto);
}

Vaga* Estacionamento::encontrarVagaLivre(const string& tipo) {
    if (tipo == "Moto") {
        for (Vaga& vaga : vagasMoto) {
            if (!vaga.estaOcupada()) {
                return &vaga;
            }
        }
    } else {
        for (Vaga& vaga : vagasCarro) {
            if (!vaga.estaOcupada()) {
                return &vaga;
            }
        }
    }

    return nullptr;
}

unique_ptr<Veiculo> Estacionamento::criarVeiculo(const DadosVeiculo& dados) {
    if (dados.tipo == "Moto") {
        return make_unique<Moto>(dados.placa, dados.modelo, dados.cor, taxaMoto);
    }

    if (dados.tipo == "Caminhonete") {
        return make_unique<Caminhonete>(dados.placa, dados.modelo, dados.cor, taxaCarro);
    }

    return make_unique<Carro>(dados.placa, dados.modelo, dados.cor, taxaCarro);
}

bool Estacionamento::registrarEntrada(const string& placa, const string& modelo,
                                      const string& cor, const string& tipo) {
    if (banco.veiculoEstaEstacionado(placa)) {
        cout << "Este veiculo ja esta estacionado.\n";
        return false;
    }

    Vaga* vaga = encontrarVagaLivre(tipo);
    if (vaga == nullptr) {
        if (tipo == "Moto") {
            cout << "Todas as vagas para motos estao ocupadas.\n";
        } else {
            cout << "Todas as vagas para carros e caminhonetes estao ocupadas.\n";
        }
        return false;
    }

    DadosVeiculo dados{placa, modelo, cor, tipo};
    DadosVeiculo cadastrado;

    if (!banco.buscarVeiculo(placa, cadastrado)) {
        if (!banco.cadastrarVeiculo(dados)) {
            cout << "Nao foi possivel cadastrar o veiculo.\n";
            return false;
        }
    }

    vaga->ocupar(placa);

    string entrada = horarioAtual();

    if (!banco.registrarEntrada(placa, entrada)) {
        vaga->liberar();
        cout << "Nao foi possivel registrar a entrada.\n";
        return false;
    }

    cout << "Entrada registrada com sucesso!\n";
    Ticket ticket(dados.placa, dados.tipo, dados.modelo, dados.cor,
                  vaga->getNumero(), entrada);
    ticket.imprimirEntrada();
    return true;
}

bool Estacionamento::registrarSaida(const string& placa) {
    DadosVeiculo dados;
    if (!banco.buscarVeiculo(placa, dados)) {
        cout << "Veiculo nao encontrado.\n";
        return false;
    }

    if (!banco.veiculoEstaEstacionado(placa)) {
        cout << "Este veiculo nao esta estacionado.\n";
        return false;
    }

    string entrada = banco.buscarEntrada(placa);
    string saida = horarioAtual();

    long long minutosEntrada = converterParaMinutos(entrada);
    long long minutosSaida = converterParaMinutos(saida);
    double minutos = static_cast<double>(minutosSaida - minutosEntrada);

    if (minutos < 0) {
        cout << "Erro ao calcular o tempo de permanencia.\n";
        return false;
    }

    unique_ptr<Veiculo> veiculo = criarVeiculo(dados);
    double valor = veiculo->calcularTarifa(minutos);
    int numeroVaga = 0;

    vector<Vaga>* vagas = dados.tipo == "Moto" ? &vagasMoto : &vagasCarro;
    for (Vaga& vaga : *vagas) {
        if (vaga.getPlacaVeiculo() == placa) {
            numeroVaga = vaga.getNumero();
            break;
        }
    }

    if (!banco.registrarSaida(placa, saida, valor)) {
        cout << "Nao foi possivel registrar a saida.\n";
        return false;
    }

    for (Vaga& vaga : *vagas) {
        if (vaga.getPlacaVeiculo() == placa) {
            vaga.liberar();
            break;
        }
    }

    cout << "Saida registrada com sucesso!\n";
    Ticket ticket(dados.placa, dados.tipo, dados.modelo, dados.cor,
                  numeroVaga, entrada);
    ticket.definirSaida(saida, minutos, valor);
    ticket.imprimirSaida();
    return true;
}

bool Estacionamento::editarVeiculo(const string& placa, const string& novoModelo,
                                   const string& novaCor) {
    DadosVeiculo dados;
    if (!banco.buscarVeiculo(placa, dados)) {
        cout << "Veiculo nao encontrado.\n";
        return false;
    }

    if (!banco.atualizarVeiculo(placa, novoModelo, novaCor)) {
        cout << "Nao foi possivel atualizar o veiculo.\n";
        return false;
    }

    cout << "Veiculo atualizado com sucesso!\n";
    return true;
}

bool Estacionamento::removerVeiculo(const string& placa) {
    DadosVeiculo dados;
    if (!banco.buscarVeiculo(placa, dados)) {
        cout << "Veiculo nao encontrado.\n";
        return false;
    }

    if (banco.veiculoEstaEstacionado(placa)) {
        cout << "Nao e possivel remover um veiculo que esta estacionado.\n";
        return false;
    }

    if (!banco.removerVeiculo(placa)) {
        cout << "Nao foi possivel remover o veiculo.\n";
        return false;
    }

    cout << "Veiculo removido com sucesso!\n";
    return true;
}

void Estacionamento::consultarVeiculo(const string& placa) {
    DadosVeiculo veiculo;

    if (!banco.buscarVeiculo(placa, veiculo)) {
        cout << "Veiculo nao encontrado.\n";
        return;
    }

    cout << "\n--- VEICULO ---\n";
    cout << "Placa: " << veiculo.placa << "\n";
    cout << "Tipo: " << veiculo.tipo << "\n";
    cout << "Modelo: " << veiculo.modelo << "\n";
    cout << "Cor: " << veiculo.cor << "\n";

    if (banco.veiculoEstaEstacionado(placa)) {
        cout << "Situacao: Estacionado\n";
        cout << "Entrada: " << banco.buscarEntrada(placa) << "\n";
    } else {
        cout << "Situacao: Nao esta no estacionamento\n";
    }
}

void Estacionamento::listarVeiculos() {
    vector<VeiculoEstacionado> veiculos = banco.listarVeiculosEstacionados();

    if (veiculos.empty()) {
        cout << "Nenhum veiculo esta atualmente no estacionamento.\n";
        return;
    }

    cout << "\n--- VEICULOS NO ESTACIONAMENTO ---\n";
    for (const VeiculoEstacionado& item : veiculos) {
        int numeroVaga = 0;

        const vector<Vaga>& vagas =
            item.veiculo.tipo == "Moto" ? vagasMoto : vagasCarro;

        for (const Vaga& vaga : vagas) {
            if (vaga.getPlacaVeiculo() == item.veiculo.placa) {
                numeroVaga = vaga.getNumero();
                break;
            }
        }

        cout << "Vaga: " << numeroVaga
                  << " | Placa: " << item.veiculo.placa
                  << " | Tipo: " << item.veiculo.tipo
                  << " | Modelo: " << item.veiculo.modelo
                  << " | Cor: " << item.veiculo.cor
                  << " | Entrada: " << item.horarioEntrada << "\n";
    }
}

void Estacionamento::mostrarVagas() const {
    int livresCarro = 0;
    int livresMoto = 0;

    for (const Vaga& vaga : vagasCarro) {
        if (!vaga.estaOcupada()) {
            livresCarro++;
        }
    }

    for (const Vaga& vaga : vagasMoto) {
        if (!vaga.estaOcupada()) {
            livresMoto++;
        }
    }

    cout << "\n--- VAGAS ---\n";
    cout << "Carro/Caminhonete: " << livresCarro << "/" << vagasCarro.size() << " livres\n";
    cout << "Moto: " << livresMoto << "/" << vagasMoto.size() << " livres\n";
}

void Estacionamento::mostrarHistorico() {
    vector<RegistroSaida> registros = banco.listarHistorico();

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

void Estacionamento::mostrarFaturamentoDoDia(const string& data) {
    double faturamento = banco.calcularFaturamentoDoDia(data);

    cout << "Faturamento de " << data << ": R$ "
              << fixed << setprecision(2) << faturamento << "\n";
}

void Estacionamento::configurarVagas(int quantidadeCarro, int quantidadeMoto) {
    if (quantidadeCarro < 0 || quantidadeMoto < 0) {
        cout << "A quantidade de vagas nao pode ser negativa.\n";
        return;
    }

    if (quantidadeCarro < static_cast<int>(vagasCarro.size())) {
        for (int i = quantidadeCarro; i < static_cast<int>(vagasCarro.size()); i++) {
            if (vagasCarro[i].estaOcupada()) {
                cout << "Nao e possivel reduzir as vagas de carro enquanto uma das vagas removidas estiver ocupada.\n";
                return;
            }
        }
        vagasCarro.erase(vagasCarro.begin() + quantidadeCarro, vagasCarro.end());
    } else {
        int atual = static_cast<int>(vagasCarro.size());
        for (int i = atual + 1; i <= quantidadeCarro; i++) {
            vagasCarro.emplace_back(i, "Carro");
        }
    }

    if (quantidadeMoto < static_cast<int>(vagasMoto.size())) {
        for (int i = quantidadeMoto; i < static_cast<int>(vagasMoto.size()); i++) {
            if (vagasMoto[i].estaOcupada()) {
                cout << "Nao e possivel reduzir as vagas de moto enquanto uma das vagas removidas estiver ocupada.\n";
                return;
            }
        }
        vagasMoto.erase(vagasMoto.begin() + quantidadeMoto, vagasMoto.end());
    } else {
        int atual = static_cast<int>(vagasMoto.size());
        for (int i = atual + 1; i <= quantidadeMoto; i++) {
            vagasMoto.emplace_back(i, "Moto");
        }
    }
}

void Estacionamento::configurarTaxas(double novaTaxaCarro, double novaTaxaMoto) {
    if (novaTaxaCarro < 0 || novaTaxaMoto < 0) {
        cout << "As taxas nao podem ser negativas.\n";
        return;
    }

    taxaCarro = novaTaxaCarro;
    taxaMoto = novaTaxaMoto;
}

double Estacionamento::getTaxaCarro() const { return taxaCarro; }
double Estacionamento::getTaxaMoto() const { return taxaMoto; }
int Estacionamento::getTotalVagasCarro() const { return static_cast<int>(vagasCarro.size()); }
int Estacionamento::getTotalVagasMoto() const { return static_cast<int>(vagasMoto.size()); }

