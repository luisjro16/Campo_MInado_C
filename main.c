#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <allegro5/allegro_image.h>

#define verde_claro al_map_rgb(169,215,81)
#define verde_escuro al_map_rgb(162, 208, 73)
#define areia_clara al_map_rgb(228, 194, 159)
#define areia_escura al_map_rgb(215, 184, 153)

const float FPS = 100;//Constante para contagem do tempo

//Tamanho da janela
const int SCREEN_W = 380;
const int SCREEN_H = 560;

//Constantes
int linha;
int coluna;
int bandeira = 0;
int bombas = 28;
int minutos = 0, segundos = 0;
int n_jogada = 0;
int jogo = 1; //0-Jogo desligado 1-Jogo Funcionando
int jogando = 1;//0-Game Over 1-Jogando 2-Ganhou 3-Menu
float pos_x, pos_y;

//Estrutura com as caracterisitcas das celulas
typedef struct campo{
    int Bomba; // 0-Nao 1-Sim
    int Aberto; // 0-Nao 1-Sim
    int Vizinhos; // 0-4 Numero de bombas
    int ZeroUsado; // 0-Nao 1-Sim
    int Marcado; // 0-Nao 1-Sim
    float coord_x;
    float coord_y;
    ALLEGRO_COLOR cor_aberto;
    ALLEGRO_COLOR cor_fechado;
};
struct campo celula[16][10];

//Procedimento para desenhar cenario
void draw_scenario(){

    int lin, cou;

    al_clear_to_color(al_map_rgb(86,138,53));
    al_draw_filled_rectangle(0, 0,
                             SCREEN_W, 60,
                             al_map_rgb(74,117,45));
    for(lin=0; lin<16; lin++){
        for(cou=0; cou<10; cou++){
            if(celula[lin][cou].Aberto == 0){
                al_draw_filled_rectangle(celula[lin][cou].coord_x, celula[lin][cou].coord_y,
                                         celula[lin][cou].coord_x+30, celula[lin][cou].coord_y+30,
                                         celula[lin][cou].cor_fechado);
            }
            if(celula[lin][cou].Aberto == 1){
                al_draw_filled_rectangle(celula[lin][cou].coord_x, celula[lin][cou].coord_y,
                                         celula[lin][cou].coord_x+30, celula[lin][cou].coord_y+30,
                                         celula[lin][cou].cor_aberto);

            }
        }
    }

}

//Procedimento de esvaziar membros da celula
void EsvaziarCelula(){
    for (linha=0; linha<16; linha++){
        for (coluna=0; coluna<10; coluna++){
            celula[linha][coluna].Bomba = 0;
            celula[linha][coluna].Aberto = 0;
            celula[linha][coluna].Vizinhos = 0;
            celula[linha][coluna].ZeroUsado = 0;
            celula[linha][coluna].Marcado = 0;
            celula[linha][coluna].coord_x = 0;
            celula[linha][coluna].coord_y = 0;
        }
    }

}

//Procedimento cadastrar coordenadas
void Coordenadas(){
    int verif_linha = -1, verif_coluna = -1;

    float tam_x = 41, tam_y = 70;
    for(linha=0; linha<16; linha++){
        if(linha != 0){
            tam_y += 30;
        }
        for(coluna=0; coluna<10;coluna++){
            celula[linha][coluna].coord_y = tam_y;
            celula[linha][coluna].coord_x = tam_x;
            tam_x+= 30;
        }
        tam_x = 41;
    }

    for(linha=0; linha<16; linha++){
        verif_linha++;
        for(coluna=0; coluna<10; coluna++){
            if(verif_linha % 2 == 0){
                verif_coluna++;
                if(verif_coluna % 2 == 0){
                    celula[linha][coluna].cor_fechado = verde_claro;
                    celula[linha][coluna].cor_aberto = areia_clara;
                }else{
                    celula[linha][coluna].cor_fechado = verde_escuro;
                    celula[linha][coluna].cor_aberto = areia_escura;
                }
            }else{
                verif_coluna++;
                if(verif_coluna % 2 == 0){
                    celula[linha][coluna].cor_fechado = verde_escuro;
                    celula[linha][coluna].cor_aberto = areia_escura;
                }else{
                    celula[linha][coluna].cor_fechado = verde_claro;
                    celula[linha][coluna].cor_aberto = areia_clara;
                }
            }
        }
    }
}


