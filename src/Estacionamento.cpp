#include "Estacionamento.h"
#include "Carro.h"
#include "Moto.h"
#include "Caminhonete.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

// Namespace anônimo: funções utilitárias internas para manipulação de datas, horas e vetores de vagas.
namespace {

// Retorna a data e hora atual do sistema formatada como "AAAA-MM-DD HH:MM:SS".
string horarioAtual() {
    auto agora = chrono::system_clock::now();
    time_t tempo = chrono::system_clock::to_time_t(agora);
    tm* tmAtual = localtime(&tempo);

    ostringstream saida;
    saida << put_time(tmAtual, "%Y-%m-%d %H:%M:%S");
    return saida.str();
}

// Converte uma string de data/hora no padrão "AAAA-MM-DD HH:MM:SS" em total de minutos desde Epoch.
long long converterParaMinutos(const string& horario) {
    tm tmHorario = {};
    istringstream entrada(horario);
    entrada >> get_time(&tmHorario, "%Y-%m-%d %H:%M:%S");
    time_t tempo = mktime(&tmHorario);
    return static_cast<long long>(tempo / 60);
}

// Procura e retorna o número da vaga ocupada por determinado veículo. Retorna 0 se não encontrar.
int numeroDaVaga(const vector<Vaga>& vagas, const string& placa) {
    for (const Vaga& vaga : vagas) {
        if (vaga.getPlacaVeiculo() == placa) {
            return vaga.getNumero();
        }
    }
    return 0;
}

// Valida se as vagas excedentes podem ser removidas sem afetar veículos estacionados.
bool podeReduzir(const vector<Vaga>& vagas, int novaQuantidade) {
    for (int i = novaQuantidade; i < static_cast<int>(vagas.size()); i++) {
        if (vagas[i].estaOcupada()) {
            return false;
        }
    }
    return true;
}

// Redimensiona o vetor de vagas, adicionando novas vagas ou removendo as excedentes.
void ajustarVagas(vector<Vaga>& vagas, int novaQuantidade, const string& tipo) {
    if (novaQuantidade < static_cast<int>(vagas.size())) {
        vagas.erase(vagas.begin() + novaQuantidade, vagas.end());
    } else {
        for (int i = static_cast<int>(vagas.size()) + 1; i <= novaQuantidade; i++) {
            vagas.emplace_back(i, tipo);
        }
    }
}
} // Fim do namespace anônimo

// Construtor: inicializa a referência do banco, define as taxas padrão e cria as vagas iniciais.
Estacionamento::Estacionamento(Banco& banco, int quantidadeCarro, int quantidadeMoto)
    : banco(banco), taxaCarro(10.0), taxaMoto(5.0) {
    configurarVagas(quantidadeCarro, quantidadeMoto);
    restaurarVagas();    
}

// Percorre a lista de vagas apropriada para o tipo de veículo e retorna a primeira vaga disponível.
Vaga* Estacionamento::encontrarVagaLivre(const string& tipo) {
    vector<Vaga>& vagas = (tipo == "Moto") ? vagasMoto : vagasCarro;
    for (Vaga& vaga : vagas) {
        if (!vaga.estaOcupada()) {
            return &vaga;
        }
    }
    return nullptr;
}

// Instancia dinamicamente o tipo correto de veículo (Moto, Caminhonete ou Carro) usando ponteiro inteligente.
unique_ptr<Veiculo> Estacionamento::criarVeiculo(const DadosVeiculo& dados) {
    if (dados.tipo == "Moto") {
        return make_unique<Moto>(dados.placa, dados.modelo, dados.cor, taxaMoto);
    }

    if (dados.tipo == "Caminhonete") {
        return make_unique<Caminhonete>(dados.placa, dados.modelo, dados.cor, taxaCarro);
    }

    return make_unique<Carro>(dados.placa, dados.modelo, dados.cor, taxaCarro);
}

// Libera todas as vagas em memória e reocupa apenas as vagas dos veículos registrados como estacionados no banco.
void Estacionamento::restaurarVagas() {
    for (Vaga& vaga : vagasCarro) vaga.liberar();
    for (Vaga& vaga : vagasMoto) vaga.liberar();

    for (const VeiculoEstacionado& item : banco.listarVeiculosEstacionados()) {
        Vaga* vaga = encontrarVagaLivre(item.veiculo.tipo);
        if (vaga != nullptr) {
            vaga->ocupar(item.veiculo.placa);
        }
    }
}

