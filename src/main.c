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
#define PONTOS_POR_FASE 100 // Pontos necessários para avançar

/*---------------------------------------------
 * Global Textures
 *-------------------------------------------*/
// Fundo
Texture2D texFundo;

// Barco
Texture2D texBarcoDir;
Texture2D texBarcoEsq;

// Peixes
Texture2D texPeixeAmareloDir;
Texture2D texPeixeAmareloEsq;
Texture2D texPeixeVerdeDir;
Texture2D texPeixeVerdeEsq;
Texture2D texPeixeAzulDir;
Texture2D texPeixeAzulEsq;
Texture2D texPeixeLaranjaDir;
Texture2D texPeixeLaranjaEsq;

// Pessoas
Texture2D texPessoaAmareloDir;
Texture2D texPessoaAmareloEsq;
Texture2D texPessoaAzulDir;
Texture2D texPessoaAzulEsq;
Texture2D texPessoaRosaDir;
Texture2D texPessoaRosaEsq; 

// Lixo
Texture2D texLixoPneu;
Texture2D texLixoSacola;
Texture2D texLixoLatinha;
Texture2D texLixoGarrafa;

// Âncora e Cabo
Texture2D texAncora;
Texture2D texCaboAncora;

/*---------------------------------------------
 * Custom types (enums, structs, unions, etc.)
 *-------------------------------------------*/
typedef enum EstadoJogo {
    TELA_TITULO = 0,
    JOGANDO,
    TELA_GAME_OVER,
    TELA_VITORIA
} EstadoJogo;

typedef enum TipoEntidade{
    ENT_LIXO,
    ENT_PEIXE,
    ENT_PESSOA
} TipoEntidade;

typedef enum TipoLixo {
    LIXO_PNEU = 15,
    LIXO_SACOLA = 12,
    LIXO_LATINHA = 10,
    LIXO_GARRAFA = 10
} TipoLixo;


typedef struct Entidade{
    TipoEntidade tipo;
    Vector2 pos;
    Vector2 vel;
    float tamanho;
    bool ativo;
    Texture2D textura;
    TipoLixo subtipoLixo; // Para lixo, guarda o valor de pontuação
} Entidade;

typedef struct ConfigFase{
    float intervaloSpawn;
    float velocidadeEntidade;
    float probabilidadeLixo;
    const char* nome;
} ConfigFase;

/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/
EstadoJogo telaAtual;

// CONFIGURAÇÃO DE 2 FASES
ConfigFase fases[2] = {
    {1.0f, 100.0f, 0.65f, "Fase 1: Águas Calmas"}, 
    {0.7f, 150.0f, 0.50f, "Fase 2: Corrente Forte"} 
};

Entidade entidades[MAX_ENTIDADES];
int qtdEntidades = 0;
float tempoSpawn = 0;
float tempoJogo = 60.0f; 
int pontos = 0;         
int pontosFase = 0;     
int indiceFase = 0;     
// VALORES AJUSTADOS PARA O NOVO FUNDO
float nivelAgua = 320.0f; 
float nivelBarco = 280.0f; // Ponto de flutuação do barco
// FIM DOS VALORES AJUSTADOS

Vector2 posJogador;
float velocidadeJogador = 180.0f;
float tamanhoJogador = 28.0f; 
Texture2D texBarcoAtual; 

Vector2 posAncora;
float velocidadeAncora = 220.0f;
float tamanhoAncora = 16.0f;
bool descendoAncora = false;
bool subindoAncora = false;
float tempoToqueDuplo = 0.0f;

/*---------------------------------------------
 * Function prototypes.
 *-------------------------------------------*/
void LoadGameAssets(void);
void UnloadGameAssets(void);
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

    LoadGameAssets();

    telaAtual = TELA_TITULO;
    texBarcoAtual = texBarcoDir;

    while (!WindowShouldClose()) {
        atualizar(GetFrameTime());
        desenhar();
    }

    UnloadGameAssets();

    CloseWindow();
    return 0;
}

/*---------------------------------------------
 * Functions.
 *-------------------------------------------*/

