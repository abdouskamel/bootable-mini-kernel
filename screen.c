#include "screen.h"
#include "asm.h"

#define RAM_SCREEN 0xb8000
#define ROW_SIZE 50
#define COL_SIZE 160
#define MEM_SIZE (ROW_SIZE * COL_SIZE * 2)
#define SCREEN_LIM (RAM_SCREEN + MEM_SIZE)

// Current row and column position of the cursor
static u8 cR = 0;
static u8 cC = 0;

void move_cursor(u8 x, u8 y)
{
    u16 c_pos = y * (COL_SIZE / 2) + x;

    outb(0x3d4, 0x0f);
    outb(0x3d5, (u8)c_pos);

    outb(0x3d4, 0xe);
    outb(0x3d5, (u8)(c_pos >> 8));
}

/*
 * Scroll the screen by n lines.
 */
void screen_scroll_up(u32 n)
{
    uchar *s_ptr;

    for (s_ptr = (uchar *)RAM_SCREEN; s_ptr < (uchar *)SCREEN_LIM; s_ptr += 2)
    {
        uchar *tmp = s_ptr + n * COL_SIZE;
        if (tmp < (uchar *)SCREEN_LIM)
        {
            *s_ptr = *tmp;
            *(s_ptr + 1) = *(tmp + 1);
        }

        else
        {
            *s_ptr = '\0';
            *(s_ptr + 1) = 0;
        }
    }

    if (cR <= n)
        cR = 0;
    else
        cR -= n;

    cC = 0;
    move_cursor(cC, cR);
}

/* Print the given character in the screen. */
void screen_putc(uchar c)
{
    uchar *s_ptr;

    switch (c)
    {
    case '\n':
        cR++;
        cC = 0;
        break;

    case '\t':
        /* TODO */
        break;

    case '\r':
        cC = 0;
        break;

    default:
        s_ptr = (uchar *)RAM_SCREEN + COL_SIZE * cR + 2 * cC;
        *s_ptr = c;
        *(s_ptr + 1) = 0x07;

        cC++;
        if (cC >= COL_SIZE / 2)
        {
            cC = 0;
            cR++;
        }
    }

    if (cR >= ROW_SIZE / 2)
        screen_scroll_up(1);

    else
        move_cursor(cC, cR);
}

/*
 * Print the given string in the screen.
 */
void screen_puts(char *str)
{
    while (*str != '\0')
        screen_putc(*(str++));
}
