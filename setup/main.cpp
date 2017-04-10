#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

#define PLANGHT 64
#define CONFCOL 9

using namespace std;

/**V struktura do ktorej jest wpisywana zawartosc pliku config V*/
struct conf{
    int resx;
    int resy;
    char lang[PLANGHT];
    int colors[CONFCOL][3];
    int levele;
    int windowed;
};

conf readconf();
void saveconf(conf tempconf);
int CreateIfNotExists( const char * sFileName, conf defaultconfig );

int main()
{
    conf defconf;
    defconf.resx = 800;
    defconf.resy = 600;
    char temptext[] = {'l','a','n','g','/','p','o','s','l','k','i','.','t','x','t','\0'};
    strcpy (defconf.lang, temptext);
    defconf.levele = 6;
    defconf.windowed = 2;
    int tempcolors[CONFCOL][3] = {
        {245,250,80},//pow
        {250,45,0},//fuel
        {64,255,64},//ang
        {250,170,20},//sco
        {127,127,127},//hud
        {64,255,64},//napi
        {245,250,80},//naglowek
        {250,170,20},//wfuel
        {130,255,130},//wsk
    };
    for (int i = 0; i < CONFCOL; ++i){
        defconf.colors[i][0] = tempcolors[i][0];
        defconf.colors[i][1] = tempcolors[i][1];
        defconf.colors[i][2] = tempcolors[i][2];
    }
    if(!CreateIfNotExists("conf.txt", defconf)){

        return 0;
    }
    int kursor = 1;
    conf config = readconf();
    while(kursor != 0) {
        cout <<"=========================\n";
        cout <<"001)resolution: " << config.resx << " x "<< config.resy <<"  windowed: " << config.windowed << "\n";
        cout <<"002)lang: " << config.lang << "\n";
        cout <<"003)levele: " << config.levele << "\n";
        cout <<"___)kolory: " << "\n";
        cout <<"004)      power:    " << config.colors[0][0] << ", "<< config.colors[0][1] << ", "<< config.colors[0][2] << "\n";
        cout <<"005)      fuel:     " << config.colors[1][0] << ", "<< config.colors[1][1] << ", "<< config.colors[1][2] << "\n";
        cout <<"006)      angle:    " << config.colors[2][0] << ", "<< config.colors[2][1] << ", "<< config.colors[2][2] << "\n";
        cout <<"007)      score:    " << config.colors[3][0] << ", "<< config.colors[3][1] << ", "<< config.colors[3][2] << "\n";
        cout <<"008)      hud:      " << config.colors[4][0] << ", "<< config.colors[4][1] << ", "<< config.colors[4][2] << "\n";
        cout <<"009)      napisy:   " << config.colors[5][0] << ", "<< config.colors[5][1] << ", "<< config.colors[5][2] << "\n";
        cout <<"010)      napisy2:  " << config.colors[6][0] << ", "<< config.colors[6][1] << ", "<< config.colors[6][2] << "\n";
        cout <<"011)      fuel2:    " << config.colors[7][0] << ", "<< config.colors[7][1] << ", "<< config.colors[7][2] << "\n";
        cout <<"012)      wskaznik: " << config.colors[8][0] << ", "<< config.colors[8][1] << ", "<< config.colors[8][2] << endl;
        cout <<"013)Przywroc ustawienia domyslne." << "\n";
        cout <<"==========================\n";
        cout <<"\nPodaj numer lini, ktora chcesz zmodyfikowac (aby zamknac wpisz \"0\"):\n";
        cin >> kursor;

        for (int f1 = 0; f1 < 25; ++f1) /**< wypelnianie ekranu pustymi liniami */
            cout << "\n";

        switch (kursor){
        case 1:
            cout << "Podaj szerokosc (x):\n";
            cin >> config.resx;
            cout << "Podaj wysokosc (y):\n";
            cin >> config.resy;
            cout << "Uruchomic w oknie \"1\" lub:\n";
            cin >> config.windowed;
            break;
        case 2:
            char tempcharar[PLANGHT];
            cout << "Podaj sciezke do pliku tekstowego\n" << "lang/";
            cin >> tempcharar;
            sprintf(config.lang, "lang/%s", tempcharar);
            break;
        case 3:
            cout << "Podaj numer ostatniego poziomu\n";
            cin >> config.levele;
            break;
        case 4:
            cout << "Podaj nowy kolor, dla wskaznika mocy [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[0][0];
            cout << "G: ";
            cin >> config.colors[0][1];
            cout << "B: ";
            cin >> config.colors[0][2];
            break;
        case 5:
            cout << "Podaj nowy kolor, dla wskaznika paliwa [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[1][0];
            cout << "G: ";
            cin >> config.colors[1][1];
            cout << "B: ";
            cin >> config.colors[1][2];
            break;
        case 6:
            cout << "Podaj nowy kolor, dla wskaznika obrotu [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[2][0];
            cout << "G: ";
            cin >> config.colors[2][1];
            cout << "B: ";
            cin >> config.colors[2][2];
            break;
        case 7:
            cout << "Podaj nowy kolor, dla wskaznika punktow [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[3][0];
            cout << "G: ";
            cin >> config.colors[3][1];
            cout << "B: ";
            cin >> config.colors[3][2];
            break;
        case 8:
            cout << "Podaj nowy kolor, dla HUDu [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[4][0];
            cout << "G: ";
            cin >> config.colors[4][1];
            cout << "B: ";
            cin >> config.colors[4][2];
            break;
        case 9:
            cout << "Podaj nowy kolor, dla napisow [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[5][0];
            cout << "G: ";
            cin >> config.colors[5][1];
            cout << "B: ";
            cin >> config.colors[5][2];
            break;
        case 10:
            cout << "Podaj nowy kolor, dla napisow(2) [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[6][0];
            cout << "G: ";
            cin >> config.colors[6][1];
            cout << "B: ";
            cin >> config.colors[6][2];
            break;
        case 11:
            cout << "Podaj nowy kolor, dla wskaznika paliwa(2) [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[7][0];
            cout << "G: ";
            cin >> config.colors[7][1];
            cout << "B: ";
            cin >> config.colors[7][2];
            break;
        case 12:
            cout << "Podaj nowy kolor, dla wskaznika [R,G,B]:\n";
            cout << "R: ";
            cin >> config.colors[8][0];
            cout << "G: ";
            cin >> config.colors[8][1];
            cout << "B: ";
            cin >> config.colors[8][2];
            break;
        case 13:
            cout << "Resetuje ustawienia.\n";
            config = defconf;
            break;
        }
        saveconf(config);
    }
    return 0;
}


