#include <sdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define TELA_LARGURA 960
#define TELA_ALTURA 640
#define MAX_ENTIDADES 256
#define TEMPO_TOQUE_DUPLO 0.25f

//Estruturas

typedef enum{

    TELA_TITULO = 0,
    JOGANDO,
    TELA_GAME_OVER,
    TELA_VITORIA

}EstadoJogo;

typedef enum{

    ENT_LIXO;
    ENT_PEIXE;
    ENT_PESSOA

}TipoEntidade;

typedef struct{

    TipoEntidade tipo;
    Vector2 pos;
    Vector2 vel;
    float tamanho;
    bool aivo;

}Entidade;

typedef struct{
float intervaloSpawn;
    float velocidadeEntidade;
    float probabilidadeLixo;
    const char* nome;
} ConfigFase;

// Variáveis estáticas globais
static EstadoJogo telaAtual;
static ConfigFase fases[4] = {
    {1.0f, 80.0f, 0.65f, "Fase 1 - Treinamento"},
    {0.8f, 110.0f, 0.60f, "Fase 2 - Acelera"},
    {0.6f, 150.0f, 0.50f, "Fase 3 - Tempestade"},
    {0.45f, 190.0f, 0.40f, "Fase 4 - Caos"}
};
static Entidade entidades[MAX_ENTIDADES];
static int qtdEntidades = 0;
static float tempoSpawn = 0;
static float tempoJogo = 60.0f;
static int pontos = 0;
static int indiceFase = 0;
static float nivelBarco = 80.0f;
static float nivelAgua = 150.0f;
static Vector2 posJogador;
static float velocidadeJogador = 180.0f;
static float tamanhoJogador = 28.0f;
static Vector2 posAncora;
static float velocidadeAncora = 220.0f;
static float tamanhoAncora = 16.0f;
static bool descendoAncora = false;
static bool subindoAncora = false;
static float tempoToqueDuplo = 0.0f;

void atualizar(float delta);
void desenhar(void);
static void ReiniciarJogo(void);
static float aleatorioFloat(float a, float b);
static void CriarEntidade(ConfigFase config);
static void AtualizarEntidades(float dt);
static void TratarColisoes(void);
static void RemoverEntidadesMortas(void);


void desenhar(){
    Begin()
}
