# Relatório do Sistema de Gerenciamento de Estacionamento

- **Disciplina:** Estruturas de Dados Orientadas a Objetos (CIN0135) 
- **Instituição:** UFPE
- **Professor:** Francisco Paulo Magalhães Simões
- **Equipe:** Thiago Silva, Gabriel Freitas, Miguel Nascimento
- **Repositório:** https://github.com/MrTicos/Sistema-de-Gerenciamento-de-um-estacionamento-

---

## 1. Introdução

Este relatório descreve o sistema de gerenciamento de estacionamento desenvolvido em **C++17** com **Programação Orientada a Objetos (POO)** e persistência em **SQLite**. O trabalho atende à opção 1 do projeto prático da disciplina: construir um sistema de informação com conceitos de POO, com diagrama de classes, código e CRUD conectado a um banco de dados.

## 2. Descrição do problema

Um estacionamento precisa saber quais vagas estão livres, quem entrou e quando, quanto cada veículo deve pagar ao sair e quanto foi arrecadado. Feito manualmente, esse controle gera erros de cobrança e perda de histórico. O sistema automatiza esse fluxo em uma aplicação de terminal, operada por um atendente.

### Escopo

- A **placa é digitada manualmente** (não há câmera nem reconhecimento automático).
- Os **horários de entrada e saída são registrados automaticamente** pelo sistema.
- Existem dois tipos de vaga (carro e moto) e três tipos de veículo (carro, moto e caminhonete, no sentido de picape/utilitário, não caminhão pesado).
- A tarifa varia apenas pelo tipo de veículo e é cobrada proporcionalmente ao tempo, em minutos.

## 3. Requisitos

### 3.1 Requisitos funcionais

| Código | Requisito |
| ------ | --------- |
| RF01 | Registrar a entrada de um veículo, cadastrando-o se for novo e alocando uma vaga compatível. |
| RF02 | Registrar a saída, calculando o tempo de permanência e o valor a pagar, e liberando a vaga. |
| RF03 | Emitir ticket de entrada e ticket de saída. |
| RF04 | Consultar a disponibilidade de vagas por tipo. |
| RF05 | Consultar um veículo pela placa. |
| RF06 | Listar os veículos atualmente estacionados. |
| RF07 | Consultar o histórico de saídas. |
| RF08 | Consultar o faturamento de uma data. |
| RF09 | Alterar a quantidade de vagas e as tarifas durante a execução. |
| RF10 | Editar os dados (modelo e cor) de um veículo cadastrado. |
| RF11 | Remover o cadastro de um veículo que não esteja estacionado. |

### 3.2 Requisitos não funcionais

- Linguagem **C++17**, com código comentado e organizado em classes.
- Persistência em **SQLite** (arquivo `estacionamento.db`, criado automaticamente).
- Compilação com **CMake 3.20+**, em Windows e Linux.
- Interface de terminal por menus.

### 3.3 Regras de negócio

| Regra | Descrição |
| ----- | --------- |
| RN01 | A placa identifica unicamente o veículo. |
| RN02 | Moto ocupa vaga de moto; carro e caminhonete ocupam vaga de carro. |
| RN03 | Vaga de moto é exclusiva para motos. |
| RN04 | Carro e caminhonete usam a mesma tarifa; moto tem tarifa própria. |
| RN05 | Um veículo já estacionado não pode registrar nova entrada. |
| RN06 | Um veículo estacionado não pode ter o cadastro removido. |
| RN07 | O histórico de estadias é preservado mesmo que o cadastro do veículo seja removido. |

## 4. Modelagem

### 4.1 Classes

| Classe | Responsabilidade |
| ------ | ---------------- |
| `Veiculo` | Classe abstrata base; guarda placa, modelo e cor e declara `calcularTarifa`. |
| `Carro`, `Moto`, `Caminhonete` | Especializações de `Veiculo`, cada uma com sua regra de tarifa e de vaga. |
| `Vaga` | Representa uma vaga (número, tipo, ocupação, placa do ocupante). |
| `Estacionamento` | Coordena as operações do sistema e mantém as vagas e tarifas. |
| `Ticket` | Reúne e imprime os dados dos comprovantes de entrada e saída. |
| `Banco` | Única classe que acessa o SQLite; implementa o CRUD. |

### 4.2 Diagrama de classes

```mermaid
classDiagram
    Veiculo <|-- Carro
    Veiculo <|-- Moto
    Veiculo <|-- Caminhonete
    Estacionamento --> Banco
    Estacionamento *-- Vaga
    Estacionamento ..> Veiculo
    Estacionamento ..> Ticket

    class Veiculo {
        <<abstract>>
        #string placa
        #string modelo
        #string cor
        +getTipo()
        +calcularTarifa(double minutos)*
        +podeUsarVagaMoto()
    }
    class Vaga {
        -int numero
        -string tipo
        -bool ocupada
        +ocupar()
        +liberar()
    }
    class Ticket {
        -string placa
        -double minutos
        -double valor
        +imprimirEntrada()
        +imprimirSaida()
    }
    class Estacionamento {
        -Banco& banco
        -vector~Vaga~ vagasCarro
        -vector~Vaga~ vagasMoto
        +registrarEntrada()
        +registrarSaida()
        +editarVeiculo()
        +removerVeiculo()
    }
    class Banco {
        -void* banco
        +cadastrarVeiculo()
        +buscarVeiculo()
        +atualizarVeiculo()
        +removerVeiculo()
        +registrarEntrada()
        +registrarSaida()
    }
```