void LoadGameAssets(void)
{
    // Fundo
    texFundo = LoadTexture("imagens/fundo-n1.png");

    // Barco
    texBarcoDir = LoadTexture("imagens/barcoDir.png");
    texBarcoEsq = LoadTexture("imagens/barcoEsq.png"); 

    // Peixes
    texPeixeAmareloDir = LoadTexture("imagens/peixeAmareloDir.png");
    texPeixeAmareloEsq = LoadTexture("imagens/peixeAmareloEsq.png");
    texPeixeVerdeDir = LoadTexture("imagens/peixeVerdeDir.png");
    texPeixeVerdeEsq = LoadTexture("imagens/peixeVerdeEsq.png");
    texPeixeAzulDir = LoadTexture("imagens/peixeAzulDir.png");
    texPeixeAzulEsq = LoadTexture("imagens/peixeAzulEsq.png");
    texPeixeLaranjaDir = LoadTexture("imagens/peixeLaranjaDir.png"); 
    texPeixeLaranjaEsq = LoadTexture("imagens/peixeLaranjaEsq.png"); 
    
    // Pessoas
    texPessoaAmareloDir = LoadTexture("imagens/pessoaAmareloDir.png");
    texPessoaAmareloEsq = LoadTexture("imagens/pessoaAmareloEsq.png");
    texPessoaAzulDir = LoadTexture("imagens/pessoaAzulDir.png");
    texPessoaAzulEsq = LoadTexture("imagens/pessoaAzulEsq.png");
    texPessoaRosaDir = LoadTexture("imagens/pessoaRosaDir.png");
    texPessoaRosaEsq = LoadTexture("imagens/pessoaRosaEsq.png");

    // Lixo
    texLixoPneu = LoadTexture("imagens/pneu.png");
    texLixoSacola = LoadTexture("imagens/sacola.png");
    texLixoLatinha = LoadTexture("imagens/latinha.png");
    texLixoGarrafa = LoadTexture("imagens/garrafaPlastico.png");

    // Âncora e Cabo
    texAncora = LoadTexture("imagens/ancora.png");
    texCaboAncora = LoadTexture("imagens/caboAncora.png");
    
    if (texBarcoDir.id != 0) {
        texBarcoAtual = texBarcoDir;
    }
}

