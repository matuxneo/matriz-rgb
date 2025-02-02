/*  -----------------------------
Autor: Matuzalem Guimaraes Leal
e-mail: matuinfo@gmail.com
Data:  2021-09-30
curso: EmbarcaTech 2025
Revisão: v2.0
---------------------------------*/

// Programa para controle de matriz de LEDs WS2812B (Neopixel) com Raspberry Pi Pico
// Exibe um número de 0 a 9 na matriz 5x5 com cores diferentes para cada dígito
// Utiliza botões A e B para incrementar e decrementar o número exibido
// Piscar o LED vermelho para indicar que o programa está em execução

// Inclusão de bibliotecas necessárias
#include <stdio.h> // Biblioteca padrão de entrada e saída
#include "pico/stdlib.h" // Biblioteca padrão do Pico
#include "hardware/pio.h" // Biblioteca de controle de PIO
#include "hardware/clocks.h" // Biblioteca de controle de clock
#include "hardware/gpio.h" // Biblioteca de controle de GPIO
#include "ws2818b.pio.h" // Programa para controle de LEDs WS2812B

// Definição de pinos
#define LED_R 13 // Pino do LED vermelho
#define LED_G 11 // Pino do LED verde
#define LED_B 12 // Pino do LED azul
#define LED_COUNT 25    // Número de LEDs na matriz
#define LED_PIN 7      // Pino de dados da matriz de LEDs
#define BUTTON_A 5 // Pino do botão A
#define BUTTON_B 6 // Pino do botão B

// Estrutura para armazenar valores de cor dos LEDs (Formato GRB)
typedef struct {   // Definição de uma estrutura de dados
    uint8_t G, R, B; // Componentes de cor
} pixel_t; // Nome do tipo de dados

typedef pixel_t npLED_t; // Definição de um novo tipo de dados
npLED_t leds[LED_COUNT]; // Vetor de LEDs

// Variáveis globais
PIO np_pio; // Ponteiro para a estrutura de controle do PIO
uint sm; // Número do state machine
volatile int current_number = 0; // Número exibido na matriz
volatile bool update_display = false; // Flag para atualização da matriz

// Inicializa a matriz de LEDs WS2812B
void npInit(uint pin) { // Função de inicialização
    uint offset = pio_add_program(pio0, &ws2818b_program);  // Adiciona o programa ao PIO
    np_pio = pio0; // Seleciona o PIO 0
    sm = pio_claim_unused_sm(np_pio, true); // Reivindica uma state machine
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f); // Inicializa o programa

    for (uint i = 0; i < LED_COUNT; i++) { // Inicializa todos os LEDs com cor preta
        leds[i] = (pixel_t){0, 0, 0}; // Cor preta
    }
}

// Define a cor de um LED específico na matriz
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) { // Função para definir a cor de um LED
    leds[index].R = r; // Define a cor vermelha
    leds[index].G = g; // Define a cor verde
    leds[index].B = b; // Define a cor azul
}

// Desliga todos os LEDs da matriz
void npClear() { // Função para desligar todos os LEDs
    for (uint i = 0; i < LED_COUNT; i++) { // Desliga todos os LEDs
        npSetLED(i, 0, 0, 0); // Cor preta
    }
}

// Envia os dados dos LEDs para a matriz
void npWrite() { // Função para enviar os dados para a matriz
    for (uint i = 0; i < LED_COUNT; i++) {  // Envia os dados de cada LED
        pio_sm_put_blocking(np_pio, sm, leds[i].G); // Envia a cor verde
        pio_sm_put_blocking(np_pio, sm, leds[i].R);  // Envia a cor vermelha
        pio_sm_put_blocking(np_pio, sm, leds[i].B); // Envia a cor azul
    }
    sleep_us(100); // Aguarda 100 ns
}

// Calcula o índice linear na matriz baseado nas coordenadas (x,y)
int getIndex(int x, int y) { // Função para calcular o índice linear
    return (y % 2 == 0) ? (24 - (y * 5 + x)) : (24 - (y * 5 + (4 - x))); // Cálculo do índice
}