### 4.3 Banco de dados

```mermaid
erDiagram
    veiculos ||--o{ estacionamentos : "tem estadias"
    veiculos {
        TEXT placa PK
        TEXT modelo
        TEXT cor
        TEXT tipo
    }
    estacionamentos {
        INTEGER id PK
        TEXT placa
        TEXT horario_entrada
        TEXT horario_saida
        REAL valor_pago
    }
```

Cada entrada cria um registro em `estacionamentos`. Na saída, o mesmo registro recebe `horario_saida` e `valor_pago`.

### 4.4 CRUD

| Operação | Funcionalidade | Tabela |
| -------- | -------------- | ------ |
| **Create** | Cadastro de veículo; registro de entrada | `veiculos`, `estacionamentos` |
| **Read** | Consulta de veículo, vagas, estacionados, histórico e faturamento | `veiculos`, `estacionamentos` |
| **Update** | Edição de modelo e cor; registro de saída | `veiculos`, `estacionamentos` |
| **Delete** | Remoção do cadastro de veículo não estacionado | `veiculos` |

## 5. Conceitos de POO utilizados

| Conceito | Como foi aplicado |
| -------- | ----------------- |
| **Classes e objetos** | Cada entidade do domínio (veículo, vaga, ticket, estacionamento) é uma classe; o sistema cria objetos em tempo de execução. |
| **Abstração** | `Veiculo` é abstrata: define o que todo veículo faz, sem fixar como a tarifa é calculada. |
| **Herança** | `Carro`, `Moto` e `Caminhonete` herdam atributos e métodos de `Veiculo`. |
| **Polimorfismo** | `calcularTarifa` é virtual pura em `Veiculo` e implementada em cada classe derivada. O chamador usa o tipo base e o comportamento é decidido pelo objeto concreto em tempo de execução. |
| **Encapsulamento** | Atributos `private` ou `protected`, acessados por métodos públicos (`get...`, `ocupar`, `liberar`). |
| **Modificadores de acesso** | `public` para a interface das classes, `protected` para o que as derivadas reaproveitam (como `placa` em `Veiculo`) e `private` para o estado interno (como `ocupada` em `Vaga`). |
| **Referências e ponteiros** | `Estacionamento` guarda uma referência (`Banco&`) para usar a mesma conexão do banco sem copiá-la; `Banco` guarda o ponteiro para o handle do SQLite; `encontrarVagaLivre` devolve um ponteiro (`Vaga*`) para a vaga original, que é ocupada sem cópia (ou `nullptr` quando não há vaga); `criarVeiculo` devolve um ponteiro inteligente (`unique_ptr<Veiculo>`), que libera o objeto automaticamente; as listagens percorrem as vagas por referência constante (`const Vaga&`). |

### Exemplo de polimorfismo no código

Em `Estacionamento::registrarSaida` (`src/Estacionamento.cpp`), o veículo é manipulado por um ponteiro para a **classe base**:

```cpp
unique_ptr<Veiculo> veiculo = criarVeiculo(dados);
double valor = veiculo->calcularTarifa(minutos);
```

A variável `veiculo` tem o tipo `Veiculo`, mas o objeto apontado é um `Carro`, uma `Moto` ou uma `Caminhonete`, e isso só é conhecido quando o programa executa. Por isso, a chamada a `calcularTarifa` executa a versão da classe derivada correta, sem nenhum `if` sobre o tipo do veículo no ponto do cálculo.

### Criação dos objetos (fábrica simples)

O método `Estacionamento::criarVeiculo` decide qual classe concreta instanciar a partir do tipo guardado no banco e devolve sempre um `unique_ptr<Veiculo>`:

```cpp
unique_ptr<Veiculo> Estacionamento::criarVeiculo(const DadosVeiculo& dados) {
    if (dados.tipo == "Moto") {
        return make_unique<Moto>(dados.placa, dados.modelo, dados.cor, taxaMoto);
    }
    if (dados.tipo == "Caminhonete") {
        return make_unique<Caminhonete>(dados.placa, dados.modelo, dados.cor, taxaCarro);
    }
    return make_unique<Carro>(dados.placa, dados.modelo, dados.cor, taxaCarro);
}
```

Esse método concentra a criação dos objetos em um só lugar e esconde das demais funções qual classe concreta foi criada. Ele segue a ideia do padrão **Factory** na sua forma simples (*Simple Factory*): o resto do código só conhece `Veiculo`.

## 6. Tecnologias

C++17, SQLite 3, CMake 3.20+, Git e GitHub.

## 7. Como executar

**Linux**

```bash
sudo apt install build-essential cmake libsqlite3-dev
cmake -S . -B build
cmake --build build
./build/estacionamento
```

**Windows (MSYS2/MinGW)**

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/msys64/ucrt64"
cmake --build build
.\build\estacionamento.exe
```

## 8. Limitações e trabalhos futuros

- Interface somente em terminal; uma interface gráfica seria um próximo passo.
- Leitura automática de placa não implementada.
- Evoluir a fábrica simples (`criarVeiculo`) para um Factory Method completo, com uma classe de fábrica própria, e aplicar outros padrões (como Singleton para a conexão com o banco).

## 9. Conclusão

O sistema cobre o ciclo completo de operação de um estacionamento e aplica abstração, herança, polimorfismo e encapsulamento em uma aplicação com banco de dados e CRUD completo. O projeto também serviu como primeira experiência da equipe com POO em C++, trabalho em grupo com Git e uso de SQLite.
