/*
* PJATK Gdansk
* PRG1 2016/2017
* Temat: Projekt semestralny - Program rysujacy zadana figure wybranym kodem ASCII
* Student: 16125
/*

/* Bibliografia ;) 
* https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms682093(v=vs.85).aspx - CONSOLE_SCREEN_BUFFER_INFO
* https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms683171(v=vs.85).aspx - GetConsoleScreenBufferInfo
* http://www.cplusplus.com/forum/beginner/121194/#msg659728 - walidacja
* http://stackoverflow.com/questions/13212043/integer-input-validation-how - walidacja
* http://stackoverflow.com/questions/1904635/warning-c4003-and-errors-c2589-and-c2059-on-x-stdnumeric-limitsintmax
	- problem z konfliktem (numeric_limits<streamsize>::max)() VS max(), pomogl zwykly nawias
* http://www.cplusplus.com/reference/ios/ios_base/width/ - ustawienie cout od lewej
* https://social.msdn.microsoft.com/Forums/vstudio/en-US/a1759ad8-56d9-487d-84c9-3020300c87a3/cc-how-get-arrow-keyscorrectly-using-getch?forum=vcgeneral
	- problem z przekazaniem znakow specjalnych (strzalki) przez getch()
* http://www.cplusplus.com/forum/beginner/4234/#msg18563 - SetConsoleCursorPosition
*/

#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h> // bibl Windowsowa
#include <limits> // dla cin.ignore(numeric_limits<streamsize>::max(), '\n');

using namespace std;

/*************************************************************************/
/* DEKLARACJE STA£YCH GLOBALNYCH */

const bool DEBUG_MODE = 0; // Gdy 1, wyswietla info o konsoli
const short MIN_SZEROKOSC_KONSOLI = 80;
const short MIN_WYSOKOSC_KONSOLI = 10;
const short MIN_ROZMIAR_FIGURY = 3;
const short PUNKT_POCZATKOWY_X = 0;
const short PUNKT_POCZATKOWY_Y = 0;
const unsigned char ENTER = 13;
const unsigned char ESC = 27;

/*************************************************************************/
/* DEKLARACJE FUNKCJI */

void ToggleCursor(bool);
void wypiszNaglowek();
short wczytajKodZnaku();
short wczytajWysokoscKonsoli();
short wczytajSzerokoscKonsoli();
short getMaxRozmiarFigury();
void gotoxy(short, short);
short wczytajRozmiarPoczatkowy();
void wypiszASCII();
void rysujFigure(short, char, short, short);
unsigned char manipulujFigura(short &, short &, short &);
void debugMode();


/*************************************************************************/
/* START */

int main()
{
	char znak;
	short rozmiar;
	short pozycjaX;
	short pozycjaY;
	unsigned char koniec = ESC;

	do {
		ToggleCursor(1);

		// Jesli powrot z rysowania, to pozwol na ucieczke i zakonczenie programu.
		if (koniec == ENTER) {
			system("cls");
			cout << "Nacisnij ESC, aby wyjsc lub dowolny inny klawisz, aby kontynuowac zabawe...";
			if (_getch() == ESC) break;
			system("cls");
		}

		// Na potrzeby sprawdzania info o konsoli.
		if (DEBUG_MODE) {
			debugMode();
		}

		wypiszNaglowek();

		if ((wczytajWysokoscKonsoli() < MIN_WYSOKOSC_KONSOLI) || (wczytajSzerokoscKonsoli() < MIN_SZEROKOSC_KONSOLI)) {
			cout << "Za maly rozmiar konsoli ";
			cout << "(min.wys. = " << MIN_WYSOKOSC_KONSOLI << ", min. szer. = " << MIN_SZEROKOSC_KONSOLI << ").\n";
			cout << "Zmien wymiary swojej konsoli i uruchom program ponownie.\n";
			system("PAUSE");
			return EXIT_SUCCESS;
		}

		pozycjaX = PUNKT_POCZATKOWY_X;
		pozycjaY = PUNKT_POCZATKOWY_Y;

		wypiszASCII();

		znak = (char)wczytajKodZnaku();
		rozmiar = wczytajRozmiarPoczatkowy();

		ToggleCursor(0);

		do {
			rysujFigure(rozmiar, znak, pozycjaX, pozycjaY);
			koniec = manipulujFigura(pozycjaX, pozycjaY, rozmiar);
		} while ( koniec != ESC && koniec != ENTER);
		//wypiszASCII();
		//wybierzZnak();
		//echoConsoleInfoOnly();
	} while (koniec != ESC);
	return EXIT_SUCCESS;
}

