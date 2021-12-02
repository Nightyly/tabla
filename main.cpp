#include <iostream>
#include <locale>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <sstream>

#include "tabla"
#include "map"

#define DB

using namespace std;

struct uint4{
    unsigned val : 4;
};

void SetWindow(int Width, int Height) {
    _COORD coord;
    coord.X = Width;
    coord.Y = Height;

    _SMALL_RECT Rect;
    Rect.Top = 0;
    Rect.Left = 0;
    Rect.Bottom = Height - 1;
    Rect.Right = Width - 1;

    HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleWindowInfo(Handle, TRUE, &Rect);
    SetConsoleScreenBufferSize(Handle, coord);
}

unsigned char getcharat(short x, short y){
    CHAR_INFO ci;
    COORD xy = {0, 0};
    SMALL_RECT rect = {x, y, x, y};
    _COORD c = {1, 1};
    return ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE),&ci,c,xy, &rect) ? ci.Char.AsciiChar : '\0';
}

void hidecursor(bool hide = true){
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 10;
   info.bVisible = !hide;
   SetConsoleCursorInfo(consoleHandle, &info);
}

vector<string> split(string str, char pattern) {

    int posInit = 0;
    int posFound = 0;
    string splitted;
    vector<string> results;

    while(posFound >= 0){
        posFound = str.find(pattern, posInit);
        splitted = str.substr(posInit, posFound - posInit);
        posInit = posFound + 1;
        results.push_back(splitted);
    }

    return results;
}

void gotoxy(int x,int y){
    HANDLE hcon;
    hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y= y;
    SetConsoleCursorPosition(hcon, dwPos);
}

