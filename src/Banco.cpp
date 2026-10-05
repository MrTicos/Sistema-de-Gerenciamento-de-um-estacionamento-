#include "Banco.h"
#include <sqlite3.h>
#include <iostream>
using namespace std;

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

Banco::~Banco() {
    if (banco != nullptr) {
        sqlite3_close(static_cast<sqlite3*>(banco));
    }
}

bool Banco::executar(const string& sql) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    char* mensagemErro = nullptr;

    if (sqlite3_exec(conexao, sql.c_str(), nullptr, nullptr, &mensagemErro) != SQLITE_OK) {
        cerr << "Erro no banco: " << mensagemErro << "\n";
        sqlite3_free(mensagemErro);
        return false;
    }

    return true;
}

bool Banco::criarTabelas() {
    const string sql =
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS veiculos ("
        "placa TEXT PRIMARY KEY,"
        "modelo TEXT NOT NULL,"
        "cor TEXT NOT NULL,"
        "tipo TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS estacionamentos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "placa TEXT NOT NULL,"
        "horario_entrada TEXT NOT NULL,"
        "horario_saida TEXT,"
        "valor_pago REAL DEFAULT 0,"
        "FOREIGN KEY (placa) REFERENCES veiculos(placa)"
        ");";

    return executar(sql);
}

bool Banco::cadastrarVeiculo(const DadosVeiculo& veiculo) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql = "INSERT INTO veiculos (placa, modelo, cor, tipo) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, veiculo.placa.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, veiculo.modelo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, veiculo.cor.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, veiculo.tipo.c_str(), -1, SQLITE_TRANSIENT);

    bool sucesso = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return sucesso;
}

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

vector<VeiculoEstacionado> Banco::listarVeiculosEstacionados() {
    vector<VeiculoEstacionado> veiculos;
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql =
        "SELECT v.placa, v.modelo, v.cor, v.tipo, e.horario_entrada "
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
        item.vaga = 0;
        veiculos.push_back(item);
    }

    sqlite3_finalize(stmt);
    return veiculos;
}

bool Banco::registrarEntrada(const string& placa, const string& horarioEntrada) {
    sqlite3* conexao = static_cast<sqlite3*>(banco);
    const char* sql = "INSERT INTO estacionamentos (placa, horario_entrada) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(conexao, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, placa.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, horarioEntrada.c_str(), -1, SQLITE_TRANSIENT);

    bool sucesso = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return sucesso;
}

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

bool Banco::atualizarVeiculo(const std::string& placa, const std::string& modelo, const std::string& cor) {
    sqlite3* db = static_cast<sqlite3*>(banco);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE veiculos SET modelo = ?, cor = ? WHERE placa = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, modelo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, cor.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, placa.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0;
    sqlite3_finalize(stmt);
    return ok;
}

bool Banco::removerVeiculo(const std::string& placa) {
    if (veiculoEstaEstacionado(placa)) return false;
    sqlite3* db = static_cast<sqlite3*>(banco);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM veiculos WHERE placa = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, placa.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0;
    sqlite3_finalize(stmt);
    return ok;
}
