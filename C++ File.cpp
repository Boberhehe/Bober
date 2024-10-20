#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <exception>
#include <iomanip>
#include <limits>
#include <stack>
#include <iterator>
#include <algorithm>
using namespace std;

void fun(ifstream& input)
{
	try {
		string line;
		stack<string> tagi;
		getline(input, line);

		if (line.find("<math") != 0) throw runtime_error("To nie jest wyrazenie MathML");
		int i;
		for (i = 2; getline(input, line); i++) {

			if (line.find("<msup>") == 0) {
				if (line.size() >= sizeof("<msup>")) throw runtime_error("Po tagu <msup> nie moze nic byc");
				tagi.push("<msup>");
				continue;
			}
			else if (line.find("</msup>") == 0) {
				if (tagi.empty() || tagi.top() != "<msup>") {
					throw runtime_error("Blad: Zamykajacy tag </msup> bez otwierajacego w linii " + to_string(i));
				}
				else {
					if (line.size() >= sizeof("</msup>")) throw runtime_error("Po tagu </msup> nie moze nic byc");
					tagi.pop();
					continue;
				}
			}

			else if (line.find("<mfenced>") == 0) {
				tagi.push("<mfenced>");
				continue;
			}
			else if (line.find("</mfenced>") == 0) {
				if (tagi.empty() || tagi.top() != "<mfenced>") {
					throw runtime_error("Blad: Zamykajacy tag </mfenced> bez otwierajacego w linii " + to_string(i));
				}
				else {
					tagi.pop();
					continue;
				}
			}

			else if (line.find("<mrow>") == 0) {
				tagi.push("<mrow>");
				continue;
			}
			else if (line.find("</mrow>") == 0) {
				if (tagi.empty() || tagi.top() != "<mrow>") {
					throw runtime_error("Blad: Zamykajacy tag </mrow> bez otwierajacego w linii " + to_string(i));
				}
				else {
					tagi.pop();
					continue;
				}
			}

			if (line.find("<mi>") == 0) {
				tagi.push("<mi>");
				if (line.find("</mi>") != string::npos) tagi.pop();
				else throw runtime_error("Blad: Otwierajacy tag <mi> bez zamykajacego w linii " + to_string(i));
			}
			else if (line.find("</mi>") != string::npos) {
				if (tagi.empty() || tagi.top() != "<mi>") throw runtime_error("Blad: Zamykajacy tag </mi> bez otwierajacego w linii " + to_string(i));
			}

			else if (line.find("<mo>") == 0) {
				tagi.push("<mo>");
				if (line.find("</mo>") != string::npos) tagi.pop();
				else throw runtime_error("Blad: Otwierajacy tag <mo> bez zamykajacego w linii " + to_string(i));
			}
			else if (line.find("</mo>") != string::npos) {
				if (tagi.empty() || tagi.top() != "<mo>") throw runtime_error("Blad: Zamykajacy tag </mo> bez otwierajacego w linii " + to_string(i));
			}

			else if (line.find("<mn>") == 0) {
				tagi.push("<mn>");
				if (line.find("</mn>") != string::npos) tagi.pop();
				else throw runtime_error("Blad: Otwierajacy tag <mn> bez zamykajacego w linii " + to_string(i));
			}
			else if (line.find("</mn>") != string::npos) {
				if (tagi.empty() || tagi.top() != "<mn>") throw runtime_error("Blad: Zamykajacy tag </mn> bez otwierajacego w linii " + to_string(i));
			}

			else if(line.find("</math>")==string::npos) throw runtime_error("Blad: Nierozpoznany tag w linii " + to_string(i));
		}

		if (line.find("</math>") == string::npos) throw runtime_error("Blad w skladniku zamykajacym </math>");
		if (i <= 3) throw runtime_error("Blad: Wyrazenie jest puste lub zapisane w jednej linii");
		if (!tagi.empty()) throw runtime_error("Blad: Nie wszystkie tagi zostaly zamkniete");
	}

	catch (runtime_error& e) {
		/*ofstream logfile("log.txt", ios::app);
		logfile << e.what() << endl;*/
		cout << e.what();
	}

}

int main()
{
	try {
		ifstream plik("Lab2.txt");
		if (!plik.is_open()) throw ios_base::failure("Blad przy otwarciu pliku");
		fun(plik);
	}

	catch (ios_base::failure& e) {
		cout << e.what();
	}
}







class mojWyjatek :public exception
{
	const char* text;
public:
	mojWyjatek(const char* s) :text(s) {};
	const char* what() const { return text; }
};

