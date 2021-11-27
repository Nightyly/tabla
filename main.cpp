#include <iostream>
#include <locale>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <windows.h>
#include <stdio.h>
#include <algorithm>

#include "tabla"
#include "map"

using namespace std;

struct uint4{
    unsigned val : 4;
};

unsigned char getcharat(short x, short y){
    CHAR_INFO ci;
    COORD xy = {0, 0};
    SMALL_RECT rect = {x, y, x, y};
    _COORD c = {1, 1};
    return ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE),&ci,c,xy, &rect) ? ci.Char.AsciiChar : '\0';
}

void hidecursor(){
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
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
    const int bohr[8] = {2, 8, 18, 32, 32, 18, 8, 2};
    for(int n = 0; electrones > 0; n++){
        if(electrones - bohr[n] > 0){
            electrones -= bohr[n];
            configuracion[n] = bohr[n];
        }
        else{
            if(electrones <= 8)
                configuracion[n] = electrones;
            else{
                configuracion[n + 1] = 8;
                electrones -= 8;
                configuracion[n] = electrones;
            }
            break;
        }
    }
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

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

inline double celsius(double k){return k - 273.15;}

int main(){
    hidecursor();

    while(1){
        string el;
        string i2;
        string nombrestr;
        int numero;
        cin >> el;
        system("cls");
        i2 = el;
        locale loc;
        try{
            numero = stoi(el);
        }
        catch(...){
            for(int n = 0; n != el.size(); n++)
                el[n] = tolower(el[n], loc);
            if(simbolo.find(el) != simbolo.end()){ //esta en simbolo
                numero = simbolo[el];
                nombrestr = simbolo.find(el) -> second;
            }
            else if(nombre.find(el) != nombre.end()){ //esta en nombre
                numero = nombre[el];
                nombrestr = nombre.find(el) -> first;
            }
            else{
                cout << "No se ha encontrado \"" << i2 << "\"." << endl;
                continue;
            }
        }
        nombrestr[0] = toupper(nombrestr[0], loc);
        string elem = tabla["order"][numero - 1];
        vector<string> vec = split(bohr(tabla[elem]["number"]), '\n');
        gotoxy(1, 1);
        cout << "Nombre: " << nombrestr;
        gotoxy(45, 1);
        cout << "Masa atomica: " << tabla[elem]["atomic_mass"];
        gotoxy(1, 3);
        cout << "Fusion: " << celsius(tabla[elem]["melt"]);
        ::printf("%cC", 167);
        gotoxy(45, 3);
        cout << "Ebullicion: " << celsius(tabla[elem]["boil"]);
        ::printf("%cC", 167);
        //creacion de cajas
        caja(0, 0, string("Nombre: " + nombrestr).size() + 2, 3);
        for(int n = 0; n != vec.size(); n++){
            gotoxy(1, 5 + n);
            cout << vec[n] << endl;
        }
        //caja(0, 0, 67, 5);
    }
}
