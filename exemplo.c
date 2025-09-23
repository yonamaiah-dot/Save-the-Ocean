#include <stdlib.h>
#include <stdio.h>

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