//Procedimento de sortear bomba
void SortearBomba(){
    int i;
    srand(time(NULL));
    for(i=0; i<28; i++){
        linha = rand() % 15;
        coluna = rand() % 9;
        if (celula[linha][coluna].Bomba == 0){
            celula[linha][coluna].Bomba = 1;
        }else{
            i--;
        }
    }
}

//Prcedimento para cadastrar a quantidade de bombas vizinhas
void VizinhoBomba(){
    int Contador(int linha_cont, int coluna_cont);
    int quantidade=0;
    for (linha=0; linha<16; linha++){
        for (coluna=0; coluna<10; coluna++){
            if (celula[linha][coluna].Bomba == 0){
                quantidade = Contador(linha, coluna);
                celula[linha][coluna].Vizinhos = quantidade;
            }
            quantidade = 0;
        }
    }
}

//Função para contar bombas vizinhas
int Contador(int linha_cont, int coluna_cont){
    int quant = 0;
    int Validez(int linha_val, int coluna_val);
    if (Validez(linha_cont-1, coluna_cont-1) == 1){
        if(celula[linha_cont-1][coluna_cont-1].Bomba == 1){
            quant++;
        }
    }
    if (Validez(linha_cont-1, coluna_cont) == 1){
        if(celula[linha_cont-1][coluna_cont].Bomba == 1){
            quant++;
        }
    }
    if (Validez(linha_cont-1, coluna_cont+1) == 1){
        if(celula[linha_cont-1][coluna_cont+1].Bomba == 1){
            quant++;
        }
    }
    if (Validez(linha_cont, coluna_cont-1) == 1){
        if(celula[linha_cont][coluna_cont-1].Bomba == 1){
            quant++;
        }
    }
    if (Validez(linha_cont, coluna_cont+1) == 1){
        if(celula[linha_cont][coluna_cont+1].Bomba == 1){
            quant++;
        }
    }
    if (Validez(linha_cont+1, coluna_cont-1) == 1){
        if(celula[linha_cont+1][coluna_cont-1].Bomba == 1){
            quant++;
        }
    }
    if (Validez(linha_cont+1, coluna_cont) == 1){
        if(celula[linha_cont+1][coluna_cont].Bomba == 1){
            quant++;
        }
    }
    if (Validez(linha_cont+1, coluna_cont+1) == 1){
        if(celula[linha_cont+1][coluna_cont+1].Bomba == 1){
            quant++;
        }
    }
    return quant;
}