void UnloadGameAssets(void)
{
    // Fundo
    UnloadTexture(texFundo);
    // Barco
    UnloadTexture(texBarcoDir);
    UnloadTexture(texBarcoEsq);
    // Peixes
    UnloadTexture(texPeixeAmareloDir);
    UnloadTexture(texPeixeAmareloEsq);
    UnloadTexture(texPeixeVerdeDir);
    UnloadTexture(texPeixeVerdeEsq);
    UnloadTexture(texPeixeAzulDir);
    UnloadTexture(texPeixeAzulEsq);
    UnloadTexture(texPeixeLaranjaDir);
    UnloadTexture(texPeixeLaranjaEsq);
    // Pessoas
    UnloadTexture(texPessoaAmareloDir);
    UnloadTexture(texPessoaAmareloEsq);
    UnloadTexture(texPessoaAzulDir);
    UnloadTexture(texPessoaAzulEsq);
    UnloadTexture(texPessoaRosaDir);
    UnloadTexture(texPessoaRosaEsq);
    // Lixo
    UnloadTexture(texLixoPneu);
    UnloadTexture(texLixoSacola);
    UnloadTexture(texLixoLatinha);
    UnloadTexture(texLixoGarrafa);
    // Âncora e Cabo
    UnloadTexture(texAncora);
    UnloadTexture(texCaboAncora);
}


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
            
            // Movimento do barco (Pode sair da tela)
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                posJogador.x += velocidadeJogador * delta;
                texBarcoAtual = texBarcoDir;
            }
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                posJogador.x -= velocidadeJogador * delta;
                texBarcoAtual = texBarcoEsq;
            }
            
            // A posição da âncora é atualizada com a do jogador
            posAncora.x = posJogador.x;
            
            // Limita a âncora para que ela não passe das bordas horizontais da tela
            float limiteMinAncora = (float)tamanhoAncora;
            float limiteMaxAncora = TELA_LARGURA - (float)tamanhoAncora;
            posAncora.x = Clamp(posAncora.x, limiteMinAncora, limiteMaxAncora);

            // A posição do jogador é forçada a ficar na mesma coluna da âncora se ela estiver no limite.
            // Isso simula o cabo esticado.
            if (posAncora.x == limiteMinAncora || posAncora.x == limiteMaxAncora) {
                 posJogador.x = posAncora.x;
            }


            // Lógica da Âncora
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

            // Lógica de Spawn
            tempoSpawn += delta;
            ConfigFase cfg = fases[indiceFase];
            if (tempoSpawn >= cfg.intervaloSpawn) {
                tempoSpawn = 0;
                CriarEntidade(cfg);
            }

            AtualizarEntidades(delta);
            TratarColisoes();
            RemoverEntidadesMortas();
            
            // Lógica de AVANÇO DE FASE / VITÓRIA
            if (pontosFase >= PONTOS_POR_FASE) {
                indiceFase++;
                if (indiceFase >= 2) {
                    // Completou todas as fases
                    telaAtual = TELA_VITORIA;
                } else {
                    // Avança para a próxima fase
                    pontosFase = 0;
                    qtdEntidades = 0;
                    tempoSpawn = 0;
                    velocidadeJogador += 20;
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
    
    // Desenha o fundo esticado para a tela cheia
    if (texFundo.id > 0) {
        DrawTexturePro(texFundo, 
                       (Rectangle){0, 0, (float)texFundo.width, (float)texFundo.height}, 
                       (Rectangle){0, 0, TELA_LARGURA, TELA_ALTURA}, 
                       (Vector2){0, 0}, 
                       0.0f, 
                       WHITE);
    } else {
        ClearBackground((Color){ 20, 100, 155, 255 }); 
    }

    // --- Início da correção: Agora cada tela é tratada de forma exclusiva ---
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
            ConfigFase cfg = fases[indiceFase];
            
            // Desenho das Entidades
            for (int i = 0; i < qtdEntidades; i++) {
                Entidade *e = &entidades[i];
                if (!e->ativo) continue;
                
                // Lógica de desenho para LIXO, PEIXE, PESSOA (mantida como estava)
                if (e->tipo == ENT_LIXO) {
                    DrawTexturePro(e->textura, 
                                   (Rectangle){ 0.0f, 0.0f, (float)e->textura.width, (float)e->textura.height },
                                   (Rectangle){ e->pos.x, e->pos.y, e->tamanho, e->tamanho },
                                   (Vector2){ e->tamanho/2, e->tamanho/2 },
                                   0.0f, 
                                   WHITE);
                                   
                } else if (e->tipo == ENT_PEIXE) {
                    
                    Texture2D texPeixeBaseDir = texPeixeLaranjaDir;
                    if (e->textura.id == texPeixeAmareloDir.id || e->textura.id == texPeixeAmareloEsq.id) texPeixeBaseDir = texPeixeAmareloDir;
                    else if (e->textura.id == texPeixeVerdeDir.id || e->textura.id == texPeixeVerdeEsq.id) texPeixeBaseDir = texPeixeVerdeDir;
                    else if (e->textura.id == texPeixeAzulDir.id || e->textura.id == texPeixeAzulEsq.id) texPeixeBaseDir = texPeixeAzulDir;
                    
                    if (e->vel.x > 0) { 
                        if (texPeixeBaseDir.id == texPeixeAmareloDir.id) e->textura = texPeixeAmareloDir;
                        else if (texPeixeBaseDir.id == texPeixeVerdeDir.id) e->textura = texPeixeVerdeDir;
                        else if (texPeixeBaseDir.id == texPeixeAzulDir.id) e->textura = texPeixeAzulDir;
                        else if (texPeixeBaseDir.id == texPeixeLaranjaDir.id) e->textura = texPeixeLaranjaDir;
                    } else if (e->vel.x < 0) {
                        if (texPeixeBaseDir.id == texPeixeAmareloDir.id) e->textura = texPeixeAmareloEsq;
                        else if (texPeixeBaseDir.id == texPeixeVerdeDir.id) e->textura = texPeixeVerdeEsq;
                        else if (texPeixeBaseDir.id == texPeixeAzulDir.id) e->textura = texPeixeAzulEsq;
                        else if (texPeixeBaseDir.id == texPeixeLaranjaDir.id) e->textura = texPeixeLaranjaEsq;
                    }
                    
                    DrawTexturePro(e->textura, 
                                   (Rectangle){ 0.0f, 0.0f, (float)e->textura.width, (float)e->textura.height },
                                   (Rectangle){ e->pos.x, e->pos.y, e->tamanho * 1.5f, e->tamanho },
                                   (Vector2){ (e->tamanho * 1.5f)/2, e->tamanho/2 },
                                   0.0f, 
                                   WHITE);
                                   
                } else if (e->tipo == ENT_PESSOA) {
                    
                    Texture2D texPessoaBaseDir = texPessoaAmareloDir;
                    if (e->textura.id == texPessoaAzulDir.id || e->textura.id == texPessoaAzulEsq.id) texPessoaBaseDir = texPessoaAzulDir;
                    else if (e->textura.id == texPessoaRosaDir.id || e->textura.id == texPessoaRosaEsq.id) texPessoaBaseDir = texPessoaRosaDir;
                    
                    if (e->vel.x > 0) {
                        if (texPessoaBaseDir.id == texPessoaAmareloDir.id) e->textura = texPessoaAmareloDir;
                        else if (texPessoaBaseDir.id == texPessoaAzulDir.id) e->textura = texPessoaAzulDir;
                        else if (texPessoaBaseDir.id == texPessoaRosaDir.id) e->textura = texPessoaRosaDir;
                    } else if (e->vel.x < 0) {
                        if (texPessoaBaseDir.id == texPessoaAmareloDir.id) e->textura = texPessoaAmareloEsq;
                        else if (texPessoaBaseDir.id == texPessoaAzulDir.id) e->textura = texPessoaAzulEsq;
                        else if (texPessoaBaseDir.id == texPessoaRosaDir.id) e->textura = texPessoaRosaEsq;
                    }
                    
                    DrawTexturePro(e->textura, 
                                   (Rectangle){ 0.0f, 0.0f, (float)e->textura.width, (float)e->textura.height },
                                   (Rectangle){ e->pos.x, e->pos.y, e->tamanho * 1.5f, e->tamanho * 1.5f },
                                   (Vector2){ (e->tamanho * 1.5f)/2, (e->tamanho * 1.5f)/2 },
                                   0.0f, 
                                   WHITE);
                }
            }
            
            // Desenho da Âncora e Cabo
            DrawTexturePro(texCaboAncora, (Rectangle){0.0f, 0.0f, (float)texCaboAncora.width, 1.0f}, (Rectangle){posJogador.x, posJogador.y, (float)texCaboAncora.width, posAncora.y - posJogador.y}, (Vector2){(float)texCaboAncora.width / 2.0f, 0.0f}, 0.0f, WHITE);
            
            DrawTexturePro(
                texAncora, 
                (Rectangle){ 0.0f, 0.0f, (float)texAncora.width, (float)texAncora.height }, 
                (Rectangle){ posAncora.x, posAncora.y, tamanhoAncora * 2.0f, tamanhoAncora * 2.0f }, 
                (Vector2){ tamanhoAncora, tamanhoAncora }, 
                0.0f, 
                WHITE
            );
            
            // Desenho do Barco
            DrawTextureV(texBarcoAtual, 
                         (Vector2){ posJogador.x - texBarcoAtual.width/2.0f, posJogador.y - (texBarcoAtual.height - 20) }, 
                         WHITE);

            // Desenho do HUD (desenhado sobre tudo)
            DrawRectangle(0,0, TELA_LARGURA, 48, (Color){10,10,10,120});
            DrawText(TextFormat("Total: %i", pontos), 16, 8, 20, RAYWHITE);
            DrawText(TextFormat("Fase %i: %i/%i", indiceFase+1, pontosFase, PONTOS_POR_FASE), 180, 8, 20, RAYWHITE);
            DrawText(cfg.nome, 380, 8, 18, RAYWHITE);
            DrawText(TextFormat("Tempo: %.0f", tempoJogo), 800, 8, 20, RAYWHITE);
            
        } break;
        
        case TELA_GAME_OVER: {
            const char* textoGameOver = "Game Over!";
            const char* textoReiniciar = "Recomeçar";
            Rectangle botaoReiniciar = { TELA_LARGURA/2 - 100, TELA_ALTURA/2 + 70, 200, 50 }; 
            
            int larguraGameOver = MeasureText(textoGameOver, 40);
            int larguraReiniciar = MeasureText(textoReiniciar, 20);
            
            DrawRectangle(0,0,TELA_LARGURA,TELA_ALTURA,(Color){0,0,0,160});
            DrawText(textoGameOver, TELA_LARGURA/2 - larguraGameOver/2, TELA_ALTURA/2 - 20, 40, RAYWHITE);
            DrawText(TextFormat("Pontos Totais: %i", pontos), TELA_LARGURA/2 - MeasureText(TextFormat("Pontos Totais: %i", pontos), 20)/2, TELA_ALTURA/2 + 30, 20, RAYWHITE); 
            DrawRectangleRec(botaoReiniciar, GREEN);
            DrawText(textoReiniciar, botaoReiniciar.x + (botaoReiniciar.width - larguraReiniciar)/2, botaoReiniciar.y + (botaoReiniciar.height - 20)/2, 20, BLACK);
        } break;
        
        case TELA_VITORIA: {
            const char* textoVitoria = "PARABENS! O OCEANO ESTÁ SALVO!";
            const char* textoReiniciar = "Jogar Novamente";
            Rectangle botaoReiniciar = { TELA_LARGURA/2 - 100, TELA_ALTURA/2 + 30, 200, 50 };

            int larguraVitoria = MeasureText(textoVitoria, 30);
            int larguraReiniciar = MeasureText(textoReiniciar, 20);

            DrawRectangle(0,0,TELA_LARGURA,TELA_ALTURA,(Color){0,0,0,120});
            DrawText(textoVitoria, TELA_LARGURA/2 - larguraVitoria/2, TELA_ALTURA/2 - 40, 30, RAYWHITE);
            DrawText(TextFormat("Pontos Totais: %i", pontos), TELA_LARGURA/2 - MeasureText(TextFormat("Pontos Totais: %i", pontos), 20)/2, TELA_ALTURA/2 + 0, 20, RAYWHITE);
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
    pontosFase = 0;
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
    texBarcoAtual = texBarcoDir;
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
    
    // LIXO
    if (r < config.probabilidadeLixo) {
        e.tipo = ENT_LIXO;
        
        float r_lixo = aleatorioFloat(0.0f, 1.0f);
        
        if (r_lixo < 0.25f) { // Pneu (15pts)
            e.textura = texLixoPneu;
            e.subtipoLixo = LIXO_PNEU;
            e.tamanho = 35.0f;
        } else if (r_lixo < 0.50f) { // Sacola (12pts)
            e.textura = texLixoSacola;
            e.subtipoLixo = LIXO_SACOLA;
            e.tamanho = 30.0f;
        } else if (r_lixo < 0.75f) { // Latinha (10pts)
            e.textura = texLixoLatinha;
            e.subtipoLixo = LIXO_LATINHA;
            e.tamanho = 20.0f;
        } else { // Garrafa (10pts)
            e.textura = texLixoGarrafa;
            e.subtipoLixo = LIXO_GARRAFA;
            e.tamanho = 20.0f;
        }
        
    // PEIXE
    } else if (r < config.probabilidadeLixo + (1.0f - config.probabilidadeLixo) * 0.714f) {
        e.tipo = ENT_PEIXE;
        
        float r_peixe = aleatorioFloat(0.0f, 1.0f);
        
        if (r_peixe < 0.25f) {
            e.textura = texPeixeAmareloDir;
        } else if (r_peixe < 0.50f) {
            e.textura = texPeixeVerdeDir;
        } else if (r_peixe < 0.75f) {
            e.textura = texPeixeLaranjaDir;
        } else {
            e.textura = texPeixeAzulDir;
        }
        e.tamanho = 28.0f; 
        
    // PESSOA
    } else {
        e.tipo = ENT_PESSOA;
        
        float r_pessoa = aleatorioFloat(0.0f, 1.0f);
        if (r_pessoa < 0.33f) { 
            e.textura = texPessoaAmareloDir;
        } else if (r_pessoa < 0.66f) {
            e.textura = texPessoaAzulDir;
        } else {
            e.textura = texPessoaRosaDir;
        }
        e.tamanho = 28.0f;
    }
   
    e.pos.x = aleatorioFloat(40, TELA_LARGURA - 40);
    e.pos.y = aleatorioFloat(nivelAgua + 20, TELA_ALTURA - 40);
    
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
                int pontosGanhos = (int)entidades[i].subtipoLixo; 
                
                pontos += pontosGanhos;
                pontosFase += pontosGanhos;
                
            } else if (entidades[i].tipo == ENT_PEIXE || entidades[i].tipo == ENT_PESSOA) {
                pontos -= 15;
                pontosFase -= 15;
            }
            if (pontosFase < 0) pontosFase = 0; 

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