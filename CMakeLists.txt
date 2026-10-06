cmake_minimum_required(VERSION 3.20)
project(estacionamento LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8)
endif()

find_package(SQLite3 REQUIRED)

add_library(nucleo STATIC
    src/Banco.cpp
    src/Caminhonete.cpp
    src/Carro.cpp
    src/Estacionamento.cpp
    src/Moto.cpp
    src/Ticket.cpp
    src/Vaga.cpp
    src/Veiculo.cpp
)
target_include_directories(nucleo PUBLIC include)
target_link_libraries(nucleo PUBLIC SQLite::SQLite3)


add_executable(estacionamento src/main.cpp)
target_link_libraries(estacionamento PRIVATE nucleo)

find_package(Qt6 COMPONENTS Widgets QUIET)

if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)

    add_executable(estacionamento_gui
        gui/main_gui.cpp
        gui/JanelaPrincipal.cpp
        gui/JanelaPrincipal.h
    )
    target_link_libraries(estacionamento_gui PRIVATE nucleo Qt6::Widgets)
else()
    message(STATUS "Qt6 nao encontrado: compilando apenas a versao de terminal.")
endif()