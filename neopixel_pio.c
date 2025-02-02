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
#include <stdio.h> // Para funções de entrada/saída
#include "pico/stdlib.h" // SDK básico do Raspberry Pi Pico
#include "hardware/pio.h" // Para programação de PIO (Peripheral I/O)
#include "hardware/clocks.h" // Controle de clocks
#include "hardware/gpio.h" // Controle de GPIO
#include "ws2818b.pio.h" // Programa PIO para controle de LEDs WS2812B

// Definição de pinos
#define LED_R 13    // Pino do LED Vermelho do RGB
#define LED_G 11    // Pino do LED Verde do RGB
#define LED_B 12    // Pino do LED Azul do RGB
#define LED_COUNT 25// Número total de LEDs na matriz 5x5
#define LED_PIN 7   // Pino de controle da matriz de LEDs
#define BUTTON_A 5  // Pino do Botão A
#define BUTTON_B 6  // Pino do Botão B

// Estrutura para armazenar valores de cor dos LEDs (Formato GRB)
struct pixel_t {
    uint8_t G, R, B; // Ordem específica para WS2812B (Green, Red, Blue)
};
typedef struct pixel_t pixel_t; // Define o tipo de dado pixel_t
typedef pixel_t npLED_t; // Define o tipo de dado npLED_t

// Variáveis globais
npLED_t leds[LED_COUNT]; // Buffer de estados dos LEDs
PIO np_pio;              // Controlador PIO utilizado
uint sm;                 // Máquina de estados (state machine) do PIO
volatile int current_number = 0;  // Número atual exibido na matriz
volatile bool update_display = false; // Flag para atualizar a exibição

// Inicialização da matriz de LEDs
void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program); // Adiciona o programa ao controlador PIO
    np_pio = pio0; // Controlador PIO utilizado
    sm = pio_claim_unused_sm(np_pio, true); // Reivindica uma máquina de estados (state machine) livre
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f); // Inicializa o programa com 800kHz
    for (uint i = 0; i < LED_COUNT; ++i) { // Inicializa o buffer de LEDs
        leds[i].R = 0; // Cor vermelha
        leds[i].G = 0; // Cor verde
        leds[i].B = 0; // Cor azul
    }
}

// Define a cor de um LED específico
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r; // Define a cor vermelha
    leds[index].G = g; // Define a cor verde
    leds[index].B = b; // Define a cor azul
}

// Desliga todos os LEDs da matriz
void npClear() { // Desliga todos os LEDs
    for (uint i = 0; i < LED_COUNT; ++i) // Desliga todos os LEDs
        npSetLED(i, 0, 0, 0); // Cor preta (desligado)
}

// Envia os dados dos LEDs para a matriz
void npWrite() {
    for (uint i = 0; i < LED_COUNT; ++i) { // Envia os dados de cada LED
        pio_sm_put_blocking(np_pio, sm, leds[i].G); // Envia a cor verde
        pio_sm_put_blocking(np_pio, sm, leds[i].R); // Envia a cor vermelha
        pio_sm_put_blocking(np_pio, sm, leds[i].B); // Envia a cor azul
    }
    sleep_us(100); // Aguarda 100us para garantir que todos os dados foram enviados
}

// Calcula o índice linear na matriz baseado nas coordenadas (x,y)
int getIndex(int x, int y) {
    return (y % 2 == 0) ? (24 - (y * 5 + x)) : (24 - (y * 5 + (4 - x))); // Mapeamento da matriz 5x5
}

