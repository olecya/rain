// консольный файловый пейджер pix.c wch.c hash.c ldir.c dir.h my.h
#include "my.h"
#include <wait.h>
#define SIZ 256

bool flag; //урать из глобальной после создания 3 колонки

int main() {
	struct dirent **entry, **entry_p;
	CURS = 0;
	OFFSET = 0;
	flag = true; //поставить тип bool - убать в локальную
	char *current;
	char buf[SIZ];
	int key = 0;
	setlocale(LC_ALL, "");
	signal(SIGWINCH, sig_handler);
	signal(SIGINT, sig_handler);
	start_ncurses();
//	entry = pwd(&RAW, getcwd(buf, SIZ), flag);
//	entry_p = pwd(&PREV, dirname(buf), flag);
	entry = pwd(&RAW, ".", flag);
	entry_p = pwd(&PREV, "..", flag);
	if(RAW.ar_len < LINES-2)
		MENULEN = RAW.ar_len;
	else
		MENULEN = LINES-2;
	cadr();
	cadr_p();
	while((key = getch()) != ERR) {
		switch(key) {
			case 'j':
				if(RAW.ar_len > 1) {
					if (CURS < RAW.ar_len - 1) {
						CURS++;
						if (CURS > OFFSET + MENULEN - 1)
							OFFSET++;
						cadr();
					}
				}
				break;
			case 'k':
				if(RAW.ar_len > 1) {
					if (CURS) {
						CURS--;
						if (CURS < OFFSET)
							OFFSET--;
						cadr();
					}
				}
				break;
			case 'G':
				if(RAW.ar_len > 1) {
					CURS = RAW.ar_len-1;
					OFFSET = 0;
					if(CURS > OFFSET + MENULEN-1)
						OFFSET = CURS - MENULEN+1;
					cadr();
				}
				break;
			case 'g':
				if(RAW.ar_len > 1) {
					CURS = 0;
					OFFSET = 0;
					cadr();
				}
				break;
			case 'h':
				if(!strcasecmp(getcwd(buf, SIZ), "/")) {
//					searchHash(buf, RAW.ar[CURS]->d_name);
					break;
				}
				if(RAW.ar_len > 1) //было > 0
					searchHash(buf, RAW.ar[CURS]->d_name); //запишем или перезапишем базу
				CURS = 0;
				OFFSET = 0;
				MENULEN = 0;
				current = strdup(basename(buf)); //нужна запись по хешу?!
//				chdir(dirname(buf));
				chdir("..");
				reset(entry);
//				entry = pwd(&RAW, buf, flag);
				entry = pwd(&RAW, ".", flag);
				reset_p(entry_p);
//				entry_p = pwd(&PREV, dirname(buf), flag);
				entry_p = pwd(&PREV, "..", flag);

				for(int i = 0; i < RAW.ar_len; i++) {
					if(!strcasecmp(current, RAW.ar[i]->d_name)) { //нужен поиск по хешу?!
						CURS = i;
						break;
					}
				}

				if(RAW.ar_len < LINES-2)
					MENULEN = RAW.ar_len;
				else
					MENULEN = LINES-2;
				if(CURS > OFFSET + MENULEN-1)
					OFFSET = CURS - MENULEN+1;

				cadr();
				cadr_p();
				free(current);
				break;
			case 'l':
				if(RAW.ar_len > 0) {
					if(ACCESS)
						break;
					reset_p(entry_p);
					entry_p = pwd(&PREV, ".", flag);
					chdir(RAW.ar[CURS]->d_name);
//					getcwd(buf, SIZ);
					CURS = 0;
					OFFSET = 0;
					MENULEN = 0;
					reset(entry);
//					entry = pwd(&RAW, buf, flag);
					entry = pwd(&RAW, ".", flag);
					if((current = searchHash(getcwd(buf, SIZ), "")) != NULL) {
						for(int i = 0; i < RAW.ar_len; i++) {
							if(!strcasecmp(current, RAW.ar[i]->d_name)) {
								CURS = i;
								break;
							}
						}
					}

					if(RAW.ar_len < LINES-2)
						MENULEN = RAW.ar_len;
					else
						MENULEN = LINES-2;
					if(CURS > OFFSET + MENULEN-1)
						OFFSET = CURS - MENULEN+1;
					cadr();
					cadr_p();
				}
				break;
			case 'a':
				flag = (flag)? false: true;	
				current = strdup(RAW.ar[CURS]->d_name);
				OFFSET = 0;
				CURS = 0;
				reset(entry);
//				entry = pwd(&RAW, getcwd(buf, SIZ), flag);
				entry = pwd(&RAW, ".", flag);
				for(int i = 0; i < RAW.ar_len; i++) {
					if(!strcasecmp(current, RAW.ar[i]->d_name)) {
						CURS = i;
						break;
					}
				}
				if(RAW.ar_len < LINES-2)
					MENULEN = RAW.ar_len;
				else
					MENULEN = LINES-2;
				if(CURS > OFFSET + MENULEN-1)
					OFFSET = CURS - MENULEN+1;

				reset_p(entry_p);
//				entry_p = pwd(&PREV, dirname(buf), flag);
				entry_p = pwd(&PREV, "..", flag);
				cadr();
				cadr_p();
				free(current);
				break;
			case 's':
				endwin();
//				system("$SHELL");
				pid_t pid = fork();
				if(!pid) {
					clear(); //удалить? тогда system();//Перехватить Ctrl+D ...?
					execl("/bin/bash", "bash", NULL);
					exit(EXIT_SUCCESS);
				}
				else
					wait(NULL);
				
				current = strdup(RAW.ar[CURS]->d_name);
				OFFSET = 0;
				CURS = 0;
				reset(entry);
				entry = pwd(&RAW, ".", flag);
				for(int i = 0; i < RAW.ar_len; i++) {
					if(!strcasecmp(current, RAW.ar[i]->d_name)) {
						CURS = i;
						break;
					}
				}
				if(RAW.ar_len < LINES-2)
					MENULEN = RAW.ar_len;
				else
					MENULEN = LINES-2;
				if(CURS > OFFSET + MENULEN-1)
					OFFSET = CURS - MENULEN+1;

				cadr();
				refresh();
				free(current);
				break;
			case 'q':
				delwin(Prev);
				delwin(Raw);
				delwin(Next);
				endwin();
				exit(EXIT_SUCCESS);
				break;
			default:
				break;
		}
	}
	return 0;
}

