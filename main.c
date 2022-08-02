#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <locale.h>

/* 화면 관련 변수 */
const int BASE = 0, MAIN = 1, MAIN_CONTENT = 2, MAIN_CONTENT_SEATS = 3;
int scr = 0;
int BLUE = 1;
int CYAN = 3;
int WHITE = 7;
int TEXT = 0;

int mute = 0;

/* 자리배치표 관련 변수 */
int w = 5, h = 6;
int iw = 5, ih = 6;
wchar_t P[7][7][6];
wchar_t names[1000][10];
int excluded[38] = { 0, };
int n = 0;

/* 친구이름 목록 관련 변수 */
int exitname = 0;
int delname = 0;

/* 비프음 */
BEEP(fq, dr)
	unsigned long fq;
	unsigned long dr;
{
	if(!mute) return Beep(fq, dr);
	return 0;
}

/* 커서이동 */
gotoxy(x, y)
    int x;
    int y;
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

/* 색변경 */
setcolor(text, back)
    unsigned short text;
    unsigned short back;
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | (back << 4));
}

/* 제목설정 */
title(t)
    wchar_t* t;
{
    int len = strlen(t);
    gotoxy(40 - len / 2 - 1, 0);
    setcolor(0, 7);
    wprintf(L" %s ", t);
}

/* 화면전환 */
screen(id)
    int id;
{
    int i, j, k;

    if(!id) {
        /* 테두리 */
        gotoxy(0, 0);
        setcolor(7, 0);
        wprintf(L"┌──────────────────────────────────────┐");
        for(i=1; i<23; i++) {
            gotoxy(0, i);
            wprintf(L"│");
            gotoxy(78, i);
            wprintf(L"│");
        }
        wprintf(L"└──────────────────────────────────────┘");

        /* 상태표시줄 */
        setcolor(15, CYAN);
        gotoxy(0, 24);
        wprintf(L"                                                                               ");

        /* 화면 */
        setcolor(7, BLUE);
        for(i=1; i<23; i++) {
            gotoxy(2, i);
            wprintf(L"                                                                            ");
        }

        /* 윈도우 XP에서 한 줄 밀리는 버그 회피 */
        gotoxy(0, 0);

        return;
    }

    if(id == MAIN || id == MAIN_CONTENT || id == MAIN_CONTENT_SEATS) {
        if(id == MAIN) {
            screen(BASE);
            title(L"자리바꾸기");
        }

        /* 좌석표 */
        gotoxy(4, 2);
        setcolor(7, BLUE);
        int pl = 0;
        char ssn[10];
        for(i=0; i<h; i++) {
            for(j=0; j<w; j++) {
                int x = 4 + j * 12;
                int xp = 0, rxp = 0, yp = 0;
                if(w == 5) rxp = 3;
                if(w == 4) rxp = 8;
                if(w == 3) rxp = 18;

                xp = rxp * j;

                if(i && h == 5) yp = 0.75 * i;
                if(i && h == 4) yp = 2 * i;
                if(i && h == 3) yp = 4.5 * i;

                if(pl) {
					for(k=pl+3; k<2 + i * 3 + yp; k++) {
						gotoxy(2, k);
						wprintf(L"                                                                            ");
					}
                }

                int snum = i * w + j + 1;

                gotoxy(x + xp, 2 + i * 3 + yp);
                if(excluded[snum]) {
					wprintf(L"             ");
					if(j != w - 1) for(k=0; k<rxp; k++) putchar(32);
					gotoxy(x + xp, 3 + i * 3 + yp);
					wprintf(L"            ");
					if(j != w - 1) for(k=0; k<rxp; k++) putchar(32);
					gotoxy(x + xp, 4 + i * 3 + yp);
					wprintf(L"            ");
					if(j != w - 1) for(k=0; k<rxp; k++) putchar(32);
                } else {
					wprintf(L"┌─ %2i ─┐", snum);
					if(j != w - 1) for(k=0; k<rxp; k++) putchar(32);
					gotoxy(x + xp, 3 + i * 3 + yp);
					wprintf(L"│");
                    wcstombs(ssn, P[i][j], 10);
					int pln = strlen(ssn);
					if(!pln);
					else if(pln <= 2) wprintf(L"   ");
					else if(pln <= 4) wprintf(L"  ");
					else if(pln <= 6) wprintf(L" ");
					wprintf(L"%s", P[i][j]);
					for(k=0; k<8-pln; k++) putchar(32);
					gotoxy(x + xp + 10, 3 + i * 3 + yp);
					wprintf(L"│");
					if(j != w - 1) for(k=0; k<rxp; k++) putchar(32);
					gotoxy(x + xp, 4 + i * 3 + yp);
					wprintf(L"└────┘");
					if(j != w - 1) for(k=0; k<rxp; k++) putchar(32);
                }

                pl = 2 + i * 3 + yp;
            }
        }

		if(id != MAIN_CONTENT_SEATS) {
			gotoxy(2, 20);
			wprintf(L"                                                                          ");
			gotoxy(2, 21);
			wprintf(L"                                                                          ");
			gotoxy(2, 22);
			wprintf(L"                                                                          ");

			/* 상태표시줄 */
			/* 윈도우 XP에서 한 줄 밀리는 버그 회피 */
			if(id == MAIN) {
				setcolor(15, CYAN);
				gotoxy(0, 24);
				wprintf(L" F2/열기  F3/저장  F7/줄  F9/칸  F4/이름  F5/시작  F1/제외  F6/나들이  ,/설정  ");
				gotoxy(0, 0);
			}
			setcolor(7, 0);
		}
    }

    scr = id;
    return;
}

