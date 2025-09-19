#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

/*---------------------------------------------
 * Library headers.
 *-------------------------------------------*/
#include "raylib/raylib.h"
#include "raylib/raymath.h"

/*---------------------------------------------
 * Macros.
 *-------------------------------------------*/
#define TELA_LARGURA 960
#define TELA_ALTURA 640
#define MAX_ENTIDADES 256
#define TEMPO_TOQUE_DUPLO 0.25f

/*--------------------------------------------
 * Constants.
 *------------------------------------------*/

/*---------------------------------------------
 * Custom types (enums, structs, unions, etc.)
 *-------------------------------------------*/
typedef enum {
    TELA_TITULO = 0,
    JOGANDO,
    TELA_GAME_OVER,
    TELA_VITORIA
} EstadoJogo;

typedef enum {
    ENT_LIXO,
    ENT_PEIXE,
    ENT_PESSOA
} TipoEntidade;

typedef struct {
    TipoEntidade tipo;
    Vector2 pos;
    Vector2 vel;
    float tamanho;
    bool ativo;
} Entidade;

typedef struct {
    float intervaloSpawn;
    float velocidadeEntidade;
    float probabilidadeLixo;
    const char* nome;
} ConfigFase;

/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/
EstadoJogo telaAtual;

ConfigFase fases[4] = {
    {1.0f, 80.0f, 0.65f, "Fase 1 - Treinamento"},
    {0.8f, 110.0f, 0.60f, "Fase 2 - Acelera"},
    {0.6f, 150.0f, 0.50f, "Fase 3 - Tempestade"},
    {0.45f, 190.0f, 0.40f, "Fase 4 - Caos"}
};

Entidade entidades[MAX_ENTIDADES];
int qtdEntidades = 0;
float tempoSpawn = 0;
float tempoJogo = 60.0f;
int pontos = 0;
int indiceFase = 0;
float nivelBarco = 80.0f;
float nivelAgua = 150.0f;

Vector2 posJogador;
float velocidadeJogador = 180.0f;
float tamanhoJogador = 28.0f;

Vector2 posAncora;
float velocidadeAncora = 220.0f;
float tamanhoAncora = 16.0f;
bool descendoAncora = false;
bool subindoAncora = false;
float tempoToqueDuplo = 0.0f;

/*---------------------------------------------
 * Function prototypes.
 *-------------------------------------------*/
void atualizar(float delta);
void desenhar(void);
void ReiniciarJogo(void);
float aleatorioFloat(float a, float b);
void CriarEntidade(ConfigFase config);
void AtualizarEntidades(float dt);
void TratarColisoes(void);
void RemoverEntidadesMortas(void);

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
    return 0;
}

/*---------------------------------------------
 * Functions.
 *-------------------------------------------*/
void atualizar(float delta) {
    switch (telaAtual) {
        case TELA_TITULO: {
            Rectangle botaoJogar = { TELA_LARGURA/2 - 100, TELA_ALTURA/2 + 20, 200, 50 };
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botaoJogar)) || IsKeyPressed(KEY_ENTER)) {
                telaAtual = JOGANDO;
                ReiniciarJogo();
            }
        } break;
        case JOGANDO: {
            tempoJogo -= delta;
            if (tempoJogo <= 0) {
                tempoJogo = 0;
                telaAtual = TELA_GAME_OVER;
            }
            // movimento horizontal do barco
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                posJogador.x += velocidadeJogador * delta;
            }
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                posJogador.x -= velocidadeJogador * delta;
            }

            posJogador.x = Clamp(posJogador.x, tamanhoJogador, TELA_LARGURA - tamanhoJogador);
            posAncora.x = posJogador.x;

            if (IsKeyPressed(KEY_SPACE)) {
                if (tempoToqueDuplo > 0) {
                    subindoAncora = true;
                    descendoAncora = false;
                    tempoToqueDuplo = 0.0f;
                } else {
                    descendoAncora = true;
                    subindoAncora = false;
                    tempoToqueDuplo = TEMPO_TOQUE_DUPLO;
                }
            }

            if (tempoToqueDuplo > 0) {
                tempoToqueDuplo -= delta;
                if (tempoToqueDuplo <= 0) {
                    tempoToqueDuplo = 0.0f;
                }
            }

            if (descendoAncora) {
                posAncora.y += velocidadeAncora * delta;
                if (posAncora.y >= TELA_ALTURA - tamanhoAncora/2) {
                    posAncora.y = TELA_ALTURA - tamanhoAncora/2;
                    descendoAncora = false;
                }
            } else if (subindoAncora) {
                posAncora.y -= velocidadeAncora * delta;
                if (posAncora.y <= nivelBarco) {
                    posAncora.y = nivelBarco;
                    subindoAncora = false;
                }
            }

            tempoSpawn += delta;
            ConfigFase cfg = fases[indiceFase];
            if (tempoSpawn >= cfg.intervaloSpawn) {
                tempoSpawn = 0;
                int qtdSpawn = 1 + (indiceFase / 1);
                for (int i = 0; i < qtdSpawn; i++) CriarEntidade(cfg);
            }

            AtualizarEntidades(delta);
            TratarColisoes();
            RemoverEntidadesMortas();
            
            if (pontos >= 100) {
                indiceFase++;
                if (indiceFase >= 4) {
                    telaAtual = TELA_VITORIA;
                } else {
                    pontos = 0;
                    qtdEntidades = 0;
                    tempoSpawn = 0;
                    velocidadeJogador += 25;
                    tempoJogo = 60.0f;
                }
            }
        } break;
        case TELA_GAME_OVER: {
            Rectangle botaoReiniciar = { TELA_LARGURA/2 - 100, TELA_ALTURA/2 + 30, 200, 50 };
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botaoReiniciar)) {
                telaAtual = TELA_TITULO;
            }
        } break;
        case TELA_VITORIA: {
            Rectangle botaoReiniciar = { TELA_LARGURA/2 - 100, TELA_ALTURA/2 + 30, 200, 50 };
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botaoReiniciar)) {
                telaAtual = TELA_TITULO;
            }
        } break;
    }
}