//Função para validar uma celula
int Validez(int linha_val, int coluna_val){
    if (linha_val>=0 && linha_val<16 && coluna_val>=0 && coluna_val<10){
        return 1;
    }else {
        return 0;
    }
}
//Função para verificar onde o mouse clicou
int PegarCoordenada(float x, float y){

    int cont=0;

    //Escolha entre Pá/Bandeira
    if((x >= 140 && x <= 170) &&
       (y >= 15 && y <= 45) &&
       jogando == 1){

        bandeira = 1;

    }
    if((x >= 220 && x <= 250) &&
       (y >= 15 && y <= 45) &&
       jogando == 1){

        bandeira = 0;

    }

    //Colocar bandeira ou abrir uma celula
    if(bandeira == 0 && jogando == 1){
        for(linha=0; linha<16; linha++){
            for(coluna=0; coluna<10; coluna++){
                if((x >= celula[linha][coluna].coord_x && x <= celula[linha][coluna].coord_x + 30)&&
                   (y >= celula[linha][coluna].coord_y && y <= celula[linha][coluna].coord_y + 30)){

                    n_jogada++;

                    if(celula[linha][coluna].Bomba == 1){
                        if(n_jogada == 1){
                            while(celula[linha][coluna].Bomba == 1){
                                Iniciar();
                                PegarCoordenada(x, y);
                                n_jogada--;
                            }
                            celula[linha][coluna].Aberto = 1;
                            return 1;
                        }else if (jogando == 1){
                            celula[linha][coluna].Aberto = 1;
                            return 0;
                        }
                    }
                    if(jogando == 1){
                        celula[linha][coluna].Aberto = 1;

                        if(celula[linha][coluna].Vizinhos == 0){
                            celula[linha][coluna].ZeroUsado = 1;
                            CelulaNula(linha, coluna);
                            AbrirNulos(linha, coluna);
                        }
                    }
                }
            }
        }
    }
    if(bandeira == 1 && jogando == 1){
        for(linha=0; linha<16; linha++){
            for(coluna=0; coluna<10; coluna++){
                if((x >= celula[linha][coluna].coord_x && x <= celula[linha][coluna].coord_x + 30)&&
                   (y >= celula[linha][coluna].coord_y && y <= celula[linha][coluna].coord_y + 30)&&
                   celula[linha][coluna].Aberto == 0){
                    celula[linha][coluna].Marcado += 1;
                    if(celula[linha][coluna].Marcado % 2 != 0){
                        bombas--;
                    }else if(celula[linha][coluna].Marcado % 2 == 0 && celula[linha][coluna].Marcado != 0){
                        bombas++;
                    }
                }
            }
        }
    }

    //Vencer
    if(jogando == 1){
        for(linha=0; linha<16; linha++){
            for(coluna=0; coluna<10; coluna++){
                if(celula[linha][coluna].Aberto == 1){
                    cont++;
                }
                if(celula[linha][coluna].Marcado == 1 && celula[linha][coluna].Bomba == 1){
                    cont++;
                }
            }
        }

        if(cont == 160){
            return 2;
        }else if(jogando == 1){
            return 1;
        }
    }

    //Verificar qual opcao escolhida Perdeu/Ganhou
    if(jogando == 0 || jogando == 2){
        if(x >= 90 && x <= 290 &&
           y >= 348 && y <= 378){
                jogo = 0;
           }
        else if(x >= 90 && x <= 290 &&
           y >= 250 && y <= 280){
                Iniciar();
                bandeira = 0;
                bombas = 28;
                minutos = 0, segundos = 0;
                n_jogada = 0;
                return 1;
           }
        else{
            return 0;
        }
    }
}

//Procedimento para abrir varias celulas nulas em cascata
void AbrirNulos(int linha, int coluna){

    celula[linha][coluna].Aberto = 1;
    if(celula[linha][coluna].Vizinhos == 0){
        celula[linha][coluna].ZeroUsado = 1;
        CelulaNula(linha, coluna);
    }

    if(celula[linha][coluna].Vizinhos == 0){
        if(Validez(linha-1, coluna) == 1){
            if(celula[linha-1][coluna].Vizinhos == 0){
                CelulaNula(linha-1, coluna);
            }
            if(celula[linha-1][coluna].ZeroUsado == 0 && celula[linha-1][coluna].Vizinhos == 0){
                AbrirNulos(linha-1, coluna);
            }
        }
        if(Validez(linha, coluna-1) == 1){
            if(celula[linha][coluna-1].Vizinhos == 0){
                CelulaNula(linha, coluna-1);
            }
            if(celula[linha][coluna-1].ZeroUsado == 0 && celula[linha][coluna-1].Vizinhos == 0){
                AbrirNulos(linha, coluna-1);
            }
        }
        if(Validez(linha, coluna+1) == 1){
            if(celula[linha][coluna+1].Vizinhos == 0){
                CelulaNula(linha, coluna+1);
            }
            if(celula[linha][coluna+1].ZeroUsado == 0 && celula[linha][coluna+1].Vizinhos == 0){
                AbrirNulos(linha, coluna+1);
            }
        }
        if(Validez(linha+1, coluna) == 1){
            if(celula[linha+1][coluna].Vizinhos == 0){
                CelulaNula(linha+1, coluna);
            }
            if(celula[linha+1][coluna].ZeroUsado == 0 && celula[linha+1][coluna].Vizinhos == 0){
                AbrirNulos(linha+1, coluna);
            }
        }

    }
}