void cadr_p() {
	char buf[SIZ];
//	struct stat sb;
	size_t size;
	int i, j, offset = 0;
	char format_side[7];
	int C4 = COLS / 4 - 2;
	getcwd(buf, SIZ);
	wclear(Prev);
	if(buf[1] == '\0') { //проверка директории на корень "/"
		sprintf(format_side, "%%-%ds", C4);
		wattron(Prev, A_REVERSE | A_BOLD | COLOR_PAIR(5));
		mvwprintw(Prev, 0, 1, format_side, buf);
		wattroff(Prev, A_REVERSE | A_BOLD | COLOR_PAIR(5));
	}
	else {
		for(i = 0; i < PREV.ar_len; i++) {
			if(!strcasecmp(PREV.ar[i]->d_name, basename(buf))) {
				if(i > LINES - 3)
				offset = i - (LINES - 3);
				break;
			}
		} // если не найдено совпадений присвоить i первый индекс, фиг там!
		for(j = offset; j < PREV.ar_len; j++) {
			int one = 0;
			size = bytesInPos(PREV.ar[j]->d_name, C4, &one);
			memcpy(buf, PREV.ar[j]->d_name, size);
			buf[size] = '\0';
			sprintf(format_side, "%%-%ds", C4 + one);
			if(PREV.ar[j]->d_type == DT_DIR)
				wattron(Prev, A_BOLD | COLOR_PAIR(5));
//			if(PREV.ar[j]->d_type == DT_LNK)
//				wattron(Prev, COLOR_PAIR(2));
//			if(PREV.ar[j]->d_type == DT_REG) {
//				lstat(RAW.ar[j]->d_name, &sb);
//				if(sb.st_mode & S_IXOTH)
//					wattron(Prev, COLOR_PAIR(1));
//			}
			if(j == i) {
				wattron(Prev, A_REVERSE);
			}
			mvwprintw(Prev, j-offset, 1, format_side, buf);
//			wattroff(Prev, A_REVERSE | A_BOLD | COLOR_PAIR(5) | COLOR_PAIR(2));
			wattroff(Prev, A_REVERSE | A_BOLD | COLOR_PAIR(5));
			if(j-offset == LINES - 3)
				break;
		}
	}
	wrefresh(Prev);
}

