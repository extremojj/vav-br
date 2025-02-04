#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definição dos pinos dos LEDs
#define LED_VERMELHO 11
#define LED_AMARELO 12
#define LED_VERDE 13

// Estados do semáforo
typedef enum {
    VERMELHO,
    AMARELO,
    VERDE
} EstadoSemaforo;

EstadoSemaforo estadoAtual = VERMELHO; // Estado inicial

// Função de callback para alternar os estados do semáforo
bool callback_temporizador(struct repeating_timer *t) {
    switch (estadoAtual) {
        case VERMELHO:
            gpio_put(LED_VERMELHO, 0);  // Apaga o LED vermelho
            gpio_put(LED_AMARELO, 1);   // Acende o LED amarelo
            estadoAtual = AMARELO;
            break;
        case AMARELO:
            gpio_put(LED_AMARELO, 0);   // Apaga o LED amarelo
            gpio_put(LED_VERDE, 1);     // Acende o LED verde
            estadoAtual = VERDE;
            break;
        case VERDE:
            gpio_put(LED_VERDE, 0);     // Apaga o LED verde
            gpio_put(LED_VERMELHO, 1);  // Acende o LED vermelho
            estadoAtual = VERMELHO;
            break;
    }
    return true; // Mantém o temporizador ativo
}

int main() {
    stdio_init_all();

    // Configuração dos pinos dos LEDs como saída
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    // Inicializa o semáforo com o LED vermelho aceso
    gpio_put(LED_VERMELHO, 1);

    // Configuração do temporizador periódico para chamar a função de callback a cada 3 segundos
    struct repeating_timer temporizador;
    add_repeating_timer_ms(3000, callback_temporizador, NULL, &temporizador);

    // Loop principal para manter o programa ativo
    while (true) {
        sleep_ms(1000); // Pausa a execução por 1 segundo
    }

    return 0;
}