//Prcedimento que chama outros procedimentos iniciais
void Iniciar(){

    EsvaziarCelula();
    Coordenadas();
    SortearBomba();
    VizinhoBomba();

}

//Procedimento para abrir celulas nulas ao redor
void CelulaNula(int linha, int coluna){
    int Validez(int linha_val, int coluna_val);

    if(Validez(linha-1, coluna-1) == 1){
        celula[linha-1][coluna-1].Aberto = 1;
    }
    if(Validez(linha-1, coluna) == 1){
        celula[linha-1][coluna].Aberto = 1;
    }
    if(Validez(linha-1, coluna+1) == 1){
        celula[linha-1][coluna+1].Aberto = 1;
    }
    if(Validez(linha, coluna-1) == 1){
        celula[linha][coluna-1].Aberto = 1;
    }
    if(Validez(linha, coluna+1) == 1){
        celula[linha][coluna+1].Aberto = 1;
    }
    if(Validez(linha+1, coluna-1) == 1){
        celula[linha+1][coluna-1].Aberto = 1;
    }
    if(Validez(linha+1, coluna) == 1){
        celula[linha+1][coluna].Aberto = 1;
    }
    if(Validez(linha+1, coluna+1) == 1){
        celula[linha+1][coluna+1].Aberto = 1;
    }
}

//Procedimeto para desenhar margem superior
void draw_margem_sup(int w, int x, int y, int z){

    al_draw_line(w, y, x, y, al_map_rgb(0,0,0), 3);
    al_draw_line(w, z, x, z, al_map_rgb(0,0,0), 3);
    al_draw_line(w, y, w, z, al_map_rgb(0,0,0), 3);
    al_draw_line(x, y, x, z, al_map_rgb(0,0,0), 3);

}

