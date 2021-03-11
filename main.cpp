#include <iostream>
#include <Windows.h>
#include <thread>
#include <time.h>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

const int screen_w = 50;
const int screen_h = 25;
int x_offset = 2;
int y_offset = 1;
int screen[screen_h][screen_w] = {}; //game screen
int score = 0;
bool snake_l = false;
bool snake_r = true;
bool snake_u = false;
bool snake_d = false;
int snake_len = 3; // initial snake size
int snake_tail_i = 12; //snake tail initial pos
int snake_tail_j = 3;
int apple_control = 1;


void gotoXY(int, int);
void start_game();
void set_border();
void draw_snake();
void listener();
void move_snake_tail(int, int);
bool move_snake_head(int, int, int, int);
int* apple_spawn();
void print_score();
void write_to_file();
int fit_write(int*);
void shift_left(string*, int);
void print_top_scores();


void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

int main() {
	ShowConsoleCursor(false);
	set_border();
	draw_snake();
	start_game();
	write_to_file();
	print_top_scores();
	return 0;
}

void start_game() {
	bool run = true;
	thread input(listener);
	Sleep(200);
	int tail_control = 2;
	int head_control = 2;
	int r = 0; int c = 0; // apple coordinates
	while (run) {
		int* apple_coord = apple_spawn();
		if (apple_coord != 0) {
			r = apple_coord[0];
			c = apple_coord[1];
		}
		tail_control = 1;
		head_control = 1;
		for (int i = 0; i < screen_h; i++) {
			for (int j = 0; j < screen_w; j++) {
				if (screen[i][j] == 1) {
					gotoXY(j + x_offset, i + y_offset);
					printf("%c[%dm#", 0x1B, 31);
				}
				else if (screen[i][j] == 2) {
					gotoXY(j + x_offset, i + y_offset);
					printf("%c[%dm|", 0x1B, 31);
				}
				else if (screen[i][j] == 3 || screen[i][j] == 4 || screen[i][j] == 5) {
					if (screen[i][j] == 3) {
						gotoXY(j + x_offset, i + y_offset);
						printf("%c[%dm#", 0x1B, 32);
					}
					if (screen[i][j] == 4) {
						if (tail_control == 1) {
							move_snake_tail(i, j);
							tail_control++;
							gotoXY(j + x_offset, i + y_offset);
							printf("%c[%dm#", 0x1B, 32);
						}
					}
					else if (screen[i][j] == 5) {
						if (head_control == 1) {
							head_control++;
							run = move_snake_head(i, j, r, c);
							gotoXY(j + x_offset, i + y_offset);
							printf("%c[%dm#", 0x1B, 32);
						}
					}	
				}
				else if (screen[i][j] == 6) {
					gotoXY(j + x_offset, i + y_offset);
					printf("%c[%dm#", 0x1B, 33);
				}
				else {
					gotoXY(j + x_offset, i + y_offset);
					std::cout << " ";
				}
			}
		}
		print_score();
		Sleep(80);
	}
	input.detach();
	gotoXY(x_offset + screen_w/2 - 7, y_offset + screen_h/2 - 5);
	std::cout << "GAME OVER";
}

void gotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void listener() {
	while (true) {
		if (GetAsyncKeyState(VK_LEFT) & 1) {
			snake_u = false; snake_d = false; snake_l = true; snake_r = false;
		}	
		else if (GetAsyncKeyState(VK_RIGHT) & 1) {
			snake_u = false; snake_d = false; snake_l = false; snake_r = true;
		}
		else if (GetAsyncKeyState(VK_UP) & 1) {
			snake_u = true; snake_d = false; snake_l = false; snake_r = false;
		}	
		else if (GetAsyncKeyState(VK_DOWN) & 1) {
			snake_u = false; snake_d = true; snake_l = false; snake_r = false;
		}	
	}
}

void set_border() {
	for (int i = 0; i < screen_h; i++) {
		for (int j = 0; j < screen_w; j++) {
			if (i == 0 || i == screen_h - 1)
				screen[i][j] = 1;
			else if (j == 0 || j == screen_w - 1)
				screen[i][j] = 2;
		}		
	}
}

void draw_snake() {
	screen[12][3] = 4; // tail denoted by 4
	screen[12][4] = 3;
	screen[12][5] = 5; //head denoted by 5
}

void move_snake_tail(int i, int j) {
	screen[i][j] = 0;
	if (screen[i][j + 1] == 3) //for right
		screen[i][j + 1] = 4;
	else if (screen[i - 1][j] == 3) //for up
		screen[i - 1][j] = 4;
	else if (screen[i][j - 1] == 3) //for left
		screen[i][j - 1] = 4;
	else if (screen[i + 1][j] == 3) //for right
		screen[i + 1][j] = 4;
}