void cadr() {
	ACCESS = 1;
//	struct stat sb;
	char *current;
	int add_format = 0;
	size_t temp;
	char buf[SIZ];
	int i, j;
	int C2 = COLS / 2 - 2;
	int C4 = COLS / 4 - 2;
	char format_raw[7], format_side[7];
	sprintf(format_side, "%%-%ds", C4); //-2

//	box(Prev, 0, 0);
//	box(Raw, 0, 0);
//	box(Next, 0, 0);

	static struct dirent **entry_n = NULL; //new

	wclear(Raw);
	if(RAW.ar_len > 0) { //главная колонка
		for(i = OFFSET, j = 0; j < MENULEN; i++, j++) {
			if(RAW.ar[i]->d_type == DT_DIR)
				wattron(Raw, A_BOLD | COLOR_PAIR(5));
			if(RAW.ar[i]->d_type == DT_LNK)
				wattron(Raw, COLOR_PAIR(2));
			if(RAW.ar[i]->d_type == DT_REG) {
//				lstat(RAW.ar[i+OFFSET]->d_name, &sb);
//				if(sb.st_mode & S_IXOTH)
				if(access(RAW.ar[i]->d_name, X_OK) == F_OK)
					wattron(Raw, COLOR_PAIR(1));
			}
			if(i == CURS)
				wattron(Raw, A_REVERSE);
			temp = bytesInPos(RAW.ar[i]->d_name, C2, &add_format);
			memcpy(buf, RAW.ar[i]->d_name, temp);
			buf[temp] = '\0';

			sprintf(format_raw, "%%-%ds", C2 + add_format);

			mvwprintw(Raw, j, 1, format_raw, buf);
			wattroff(Raw, A_REVERSE | A_BOLD | COLOR_PAIR(5) | COLOR_PAIR(2));
		}
		getcwd(buf, SIZ);
		wclear(Next);
		if(RAW.ar[CURS]->d_type == DT_DIR ) { //выводим на экран третий столбец
			if(!access(RAW.ar[CURS]->d_name, R_OK)) {
				if(buf[1] != '\0')
					strcat(buf, "/");
				strcat(buf, RAW.ar[CURS]->d_name);
				ACCESS = 0;
				if(entry_n)
					reset_n(entry_n);
				entry_n = pwd(&NEXT, RAW.ar[CURS]->d_name, flag);
				current = listHash(buf); 
				for(i = 0; i < NEXT.ar_len; i++) {
					add_format = 0;
					temp = bytesInPos(NEXT.ar[i]->d_name, C4, &add_format);
					sprintf(format_side, "%%-%ds", C4 + add_format);
					memcpy(buf, NEXT.ar[i]->d_name, temp);
					buf[temp] = '\0';
					if(NEXT.ar[i]->d_type == DT_DIR)
						wattron(Next, A_BOLD | COLOR_PAIR(5));
					if(current) {
					       	if(!strcasecmp(NEXT.ar[i]->d_name, current))
							wattron(Next, A_REVERSE);
					}
					else {
						if(i == 0)
							wattron(Next, A_REVERSE);
					}
					mvwprintw(Next, i, 1, format_side, buf);
					wattroff(Next, A_REVERSE | A_BOLD | COLOR_PAIR(5));					
				}
				if(!i) {
					memcpy(buf, "Empty", 6);
					if(C4 < 5 && C4 > 0)
						buf[C4] = '\0';
					else if(C4 <= 0)
						buf[1] = '\0';
					sprintf(format_side, "%%-%ds", C4);
					wattron(Next, COLOR_PAIR(3));
					mvwprintw(Next, 0, 1, format_side, buf);
					wattroff(Next, COLOR_PAIR(3));
				}
			}
			else {
				ACCESS = 1;
				memcpy(buf, "Not accessible", 15);
				if(C4 < 15 && C4 > 0)
					buf[C4] = '\0';
				else if(C4 <= 0)
					buf[1] = '\0';
				sprintf(format_side, "%%-%ds", C4);
				wattron(Next, COLOR_PAIR(3));
				mvwprintw(Next, 0, 1, format_side, buf);
				wattroff(Next, COLOR_PAIR(3));
			}
		}
		wrefresh(Next);
	}
	else {
		wclear(Next);
		memcpy(buf, "Empty", 6);
		if(C2 < 5 && C2 > 0)
			buf[C2] = '\0';
		else if(C2 <= 0)
			buf[1] = '\0';
		sprintf(format_raw, "%%-%ds", C2);
		wattron(Raw, COLOR_PAIR(3));
		mvwprintw(Raw, 0, 1, format_raw, buf);
		wattroff(Raw, COLOR_PAIR(3));
		wrefresh(Next);
	}
	wrefresh(Raw);
}