/* 입력받는 함수 */
input(s, l, x, y, flag, def)
    wchar_t* s;
    int l;
    int x;
    int y;
    int flag;
    const wchar_t* def;
{
    wchar_t c;
    int cnt = 0;
    s[0] = 0;

    /* 미리 입력된 문자열 */
    char sdef[300] = "";
    int lc = 0;
    if(def[0]) {
		wcstombs(sdef, def, sizeof sdef);
        wcscpy(s, def);
        cnt = wcslen(def);
        wprintf(L"%s", def);
		lc = def[cnt - 1];
    }

    int scnt = strlen(sdef);
    int i;
    while(1) {
        c = _getwch();

        /* 리턴키 */
        if(c == 10 || c == 13) break;

        /* ESC키 */
        if(c == 27 && flag == 0 && !cnt) {
			n = 0;
			for(i=0; i<1000; i++) names[i][0] = 0;
			s[0] = 0;
			exitname = 1;
			return 0;
        }
        else if(c == 27 && flag == 1) {
			s[0] = 1;
			s[1] = 0;
			return 0;
        }
        else if(c == 27 && flag == 2) {
			s[0] = 0;
			return 0;
        }

        /* ESC키 2 */
        if(c == 27 && flag == 2) {
			return 0;
        }

        /* 특수키 */
        if(c == 224) {
            do c = _getwch();
            while(c == 224);

            /* Del키 */
            if(c == 83 && flag == 0 && !cnt) {
				if(!n) {
					BEEP(950, 100);
					continue;
				}
                delname = 1;
                s[0] = 0;
                n--;
                names[n][0] = 0;
                return 0;
            }
            continue;
        }

        /* 지우기 */
        if(c == 8) {
			if(!scnt) continue;
			if(lc >= 128) scnt -= 2;
			else scnt--;
			s[cnt] = 0;
			cnt--;
			gotoxy(x + scnt, y);
			if(lc >= 128) printf("  ");
			else printf(" ");
			gotoxy(x + scnt, y);
			lc = s[cnt-1];
			continue;
        }

        if(scnt >= (c >= 128 ? l - 1 : l)) {
            BEEP(950, 100);
            continue;
        }
        if(flag == 0) {
            if(!(c > 123 || (c >= 48 && c <= 57) || c == 32 || (c >= 65 && c <= 'Z') || (c >= 97 && c <= 'z'))) {
                BEEP(950, 100);
                continue;
            }
        } else if(flag == 2) {
            if(!(c > 123 || (c >= 65 && c <= 'Z') || c == 32 || (c >= 97 && c <= 'z') || (c >= 48 && c <= 57))) {
                BEEP(950, 100);
                continue;
            }
        } else if(flag == 1) {
            if(!(c == 32 || (c >= 48 && c <= 57))) {
                BEEP(950, 100);
                continue;
            }
        }
        s[cnt] = c;
        _putwch(c);
        cnt++;
        if(c >= 128) scnt += 2;
        else scnt++;
        lc = c;
    }
    s[cnt] = 0;

    return cnt;
}

