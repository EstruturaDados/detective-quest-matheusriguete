#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 10

// Estrutura da árvore binária (mapa da mansão)
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Estrutura da BST (pistas coletadas)
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// Estrutura da tabela hash (associação pista → suspeito)
typedef struct HashNode {
    char pista[100];
    char suspeito[50];
    struct HashNode *prox;
} HashNode;

// Cria dinamicamente um cômodo com nome e pista
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// Calcula o índice da hash a partir da pista
int hashFunc(const char *pista) {
    int soma = 0;
    for (int i = 0; pista[i] != '\0'; i++) soma += pista[i];
    return soma % TAM_HASH;
}

// Insere uma associação pista/suspeito na tabela hash
void inserirNaHash(HashNode **tabela, const char *pista, const char *suspeito) {
    int indice = hashFunc(pista);
    HashNode *novo = (HashNode*) malloc(sizeof(HashNode));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = tabela[indice];
    tabela[indice] = novo;
}

// Busca o suspeito correspondente a uma pista
char* encontrarSuspeito(HashNode **tabela, const char *pista) {
    int indice = hashFunc(pista);
    HashNode *atual = tabela[indice];
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0)
            return atual->suspeito;
        atual = atual->prox;
    }
    return NULL;
}

// Insere uma nova pista na árvore BST
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *nova = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(nova->pista, pista);
        nova->esquerda = nova->direita = NULL;
        return nova;
    }
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    return raiz;
}

// Exibe as pistas em ordem alfabética
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

// Explora a mansão, coleta pistas e armazena na BST
void explorarSalas(Sala *atual, PistaNode **raizPistas, HashNode **tabela) {
    char opcao;

    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("Você encontrou uma pista: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista aqui.\n");
        }

        printf("Escolha o caminho (e - esquerda, d - direita, s - sair): ");
        scanf(" %c", &opcao);

        if (opcao == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (opcao == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (opcao == 's') {
            printf("\nExploração encerrada!\n");
            return;
        } else {
            printf("Caminho inválido.\n");
        }
    }
}

// Conta quantas pistas coletadas pertencem a um suspeito específico
int contarPistasDoSuspeito(PistaNode *raiz, HashNode **tabela, const char *suspeito) {
    if (raiz == NULL) return 0;
    int count = 0;
    char *sus = encontrarSuspeito(tabela, raiz->pista);
    if (sus != NULL && strcmp(sus, suspeito) == 0)
        count = 1;
    return count + contarPistasDoSuspeito(raiz->esquerda, tabela, suspeito)
                 + contarPistasDoSuspeito(raiz->direita, tabela, suspeito);
}

// Verifica o julgamento final e exibe o resultado
void verificarSuspeitoFinal(PistaNode *raiz, HashNode **tabela) {
    char suspeito[50];
    printf("\nQuem você acha que é o culpado? ");
    scanf(" %[^\n]", suspeito);

    int qtd = contarPistasDoSuspeito(raiz, tabela, suspeito);

    printf("\n--- Julgamento Final ---\n");
    if (qtd >= 2)
        printf("Você acertou! %s é o culpado, com %d pistas ligadas a ele!\n", suspeito, qtd);
    else
        printf("Você errou! Apenas %d pista(s) apontam para %s. Continue investigando!\n", qtd, suspeito);
}

// Libera memória das árvores e da tabela hash
void liberarSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}

void liberarPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

void liberarHash(HashNode **tabela) {
    for (int i = 0; i < TAM_HASH; i++) {
        HashNode *atual = tabela[i];
        while (atual != NULL) {
            HashNode *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
}

// Programa principal
int main() {
    // Montagem fixa do mapa
    Sala *hall = criarSala("Hall de Entrada", "Pegadas na poeira");
    Sala *salaEstar = criarSala("Sala de Estar", "Copo quebrado");
    Sala *cozinha = criarSala("Cozinha", "Faca suja");
    Sala *biblioteca = criarSala("Biblioteca", "Livro fora do lugar");
    Sala *jardim = criarSala("Jardim", "Luva caída");

    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    // Criação da tabela hash
    HashNode *tabela[TAM_HASH] = {NULL};
    inserirNaHash(tabela, "Pegadas na poeira", "Sr. Mostarda");
    inserirNaHash(tabela, "Copo quebrado", "Sra. White");
    inserirNaHash(tabela, "Faca suja", "Coronel Mustard");
    inserirNaHash(tabela, "Livro fora do lugar", "Srta. Scarlet");
    inserirNaHash(tabela, "Luva caída", "Coronel Mustard");

    // Início da exploração
    PistaNode *raizPistas = NULL;
    printf("Bem-vindo ao Detective Quest!\n");
    explorarSalas(hall, &raizPistas, tabela);

    printf("\nPistas coletadas em ordem alfabética:\n");
    exibirPistas(raizPistas);

    verificarSuspeitoFinal(raizPistas, tabela);

    liberarSalas(hall);
    liberarPistas(raizPistas);
    liberarHash(tabela);

    return 0;
}

