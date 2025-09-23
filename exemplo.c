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

int main(void){
    
static void TratarColisoes(void) {
    Rectangle ancoraRec = { posAncora.x - tamanhoAncora/2, posAncora.y -
tamanhoAncora/2, tamanhoAncora, tamanhoAncora };
    for (int i = 0; i < qtdEntidades; i++) {
        if (!entidades[i].ativo) continue;
        if (CheckCollisionCircleRec(entidades[i].pos, entidades[i].tamanho/2, ancoraRec)) {
            if (entidades[i].tipo == ENT_LIXO) {
                pontos += 10;
            } else if (entidades[i].tipo == ENT_PEIXE || entidades[i].tipo == ENT_PESSOA) {
                pontos -= 15;
            }
            entidades[i].ativo = false;
            subindoAncora = true;
            descendoAncora = false;
        }
    }   
}
static void CriarEntidade(ConfigFase config) {
    if (qtdEntidades >= MAX_ENTIDADES) return;
    Entidade e = {0};
    float r = aleatorioFloat(0.0f, 1.0f);
    if (r < config.probabilidadeLixo) e.tipo = ENT_LIXO;
    else if (r < config.probabilidadeLixo + (1.0f - config.probabilidadeLixo) * 0.714f) e.tipo = ENT_PEIXE;
    else e.tipo = ENT_PESSOA;
    e.pos.x = aleatorioFloat(40, TELA_LARGURA - 40);
    e.pos.y = aleatorioFloat(nivelAgua + 20, TELA_ALTURA - 40);
    e.tamanho = (e.tipo == ENT_LIXO) ? aleatorioFloat(14, 28) : aleatorioFloat(22, 40);
    e.vel.x = aleatorioFloat(-config.velocidadeEntidade/2, config.velocidadeEntidade/2);
    e.vel.y = aleatorioFloat(-config.velocidadeEntidade/2, config.velocidadeEntidade/2);
    e.ativo = true;
    entidades[qtdEntidades++] = e;
}
static void AtualizarEntidades(float dt) {
    for (int i = 0; i < qtdEntidades; i++) {
        if (!entidades[i].ativo) continue;
        entidades[i].pos.x += entidades[i].vel.x * dt;
        entidades[i].pos.y += entidades[i].vel.y * dt;
        if (entidades[i].pos.x < 0) entidades[i].pos.x = TELA_LARGURA;
        if (entidades[i].pos.x > TELA_LARGURA) entidades[i].pos.x = 0;
        if (entidades[i].pos.y < nivelAgua) {
            entidades[i].pos.y = nivelAgua;
            entidades[i].vel.y *= -1;
        }
        if (entidades[i].pos.y > TELA_ALTURA) {
            entidades[i].pos.y = TELA_ALTURA;
            entidades[i].vel.y *= -1;
        }
    }
}
static void RemoverEntidadesMortas(void) {
    int gravar = 0;
    for (int i = 0; i < qtdEntidades; i++) {
        if (entidades[i].ativo) {
            if (gravar != i) entidades[gravar] = entidades[i];
            gravar++;
        }
    }
    qtdEntidades = gravar;
}

}