/* KONIEC */

/*************************************************************************/
/* DEFINICJE FUNKCJI */

void wypiszNaglowek() {
	cout << "\
PJATK Gdansk \n\
PRG1 2016/2017 \n\
Temat: Projekt semestralny - Program rysujacy zadana figure wybranym kodem ASCII \n\
Student: 16125 \n\n";
}

void debugMode() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	cout << "DEBUG MODE ON\n";
	cout << "\tdwSize: " << csbi.dwSize.X << " , " << csbi.dwSize.Y << endl;
	cout << "\tsrWindowTL: " << csbi.srWindow.Left << " , " << csbi.srWindow.Top << endl;
	cout << "\tsrWindowBR: " << csbi.srWindow.Right << " , " << csbi.srWindow.Bottom << endl;
	cout << "\tdwMaximumWindowSize: " << csbi.dwMaximumWindowSize.X << " , " << csbi.dwMaximumWindowSize.Y << endl;
	cout << endl;
}

void ToggleCursor(bool trigger)
{
	::HANDLE hConsoleOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	::CONSOLE_CURSOR_INFO hCCI;
	::GetConsoleCursorInfo(hConsoleOut, &hCCI);
	hCCI.bVisible = trigger;
	::SetConsoleCursorInfo(hConsoleOut, &hCCI);
}

void wypiszASCII() {
	unsigned char ch;
	unsigned short i;
	cout << endl << "Czy wyswietlic tablice znakow ASCII?" << endl;
	cout << "Tak [1]\nNie [dowolny klawisz]\n";

	ch = _getch();
	if (ch == '1') {
		cout << endl;
		for (i = 33; i < 254; i++)
		{
			ch = i;
			if (!((i - 33) % 7)) cout << "\n";
			cout.width(4);
			cout << left << i;
			cout << "-> " << ch << "   ";
		}
	}
	cout << endl;
}

short wczytajKodZnaku() {
/* Przyjmuje liczby calkowite od 33 do 255 */

	string komunikatZacheta = "Podaj kod znaku ASCII (dopuszczalne od 33 do 255): ";
	string komunikatBlad = "\tPodano niewlasciwa wartosc, sprobuj ponownie.\n";
	short wejscie;
	do {
		cout << komunikatZacheta;
		if ((cin >> wejscie) && (wejscie > 32) && (wejscie < 255)) {
			//dobrze ale jak podasz np. 88mpiafmsnfls to smieci przerzuca na nastepne cin=...
			break;
		}
		else {
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			cout << komunikatBlad << endl;
		}
	} while (true);
	return wejscie;
}

short wczytajWysokoscKonsoli() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

short wczytajSzerokoscKonsoli() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

short getMaxRozmiarFigury() {
	return min(wczytajWysokoscKonsoli(), (wczytajSzerokoscKonsoli()+1)/2);
}

void gotoxy(short x, short y) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD c; //SHORT x, SHORT y

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	c.X = x + csbi.srWindow.Left;
	c.Y = y + csbi.srWindow.Top;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

