#ifndef SCREEN_H
#define SCREEN_H

#include "types.h"

void move_cursor(u8 x, u8 y);

/*
 * Scroll the screen by n lines.
 */
void screen_scroll_up(u32 n);

/* Print the given character in the screen. */
void screen_putc(uchar c);

/*
 * Print the given string in the screen.
 */
void screen_puts(char *str);

#endif