bool move_snake_head(int i, int j, int r, int c) {
	screen[i][j] = 3;
	if (snake_r) {
		if (screen[i][j + 1] == 1 || screen[i][j + 1] == 2 || screen[i][j + 1] == 3 || screen[i][j + 1] == 4) // checking for collision
			return false;
		if (screen[i][j + 1] == 6) {  // found apple
			screen[i][j + 2] = 5;
			screen[i][j + 1] = 3;
			apple_control = 1;
			score += 10;
		} 
		else
			screen[i][j + 1] = 5;
	}	
	else if (snake_u) {
		if (screen[i - 1][j] == 1 || screen[i - 1][j] == 2 || screen[i - 1][j] == 3 || screen[i - 1][j] == 4) // checking for collision
			return false;
		if (screen[i - 1][j] == 6) {  // found apple
			screen[i - 2][j] = 5;
			screen[i - 1][j] = 3;
			apple_control = 1;
			score += 10;
		}
		else
			screen[i - 1][j] = 5;
	}	
	else if (snake_l) {
		if (screen[i][j - 1] == 1 || screen[i][j - 1] == 2 || screen[i][j - 1] == 3 || screen[i][j - 1] == 4) // checking for collision
			return false;
		if (screen[i][j - 1] == 6) {  // found apple
			screen[i][j - 2] = 5;
			screen[i][j - 1] = 3;
			apple_control = 1;
			score += 10;
		}
		else
			screen[i][j - 1] = 5;
	}
	else if (snake_d) {
		if (screen[i + 1][j] == 1 || screen[i + 1][j] == 2 || screen[i + 1][j] == 3 || screen[i + 1][j] == 4) // checking for collision
			return false;
		if (screen[i + 1][j] == 6) {  // found apple
			screen[i + 2][j] = 5;
			screen[i + 1][j] = 3;
			apple_control = 1;
			score += 10;
		}
		else
			screen[i + 1][j] = 5;
	}
	return true;
}

int* apple_spawn() {
	if (apple_control == 1) {
		srand(time(0));
		int r1 = rand() % 23;
		if (r1 == 0 || r1 == 1) r1 = 2;
		int r2 = rand() % 48;
		if (r2 == 0 || r2 == 1) r2 = 2;
		int coord[2] = { r1, r2 };
		screen[r1][r2] = 6; // apple denoted by 6
		apple_control = 0;
		return coord;
	}
	return 0;
}

void print_score() {
	gotoXY(x_offset + screen_w + 5, y_offset + 3);
	std::cout << "Snake Game :)";
	gotoXY(x_offset + screen_w + 5, y_offset + 5);
	std::cout << "Score: " << score;
}

void write_to_file() {
	int high_s[5] = {};
	string Nhigh_s[5];
	string line;

	char username[20];
	DWORD username_len = 20;
	GetUserNameA(username, &username_len);
	int k = 0;
	ifstream file_r;
	file_r.open("highscores.txt");
	if (file_r.is_open()) {
		while (getline(file_r, line)) {
			Nhigh_s[k] = line;
			string f_score;
			for (int i = 0; line[i] != ':'; i++) {
				f_score += line[i];
			}
			high_s[k] = stoi(f_score);
			k++;
		}
		file_r.close();
	}
	else std::cout << "File Could Not Be Opened" << endl;
	if (score > high_s[0]) {
		Nhigh_s[0] = "0";
		int index = fit_write(high_s);
		shift_left(Nhigh_s, index);
		ofstream file_w;
		file_w.open("highscores.txt");
		if (file_w.is_open()) {
			for (int i = 0; i < 5; i++) {
				if (Nhigh_s[i] == "0")
					file_w << score << ":" << username << "\n";
				else {
					file_w << Nhigh_s[i] << "\n";
				}
			}
			file_w.close();
		}
		else
			std::cout << "File could not be opened" << endl;
	}
}

int fit_write(int* high_s) {
	int i = 0;
	for (i = 1; i < 5; i++) {
		if (high_s[i] <= score) {
			continue;
		}
		else break;
	}
	return i - 1;
}

void shift_left(string* arr, int n) {
	for (int i = 0; i <= n; i++) {
		if (i != 4)
			arr[i] = arr[i + 1];
	}
	arr[n] = "0";
}

void print_top_scores() {
	string line;
	int i = 7;
	ifstream file_r;
	file_r.open("highscores.txt");
	if (file_r.is_open()) {
		gotoXY(x_offset + screen_w + 5, y_offset + i++);
		std::cout << "Top Scores:-";
		while (getline(file_r, line)) {
			gotoXY(x_offset + screen_w + 7, y_offset + i++);
			std::cout << line;
		}
	}
	gotoXY(0, y_offset + screen_h + 2);
}

			
