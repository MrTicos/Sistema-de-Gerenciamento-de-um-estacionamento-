#include "Estacionamento.h"
#include "Carro.h"
#include "Moto.h"
#include "Caminhonete.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

// ---------------------------------------------------------------------------
// Funções auxiliares (visíveis só neste arquivo)
// ---------------------------------------------------------------------------
namespace {

// Data/hora atual no formato usado no banco: AAAA-MM-DD HH:MM:SS
string horarioAtual() {
    auto agora = chrono::system_clock::now();
    time_t tempo = chrono::system_clock::to_time_t(agora);
    tm* tmAtual = localtime(&tempo);

    ostringstream saida;
    saida << put_time(tmAtual, "%Y-%m-%d %H:%M:%S");
    return saida.str();
}

// Converte "AAAA-MM-DD HH:MM:SS" em minutos desde 1970 (para calcular a permanência).
long long converterParaMinutos(const string& horario) {
    tm tmHorario = {};
    istringstream entrada(horario);
    entrada >> get_time(&tmHorario, "%Y-%m-%d %H:%M:%S");
    time_t tempo = mktime(&tmHorario);
    return static_cast<long long>(tempo / 60);
}

// Número da vaga que guarda a placa informada (0 se não encontrar).
int numeroDaVaga(const vector<Vaga>& vagas, const string& placa) {
    for (const Vaga& vaga : vagas) {
        if (vaga.getPlacaVeiculo() == placa) {
            return vaga.getNumero();
        }
    }
    return 0;
}

// Só é possível reduzir a quantidade de vagas se as vagas removidas estiverem livres.
bool podeReduzir(const vector<Vaga>& vagas, int novaQuantidade) {
    for (int i = novaQuantidade; i < static_cast<int>(vagas.size()); i++) {
        if (vagas[i].estaOcupada()) {
            return false;
        }
    }
    return true;
}

// Aumenta ou diminui a lista de vagas até chegar na quantidade desejada.
void ajustarVagas(vector<Vaga>& vagas, int novaQuantidade, const string& tipo) {
    if (novaQuantidade < static_cast<int>(vagas.size())) {
        vagas.erase(vagas.begin() + novaQuantidade, vagas.end());
    } else {
        for (int i = static_cast<int>(vagas.size()) + 1; i <= novaQuantidade; i++) {
            vagas.emplace_back(i, tipo);
        }
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Construção e métodos privados
// ---------------------------------------------------------------------------

// Cria as vagas e recupera do banco quem já estava estacionado.
Estacionamento::Estacionamento(Banco& banco, int quantidadeCarro, int quantidadeMoto)
    : banco(banco), taxaCarro(10.0), taxaMoto(5.0) {
    configurarVagas(quantidadeCarro, quantidadeMoto);
    restaurarVagas();
}

// Fábrica: o banco guarda o tipo como texto; aqui ele vira o objeto certo.
// Este é o único lugar que compara o nome do tipo. Depois disso, todo o resto do
// sistema só usa métodos virtuais de Veiculo (polimorfismo).
unique_ptr<Veiculo> Estacionamento::criarVeiculo(const DadosVeiculo& dados) const {
    if (dados.tipo == "Moto") {
        return make_unique<Moto>(dados.placa, dados.modelo, dados.cor, taxaMoto);
    }

    if (dados.tipo == "Caminhonete") {
        return make_unique<Caminhonete>(dados.placa, dados.modelo, dados.cor, taxaCarro);
    }

    return make_unique<Carro>(dados.placa, dados.modelo, dados.cor, taxaCarro);
}

// Quem decide o grupo de vagas é o PRÓPRIO veículo (podeUsarVagaMoto), sem if de texto.
vector<Vaga>& Estacionamento::vagasPara(const Veiculo& veiculo) {
    return veiculo.podeUsarVagaMoto() ? vagasMoto : vagasCarro;
}

const vector<Vaga>& Estacionamento::vagasPara(const Veiculo& veiculo) const {
    return veiculo.podeUsarVagaMoto() ? vagasMoto : vagasCarro;
}

// Devolve um PONTEIRO para a primeira vaga livre do grupo do veículo (nullptr se lotado).
Vaga* Estacionamento::encontrarVagaLivre(const Veiculo& veiculo) {
    for (Vaga& vaga : vagasPara(veiculo)) {
        if (!vaga.estaOcupada()) {
            return &vaga;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Restauração das vagas ao abrir o programa
// ---------------------------------------------------------------------------

// As vagas vivem na memória; ao reabrir o programa elas nascem todas livres.
// Este método percorre os veículos que o banco diz estarem estacionados e ocupa as vagas.
// Primeiro passo: quem tem número de vaga gravado volta para a MESMA vaga.
// Segundo passo: quem não tem (banco antigo) ou cuja vaga sumiu recebe a primeira livre.
void Estacionamento::restaurarVagas() {
    for (Vaga& vaga : vagasCarro) vaga.liberar();
    for (Vaga& vaga : vagasMoto) vaga.liberar();

    vector<VeiculoEstacionado> estacionados = banco.listarVeiculosEstacionados();
    vector<const VeiculoEstacionado*> semVaga;   // ponteiros para os que ficaram para o 2º passo

    for (const VeiculoEstacionado& item : estacionados) {
        unique_ptr<Veiculo> veiculo = criarVeiculo(item.veiculo);
        vector<Vaga>& vagas = vagasPara(*veiculo);

        bool numeroValido = item.vaga >= 1 && item.vaga <= static_cast<int>(vagas.size());
        if (numeroValido && !vagas[item.vaga - 1].estaOcupada()) {
            vagas[item.vaga - 1].ocupar(item.veiculo.placa);
        } else {
            semVaga.push_back(&item);
        }
    }

    for (const VeiculoEstacionado* item : semVaga) {
        unique_ptr<Veiculo> veiculo = criarVeiculo(item->veiculo);
        Vaga* vaga = encontrarVagaLivre(*veiculo);
        if (vaga != nullptr) {
            vaga->ocupar(item->veiculo.placa);
        }
    }
}

// ---------------------------------------------------------------------------
// Entrada e saída
// ---------------------------------------------------------------------------

Resultado Estacionamento::registrarEntrada(const string& placa, const string& modelo,
                                           const string& cor, const string& tipo,
                                           Ticket* ticket) {
    if (placa.empty()) {
        return {false, "Informe a placa do veiculo."};
    }

    if (banco.veiculoEstaEstacionado(placa)) {
        return {false, "Este veiculo ja esta estacionado."};
    }

    // A placa identifica o veículo: se já existe cadastro, ele tem prioridade.
    DadosVeiculo dados{placa, modelo, cor, tipo};
    DadosVeiculo cadastrado;
    bool jaCadastrado = banco.buscarVeiculo(placa, cadastrado);
    if (jaCadastrado) {
        dados = cadastrado;
    } else if (modelo.empty() || cor.empty()) {
        return {false, "Informe o modelo e a cor do veiculo."};
    }

    // O próprio objeto Veiculo diz em que grupo de vagas ele estaciona.
    unique_ptr<Veiculo> veiculo = criarVeiculo(dados);
    Vaga* vaga = encontrarVagaLivre(*veiculo);
    if (vaga == nullptr) {
        if (veiculo->podeUsarVagaMoto()) {
            return {false, "Todas as vagas para motos estao ocupadas."};
        }
        return {false, "Todas as vagas para carros e caminhonetes estao ocupadas."};
    }

    if (!jaCadastrado && !banco.cadastrarVeiculo(dados)) {
        return {false, "Nao foi possivel cadastrar o veiculo."};
    }

    vaga->ocupar(placa);
    string entrada = horarioAtual();

    // A vaga é gravada no banco para ser restaurada se o programa for reaberto.
    if (!banco.registrarEntrada(placa, entrada, vaga->getNumero())) {
        vaga->liberar();   // desfaz a ocupação se não conseguiu gravar
        return {false, "Nao foi possivel registrar a entrada."};
    }

    if (ticket != nullptr) {
        *ticket = Ticket(dados.placa, dados.tipo, dados.modelo, dados.cor,
                         vaga->getNumero(), entrada);
    }
    return {true, "Entrada registrada com sucesso!"};
}

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

    // Polimorfismo: a chamada é sempre a mesma, mas Carro, Moto e Caminhonete
    // calculam a tarifa cada um com a sua regra.
    unique_ptr<Veiculo> veiculo = criarVeiculo(dados);
    double valor = veiculo->calcularTarifa(minutos);

    vector<Vaga>& vagas = vagasPara(*veiculo);
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

// ---------------------------------------------------------------------------
// Gerenciamento de veículos (Update e Delete do CRUD)
// ---------------------------------------------------------------------------

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
    return {true, "Veiculo removido com sucesso! O historico de saidas foi mantido."};
}

// ---------------------------------------------------------------------------
// Consultas
// ---------------------------------------------------------------------------

bool Estacionamento::consultarVeiculo(const string& placa, InfoVeiculo& info) {
    if (!banco.buscarVeiculo(placa, info.dados)) {
        return false;
    }

    info.estacionado = banco.veiculoEstaEstacionado(placa);
    info.horarioEntrada = info.estacionado ? banco.buscarEntrada(placa) : "";
    return true;
}

vector<VeiculoEstacionado> Estacionamento::listarVeiculos() {
    vector<VeiculoEstacionado> veiculos = banco.listarVeiculosEstacionados();

    // A vaga real é a que está ocupada em memória (ela já foi restaurada do banco).
    for (VeiculoEstacionado& item : veiculos) {
        unique_ptr<Veiculo> veiculo = criarVeiculo(item.veiculo);
        item.vaga = numeroDaVaga(vagasPara(*veiculo), item.veiculo.placa);
    }
    return veiculos;
}

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

vector<RegistroSaida> Estacionamento::historico() {
    return banco.listarHistorico();
}

double Estacionamento::faturamentoDoDia(const string& data) {
    return banco.calcularFaturamentoDoDia(data);
}

// ---------------------------------------------------------------------------
// Configurações
// ---------------------------------------------------------------------------

Resultado Estacionamento::configurarVagas(int quantidadeCarro, int quantidadeMoto) {
    if (quantidadeCarro < 0 || quantidadeMoto < 0) {
        return {false, "A quantidade de vagas nao pode ser negativa."};
    }

    // Valida as duas reduções ANTES de alterar qualquer coisa (tudo ou nada).
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

Resultado Estacionamento::configurarTaxas(double novaTaxaCarro, double novaTaxaMoto) {
    if (novaTaxaCarro < 0 || novaTaxaMoto < 0) {
        return {false, "As taxas nao podem ser negativas."};
    }

    taxaCarro = novaTaxaCarro;
    taxaMoto = novaTaxaMoto;
    return {true, "Tarifas atualizadas."};
}

double Estacionamento::getTaxaCarro() const { return taxaCarro; }
double Estacionamento::getTaxaMoto() const { return taxaMoto; }
int Estacionamento::getTotalVagasCarro() const { return static_cast<int>(vagasCarro.size()); }
int Estacionamento::getTotalVagasMoto() const { return static_cast<int>(vagasMoto.size()); }
const vector<Vaga>& Estacionamento::getVagasCarro() const { return vagasCarro; }
const vector<Vaga>& Estacionamento::getVagasMoto() const { return vagasMoto; }
