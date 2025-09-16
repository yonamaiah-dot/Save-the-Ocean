#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "raylib/raylib.h"
#include "raylib/raylib/raymath.h"

#define TELA_LARGURA 960
#define TELA_ALTURA 640

typedef enum{
    TELA_TITULO = 0,
    JOGANDO,
    TELA_GAME_OVER,
    TELA_VITORIA
}EstadoJogo;

typedef enum{
   ENT_LIXO,
   ENT_PEIXE,
   ENT_PESSOA
}TipoEntidade;

void atualizar(float delta);

int main(void){

    SetConfigFlags(FLAG_MSA-A_4X_HINT);
    InitWindow(TELA_LARGURA,TELA_ALTURA,"Salvar o Oceano");
    SetTargetFPS(60);
    srand(time(NULL));

    telaAtual = TELA_TITULO;

    while(!WindowShouldClose()){
        atualizar(GetFrameTime());
        desenhar();
    }

    CloseWindow();
    return 0;

}

void atualizar(float delta){

 switch (telaAtual){ 
        case TELA_TITULO:{
            const char* titulo ="Save The Ocean";
            const char* textoBoao = "Começar";

            Rectangle botaoJogar= {
                TELA_LARGURA / 2 - 100,
                TELA_ALTURA / 2 + 20, 200, 50
            };

            int larguraTitulo =           
            MeasureText(titulo, 40);
            int larguraBotao =
            MeasureText(textoBotao, 20);
        
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
            DrawLineEx((Vector2){0, nivelAgua}, (Vector2){TELA_LARGURA, nivelAgua}, 2, BLUE);
            
        }
    }
}