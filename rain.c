// консольный файловый пейджер rain.c wch.c hash.c ldir.c dir.h my.h
#include "my.h"
#define SIZ 256

int main() {
	struct dirent **entry;
	CURS = 0;
	OFFSET = 0;
	bool flag = true;
	char *tmp;
	char temp[SIZ] = {0};
	char buf[SIZ];
	int key = 0;
	setlocale(LC_ALL, "");
	signal(SIGWINCH, sig_handler);
	signal(SIGINT, sig_handler);
	start_ncurses();
	entry = pwd(&RAW, getcwd(buf, SIZ), flag);
	if(RAW.ar_len < LINES-2)
		MENULEN = RAW.ar_len;
	else
		MENULEN = LINES-2;
	cadr();
	initHash();
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
				if(!strcasecmp(getcwd(buf, SIZ), "/"))
					break;
				if(RAW.ar_len > 0)
					searchHash(buf, RAW.ar[CURS]->d_name); //запишем или перезапишем базу
				CURS = 0;
				OFFSET = 0;
				MENULEN = 0;
				memcpy(temp, basename(buf), SIZ);
				chdir(dirname(buf));
				reset(entry);
				entry = pwd(&RAW, buf, flag);

				for(int i = 0; i < RAW.ar_len; i++) {
					if(!strcasecmp(temp, RAW.ar[i]->d_name)) {
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
				break;
			case 'l':
				if(RAW.ar_len > 0) {
					if(ACCESS)
						break;
					getcwd(buf, SIZ);
					//				searchHash(buf, RAW.ar[CURS]->d_name); //запишем или перезапишем в базу
					if(buf[1] != '\0')
						strcat(buf, "/");
					strcat(buf, RAW.ar[CURS]->d_name);
					chdir(buf);

					CURS = 0;
					OFFSET = 0;
					MENULEN = 0;
					reset(entry);
					entry = pwd(&RAW, buf, flag);
					if((tmp = searchHash(buf, "")) != NULL) {
						for(int i = 0; i < RAW.ar_len; i++) {
							if(!strcasecmp(tmp, RAW.ar[i]->d_name)) {
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
				}
				break;
			case 'a':
				flag = (flag)? false: true;	
				if(RAW.ar_len > 0) {
					tmp = RAW.ar[CURS]->d_name;
					OFFSET = 0;
					CURS = 0;
					reset(entry);
					entry = pwd(&RAW, getcwd(buf, SIZ), flag);
					for(int i = 0; i < RAW.ar_len; i++) {
						if(!strcasecmp(tmp, RAW.ar[i]->d_name)) {
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
				}
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

void cadr() {
	ACCESS = 1;
	struct stat sb;

	int add_format;
	size_t temp;

	int i;
	int C2 = COLS / 2 - 2;
	int C4 = COLS / 4 - 2;
	char format_raw[7], format_side[7] = {0};
	sprintf(format_side, "%%-%ds", C4-2);
//	sprintf(format_raw, "%%-%ds", COLS/2-2);

//	wclear(Prev);
//	wclear(Raw);
//	wclear(Next);
//	clear();

//	box(Prev, 0, 0);
//	box(Raw, 0, 0);
//	box(Next, 0, 0);

	struct dirent *entry_prev;
	DIR *dir;
	char buf[SIZ] = {0};
	char currentdir[SIZ] = {0};
	getcwd(currentdir, SIZ);
//	char *prev_dir;
	char str_line[SIZ] = {0};
	if(!strcasecmp(currentdir, "/")) {
		memcpy(str_line, "/", 2);
		mvwprintw(Prev, 0, 1, format_side, str_line);
	} else {
		getcwd(buf, SIZ);
		dir = opendir(dirname(buf));
		for(i = 0;(entry_prev = readdir(dir)) != NULL; i++) {
			if(!strcasecmp(entry_prev->d_name, "..") || !strcasecmp(entry_prev->d_name, ".")) {
				i--;
				continue;
			}
			else if(strchr(".", entry_prev->d_name[0])) { //прячем скрытые файлы
				i--;
				continue;
			}
			memset(str_line, 0, SIZ);
			int one;
			memcpy(str_line, entry_prev->d_name, bytesInPos(entry_prev->d_name, C4, &one));
			mvwprintw(Prev, i, 1, format_side, str_line);
		}
		closedir(dir);
	}

	if(RAW.ar_len > 0) {
		for(i = 0; i < MENULEN; i++) {
			if(RAW.ar[i+OFFSET]->d_type == DT_DIR)
				wattron(Raw, A_BOLD | COLOR_PAIR(5));
			if(RAW.ar[i+OFFSET]->d_type == DT_LNK)
				wattron(Raw, COLOR_PAIR(2));
			if(RAW.ar[i+OFFSET]->d_type == DT_REG) {
				lstat(RAW.ar[i+OFFSET]->d_name, &sb);
				if(sb.st_mode & S_IXOTH)
					wattron(Raw, COLOR_PAIR(1));
			}
			if(i+OFFSET == CURS)
				wattron(Raw, A_REVERSE);
			temp = bytesInPos(RAW.ar[i+OFFSET]->d_name, C2, &add_format);
			memset(str_line, 0, SIZ);
			memcpy(str_line, RAW.ar[i+OFFSET]->d_name, temp);

			memset(format_raw, 0, 7); //4
			sprintf(format_raw, "%%-%ds", COLS/2-2 + add_format);

			mvwprintw(Raw, i, 1, format_raw, str_line);
			wattroff(Raw, A_REVERSE | A_BOLD | COLOR_PAIR(5) | COLOR_PAIR(2));
		}
		if(RAW.ar[CURS]->d_type == DT_DIR ) { //выводим на экран третий столбец
			dir = opendir(RAW.ar[CURS]->d_name);
			if(dir) {
				ACCESS = 0;
				for(i = 0; (entry_prev = readdir(dir)) != NULL; i++) {
					if(!strcasecmp(entry_prev->d_name, "..") || !strcasecmp(entry_prev->d_name, ".")) {
						i--;
						continue;
					}
					else if(strchr(".", entry_prev->d_name[0])) { //прячем скрытые файлы
						i--;
						continue;
					}
					memset(str_line, 0, SIZ);
					int two;
					memcpy(str_line, entry_prev->d_name, bytesInPos(entry_prev->d_name, C4, &two));
					mvwprintw(Next, i, 1, format_side, str_line);
				}
				closedir(dir);
				if(!i) {
					memset(str_line, 0, SIZ);
					int three;
					memcpy(str_line, "Empty", bytesInPos("Empty", C4, &three));
					wattron(Next, COLOR_PAIR(3));
					mvwprintw(Next, 0, 1, format_side, str_line);
					wattroff(Next, COLOR_PAIR(3));
				}
			}
			else {
				ACCESS = 1;
				memset(str_line, 0, SIZ);
				int four;
				memcpy(str_line, "Not accessible", bytesInPos("Not accessible", C4, &four));
				wattron(Next, COLOR_PAIR(3));
				mvwprintw(Next, 0, 1, format_side, str_line);
				wattroff(Next, COLOR_PAIR(3));
			}
		}
	}
	else {
		memcpy(str_line, "Empty", COLS/2-2);
		memset(format_raw, 0, 7); //4
//		temp = bytesInPos(str_line, C2, &add_format);
		sprintf(format_raw, "%%-%ds", COLS/2-2); //+add_format
		wattron(Raw, COLOR_PAIR(3));
		mvwprintw(Raw, 0, 1, format_raw, str_line);
		wattroff(Raw, COLOR_PAIR(3));
	}
	clear();
	refresh();
	wrefresh(Prev);
	wrefresh(Raw);
	wrefresh(Next);
}

void start_ncurses(void) {
	initscr();
	cbreak();
	start_color();
	init_pair(1,COLOR_GREEN,0);
	init_pair(2,COLOR_CYAN,0);
	init_pair(3,COLOR_RED,0);
	init_pair(4,COLOR_YELLOW,0);
	init_pair(5,COLOR_BLUE,0);
	noecho();
	curs_set(FALSE);
	int Y = LINES-2;
	int X = COLS/4;
	Prev = newwin(Y, X, 1, 0);
	Raw = newwin(Y, COLS/2, 1, X);
	Next = newwin(Y, X, 1, COLS/2+X);
}

void sig_handler(int signo) {
	if(signo == SIGWINCH) {
		endwin();
		refresh();
		wclear(Prev);
		wclear(Raw);
		wclear(Next);
//		clear();
//		wresize(stdscr, COLS, LINES); //
		int X = COLS/4;
		int Y = LINES-2;
		wresize(Prev, Y, X);
		wresize(Raw, Y, COLS/2);
		wresize(Next, Y, X);
		mvwin(Raw, 1, X);
		mvwin(Next, 1, COLS/2+X);
		OFFSET = 0;
		if(RAW.ar_len < LINES-2)
			MENULEN = RAW.ar_len;
		else
			MENULEN = LINES-2;
		if(CURS > OFFSET + MENULEN-1)
			OFFSET = CURS - MENULEN+1;
		cadr();
	}
	else if(signo == SIGINT) {
//END_PROG:
		delwin(Prev);
		delwin(Raw);
		delwin(Next);
		endwin();
		fprintf(stderr, "%s\n", "Good bye! See you!");
			exit(1);
	}
}

void reset(struct dirent ** entry) {
	for(int i = 0; i < RAW.len; i++) {
		free(entry[i]);
	}
	free(entry);
	free(RAW.ar);
}

