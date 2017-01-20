//Napisz program na rozwi¹zanie uk³adu dwóch równañ liniowych.
//min req.
//Rozmiar buforu ekranu 120 / 40 (ustalic)
//Rozmiar okna jw.
//min rozmiar figury

/* Bibliografia ;) 
* https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms682093(v=vs.85).aspx - CONSOLE_SCREEN_BUFFER_INFO
* https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms683171(v=vs.85).aspx - GetConsoleScreenBufferInfo
* http://www.cplusplus.com/forum/beginner/121194/#msg659728 - walidacja
* http://stackoverflow.com/questions/13212043/integer-input-validation-how - walidacja
* http://stackoverflow.com/questions/1904635/warning-c4003-and-errors-c2589-and-c2059-on-x-stdnumeric-limitsintmax
	- problem z konfliktem (numeric_limits<streamsize>::max)() VS max(), pomogl zwykly nawias
*/

#include <iostream>
#include <string>
//#define _USE_MATH_DEFINES //na potrzeby pi
//#include <math.h>
//#define NOMINMAX //
#include <windows.h>
#include <conio.h> // bibl Windowsowa
#include <limits> // dla cin.ignore(numeric_limits<streamsize>::max(), '\n');

using namespace std;

/*************************************************************************/
/* DEKLARACJE STA£YCH GLOBALNYCH */

const bool DEBUG_MODE = 1;
const short MIN_SZEROKOSC_KONSOLI = 120;
const short MIN_WYSOKOSC_KONSOLI = 40;
const short MIN_ROZMIAR_FIGURY = 5;
const char DOMYSLNY_ZNAK = 'X';

/*************************************************************************/
/* DEKLARACJE FUNKCJI */

void wypiszNaglowek() {
	cout << "\
PJATK Gdansk \n\
PRG1 2016/2017 \n\
Temat: Projekt semestralny - Program rysujacy zadana figure wybranym kodem ASCII \n\
Student: 16125 \n\n";
}

int wczytajLiczbeCalkowita(string, bool);

short wczytajKodZnaku();

short wczytajWysokoscKonsoli();
short wczytajSzerokoscKonsoli();

void gotoxy(short x, short y) {
	COORD c; //SHORT x, SHORT y
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
int wherex() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.dwCursorPosition.X;
	/*
	dwSize
     A COORD structure that contains the size of the console screen buffer, in character columns and rows.
	dwCursorPosition
	 A COORD structure that contains the column and row coordinates of the cursor in the console screen buffer.
	srWindow
	 A SMALL_RECT structure that contains the console screen buffer coordinates of the upper-left and lower-right corners of the display window.
	dwMaximumWindowSize
	 A COORD structure that contains the maximum size of the console window, in character columns and rows, given the current screen buffer size and font and the screen size.
	*/
}
int wherey() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.dwCursorPosition.Y;
}

short wczytajRozmiarPoczatkowy();

void wypiszASCII() {
	//short x = 0, short y = 0, short wierszy = 10, short kolumn = 60
	
	char ch;
	int i;
	for (i = 33; i < 255; i++)
	{
		ch = i;
		cout << i << "-> " << ch << "\t";
	}
}

void rysujFigure(short rozmiar, char znak, short x, short y)
{
	system("cls");
	short belkaFigury;
	short rozmiarObl = rozmiar / 2;
	bool czyParzystyRozmiar;

	if (rozmiar % 2 == 0) {
		czyParzystyRozmiar = 1;
	}
	else {
		czyParzystyRozmiar = 0;
	}

	rozmiarObl = rozmiarObl * 2;


	if (rozmiar % 2 == 0) {
		belkaFigury = rozmiar / 2;
	}
	else {
		belkaFigury = (rozmiar + 1) / 2;
	}

	for (int i = 0; i < rozmiarObl; i++)
	{
		gotoxy(x + i, y + i);
		cout << znak;
		gotoxy(x + (2 * rozmiarObl - i - czyParzystyRozmiar), y + i);
		cout << znak;
		gotoxy(x + belkaFigury + i + 1, y + belkaFigury);
		cout << znak;
		if (!czyParzystyRozmiar) {
			gotoxy(x + rozmiarObl, y + rozmiarObl);
			cout << znak;
		}
	}
	/*for (int i = 0; i < rozmiar / 2; i++)
	{
		gotoxy(x - ((rozmiar / 2) - i), y - ((rozmiar / 2) - i));
		cout << znak;
		gotoxy(x - ((rozmiar / 2) - i), y + ((rozmiar / 2) - i));
		cout << znak;
	}*/
}

