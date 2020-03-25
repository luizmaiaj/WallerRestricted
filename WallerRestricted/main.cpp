// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

//INCLUSAO DE BIBLIOTECAS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define gens 51                    //NUMERO DE GERACOES
#define pop 500                    //TAMANHO DA POPULACAO
#define pop_70 350                 //70% DA POPULACAO
#define pop_30 150                 //30% DA POPULACAO
#define runs 5                     //NUMERO DE EXECUCOES
#define limit 120                  //LIMITA COMPRIMENTO DO INDIVIDUO
#define run_steps 200              //NUMERO DE PASSOS DA ARVORE

/*
  -------------------------------------------------------------
  BASEADO EM ALOCACAO DINAMICA DE MEMORIA BUSCA NODE LEFT-RIGHT

  SALVA UM CAMINHO POR GERACAO, OS CEM MELHORES INDIVIDUOS DA
  ULTIMA GERACAO E OS DADOS SOBRE A EVOLUCAO DOS MESMOS.
  -------------------------------------------------------------

  INDIVIDUO DEVE PERCORRER A MATRIZ SEMPRE PROXIMO A PAREDE (UMA COLUNA/LINHA)
  E COMPLETAR SUA VOLTA EM NO MAXIMO 200 PASSOS.
  SAO CONSIDERADOS PASSOS OS TERMINAIS. O FITNESS MAXIMO SERA 60 POR EXECUCAO.

  FUNCOES  :
    PROGN3 (3), EXECUTA TRES RAMOS EM SEQUENCIA;
    PROGN2 (2), EXECUTA DOIS RAMOS EM SEQUENCIA;
    IFWALL (I), EXECUTA RAMO ESQUERDO SE NAO HOUVER PAREDE A FRENTE E DIREITO SE HOUVER.

  TERMINAIS:
    WALKFRONT (F), FAZ ROBO ANDAR PRA FRENTE;
    WALKBACK  (B), FAZ ROBO ANDAR PRA TRAS;
    RIGHT     (R), FAZ ROBO VIRAR 90o A DIREITA;
    LEFT      (L), FAZ ROBO VIRAR 90o A ESQUERDA.

  NUMERO DE GERACOES: G = geracoes.
  TAMANHO DA POPULACAO: M = populacao.

  PROBABILIDADE DE REPRODUCAO = 30%  <- MELHORES INDIVIDUOS
  PROBABILIDADE DE CRUZAMENTO = 70%  <- MELHORES INDIVIDUOS
  PROBABILIDADE DE MUTACAO    =  0%  <- PIORES   INDIVIDUOS

  OUTROS DADOS:
    COMPLEXIDADE DOS INDIVIDUOS E LIMITADA.
*/


//ESTRUTURA UTILIZADA EM CADA FOLHA DO INDIVIDUO
struct tree {
    char info;           //GUARDA UM CARACTERE REFERENTE A UM TERMINAL OU FUNCAO
    struct tree* top;
    struct tree* right;
    struct tree* center; //PONTEIROS PARA 'STRUCT TREE'
    struct tree* left;
};

//ESTRUTURA BASE DO INDIVIDUO OU RAIZ
struct ind {
    int fitness;

    struct tree* inicio;
};

//CONTROLA O NUMERO DE SORTEIOS DO INDIVIDUO
int n;

//DIRECAO, LINHA E COLUNA ATUAIS DO ROBO
int direcao, robolin, robocol;

//MATRIZES
int enviro[20][20],  //REFERENCIA DE MOVIMENTACAO
way[20][20],       //REFERENCIA DO CAMINHO IDEAL
steps[20][20][2];  //ARMAZENA PASSOS DO INDIVIDUO

int fit,             //GUARDA FITNESS ENQUANTO O INDIVIDUO E EXECUTADO
pos;               //GUARDA POSICAO DO RAMO DE TROCA

//RECEBE RETORNO DE POSICAO DE CRUZAMENTO
struct tree* ptr;