// Exibe um número na matriz com cores diferentes para cada dígito
void displayNumber(int number) {
    uint8_t colors[10][3] = { // Cores para cada dígito
        {255, 0, 0},    // 0: Vermelho
        {0, 255, 0},    // 1: Verde
        {0, 0, 255},    // 2: Azul
        {255, 255, 0},  // 3: Amarelo
        {255, 0, 255},  // 4: Magenta
        {0, 255, 255},  // 5: Ciano
        {255, 165, 0},  // 6: Laranja
        {128, 0, 128},  // 7: Roxo
        {0, 128, 128},  // 8: Verde-azulado
        {255, 255, 255} // 9: Branco
    };
    static int numbers[10][5][5] = { // Representação de cada dígito na matriz 5x5
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
    for (int y = 0; y < 5; y++) { // Exibe o número na matriz
        for (int x = 0; x < 5; x++) { // Percorre cada LED da matriz
            if (numbers[number][y][x]) { // Verifica se o LED deve ser aceso
                int pos = getIndex(x, y); // Calcula o índice linear do LED
                npSetLED(pos, colors[number][0], colors[number][1], colors[number][2]); // Define a cor do LED
            }
        }
    }
    npWrite(); // Atualiza a matriz de LEDs
}

// Função de debounce e tratamento de interrupção para o botão A
void button_a_irq_handler(uint gpio, uint32_t events) { 
    static uint32_t last_press_time = 0; // Último tempo de pressionamento
    uint32_t current_time = to_ms_since_boot(get_absolute_time()); // Tempo atual em ms
    if ((current_time - last_press_time) > 200) { // Debounce de 200ms
        current_number = (current_number + 1) % 10; // Incrementa o número atual
        update_display = true; // Sinaliza que a exibição deve ser atualizada
        last_press_time = current_time; // Atualiza o tempo de pressionamento
    }
    gpio_acknowledge_irq(gpio, events); // Limpa o estado da interrupção
}

// Função de debounce e tratamento de interrupção para o botão B
void button_b_irq_handler(uint gpio, uint32_t events) { 
    static uint32_t last_press_time = 0; // Último tempo de pressionamento
    uint32_t current_time = to_ms_since_boot(get_absolute_time()); // Tempo atual em ms
    if ((current_time - last_press_time) > 200) { // Debounce de 200ms
        current_number = (current_number - 1 + 10) % 10;
        update_display = true; // Sinaliza que a exibição deve ser atualizada
        last_press_time = current_time; // Atualiza o tempo de pressionamento
    }
    gpio_acknowledge_irq(gpio, events); // Limpa o estado da interrupção
}

// Função de callback para piscar o LED vermelho
int64_t blinkRedLED(alarm_id_t id, void *user_data) {
    static bool led_state = false; // Estado atual do LED
    gpio_put(LED_R, led_state); // Alterna o estado do LED vermelho
    led_state = !led_state; // Inverte o estado do LED
    return 200 * 1000; // Retorna 200ms para próxima execução (5Hz)
}

// Função principal
int main() {
    stdio_init_all(); // Inicializa todo o hardware básico
    
    // Configuração inicial da matriz de LEDs
    npInit(LED_PIN); // Inicializa a matriz de LEDs
    npClear(); // Limpa a matriz

    // Configuração dos botões
    gpio_init(BUTTON_A); // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_A); // Habilita o resistor de pull-up interno
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_a_irq_handler); // Habilita a interrupção de pull-down

    gpio_init(BUTTON_B); // Inicializa o pino do botão B
    gpio_set_dir(BUTTON_B, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_B); // Habilita o resistor de pull-up interno
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_b_irq_handler); // Habilita a interrupção de pull-down

    // Configuração dos LEDs RGB
    gpio_init(LED_R); // Inicializa o pino do LED vermelho
    gpio_set_dir(LED_R, GPIO_OUT); // Define o pino como saída
    gpio_init(LED_G); // Inicializa o pino do LED verde
    gpio_set_dir(LED_G, GPIO_OUT); // Define o pino como saída
    gpio_init(LED_B); // Inicializa o pino do LED azul
    gpio_set_dir(LED_B, GPIO_OUT); // Define o pino como saída

    displayNumber(current_number); // Exibe o número inicial (0)

    // Configura o alarme para piscar o LED vermelho
    add_alarm_in_ms(200, blinkRedLED, NULL, true);

    // Loop principal
    while (true) {
        if (update_display) { // Atualiza a exibição apenas quando necessário
            displayNumber(current_number); // Exibe o número atual
            update_display = false; // Reseta a flag de atualização
        }
        tight_loop_contents(); // Mantém o processador ocupado, mas permite que interrupções ocorram
    }
}