conf readconf(){ //wczytuje plik config
    conf tempconf;
    fstream conffile ("conf.txt", ios::in);
    while(conffile.is_open() && !conffile.eof()){
        unsigned int param = 0;
        char buf[1024];
        char litera;
        for(int i = 0; i < 1024; ++i){
            conffile.get(litera);
            if(litera == '['){
                buf[i] = '\0';
                break;
            }
            buf[i] = litera;
        }
        for(int i = 0; i < 1024; ++i){
            if(buf[i]=='\0') break;
            param = param + buf[i];
        }
        int ind = -1;
        switch (param){
            case 1108:
                tempconf.resx = 0;
                tempconf.resy = 0;
                while (litera != '[') conffile.get(litera);
                while (litera != 'x') {
                    conffile.get(litera);
                    if(litera != 'x') tempconf.resx = tempconf.resx *10 + litera-48;
                }
                while (litera != ']') {
                    conffile.get(litera);
                    if(litera != ']') tempconf.resy = tempconf.resy *10 + litera-48;
                }
                break;
            case 418:
                tempconf.lang[0] = 'l';
                tempconf.lang[1] = 'a';
                tempconf.lang[2] = 'n';
                tempconf.lang[3] = 'g';
                tempconf.lang[4] = '/';
                while (litera != '[') conffile.get(litera);
                for (int i = 5; i<PLANGHT; ++i){
                    conffile.get(litera);
                    if ( litera == ']') {
                        tempconf.lang[i] = '\0';
                        break;
                    }
                    tempconf.lang[i] = litera;
                }
                break;
            case 651:
                tempconf.levele = 0;
                while (litera != '[') conffile.get(litera);
                while (litera != ']') {
                    conffile.get(litera);
                    if(litera != ']') tempconf.levele = tempconf.levele *10 + litera-48;
                }
                break;
            //falltrough przez kolory
            case 545:
                if (ind == -1) ind = 0; //hud pow col
            case 523:
                if (ind == -1) ind = 1; //hud fuel col
            case 513:
                if (ind == -1) ind = 2; //hud ang col
            case 528:
                if (ind == -1) ind = 3; //hud sco col
            case 300:
                if (ind == -1) ind = 4; //hud col bac
            case 306:
                if (ind == -1) ind = 5; //napi col
            case 434:
                if (ind == -1) ind = 6; //napi naglowek stopka col
            case 541:
                if (ind == -1) ind = 7; //hud wskaznik fuel col
            case 544:
                if (ind == -1) ind = 8; //hud wskaznik col
                while (litera != '[') conffile.get(litera);
                for (int i = 0; i < 3; ++i){
                    conffile.get(litera);
                    tempconf.colors[ind][i] = litera - 48;
                    conffile.get(litera);
                    tempconf.colors[ind][i] = tempconf.colors[ind][i]*10 + litera - 48;
                    conffile.get(litera);
                    tempconf.colors[ind][i] = tempconf.colors[ind][i]*10 + litera - 48;
                }
                break;
            //falltrough END
            case 865:
                while (litera != '[') conffile.get(litera);
                conffile.get(litera);
                tempconf.windowed = litera - 48;
                break;
        }
        conffile.getline(buf, 1024);

    }
    conffile.close();
    return tempconf;
}

