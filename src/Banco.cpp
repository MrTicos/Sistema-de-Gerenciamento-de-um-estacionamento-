#include "Banco.h"
#include <sqlite3.h>
#include <iostream>
using namespace std;

// ---------------------------------------------------------------------------
// Conexão
// ---------------------------------------------------------------------------

// Abre (ou cria) o arquivo do banco. A conexão é guardada como void* para que
// o header não precise conhecer o SQLite.
Banco::Banco(const string& nomeArquivo) : banco(nullptr) {
    sqlite3* conexao = nullptr;

    if (sqlite3_open(nomeArquivo.c_str(), &conexao) != SQLITE_OK) {
        cerr << "Erro ao abrir o banco de dados.\n";
        if (conexao != nullptr) {
            sqlite3_close(conexao);
        }
        return;
    }

    banco = conexao;
}

// Fecha a conexão ao destruir o objeto.
Banco::~Banco() {
    if (banco != nullptr) {
        sqlite3_close(static_cast<sqlite3*>(banco));
    }
}

// Executa um comando SQL simples (sem parâmetros). Devolve false e mostra o erro se falhar.
bool Banco::executar(const string& sql) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    char* mensagemErro = nullptr;

    if (sqlite3_exec(conexao, sql.c_str(), nullptr, nullptr, &mensagemErro) != SQLITE_OK) {
        cerr << "Erro no banco: " << (mensagemErro != nullptr ? mensagemErro : "desconhecido") << "\n";
        sqlite3_free(mensagemErro);
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Criação e atualização das tabelas
// ---------------------------------------------------------------------------

// Cria as tabelas se não existirem.
//  - veiculos: cadastro (a placa é a chave primária).
//  - estacionamentos: cada permanência (entrada, vaga, saída e valor pago).
// A placa em "estacionamentos" é guardada como texto, SEM chave estrangeira:
// assim o histórico é preservado mesmo quando o cadastro do veículo é removido.
bool Banco::criarTabelas() {
    const string sql =
        "CREATE TABLE IF NOT EXISTS veiculos ("
        "placa TEXT PRIMARY KEY,"
        "modelo TEXT NOT NULL,"
        "cor TEXT NOT NULL,"
        "tipo TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS estacionamentos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "placa TEXT NOT NULL,"
        "vaga INTEGER,"
        "horario_entrada TEXT NOT NULL,"
        "horario_saida TEXT,"
        "valor_pago REAL DEFAULT 0"
        ");";

    if (!executar(sql)) {
        return false;
    }

    // Bancos criados por versões antigas do programa precisam ser convertidos.
    if (historicoPrecisaMigrar()) {
        return migrarHistorico();
    }
    return true;
}

// Verifica se a tabela "estacionamentos" está no formato antigo:
// com chave estrangeira (impedia remover veículo com histórico) ou sem a coluna "vaga".
bool Banco::historicoPrecisaMigrar() {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    sqlite3_stmt* stmt = nullptr;
    bool temChaveEstrangeira = false;
    bool temColunaVaga = false;

    // Uma linha para cada chave estrangeira da tabela.
    if (sqlite3_prepare_v2(conexao, "PRAGMA foreign_key_list(estacionamentos);", -1, &stmt, nullptr) == SQLITE_OK) {
        temChaveEstrangeira = sqlite3_step(stmt) == SQLITE_ROW;
    }
    sqlite3_finalize(stmt);

    // Uma linha para cada coluna (a coluna 1 do resultado é o nome).
    stmt = nullptr;
    if (sqlite3_prepare_v2(conexao, "PRAGMA table_info(estacionamentos);", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string nome = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            if (nome == "vaga") {
                temColunaVaga = true;
            }
        }
    }
    sqlite3_finalize(stmt);

    return temChaveEstrangeira || !temColunaVaga;
}

// Recria a tabela "estacionamentos" no formato atual, copiando todo o histórico.
// Tudo acontece numa transação: se algo falhar, nada é alterado.
bool Banco::migrarHistorico() {
    const string sql =
        "BEGIN;"
        "CREATE TABLE estacionamentos_nova ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "placa TEXT NOT NULL,"
        "vaga INTEGER,"
        "horario_entrada TEXT NOT NULL,"
        "horario_saida TEXT,"
        "valor_pago REAL DEFAULT 0"
        ");"
        "INSERT INTO estacionamentos_nova (id, placa, horario_entrada, horario_saida, valor_pago) "
        "SELECT id, placa, horario_entrada, horario_saida, valor_pago FROM estacionamentos;"
        "DROP TABLE estacionamentos;"
        "ALTER TABLE estacionamentos_nova RENAME TO estacionamentos;"
        "COMMIT;";

    if (executar(sql)) {
        return true;
    }

    executar("ROLLBACK;");   // desfaz o que tiver sido feito antes do erro
    return false;
}

// ---------------------------------------------------------------------------
// CREATE
// ---------------------------------------------------------------------------

// Insere um veículo novo. Falha se a placa já existir (chave primária).
bool Banco::cadastrarVeiculo(const DadosVeiculo& veiculo) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql = "INSERT INTO veiculos (placa, modelo, cor, tipo) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    // Os "?" são preenchidos pelo bind: o texto nunca vira parte do comando SQL.
    sqlite3_bind_text(stmt, 1, veiculo.placa.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, veiculo.modelo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, veiculo.cor.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, veiculo.tipo.c_str(), -1, SQLITE_TRANSIENT);

    bool sucesso = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return sucesso;
}