//PONTEIRO PARA ARQUIVO
FILE* gene, * img;

//GUARDA INDIVIDUO ANTES DE ESCREVE-LO PARA ARQUIVO
char texto[2000];

/*
   FITNESS

   RETORNA OU NAO PONTO DE FITNESS
*/
int fitness(robolin, robocol)
{
    if (way[robolin][robocol] == 1)
    {
        way[robolin][robocol] = 0;
        return(1);
    }

    else return(0);

}

/*
   FREEMEM

   LIBERA MEMORIA DE UMA 'STRUCT TREE'
*/
void freemem(struct tree* pointer)
{
    if (pointer->left)
        freemem(pointer->left);

    if (pointer->center)
        freemem(pointer->center);

    if (pointer->right)
        freemem(pointer->right);

    free(pointer);
}

/*
   IFWALL

   SE TIVER PAREDE (SOMENTE) EM FRENTE RETORNA 1
*/
int ifwall(int direcao, int robolin, int robocol)
{
    switch (direcao)
    {
    case 1:
        if (enviro[robolin][robocol + 2] == 1 ||
            enviro[robolin][robocol + 1] == 1)
            return(1);

        else
            return(0);
        break;

    case 2:
        if (enviro[robolin + 2][robocol] == 1 ||
            enviro[robolin + 1][robocol] == 1)
            return(1);

        else
            return(0);
        break;

    case 3:
        if (enviro[robolin][robocol - 2] == 1 ||
            enviro[robolin][robocol - 1] == 1)
            return(1);

        else
            return(0);
        break;

    case 4:
        if (enviro[robolin - 2][robocol] == 1 ||
            enviro[robolin - 1][robocol])
            return(1);

        else
            return(0);
        break;
    }
}

/*
   WALKFRONT

   FAZ ROBO ANDAR PARA FRENTE
*/
int walkfront(int direcao, int robolin, int robocol)
{
    switch (direcao)
    {
    case 1:
        if (enviro[robolin][robocol + 1] == 0)
            robocol++;
        return(robocol);
        break;

    case 2:
        if (enviro[robolin + 1][robocol] == 0)
            robolin++;
        return(robolin);
        break;

    case 3:
        if (enviro[robolin][robocol - 1] == 0)
            robocol--;
        return(robocol);
        break;

    case 4:
        if (enviro[robolin - 1][robocol] == 0)
            robolin--;
        return(robolin);
        break;
    }
}

/*
   WALKBACK

   FAZ ROBO ANDAR PARA TRAS
*/
int walkback(int direcao, int robolin, int robocol)
{
    switch (direcao)
    {
    case 1:
        if (enviro[robolin][robocol - 1] == 0)
            robocol--;
        return(robocol);
        break;

    case 2:
        if (enviro[robolin - 1][robocol] == 0)
            robolin--;
        return(robolin);
        break;

    case 3:
        if (enviro[robolin][robocol + 1] == 0)
            robocol++;
        return(robocol);
        break;

    case 4:
        if (enviro[robolin + 1][robocol] == 0)
            robolin++;
        return(robolin);
        break;
    }
}

/*
   LEFT

   FAZ ROBO VIRAR 90o A ESQUERDA
*/
int left(int direcao)
{
    switch (direcao)
    {
    case 1:
        direcao = 4;
        break;

    case 2:
        direcao = 1;
        break;

    case 3:
        direcao = 2;
        break;

    case 4:
        direcao = 3;
        break;
    }

    return(direcao);
}

/*
   RIGHT

   FAZ ROBO VIRAR 90o A DIREITA
*/
int right(int direcao)
{
    switch (direcao)
    {
    case 1:
        direcao = 2;
        break;

    case 2:
        direcao = 3;
        break;

    case 3:
        direcao = 4;
        break;

    case 4:
        direcao = 1;
        break;
    }

    return(direcao);
}

