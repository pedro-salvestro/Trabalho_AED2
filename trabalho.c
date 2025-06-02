#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>

typedef enum { INTEIRO, RACIONAL } TipoDado;

typedef struct {
    void* dados;
    int topo;
    TipoDado tipo;
} Pilha;

// Variáveis globais para tempo preciso
LARGE_INTEGER t_inicio, t_fim, freq;

void iniciar_tempo() {
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t_inicio);
}

double encerrar_tempo_micros() {
    QueryPerformanceCounter(&t_fim);
    return (double)(t_fim.QuadPart - t_inicio.QuadPart) * 1e6 / freq.QuadPart;
}

long long encerrar_tempo_nanos() {
    QueryPerformanceCounter(&t_fim);
    return (long long)((t_fim.QuadPart - t_inicio.QuadPart) * 1e9 / freq.QuadPart);
}

void formatar_tempo(double tempo_micros, char* buffer) {
    long long total_micros = (long long)tempo_micros;
    int horas = (int)(total_micros / 3600000000LL);
    total_micros %= 3600000000LL;
    int minutos = (int)(total_micros / 60000000LL);
    total_micros %= 60000000LL;
    int segundos = (int)(total_micros / 1000000LL);
    total_micros %= 1000000LL;
    int milissegundos = (int)(total_micros / 1000LL);
    int microssegundos = (int)(total_micros % 1000LL);

    sprintf(buffer, "%02d:%02d:%02d.%03d%03d", horas, minutos, segundos, milissegundos, microssegundos);
}

// Função para mostrar todos os elementos da pilha
void mostrar_pilha(Pilha* p) {
    printf("Pilha:\n");
    for (int i = 0; i <= p->topo; i++) {
        if (p->tipo == INTEIRO)
            printf("%d ", ((int*)p->dados)[i]);
        else
            printf("%.2f ", ((float*)p->dados)[i]);
    }
    printf("\n");
}

// Inicialização e gerenciamento
void inicializar_pilha(Pilha* p, int tamanho, TipoDado tipo) {
    p->topo = -1;
    p->tipo = tipo;
    if (tipo == INTEIRO)
        p->dados = malloc(tamanho * sizeof(int));
    else
        p->dados = malloc(tamanho * sizeof(float));
}

void liberar_pilha(Pilha* p) {
    free(p->dados);
}

void push(Pilha* p, void* valor) {
    p->topo++;
    if (p->tipo == INTEIRO)
        ((int*)p->dados)[p->topo] = *(int*)valor;
    else
        ((float*)p->dados)[p->topo] = *(float*)valor;
}

void pop(Pilha* p) {
    if (p->topo >= 0)
        p->topo--;
}

void remover_em_massa(Pilha* p, float percentual) {
    int nova_altura = (int)((1.0 - percentual) * (p->topo + 1));
    if (nova_altura < 0) nova_altura = 0;
    p->topo = nova_altura - 1;
}

int buscar(Pilha* p, void* valor) {
    for (int i = 0; i <= p->topo; i++) {
        if (p->tipo == INTEIRO && ((int*)p->dados)[i] == *(int*)valor)
            return i;
        else if (p->tipo == RACIONAL && fabs(((float*)p->dados)[i] - *(float*)valor) < 1e-3)
            return i;
    }
    return -1;
}

// Counting Sort para inteiros
void counting_sort_inteiro(Pilha* p) {
    int n = p->topo + 1;
    int* arr = (int*)p->dados;

    if (n <= 0) return;

    int max = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];

    int* count = calloc(max + 1, sizeof(int));
    int* output = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) count[arr[i]]++;
    for (int i = 1; i <= max; i++) count[i] += count[i - 1];
    for (int i = n - 1; i >= 0; i--) output[--count[arr[i]]] = arr[i];
    for (int i = 0; i < n; i++) arr[i] = output[i];

    free(count);
    free(output);
}

// Counting Sort para racionais
void counting_sort_racional(Pilha* p, int casas_decimais) {
    int n = p->topo + 1;
    float* arr = (float*)p->dados;
    if (n <= 0) return;

    int fator = 1;
    for (int i = 0; i < casas_decimais; i++) fator *= 10;

    int* arr_int = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        arr_int[i] = (int)(arr[i] * fator + 0.5f);

    int max = arr_int[0];
    for (int i = 1; i < n; i++)
        if (arr_int[i] > max)
            max = arr_int[i];

    int* count = calloc(max + 1, sizeof(int));
    int* output = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) count[arr_int[i]]++;
    for (int i = 1; i <= max; i++) count[i] += count[i - 1];
    for (int i = n - 1; i >= 0; i--) output[--count[arr_int[i]]] = arr_int[i];
    for (int i = 0; i < n; i++) arr[i] = (float)output[i] / fator;

    free(count);
    free(output);
    free(arr_int);
}