void parser(const string& nazwa)
{
	ifstream input(nazwa);
	if (!input.is_open()) throw ios_base::failure("Blad otwarcia pliku");
	input.seekg(0, ios::end);
	if (input.tellg() == 0) throw runtime_error("Plik jest pusty");
	input.seekg(0, ios::beg);

	int licznik[256] = { 0 };
	char znak, poprz;
	for (int i = 0; input >> znak;)
		if (znak == '*' || znak == '_' || znak == '\'') {
			if (i == 0) {
				poprz = znak;
				licznik[znak]++;
				i++;
			}
			else if (poprz == znak && licznik[znak] % 2 != 0) licznik[znak]++;
			else if (poprz != znak && licznik[poprz] % 2 != 0) {
				mojWyjatek m("Nie moze byc zagniezdzania");
				throw m;
			}
			else {
				licznik[znak]++;
				poprz = znak;
			}
		}

	for (int i = 0; i < 256; ++i)
		if (licznik[i] > 0 && licznik[i] % 2 != 0) {
			mojWyjatek m("Liczba znakow formatujacych musi byc parzysta");
			throw m;
		}

	//for (int i = 0; i < 256; i++)
	//	if (licznik[i] > 0) printf("%c: %d\n", i, licznik[i]);
}

int main()
{
	try { parser("parser.txt"); }

	catch (const ios_base::failure& e) {
		cout << e.what();
	}
	catch (const runtime_error& e) {
		cout << e.what();
	}
	catch (const mojWyjatek& e) {
		cout << e.what();
	}
	cout << endl;
	return 0;
}







class mojWyjatek :public exception
{
public:
	const char* what() const { return "Za duzo niepoprawnych odpowiedzi\n"; }
};

int losuj(int n)
{
	int wrong = 0;
	time_t start, finish;
	srand((unsigned)time(NULL));
	cout << "Podaj kod ASCII liter\n";

	time(&start);
	for (int i = 0; i < n; i++) {
		if (wrong == 2) {
			mojWyjatek m;
			throw m;
		}

		int guess;
		char litera = 'a' + rand() % 26;
		cout << litera << ": ";
		cin >> guess;
		if (guess != (int)litera) wrong++;
	}
	time(&finish);

	return difftime(finish, start);
}

int main()
{
	bool koniec = false;
	int czas=0;
	while (!koniec) {
		try {
			cout << "Podaj n: ";
			int n;
			cin >> n;
			czas = losuj(n);
			koniec = true;
		}

		catch (mojWyjatek& e) {
			cout << e.what();
			cout << "Czy chcesz sprobowac ponownie?\n1) Tak\n2) Nie\n";

			int odpowiedz;
			cin >> odpowiedz;
			if (odpowiedz == 2) koniec = true;
			else koniec = false;
		}
	}
	cout << "Koniec gry\n";
	cout << "Czas: " << czas;
	return 0;
}







bool isOperand(char x)
{
	if (x == '+' || x == '-' || x == '*' || x == '/') return false;
	else return true;
}

int oblicz(const string& postfix)
{
	stack<int> s;

	for (int i = 0; i < postfix.length(); i++) {
		if (isOperand(postfix[i]) && !isdigit(postfix[i])) throw runtime_error("Wyrazenie nie moze miec liter");
		else if (isOperand(postfix[i])) s.push(postfix[i] - '0');
		else {
			if (s.size() < 2) throw runtime_error("Za malo operandow");
			int prawy = s.top();
			s.pop();
			int lewy = s.top();
			s.pop();
			switch (postfix[i]) {
			case '+':
				s.push(lewy + prawy);
				break;

			case '-':
				s.push(lewy - prawy);
				break;

			case '*':
				s.push(lewy * prawy);
				break;

			case '/':
				s.push(lewy / prawy);
				break;
			}
		}
	}
	return s.top();
}

int main()
{
	try {
		ifstream input("postfix.txt");
		if (!input) throw ios_base::failure("File not found");
		input.seekg(0, ios::end);
		if (input.tellg() == 0) throw runtime_error("File is empty");
		input.seekg(0, ios::beg);

		while (!input.eof()) {
			string postfix;
			input >> postfix;
			cout << oblicz(postfix) << endl;
		}
	}
	catch (const ios_base::failure& e) {
		cerr << "I/O Error: " << e.what() << endl;
	}
	catch (const runtime_error& e) {
		cerr << "Runtime Error: " << e.what() << endl;
	}
	catch (const exception& e) {
		cerr << "Error: " << e.what() << endl;
	}

	return 0;
}