// Abre uma permanência: placa, vaga e horário de entrada (saída fica vazia).
bool Banco::registrarEntrada(const string& placa, const string& horarioEntrada, int vaga) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql = "INSERT INTO estacionamentos (placa, vaga, horario_entrada) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, placa.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, vaga);
    sqlite3_bind_text(stmt, 3, horarioEntrada.c_str(), -1, SQLITE_TRANSIENT);

    bool sucesso = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return sucesso;
}

// ---------------------------------------------------------------------------
// READ
// ---------------------------------------------------------------------------

// Procura o cadastro pela placa. Preenche "veiculo" e devolve true se encontrar.
bool Banco::buscarVeiculo(const string& placa, DadosVeiculo& veiculo) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql = "SELECT placa, modelo, cor, tipo FROM veiculos WHERE placa = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, placa.c_str(), -1, SQLITE_TRANSIENT);
    bool encontrou = false;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        veiculo.placa = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        veiculo.modelo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        veiculo.cor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        veiculo.tipo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        encontrou = true;
    }

    sqlite3_finalize(stmt);
    return encontrou;
}

// Lista quem está no estacionamento agora (permanências sem horário de saída),
// juntando com o cadastro para trazer modelo, cor e tipo. Vaga 0 = banco antigo, sem vaga gravada.
vector<VeiculoEstacionado> Banco::listarVeiculosEstacionados() {
    vector<VeiculoEstacionado> veiculos;
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql =
        "SELECT v.placa, v.modelo, v.cor, v.tipo, e.horario_entrada, COALESCE(e.vaga, 0) "
        "FROM veiculos v "
        "INNER JOIN estacionamentos e ON v.placa = e.placa "
        "WHERE e.horario_saida IS NULL "
        "ORDER BY v.placa;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return veiculos;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        VeiculoEstacionado item;
        item.veiculo.placa = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        item.veiculo.modelo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        item.veiculo.cor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        item.veiculo.tipo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        item.horarioEntrada = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        item.vaga = sqlite3_column_int(stmt, 5);
        veiculos.push_back(item);
    }

    sqlite3_finalize(stmt);
    return veiculos;
}