int main(){

    ALLEGRO_DISPLAY *display = NULL;//Variavel que representa o display
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;//Variavel que representa a fila de eventos
    ALLEGRO_TIMER *timer = NULL;//Variavel que representa o time
    ALLEGRO_FONT *fonte = NULL;//Variavel que representa a fonte
    ALLEGRO_FONT *fonte_relogio = NULL;
    ALLEGRO_FONT *fonte_menssagem = NULL;
    ALLEGRO_BITMAP *imagem_bomba = NULL;//Variavel que representa a imagem da bomba
    ALLEGRO_BITMAP *imagem_bandeira = NULL;
    ALLEGRO_BITMAP *imagem_pa = NULL;
    ALLEGRO_BITMAP *imagem_relogio = NULL;
    ALLEGRO_BITMAP *img_etiqueta_perder = NULL;
    ALLEGRO_BITMAP *img_etiqueta_ganhar = NULL;

    //Rotinas de inicialização---------------

    //Iniciar o allegro
    if(!al_init()){
        fprintf(stderr, "Erro ao iniciar o allegro\n");
        return -1;
    }

    //Criar tela com as dimensões ja feitas
    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display){
        fprintf(stderr, "Erro ao criar a tela\n");
        return -1;
    }

    //Cria um temporizador que incrementa a cada 1.0/FPS sgundos
    timer = al_create_timer(1.0/FPS);
    if(!timer){
        fprintf(stderr, "Erro ao criar um timer\n");
        return -1;
    }

    //inicializa o modulo de primitivas do allegro
    if(!al_init_primitives_addon()){
        fprintf(stderr, "Failed to initialize primitives\n");
        return -1;
    }

    //Criar fila de eventos
    event_queue = al_create_event_queue();
    if(!event_queue){
        fprintf(stderr, "Failed to create event queue\n");
        al_destroy_display(display);
        return -1;
    }

    //Instala o teclado
    if(!al_install_keyboard()){
        fprintf(stderr, "Failed to install keyboard\n");
        return -1;
    }

    //Instala o mouse
    if(!al_install_mouse()){
        fprintf(stderr, "Failed to install mouse\n");
        return -1;
    }

    //Registra na fila os eventos de tela
    al_register_event_source(event_queue, al_get_display_event_source(display));
    //Registra na fila os eventos do teclado
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    //Registra na fila os eventos do mouse
    al_register_event_source(event_queue, al_get_mouse_event_source());
    //Registra na fila de eventos o time
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    //Inicialização de add-on para usar fontes
    al_init_font_addon();

    //Inicialização do add-on para uso de fontes True Type
    al_init_ttf_addon();

    //Carregar a fonte
    fonte = al_load_font("Pixellari.ttf", 26, NULL);
    fonte_relogio = al_load_font("Pixellari.ttf", 21, NULL);
    fonte_menssagem = al_load_font("Pixellari.ttf", 35, NULL);

    //Inicialização do add-on para utilizar imagem
    al_init_image_addon();

    //Carregar a imagem
    imagem_bomba = al_load_bitmap("bombear.png");
    imagem_bandeira = al_load_bitmap("bandeira.png");
    imagem_pa = al_load_bitmap("pa.png");
    imagem_relogio = al_load_bitmap("cronometro.png");
    img_etiqueta_perder = al_load_bitmap("etiqueta.png");
    img_etiqueta_ganhar = al_load_bitmap("etiqueta_win.png");

    //------------------------------------------

    Iniciar();

    //inicia temporizador
    al_start_timer(timer);

    while(jogo){

        //Capturar o evento
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        //Se o tipo de evento for um clique do mouse
        if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            jogando = PegarCoordenada(ev.mouse.x, ev.mouse.y);
            printf("\nCoordenada: %d %d\n", ev.mouse.x, ev.mouse.y);
        }
        //Se o cursor passar em cima das opções (vencer/perder)
        else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){
            pos_x = ev.mouse.x;
            pos_y = ev.mouse.y;
        }
        //Se o tipo do evento for um temporizador, ou seja, se o tempo passou de t para t+1
        else if(ev.type == ALLEGRO_EVENT_TIMER){
                draw_scenario();

                for(linha=0; linha<16; linha++){
                    for(coluna=0; coluna<10; coluna++){
                        if(celula[linha][coluna].Aberto == 1 && celula[linha][coluna].Bomba == 0){
                            if(celula[linha][coluna].Vizinhos == 1){
                                al_draw_textf(fonte, al_map_rgb(27, 117, 207), celula[linha][coluna].coord_x + 8, celula[linha][coluna].coord_y + 5, NULL, "%d", celula[linha][coluna].Vizinhos);
                            }
                            if(celula[linha][coluna].Vizinhos == 2){
                                al_draw_textf(fonte, al_map_rgb(55, 141, 60), celula[linha][coluna].coord_x + 8, celula[linha][coluna].coord_y + 5, NULL, "%d", celula[linha][coluna].Vizinhos);
                            }
                            if(celula[linha][coluna].Vizinhos == 3){
                                al_draw_textf(fonte, al_map_rgb(205, 49, 46), celula[linha][coluna].coord_x + 8, celula[linha][coluna].coord_y + 5, NULL, "%d", celula[linha][coluna].Vizinhos);
                            }
                            if(celula[linha][coluna].Vizinhos == 4){
                                al_draw_textf(fonte, al_map_rgb(181, 72, 241), celula[linha][coluna].coord_x + 8, celula[linha][coluna].coord_y + 5, NULL, "%d", celula[linha][coluna].Vizinhos);
                            }
                            if(celula[linha][coluna].Vizinhos == 5){
                                al_draw_textf(fonte, al_map_rgb(241, 156, 50), celula[linha][coluna].coord_x + 8, celula[linha][coluna].coord_y + 5, NULL, "%d", celula[linha][coluna].Vizinhos);
                            }
                            if(celula[linha][coluna].Vizinhos == 6){
                                al_draw_textf(fonte, al_map_rgb(73, 164, 164), celula[linha][coluna].coord_x + 8, celula[linha][coluna].coord_y + 5, NULL, "%d", celula[linha][coluna].Vizinhos);
                            }
                            if(celula[linha][coluna].Vizinhos == 7){
                                al_draw_textf(fonte, al_map_rgb(67, 47, 43), celula[linha][coluna].coord_x + 8, celula[linha][coluna].coord_y + 5, NULL, "%d", celula[linha][coluna].Vizinhos);
                            }
                        }
                        if(celula[linha][coluna].Bomba == 1 && celula[linha][coluna].Aberto == 1){
                            al_draw_bitmap(imagem_bomba, celula[linha][coluna].coord_x + 3, celula[linha][coluna].coord_y + 3, 0);
                        }
                    }
                }
                for(linha=0; linha<16; linha++){
                    for(coluna=0; coluna<10; coluna++){
                        if(celula[linha][coluna].Marcado != 0){
                                if(celula[linha][coluna].Marcado % 2 != 0 && celula[linha][coluna].Aberto == 0){
                                    al_draw_bitmap(imagem_bandeira, celula[linha][coluna].coord_x + 3, celula[linha][coluna].coord_y + 3, 0);
                                }else{
                                    if(celula[linha][coluna].Aberto == 0){
                                        al_draw_filled_rectangle(celula[linha][coluna].coord_x, celula[linha][coluna].coord_y,
                                                                celula[linha][coluna].coord_x + 30, celula[linha][coluna].coord_y + 30,
                                                                celula[linha][coluna].cor_fechado);
                                    }
                                    if(celula[linha][coluna].Aberto == 1){
                                        al_draw_filled_rectangle(celula[linha][coluna].coord_x, celula[linha][coluna].coord_y,
                                                                celula[linha][coluna].coord_x + 30, celula[linha][coluna].coord_y + 30,
                                                                celula[linha][coluna].cor_aberto);
                                    }
                                    celula[linha][coluna].Marcado = 0;
                            }
                        }
                    }
                }

                //Interface de informações adicionais
                draw_margem_sup(20, 90, 15, 45);
                al_draw_filled_rectangle(20, 15,
                                 90, 45,
                                 al_map_rgb(74, 110, 45));
                al_draw_bitmap(imagem_bomba, 22, 18, 0);
                if(bombas >= 0){
                    al_draw_textf(fonte, al_map_rgb(255, 255, 255), 56, 20, NULL, "%0.2d", bombas);
                }else{
                    al_draw_textf(fonte, al_map_rgb(241, 50, 52), 47, 20, NULL, "%0.2d", bombas);
                }

                if(bandeira == 1){
                    draw_margem_sup(140, 170, 15, 45);
                    al_draw_filled_rectangle(140, 15,
                                     170, 45,
                                     al_map_rgb(76, 149, 55));

                    al_draw_bitmap(imagem_bandeira, 142, 20, 0);

                    draw_margem_sup(220, 250, 15, 45);
                    al_draw_filled_rectangle(220, 15,
                                     250, 45,
                                     al_map_rgb(28, 100, 28));
                    al_draw_bitmap(imagem_pa, 222, 20, 0);
                }
                if(bandeira == 0){
                    draw_margem_sup(140, 170, 15, 45);
                    al_draw_filled_rectangle(140, 15,
                                     170, 45,
                                     al_map_rgb(28, 100, 28));

                    al_draw_bitmap(imagem_bandeira, 142, 20, 0);

                    draw_margem_sup(220, 250, 15, 45);
                    al_draw_filled_rectangle(220, 15,
                                     250, 45,
                                     al_map_rgb(76, 149, 55));
                    al_draw_bitmap(imagem_pa, 222, 20, 0);
                }

                if(al_get_timer_count(timer)%(int)FPS == 0 && jogando == 1){
                   if(segundos < 60){
                    segundos++;
                   }else{
                    segundos = 0;
                    minutos++;
                   }
                }

                draw_margem_sup(290, 360, 15, 45);
                al_draw_filled_rectangle(290, 15,
                                 360, 45,
                                 al_map_rgb(74, 110, 45));
                al_draw_bitmap(imagem_relogio, 291, 18, 0);
                al_draw_textf(fonte_relogio, al_map_rgb(255, 255, 255), 318, 22, NULL, "%d:%0.2d", minutos, segundos);

            //Interfaces para ganhar/perder
            if(jogando == 0){
                al_draw_filled_rectangle(88, 192, 292, 402, al_map_rgb(181, 72, 240));
                al_draw_filled_rectangle(90, 200, 290, 400, al_map_rgb(74,47,102));

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 116, 254, NULL, "NOVO JOGO");
                if(pos_x >= 90 && pos_x <=290  && pos_y >= 254 && pos_y <= 280){
                    al_draw_filled_rectangle(90, 250, 290, 280, al_map_rgb(255, 255, 255));
                    al_draw_text(fonte, al_map_rgb(0, 0, 0), 116, 254, NULL, "NOVO JOGO");
                }

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 156, 303, NULL, "MENU");
                if(pos_x >= 90 && pos_x <=290  && pos_y >= 303 && pos_y <= 329){
                    al_draw_filled_rectangle(90, 300, 290, 330, al_map_rgb(255, 255, 255));
                    al_draw_text(fonte, al_map_rgb(0, 0, 0), 156, 303, NULL, "MENU");
                }

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 162, 352, NULL, "SAIR");
                if(pos_x >= 90 && pos_x <=290  && pos_y >= 352 && pos_y <= 378){
                    al_draw_filled_rectangle(90, 348, 290, 378, al_map_rgb(255, 255, 255));
                    al_draw_text(fonte, al_map_rgb(0, 0, 0), 162, 352, NULL, "SAIR");
                }

                al_draw_bitmap(img_etiqueta_perder, 62, 70, 0);
                al_draw_text(fonte_menssagem, al_map_rgb(0, 0, 0), 92, 168, NULL, "GAME OVER");
                al_draw_text(fonte_menssagem, al_map_rgb(255, 255, 255), 94, 167, NULL, "GAME OVER");
            }
            if(jogando == 2){
                al_draw_filled_rectangle(88, 192, 292, 402, al_map_rgb(219, 50, 53));
                al_draw_filled_rectangle(90, 200, 290, 400, al_map_rgb(141, 33, 36));

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 116, 254, NULL, "NOVO JOGO");
                if(pos_x >= 90 && pos_x <=290  && pos_y >= 254 && pos_y <= 280){
                    al_draw_filled_rectangle(90, 250, 290, 280, al_map_rgb(255, 255, 255));
                    al_draw_text(fonte, al_map_rgb(0, 0, 0), 116, 254, NULL, "NOVO JOGO");
                }

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 156, 303, NULL, "MENU");
                if(pos_x >= 90 && pos_x <=290  && pos_y >= 303 && pos_y <= 329){
                    al_draw_filled_rectangle(90, 300, 290, 330, al_map_rgb(255, 255, 255));
                    al_draw_text(fonte, al_map_rgb(0, 0, 0), 156, 303, NULL, "MENU");
                }

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 162, 352, NULL, "SAIR");
                if(pos_x >= 90 && pos_x <=290  && pos_y >= 352 && pos_y <= 378){
                    al_draw_filled_rectangle(90, 348, 290, 378, al_map_rgb(255, 255, 255));
                    al_draw_text(fonte, al_map_rgb(0, 0, 0), 162, 352, NULL, "SAIR");
                }

                al_draw_bitmap(img_etiqueta_ganhar, 62, 70, 0);
                al_draw_text(fonte_menssagem, al_map_rgb(0, 0, 0), 116, 168, NULL, "VITORIA");
                al_draw_text(fonte_menssagem, al_map_rgb(255, 255, 255), 118, 167, NULL, "VITORIA");
            }

            //Atualiza a tela (quando houver algo para mostrar)
            al_flip_display();

        }
        //Se o tipo do evento for o fechamento de tela
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            jogo = 0;
        }
    }

    return 0;
}
