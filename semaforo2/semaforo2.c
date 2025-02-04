#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define LED_AZUL 11
#define LED_VERMELHO 12
#define LED_VERDE 13
#define BOTAO 5

typedef enum {
    ESTADO_DESLIGADO,
    ESTADO_TODOS_LIGADOS,
    ESTADO_VERMELHO_E_VERDE,
    ESTADO_SOMENTE_VERDE
} Estado;

static Estado estado_atual = ESTADO_DESLIGADO;
static bool sequencia_ativa = false;

void configurar_gpio() {
    gpio_init(LED_AZUL);
    gpio_init(LED_VERMELHO);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(BOTAO);
    gpio_set_dir(BOTAO, GPIO_IN);
    gpio_pull_up(BOTAO);
}

void desligar_leds() {
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_VERDE, 0);
}

int64_t alarme_callback(alarm_id_t id, void *user_data) {
    switch (estado_atual) {
        case ESTADO_TODOS_LIGADOS:
            gpio_put(LED_AZUL, 0);
            estado_atual = ESTADO_VERMELHO_E_VERDE;
            break;
        case ESTADO_VERMELHO_E_VERDE:
            gpio_put(LED_VERMELHO, 0);
            estado_atual = ESTADO_SOMENTE_VERDE;
            break;
        case ESTADO_SOMENTE_VERDE:
            gpio_put(LED_VERDE, 0);
            estado_atual = ESTADO_DESLIGADO;
            sequencia_ativa = false;
            return 0;
        default:
            return 0;
    }
    add_alarm_in_ms(3000, alarme_callback, NULL, false);
    return 0;
}

void iniciar_sequencia() {
    if (!sequencia_ativa) {
        sequencia_ativa = true;
        estado_atual = ESTADO_TODOS_LIGADOS;
        gpio_put(LED_AZUL, 1);
        gpio_put(LED_VERMELHO, 1);
        gpio_put(LED_VERDE, 1);
        add_alarm_in_ms(3000, alarme_callback, NULL, false);
    }
}

// Professor pediu que o debounce fosse opcional
bool debounce_botao() {
    static uint32_t ultimo_tempo = 0;
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if (tempo_atual - ultimo_tempo > 200) {
        ultimo_tempo = tempo_atual;
        return true;
    }
    return false;
}

int main() {
    stdio_init_all();
    configurar_gpio();
    desligar_leds();
    
    while (true) {
        if (!gpio_get(BOTAO) && debounce_botao()) {
            iniciar_sequencia();
        }
        sleep_ms(10);
    }
    return 0;
}