/*
   ALLOC

   ALOCA MEMORIA PARA UMA ESTRUTURA TREE E RETORNA PONTEIRO PARA ESSA ALOCACAO
*/
struct tree* alloc(void)
{
    struct tree* pointer;

    pointer = (struct tree*) malloc(sizeof(struct tree));
    if (!pointer)
    {
        printf("\n\nFalha na alocacao de memoria!!!\n\n");
        exit(1);
    }

    else return(pointer);
}

/*
   SORT

   RECEBE PONTEIRO E SORTEIA INFO
*/
struct tree* sort(struct tree* pointer)
{
    int sorteio;    //GUARDA NUMERO SORTEADO
    struct tree* aux;  //AUXILIAR DE INICIALIZACAO DO PONTEIRO

    aux = alloc();
    aux->top = NULL;
    aux->left = NULL;
    aux->center = NULL;
    aux->right = NULL;

    if (n == 1)
        sorteio = 1;  //NA PRIMEIRA EXECUCAO SORTEIA `PROGN3`

    else if (n < limit)
        sorteio = (rand() % 7) + 1;  //sorteio RECEBE NUMERO DE 1 a 7

    else sorteio = (rand() % 4) + 4;  //SE EXCEDER 300 SORTEIOS PASSA A SORTEAR SOMENTE TERMINAIS

    n++;  //GUARDA COMPLEXIDADE DO INDIVIDUO

    switch (sorteio)
    {
    case 1:
        pointer->info = 51;  //GUARDA `3` EM INFO REFERENTE A `PROGN3`
        break;
    case 2:
        pointer->info = 50;  //GUARDA `2` EM INFO REFERENTE A `PROGN2`
        break;
    case 3:
        pointer->info = 73;  //GUARDA `I` EM INFO REFERENTE A `IFWALL`
        break;
    case 4:
        pointer->info = 70;  //GUARDA `F` EM INFO REFERENTE A `WALKFRONT`
        break;
    case 5:
        pointer->info = 66;  //GUARDA `B` EM INFO REFERENTE A `WALKBACK`
        break;
    case 6:
        pointer->info = 76;  //GUARDA `L` EM INFO REFERENTE A `LEFT`
        break;
    case 7:
        pointer->info = 82;  //GUARDA `R` EM INFO REFERENTE A `RIGHT`
        break;
    }

    if (sorteio == 4 || sorteio == 5 || sorteio == 6 || sorteio == 7)
    {
        pointer->left = NULL;
        pointer->center = NULL;
        pointer->right = NULL;
    }

    else if (sorteio == 3 || sorteio == 2)
    {
        pointer->left = aux;
        pointer->center = NULL;
        pointer->right = aux;
    }

    else
    {
        pointer->left = aux;
        pointer->center = aux;
        pointer->right = aux;
    }

    freemem(aux);

    return(pointer);
}

/*
   MAKETREE

   RECEBE PONTEIRO E CRIA INDIVIDUO
*/
struct tree* maketree(struct tree* pointer)
{
    struct tree* aux;

    pointer = sort(pointer);  //RECEBE INFORMACAO SORTEADA

    if (pointer->left)  //CHECA VALIDADE DO PONTEIRO
    {
        aux = alloc();        //ALOCA aux

        pointer->left = aux;  //left APONTA aux

        aux->top = pointer;   //top APONTA pointer

        aux = maketree(aux);  //CONTINUA ARVORE
    }

    if (pointer->center)  //CHECA VALIDADE DO PONTEIRO
    {
        aux = alloc();          //ALOCA aux

        pointer->center = aux;  //center APONTA aux

        aux->top = pointer;     //top APONTA pointer

        aux = maketree(aux);    //CONTINUA ARVORE
    }

    if (pointer->right)  //CHECA VALIDADE DO PONTEIRO
    {
        aux = alloc();         //ALOCA aux

        pointer->right = aux;  //right APONTA aux

        aux->top = pointer;    //top APONTA pointer

        aux = maketree(aux);   //CONTINUA ARVORE
    }