// Registra a entrada de um veículo: valida cadastro/vagas, ocupa a vaga, grava no banco e gera o ticket.
Resultado Estacionamento::registrarEntrada(const string& placa, const string& modelo,
                                           const string& cor, const string& tipo,
                                           Ticket* ticket) {
    if (placa.empty()) {
        return {false, "Informe a placa do veiculo."};
    }

    if (banco.veiculoEstaEstacionado(placa)) {
        return {false, "Este veiculo ja esta estacionado."};
    }

    DadosVeiculo dados{placa, modelo, cor, tipo};
    DadosVeiculo cadastrado;
    bool jaCadastrado = banco.buscarVeiculo(placa, cadastrado);
    if (jaCadastrado) {
        dados = cadastrado;
    } else if (modelo.empty() || cor.empty()) {
        return {false, "Informe o modelo e a cor do veiculo."};
    }

    Vaga* vaga = encontrarVagaLivre(dados.tipo);
    if (vaga == nullptr) {
        if (dados.tipo == "Moto") {
            return {false, "Todas as vagas para motos estao ocupadas."};
        }
        return {false, "Todas as vagas para carros e caminhonetes estao ocupadas."};
    }

    if (!jaCadastrado && !banco.cadastrarVeiculo(dados)) {
        return {false, "Nao foi possivel cadastrar o veiculo."};
    }

    vaga->ocupar(placa);
    string entrada = horarioAtual();

    if (!banco.registrarEntrada(placa, entrada)) {
        vaga->liberar();
        return {false, "Nao foi possivel registrar a entrada."};
    }

    if (ticket != nullptr) {
        *ticket = Ticket(dados.placa, dados.tipo, dados.modelo, dados.cor,
                         vaga->getNumero(), entrada);
    }
    return {true, "Entrada registrada com sucesso!"};
}

// Registra a saída: calcula o tempo e a tarifa via polimorfismo, atualiza o banco, libera a vaga e gera o ticket de saída.
Resultado Estacionamento::registrarSaida(const string& placa, Ticket* ticket) {
    DadosVeiculo dados;
    if (!banco.buscarVeiculo(placa, dados)) {
        return {false, "Veiculo nao encontrado."};
    }

    if (!banco.veiculoEstaEstacionado(placa)) {
        return {false, "Este veiculo nao esta estacionado."};
    }

    string entrada = banco.buscarEntrada(placa);
    string saida = horarioAtual();

    double minutos = static_cast<double>(converterParaMinutos(saida) - converterParaMinutos(entrada));
    if (minutos < 0) {
        return {false, "Erro ao calcular o tempo de permanencia."};
    }

    unique_ptr<Veiculo> veiculo = criarVeiculo(dados);
    double valor = veiculo->calcularTarifa(minutos);

    vector<Vaga>& vagas = (dados.tipo == "Moto") ? vagasMoto : vagasCarro;
    int numeroVaga = numeroDaVaga(vagas, placa);

    if (!banco.registrarSaida(placa, saida, valor)) {
        return {false, "Nao foi possivel registrar a saida."};
    }

    for (Vaga& vaga : vagas) {
        if (vaga.getPlacaVeiculo() == placa) {
            vaga.liberar();
            break;
        }
    }

    if (ticket != nullptr) {
        *ticket = Ticket(dados.placa, dados.tipo, dados.modelo, dados.cor, numeroVaga, entrada);
        ticket->definirSaida(saida, minutos, valor);
    }
    return {true, "Saida registrada com sucesso!"};
}

// Atualiza informações de modelo e cor de um veículo cadastrado no banco.
Resultado Estacionamento::editarVeiculo(const string& placa, const string& novoModelo,
                                        const string& novaCor) {
    DadosVeiculo dados;
    if (!banco.buscarVeiculo(placa, dados)) {
        return {false, "Veiculo nao encontrado."};
    }

    if (novoModelo.empty() || novaCor.empty()) {
        return {false, "Informe o modelo e a cor do veiculo."};
    }

    if (!banco.atualizarVeiculo(placa, novoModelo, novaCor)) {
        return {false, "Nao foi possivel atualizar o veiculo."};
    }
    return {true, "Veiculo atualizado com sucesso!"};
}