void rysujFigure2(short coordX, short coordY, char znak, short rozmiar) {
	cout << znak;
}

bool figuraMain(char, short);


/*************************************************************************/
/* FUNKCJE POMOCNICZE */

char input2ascii() {
		char input;
		cin >> input;
		cout << (short)input;
		return input;
	}
void echoConsoleInfoOnly() { /*Funkcja na potrzeby badawcze ;)*/
	char znak;
	do {
		system("cls");
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		cout << "dwSize: " << csbi.dwSize.X << " , " << csbi.dwSize.Y << endl;
		cout << "dwCursorPosition: " << csbi.dwCursorPosition.X << " , " << csbi.dwCursorPosition.Y << endl;
		cout << "srWindowTL: " << csbi.srWindow.Left << " , " << csbi.srWindow.Top << endl;
		cout << "srWindowBR: " << csbi.srWindow.Right << " , " << csbi.srWindow.Bottom << endl;
		cout << "dwMaximumWindowSize: " << csbi.dwMaximumWindowSize.X << " , " << csbi.dwMaximumWindowSize.Y << endl;
		gotoxy(csbi.srWindow.Right, csbi.srWindow.Bottom);
		cout << "#";

		znak = _getch();
	} while (znak != 27);
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


/*************************************************************************/
/* START */

int main()
{
	char znak = DOMYSLNY_ZNAK;
	short rozmiar = MIN_ROZMIAR_FIGURY;
	short maxRozmiarFigury = MIN_WYSOKOSC_KONSOLI;
	
	if (DEBUG_MODE) { debugMode(); }

	wypiszNaglowek();
	
	znak = (char)wczytajKodZnaku();

	rozmiar = wczytajRozmiarPoczatkowy();
	
	cout << znak << endl;
	cout << rozmiar;

	rysujFigure(rozmiar,znak,0,0);

	//figuraMain(znak, rozmiar);
	
	//wypiszASCII();

	//wybierzZnak();

	//echoConsoleInfoOnly();

	


	//oczekiwanie na nacisniecie Esc
	//cout << endl << endl << "Kliknij dowolny klawisz, aby zakonczyc program...";
	_getch();

	//koniec funkcji main, koniec programu
	return EXIT_SUCCESS;
}
/* KONIEC */

/*************************************************************************/
/* DEFINICJE FUNKCJI */

int wczytajLiczbeCalkowita(string komunikat, bool czyMusiBycDodatnia) {
	int wczytanaLiczba;
	cout << komunikat << endl;
	cin >> wczytanaLiczba;
	//dopoki uzytkownik podaje zla wartosc pytamy ponownie - uzywamy petli while
	//cin.fail() zwraca prawde, jesli nie udalo sie poprawnie wczytac wartosci do zmiennej danego typu
	while (cin.fail() || (czyMusiBycDodatnia && wczytanaLiczba <= 0)) {
		//clear() wyczysci flage fail
		cin.clear();
		//ignore wyczysci pozostale dane znajdujace sie w buforze strumienia wejsciowego (cin) az do nowego wiersza 
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');
		cout << "Niepoprawna wartosc." << endl;
		cout << komunikat << endl;
		cin >> wczytanaLiczba;
	}
	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	return wczytanaLiczba;
}

short wczytajKodZnaku() {
/* Przyjmuje liczby calkowite od 33 do 255 */

	string komunikatZacheta = "Podaj kod znaku ASCII (dopuszczalne od 33 do 255): ";
	string komunikatBlad = "\tPodano niewlasciwa wartosc, sprobuj ponownie.\n";
	short wejscie;
	do {
		cout << komunikatZacheta;
		if ((cin >> wejscie) && (wejscie > 32) && (wejscie < 256)) {
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
	return csbi.srWindow.Bottom + 1;
}

short wczytajSzerokoscKonsoli() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Right + 1;
}

short wczytajRozmiarPoczatkowy() {
/* Przyjmuje liczby ca³kowite od MIN_ROZMIAR_FIGURY do MAX_ROZMIAR_FIGURY */

	short wejscie;
	short maxRozmiarFigury = wczytajWysokoscKonsoli();
	string komunikatZacheta = "Podaj rozmiar poczatkowy figury: ";
	string komunikatBlad = "\tPodano niewlasciwa wartosc, sprobuj ponownie.\n";

	do {
		cout << komunikatZacheta;
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

bool figuraMain(char znak, short rozmiarPoczatkowy) {
	void rysujFigure2(short coordX, short coordY, char znak, short rozmiar);

	return false;
}