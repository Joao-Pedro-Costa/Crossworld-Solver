#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------ */
/* A FAZER                                    */
/* arrumar os comentários e partir pro abraço */
/* ------------------------------------------ */


/* Struct para guardar o indice de uma string no vetor de palavras, e a posicao que ela começa no tabuleiro, e a direção que está escrita.             */
/* Por exemplo, se disponiveis[3] = {0, 4, 2, 2} significa que a string de palavras[3] já está sendo utilizada (indice ==0), na posição i == 4, j == 2 */
/* e foi escrita na horizontal (dir == 2)                                                                                                              */
typedef struct {
    int indice;
    int x;
    int y;
    int dir;
} posicao;

/* Struct para a matriz que guardará o tabuleiro. Guarda, em uma mesma variável, qual o char armazenado, e quais strings estão usando aquela posição.  */
typedef struct {
    int tabuleiro;
    int prop;
} matriz;

/* Definição de item das pilhas usadas em classe alterado para utilizar a struct de posição criada */
typedef posicao item;

/* Struct de pilha criada em aula */
typedef struct {
  item *v;
  int topo;
  int tam;
} pilha;

/* Protótipos das funções de pilha criadas em aula */
pilha * criaPilha (int tam);
void destroiPilha (pilha * p);
void empilha (pilha * p, item x);
item desempilha (pilha * p);
int pilhaVazia (pilha * p);
item topoDaPilha (pilha *p);
pilha * aumentaPilha (pilha * p); 

/* ------------------------------- */
/* Protótipos das funções criadas. */
/* ------------------------------- */

/* Principal função do programa, tenta criar o tabuleiro proposto */
int criaTabuleiro (matriz ** tabela, int linhas, int colunas);

/* Verifica se a direção dir é válida para a posição (x,y)                                                     */
int direcaoValida (matriz ** tabela, posicao a, int linhas, int colunas);

/* Verifica se a string em palavra pode ser colocada a partir da posição ( x, y ), utilizando a direção dir    */
int palavraValida (matriz ** tabela, int linhas, int colunas, posicao pos, char * palavra);

/* Coloca os valores da string palavra na posição de tabuleiro começando em ( x, y ), utilizando a direção dir */
void colocaPalavra (matriz ** tabela, posicao pos, char * palavra);

/* Coloca 0 nos valores onde antes estavam a string palavra.                                                   */ 
void retiraPalavra (matriz ** tabela, posicao pos, int size);

/* Verifica se uma string  começada em ( x, y ) criou uma string que não está nas strings desejadas.           */
int verificaNova (matriz ** tabela, posicao nova, int size, char ** palavras, posicao * disponiveis, int num_pal, int linhas, int colunas);

/* Recebe um tabuleiro tamanho linhas X colunas, e imprime os chars correspondentes.                           */
void printaTabuleiro (matriz ** tabela, int linhas, int colunas);

/* incrementa a posição/direção, respeitando o máximo do número de colunas                                     */
void proxima (posicao * pos, int max_j);

/* verifica se o tabuleiro inteiro foi preenchido */
int tabuleiroCheio (matriz ** tabula, int linhas, int colunas); 

/* ------------------------------- */
/* Função main                     */
/* ------------------------------- */

int main () {
    int linhas, colunas;    /* variáveis para o tamanho da tabela */
    matriz ** tabela; /* ponteiro para a tabela alocada dinamicamente que guardará as tabelas passadas */
    int i; /* variável de suporte para loop */
    int inst = 1; /* variável que guarda o número de instâncias testadas. */

    /* recebe os tamanhos do tabuleiro inicial */
    scanf("%d %d", &linhas, &colunas);

    /* mantém o programa rodando até receber como entrada uma matriz 0x0 */
    while (linhas + colunas != 0) {
 
        /* aloca a tabela que será utilizada */
        tabela = malloc (linhas * sizeof(matriz*));
        for (i=0; i<linhas; i++)
            tabela[i]  = malloc (colunas * sizeof(matriz));

        /* chama a função que vai criar o tabuleiro                  */
        /* se der certo, chama a função que vai imprimir o tabuleiro */
        if (criaTabuleiro(tabela, linhas, colunas)){
            printf("\nInstancia %d\n", inst);
            printaTabuleiro (tabela, linhas, colunas);
        }
        /* se não der certo, imprime "nao ha solucao" */
        else {
            printf("\nInstancia %d\n", inst);
            printf ("nao ha solucao\n");
        }

        /* desaloca a tabela criada */
        for (i = 0; i < linhas; i++)
            free(tabela[i]);
        free(tabela);
        inst++;

        /* recebe os tamanhos do próximo tabuleiro */
        scanf("%d %d", &linhas, &colunas);

    }

    return 0;
}

