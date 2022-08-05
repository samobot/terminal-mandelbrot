//Mandelbrot Set for terminal using ncurses
#include <ncurses.h>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

const string TITLE = "Mandelbrot Set";
const double xMin = -1.0;
const double xMax = 1.0;
const double yMin = -1.0;
const double yMax = 1.0;

vector<vector<bool>> board;

class IDouble {
	public:
		double r;
		double i;
};

IDouble mult(IDouble a, IDouble b) {
	IDouble c;
	c.r = (a.r*b.r)-(a.i*b.i);
	c.i = (a.r*b.i)+(a.i*b.r);
	return c;
}

IDouble sq(IDouble a) {
	return mult(a, a);
}

IDouble add(IDouble a, IDouble b) {
	IDouble c;
	c.r = a.r+b.r;
	c.i = a.i+b.i;
	return c;
}

double distFromOrgin(IDouble a) {
	return sqrt(a.r*a.r + a.i*a.i);
}

IDouble absoluteToRelative(int x, int y, int xmax, int ymax, double xrmin, double xrmax, double yrmin, double yrmax) {
	double pctx = x/(double)xmax;
	double pcty = y/(double)ymax;
	double finalr = xrmin + ((xrmax - xrmin)*pctx);
	double finali = yrmin + ((yrmax - yrmin)*pcty);
	IDouble c;
	c.r = finalr;
	c.i = finali;
	return c;
}

bool isPart(IDouble c) {
	IDouble z = (IDouble) {0,0};
	for(int i = 0; i < 500; i++) {
		z = add(sq(z), c);
		if(distFromOrgin(z) > 5) break;
	}
	if(distFromOrgin(z) < 5) return true;
	else return false;
}

vector<vector<bool>> drawBoard(vector<vector<bool>> board, int TERM_HEIGHT, int TERM_WIDTH, double startx, double starty, double endx, double endy) {
	IDouble calc;
	for(int y = 0; y < TERM_HEIGHT*2; y++) {
		for(int x = 0; x < TERM_WIDTH; x++) {
			calc = absoluteToRelative(x, y, TERM_WIDTH, TERM_HEIGHT*2, startx, endx, starty, endy);
			board[y][x] = isPart(calc);
		}
	}
	return board;
}

int paintBuffer(WINDOW* scr, int TERM_HEIGHT, int TERM_WIDTH) {
	
	mvprintw(0, (TERM_WIDTH/2)-(TITLE.length()/2), TITLE.c_str());

	//mvprintw(1, 1, "(%d, %d)", termMinX, termMinY);
	//mvprintw(2, 1, "(%d, %d)", termMaxX, termMaxY);
	//mvprintw(1, 1, "%dx%d", TERM_WIDTH, TERM_HEIGHT);

	for(int y = 0; y < TERM_HEIGHT; y++) {
		for(int x = 0; x < TERM_WIDTH; x++) {
			if(board[2*y][x]) {
				if(board[2*y+1][x]) {
					wattron(scr, COLOR_PAIR(1));
					mvwaddstr(scr, y, x, "█");
					wattroff(scr, COLOR_PAIR(1));
				} else {
					wattron(scr, COLOR_PAIR(1));
          mvwaddstr(scr, y, x, "▀");
          wattroff(scr, COLOR_PAIR(1));
				}
			} else {
				if(board[2*y+1][x]) {
					wattron(scr, COLOR_PAIR(1));
          mvwaddstr(scr, y, x, "▄");
          wattroff(scr, COLOR_PAIR(1));
				} else {
					mvwaddstr(scr, y, x, " ");
				}
			}
		}
	}

	wrefresh(scr);

	return 0;
}

int main() {
	setlocale(LC_ALL, "");

	initscr();
	cbreak();
	noecho();
	
	start_color();
	init_color(COLOR_BLACK, 102, 78, 137);
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);

	int termMinY = 0;
	int termMinX = 0;
	int termMaxY = 0;
	int termMaxX = 0;

	getbegyx(stdscr, termMinY, termMinX);
	getmaxyx(stdscr, termMaxY, termMaxX);
	const int TERM_HEIGHT = termMaxY - termMinY - 1;
	const int TERM_WIDTH = termMaxX - termMinX;
	
	WINDOW* mainwin = newwin(TERM_HEIGHT, TERM_WIDTH, 1, 0);
	refresh();

	board = vector<vector<bool>> (TERM_HEIGHT*2, vector<bool> (TERM_WIDTH, false));
	
	double startx = -2;
	double endx = 0.5;
	double starty = -0.95;
	double endy = 0.95;

	board = drawBoard(board, TERM_HEIGHT, TERM_WIDTH, startx, starty, endx, endy);

	paintBuffer(mainwin, TERM_HEIGHT, TERM_WIDTH);
	
	/*box(stdscr, 0, 0);
	
	mvprintw(0, (TERM_WIDTH/2)-(TITLE.length()/2), TITLE.c_str());
	
	mvprintw(1, 1, "(%d, %d)", termMinX, termMinY);
	mvprintw(2, 1, "(%d, %d)", termMaxX, termMaxY);
	mvprintw(3, 1, "%dx%d", TERM_WIDTH, TERM_HEIGHT);
	*/

	int c = getch();
	while(c != 'q') {
		switch(c) {
			case 'a':
				startx+= 0.02*(startx - endx);
				endx+= 0.02*(startx - endx);
				break;
			case 'd':
				startx+= 0.02*(endx - startx);
				endx+= 0.02*(endx - startx);
				break;
			case 'w':
				starty+= 0.02*(starty - endy);
				endy+= 0.02*(starty - endy);
				break;
			case 's':
				starty+= 0.02*(endy - starty);
				endy+= 0.02*(endy - starty);
				break;
			case '-':
				startx+= (endx-startx)*0.02;
				endx-= (endx-startx)*0.02;
				starty+= (endy-starty)*0.02;
				endy-= (endy-starty)*0.02;
				break;
			case '=':
				startx-= (endx-startx)*0.02;
				endx+= (endx-startx)*0.02;
				starty-= (endy-starty)*0.02;
				endy+= (endy-starty)*0.02;
				break;
		}
		board = drawBoard(board, TERM_HEIGHT, TERM_WIDTH, startx, starty, endx, endy);
		paintBuffer(mainwin, TERM_HEIGHT, TERM_WIDTH);
		c = getch();
	}

	endwin();

	return 0;
}
