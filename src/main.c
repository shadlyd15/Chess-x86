#include "util.h"
#include "screen.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "timer.h"
#include "system.h"
#include "keyboard.h"
#include "fpu.h"
#include "Adafruit_GFX.h"
#include "pieces_gfx.h"
#include "game_gfx.h"
#include "chess_engine.h"
#include "game_manager.h"

#define FPS 3000
#define BUFFER_SIZE 4

game_t chess;
game_t * game = &chess;

void key_handler(int c){
    game_send_key(game, c);
}

void _main(u32 magic) {

    idt_init();
    isr_init();
    fpu_init();
    irq_init();
    screen_init();
    timer_init();

    keyboard_init(key_handler);
    Adafruit_GFX_Init(SCREEN_WIDTH, SCREEN_HEIGHT);

    u32 last_frame = 0;
    struct pieces_t pieces = get_pieces();
    
    while (true) {
        manage_game_states(game);

        const u32 now = (u32) timer_get();
        if ((now - last_frame) > (TIMER_TPS / FPS)) {
            last_frame = now;
            manage_rendering(game);
            screen_swap();
        }
    }
}