/* --------------------------------- */
/* Implementação das funções criadas */
/* --------------------------------- */

/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: criaTabuleiro                                                                                            */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Principal função do programa, tenta criar o tabuleiro proposto                                                   */
/* Utiliza backtrack para tentar colocar as diversas palavras enviadas no tabuleiro                                 */
/* O backtrack é realizado em relação às posições: para cada posição, testa até encontrar uma palavra que possa ser */
/* colocada ali. Caso não seja possível nenhuma, faz o backtrack para a última posição válida.                      */
/* Foi considerado que uma palavra só poderá começar nas primeiras linha e coluna, ou logo após uma posição que não */
/* receberá nenhuma palavra.                                                                                        */
/* Retorna 1 caso consiga criar o tabuleiro, ou retorna 0 caso não consiga.                                         */
/* ---------------------------------------------------------------------------------------------------------------- */
int criaTabuleiro (matriz ** tabela, int linhas, int colunas) {
    int num_pal, max_pal; /* número de palavras a ser testado, tamanho máximo da palavra */
    char ** palavras; /* guardará as palavras a serem testadas */
    pilha * tentativas; /* pilha que guardará as palavras que foram usadas */
    int i,j; /* variáveis de suporte para loops */
    posicao atual; /* struct que guarda o indice da palavra no vetor de palavras, e a posição do seu começo no tabuleiro */
    posicao nula; /* struct que guarda o valor {1, 0, 0, 1}  */
    posicao * disponiveis; /* vetor que guarda quais palavras foram utilizadas, e quais estão disponíveis para serem usadas. */
                           /* disponiveis[x] == 1 significa que palavras[x] está disponível.                                 */
                           /* para disponiveis[x] == 0, disponiveis[x].i/j guardam onde a palavra começa no tabuleiro.       */


    /* cria a pilha que será utilizada para o backtrack, com tamanho igual ao número de posições no tabuleiro. */
    tentativas = criaPilha (linhas * colunas);

    /* configuar a variável que guarda o valor nulo da struct    */
    /* índice == 1 significa que a posição não foi usada.        */
    /* x == 0, y ==0, dir ==1 é a primeira posição a ser testada */
    nula.indice = 1;
    nula.x = 0; 
    nula.y = 0;
    nula.dir = 1;

    /* determina o maior tamanho possível para uma palavra como o máximo entre número de linhas e de colunas */
    /*  o +1 coloca espaço para o caracter final '\0'                                                        */
    if (linhas > colunas)
        max_pal = linhas+1;
    else
        max_pal = colunas+1;


    /* recebe o tabuleiro e o guarda em tabela. Inicializa a variável prop de cada posição como 0.  */
    for (i = 0; i < linhas; i ++)
        for (j = 0; j < colunas; j++){
            scanf("%d", &tabela[i][j].tabuleiro);
            tabela[i][j].prop = 0;
        }

    /* recebe o número de palavras, e aloca a matriz que receberá as palavras */
    scanf("%d", &num_pal);
    palavras = malloc (num_pal * sizeof(char*));
    for (i=0; i< num_pal; i++) 
        palavras[i] = malloc(max_pal * sizeof(char));

    /* Prepara o vetor que guardará quais palavras estão disponíveis para serem usadas, e a posição que a palavra está. */
    disponiveis = malloc (num_pal * sizeof(posicao));
    for (i=0; i < num_pal; i++){
        disponiveis[i] = nula;
    }

    /* recebe as palavras que serão usadas no tabuleiro */ 
    for (i=0; i<num_pal; i++)
        scanf("%s", palavras[i]);


    /* inicialização do loop de backtrack, começando na primeira linha e coluna do tabuleiro, e da string palavras[0] */
    atual.indice = 0;
    atual.x = 0;
    atual.y = 0;
    atual.dir = 1;

    /* loop de backtrack, testando as posições do tabuleiro para uma palavra válida */
    /* condição de parada: testar todas as posições do tabuleiro.                   */
    while (atual.x < linhas) {

        /* check de back track: não foi encontrada nenhuma palavra possível para a última posição */
        if (atual.indice >= num_pal) {

            /* se a pilha está vazia, não há nenhuma configuração que seja solução. Desaloca os vetores e retorna 0. */
            if (pilhaVazia(tentativas)) {
                destroiPilha(tentativas);
                for (i = 0; i < num_pal; i++)
                    free(palavras[i]);
                free(palavras);
                free(disponiveis);
                return 0;
            }

            /* caso ainda haja itens na pilha, retorna para a última posição encontrada, salva na pilha. */
            atual = desempilha(tentativas);

            /* retira a palavra que recebeu o backtrack do tabuleiro, e retorna a respectiva posição de verificação do vetor disponíveis para a condição nula */
            retiraPalavra(tabela, atual, strlen(palavras[atual.indice]));
            disponiveis[atual.indice] = nula;

            /* tenta a próxima palavra */
            atual.indice++;
        }

        /* procura a próxima posição+direção no tabuleiro que possa receber uma string */
        while (atual.x < linhas && !direcaoValida(tabela, atual, linhas, colunas))
            proxima(&atual, colunas);
    
        /* se passou por todo o tabuleiro e não há mais posições livres, então prepara para encerrar a função, desalocando os vetores criados */
        if (atual.x >= linhas) {
            destroiPilha(tentativas);
            for (i = 0; i < num_pal; i++)
                free(palavras[i]);
            free(palavras);
            free(disponiveis);

            /* se não há mais posições livres, e o tabuleiro foi completamente preenchido, então existe solução e retorna 1.*/
            /* se não há mais posições livres, e ainda há posições em branco no tabuleiro, não existe solução, e retorna 0. */
            return tabuleiroCheio(tabela, linhas, colunas);
        }

        /* procura uma palavra que possa ser colocada na posição atual. "disponíveis" checka se a palavra já foi usada, palavraValida checka se ela pode ser */
        /* colocada na posição atual. */
        while (atual.indice < num_pal && !(disponiveis[atual.indice].indice && palavraValida(tabela, linhas, colunas, atual, palavras[atual.indice]))) {
            atual.indice++;
        }

        /* caso exista uma palavra que possa ser colocada a partir da posição atual */
        if (atual.indice < num_pal) {

            /* inicialmente coloca a palavra no tabuleiro */
            colocaPalavra(tabela, atual, palavras[atual.indice]);

            /* verifica se a palavra colocada criou alguma nova string indesejada. */
            if (verificaNova(tabela, atual, strlen(palavras[atual.indice]), palavras, disponiveis, num_pal, linhas, colunas)) {

                /* caso não tenha sido criada nenhuma string indesejada, a tentativa atual foi bem sucedida. Empilha a solução e prepara para testar a próxima posição */
                empilha(tentativas, atual);
                disponiveis[atual.indice] = atual;
                disponiveis[atual.indice].indice = 0;
                atual.indice = 0;
                proxima(&atual, colunas);
            }

            /* caso tenha sido criada alguma string indesejada, retira a palavra, e prepara para testar a próxima palavra na mesma posição */
            else {
                retiraPalavra(tabela, atual, strlen(palavras[atual.indice]));
                disponiveis[atual.indice] = nula;
                atual.indice++;
            }
        }
    }


    /* desaloca os espaços de memória utilizados no programa */
    destroiPilha(tentativas);
    for (i = 0; i < num_pal; i++)
        free(palavras[i]);
    free(palavras);
    free(disponiveis);

    /* se não há mais posições livres, e o tabuleiro foi completamente preenchido, então existe solução e retorna 1.*/
    /* se não há mais posições livres, e ainda há posições em branco no tabuleiro, não existe solução, e retorna 0. */
    return tabuleiroCheio(tabela, linhas, colunas);
}
    