void desenhar(void) {
    BeginDrawing();
    ClearBackground((Color){ 20, 100, 155, 255 });

    switch (telaAtual) {
        case TELA_TITULO: {
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
            DrawRectangle(0,0, TELA_LARGURA, 48, (Color){10,10,10,120});
            DrawText(TextFormat("Pontos: %i", pontos), 16, 8, 20, RAYWHITE);
            DrawText(TextFormat("Fase: %i / 4", indiceFase+1), 220, 8, 20, RAYWHITE);
            DrawText(fases[indiceFase].nome, 360, 8, 18, RAYWHITE);
            DrawText(TextFormat("Tempo: %.0f", tempoJogo), 800, 8, 20, RAYWHITE);
            DrawLineEx((Vector2){0, nivelAgua}, (Vector2){TELA_LARGURA, nivelAgua}, 2, BLUE);

            for (int i = 0; i < qtdEntidades; i++) {
                Entidade *e = &entidades[i];
                if (!e->ativo) continue;
                
                if (e->tipo == ENT_LIXO) {
                    DrawRectanglePro((Rectangle){ e->pos.x, e->pos.y, e->tamanho, e->tamanho }, (Vector2){e->tamanho/2, e->tamanho/2}, 0, (Color){150, 150, 150, 255});
                    DrawLine(e->pos.x - e->tamanho/3, e->pos.y - e->tamanho/3, e->pos.x + e->tamanho/3, e->pos.y + e->tamanho/3, BLACK);
                    DrawLine(e->pos.x - e->tamanho/3, e->pos.y + e->tamanho/3, e->pos.x + e->tamanho/3, e->pos.y - e->tamanho/3, BLACK);
                } else if (e->tipo == ENT_PEIXE) {
                    DrawCircleV(e->pos, e->tamanho/2, (Color){80, 200, 120, 255});
                    DrawTriangle((Vector2){e->pos.x - e->tamanho/2, e->pos.y}, (Vector2){e->pos.x - e->tamanho/2 - e->tamanho/3, e->pos.y - e->tamanho/4}, (Vector2){e->pos.x - e->tamanho/2 - e->tamanho/3, e->pos.y + e->tamanho/4}, (Color){50,160,90,255});
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

void ReiniciarJogo(void) {
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

float aleatorioFloat(float a, float b) {
    return a + (float)rand() / RAND_MAX * (b - a);
}

void CriarEntidade(ConfigFase config) {
    if (qtdEntidades >= MAX_ENTIDADES) {
        return;
    }
    Entidade e = {0};
    float r = aleatorioFloat(0.0f, 1.0f);
    if (r < config.probabilidadeLixo) {
        e.tipo = ENT_LIXO;
    } else if (r < config.probabilidadeLixo + (1.0f - config.probabilidadeLixo) * 0.714f) {
        e.tipo = ENT_PEIXE;
    } else {
        e.tipo = ENT_PESSOA;
    }
    
    e.pos.x = aleatorioFloat(40, TELA_LARGURA - 40);
    e.pos.y = aleatorioFloat(nivelAgua + 20, TELA_ALTURA - 40);
    e.tamanho = (e.tipo == ENT_LIXO) ? aleatorioFloat(14, 28) : aleatorioFloat(22, 40);
    e.vel.x = aleatorioFloat(-config.velocidadeEntidade/2, config.velocidadeEntidade/2);
    e.vel.y = aleatorioFloat(-config.velocidadeEntidade/2, config.velocidadeEntidade/2);
    e.ativo = true;
    entidades[qtdEntidades++] = e;
}

void AtualizarEntidades(float dt) {
    for (int i = 0; i < qtdEntidades; i++) {
        if (!entidades[i].ativo) continue;
        entidades[i].pos.x += entidades[i].vel.x * dt;
        entidades[i].pos.y += entidades[i].vel.y * dt;

        if (entidades[i].pos.x < 0) {
            entidades[i].pos.x = TELA_LARGURA;
        }
        if (entidades[i].pos.x > TELA_LARGURA) {
            entidades[i].pos.x = 0;
        }
        
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

void TratarColisoes(void) {
    Rectangle ancoraRec = { posAncora.x - tamanhoAncora/2, posAncora.y - tamanhoAncora/2, tamanhoAncora, tamanhoAncora };
    for (int i = 0; i < qtdEntidades; i++) {
        if (!entidades[i].ativo) {
            continue;
        }
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

void RemoverEntidadesMortas(void) {
    int gravar = 0;
    for (int i = 0; i < qtdEntidades; i++) {
        if (entidades[i].ativo) {
            if (gravar != i) entidades[gravar] = entidades[i];
            gravar++;
        }
    }
    qtdEntidades = gravar;
}