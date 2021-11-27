#include <iostream>
#include <locale>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <windows.h>
#include <stdio.h>

#include "tabla"
#include "map"

using namespace std;

char getcharat(short x, short y){
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
    SetConsoleCursorPosition(hcon,dwPos);  
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
                configuracion[n] = 8;
                electrones -= 8;
                configuracion[n + 1] = electrones;
            }
            break;
        }
    }
    return fmt::format("\t\\\t \\\t \\\t \\\t \\\t \\\t \\\t \\\n@\t{0}\t {1}\t {2}\t {3}\t {4}\t {5}\t {6}\t {7}\n\t/\t /\t /\t /\t /\t /\t /\t /", configuracion[0], configuracion[1], configuracion[2], configuracion[3], configuracion[4], configuracion[5], configuracion[6], configuracion[7]);
}

void circundante(bool* buf, int x, int y){
    // arriba
    if(y == 0)
        buf[0] = 0;
    else{
        char c = getcharat(x, y - 1);
        if(c != 188 && c != 200 && c != 202 && c != 205)
            buf[0] = 1;
        else
            buf[0] = 0;
    }
}

void caja(int x_pos, int y_pos, int dim_x, int dim_y){
    gotoxy(x_pos, y_pos);
    bool circ[4]; // 0: arriba, 1: derecha, 2: izquierda, 3: abajo
    circundante(circ, x_pos, y_pos);
    circ[3] = circ[1] = 1;
    char c = getcharat(x_pos, y_pos);
    
    for(int n = 1; n != dim_x; n++)
        ::printf("%c", 205);
    ::printf("%c", 187);
    for(int n = 1; n != dim_y - 1; n++){
        gotoxy(x_pos, y_pos + n);
        ::printf("%c", 186);
    }
    for(int n = 1; n != dim_y - 1; n++){
        gotoxy(x_pos + dim_x, y_pos + n);
        ::printf("%c", 186);
    }
    gotoxy(x_pos, y_pos + dim_y - 1);
    ::printf("%c", 200);
    for(int n = 1; n != dim_x; n++){
        ::printf("%c", 205);
    }
    ::printf("%c", 188);
}

int main(){
    hidecursor();

    while(1){
        string elem;
        string i2;
        int numero;
        cin >> elem;
        system("cls");
        i2 = elem;
        locale loc;
        try{
            numero = stoi(elem); 
        }
        catch(...){
            for(int n = 0; n != elem.size(); n++)
                elem[n] = tolower(elem[n], loc);
            if(simbolo.find(elem) != simbolo.end()){ //esta en simbolo
                numero = simbolo[elem];
                i2 = "";
                i2.shrink_to_fit();
            }
            else if(nombre.find(elem) != nombre.end()){ //esta en nombre
                numero = nombre[elem];
                i2 = "";
                i2.shrink_to_fit();
            }
            else{
                cout << "No se ha encontrado \"" << i2 << "\"." << endl;
                continue;
            }
        }
        string el = tabla["order"][numero - 1];
        vector<string> vec = split(bohr(119), '\n');
        for(int n = 0; n != vec.size(); n++){
            gotoxy(1, 1 + n);
            cout << vec[n] << endl;
        }
        caja(0, 0, 66, 5);
        gotoxy(0, 5);
        caja(1, 1, 3, 7);
    }
}