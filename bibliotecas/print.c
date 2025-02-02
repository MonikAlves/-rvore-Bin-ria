#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "arvore.h"
#include "print.h"

#define MAX_HEIGHT 1000

int lprofile[MAX_HEIGHT];
int rprofile[MAX_HEIGHT];
int gap = 3;
int print_next = 0;

void limparTerminal() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void exibirMenu() {
    limparTerminal();
    printf("=============================\n");
    printf("=       Árvore Binária      =\n");
    printf("=============================\n");
    printf("= 1. Adicionar Nós          =\n");
    printf("= 2. Remover Nós            =\n");
    printf("= 3. Procurar Valor         =\n");
    printf("= 4. Refazer a Árvore       =\n");
    printf("= 5. Sair                   =\n");
    printf("=============================\n\n");
}

void print_level(asciinode *nodo, int x, int nivel) {
    int i, ehEsquerda;
    if (nodo == NULL) return;
    ehEsquerda = (nodo->parent_dir == -1);
    if (nivel == 0) {
        for (i = 0; i < (x - print_next - ((nodo->lablen - ehEsquerda) / 2)); i++) {
            printf(" ");
        }
        print_next += i;
        printf("%s", nodo->label);
        print_next += nodo->lablen;
    } else if (nodo->edge_length >= nivel) {
        if (nodo->esquerda != NULL) {
            for (i = 0; i < (x - print_next - nivel); i++) {
                printf(" ");
            }
            print_next += i;
            printf("/");
            print_next++;
        }
        if (nodo->direita != NULL) {
            for (i = 0; i < (x - print_next + nivel); i++) {
                printf(" ");
            }
            print_next += i;
            printf("\\");
            print_next++;
        }
    } else {
        print_level(nodo->esquerda, x - nodo->edge_length - 1, nivel - nodo->edge_length - 1);
        print_level(nodo->direita, x + nodo->edge_length + 1, nivel - nodo->edge_length - 1);
    }
}

void compute_lprofile(asciinode *nodo, int x, int y) {
    int i, ehEsquerda;
    if (nodo == NULL) return;
    ehEsquerda = (nodo->parent_dir == -1);
    lprofile[y] = fmin(lprofile[y], x - ((nodo->lablen - ehEsquerda) / 2));
    if (nodo->esquerda != NULL) {
        for (i = 1; i <= nodo->edge_length && y + i < MAX_HEIGHT; i++) {
            lprofile[y + i] = fmin(lprofile[y + i], x - i);
        }
    }
    compute_lprofile(nodo->esquerda, x - nodo->edge_length - 1, y + nodo->edge_length + 1);
    compute_lprofile(nodo->direita, x + nodo->edge_length + 1, y + nodo->edge_length + 1);
}

void compute_rprofile(asciinode *nodo, int x, int y) {
    int i, naoEhEsquerda;
    if (nodo == NULL) return;
    naoEhEsquerda = (nodo->parent_dir != -1);
    rprofile[y] = fmax(rprofile[y], x + ((nodo->lablen - naoEhEsquerda) / 2));
    if (nodo->direita != NULL) {
        for (i = 1; i <= nodo->edge_length && y + i < MAX_HEIGHT; i++) {
            rprofile[y + i] = fmax(rprofile[y + i], x + i);
        }
    }
    compute_rprofile(nodo->esquerda, x - nodo->edge_length - 1, y + nodo->edge_length + 1);
    compute_rprofile(nodo->direita, x + nodo->edge_length + 1, y + nodo->edge_length + 1);
}

void compute_edge_lengths(asciinode *nodo) {
    int h, hmin, i, delta;
    if (nodo == NULL) return;
    compute_edge_lengths(nodo->esquerda);
    compute_edge_lengths(nodo->direita);

    if (nodo->direita == NULL && nodo->esquerda == NULL) {
        nodo->edge_length = 0;
    } else {
        if (nodo->esquerda != NULL) {
            for (i = 0; i < nodo->esquerda->height && i < MAX_HEIGHT; i++) {
                rprofile[i] = -(1<<20);
            }
            compute_rprofile(nodo->esquerda, 0, 0);
            hmin = nodo->esquerda->height;
        } else {
            hmin = 0;
        }
        if (nodo->direita != NULL) {
            for (i = 0; i < nodo->direita->height && i < MAX_HEIGHT; i++) {
                lprofile[i] = (1<<20);
            }
            compute_lprofile(nodo->direita, 0, 0);
            hmin = fmin(nodo->direita->height, hmin);
        } else {
            hmin = 0;
        }
        delta = 4;
        for (i = 0; i < hmin; i++) {
            delta = fmax(delta, gap + 1 + rprofile[i] - lprofile[i]);
        }

        if (((nodo->esquerda != NULL && nodo->esquerda->height == 1) ||
             (nodo->direita != NULL && nodo->direita->height == 1)) && delta > 4) {
            delta--;
        }

        nodo->edge_length = ((delta + 1) / 2) - 1;
    }

    h = 1;
    if (nodo->esquerda != NULL) {
        h = fmax(nodo->esquerda->height + nodo->edge_length + 1, h);
    }
    if (nodo->direita != NULL) {
        h = fmax(nodo->direita->height + nodo->edge_length + 1, h);
    }
    nodo->height = h;
}

asciinode * build_ascii_tree_recursive(Arvore * t) {
    asciinode * nodo;

    if (t == NULL) return NULL;

    nodo = malloc(sizeof(asciinode));
    nodo->esquerda = build_ascii_tree_recursive(t->esquerda);
    nodo->direita = build_ascii_tree_recursive(t->direita);

    if (nodo->esquerda != NULL) {
        nodo->esquerda->parent_dir = -1;
    }

    if (nodo->direita != NULL) {
        nodo->direita->parent_dir = 1;
    }

    sprintf(nodo->label, "%d", t->valor);
    nodo->lablen = strlen(nodo->label);

    return nodo;
}

asciinode * build_ascii_tree(Arvore * t) {
    asciinode *nodo;
    if (t == NULL) return NULL;
    nodo = build_ascii_tree_recursive(t);
    nodo->parent_dir = 0;
    return nodo;
}

void free_ascii_tree(asciinode *nodo) {
    if (nodo == NULL) return;
    free_ascii_tree(nodo->esquerda);
    free_ascii_tree(nodo->direita);
    free(nodo);
}

// Função para imprimir a árvore em ordem de nível
void imprimirArvore(Arvore* root) {
    asciinode* proot = build_ascii_tree(root);
    int i, xmin, max_level;
    if (proot == NULL) return;
    compute_edge_lengths(proot);
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
        lprofile[i] = (1<<20);
    }
    compute_lprofile(proot, 0, 0);
    xmin = 0;
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
        xmin = fmin(xmin, lprofile[i]);
    }
    for (i = 0; i < proot->height; i++) {
        print_next = 0;
        print_level(proot, -xmin, i);
        printf("\n");
    }
    if (proot->height >= MAX_HEIGHT) {
        printf("(Tree is taller than %d, will be truncated)\n", MAX_HEIGHT);
    }
    free_ascii_tree(proot);
}