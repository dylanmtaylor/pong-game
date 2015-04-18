/* Pong Game

Designed for play on any size screen. The game will resize to fit it correctly.
Our game has multiple bonus features that require a terminal supporting a color
display. Please play using a terminal that supports color such as gnome-terminal or xterm.
If connecting to the othello server to test the game, you should connect using PuTTY.

Authors: Dylan Taylor, Travis Hayes */
#include <curses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//removed useless garbage from these define statements.
#define	BLANK		' '
#define	DFL_SYMBOL	'O'
#define	TOP_ROW		2
#define	LEFT_EDGE	1
#define	X_TTM		3
#define	Y_TTM		3

/** the ping pong ball **/

struct ppball
{
    int	y_pos, x_pos,
        y_dir, x_dir;
    char	symbol ;

} ;


struct ppball the_ball ;

int ticks = 50;
int winrows, wincols;
int padsize = 5;
int padposL, padposR;
int lscore = 0;
int rscore = 0;
int paused = 0; //1 = true, 0 = false
int reset_after_score = 1;

/**  the main loop  **/

void set_up();
void redrawscreen();
void drawpaddles();
void movement();
void reset_ball();

int main()
{
    int c;

    set_up();

    while ( ( c = getch()) != 'q' )   /* type q key to end the program */
    {
        switch(c)
        {
            //movement. conditionals prevent paddels from leaving bounds
            //also prevents gameplay while paused
        case KEY_UP: //right paddel up
            if (padposR > 2 && !paused)
            {
                padposR--;
            }
            break;
        case KEY_DOWN: //right paddel down
            if (padposR < (winrows - padsize - 2) && !paused)
            {
                padposR++;
            }
            break;
        case 'a': //left paddel up
            if (padposL > 2 && !paused)
            {
                padposL--;
            }
            break;
        case 'z': //left paddel down
            if (padposL < (winrows - padsize - 2) && !paused)
            {
                padposL++;
            }
            break;
            //game functionality
        case ' ': //space pauses game
            paused = (paused) ? 0 : 1;
            break;
        case 'f': //faster
            if (ticks < 100)
            {
                ticks = ticks + 10;
            }
            break;
        case 's': //slower
            if (ticks > 10)
            {
                ticks = ticks - 10;
            }
            break;
        default:
            break;
        }
        napms(1000/ticks); //curses sleep function
        if (!paused)
        {
            movement();
        }
        redrawscreen();
    }

    endwin(); /* put back to normal	*/

}

void movement()
{
    //bounce logic
    mvaddch(the_ball.y_pos, the_ball.x_pos, ' '); //erase ball
    //bounce logic for the sides of the screen
    the_ball.x_dir *= (the_ball.x_pos + the_ball.x_dir < 2) ? -1 : 1;
    the_ball.y_dir *= (the_ball.y_pos + the_ball.y_dir < 2) ? -1 : 1;
    the_ball.x_dir *= (the_ball.x_pos + the_ball.x_dir > wincols - 1) ? -1 : 1;
    the_ball.y_dir *= (the_ball.y_pos + the_ball.y_dir > winrows - 1) ? -1 : 1;
    //make the ball move
    the_ball.x_pos += the_ball.x_dir;
    the_ball.y_pos += the_ball.y_dir;
    //draw the ball
    mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol);
    //scoring and physics for changing direction when hitting different parts of the paddle
    if (the_ball.x_pos <= 2 && the_ball.x_dir == -1) //for the left paddle
    {
        if ((the_ball.y_pos < padposL) || (the_ball.y_pos > padposL + padsize)) //doesn't hit paddle
        {
            bkgd( COLOR_PAIR(2));
            mvaddstr(0, wincols/2 - 5, "SCORED!");
            refresh();
            rscore++;
            the_ball.x_dir = 1;
            if (reset_after_score) reset_ball();
            napms(1200);
            bkgd( COLOR_PAIR(1));
        }
        else if (the_ball.y_pos > padposL + (padsize/2)) //greater half of paddle
        {
            the_ball.y_dir = 1;
        }
        else if (the_ball.y_pos < padposL + (padsize/2)) //lesser half of paddle
        {
            the_ball.y_dir = -1;
        } else { //straight on
            the_ball.y_dir = 0;
        }
    }
    else if (the_ball.x_pos > wincols -2 && the_ball.x_dir == 1) //checks for the right paddle
    {
        if ((the_ball.y_pos < padposR) || (the_ball.y_pos > padposR + padsize)) //doesn't hit paddle
        {

            bkgd( COLOR_PAIR(2));
            mvaddstr(0, wincols/2 - 5, "SCORED!");
            refresh();
            lscore++;
            the_ball.x_dir = -1;
            if (reset_after_score) reset_ball();
            napms(1200);
            bkgd( COLOR_PAIR(1));
        }
        else if (the_ball.y_pos > padposR + (padsize/2)) //greater half of paddle
        {
            the_ball.y_dir = 1;
        }
        else if (the_ball.y_pos < padposR + (padsize/2)) //lesser half of paddle
        {
            the_ball.y_dir = -1;
        } else { //straight on
            the_ball.y_dir = 0;
        }
    }
}

void reset_ball()
{
    mvaddch(the_ball.y_pos, the_ball.x_pos, ' '); //erase ball
    //put it back in the center with initial direction of up right
    the_ball.y_pos = winrows/2;
    the_ball.x_pos = wincols/2;
    the_ball.y_dir = 0;
    the_ball.x_dir = 1;
    paused = 1;
    mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol); //draw ball again
}

void drawpaddles()
{
    //modified to scale dynamically to the screen and draw the paddles correctly using
    //the variables given instead of hard-coded values (was annoying to switch this, but it works)
    int c;
    for(c=winrows; c--; )
    {
        if(c < 2 || c >= winrows-1)
            continue;
        // left paddle
        if(c >= padposL && c <= padposL + padsize)
            mvaddch(c, 0, ' ' | A_REVERSE);
        else
            mvaddch(c, 0, ' ');
        // right paddle
        if(c >= padposR && c <= padposR+padsize)
            mvaddch(c, wincols-1, ' ' | A_REVERSE);
        else
            mvaddch(c, wincols-1, ' ');
    }
}

void redrawscreen()
{
    //split from set_up provided with added functionality.
    move(1, 0);
    hline(ACS_HLINE, wincols);
    move( winrows-1, 0);
    hline(ACS_HLINE,  wincols);

    attroff(A_BOLD);
    drawpaddles();

    /* draw the score board */
    char scl[25]; //more than enough buffer space
    sprintf(scl, "Left Score: %02d", lscore);
    mvaddstr(0, 8, scl);
    char scr[25];
    sprintf(scr, "Right Score: %02d", rscore);
    mvaddstr(0, wincols-22,scr);

    char p[10];
    if (paused)
    {
        sprintf(p, "PAUSED!");
    }
    else
    {
        sprintf(p, "       ");
    }
    mvaddstr(0, wincols/2 - 5, p);

    refresh();
}

void set_up()
/*
 *	init structure and other stuff
 */
{

    initscr();
    start_color();
    refresh();
    //use putty,xterm or gnome-terminal to view this in color.
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); //default
    init_pair(2, COLOR_GREEN, COLOR_BLACK); //when player scores, change color momentarily
    bkgd( COLOR_PAIR(1));
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);

    getmaxyx(stdscr, winrows, wincols);

    the_ball.symbol = DFL_SYMBOL ;
    reset_ball();

    //center the paddles vertically.
    padposL = padposR = ((winrows/2)-(padsize/2));

    redrawscreen();

}