main(argc, argv)
	int argc;
	char* argv[];
{
	/* 인코딩 & 시드 설정 */
	setlocale(0, "");
	srand(time(0));

	/* 윈도우 10/11에서 화면 깨지는 현상 패치 */
	HKEY hKey;
	DWORD regbuf;
	LONG hr;
	unsigned long regtyp = REG_DWORD, regszz = 1024;
    hr = RegOpenKeyEx(HKEY_CURRENT_USER, "Console", 0, KEY_ALL_ACCESS, &hKey);
    if(hr == ERROR_SUCCESS) {
		hr = RegQueryValueEx(hKey, "ForceV2", 0, &regtyp, (LPBYTE)&regbuf, &regszz);
		if(hr == ERROR_SUCCESS && regbuf == (DWORD)1) {
			DWORD regset = 0x0;
            hr = RegSetValueEx(hKey, "ForceV2", 0, REG_DWORD, (const BYTE*) &regset, sizeof(regset));
			RegCloseKey(hKey);
            if(hr == ERROR_SUCCESS) {
            	char cmd[1024] = "start ";
				strcat(cmd, argv[0]);
				system(cmd);
                return 0;
            } else {
				printf(L"제목 표시줄 마우스 우클릭 후, 속성에 들어가서 밑에 있는 '레거시 콘솔 사용'에 체크하고 다시 실행해 주세요.\n");
				getch();
				return 0;
            }
		} else {
			RegCloseKey(hKey);
		}
    }

    /* 환경설정 */
    FILE *config;
    char ckey[300];
    int cval;
    if(config = fopen("SETSEAT.INI", "r")) {
		fscanf(config, "[Setseat]\n");
		while(!feof(config)) {
			fscanf(config, "%[^=]=%i\n", ckey, &cval);
			if(!strcasecmp(ckey, "BGCOLOR")) {
                if(cval >= 0 && cval <= 15)
					BLUE = cval;
			}
			else if(!strcasecmp(ckey, "STATUSCOLOR")) {
                if(cval >= 0 && cval <= 15)
					CYAN = cval;
			}
			else if(!strcasecmp(ckey, "DIALOGCOLOR")) {
                if(cval >= 0 && cval <= 15)
					WHITE = cval;
			}
			else if(!strcasecmp(ckey, "DIALOGTEXT")) {
                if(cval >= 0 && cval <= 15)
					TEXT = cval;
			}
			else if(!strcasecmp(ckey, "ROWS")) {
                if(cval >= 3 && cval <= 6)
					h = ih = cval;
			}
			else if(!strcasecmp(ckey, "COLS")) {
                if(cval >= 3 && cval <= 6)
					w = iw = cval;
			}
			else if(!strcasecmp(ckey, "BEEP")) {
                mute = !cval;
			}
		}
        fclose(config);
    }

    /* 도스창 초기세팅 */
    system("mode con cols=80 lines=25");
    system("cls");
    screen(MAIN);

    /* 자판 누르기 대기 */
    int key = 0;
    int i, j, k;
    do {
        int k = getch();
        if(key == 1000) key += k;
        else key = k;
        if(!key) key = 1000;
        else {
            /* 열기 */
            if(key == 1060) {
				void filedialog() {
					screen(MAIN_CONTENT);

					gotoxy(12, 3);
					setcolor(TEXT, WHITE);
					wprintf(L"┌────────  자리 배치표 열기  ────────┐");

					gotoxy(12, 4);
					setcolor(TEXT, WHITE);
					wprintf(L"│                                                    │");
					setcolor(0, 0);
					wprintf(L"  ");

					gotoxy(12, 5);
					setcolor(TEXT, WHITE);
					wprintf(L"│  ┌ 선택 ───────────────────┐  │");
					setcolor(0, 0);
					wprintf(L"  ");

					for(i=6; i<=15; i++) {
						gotoxy(12, i);
						setcolor(TEXT, WHITE);
						wprintf(L"│  │                                            │  │");
						setcolor(0, 0);
						wprintf(L"  ");
					}

					gotoxy(12, 16);
					setcolor(TEXT, WHITE);
					wprintf(L"│  └──────────────────────┘  │");
					setcolor(0, 0);
					wprintf(L"  ");

					gotoxy(12, 17);
					setcolor(TEXT, WHITE);
					wprintf(L"│                                                    │");
					setcolor(0, 0);
					wprintf(L"  ");

					gotoxy(12, 18);
					setcolor(TEXT, WHITE);
					wprintf(L"│             < O/열기 >       < 취소 >              │");
					setcolor(0, 0);
					wprintf(L"  ");

					gotoxy(12, 19);
					setcolor(TEXT, WHITE);
					wprintf(L"└──────────────────────────┘");
					setcolor(0, 0);
					wprintf(L"  ");
					gotoxy(14, 20);
					wprintf(L"                                                        ");
				}

				void errfile() {
                    screen(MAIN_CONTENT);

					gotoxy(21, 7);
                    setcolor(TEXT, WHITE);
                    wprintf(L"┌────── 알립니다 ──────┐");

                    gotoxy(21, 8);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 9);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│  선택한 화일이 손상되었습니다.   │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 10);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 11);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│             < 확인 >             │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 12);
                    setcolor(TEXT, WHITE);
                    wprintf(L"└─────────────────┘");
                    setcolor(0, 0);
                    wprintf(L"  ");
                    gotoxy(23, 13);
                    wprintf(L"                                      ");

                    getch();
                    screen(MAIN_CONTENT);
				}

				filedialog();

				int cc, sel = 1, cnt = 0, sd = 1, ed = 10, rc = 0;
				char selfile[300] = "";
				char filename[300];
				wchar_t wfilename[300];
				while(1) {
					/* 파일 목록 */
					DIR *d;
					struct dirent *dir;
					d = opendir(".");
					i = 1, cnt = 0, rc = 0;

					if(d) {
						while(dir = readdir(d)) {
							if(i > ed) { cnt++; continue; }
							if(i < sd) { i++, cnt++; continue; }
							strcpy(filename, dir->d_name);
							int len = strlen(filename);
							if(len < 5) continue;
							if (
								filename[len-4] == '.' &&
								(filename[len-3] == 'f' || filename[len-3] == 'F') &&
								(filename[len-2] == 'u' || filename[len-2] == 'U') &&
								(filename[len-1] == 'n' || filename[len-1] == 'N')
							) {
								if(sel == i) setcolor(7, 0), strcpy(selfile, filename);
								else setcolor(TEXT, WHITE);
								int y = (i - 1) % 10;
								gotoxy(18, 6 + y);

								mbstowcs(wfilename, filename, 300);
								wprintf(L" %s", wfilename);
								for(k=1; k<=43 - len; k++) putchar(32);
								cnt++;
								rc++;
								i++;
							}
						}
						closedir(d);
						setcolor(TEXT, WHITE);
						for(j=rc+1; j<=10; j++) {
							gotoxy(18, 5 + j);
							wprintf(L"%44s", L"");
						}
						if(!rc) gotoxy(31, 10), wprintf(L"(화일이 없습니다)"), sel = 0;
					}

					cc = getch();

					/* 선택 */
					if(cc == 224) {
						do cc = getch();
						while(cc == 224);
						if(cc == 72 && sel > 1) {
							sel--;
							if(sel < sd) sd -= 10, ed -= 10;
						}
						if(cc == 80 && sel < cnt && sel) {
							sel++;
							if(sel > ed) sd += 10, ed += 10;
						}
					}

					/* 열기 */
					else if((cc == 10 || cc == 13 || cc == 'o' || cc == 'O') && selfile[0]) {
						FILE *tf;
						tf = fopen(selfile, "r");

						int _w, _h, sc;
						fscanf(tf, "%i", &_w);
						if(feof(tf)) { errfile(); break; }
						fscanf(tf, "%i", &_h);
						if(feof(tf)) { errfile(); break; }
						fscanf(tf, "%i", &sc);
						if(feof(tf)) { errfile(); break; }
						char _name[10];
						wchar_t wname[10];
						int chk = 0;
						n = 0;
                        for(i=0; i<1000; i++) names[i][0] = 0;
						for(i=0; i<sc; i++) {
							fscanf(tf, "%s", _name);
							if(feof(tf)) { chk = 1; break; }
							mbstowcs(wname, _name, 10);
							wcscpy(names[i], wname);
						}
						if(chk) { errfile(); break; }
						n = sc;
						chk = 0;
						for(i=0; i<_h; i++)
							for(j=0; j<_w; j++) {
								char sname[10];
								wchar_t wsname[10];
								fscanf(tf, "%s", sname);
								mbstowcs(wsname, sname, 10);
								if(sname[0] != '.')
									wcscpy(P[i][j], wsname);
								else
									P[i][j][0] = 0;
							}
						if(chk) { errfile(); break; }
						chk = 0;
						for(i=0; i<38; i++) {
							int ed;
							fscanf(tf, "%i", &ed);
							if(feof(tf)) { chk = 1; break; }
							excluded[i] = ed;
						}
						if(chk) { errfile(); break; }

						w = _w, h = _h;
						fclose(tf);
						screen(MAIN_CONTENT);
						break;
					}

					/* 취소 */
					else if(cc == 27) {
						screen(MAIN_CONTENT);
						break;
					}
				}
            }

            /* 저장 */
            else if(key == 1061) {
				/* 창 그리기 */
                gotoxy(21, 7);
                setcolor(TEXT, WHITE);
                wprintf(L"┌──── 자리 배치표 저장 ────┐");

                gotoxy(21, 8);
                setcolor(TEXT, WHITE);
                wprintf(L"│ ┌ 배치표 이름  ───────┐ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 9);
                setcolor(TEXT, WHITE);
                wprintf(L"│ │                            │ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 10);
                setcolor(TEXT, WHITE);
                wprintf(L"│ └──────────────┘ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 11);
                setcolor(TEXT, WHITE);
                wprintf(L"│                                  │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 12);
                setcolor(TEXT, WHITE);
                wprintf(L"│         < 확인 >  < 취소 >       │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 13);
                setcolor(TEXT, WHITE);
                wprintf(L"└─────────────────┘");
                setcolor(0, 0);
                wprintf(L"  ");
                gotoxy(23, 14);
                wprintf(L"                                      ");

                gotoxy(26, 9);
                setcolor(TEXT, WHITE);

                /* 입력 받기 */
                wchar_t wfn[30] = L"";
                char fn[30];
                input(wfn, 28, 26, 9, 2, L"");
				wcstombs(fn, wfn, 30);
                if(fn[0]) {
					FILE *tf;
					strcat(fn, ".FUN");
					tf = fopen(fn, "w");
					fprintf(tf, "%i %i %i ", w, h, n);
					for(i=0; i<n; i++) {
						char sns[10];
						wcstombs(sns, names[i], 10);
						fprintf(tf, "%s ", sns);
					}
					for(i=0; i<h; i++)
						for(j=0; j<w; j++) {
							if(P[i][j][0]) {
                                char spn[10];
                                wcstombs(spn, P[i][j], 10);
								fprintf(tf, "%s ", spn);
							} else {
								fprintf(tf, ". ");
							}
						}
					for(i=0; i<38; i++)
						fprintf(tf, "%i ", excluded[i]);
					fclose(tf);
                }

                screen(MAIN_CONTENT);
            }

            /* 친구 이름 목록 지정 */
            else if(key == 1062) {
                /* 창 그리기 */
                int namelist()
                {
                    k = 0;
					for(i=(n>10?(n-10):0); i<(n>10?n:10); i++, k++) {
						gotoxy(13, 7 + k);
						setcolor(TEXT, WHITE);
						wprintf(L"│  %2i. ", i + 1);
						if(names[i][0])
                            wprintf(L"%-8s", names[i]);
                        else
                            wprintf(L"%-16s", L"--------");
						gotoxy(34, 7 + k);
						wprintf(L"  ");
					}
					return 0;
                }

                gotoxy(13, 3);
                setcolor(TEXT, WHITE);
                wprintf(L"┌────────── 친구목록 ──────────┐");
                gotoxy(13, 4);
                setcolor(TEXT, WHITE);
                wprintf(L"│┌ 이름 ────┐                                │");
                setcolor(0, 0);
                wprintf(L"  ");
                gotoxy(13, 5);
                setcolor(TEXT, WHITE);
                wprintf(L"││              │                                │");
                setcolor(0, 0);
                wprintf(L"  ");
                gotoxy(13, 6);
                setcolor(TEXT, WHITE);
                wprintf(L"│└───────┘                                │");
                setcolor(0, 0);
                wprintf(L"  ");

                for(i=0; i<10; i++) {
                    gotoxy(13, 7 + i);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");
                }

                gotoxy(13, 17);
                setcolor(TEXT, WHITE);
                wprintf(L"│                                                  │");
                setcolor(0, 0);
                wprintf(L"  ");
                gotoxy(13, 18);
                setcolor(TEXT, WHITE);
                wprintf(L"│               < 완료 >  < 초기화 >               │");
                setcolor(0, 0);
                wprintf(L"  ");
                gotoxy(13, 19);
                setcolor(TEXT, WHITE);
                wprintf(L"└─────────────────────────┘");
                setcolor(0, 0);
                wprintf(L"  ");
                gotoxy(15, 20);
                wprintf(L"                                                      ");

                setcolor(TEXT, WHITE);
                gotoxy(36, 5);
                wprintf(L" * 목록을 초기화하고 닫으려면");
                gotoxy(36, 6);
                wprintf(L"   <Esc> 키를 누르십시오.");
                gotoxy(36, 8);
                wprintf(L" * 이름은 영숫자 8자, ");
                gotoxy(36, 9);
                wprintf(L"   한글 4자까지 입력됩니다.");
                gotoxy(36, 11);
                wprintf(L" * 다 적었으면 이름칸을 비운");
                gotoxy(36, 12);
                wprintf(L"   채로 리턴키를 누르십시오.");
                gotoxy(36, 14);
                wprintf(L" * 마지막으로 추가한 이름을");
                gotoxy(36, 15);
                wprintf(L"   지우려면 <Delete> 키를");
                gotoxy(36, 16);
                wprintf(L"   누르십시오.");

                /* 입력 받기 */
                setcolor(TEXT, WHITE);
                gotoxy(38, 4);
                int idx = 0;
                for(i=0; i<1000; i++) {
					if(names[i][0]) {
						idx = i + 1;
					} else break;
                }
                exitname = 0;
                delname = 0;
                while(1) {
                    delname = 0;
					namelist();
                    gotoxy(17, 5);
                    wprintf(L"        ");
                    gotoxy(17, 5);
                    input(names[idx], 8, 17, 5, 0, L"");
                    if(exitname) break;
                    else if(delname) { names[idx][0] = 0, idx--; continue; }
                    if(!names[idx][0]) break;
                    n = idx + 1;
                    idx++;
                }
                exitname = 0;
                delname = 0;
                screen(MAIN_CONTENT);
            }

            /* 시작 */
            else if(key == 1063) {
                if(!n) {
                    /* 창 그리기 */
                    gotoxy(21, 7);
                    setcolor(TEXT, WHITE);
                    wprintf(L"┌────── 알립니다 ──────┐");

                    gotoxy(21, 8);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 9);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│  친구이름 목록이 없습니다. F4를  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 10);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│ 눌러서 목록을 만드십시오.        │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 11);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 12);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│             < 확인 >             │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 13);
                    setcolor(TEXT, WHITE);
                    wprintf(L"└─────────────────┘");
                    setcolor(0, 0);
                    wprintf(L"  ");
                    gotoxy(23, 14);
                    wprintf(L"                                      ");

                    getch();
                    screen(MAIN_CONTENT);

                    continue;
                }

                int chk = 0;
                for(i=0; i<h; i++)
					for(j=0; j<w; j++)
						if(P[i][j][0]) chk = 1;
				if(chk) {
					/* 창 그리기 */
                    gotoxy(21, 7);
                    setcolor(TEXT, WHITE);
                    wprintf(L"┌─────── 경고 ───────┐");

                    gotoxy(21, 8);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 9);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│  현재의 자리배치표를 지우고 새로 │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 10);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│ 자리를 배치하시겠습니까?         │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 11);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 12);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│       < Y/예 >  < N/아니요 >     │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 13);
                    setcolor(TEXT, WHITE);
                    wprintf(L"└─────────────────┘");
                    setcolor(0, 0);
                    wprintf(L"  ");
                    gotoxy(23, 14);
                    wprintf(L"                                      ");

                    int kk = getch();
                    if(kk != 'Y' && kk != 'y' && kk != 13 && kk != 10) {
						screen(MAIN_CONTENT);
						continue;
                    }
				}

				int scnt = 0;
				for(i=0; i<h; i++)
					for(j=0; j<w; j++)
						if(!excluded[i * w + j + 1]) scnt++;
				if(scnt < n) {
					/* 창 그리기 */
                    gotoxy(21, 7);
                    setcolor(TEXT, WHITE);
                    wprintf(L"┌────── 알립니다 ──────┐");

                    gotoxy(21, 8);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 9);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│  현재 친구들의 수가 앉을 좌석의  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 10);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│ 수보다 많습니다.                 │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 11);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│                                  │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 12);
                    setcolor(TEXT, WHITE);
                    wprintf(L"│             < 확인 >             │");
                    setcolor(0, 0);
                    wprintf(L"  ");

                    gotoxy(21, 13);
                    setcolor(TEXT, WHITE);
                    wprintf(L"└─────────────────┘");
                    setcolor(0, 0);
                    wprintf(L"  ");
                    gotoxy(23, 14);
                    wprintf(L"                                      ");

                    getch();
                    screen(MAIN_CONTENT);

					continue;
				}

                setcolor(7, BLUE);
                for(i=0; i<h; i++)
					for(j=0; j<w; j++)
						P[i][j][0] = 0;

                for(i=0; i<n; i++) {
                    wchar_t prev[10] = L"", cur[10] = L"", next[10] = L"";
                    if(i) wcscpy(prev, names[i-1]);
                    wcscpy(cur, names[i]);
                    if(i != n-1) wcscpy(next, names[i+1]);
                    if(!next[0]) wcscpy(next, L"끝!");
                    gotoxy(4, 20);
					wprintf(L"────────────────────────────────────");
                    gotoxy(4, 21);
					wprintf(L" [                  ] ==> [                  ] ==> [                  ]");
                    gotoxy(11, 21);
                    wprintf(L"%s", prev);
                    gotoxy(36, 21);
                    wprintf(L"%s", cur);
                    gotoxy(61, 21);
                    wprintf(L"%s", next);
                    while(1) {
						int sw = rand() % w, sh = rand() % h;
						if(P[sh][sw][0]) continue;
						if(excluded[sh * w + sw + 1]) continue;
						wcscpy(P[sh][sw], cur);
						break;
                    }
                    screen(MAIN_CONTENT_SEATS);
                    BEEP(850, 200);
                    Sleep(750);
                }
                BEEP(850, 800);
            }

            /* 제외 좌석 */
            else if(key == 1059) {
                /* 창 그리기 */
                gotoxy(21, 7);
                setcolor(TEXT, WHITE);
                wprintf(L"┌──── 배치 제외할 좌석 ────┐");

                gotoxy(21, 8);
                setcolor(TEXT, WHITE);
                wprintf(L"│ ┌ 좌석번호 (공백 구분) ───┐ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 9);
                setcolor(TEXT, WHITE);
                wprintf(L"│ │                            │ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 10);
                setcolor(TEXT, WHITE);
                wprintf(L"│ └──────────────┘ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 11);
                setcolor(TEXT, WHITE);
                wprintf(L"│                                  │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 12);
                setcolor(TEXT, WHITE);
                wprintf(L"│             < 확인 >             │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(21, 13);
                setcolor(TEXT, WHITE);
                wprintf(L"└─────────────────┘");
                setcolor(0, 0);
                wprintf(L"  ");
                gotoxy(23, 14);
                wprintf(L"                                      ");

                gotoxy(26, 9);
                setcolor(TEXT, WHITE);

                /* 입력 받기 */
                wchar_t ss[30] = L"";
                wchar_t def[30] = L"";
                for(i=1; i<38; i++) {
                    if(excluded[i]) {
						wchar_t sn[30] = L"";
                        swprintf(sn, L"%i ", i);
                        wcscat(def, sn);
                    }
                }
                def[wcslen(def)-1] = 0;
                input(ss, 28, 26, 9, 1, def);
                if(ss[0] == 1 && !ss[1]) {
					screen(MAIN_CONTENT);
					continue;
                }

                /* 변수에 넣기 */
                for(i=0; i<38; i++) excluded[i] = 0;
                int len = wcslen(ss);
                if(!len) {
					for(i=0; i<38; i++) excluded[i] = 0;
                } else {
					wchar_t nn[30] = L"";
					for(i=0, j=0; i<=len; i++) {
						if(ss[i] >= 48 && ss[i] <= 57 && i < len) {
							nn[j++] = ss[i];
						} else if(nn[0]) {
							nn[j] = 0;
							if(wcslen(nn) > 2) continue;
							int v;
							swscanf(nn, L"%i", &v);
							if(v <= 36) excluded[v] = 1;
							j = 0, nn[0] = 0;
						}
					}
                }

                screen(MAIN_CONTENT);
            }

            /* 줄줄임 */
            else if(key == 1065) {
                if(h == 3) continue;
                h--;
                screen(MAIN_CONTENT);
            }

            /* 줄늘림 */
            else if(key == 1066) {
                if(h == 6) continue;
                h++;
                screen(MAIN_CONTENT);
            }

            /* 칸줄임 */
            else if(key == 1067) {
                if(w == 3) continue;
                w--;
                screen(MAIN_CONTENT);
            }

            /* 칸늘림 */
            else if(key == 1068) {
                if(w == 6) continue;
                w++;
                screen(MAIN_CONTENT);
            }

            /* 나들이 */
            else if(key == 1064) {
				setcolor(7, 0);
				system("cls");
				_putws(L"돌아가려면 EXIT를 입력하십시오.\n");
                system("cmd /k");
                screen(MAIN);
            }

            /* 설정 */
            else if(key == 44) {
                wchar_t* color(id)
					int id;
				{
					if(id == 0) return L"검정";
					if(id == 1) return L"남색";
					if(id == 2) return L"초록";
					if(id == 3) return L"청록";
					if(id == 4) return L"벽돌";
					if(id == 5) return L"자주";
					if(id == 6) return L"황록";
					if(id == 7) return L"회색";
					if(id == 8) return L"재색";
					if(id == 9) return L"파랑";
					if(id == 10) return L"연두";
					if(id == 11) return L"바다";
					if(id == 12) return L"빨강";
					if(id == 13) return L"분홍";
					if(id == 14) return L"노랑";
					if(id == 15) return L"흰색";
					return L"ㅡㅡ";
				}

				/* 창 그리기 */
                gotoxy(15, 7);
                setcolor(TEXT, WHITE);
                wprintf(L"┌───────── 환경설정 ─────────┐");

                gotoxy(15, 8);
                setcolor(TEXT, WHITE);
                wprintf(L"│ ┌ 화면색상 ───────────────┐ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 9);
                setcolor(TEXT, WHITE);
                wprintf(L"│ │ B/배경   ◀ %s ▶ S/상태줄 ◀ %s ▶│ │", color(BLUE), color(CYAN));
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 10);
                setcolor(TEXT, WHITE);
                wprintf(L"│ │ W/창배경 ◀ %s ▶ T/창글자 ◀ %s ▶│ │", color(WHITE), color(TEXT));
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 11);
                setcolor(TEXT, WHITE);
                wprintf(L"│ └────────────────────┘ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 12);
                setcolor(TEXT, WHITE);
                wprintf(L"│ ┌ 초기 좌석 상태 ────────────┐ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 13);
                setcolor(TEXT, WHITE);
                wprintf(L"│ │ R/줄 수 ◀ %i ▶      C/칸 수 ◀ %i ▶   │ │", ih, iw);
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 14);
                setcolor(TEXT, WHITE);
                wprintf(L"│ └────────────────────┘ │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 15);
                setcolor(TEXT, WHITE);
                wprintf(L"│  [%c] E/신호음                                │", mute ? ' ' : 'X');
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 16);
                setcolor(TEXT, WHITE);
                wprintf(L"│                                              │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 17);
                setcolor(TEXT, WHITE);
                wprintf(L"│              < 저장 >  < 취소 >              │");
                setcolor(0, 0);
                wprintf(L"  ");

                gotoxy(15, 18);
                setcolor(TEXT, WHITE);
                wprintf(L"└───────────────────────┘");
                setcolor(0, 0);
                wprintf(L"  ");
                gotoxy(17, 19);
                wprintf(L"                                                  ");
                setcolor(TEXT, WHITE);

                int kk;
                int mode = 'B';
                int set[92] = { 0, };
                set['B'] = BLUE;
                set['S'] = CYAN;
                set['W'] = WHITE;
                set['T'] = TEXT;
                set['R'] = ih;
                set['C'] = iw;
                set['E'] = !mute;
                gotoxy(21, 9);

                do {
					kk = getch();
					if(kk >= 97 && kk <= 'z') kk -= 32;
					if(kk == 10 || kk == 13) {
						BLUE = set['B'];
						CYAN = set['S'];
						WHITE = set['W'];
						TEXT = set['T'];
						mute = !set['E'];
						ih = set['R'];
						iw = set['C'];
						FILE *config2;
						config2 = fopen("SETSEAT.INI", "w");
						fprintf(config2, "[Setseat]\nBGCOLOR=%i\nSTATUSCOLOR=%i\nDIALOGCOLOR=%i\nDIALOGTEXT=%i\nCOLS=%i\nROWS=%i\nBEEP=%i\n",
											BLUE, CYAN, WHITE, TEXT, iw, ih, !mute);
						fclose(config2);
						screen(MAIN);
						break;
					}
					if(kk == 'B' || kk == 'S' || kk == 'W' || kk == 'R' || kk == 'C' || kk == 'E' || kk == 'T')
						mode = kk;
					if(kk == 224) {
						do kk = getch();
						while(kk == 224);

						if(mode == 'R' || mode == 'C') {
							if(kk == 75 || kk == 72) {
								if(set[mode] > 3) set[mode]--;
								else BEEP(950, 100);
							}
							if(kk == 77 || kk == 80) {
								if(set[mode] < 6) set[mode]++;
								else BEEP(950, 100);
							}
							if(mode == 'R') gotoxy(32, 13);
							if(mode == 'C') gotoxy(53, 13);
							printf("%i", set[mode]);
						} else if(mode != 'E') {
							if(kk == 75 || kk == 72) {
								if(mode == 'B' && set[mode] == 12) set[mode] = 9;
								else if(mode == 'B' && set[mode] == 8) set[mode] = 6;
								else if(mode == 'W' && set['W'] - set['T'] == 1) {
									if(set[mode] == 1) BEEP(950, 100);
									else set[mode] -= 2; }
								else if(mode == 'T' && set['T'] - set['W'] == 1) {
									if(set[mode] == 1) BEEP(950, 100);
									else set[mode] -= 2; }
								else if(set[mode] > 0) set[mode]--;
								else BEEP(950, 100);
							}
							if(kk == 77 || kk == 80) {
								if(mode == 'B' && set[mode] == 9) set[mode] = 12;
								else if(mode == 'B' && set[mode] == 6) set[mode] = 8;
								else if(mode == 'B' && set[mode] == 8) set[mode] = 9;
								else if(mode == 'W' && set['T'] - set['W'] == 1) {
									if(set[mode] == 14) BEEP(950, 100);
									else set[mode] += 2; }
								else if(mode == 'T' && set['W'] - set['T'] == 1) {
									if(set[mode] == 14) BEEP(950, 100);
									else set[mode] += 2; }
								else if(set[mode] < (mode == 'W' || mode == 'T' ? 15 : (mode == 'S' ? 8 : 6))) set[mode]++;
								else BEEP(950, 100);
							}
							if(mode == 'B') gotoxy(33, 9);
							if(mode == 'S') gotoxy(53, 9);
							if(mode == 'W') gotoxy(33, 10);
							if(mode == 'T') gotoxy(53, 10);
							wprintf(L"%s", color(set[mode]));
						}
					}
					if(kk == 'R') gotoxy(21, 13);
					if(kk == 'C') gotoxy(42, 13);
					if(kk == 'B') gotoxy(21, 9);
					if(kk == 'S') gotoxy(41, 9);
					if(kk == 'W') gotoxy(21, 10);
					if(kk == 'T') gotoxy(41, 10);
					if(kk == 'E' || (mode == 'E' && kk == 32)) {
						set['E'] = mute;
						gotoxy(20, 15);
						putchar(mute ? 'X' : ' ');
						mute = !mute;
						gotoxy(23, 15);
					}

					if(kk == 27) {
						screen(MAIN_CONTENT);
						break;
					}
                } while(1);
            }
        }
    } while(key != 27);

    /* 종료 */
	system("cls");
    setcolor(7, 0);
    return 0;
}