/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: direcaoValida                                                                                            */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Verifica se a direção dir é válida para a posição (pos)                                                          */
/* Uma direção é considerada válida se antes da posição do primeiro char da string há uma posição que não possa     */
/* receber char, ou então está na primeira linha ou coluna                                                          */
/* ---------------------------------------------------------------------------------------------------------------- */
int direcaoValida (matriz ** tabela, posicao a, int linhas, int colunas) {
    int tam = 0; /* variável que guarda o tamanho de string permitido a partir da posição+direção atual */

    /* se a posição atual possui valor == -1, então não pode receber char e é inválida.*/
    if (tabela[a.x][a.y].tabuleiro == -1) return 0;

    /* para o caso vertical, verifica se a posição está na primeira linha, ou logo abaixo de uma posição que não pode receber char. */
    if (a.dir == 1 && (a.x == 0 || (a.x > 0 && tabela[a.x-1][a.y].tabuleiro == -1))){

        /* verifica o tamanho máximo da string que pode ser colocada a partir da posição atual. */
        while (a.x+tam < linhas && tabela[a.x+tam][a.y].tabuleiro != -1)
            tam++;

        /* uma posição que só possa receber um char não é considerada válida. */
        if (tam > 1) return 1;
        return 0;
    }

    /* para o caso horizontal, verifica se a posição está na primeira coluna, ou logo à direita de uma posição que não pode receber char.*/
    if (a.dir == 2 && (a.y == 0 || (a.y > 0 && tabela[a.x][a.y-1].tabuleiro == -1))) {

        /* verifica o tamanho máximo da string que pode ser colocada a partir da posição atual. */
        while (a.y+tam < colunas && tabela[a.x][a.y+tam].tabuleiro != -1)
            tam++;

        /* uma posição que só possa receber um char não é considerada válida. */    
        if (tam > 1) return 1;
        return 0;
    }

    /* caso receba algum valor de dir que não é esperado, retorna 0.*/
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: palavraValida                                                                                            */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Verifica se a string palavra pode ser colocada a partir da posição (pos),                                        */
/* A string pode ser colocada se a posição onde seus chars serão colcados está vazia, ou se está ocupada com o      */
/* mesmo char.                                                                                                      */
/* Retorna 1 caso a palavra possa ser colocada, e retorna 0 caso a palavra não possa.                               */
/* ---------------------------------------------------------------------------------------------------------------- */
int palavraValida (matriz ** tabela,int linhas, int colunas, posicao pos, char * palavra){
    int i=0; /* variável de suporte para loop */
    int tam_max; /* variável para guardar o máximo tamanho permitido a partir da posição indicada. */
    size_t tam = strlen (palavra); /* tamanho da string recebida */ 

    /* antes de tentar colocar a palavra, verifica se a posição pode receber uma palavra na direção indicada */
    if (!direcaoValida(tabela, pos, linhas, colunas)) return 0;

    /* verifica o tamanho da string que poderá ser alocada a partir das condições desejadas. */
    tam_max = 0;
    
    /* caso vertical: incrementa i até encontrar uma posição que não seja possível colocar char */
    if (pos.dir == 1) 
        while (pos.x+tam_max < linhas && tabela[pos.x+tam_max][pos.y].tabuleiro != -1)
            tam_max++;

    /*caso horizontal: incrementa j até encontrar uma posição que não seja possível colocar char */
    else if (pos.dir == 2)
        while (pos.y+tam_max < colunas && tabela[pos.x][pos.y+tam_max].tabuleiro != -1 )
            tam_max++;

    /* se o espaço a ser colocado for diferente do número de chars na string, ela não pode ser colocada ali. */
    if (!(tam == tam_max)) return 0;

    while (i < tam) {

        /* Para o caso horizontal, i incrmenta x */
        if (pos.dir == 1) {

            /* Enquanto os chares forem iguais, ou o da tabela for 0, continua no loop */
            if (tabela[pos.x+i][pos.y].tabuleiro == palavra[i] || tabela[pos.x+i][pos.y].tabuleiro == 0)
                i++;

            /* Caso um não seja, a palavra não pode ser colocada nesta posição, então retorna 0 */
            else 
                return 0;
        }

        /* Para o caso vertical, i incrementa y */
        else if (pos.dir == 2) {

            /* Mesma lógica do caso anterior */
            if (tabela[pos.x][pos.y+i].tabuleiro == palavra[i] || tabela[pos.x][pos.y+i].tabuleiro == 0)
                i++;
            else 
                return 0;
        }
    }

    /* Caso a função chegue neste ponto, todos os chares são iguais, ou o espaço na tabela está vazio.  */
    /* Portanto, a palavra pode ser colocada ali.                                                       */
    return 1;
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: colocaPalavra                                                                                            */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Coloca os valores da string palavra na posição de tabuleiro começando em (pos), sem se preocupar se as posições  */
/* são válidas ou não.                                                                                              */
/* Coloca em posição.prop qual a palavra é proprietária da string que está usando aquela posição.                   */
/* Dados armazenados como int.                                                                                      */
/* ---------------------------------------------------------------------------------------------------------------- */
void colocaPalavra (matriz ** tabela, posicao pos, char * palavra){
    int i; /* variável de suporte para loop */
    size_t tam = strlen (palavra); /* tamanho da string recebida */ 

    for (i = 0; i < tam; i++) {

        /* Para o caso horizontal, i incrmenta x */
        if (pos.dir == 1) {

            /* colca o char na posição proposta */
            tabela[pos.x+i][pos.y].tabuleiro = palavra[i];

            /* salva que a palavra que está sendo colocada está utilizando aquela posição */
            tabela[pos.x+i][pos.y].prop += pos.indice;
        }


        /* Para o caso vertical, i incrementa y */
        else if (pos.dir == 2) {

            /* coloca o char na posição proposta */
            tabela[pos.x][pos.y+i].tabuleiro = palavra[i];

            /* salva que a palavra que está sendo colocada está utilizando aquela posição */
            tabela[pos.x][pos.y+i].prop += pos.indice;
        }
    }
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: retiraPalavra                                                                                            */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Coloca 0 nos valores onde antes estavam a string palavra.                                                        */ 
/* Primeiro retira a propriedade da palavra a ser retirada de uma posição. Se posição.prop == 0, então nenhuma      */
/* outra string está utilizando aquela posição, e portanto pode ser removida.                                       */
/* ---------------------------------------------------------------------------------------------------------------- */
void retiraPalavra (matriz ** tabela, posicao pos, int size){
    int i; /* variável de suporte para loop */

    for (i = 0; i < size; i++) {

        /* caso vertical, incrementa x*/
        if (pos.dir == 1) {

            /* retira a palavra como proprietária da posição atual */
            tabela[pos.x+i][pos.y].prop -= pos.indice;

            /* se posição.prop == 0, então nenhuma string está usando a posição atual, e ela pode ser removida. */
            if (!tabela[pos.x+i][pos.y].prop) tabela[pos.x+i][pos.y].tabuleiro = 0;
        }

        /* caso horizontal, incrementa y*/
        else if (pos.dir == 2) {
            
            /* retira a propriedade da palavra sendo removida da posição atual */
            tabela[pos.x][pos.y+i].prop -= pos.indice;
            
            /* se tabela.prop == 0, então nenhuma string está usando a posição atual, e ela pode ser removida. */
            if (!tabela[pos.x][pos.y+i].prop) tabela[pos.x][pos.y+i].tabuleiro =0;
        }
    }

}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: verificaNova                                                                                             */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Verifica se uma string  começada em (nova) criou uma string que não está nas strings desejadas.                */
/* Caso encontre uma string adjacente à string dada, verifica se ela é uma das desejadas.                           */
/* Também verifica se está na mesma posição que a string já apareceu, evitando duplicatas.                          */
/* Retorna 1 caso não haja strings problemáticas adjacentes, e retorna 0 caso haja.                                 */
/* ---------------------------------------------------------------------------------------------------------------- */
int verificaNova (matriz ** tabela, posicao nova, int size, char ** palavras, posicao * disponiveis, int num_pal, int linhas, int colunas){
    int i, j, k; /* variáveis de suporte para loop */
    int pos; /* variável que verifica a posição inicial da string adjacente */
    int ok; /* variável que guarda o resultado da verificação*/
    int x = nova.x, y = nova.y; /* variáveis para guardar a posição */
    int testa_sub;
    /* verifica para cada char da string nova */

    for (i = 0; i < size; i++) {

        /*  inicialmenete considera que não há problemas */
        ok = 1;

        /* Caso vertical: i incrementa x, verificação de adjacentes é na horizontal */
        if (nova.dir == 1) {
            /*printf("procurando ao redor de: %c\n", tabela[x+i][y].tabuleiro);*/

            /* Caso seja encontrado uma posição adjacente com um char, começa a procurar se a string formada é válida. */
            if ((y < colunas-1 && tabela[x+i][y+1].tabuleiro > 0) || (y != 0 && tabela[x+i][y-1].tabuleiro > 0 )) {
                /*if (y < colunas-1 &&tabela[x+i][y+1].tabuleiro > 0) printf("char encontrado a esquerda: %c\n", tabela[x+i][y+1].tabuleiro);
                else printf("char encontrado a direita: %c\n", tabela[x+i][y-1].tabuleiro);*/

                /* procura a posição inicial da string adjacente encontrada, e salva em pos */
                pos = y;
                while (pos > 0 && tabela[x+i][pos-1].tabuleiro >= 0)
                    pos --;

                if (!tabela[x+i][pos].tabuleiro)
                    ok = 1;
                else {
                    ok = 0;
                    /*printf("primeiro char: %c\n", tabela[x+i][pos].tabuleiro);*/

                    /* procura no vetor de strings se a string adjacente econtrada é uma das especificadas */
                    for (j = 0; j < num_pal; j++) 

                        /* procura inicialmente pelo primeiro char da string */
                        if (tabela[x+i][pos].tabuleiro == palavras[j][0]) {
                            /* se encontra uma potencial string igual, considera que pode ser uma certa */
                            ok = 1;
                            testa_sub = 1;
                            /* verifica se o resto das strings são iguais. Se encontra uma posição em branco (0) para, pois não é string completa */
                            for (k=0; (pos+k < colunas && tabela[x+i][pos+k].tabuleiro >= 0 ); k++) {
                                /*printf("Comparando %c e %c, palavra: %s\n", tabela[x+i][pos+k].tabuleiro, palavras[j][k], palavras[j]);*/
                                if (!tabela[x+i][pos+k].tabuleiro) {
                                    /*printf("string incompleta!\n");*/
                                    ok = 1;
                                    testa_sub = 0;
                                    break;
                                }
                                if (tabela[x+i][pos+k].tabuleiro != palavras[j][k]) {
                                    ok = 0;
                                    testa_sub = 0;
                                    break;
                                    /*printf("ERRO: palavra diferente\n");*/
                                }
                            }

                            /* caso específico da string adjacente ser uma substring de outra encontrada */ 
                            if (testa_sub && k != strlen(palavras[j])) {
                                ok = 0;
                                /*printf("ERRO: tamanhos diferentes: %d e %d\n", k, strlen(palavras[j]));*/
                            }

                            /* caso ambas as strings sejam iguais, verifica se ela foi utilizada. Se sim, mas em posição ou direção diferentes, é uma duplicata indesejada */
                            if (ok && k == strlen(palavras[j]) && !disponiveis[j].indice && ( disponiveis[j].x != x+i || disponiveis[j].y != pos || disponiveis[j].dir != 2)) {
                                ok = 0; 
                                /*printf("ERRO: duplicata (%d, %d, %d) em (%d, %d, %d)\n", x+i, pos, 2, disponiveis[j].x, disponiveis[j].y, disponiveis[j].dir);*/
                            }
                            
                            /* caso passe por todas as verificações, ele encontrou uma palavra que é igual, na mesma posição */
                            if (ok)
                                break;
                        }
                }

                /* caso encontre uma string problemática, retorna 0 .*/
                if (!ok) return 0;
            }

        }
        /* caso horizontal : i incrementa y, verificação de adjacentes é feita na vertical. */
        else if (nova.dir == 2) {
            /*printf("procurando ao redor de: %c\n", tabela[x][y+i].tabuleiro);*/
            /* Caso seja encontrado uma posição adjacente com um char, começa a procurar se a string formada é válida. */
            if ((x < linhas - 1 && tabela[x+1][y+i].tabuleiro > 0) || (x != 0 && tabela[x-1][y+i].tabuleiro > 0 )) {
                /*if (x < linhas - 1 &&tabela[x+1][y+i].tabuleiro > 0) printf("char encontrado abaixo: %c\n", tabela[x+1][y+i].tabuleiro);
                else printf("char encontrado acima: %c\n", tabela[x-1][y+i].tabuleiro);*/
                /* procura a posição inicial da string adjacente encontrada, e salva em pos */
                pos = x;
                while (pos > 0 && tabela[pos-1][y+i].tabuleiro >= 0)
                    pos --;

                if (!tabela[pos][y+i].tabuleiro)
                    ok = 1;
                else {
                    ok = 0;
                    /*printf("primeiro char: %c\n", tabela[pos][y+i].tabuleiro);*/
                    /* procura no vetor de strings se a string adjacente econtrada é uma das especificadas */
                    for (j = 0; j < num_pal; j++) 

                        /* procura inicialmente pelo primeiro char da string */
                        if (tabela[pos][y+i].tabuleiro == palavras[j][0]) {
                            /* se encontra uma potencial string igual, considera que pode ser uma certa */
                            ok = 1;
                            testa_sub = 1;
                            /* verifica se o resto das strings são iguais. Se encontra uma posição em branco (0) para, pois não é string completa */
                            for (k=0; (pos+k < linhas && tabela[pos+k][y+i].tabuleiro >= 0 ); k++) {
                                /*printf("Comparando %c e %c, palavra: %s\n", tabela[pos+k][y+i].tabuleiro, palavras[j][k], palavras[j]);*/
                                if (!tabela[pos+k][y+i].tabuleiro) {
                                    /*printf("string incompleta!\n");*/
                                    ok = 1;
                                    testa_sub = 0;
                                    break;
                                }
                                if (tabela[pos+k][y+i].tabuleiro != palavras[j][k]) {
                                    ok = 0;
                                    testa_sub = 0;
                                    break;
                                    /*printf("ERRO: palavra diferente\n");*/
                                }
                            }
                            /* caso específico da string adjacente ser uma substring de outra encontrada */ 
                            if (testa_sub && k != strlen(palavras[j])) {
                                ok = 0;
                                /*printf("ERRO: tamanhos diferentes: %d e %d\n", k, strlen(palavras[j]));*/
                            }
                        
                            /* caso ambas as strings sejam iguais, verifica se ela foi utilizada. Se sim, mas em posição ou direção diferentes, é uma duplicata indesejada */
                            if (ok && disponiveis[j].indice == 0 && ( disponiveis[j].x != pos || disponiveis[j].y != y+i || disponiveis[j].dir != 1)) {
                                ok = 0; 
                                /*printf("ERRO: duplicata (%d, %d, %d) em (%d, %d, %d)\n", pos, y+i, 1, disponiveis[j].x, disponiveis[j].y, disponiveis[j].dir);*/
                            }
                            
                            /* caso passe por todas as verificações, ele encontrou uma palavra que é igual, na mesma posição */
                            if (ok)
                                break;
                        }
                }

                /* caso encontre uma string problemática, retorna 0 .*/
                if (!ok) return 0;
            }

        }
    }

    /* caso a função saia do loop, ou não existem strings adjacentes, ou todas as strings adjacentes estão entre as especificadas e não existem duplicatas. */
    return 1;
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: printaTabuleiro                                                                                          */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Recebe um tabuleiro tamanho linhas X colunas, e imprime os chars correspondentes.                                */
/* Os chars estão armazenados como int na tabela. Uma posição com valor "-1" é considerada posição que não pode     */
/* receber char, e é impressa como "*"                                                                              */
/* ---------------------------------------------------------------------------------------------------------------- */
void printaTabuleiro (matriz ** tabela, int linhas, int colunas) {
    int i, j; /* variáveis de suporte para loop*/

    for (i = 0; i < linhas; i++){ 
        for (j = 0; j < colunas; j++) {

            /* se o valor da tabela é > 0, a posição guarda um char e porntanto o char guardado */
            if (tabela[i][j].tabuleiro > 0) 
                printf("%c ", tabela[i][j].tabuleiro);
            
            /* se o valor da tabela é == 0, a posição guarda uma posição livre, imprimindo 0 nesta posição*/
            else if (!tabela[i][j].tabuleiro)
                printf("0 ");

            /* se o valor da tabela é < 0, a posição não pode receber chars, então imprime * no lugar. */
            else 
                printf("* ");
        }
        printf("\n");
    }
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: proxima                                                                                                  */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Incrementa a posição/direção, respeitando o máximo do número de colunas                                          */
/* Não considera o máximo número de linhas, isto será tratado fora da chamada da função.                            */
/* ---------------------------------------------------------------------------------------------------------------- */
void proxima (posicao * pos, int max_j) {
    if (pos->dir <2) pos->dir++;
    else if (pos->y < max_j) {
        pos->dir = 1;
        pos->y++;
        if (pos->y >= max_j) {
            pos->y = 0;
            pos->x++;
        }
    }
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Função: tabuleiroCheio                                                                                           */
/* ---------------------------------------------------------------------------------------------------------------- */
/* Verifica se o tabuleiro inteiro foi preenchido                                                                   */
/* O tabuleiro é considerado preenchido se não existem posições que sejam 0.                                        */
/* ---------------------------------------------------------------------------------------------------------------- */
int tabuleiroCheio (matriz ** tabela, int linhas, int colunas ){ 
    int i, j; /* variáveis de suporte para loop */

    for (i = 0; i < linhas; i++)
        for (j = 0; j < colunas; j++)
            if (!tabela[i][j].tabuleiro) return 0;
    
    return 1;
}

/* ---------------------------------------------------- */
/* implementação das funções de pilha dadas em aula     */
/* únicas alterações feitas foram para retirar Warnings */
/* ---------------------------------------------------- */
pilha * criaPilha (int tam) {
  pilha * p = malloc(sizeof(pilha));
  p->tam = tam;
  p->topo = 0;
  p->v = malloc (tam * sizeof(item));
  if (p->v == NULL)
    return NULL; 
  return p;
} 
  
void destroiPilha (pilha * p) {
  free (p->v);
  free (p);
} 
  
void empilha (pilha * p, item x) {
  if (p->topo == p->tam)
    p = aumentaPilha (p);
  p->v[p->topo] = x;
  p->topo ++;
}
  
item desempilha (pilha * p) {
    posicao a;
    if (!pilhaVazia (p)){
        p->topo --;
        return (p->v[p->topo]);
    }
    /* adicionado um retorno nulo para evitar o warning que diz que o programa não retorna nada */
    a.dir =0;
    a.x=0;
    a.y=0;
    a.indice=0;
    return a;
}
 
int pilhaVazia (pilha * p) {
  return (p->topo == 0); 
}

item topoDaPilha (pilha *p) {
    posicao a;
    if (!pilhaVazia(p)){
        return (p->v[p->topo - 1]);
    }

    /* adicionado um retorno nulo para evitar o warning que diz que o programa não retorna nada */
    a.dir =0;
    a.x=0;
    a.y=0;
    a.indice=0;
    return a;
}

/* dobra o tamanho da pilha */
pilha * aumentaPilha (pilha * P) {
  item *w = malloc (2 * P->tam * sizeof(item)); 
  int i; 
  for (i = 0; i < P->tam; i++)
    w[i] = P->v[i]; 
  P->tam = 2 * P->tam; 
  free(P->v);
  P->v = w;
  return (P); 
} 