// Geração de dados aleatórios
void preencher_aleatorio(Pilha* p, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (p->tipo == INTEIRO) {
            int valor = rand() % 1000;
            push(p, &valor);
        } else {
            float valor = (float)(rand() % 10000) / 100.0f;
            push(p, &valor);
        }
    }
}

int main() {
    srand((unsigned int)time(NULL));
    Pilha pilha;
    int tamanho;

    printf("Informe o tipo da pilha (0 = inteiro, 1 = racional): ");
    int tipo;
    scanf("%d", &tipo);

    printf("Informe o tamanho da pilha: ");
    scanf("%d", &tamanho);

    inicializar_pilha(&pilha, tamanho, tipo == 0 ? INTEIRO : RACIONAL);

    iniciar_tempo();
    preencher_aleatorio(&pilha, tamanho);
    double tempo_gerar_micros = encerrar_tempo_micros();
    long long tempo_gerar_nanos = encerrar_tempo_nanos();

    char tempo_formatado[30];
    formatar_tempo(tempo_gerar_micros, tempo_formatado);

    printf("Tempo para gerar a pilha: %s\n", tempo_formatado);
    printf("Tempo em nanosegundos: %lld ns\n", tempo_gerar_nanos);

    int opcao;
    do {
        printf("\nOpções:\n1 - Ordenar\n2 - Inserir\n3 - Buscar\n4 - Remover Topo\n5 - Remover em massa\n0 - Sair\nEscolha: ");
        scanf("%d", &opcao);

        iniciar_tempo();

        switch (opcao) {
            case 1:
                if (pilha.tipo == INTEIRO)
                    counting_sort_inteiro(&pilha);
                else
                    counting_sort_racional(&pilha, 2);

                {
                    double tempo_op_micros = encerrar_tempo_micros();
                    long long tempo_op_nanos = encerrar_tempo_nanos();

                    formatar_tempo(tempo_op_micros, tempo_formatado);
                    printf("Tempo para ordenar: %s\n", tempo_formatado);
                    printf("Tempo em nanosegundos: %lld ns\n", tempo_op_nanos);
                }

                // Perguntar se deseja imprimir a pilha ordenada
                printf("Deseja imprimir a pilha ordenada? (1 = sim, 0 = não): ");
                int imprimir;
                scanf("%d", &imprimir);
                if (imprimir == 1) {
                    mostrar_pilha(&pilha);
                }
                break;

            case 2:
                if (pilha.tipo == INTEIRO) {
                    int val;
                    printf("Valor: "); scanf("%d", &val);
                    push(&pilha, &val);
                } else {
                    float val;
                    printf("Valor: "); scanf("%f", &val);
                    push(&pilha, &val);
                }

                {
                    double tempo_op_micros = encerrar_tempo_micros();
                    long long tempo_op_nanos = encerrar_tempo_nanos();

                    formatar_tempo(tempo_op_micros, tempo_formatado);
                    printf("Tempo para inserir: %s\n", tempo_formatado);
                    printf("Tempo em nanosegundos: %lld ns\n", tempo_op_nanos);
                }

                break;

            case 3:
                if (pilha.tipo == INTEIRO) {
                    int val;
                    printf("Valor: "); scanf("%d", &val);
                    int res = buscar(&pilha, &val);
                    printf(res >= 0 ? "Encontrado\n" : "Não encontrado\n");
                } else {
                    float val;
                    printf("Valor: "); scanf("%f", &val);
                    int res = buscar(&pilha, &val);
                    printf(res >= 0 ? "Encontrado\n" : "Não encontrado\n");
                }

                {
                    double tempo_op_micros = encerrar_tempo_micros();
                    long long tempo_op_nanos = encerrar_tempo_nanos();

                    formatar_tempo(tempo_op_micros, tempo_formatado);
                    printf("Tempo para buscar: %s\n", tempo_formatado);
                    printf("Tempo em nanosegundos: %lld ns\n", tempo_op_nanos);
                }

                break;

            case 4:
                pop(&pilha);

                {
                    double tempo_op_micros = encerrar_tempo_micros();
                    long long tempo_op_nanos = encerrar_tempo_nanos();

                    formatar_tempo(tempo_op_micros, tempo_formatado);
                    printf("Tempo para remover topo: %s\n", tempo_formatado);
                    printf("Tempo em nanosegundos: %lld ns\n", tempo_op_nanos);
                }

                break;

            case 5:
                remover_em_massa(&pilha, 0.8f);

                {
                    double tempo_op_micros = encerrar_tempo_micros();
                    long long tempo_op_nanos = encerrar_tempo_nanos();

                    formatar_tempo(tempo_op_micros, tempo_formatado);
                    printf("Tempo para remover em massa: %s\n", tempo_formatado);
                    printf("Tempo em nanosegundos: %lld ns\n", tempo_op_nanos);
                }

                break;

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opção inválida!\n");
        }

    } while (opcao != 0);

    liberar_pilha(&pilha);
    return 0;
}