    return(pointer);
}

/*
   COPY

   RECEBE DOIS PONTEIROS E FAZ UMA COPIA DO SEGUNDO NO PRIMEIRO
*/
struct tree* copy(struct tree* pointer1, struct tree* pointer2)
{
    struct tree* aux;

    pointer2->info = pointer1->info;

    if (pointer1->left)
    {
        aux = alloc();

        pointer2->left = aux;

        aux->top = pointer2;

        copy(pointer1->left, aux);
    }

    if (pointer1->center)
    {
        aux = alloc();

        pointer2->center = aux;

        aux->top = pointer2;

        copy(pointer1->center, aux);
    }

    if (pointer1->right)
    {
        aux = alloc();

        pointer2->right = aux;

        aux->top = pointer2;

        copy(pointer1->right, aux);
    }

    return(pointer2);
}

/*
   PRINT

   IMPRIME INDIVIDUO
*/
void print(struct tree* pointer)
{

    printf("%c ", pointer->info);

    if (pointer->left)
        print(pointer->left);

    if (pointer->center)
        print(pointer->center);

    if (pointer->right)
        print(pointer->right);
}

/*
   EXECUTE

   EXECUTA INDIVIDUO
*/
void execute(struct tree* pointer)
{

    switch (pointer->info)
    {
    case 51:                    //PROGN3
        execute(pointer->left);   //CHAMA RAMO ESQUERDO

        execute(pointer->center); //CHAMA RAMO CENTRAL

        execute(pointer->right);  //CHAMA RAMO DIREITO
        break;

    case 50:                    //PROGN2
        execute(pointer->left);

        execute(pointer->right);
        break;

    case 73:                   //IFWALL
        if (ifwall(direcao, robolin, robocol) == 0)
            execute(pointer->left);

        else
            execute(pointer->right);
        break;

    case 70:                   //WALKFRONT
        if (direcao == 1 || direcao == 3)
            robocol = walkfront(direcao, robolin, robocol);

        else robolin = walkfront(direcao, robolin, robocol);

        fit += fitness(robolin, robocol);

        steps[robolin][robocol][0] = n;

        steps[robolin][robocol][1] = direcao;

        n++;
        break;

    case 66:                   //WALKBACK
        if (direcao == 1 || direcao == 3)
            robocol = walkback(direcao, robolin, robocol);

        else robolin = walkback(direcao, robolin, robocol);

        fit += fitness(robolin, robocol);

        steps[robolin][robocol][0] = n;

        steps[robolin][robocol][1] = direcao;

        n++;
        break;

    case 76:                   //LEFT
        direcao = left(direcao);

        steps[robolin][robocol][0] = n;

        steps[robolin][robocol][1] = direcao;

        n++;
        break;

    case 82:                   //RIGHT
        direcao = right(direcao);

        steps[robolin][robocol][0] = n;

        steps[robolin][robocol][1] = direcao;

        n++;
        break;
    }
}

/*
   SETWAY

   PREENCHE CAMINHO IDEAL
*/
void setway(void)
{
    int linha, colun;

    for (linha = 0; linha < 20; linha++)           //PREENCHE LATERAL 
        for (colun = 0; colun < 20; colun++)
        {
            if (linha == 0 || linha == 19 || colun == 0 || colun == 19)
                way[linha][colun] = 111;

            else
                way[linha][colun] = 0;

            if (linha < 6 && colun > 13)       //PAREDE INTERNA
                way[linha][colun] = 111;

            if (linha > 13 && colun < 6)        //PAREDE INTERNA
                way[linha][colun] = 111;

            switch (linha)                      //CAMINHO HORIZONTAL
            {
            case 2:
                if (colun > 1 && colun < 13)
                    way[linha][colun] = 1;
                break;

            case 7:
                if (colun > 11 && colun < 18)
                    way[linha][colun] = 1;
                break;

            case 12:
                if (colun > 1 && colun < 8)
                    way[linha][colun] = 1;
                break;

            case 17:
                if (colun > 6 && colun < 18)
                    way[linha][colun] = 1;
                break;
            }

            switch (colun)                       //CAMINHO VERTICAL
            {
            case 2:
                if (linha > 2 && linha < 12)
                    way[linha][colun] = 1;
                break;

            case 7:
                if (linha > 12 && linha < 17)
                    way[linha][colun] = 1;
                break;

            case 12:
                if (linha > 2 && linha < 7)
                    way[linha][colun] = 1;
                break;

            case 17:
                if (linha > 7 && linha < 17)
                    way[linha][colun] = 1;
                break;
            }
        }
}