// True se existe uma permanência dessa placa ainda sem horário de saída.
bool Banco::veiculoEstaEstacionado(const string& placa) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql = "SELECT COUNT(*) FROM estacionamentos WHERE placa = ? AND horario_saida IS NULL;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, placa.c_str(), -1, SQLITE_TRANSIENT);
    bool estacionado = false;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        estacionado = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return estacionado;
}

// Devolve o horário de entrada da permanência em andamento (texto vazio se não houver).
string Banco::buscarEntrada(const string& placa) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql = "SELECT horario_entrada FROM estacionamentos WHERE placa = ? AND horario_saida IS NULL LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;
    string entrada;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return entrada;
    }

    sqlite3_bind_text(stmt, 1, placa.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        entrada = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return entrada;
}

// Todas as permanências já encerradas, da mais recente para a mais antiga.
vector<RegistroSaida> Banco::listarHistorico() {
    vector<RegistroSaida> registros;
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql =
        "SELECT id, placa, horario_entrada, horario_saida, valor_pago "
        "FROM estacionamentos WHERE horario_saida IS NOT NULL ORDER BY id DESC;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return registros;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        RegistroSaida registro;
        registro.id = sqlite3_column_int(stmt, 0);
        registro.placa = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        registro.horarioEntrada = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        registro.horarioSaida = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        registro.valorPago = sqlite3_column_double(stmt, 4);
        registros.push_back(registro);
    }

    sqlite3_finalize(stmt);
    return registros;
}

// Soma o valor pago nas saídas do dia (os 10 primeiros caracteres da data são AAAA-MM-DD).
double Banco::calcularFaturamentoDoDia(const string& data) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql =
        "SELECT COALESCE(SUM(valor_pago), 0) FROM estacionamentos "
        "WHERE horario_saida IS NOT NULL AND substr(horario_saida, 1, 10) = ?;";
    sqlite3_stmt* stmt = nullptr;
    double faturamento = 0.0;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return faturamento;
    }

    sqlite3_bind_text(stmt, 1, data.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        faturamento = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return faturamento;
}

// ---------------------------------------------------------------------------
// UPDATE
// ---------------------------------------------------------------------------

// Fecha a permanência em andamento, gravando horário de saída e valor pago.
// Só devolve true se havia mesmo uma permanência aberta (sqlite3_changes > 0).
bool Banco::registrarSaida(const string& placa, const string& horarioSaida, double valorPago) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql =
        "UPDATE estacionamentos SET horario_saida = ?, valor_pago = ? "
        "WHERE placa = ? AND horario_saida IS NULL;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, horarioSaida.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, valorPago);
    sqlite3_bind_text(stmt, 3, placa.c_str(), -1, SQLITE_TRANSIENT);

    bool sucesso = sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(conexao) > 0;
    sqlite3_finalize(stmt);
    return sucesso;
}

// Troca modelo e cor de um veículo cadastrado (a placa identifica qual).
bool Banco::atualizarVeiculo(const string& placa, const string& modelo, const string& cor) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE veiculos SET modelo = ?, cor = ? WHERE placa = ?;";

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, modelo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, cor.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, placa.c_str(), -1, SQLITE_TRANSIENT);

    bool sucesso = sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(conexao) > 0;
    sqlite3_finalize(stmt);
    return sucesso;
}

// ---------------------------------------------------------------------------
// DELETE
// ---------------------------------------------------------------------------

// Remove só o CADASTRO. Veículo estacionado não pode ser removido.
// O histórico de saídas fica intacto (a placa lá é só texto, sem vínculo com o cadastro).
bool Banco::removerVeiculo(const string& placa) {
    if (veiculoEstaEstacionado(placa)) {
        return false;
    }

    sqlite3* conexao = static_cast<sqlite3*>(banco);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM veiculos WHERE placa = ?;";

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, placa.c_str(), -1, SQLITE_TRANSIENT);

    bool sucesso = sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(conexao) > 0;
    sqlite3_finalize(stmt);
    return sucesso;
}
