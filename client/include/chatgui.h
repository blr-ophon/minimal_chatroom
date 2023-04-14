#ifndef CHATGUI_H
#define CHATGUI_H

#include <curses.h>
#include <string.h>

struct chatgui{
    WINDOW *in_win;
    int in_win_x;
    int in_win_y;
    int in_win_w;
    int in_win_h;
    WINDOW *out_win;
    int out_win_x;
    int out_win_y;
    int out_win_w;
    int out_win_h;
};


//TODO: must implement window scrolling in all windows (in and out)
void chatgui_init(struct chatgui *gui); //initialized values based on user hardware


void chatgui_input(struct chatgui *gui, char *buf, size_t n);
//draws message window box, scans user string and clears the window box.
//Returns the inputed string


void chatgui_out(struct chatgui *gui, char *msg);
//prints received message to window.

void chatgui_render(struct chatgui *gui);


#endif