/*
   SETEMVIRO

   MATRIZ AMBIENTE
*/
void setenviro(void)
{
    int linha, colun;

    for (linha = 0; linha < 20; linha++)           //PREENCHE LATERAL 
        for (colun = 0; colun < 20; colun++)
        {
            if (linha == 0 || linha == 19 || colun == 0 || colun == 19)
                enviro[linha][colun] = 1;

            else
                enviro[linha][colun] = 0;

            if (linha < 6 && colun > 13)       //PAREDE INTERNA INFERIOR ESQUERDA
                enviro[linha][colun] = 1;

            if (linha > 13 && colun < 6)        //PAREDE INTERNA SUPERIOR DIREITA
                enviro[linha][colun] = 1;
        }
}

/*
   SETBEST

   IMAGEM DO CAMINHO
*/
void setbest(unsigned char matriz[20][20][3])
{
    int linha, colun, R = 0, G = 1, B = 2;

    for (linha = 0; linha < 20; linha++)           //PREENCHE LATERAL 
        for (colun = 0; colun < 20; colun++)
        {
            if (linha == 0 || linha == 19 || colun == 0 || colun == 19)
                matriz[linha][colun][R] = matriz[linha][colun][G] = matriz[linha][colun][B] = 0;

            else
                matriz[linha][colun][R] = matriz[linha][colun][G] = matriz[linha][colun][B] = 255;

            if (linha < 6 && colun > 13)       //PAREDE INTERNA SUPERIOR DIREITA
                matriz[linha][colun][R] = matriz[linha][colun][G] = matriz[linha][colun][B] = 0;

            if (linha > 13 && colun < 6)        //PAREDE INTERNA INFERIOR ESQUERDA
                matriz[linha][colun][R] = matriz[linha][colun][G] = matriz[linha][colun][B] = 0;
        }
}

/*
   LENGTH

   MEDE COMPLEXIDADE DO INDIVIDUO
*/
int length(struct tree* pointer)
{

    if (pointer->left)
    {
        length(pointer->left);
        n++;
    }

    if (pointer->center)
    {
        length(pointer->center);
        n++;
    }

    if (pointer->right)
    {
        length(pointer->right);
        n++;
    }

    return(n);

}

/*
   COUNT

   RETORNA PONTEIRO PARA POSICAO DE CRUZAMENTO
*/
struct tree* count(struct tree* pointer)
{
    if (pointer->left)
    {
        n--;

        if (n > 0)
            count(pointer->left);

        else if (n == 0)
        {
            pos = 0;

            ptr = pointer->left;
        }
    }

    if (pointer->center)
    {
        n--;

        if (n > 0)
            count(pointer->center);

        else if (n == 0)
        {
            pos = 1;

            ptr = pointer->center;
        }
    }

    if (pointer->right)
    {
        n--;

        if (n > 0)
            count(pointer->right);

        else if (n == 0)
        {
            pos = 2;

            ptr = pointer->right;
        }
    }
}