void saveconf(conf tempconf){
    fstream conffile ("conf.txt", ios::out|ios::trunc);
    if (conffile.is_open()){
        char bufor[1024];

        sprintf(bufor, "resolution[%dx%d]::\n?",tempconf.resx, tempconf.resy);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "lang[%s]::\n?",tempconf.lang);
        for(int i = 0; bufor[i] != '?'; ++i) {
            conffile.put(bufor[i]);
            if (bufor[i] == '[') i += 5;
        }

        sprintf(bufor, "levels[%d]::\n?",tempconf.levele);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "windowed[%d]::\n?",tempconf.windowed);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcpow[%03d%03d%03d]::\n?", tempconf.colors[0][0], tempconf.colors[0][1], tempconf.colors[0][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcfue[%03d%03d%03d]::\n?", tempconf.colors[1][0], tempconf.colors[1][1], tempconf.colors[1][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcang[%03d%03d%03d]::\n?", tempconf.colors[2][0], tempconf.colors[2][1], tempconf.colors[2][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcsco[%03d%03d%03d]::\n?", tempconf.colors[3][0], tempconf.colors[3][1], tempconf.colors[3][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hca[%03d%03d%03d]::\n?", tempconf.colors[4][0], tempconf.colors[4][1], tempconf.colors[4][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "nca[%03d%03d%03d]::\n?", tempconf.colors[5][0], tempconf.colors[5][1], tempconf.colors[5][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "ncns[%03d%03d%03d]::\n?", tempconf.colors[6][0], tempconf.colors[6][1], tempconf.colors[6][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcfuw[%03d%03d%03d]::\n?", tempconf.colors[7][0], tempconf.colors[7][1], tempconf.colors[7][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcwsk[%03d%03d%03d]::?", tempconf.colors[8][0], tempconf.colors[8][1], tempconf.colors[8][2]);
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);
    }
    conffile.close();
}

int CreateIfNotExists( const char * sFileName, conf defaultconfig ) {
    ifstream otworzPlik;
    otworzPlik.open( sFileName );
    if( otworzPlik.good() )
    {
        otworzPlik.close();
        return 1; //plik istnieje
    }
    ofstream nowyPlik;
    nowyPlik.open( sFileName );
    if( nowyPlik.good() )
    {
        nowyPlik.close();
        saveconf(defaultconfig);
        return 2; //uda³o siê utworzyæ plik
    }
    return 0; //nie da siê utworzyæ pliku i plik nie istnieje
}