short wczytajRozmiarPoczatkowy() {
/* Przyjmuje liczby ca³kowite od MIN_ROZMIAR_FIGURY do MAX_ROZMIAR_FIGURY */

	short wejscie;
	short maxRozmiarFigury;
	string komunikatZacheta = "Podaj rozmiar poczatkowy figury ";
	string komunikatBlad = "\tPodano niewlasciwa wartosc, sprobuj ponownie.\n";

	do {
		maxRozmiarFigury = getMaxRozmiarFigury();

		cout << komunikatZacheta;
		cout << "(min=" << MIN_ROZMIAR_FIGURY << ", max=" << maxRozmiarFigury << "): ";
		if ((cin >> wejscie) && (wejscie >= MIN_ROZMIAR_FIGURY) && (wejscie <= maxRozmiarFigury)) {
			//good type
			break;
		}
		else {
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			cout << komunikatBlad << endl;
		}
	} while (true);
	return wejscie;

	/*
	cout << komunikatZacheta;
	cin >> wejscie;

	while (cin.fail() || cin.get() != '\n' || wejscie < MIN_ROZMIAR_FIGURY || wejscie > maxRozmiarFigury) {
		//clear() wyczysci flage fail
		cin.clear();
		//ignore wyczysci pozostale dane znajdujace sie w buforze strumienia wejsciowego (cin) az do nowego wiersza 
		cin.ignore((numeric_limits<streamsize>::max)(), '\n'); //numeric_limits<streamsize>::max() nie dziala, za malo parametrow dla makra max()?!
		cout << "Niepoprawna wartosc." << endl;
		cout << "Podaj..." << endl;
		cin >> wejscie;
	}
	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	return wejscie;*/
}

void rysujFigure(short rozmiar, char znak, short x, short y)
{
	system("cls");
	short pozycjaPoprzeczki;
	bool czyParzystyRozmiar;

	if (rozmiar % 2 == 0) {
		czyParzystyRozmiar = 1;
	}
	else {
		czyParzystyRozmiar = 0;
	}

	if (rozmiar % 2 == 0) {
		pozycjaPoprzeczki = rozmiar / 2 - 1;
	}
	else {
		pozycjaPoprzeczki = (rozmiar - 1) / 2;
	}

	for (int i = 0; i < rozmiar; i++)
	{
		gotoxy(x + i, y + i);
		cout << znak;
		if (i < rozmiar - 1) {
			gotoxy(x + (2 * rozmiar - i - 2), y + i);
			cout << znak;
		}
		if (i < rozmiar - 2 + czyParzystyRozmiar) {
			gotoxy(x + pozycjaPoprzeczki + i + 1, y + pozycjaPoprzeczki);
			cout << znak;
		}
	}
}

unsigned char manipulujFigura(short &pozycjaX, short &pozycjaY, short &rozmiar)
{
	unsigned char wejscie;
	bool nieRysujPonownie;
	short szerokoscFigury = rozmiar * 2 - 1;
	short wysokoscKonsoli = wczytajWysokoscKonsoli();
	short szerokoscKonsoli = wczytajSzerokoscKonsoli();

	bool czyMoznaWGore = (pozycjaY > 0);
	bool czyMoznaWDol = ((pozycjaY + rozmiar) < wysokoscKonsoli);
	bool czyMoznaWLewo = (pozycjaX > 0);
	bool czyMoznaWPrawo = ((pozycjaX + szerokoscFigury) < szerokoscKonsoli);

	do {
		nieRysujPonownie = false;
		wejscie = _getch();
		//for detect the function\arrow keys we must call the getch() again, testing if a is '0' or '0xE0'
		if (wejscie == 0 || wejscie == 0xE0) wejscie = _getch();
		switch (wejscie) {
		case 72:
			if (czyMoznaWGore)
				pozycjaY--;
			break;
		case 80:
			if (czyMoznaWDol)
				pozycjaY++;
			break;
		case 75:
			if (czyMoznaWLewo)
				pozycjaX--;
			break;
		case 77:
			if (czyMoznaWPrawo)
				pozycjaX++;
			break;
		case 45:
			if (rozmiar > MIN_ROZMIAR_FIGURY)
				rozmiar--;
			break;
		case 43:
			if (((pozycjaY + rozmiar) < wysokoscKonsoli) && ((pozycjaX + szerokoscFigury + 1) < szerokoscKonsoli)) {
				rozmiar++;
			}
			break;
		case ESC: //Esc
			break;
		case ENTER: //Enter
			break;
		default:
			nieRysujPonownie = true;
			break;
		}
	} while (nieRysujPonownie);
	return wejscie;
}