string bohr(int electrones){
    int configuracion[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    #ifndef DB
    const int bohr[8] = {2, 8, 18, 32, 32, 18, 8, 2};
    for(int n = 0; electrones > 0; n++){
        if(electrones > bohr[n]){
            configuracion[n] = bohr[n];
            electrones -= bohr[n];
        }
        else if(electrones <= 8){
            configuracion[n] = electrones;
            break;
        }
        else{
            configuracion[n] = 8;
            configuracion[n + 1] = electrones - 8;
            int aux, aux2;
            aux = configuracion[n];
            aux2 = configuracion[n + 1];
            if(aux2 > aux){
                configuracion[n] = aux2;
                configuracion[n + 1] = aux;
            }
            break;
        }
    }
    #endif // DB
    #ifdef DB
    string nombre = tabla["order"][electrones - 1];
    for(int n = 0; n != 8 ; n++){
        try{
            configuracion[n] = tabla[nombre]["shells"][n];
        }
        catch(...){
            break;
        }
    }
    #endif // DB
    return fmt::format("\t\\\t \\\t \\\t \\\t \\\t \\\t \\\t \\\n@\t{0}\t {1}\t {2}\t {3}\t {4}\t {5}\t {6}\t {7}\n\t/\t /\t /\t /\t /\t /\t /\t /", configuracion[0], configuracion[1], configuracion[2], configuracion[3], configuracion[4], configuracion[5], configuracion[6], configuracion[7]);
}

void circundante(bool* buf, int x, int y){ //si es 0, significa que no hay enlaces para esa direccion
    unsigned char c;
    // arriba
    if(y == 0)
        buf[0] = 0;
    else{
        c = getcharat(x, y - 1);
        if(c != 185 && c != 186 && c != 187 && c != 201 && c != 203 && c != 204 && c != 206)
            buf[0] = 0;
        else
            buf[0] = 1;
    }

    // derecha
    c = getcharat(x + 1, y);
    if(c != 185 && c != 187 && c != 188 && c != 202 && c != 203 && c != 205 && c != 206)
        buf[1] = 0;
    else
        buf[1] = 1;

    //izquierda
    if(x == 0)
        buf[2] = 0;
    else{
        c = getcharat(x - 1, y);
        if(c != 200 && c != 201 && c != 202 && c != 203 && c != 204 && c != 205 && c != 206)
            buf[2] = 0;
        else
            buf[2] = 1;
    }

    //abajo
    c = getcharat(x, y + 1);
    if(c != 185 && c != 186 && c != 188 && c != 200 && c != 202 && c != 204 && c != 206)
        buf[3] = 0;
    else
        buf[3] = 1;
}

unsigned char caracter(const bool* circ){
    // 0: arriba, 1: derecha, 2: izquierda, 3: abajo
    unsigned char retval[16] = {0, 0, 0, 187, 0, 201, 205, 203, 0, 186, 188, 185, 200, 204, 202, 206};
    uint4 u4{0};
    u4.val |= (circ[0] << 3);
    u4.val |= (circ[1] << 2);
    u4.val |= (circ[2] << 1);
    u4.val |= circ[3];
    return retval[u4.val];
}

void caja(int x_pos, int y_pos, int dim_x, int dim_y){
    bool circ[4]; // 0: arriba, 1: derecha, 2: izquierda, 3: abajo
    circundante(circ, x_pos, y_pos);
    circ[3] = circ[1] = 1;
    gotoxy(x_pos, y_pos);
    ::printf("%c", caracter(circ));

    for(int n = 1; n != dim_x - 1; n++){
        if(dim_x == 1)
            break;
        circundante(circ, x_pos + n, y_pos);
        circ[1] = circ[2] = 1;
        ::printf("%c", caracter(circ));
    }
    circundante(circ, x_pos + dim_x - 1, y_pos);
    circ[2] = circ[3] = 1;
    ::printf("%c", caracter(circ));

    for(int n = 1; n != dim_y - 1; n++){
        if(dim_y == 1)
            break;
        gotoxy(x_pos, y_pos + n);
        circundante(circ, x_pos, y_pos + n);
        circ[0] = circ[3] = 1;
        ::printf("%c", caracter(circ));
    }
    for(int n = 1; n != dim_y - 1; n++){
        if(dim_y == 1)
            break;
        gotoxy(x_pos + dim_x - 1, y_pos + n);
        circundante(circ, x_pos + dim_x - 1, y_pos + n);
        circ[0] = circ[3] = 1;
        ::printf("%c", caracter(circ));
    }
    if(dim_y == 1)
        gotoxy(x_pos, y_pos + dim_y);
    else
        gotoxy(x_pos, y_pos + dim_y - 1);
    if(dim_y == 1)
        circundante(circ, x_pos, y_pos + dim_y);
    else
        circundante(circ, x_pos, y_pos + dim_y - 1);
    circ[0] = circ[1] = 1;
    ::printf("%c", caracter(circ));
    for(int n = 1; n != dim_x - 1; n++){
        if(dim_x == 1)
            break;
        circundante(circ, x_pos + n, y_pos + dim_y - 1);
        circ[1] = circ[2] = 1;
        ::printf("%c", caracter(circ));
    }
    if(dim_x == 1 && dim_y == 1)
        circundante(circ, x_pos + dim_x, y_pos + dim_y);
    else if(dim_x == 1)
        circundante(circ, x_pos + dim_x, y_pos + dim_y - 1);
    else if(dim_y == 1)
        circundante(circ, x_pos + dim_x - 1, y_pos + dim_y);
    else
        circundante(circ, x_pos + dim_x - 1, y_pos + dim_y - 1);
    circ[0] = circ[2] = 1;
    ::printf("%c", caracter(circ));
}

string ReplaceAll(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

string nummagneticos(int* buf, string conf){
    //0: n, 1: l, 2: m
    int nivel = stoi(conf);
    char letra = conf[1];
    int azimuto;
    int magnetico;
    int aux;
    int nelectrones = stoi(conf.substr(2));
    buf[0] = nivel;
    if(letra == 's'){
        azimuto = 0;
        magnetico = 0;
        aux = 1;
    }
    else if(letra == 'p'){
        azimuto = 1;
        if(nelectrones == 1 || nelectrones == 4)
            magnetico = -1;
        else if(nelectrones == 2 || nelectrones == 5)
            magnetico = 0;
        else
            magnetico = 1;
        aux = 3;
    }
    else if(letra == 'd'){
        azimuto = 2;
        if(nelectrones == 1 || nelectrones == 6)
            magnetico = -2;
        else if(nelectrones == 2 || nelectrones == 7)
            magnetico = -1;
        else if(nelectrones == 3 || nelectrones == 8)
            magnetico = 0;
        else if(nelectrones == 4 || nelectrones == 9)
            magnetico = 1;
        else
            magnetico = 2;
        aux = 5;
    }
    else{
        azimuto = 3;
        if(nelectrones == 1 || nelectrones == 8)
            magnetico = -3;
        else if(nelectrones == 2 || nelectrones == 9)
            magnetico = -2;
        else if(nelectrones == 3 || nelectrones == 10)
            magnetico = -1;
        else if(nelectrones == 4 || nelectrones == 11)
            magnetico = 0;
        else if(nelectrones == 5 || nelectrones == 12)
            magnetico = 1;
        else if(nelectrones == 6 || nelectrones == 13)
            magnetico = 2;
        else
            magnetico = 3;
        aux = 7;
    }
    buf[1] = azimuto;
    buf[2] = magnetico;

    if(nelectrones <= aux)
        return "+1/2";
    return "-1/2";

}

inline double celsius(double k){return k - 273.15;}

int main(){
    SetWindow(67, 16);
    cout << "\t\t         Seleccion: ";

    while(1){
        string el;
        string i2;
        int numero;
        cin >> el;
        hidecursor();
        system("cls");
        i2 = el;
        locale loc;
        try{
            numero = stoi(el);
            if(numero <= 0 || numero >= 120)
                throw invalid_argument("");
        }
        catch(...){
            for(unsigned int n = 0; n != el.size(); n++)
                el[n] = tolower(el[n], loc);
            if(simbolo.find(el) != simbolo.end()){ //esta en simbolo
                numero = simbolo[el];
            }
            else if(nombre.find(el) != nombre.end()){ //esta en nombre
                numero = nombre[el];
            }
            else{
                cout << "No se ha encontrado \"" << i2 << "\"." << endl;
                cout << "\t\t         Seleccion: ";
                hidecursor(false);
                continue;
            }
        }

        int f_size;

        string elem = tabla["order"][numero - 1];
        gotoxy(1, 1);
        cout << "Nombre: " << ReplaceAll(tabla[elem]["name"], "\"", "");
        gotoxy(45, 1);
        cout << "Masa atomica: " << to_string(tabla[elem]["atomic_mass"]).substr(0, 7);
        gotoxy(1, 3);
        try{
            cout << fixed;
            cout.precision(1);
            cout << "Fusion: " << celsius(tabla[elem]["melt"]);
            ::printf("%cC", 167);
            stringstream sstream;
            sstream << fixed;
            sstream.precision(2);
            double aux = celsius(tabla[elem]["melt"]);
            sstream << "Fusion: " << aux << "oo";
            f_size = sstream.str().size();
        }
        catch(...){
            cout << "Indefinido";
            f_size = 18;
        }
        gotoxy(45, 3);
        try{
            cout << "Ebullicion: " << to_string(celsius(tabla[elem]["boil"])).substr(0, 7);
            ::printf("%cC", 167);
        }
        catch(...){
            cout << "Indefinido";
        }
        gotoxy(27, 1);
        cout << "Simbolo: " << ReplaceAll(tabla[elem]["symbol"], "\"", "");
        gotoxy(27, 3);
        cout << "Numero: " << tabla[elem]["number"];
        vector<string> vec = split(bohr(tabla[elem]["number"]), '\n');
        for(unsigned int n = 0; n != vec.size(); n++){
            gotoxy(1, 5 + n);
            cout << vec[n] << endl;
        }

        #ifndef DB
        vector<string> conf = split(ReplaceAll(tabla[elem]["config"], "\"", ""), ' ');
        #endif // DB
        #ifdef DB
        vector<string> conf = split(ReplaceAll(tabla[elem]["electron_configuration"], "\"", ""), ' ');
        #endif // DB
        gotoxy(1, 9);
        cout << "Configuracion: ";
        int skip = 0;
        for(unsigned int n = 0; n != conf.size(); n++){
            if(n == 12){
                gotoxy(1, 10);
                skip = 1;
                cout << "               ";
            }
            cout << conf[n] << " ";
        }
        gotoxy(1, 10 + skip);
        #ifndef DB
        cout << "Configuracion(kernel): " << ReplaceAll(tabla[elem]["config_kernel"], "\"", "");
        #endif // DB
        #ifdef DB
        cout << "Configuracion(kernel): " << ReplaceAll(tabla[elem]["electron_configuration_semantic"], "\"", "");
        #endif // DB
        gotoxy(1, 12 + skip);
        int buf[3];
        string NMS;
        string auxstr;

        #ifndef DB
        auxstr = tabla[elem]["config"];
        #endif // DB
        #ifdef DB
        auxstr = tabla[elem]["electron_configuration"];
        #endif // DB

        vector<string> auxvec;
        auxvec = split(auxstr, ' ');
        NMS = nummagneticos(buf, auxvec.back());
        //0: n, 1: l, 2: m
        cout << "N:\t"<< buf[0] << "\tL:\t" << buf[1] << "\tM:\t" << buf[2] << "\tS:\t" << NMS;
        //creacion de cajas
        int aux;
        int aux2;
        aux = string("Nombre: " + ReplaceAll(tabla[elem]["name"], "\"", "")).size();
        if(aux >= f_size){
            caja(0, 0, aux + 2, 3); // Nombre
            caja(0, 2, aux + 2, 3); // Fusion
        }
        else{
            caja(0, 0, f_size + 2, 3); // Nombre
            caja(0, 2, f_size + 2, 3); // Fusion
        }
        aux = string("Simbolo: " + ReplaceAll(tabla[elem]["symbol"], "\"", "")).size();
        aux2 = string("Numero: " + to_string(tabla[elem]["number"])).size();
        if(aux >= aux2){
            caja(26, 0, aux + 2, 3); // simbolo
            caja(26, 2, aux + 2, 3); // numero
        }
        else{
            caja(26, 0, aux2 + 2, 3); // simbolo
            caja(26, 2, aux2 + 2, 3); // numero
        }
        caja(44, 0, 23, 3); // masa atomica
        caja(44, 2, 23, 3); // ebullicion
        caja(0, 0, 67, 12 + skip); // marco
        caja(0, 4, 67, 5); // bohr
        caja(0, 11 + skip, 67, 3); // numeros magneticos
        caja(15, 11 + skip, 17, 3); // NM: L
        caja(31, 11 + skip, 17, 3); // NM: M

        SetWindow(67, 15 + skip);
        SetWindow(67, 15 + skip);
        gotoxy(27, 14 + skip);
        cout << "Seleccion: ";
        hidecursor(false);
    }
}