/*
   SALVAR

   SALVA INDIVIDUOS EM DISCO
*/
void salvar(int fitn, int m, int pontos)
{
    char arquivo[20];
    int comp;

    sprintf(arquivo, "robo%d.f%.3d", m, fitn);

    if ((gene = fopen(arquivo, "w+")) != NULL)
    {
        comp = strlen(texto);
        fwrite(texto, sizeof(char), comp, gene);
        fprintf(gene, "\n\nPONTOS DE COMPLEXIDADE = %d", pontos);
        fclose(gene);
    }
}

/*
   PRINTSTR

   GUARDA INDIVIDUO EM UMA STRING
*/
void printstr(struct tree* pointer)
{

    texto[n] = pointer->info;
    n++;

    if (pointer->left)
        printstr(pointer->left);

    if (pointer->center)
        printstr(pointer->center);

    if (pointer->right)
        printstr(pointer->right);

}

//**********************
//* PROGRAMA PRINCIPAL *
//**********************
void main(void)
{
    struct ind individuo[pop],  //PONTEIRO INICIAL DO INDIVIDUO
        individuotemp[pop_70];

    int stime;    //REPASSA A HORA PARA SEMENTE DO RANDOM
    long ltime;   //RECEBE A HORA PARA RANDOM

    int linha, colun;  //AUXILIARES DE IMPRESSAO DA MATRIZ

    int sorteio, best;  //AUXILIAR DE SORTEIO, MELHOR FITNESS DA GERACAO

    int i, j, k, gap, list[pop_70],  //LOOPING E AUXILIAR DE SORTEIO
        crosspoint[2],             //PONTOS DE CRUZAMENTO
        postree[2];                //POSICAO DO RAMO

    struct ind x, pai[2];  //AUXILIARES DE ORGANIZACAO E CRUZAMENTO

    char a[5]; //AUXILIAR DE ORGANIZACAO           

    struct tree* pointer[4];  //GUARDA QUATRO PONTOS DE CRUZAMENTO

    int geracao, nsort, num;  //GERACAO ATUAL, NUMERO DE INDIVIDUOS A SEREM SORTEADOS E CONTROLE DE CRUZAMENTOS

    float media_geracao;  //MEDIA DE FITNESS DA GERACAO

    int maior_geracao;    //MAIOR FITNESS DA GERACAO

    unsigned char best_way[20][20][3];

    struct bests
    {
        unsigned char matriz[20][20][3];  //MELHOR CAMINHO
    }caminho[gens];

    //SEMENTE DO RANDOM
    ltime = time(NULL);
    stime = (unsigned)ltime / 2;
    srand(stime);

    //ALOCA INICIO PARA TODOS OS INDIVIDUOS E SORTEIA OS INDIVIDUOS
    printf("\nAlocando e Sorteando\n");

    for (i = 0; i < pop; i++)
    {
        individuo[i].inicio = alloc();    //ALOCA
        individuo[i].inicio->top = NULL;
        individuo[i].fitness = 0;

        n = 1;                            //SORTEIA
        individuo[i].inicio = maketree(individuo[i].inicio);

        if (i < pop_70)
        {
            individuotemp[i].inicio = alloc();
            individuotemp[i].fitness = 0;
        }
    }

    if ((gene = fopen("dados.txt", "w+")) == NULL)
        exit(1);
    else
    {
        fprintf(gene, "Luiz Carlos Maia Junior\nRobo Seguidor de Paredes Versao 0.137\n");
        fprintf(gene, "Caracteristicas:\n Geracoes %d, Execucoes %d\n", gens, runs);
        fprintf(gene, "Limite %d, Passos %d\n", limit, run_steps);
        fprintf(gene, "GERACAO\tMEDIA\t\tMAIOR\n");
    }

    setenviro();

    for (geracao = 0; geracao < gens; geracao++)
    {
        media_geracao = maior_geracao = best = 0;

        setbest(best_way);

        //EXECUCAO DO INDIVIDUO
        printf("\nExecutando\n");

        for (i = 0; i < pop; i++)
        {
            for (j = 0; j < runs; j++)
            {
                setway();  //CAMINHO IDEAL

                for (linha = 0; linha < 20; linha++)
                    for (colun = 0; colun < 20; colun++)
                        steps[linha][colun][0] = steps[linha][colun][1] = 0;

                fit = 0;

                direcao = (rand() % 4) + 1;     //SORTEIO DA DIRECAO

                robocol = (rand() % 18) + 1;    //SORTEIO DA COLUNA

                if (robocol < 6)                 //SORTEIO DA LINHA
                    robolin = (rand() % 13) + 1;

                else if (robocol > 13)
                    robolin = (rand() % 13) + 6;

                else robolin = (rand() % 18) + 1;

                for (n = 0; n < 200;)
                    execute(individuo[i].inicio);

                individuo[i].fitness += fit;

                if (fit >= best)
                {
                    unsigned char green = 255, blue = 255, yellow = 255, red = 255;

                    setbest(best_way);

                    best = fit;

                    for (linha = 0; linha < 20; linha++)
                        for (colun = 0; colun < 20; colun++)
                            if (steps[linha][colun][1] > 0)
                            {
                                switch (steps[linha][colun][1])
                                {
                                case 1:
                                    best_way[linha][colun][0] = 0;
                                    best_way[linha][colun][1] = green;
                                    best_way[linha][colun][2] = 0;
                                    break;
                                case 2:
                                    best_way[linha][colun][0] = 0;
                                    best_way[linha][colun][1] = 0;
                                    best_way[linha][colun][2] = blue;
                                    break;
                                case 3:
                                    best_way[linha][colun][0] = yellow;
                                    best_way[linha][colun][1] = yellow;
                                    best_way[linha][colun][2] = 0;
                                    break;
                                case 4:
                                    best_way[linha][colun][0] = red;
                                    best_way[linha][colun][1] = 0;
                                    best_way[linha][colun][2] = 0;
                                    break;
                                }
                            }
                }
            }
        }

        //*****************************
        //ORGANIZA EM ORDEM DECRESCENTE
        printf("\nOrganizando\n");

        a[0] = 7;
        a[1] = 5;
        a[2] = 3;
        a[3] = 2;
        a[4] = 1;

        for (k = 0; k < 5; k++)
        {
            gap = a[k];
            for (i = gap; i < pop; ++i)
            {
                x = individuo[i];
                for (j = i - gap; x.fitness > individuo[j].fitness && j >= 0; j = j - gap)
                    individuo[j + gap] = individuo[j];
                individuo[j + gap] = x;
            }
        }

        //CRUZAMENTO
        printf("\nCruzando\n");

        for (i = 0; i < pop_70; i++)
            list[i] = i;

        nsort = pop_70;

        for (num = 0; num < pop_70; num += 2)
        {
            for (i = 0; i < 2; i++)  //SORTEIO DOS PAIS
            {
                if (nsort > 0)
                    sorteio = rand() % nsort;

                else sorteio = 0;

                pai[i].inicio = alloc();

                pai[i].inicio = copy(individuo[list[sorteio]].inicio, pai[i].inicio);

                nsort--;

                list[sorteio] = list[nsort];

                n = 1;

                crosspoint[i] = (rand() % (length(pai[i].inicio) - 1)) + 2;
            }

            for (i = 0; i < 2; i++)
            {
                n = crosspoint[i] - 1;

                count(pai[i].inicio);

                pointer[i] = ptr;

                postree[i] = pos;
            }

            for (i = 0; i < 2; i++)
                pointer[i + 2] = pointer[i]->top;

            switch (postree[0])
            {
            case 0:
                pointer[2]->left = pointer[1];

                pointer[1]->top = pointer[2];
                break;

            case 1:
                pointer[2]->center = pointer[1];

                pointer[1]->top = pointer[2];
                break;

            case 2:
                pointer[2]->right = pointer[1];

                pointer[1]->top = pointer[2];
                break;
            }

            switch (postree[1])
            {
            case 0:
                pointer[3]->left = pointer[0];

                pointer[0]->top = pointer[3];
                break;

            case 1:
                pointer[3]->center = pointer[0];

                pointer[0]->top = pointer[3];
                break;

            case 2:
                pointer[3]->right = pointer[0];

                pointer[0]->top = pointer[3];
                break;
            }

            individuotemp[num].inicio = pai[0].inicio;

            individuotemp[num + 1].inicio = pai[1].inicio;

        }

        printf("\nGeracao: %d", geracao + 1);
        for (num = 0; num < pop; num++)
        {
            media_geracao += individuo[num].fitness;

            if (individuo[num].fitness > maior_geracao)
                maior_geracao = individuo[num].fitness;
        }

        media_geracao /= pop;

        printf("\n\nMEDIA GERACAO = %3.4f, MAIOR GERACAO = %3d\n", media_geracao, maior_geracao);

        fprintf(gene, "%d\t%3.4f  \t%3d\n", geracao + 1, media_geracao, maior_geracao);

        //IMPRIME INDIVIDUO, FITNESS E COMPLEXIDADE
        printf("\n");

        for (i = 0; i < 10; i++)
        {
            n = 1;

            printf("\n");

            printf("\nINDIVIDUO = %.2d, FITNESS = %3d, e COMPRIMENTO = %3d\n", i, individuo[i].fitness, length(individuo[i].inicio));
        }

        printf("\n");
        printf("\n**********************************************************\n");

        for (linha = 0; linha < 20; linha++)
            for (colun = 0; colun < 20; colun++)
                for (i = 0; i < 3; i++)
                    caminho[geracao].matriz[linha][colun][i] = best_way[linha][colun][i];

        if (geracao < gens)
            for (num = 0; num < pop_70; num++)
            {
                individuo[pop_30 + num] = individuotemp[num];

                if (geracao < (gens - 1))
                    if (num < pop_30)
                        individuo[num].fitness = 0;
            }
    }

    fclose(gene);

    //SALVAR
    printf("\nSalvando individuos...\n");
    for (i = 0; i < 10; i++)
    {
        n = 0;
        printstr(individuo[i].inicio);
        texto[n] = NULL;

        salvar(individuo[i].fitness, i, length(individuo[i].inicio));
    }

    //LIBERA MEMORIA

    printf("\nLiberando Memoria...\n");
    for (i = 0; i < pop; i++)
        freemem(individuo[i].inicio);

    printf("\nSalvando caminhos...\n");
    for (geracao = 0; geracao < gens; geracao++)
    {
        char arquivo[20];

        sprintf(arquivo, "caminho%.2d.ppm", geracao);

        //SALVA UMA MATRIZ PARA UM ARQUIVO TIPO PPM

        if ((img = fopen(arquivo, "w+")) == NULL)
        {
            printf("\n\n   Arquivo nao pode ser criado.\n\n");
            exit(-1);
        }

        fprintf(img, "P3 \n 20 20 \n 255 \n");  //GRAVA COMECO

        for (linha = 0; linha < 20; linha++)     //GRAVA NO ARQUIVO
        {
            for (colun = 0; colun < 20; colun++)
                fprintf(img, "%d %d %d   ", caminho[geracao].matriz[linha][colun][0], caminho[geracao].matriz[linha][colun][1], caminho[geracao].matriz[linha][colun][2]);

            fprintf(img, "\n");
        }

        fclose(img);  //FECHA ARQUIVO

        printf(".");

        sprintf(texto, "convert %s caminho%.2d.gif", arquivo, geracao);

        system(texto);

        sprintf(texto, "rm %s -f", arquivo);

        system(texto);

    }

    printf("\n\n\"FIM\"\n\n");

    //SINAL DE SAIDA
    exit(0);
}