// Exibe um número na matriz com cores diferentes
void displayNumber(int number) { // Função para exibir um número
    uint8_t colors[10][3] = {  // Cores para cada dígito         
        {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {255, 0, 255},
        {0, 255, 255}, {255, 165, 0}, {128, 0, 128}, {0, 128, 128}, {255, 255, 255}
    };

    static int numbers[10][5][5] = { // Representação de cada dígito
        {{1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1}}, // 0
        {{0,0,1,1,0}, {0,1,1,1,0}, {0,0,1,1,0}, {0,0,1,1,0}, {0,1,1,1,1}}, // 1
        {{1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1}}, // 2
        {{1,1,1,1,1}, {0,0,0,0,1}, {0,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1}}, // 3
        {{1,0,0,1,1}, {1,0,0,1,1}, {1,1,1,1,1}, {0,0,0,1,1}, {0,0,0,1,1}}, // 4
        {{1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1}}, // 5
        {{1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1}}, // 6
        {{1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0}}, // 7
        {{1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1}}, // 8
        {{1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1}}  // 9
    };

    npClear(); // Limpa a matriz
    for (int y = 0; y < 5; y++) {   // Exibe o número na matriz
        for (int x = 0; x < 5; x++) { // Percorre a matriz
            if (numbers[number][y][x]) { // Verifica se o LED deve ser aceso
                int pos = getIndex(x, y); // Calcula o índice linear
                npSetLED(pos, colors[number][0], colors[number][1], colors[number][2]); // Define a cor do LED
            }
        }
    }
    npWrite(); // Envia os dados para a matriz
}

// Função única para lidar com interrupções de ambos os botões
void gpio_irq_handler(uint gpio, uint32_t events) { // Função de interrupção
    static uint32_t last_press_time = 0; // Último tempo de pressão do botão
    uint32_t current_time = to_ms_since_boot(get_absolute_time()); // Tempo atual em ms

    if ((current_time - last_press_time) > 200) { // Verifica se o botão foi pressionado
        if (gpio == BUTTON_A) { // Verifica qual botão foi pressionado
            current_number = (current_number + 1) % 10; // Incrementa
             printf("Botão A pressionado, valor incrementado. Número atual: %d\n", current_number); // Mensagem para o botão A
        } else if (gpio == BUTTON_B) { // Verifica qual botão foi pressionado
            current_number = (current_number - 1 + 10) % 10; // Decrementa
            printf("Botão B pressionado, valor decrementado. Número atual: %d\n", current_number); // Mensagem para o botão B
        }
        update_display = true; // Atualiza a matriz
        last_press_time = current_time; // Atualiza o tempo da última pressão
    }
}

// Função de callback para piscar o LED vermelho continuamente
bool blinkRedLED(struct repeating_timer *t) {
    static bool led_state = false; // Estado atual do LED
    gpio_put(LED_R, led_state); // Alterna o estado do LED vermelho
    led_state = !led_state; // Inverte o estado do LED
    return true; // Mantém o temporizador ativo
}

// Função principal
int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    npInit(LED_PIN); // Inicializa a matriz de LEDs
    npClear(); // Limpa a matriz

    gpio_init(BUTTON_A); // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_A); // Habilita o pull-up
    
    gpio_init(BUTTON_B); // Inicializa o pino do botão B
    gpio_set_dir(BUTTON_B, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_B); // Habilita o pull-up

     // Configuração dos LEDs RGB
    gpio_init(LED_R); // Inicializa o pino do LED vermelho
    gpio_set_dir(LED_R, GPIO_OUT); // Define o pino como saída

    // Registra a mesma função de interrupção para ambos os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);  // Habilita a interrupção do botão A
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Habilita a interrupção do botão B

    displayNumber(current_number); // Exibe o número inicial

   // Criação de um temporizador para piscar o LED vermelho a cada 200ms (5Hz)
    struct repeating_timer timer;   // Estrutura de temporizador
    add_repeating_timer_ms(200, blinkRedLED, NULL, &timer); // Adiciona o temporizador

    while (true) { // Loop infinito
        if (update_display) { // Verifica se a matriz deve ser atualizada
            displayNumber(current_number); // Exibe o número
            update_display = false; // Reseta a flag
        }
        tight_loop_contents(); // Aguarda
    }
}
