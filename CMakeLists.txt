cmake_minimum_required(VERSION 3.20)
project(estacionamento LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

add_executable(estacionamento
    src/main.cpp
    src/Banco.cpp
    src/Caminhonete.cpp
    src/Carro.cpp
    src/Estacionamento.cpp
    src/Moto.cpp
    src/Ticket.cpp
    src/Vaga.cpp
    src/Veiculo.cpp
)

target_include_directories(estacionamento PRIVATE include)
target_link_libraries(estacionamento PRIVATE SQLite::SQLite3)
