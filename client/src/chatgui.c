#include "chatgui.h"


void chatgui_init(struct chatgui *gui){
    initscr();  //initialize and clear screen
    cbreak();   //ctrl-c to exit program
    memset(gui, 0, sizeof(struct chatgui));

    //output window box
    gui->out_win_h = (2*LINES)/3 - 4;   
    gui->out_win_w = COLS-8;   
    gui->out_win = newwin(
            gui->out_win_h,
            gui->out_win_w,
            4,4);  //(height, width, start_y, start_x)

    refresh();
    wrefresh(gui->out_win);  //refresh window


    //input window box
    gui->in_win_h = (LINES)/3-8;   
    gui->in_win_w = COLS-8;   
    //output window box
    gui->in_win = newwin(
            gui->in_win_h,
            gui->in_win_w,
            gui->out_win_h +4,
            4);  //(height, width, start_y, start_x)

    refresh();
    wrefresh(gui->in_win);  //refresh window
}

//draws message window box, scans user string and clears the window box.
//Returns the inputed string
void chatgui_input(struct chatgui *gui, char *buf, size_t n){
    curs_set(1);    //visible cursor
    mvwprintw(gui->in_win, 1, 1, "> ");
    wgetnstr(gui->in_win, buf, n);
    wclear(gui->in_win);
    wrefresh(gui->in_win);
    curs_set(0);    //invisible cursor
}

//prints received message to window.
void chatgui_out(struct chatgui *gui, char *msg){
    wprintw(gui->out_win, "%s\n", msg);
    wrefresh(gui->out_win);
}

//draw borders and move cursor everytime a message is received or sent
void chatgui_render(struct chatgui *gui){
    curs_set(0);    //invisible cursor
    box(gui->out_win, 0, 0);
    wrefresh(gui->out_win);

    box(gui->in_win, 0, 0);
    wrefresh(gui->in_win);
    refresh();
}