void start_ncurses(void) {
	initscr();
//	raw();
//	endwin(); //что это? не едут лыжы!
	cbreak();
	noecho();
	curs_set(FALSE);
	clear();
	start_color();
	init_pair(1,COLOR_GREEN,0);
	init_pair(2,COLOR_CYAN,0);
	init_pair(3,COLOR_RED,0);
	init_pair(4,COLOR_YELLOW,0);
	init_pair(5,COLOR_BLUE,0);
	int Y = LINES-2;
	int X = COLS/4;
	Prev = newwin(Y, X, 1, 0);
	Raw = newwin(Y, COLS/2, 1, X);
	Next = newwin(Y, X, 1, COLS/2+X);
	refresh();
//	wrefresh(Prev);
//	wrefresh(Raw);
//	wrefresh(Next);
}

void sig_handler(int signo) {
	if(signo == SIGWINCH) {
		clear();
		endwin();
		refresh();
//		resizeterm(LINES, COLS);
//		wresize(stdscr, LINES, COLS); //
		int X = COLS/4;
		int Y = LINES-2;
		wresize(Prev, Y, X);
		wresize(Raw, Y, COLS/2);
		wresize(Next, Y, X);
//		mvwin(Prev, 1, 0); //точка привязки не изменилась
		mvwin(Raw, 1, X);
		mvwin(Next, 1, COLS/2+X);
		OFFSET = 0;
		if(RAW.ar_len < LINES-2)
			MENULEN = RAW.ar_len;
		else
			MENULEN = LINES-2;
		if(CURS > OFFSET + MENULEN-1)
			OFFSET = CURS - MENULEN+1;
		refresh();
		cadr();
		cadr_p();
	}
	else if(signo == SIGINT) {
//END_PROG: //написать atexit
		delwin(Prev);
		delwin(Raw);
		delwin(Next);
		endwin();
		fprintf(stderr, "%s\n", "Good bye! See you!");
			exit(1);
	}
}
// убрать глобальные структуры и переписать в одну функцию
void reset(struct dirent ** entry) {
	for(int i = 0; i < RAW.len; i++) {
		free(entry[i]);
	}
	free(entry);
	free(RAW.ar);
}
void reset_p(struct dirent ** entry_p) {
	for(int i = 0; i < PREV.len; i++) {
		free(entry_p[i]);
	}
	free(entry_p);
	free(PREV.ar);
}
void reset_n(struct dirent ** entry_n) {
	for(int i = 0; i < NEXT.len; i++) {
		free(entry_n[i]);
	}
	free(entry_n);
	free(NEXT.ar);
}
