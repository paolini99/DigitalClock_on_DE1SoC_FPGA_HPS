#include <stdint.h>

// Base addresses
#define TIMER_BASE_ADDR 0xFF200080
#define DISPLAY_BASE_ADDR 0xFF200098
#define ENCODER_BASE_ADDR 0xFF200000
#define BUTTON_BASE_ADDR 0xFF200040

// Timer registers
#define TIMER_READ_REG *(volatile uint32_t *)(TIMER_BASE_ADDR)

// Display registers
#define DISPLAY_WRITE_REG *(volatile uint32_t *)(DISPLAY_BASE_ADDR)

// Encoder register
#define ENCODER_READ_REG *(volatile int32_t *)(ENCODER_BASE_ADDR)

// Button register
#define BUTTON_READ_REG *(volatile uint32_t *)(BUTTON_BASE_ADDR)

// Variabili globali
volatile uint8_t hours = 0;
volatile uint8_t minutes = 0;
volatile uint8_t seconds = 0;
volatile int32_t last_encoder_value = 0;
volatile uint8_t encoder_mode = 3; // 0: secondi, 1: minuti, 2: ore, 3: non selezionato
volatile uint8_t blink_state = 0;  // 0: spento, 1: acceso

// Prototipi
void update_display();
void handle_button_click();
void check_encoder();
void update_time();
void toggle_blink_state();
int32_t read_encoder();

// Funzione per leggere il valore dell'encoder
int32_t read_encoder() {
    return ENCODER_READ_REG / 4; // Dividi per 4 se necessario per il tuo sistema
}

// Funzione per aggiornare l'ora
void update_time() {
    seconds++;
    if (seconds == 60) {
        seconds = 0;
        minutes++;
        if (minutes == 60) {
            minutes = 0;
            hours++;
            if (hours == 24) {
                hours = 0;
            }
        }
    }
    // Aggiorna il lampeggio a ogni aggiornamento dei secondi
    if (encoder_mode != 3) { // Lampeggio solo se una modalità è selezionata
        toggle_blink_state();
    }
}

// Funzione per gestire il lampeggio
void toggle_blink_state() {
    blink_state = !blink_state; // Cambia stato a ogni secondo
}

// Funzione per gestire il clic del tasto
void handle_button_click() {
    static uint8_t last_button_state = 0;
    uint8_t current_button_state = (BUTTON_READ_REG & 0x1);

    if (current_button_state && !last_button_state) {
        encoder_mode = (encoder_mode + 1) % 4; // Cicla tra 0, 1, 2, 3
    }

    last_button_state = current_button_state;
}

// Funzione per gestire l'encoder
void check_encoder() {
    if (encoder_mode == 3) return; // Nessuna azione se non selezionato

    int32_t current_encoder_value = read_encoder();

    if (current_encoder_value != last_encoder_value) {
        int32_t delta = current_encoder_value - last_encoder_value;

        if (encoder_mode == 0) { // Modifica secondi
            seconds = (seconds + delta + 60) % 60;
        } else if (encoder_mode == 1) { // Modifica minuti
            minutes = (minutes + delta + 60) % 60;
        } else if (encoder_mode == 2) { // Modifica ore
            hours = (hours + delta + 24) % 24;
        }

        last_encoder_value = current_encoder_value;

        // Aggiorna immediatamente il display
        update_display();
    }
}

// Funzione per aggiornare i display a 7 segmenti con lampeggio
void update_display() {
    uint32_t display_value = 0;

    // Lampeggia le ore se la modalità encoder è attiva sulle ore
    if (encoder_mode == 2 && !blink_state) {
        display_value |= (0xF << 20); // Decine ore
        display_value |= (0xF << 16); // Unita ore
    } else {
        display_value |= ((hours / 10) & 0xF) << 20; // Decine ore
        display_value |= (hours % 10) << 16;        // Unita ore
    }

    // Lampeggia i minuti se la modalità encoder è attiva sui minuti
    if (encoder_mode == 1 && !blink_state) {
        display_value |= (0xF << 12); // Decine minuti
        display_value |= (0xF << 8);  // Unita minuti
    } else {
        display_value |= ((minutes / 10) & 0xF) << 12; // Decine minuti
        display_value |= (minutes % 10) << 8;          // Unita minuti
    }

    // Lampeggia i secondi se la modalità encoder è attiva sui secondi
    if (encoder_mode == 0 && !blink_state) {
        display_value |= (0xF << 4); // Decine secondi
        display_value |= 0xF;       // Unita secondi
    } else {
        display_value |= ((seconds / 10) & 0xF) << 4;  // Decine secondi
        display_value |= (seconds % 10);              // Unita secondi
    }

    // Scrive il valore nei registri del display
    DISPLAY_WRITE_REG = display_value;
}

// Funzione principale
int main() {
    uint32_t last_timer_value = 0;

    while (1) {
        // Leggi il valore corrente del timer
        uint32_t current_timer_value = TIMER_READ_REG;

        // Aggiorna l'orologio se il valore del timer è cambiato
        if (current_timer_value != last_timer_value) {
            last_timer_value = current_timer_value;

            // Aggiorna il tempo
            update_time();

            // Aggiorna il display
            update_display();
        }

        // Gestisci il clic del tasto
        handle_button_click();

        // Controlla l'encoder
        check_encoder();
    }

    return 0;
}