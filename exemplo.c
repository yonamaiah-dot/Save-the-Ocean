#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "raylib/raylib.h"
#include "raylib/raymath.h"

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

typedef enum TipoEntidade{

    ENT_LIXO,
    ENT_PEIXE,
    ENT_PESSOA

}TipoEntidade;

typedef struct Entidade{

    TipoEntidade tipo;
    Vector2 pos;
    Vector2 vel;
    float tamanho;
    bool ativo;

}Entidade;

typedef struct ConfigFase{
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

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(TELA_LARGURA, TELA_ALTURA, "Save The Ocean");
    SetTargetFPS(60);
    srand(time(NULL));

    telaAtual = TELA_TITULO;

    while (!WindowShouldClose()) {
        atualizar(GetFrameTime());
        desenhar();
    }

    CloseWindow();
}

}
    
void desenhar(void) {
    BeginDrawing();
    ClearBackground((Color){ 20, 100, 155, 255 });

    switch (telaAtual) {
        case TELA_TITULO: { 
            
            //tela do começo
            const char* titulo = "Save The Ocean";
            const char* textoBotao = "Começar";
            Rectangle botaoJogar = { TELA_LARGURA/2 - 100, TELA_ALTURA/2 + 20, 200, 50 };
            
            int larguraTitulo = MeasureText(titulo, 40);
            int larguraBotao = MeasureText(textoBotao, 20);

            DrawText(titulo, TELA_LARGURA/2 - larguraTitulo/2, TELA_ALTURA/2 - 80, 40, RAYWHITE);
            DrawRectangleRec(botaoJogar, GREEN);
            DrawText(textoBotao, botaoJogar.x + (botaoJogar.width - larguraBotao)/2, botaoJogar.y + (botaoJogar.height - 20)/2, 20, BLACK);

        } break;
        case JOGANDO: {
            
             // o jogo estando sendo jogado
            
            DrawRectangle(0,0, TELA_LARGURA, 48, (Color){10,10,10,120});
            DrawText(TextFormat("Pontos: %i", pontos), 16, 8, 20, RAYWHITE);
            DrawText(TextFormat("Fase: %i / 4", indiceFase+1), 220, 8, 20, RAYWHITE);
            DrawText(fases[indiceFase].nome, 360, 8, 18, RAYWHITE);
            DrawText(TextFormat("Tempo: %.0f", tempoJogo), 800, 8, 20, RAYWHITE);
            DrawLineEx((Vector2){0, nivelAgua}, (Vector2){TELA_LARGURA, nivelAgua}, 2, BLUE);

            for (int i = 0; i < qtdEntidades; i++) {
                Entidade *e = &entidades[i];
                if (!e->ativo) continue;
                
                   //desenha o lixo
                if (e->tipo == ENT_LIXO) { 
                    DrawRectanglePro((Rectangle){ e->pos.x, e->pos.y, e->tamanho, e->tamanho }, (Vector2){e->tamanho/2, e->tamanho/2}, 0, (Color){150, 150, 150, 255});
                    DrawLine(e->pos.x - e->tamanho/3, e->pos.y - e->tamanho/3, e->pos.x + e->tamanho/3, e->pos.y + e->tamanho/3, BLACK);
                    DrawLine(e->pos.x - e->tamanho/3, e->pos.y + e->tamanho/3, e->pos.x + e->tamanho/3, e->pos.y - e->tamanho/3, BLACK);

                    //desenha o peixe
                } else if (e->tipo == ENT_PEIXE) {
                    DrawCircleV(e->pos, e->tamanho/2, (Color){80, 200, 120, 255});
                    DrawTriangle((Vector2){e->pos.x - e->tamanho/2, e->pos.y}, (Vector2){e->pos.x - e->tamanho/2 - e->tamanho/3, e->pos.y - e->tamanho/4}, (Vector2){e->pos.x - e->tamanho/2 - e->tamanho/3, e->pos.y + e->tamanho/4}, (Color){50,160,90,255});
                
                    //desenha as pessoas
                } else if (e->tipo == ENT_PESSOA) {
                    DrawCircle(e->pos.x, e->pos.y - e->tamanho*0.25f, e->tamanho*0.22f, (Color){240,200,180,255});
                    DrawRectangle(e->pos.x - e->tamanho*0.3f, e->pos.y - e->tamanho*0.0f, e->tamanho*0.6f, e->tamanho*0.8f, (Color){120, 80, 160, 255});
                }
            }
            
            DrawLineEx(posJogador, posAncora, 2, BROWN);
            DrawRectanglePro((Rectangle){posAncora.x, posAncora.y, tamanhoAncora, tamanhoAncora}, (Vector2){tamanhoAncora/2, tamanhoAncora/2}, 0, (Color){100, 100, 100, 255});
            DrawTriangle((Vector2){posJogador.x, posJogador.y - tamanhoJogador/2}, (Vector2){posJogador.x - tamanhoJogador, posJogador.y + tamanhoJogador/2}, (Vector2){posJogador.x + tamanhoJogador, posJogador.y + tamanhoJogador/2}, (Color){240, 240, 60, 255});
       
        } break;
        case TELA_GAME_OVER: { 

            //desenha a tela de derrota
            const char* textoGameOver = "Game Over";
            const char* textoReiniciar = "Recomeçar";
            Rectangle botaoReiniciar = { TELA_LARGURA/2 - 100, TELA_ALTURA/2 + 30, 200, 50 };
            
            int larguraGameOver = MeasureText(textoGameOver, 40);
            int larguraReiniciar = MeasureText(textoReiniciar, 20);
            
            DrawRectangle(0,0,TELA_LARGURA,TELA_ALTURA,(Color){0,0,0,160});
            DrawText(textoGameOver, TELA_LARGURA/2 - larguraGameOver/2, TELA_ALTURA/2 - 20, 40, RAYWHITE);
            DrawRectangleRec(botaoReiniciar, GREEN);
            DrawText(textoReiniciar, botaoReiniciar.x + (botaoReiniciar.width - larguraReiniciar)/2, botaoReiniciar.y + (botaoReiniciar.height - 20)/2, 20, BLACK);
        
        } break;
        case TELA_VITORIA: {

            //desenha a tela de vitoria
            const char* textoVitoria = "PARABENS! Voce completou todas as fases!";
            const char* textoReiniciar = "Recomeçar";
            Rectangle botaoReiniciar = { TELA_LARGURA/2 - 100, TELA_ALTURA/2 + 30, 200, 50 };

            int larguraVitoria = MeasureText(textoVitoria, 30);
            int larguraReiniciar = MeasureText(textoReiniciar, 20);

            DrawRectangle(0,0,TELA_LARGURA,TELA_ALTURA,(Color){0,0,0,120});
            DrawText(textoVitoria, TELA_LARGURA/2 - larguraVitoria/2, TELA_ALTURA/2 - 20, 30, RAYWHITE);
            DrawRectangleRec(botaoReiniciar, GREEN);
            DrawText(textoReiniciar, botaoReiniciar.x + (botaoReiniciar.width - larguraReiniciar)/2, botaoReiniciar.y + (botaoReiniciar.height - 20)/2, 20, BLACK);
        } break;
    }

    EndDrawing();
}

static void ReiniciarJogo(void) {
    qtdEntidades = 0;
    tempoSpawn = 0;
    tempoJogo = 60.0f;
    pontos = 0;
    indiceFase = 0;
    posJogador.x = TELA_LARGURA/2;
    posJogador.y = nivelBarco;
    posAncora.x = posJogador.x;
    posAncora.y = posJogador.y;
    velocidadeJogador = 180.0f;
    velocidadeAncora = 220.0f;
    descendoAncora = false;
    subindoAncora = false;
    tempoToqueDuplo = 0.0f;
}

static float aleatorioFloat(float a, float b) {
    return a + (float)rand() / RAND_MAX * (b - a);
}

static void TratarColisoes(void) {

    // Trata as colisoes das ancoras e as colisoes das entidades
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

    // cria as entidades
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