// Remove o cadastro de um veículo do banco de dados, desde que não esteja estacionado.
Resultado Estacionamento::removerVeiculo(const string& placa) {
    DadosVeiculo dados;
    if (!banco.buscarVeiculo(placa, dados)) {
        return {false, "Veiculo nao encontrado."};
    }

    if (banco.veiculoEstaEstacionado(placa)) {
        return {false, "Nao e possivel remover um veiculo que esta estacionado."};
    }

    if (!banco.removerVeiculo(placa)) {
        return {false, "Nao foi possivel remover o veiculo."};
    }
    return {true, "Veiculo removido com sucesso!"};
}

// Consulta dados cadastrais e o status atual de ocupação do veículo.
bool Estacionamento::consultarVeiculo(const string& placa, InfoVeiculo& info) {
    if (!banco.buscarVeiculo(placa, info.dados)) {
        return false;
    }

    info.estacionado = banco.veiculoEstaEstacionado(placa);
    info.horarioEntrada = info.estacionado ? banco.buscarEntrada(placa) : "";
    return true;
}

// Retorna a lista de veículos estacionados mapeando cada um ao número de vaga correspondente.
vector<VeiculoEstacionado> Estacionamento::listarVeiculos() {
    vector<VeiculoEstacionado> veiculos = banco.listarVeiculosEstacionados();

    for (VeiculoEstacionado& item : veiculos) {
        const vector<Vaga>& vagas = (item.veiculo.tipo == "Moto") ? vagasMoto : vagasCarro;
        item.vaga = numeroDaVaga(vagas, item.veiculo.placa);
    }
    return veiculos;
}

// Calcula o total de vagas ocupadas e disponíveis por categoria no momento.
ResumoVagas Estacionamento::resumoVagas() const {
    ResumoVagas resumo{0, static_cast<int>(vagasCarro.size()),
                       0, static_cast<int>(vagasMoto.size())};

    for (const Vaga& vaga : vagasCarro) {
        if (!vaga.estaOcupada()) resumo.livresCarro++;
    }
    for (const Vaga& vaga : vagasMoto) {
        if (!vaga.estaOcupada()) resumo.livresMoto++;
    }
    return resumo;
}

// Retorna o histórico completo de saídas registradas no banco.
vector<RegistroSaida> Estacionamento::historico() {
    return banco.listarHistorico();
}

// Retorna a soma de faturamento em reais para uma determinada data.
double Estacionamento::faturamentoDoDia(const string& data) {
    return banco.calcularFaturamentoDoDia(data);
}

// Altera a quantidade de vagas operacionais do estacionamento após validar se as vagas excedentes estão livres.
Resultado Estacionamento::configurarVagas(int quantidadeCarro, int quantidadeMoto) {
    if (quantidadeCarro < 0 || quantidadeMoto < 0) {
        return {false, "A quantidade de vagas nao pode ser negativa."};
    }

    if (!podeReduzir(vagasCarro, quantidadeCarro)) {
        return {false, "Nao e possivel reduzir as vagas de carro enquanto uma das vagas removidas estiver ocupada."};
    }
    if (!podeReduzir(vagasMoto, quantidadeMoto)) {
        return {false, "Nao e possivel reduzir as vagas de moto enquanto uma das vagas removidas estiver ocupada."};
    }

    ajustarVagas(vagasCarro, quantidadeCarro, "Carro");
    ajustarVagas(vagasMoto, quantidadeMoto, "Moto");
    return {true, "Quantidade de vagas atualizada."};
}

// Atualiza as tarifas por hora cobradas para carros e motos.
Resultado Estacionamento::configurarTaxas(double novaTaxaCarro, double novaTaxaMoto) {
    if (novaTaxaCarro < 0 || novaTaxaMoto < 0) {
        return {false, "As taxas nao podem ser negativas."};
    }

    taxaCarro = novaTaxaCarro;
    taxaMoto = novaTaxaMoto;
    return {true, "Tarifas atualizadas."};
}

// Métodos Getters para consultar taxas e coleções de vagas:
double Estacionamento::getTaxaCarro() const { return taxaCarro; }
double Estacionamento::getTaxaMoto() const { return taxaMoto; }
int Estacionamento::getTotalVagasCarro() const { return static_cast<int>(vagasCarro.size()); }
int Estacionamento::getTotalVagasMoto() const { return static_cast<int>(vagasMoto.size()); }
const vector<Vaga>& Estacionamento::getVagasCarro() const { return vagasCarro; }
const vector<Vaga>& Estacionamento::getVagasMoto() const { return vagasMoto; }
