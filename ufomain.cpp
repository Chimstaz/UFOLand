#include <allegro.h>
#include <alpng.h>
#include <math.h>
#include <iostream>
//#include <conio.h>
#include <fstream>
#include <openssl/sha.h>
#include <boost/chrono.hpp>
#include <vector>
using namespace std;

#define PI 3.14159265
#define MAXSPEED 30 /**< maksymalna/minimalna predkosc statku (zmiana wiaze sie z przerobieniem hud[tylko obrazka]) */
#define MINSPEED 0
/**V wartosci wyjscia z funkcji menu() V*/
#define KOMQUIT 5
#define KOMPLAY 1
#define KOMINST 2
#define KOMOPT 4
#define KOMSCO 3
#define KOMEDIT 6

#define MOVNUM 32 /**< ilosc moverow na poziom */
#define MOVFRAME 10 /**< maksymalna ilosc obrazkow dla movera (jesli wieksze niz 10 to trzeba zmienic funkcje wczytujaca) */
#define PLANGHT 64 /**< maksymalna dlugosc sciezki */
#define SCORL 16 /**< maksymalna ilosc cyfr w wyniku punktowym */
#define CONFCOL 9 /**< ilosc kolorow w configu */
#define OPTNUMLIN 16
/**V maksymalne dlugosci konkretnych wierszy w pliku lang V*/
#define NAZWAL 32
#define MGRAL 32
#define MINSL 32
#define MKONL 32
#define MOPTL 32
#define MSCOL 32
#define INSTRLINEL 32
#define POWERL 16
#define ANGLEL 16
#define PAUSEL 16
#define WINL 16
#define FUELL 16
#define HSNW1L 32
#define HSNW2L 32
#define HSNWS1L 32
#define HSNWS2L 32
#define HSNAMEL 32
#define HSVNL 32
#define HSVSL 32
#define OPTLINEL 24
/**V structura przechowujaca ruchome obiekty V*/
struct mover{
	   fstream conf; // otwarty plik zawierajacy opis klatek movera
	   char path[PLANGHT]; //sciezka do pliku zawierajacego opis klatek movera (??)
	   char img[PLANGHT]; //sciezka do pliku zawierajacego spis obrazkow
	   BITMAP * frame[MOVFRAME]; //wszystkie(max 10) obrazkow uzywanych przez mover (nie trzeba ciagle wczytywac)
	   int x; // wspolzedne aktualnej klatki
	   int y;
       short int z; //okresla kolejnosc rysowania (dla aktualnej klatki)
	   unsigned int num; //miejsce w ktorym skonczono czytac plik movera (od tego miejsca bedzie czytana nastepna klatka)
	   short int kat; // kat obrotu aktualnej klatki
	   short int numframe; // aktualnie uzywany obrazek (z listy frame)
	   bool istnieje; // czy mover jest uzywany na danym poziomie
};
/**V structura przechowujaca wszystkie napisy uzyte w grze V*/
struct napi {
       char nazwa[NAZWAL];
       char mgra[MGRAL];
       char mins[MINSL];
       char mkon[MKONL];
       char mopt[MOPTL];
       char msco[MSCOL];
       char instr1_1[INSTRLINEL];
       char instr1_2[INSTRLINEL];
       char instr1_3[INSTRLINEL];
       char instr1_4[INSTRLINEL];
       char instr1_5[INSTRLINEL];
       char instr1_6[INSTRLINEL];
       char instr1_7[INSTRLINEL];
       char instr2_1[INSTRLINEL];
       char instr2_2[INSTRLINEL];
       char instr2_3[INSTRLINEL];
       char instr2_4[INSTRLINEL];
       char instr2_5[INSTRLINEL];
       char instr2_6[INSTRLINEL];
       char instr2_7[INSTRLINEL];
       char instr3_1[INSTRLINEL];
       char instr3_2[INSTRLINEL];
       char instr3_3[INSTRLINEL];
       char instr3_4[INSTRLINEL];
       char instr3_5[INSTRLINEL];
       char instr3_6[INSTRLINEL];
       char instr3_7[INSTRLINEL];
       char instr4_1[INSTRLINEL];
       char instr4_2[INSTRLINEL];
       char instr4_3[INSTRLINEL];
       char instr4_4[INSTRLINEL];
       char instr4_5[INSTRLINEL];
       char instr4_6[INSTRLINEL];
       char instr4_7[INSTRLINEL];
       char instr1_n[INSTRLINEL];
       char instr1_s[INSTRLINEL];
       char instr2_n[INSTRLINEL];
       char instr2_s[INSTRLINEL];
       char instr3_n[INSTRLINEL];
       char instr3_s[INSTRLINEL];
       char instr4_n[INSTRLINEL];
       char instr4_s[INSTRLINEL];
       char power[POWERL];
       char angle[ANGLEL];
       char pause[PAUSEL];
       char win[WINL];
       char fuel[FUELL];
       char scor[SCORL];
       char hsnw1[HSNW1L];
       char hsnw2[HSNW2L];
       char hsnws1[HSNWS1L];
       char hsnws2[HSNWS2L];
       char hsname[HSNAMEL];
       char hsvn[HSVNL];
       char hsvs[HSVSL];
       char opt0[OPTNUMLIN][OPTLINEL];
       char opt_roz[OPTNUMLIN][OPTLINEL];
       char opt_col[OPTNUMLIN][OPTLINEL];
       char opt_lang[OPTNUMLIN][OPTLINEL];
};
/**V struktura do ktorej jest wpisywana zawartosc pliku config V*/
struct conf{
    int resx; //rozdielczosc
    int resy;
    char lang[PLANGHT]; // sciezka do pliku jezykowego
    int colors[CONFCOL]; // kolory podlegajace modyfikacji
    int levele; // ilosc poziomow
    int windowed; // okno/fullscreen
};
/**V struktura przechowujaca klatke movera w trybie edycji V*/
struct e_mov{
	   int x; // wspolzedne aktualnej klatki
	   int y;
       short int z; //okresla kolejnosc rysowania (dla aktualnej klatki)
	   short int kat; // kat obrotu aktualnej klatki
	   short int numframe; // aktualnie uzywany obrazek (z listy frame)
	   bool istnieje; // czy mover jest uzywany na danym poziomie
};

BITMAP * tworzenie_okna_i_bufora(int szerokosc, int wysokosc, int windowed);
void obliczanie_sil (int ciag, int kat_a, int przyciaganie, double zwrot[]);
void sterowanie(int &ciag, int &obrot);
void wskaznik (int x, int y, BITMAP * bufor, int col);
void odwrocenie ( int kat_ob, BITMAP * bufor, int xprze, int color);
void predkosc ( int ciag, BITMAP *bufor, int xprze, int color);
void hudfuel ( int fuel, int fuelall, BITMAP* bufor, int xprze, int color);
void hudcolor (int colors[CONFCOL], BITMAP* hud);
void generatehud (BITMAP* hud, conf config, napi napisy, FONT* normfont, FONT* starfont);
void printonhud (BITMAP* hud, BITMAP* bufor, int xall, conf config, FONT* normfont, FONT* starfont,
               int ciag, int fuel, int fuelall, int y, int xsr, int kat_ob, int score);



napi tekst(char name[]);
void instrukcja(napi napisy, FONT* head, FONT* foot, FONT* norm, FONT* star, BITMAP* bufor, int c, int c2);

void zderzenia( int x, int y, int kat, int szerokosc, int wysokosc, int zwrot[], BITMAP * bufor);
void draw_map(BITMAP * bufor, BITMAP * mapa, int x, int y);
void grub(int x, int y, BITMAP * bufor);
void wygrana(BITMAP* bufor, char win[], FONT* star, FONT* norm);
void pauza(BITMAP* bufor, char pause[], FONT* star, FONT* norm);
void dzwiek_silnik (int ciag, SAMPLE * silnik);
void trafienie_g(int hit, BITMAP* bufor, int x, int y, int kat);
void rgb_to_bgr(BITMAP * bitmapa);
void animpipe(BITMAP * bitmapa);
void animinstrt(BITMAP * bitmapa);

void printmover(mover lista[], int x, int y, BITMAP * bufor, int z);
void refreshmover(mover lista[]);
void loadmover(mover lista[], int level);
void loadimgmover (mover lista[]);
void fillmover(mover lista[], int n);

int do_sha1_file(char *name, unsigned char *out);
int do_sha1_map_pack(unsigned char *out, int alllevel);

void highscore(char name[], napi napisy, int score, BITMAP * bufor, FONT * star, FONT * norm, FONT * czyt);
void viewscore(char name[], napi napisy, BITMAP * bufor, FONT * star, FONT * norm, FONT * czyt, int wyr);
int CreateIfNotExists( const char * sFileName );

conf readconf();
void saveconf(conf tempconf);
BITMAP* options_main(BITMAP* bufor, BITMAP* pipe, napi napisy, FONT * star, FONT * norm, FONT * czyt, conf tempconf, BITMAP* hud);

BITMAP* level(int level, int &x, int &y, int &grawitacja, int &fuel);
BITMAP* levelfront(int level);
BITMAP* levelback(int level);
int menu( napi tekst, FONT* norm, FONT* star, BITMAP* bufor, BITMAP* pipe, int xprze, int col);

int e_wybor_poz(BITMAP* bufor, FONT* czyt, int col, int maxlevel);
void e_loadmover(mover lista[], vector<e_mov> klatkilista[MOVNUM]);
void e_refreshmover(mover lista[MOVNUM], vector<e_mov> klatkilista[MOVNUM], int liczniki[MOVNUM]);
void e_savemover(mover lista[MOVNUM], vector<e_mov> klatkilista[MOVNUM]);

int main() {
    allegro_init();
	alpng_init();
    install_keyboard();
    install_sound( DIGI_AUTODETECT, MIDI_AUTODETECT, "" );
    BITMAP * bufor = NULL;
    bufor = tworzenie_okna_i_bufora(200, 150, 0);
    destroy_bitmap(bufor);
    conf config = readconf();

    set_volume( 255, 255 );
    bufor = tworzenie_okna_i_bufora(config.resx, config.resy, config.windowed);

    double przemieszczenie[2];
    int ciag = 10;
    int ciagsterowy = 30;
    int kat_b = 0;
    int kat_ob = 0;
    int x = 350;
    int y = 200;
    double xfiz = 350;
    double yfiz = 200;
    int komunikat = 0;
    int hitbox[11];
    int smierc = 0;
    int fuelall = 0;
    int fuel = 0;
    int grawitacja;
    int poziom;
    int score = 0;
    int dlognia = 0;

    mover movlist[MOVNUM]; fillmover(movlist, MOVNUM);
    BITMAP * ufo = load_png( "png/ufo.png", default_palette ); rgb_to_bgr(ufo);
    BITMAP * hud = load_png( "png/hud.png", default_palette ); rgb_to_bgr(hud);

    BITMAP * mapa = load_png( "png/map/1/map.png", default_palette ); rgb_to_bgr(mapa);
    BITMAP * mapafront = load_png( "png/map/1/mapfront.png", default_palette ); rgb_to_bgr(mapafront);
    BITMAP * mapaback = load_png( "png/map/1/mapback.png", default_palette ); rgb_to_bgr(mapaback);

    BITMAP * pipe = load_png("png/pipe.png", default_palette); rgb_to_bgr(pipe);

    BITMAP * ogien = load_png("png/ogien.png", default_palette); rgb_to_bgr(ogien);
    BITMAP * ogientlo = load_png("png/ogientlo.png", default_palette); rgb_to_bgr(ogientlo);

	BITMAP * wybuch = load_png( "png/wybuch.png", default_palette ); rgb_to_bgr(wybuch);
    FONT * normfont = load_font("font/normfont.pcx",default_palette, NULL);
    FONT * starfont = load_font("font/starfonts1.pcx",default_palette, NULL);
    FONT * czytfont = load_font("font/czytfont.pcx",default_palette, NULL);
//    textprintf_ex( screen, font, 10, 10,  makecol(64, 255, 64), 0, "przed" );
    SAMPLE * bummwav = load_sample("bum.wav");
	SAMPLE * silnik = load_sample("engine.wav");
    napi napisy;
    napisy = tekst(config.lang);

    unsigned char sha1map[20];
    char sha1text[256] = "";
    do_sha1_map_pack(sha1map, config.levele);
    for (int i = 0; i < 20; i++){
        sprintf(sha1text,"%s%02x",sha1text, sha1map[i]);
    }
//    allegro_message("%s", sha1text);
    int xsr = (bufor->w - ufo->h)/2; // środek bufora (dla statku)
    int xall = (bufor->w - 800)/2; // start pelno ekranowych bitmap (hud)

    hudcolor (config.colors, hud);

    int tik_per_ms;
    {/**< ustalenie ile tików procesora wystepuje w jednej milisekundzie */
    boost::chrono::high_resolution_clock::time_point start;
    boost::chrono::nanoseconds ns;
    start = boost::chrono::high_resolution_clock::now();
    rest(1000);
    ns = boost::chrono::high_resolution_clock::now() - start;
    tik_per_ms = ns.count()/1000.000;
    }
    while(!key[KEY_ESC] && komunikat != KOMQUIT){
         komunikat = menu(napisy, normfont, starfont, bufor, pipe, xall, config.colors[5]);
		 if(komunikat == KOMSCO){
            viewscore(sha1text, napisy, bufor, starfont, normfont, czytfont, -1);
         }
		 if(komunikat == KOMOPT){
            bufor = options_main(bufor, pipe, napisy, starfont, normfont, czytfont, config, hud);
            config = readconf();
            xsr = (bufor->w - ufo->h)/2; // środek bufora (dla statku)
            xall = (bufor->w - 800)/2; // start pelno ekranowych bitmap (hud)
            destroy_bitmap(hud);
            hud = load_png( "png/hud.png", default_palette ); rgb_to_bgr(hud);
            hudcolor (config.colors, hud);
         }
		 if(komunikat == KOMINST){
            instrukcja(napisy, normfont, normfont, czytfont, starfont, bufor, config.colors[5], config.colors[6]);
         }
         if(komunikat == KOMEDIT){
            /*
            -wczytac sciezke do poziomu
            -ladowanie mapy
            -wczytanie HUD edytora
            -strzalki przemieszczanie po mapie
            -WSAD kursor movera
            -QE obrot movera
            -wypisanie listy (lewej, prawej)
            - lista po lewej (U ^;J V) wybor movera
            - lista po prawej klatki movera
                -N nastepna klatak
                -M poprzednia klatka
                -B usun klatke
                -V dodaj klatke
                -ZX warstwa
                -1-0 wybor obrazka
            //-P wprowadzanie sciezki obrazkow
            */
            poziom = e_wybor_poz(bufor, czytfont, config.colors[5] , config.levele);
            vector<e_mov> e_framlist[MOVNUM];
            int liczniki[MOVNUM] = {};
            int e_wybMov = 0;
            int xedit, yedit;
            int edit1, edit2;
            destroy_bitmap( mapa );
   			destroy_bitmap( mapafront );
    		destroy_bitmap( mapaback );
			mapa = level(poziom, xedit, yedit, edit1, edit2);
            xedit -= xall;
            mapaback = levelback(poziom);
       		mapafront = levelfront(poziom);
   			loadmover(movlist, poziom);
            loadimgmover (movlist);
            e_loadmover(movlist, e_framlist);
            e_refreshmover(movlist, e_framlist, liczniki);
            int play = -1;
            for (int movdel = 0; movdel < MOVNUM; movdel++) {
                if (movlist[movdel].conf.is_open()) movlist[movdel].conf.close();///zamykam config movera bo nie jest juz potrzebny
            }// ewentalnie (przy zapisie) otworzyc do zapisu (wyswietlic monit czy cus) /// zrobione w funkcji e_savemover()
            while(!key[KEY_ESC]){
                if(key[KEY_LEFT]) x-=5;
                if(key[KEY_RIGHT]) x+=5;
                if(key[KEY_UP]){
                    play *= -1;
                    while(key[KEY_UP]) rest(10);
                }
                if(play==1) e_refreshmover(movlist, e_framlist, liczniki);
                else{
                    if(key[KEY_N]){
                        for(int i = 0; i<MOVNUM; i++)
                            liczniki[i] -= 2;
                        e_refreshmover(movlist, e_framlist, liczniki);
                        while(key[KEY_N]&&!key[KEY_LSHIFT]) rest(10);
                    }
                    if(key[KEY_M]){
                        e_refreshmover(movlist, e_framlist, liczniki);
                        while(key[KEY_M]&&!key[KEY_LSHIFT]) rest(10);
                    }
                    if(key[KEY_EQUALS]){
                        ///+jedna klatka
                        vector<e_mov>::iterator it;
                        it = e_framlist[e_wybMov].begin();
                        it += liczniki[e_wybMov];
                        e_framlist[e_wybMov].insert(it, e_framlist[e_wybMov][liczniki[e_wybMov]]);
                        //for(int i = 0; i<MOVNUM; i++)
                          //  --liczniki[i];
                        //++liczniki[e_wybMov];
                        e_refreshmover(movlist, e_framlist, liczniki);
                        while(key[KEY_EQUALS]&&!key[KEY_LSHIFT]) rest(10);
                    }
                    if(key[KEY_MINUS]){
                        ///-jedna klatka
                        vector<e_mov>::iterator it;
                        it = e_framlist[e_wybMov].begin();
                        it += liczniki[e_wybMov];
                        if(e_framlist[e_wybMov].size()>1){
                                e_framlist[e_wybMov].erase(it);
                        for(int i = 0; i<MOVNUM; i++)
                            ----liczniki[i];
                        e_refreshmover(movlist, e_framlist, liczniki);
                        }
                        while(key[KEY_MINUS]&&!key[KEY_LSHIFT]) rest(10);
                    }
                    ///Czy (klawisz[]wcisniety?){wykonaj akcje; dopoki (klawisz[]wcisniety && LSHIFT puszczony)czekaj na puszczenie[] lub wcisniecie modyfikatora LSHIFT}
                    if(key[KEY_W]) {--e_framlist[e_wybMov][liczniki[e_wybMov]].y; while(key[KEY_W]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_S]) {++e_framlist[e_wybMov][liczniki[e_wybMov]].y; while(key[KEY_S]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_A]) {--e_framlist[e_wybMov][liczniki[e_wybMov]].x; while(key[KEY_A]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_D]) {++e_framlist[e_wybMov][liczniki[e_wybMov]].x; while(key[KEY_D]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_Z]) {--e_framlist[e_wybMov][liczniki[e_wybMov]].z; while(key[KEY_Z]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_X]) {++e_framlist[e_wybMov][liczniki[e_wybMov]].z; while(key[KEY_X]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_E]) {e_framlist[e_wybMov][liczniki[e_wybMov]].kat = (e_framlist[e_wybMov][liczniki[e_wybMov]].kat+1)%256; while(key[KEY_E]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_Q]) {e_framlist[e_wybMov][liczniki[e_wybMov]].kat = (e_framlist[e_wybMov][liczniki[e_wybMov]].kat-1)%256;if(e_framlist[e_wybMov][liczniki[e_wybMov]].kat<0)e_framlist[e_wybMov][liczniki[e_wybMov]].kat+=256; while(key[KEY_Q]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_R]) {e_framlist[e_wybMov][liczniki[e_wybMov]].numframe = (e_framlist[e_wybMov][liczniki[e_wybMov]].numframe+1)%10; while(key[KEY_R]&&!key[KEY_LSHIFT]) rest(10);}
                    if(key[KEY_F]) {e_framlist[e_wybMov][liczniki[e_wybMov]].numframe = (e_framlist[e_wybMov][liczniki[e_wybMov]].numframe-1)%10;if(e_framlist[e_wybMov][liczniki[e_wybMov]].numframe<0)e_framlist[e_wybMov][liczniki[e_wybMov]].numframe+=10; while(key[KEY_F]&&!key[KEY_LSHIFT]) rest(10);}

                    for(int i = 0; i<MOVNUM; i++) /// e_refreshmover() przesuwa liczniki o 1, a ja chce zeby pozostaly w tym samym miejscu
                        --liczniki[i];
                    e_refreshmover(movlist, e_framlist, liczniki);
                }
                if(key[KEY_U]){
                    --e_wybMov;
                    if(e_wybMov < 0) e_wybMov = 0;
                    while(key[KEY_U]&&!key[KEY_LSHIFT]) rest(10);
                }
                if(key[KEY_J]){
                    ++e_wybMov;
                    if(e_wybMov >= MOVNUM) e_wybMov = MOVNUM-1;
                    else if(movlist[e_wybMov].istnieje == false) --e_wybMov;
                    while(key[KEY_J]&&!key[KEY_LSHIFT]) rest(10);
                }
                if(key[KEY_P]){
                    e_savemover(movlist, e_framlist);
                    while(key[KEY_P]) rest(10);
                }
                printmover(movlist, x,0, bufor, 1);
                draw_map(bufor, mapaback, x, 0);
                printmover(movlist, x,0, bufor, 2);
                draw_map(bufor, mapa, x, 0);
                printmover(movlist, x,0, bufor, 3);
                printmover(movlist, x,0, bufor, 4);
                draw_map(bufor, mapafront, x, 0);
                printmover(movlist, x,0, bufor, 5);
                ///wypisze listy:

                //textprintf_ex( screen, font, bufor->w -100, 20,  makecol(64, 255, 64), 0, "naglowek lini: %d", line);
                {///prawa lista
                    int i;
                    textprintf_ex( bufor, font, bufor->w-250, 10,  makecol(64, 255, 64), 0, "   nr:    x;    y; z;kat;img");
                    for(int j = 0; j<20;++j){
                        i = liczniki[e_wybMov]-10+j;
                        if(i < 0){
                            i += e_framlist[e_wybMov].size();
                        }
                        if(i >= e_framlist[e_wybMov].size()){
                            i %= e_framlist[e_wybMov].size();
                        }
                        textprintf_ex( bufor, font, bufor->w-250, 30+j*10,  makecol(64, 255, 64), 0, "%5d:% 5d % 5d % 2d %3d %3d", 10-j,e_framlist[e_wybMov][i].x,e_framlist[e_wybMov][i].y,e_framlist[e_wybMov][i].z,e_framlist[e_wybMov][i].kat,e_framlist[e_wybMov][i].numframe);
                    }
                }

                {///lewa lista
                int i;
                for(i = 0; i<MOVNUM && movlist[i].istnieje; ++i)
                    if(i == e_wybMov) textprintf_ex( bufor, font, 10, 20+i*10,  makecol(64, 255, 64), 0, "mover: %d<", i);
                    else textprintf_ex( bufor, font, 10, 20+i*10,  makecol(64, 255, 64), 0, "mover: %d ", i);
                textprintf_ex( bufor, font, 10, 20+i*10,  makecol(64, 255, 64), 0, "U-poprzedni; J-nastepny");
                }
                textprintf_ex( bufor, font, 10, bufor->h-20,  makecol(64, 255, 64), 0, "Q\\E-kat; WSAD-x\\y; Z\\X-z; R\\F-img; -\\+ -frame; M\\N-przewijanie; LSHIFT - szybko; A_UP-odtworz; P-zapisz");

                stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                            0, 0, SCREEN_W, SCREEN_H);
                clear_to_color( bufor, makecol( 0, 0, 0 ));
                rest(10);
            }
            komunikat = KOMQUIT;
         }
         if(komunikat == KOMPLAY){
            BITMAP* bufor_fiz = create_bitmap(ufo->w*2,ufo->h*2); /**< bufor uzywany do obliczania zderzen statku */
            clear_to_color( bufor_fiz, makecol( 0, 0, 0 ));
            generatehud (hud, config, napisy, normfont, starfont);/**< generuje napisy na hud */
            poziom = 1;
            destroy_bitmap( mapa );
   			destroy_bitmap( mapafront );
    		destroy_bitmap( mapaback );
			mapa = level(poziom, x, y, ciag, fuelall);
            x -= xall;
            mapaback = levelback(poziom);
       		mapafront = levelfront(poziom);
   			loadmover(movlist, poziom);
            loadimgmover (movlist);
			ciagsterowy = ciag * 3;
            kat_b = 0;
            kat_ob = 0;
            smierc = 0;
            grawitacja = ciag;
            fuel = fuelall;
            score = 0;
            play_sample( silnik, 255, 127, 1000, 1 );
            przemieszczenie[0] = 0;
            przemieszczenie[1] = 0;
            xfiz = x;
            yfiz = y;
            while (!key[KEY_ESC] && !key[KEY_Q] && smierc <= 40) {
                boost::chrono::high_resolution_clock::time_point start = boost::chrono::high_resolution_clock::now();
        		sterowanie( ciagsterowy, kat_b );
        		if( kat_b > 360 ) kat_b = 0; // dwie poni¿sze linijki normalizuj¹ wartoœæ obortu statku do jednego pe³nego k¹ta (unikniêcie sytuacj przekrêcenia statku np do k¹tu 720 lub -40)
                if( kat_b < 0 ) kat_b = 360;
                if(fuel < 0) {
                        ciagsterowy = 0;
                        if ( ((grawitacja > 0 && y > 500) || (grawitacja < 0 && y < 100) || grawitacja == 0) && smierc == 0) smierc = 1;
                }
                ciag = ciagsterowy/3; // ciag sterowy ma rozdzielczosc wieksza 3 razy, aby sapowolniæ manewr przyspieszania\zwalniania statku
                if(ciag < MINSPEED ){// dwa if'y zawê¿aj¹ zakres ci¹gu
                         ciag = MINSPEED;
                         ciagsterowy = MINSPEED * 3;
                }
                if(ciag > MAXSPEED ){
                        ciag = MAXSPEED;
                        ciagsterowy = MAXSPEED * 3;
                }
                fuel -= ciag;
                dzwiek_silnik(ciag, silnik);
        		obliczanie_sil( ciag, kat_b, grawitacja, przemieszczenie);
         		xfiz += przemieszczenie[0]; // prze³o¿enie przemieszczenia na wspó³rzêdne + dzielnik do obni¿ania prêdkoœci
        		yfiz -= przemieszczenie[1];
                x = xfiz;
                y = yfiz;
                kat_ob = kat_b*255/360; // k¹t obrotu statku w allegro jest z zakresu 0-255 a nie 0-360

        		refreshmover(movlist);

				printmover(movlist, x+xsr- bufor_fiz->w/4,y- bufor_fiz->h/4, bufor_fiz, 2); /**< mape movery rysuje w malym buforze fizycznym */
				draw_map(bufor_fiz, mapa, x+xsr- bufor_fiz->w/4, y- bufor_fiz->h/4);//mapa przed zderzeniami!!!
                printmover(movlist, x+xsr- bufor_fiz->w/4,y- bufor_fiz->h/4, bufor_fiz, 3);
				zderzenia( ufo->w/2, ufo->h/2, kat_b, ufo->w, ufo->h, hitbox, bufor_fiz);

                blit(ogientlo, ufo, 0, 0, 36, 76, ogientlo->w, ogientlo->h);

                for (int pent = 0; pent < 3 && smierc == 0; ++pent){
                    if(hitbox[pent] > 0) floodfill(ufo, 24 *(1+pent), 90, makecol(15,255,9));
                    else floodfill(ufo, 24 *(1+pent), 90, makecol(127,127,127));
                }
                for (int pent = 3; pent < 6; ++pent){
                    if(hitbox[pent] > 0 || smierc > 0) floodfill(ufo, 24 *(pent - 2), 90, makecol(232,35,0));
                }

                //rysowanie ognia na statku
                dlognia = ogien->h * ciag/MAXSPEED;
                masked_blit(ogien, ufo, 0, ogien->h - dlognia, 36, 76, ogien->w, dlognia);

				printmover(movlist, x,0, bufor, 1);
				draw_map(bufor, mapaback, x, 0);
				printmover(movlist, x,0, bufor, 2);
                draw_map(bufor, mapa, x, 0);
                printmover(movlist, x,0, bufor, 3);

				if( smierc == 0 && hitbox[3] == 0 && hitbox[4] == 0 && hitbox[5] == 0 && hitbox[6] == 0 &&
                                   hitbox[7] == 0 && hitbox[8] == 0 && hitbox[9] == 0 && hitbox[10] == 0)
                            rotate_sprite( bufor, ufo, xsr, y, itofix( kat_ob ));  // rysowanie statku w buforze

				for (int pent = 6; pent < 11 && smierc == 0; ++pent){
					if( hitbox[pent] > 0 ) trafienie_g(pent, bufor, xsr, y, kat_ob);
				}

				printmover(movlist, x,0, bufor, 4);
				draw_map(bufor, mapafront, x, 0);
				printmover(movlist, x,0, bufor, 5);

                printonhud (hud, bufor, xall, config, normfont, starfont, ciag, fuel, fuelall, y, xsr, kat_ob, score);

				if( smierc > 0 || hitbox[3] > 0 || hitbox[4] > 0 || hitbox[5] > 0 || hitbox[6] > 0 || hitbox[7] > 0 || hitbox[8] > 0 || hitbox[9] > 0 || hitbox[10] > 0){
                    stop_sample( silnik );
                    if (smierc == 1) play_sample( bummwav, 255, 127, 1000, 0 );
					smierc++;
                    x -= przemieszczenie[0]/1.2; // prze³o¿enie przemieszczenia na wspó³rzêdne + dzielnik do obni¿ania prêdkoœci
        		    y += przemieszczenie[1]/1.2;
        		    if (smierc > 40){
		   			   //stop_sample(bummwav);
					   grub(xsr, y, bufor);
					   highscore(sha1text, napisy, score, bufor, starfont, normfont, czytfont);
	   				} else if(smierc < 25){
					  	    rotate_sprite( bufor, ufo, xsr, y, itofix( kat_ob ));  // rysowanie statku w buforze
					  	    for (int pent = 6; pent < 11; ++pent){
								if( hitbox[pent] > 0 ) {
									trafienie_g( pent, bufor, xsr, y, kat_ob);
								}
							}
					}
					if ((smierc / 3) <= 12) masked_blit( wybuch, bufor, smierc/3*100, 0, xsr, y, 100, 100 );

				}
                else if ( hitbox[0] > 0 && hitbox[1] > 0 && hitbox[2] > 0 &&
                         przemieszczenie[1] <= 1 && przemieszczenie[1] >= -1 &&
                         przemieszczenie[0] <= 1 && przemieszczenie[0] >= -1
                         && y >= -70 && y <= 590){
                    stop_sample( silnik );
                    wygrana(bufor, napisy.win, starfont, normfont);
                    score += fuel;
                    poziom++;
                    if(poziom > config.levele){
                            poziom--;
					}
	                destroy_bitmap( mapa );
    				destroy_bitmap( mapafront );
    				destroy_bitmap( mapaback );
					mapaback = levelback(poziom);
					mapa = level(poziom, x, y, ciag, fuelall);
                    x -= xall;
                    mapafront = levelfront(poziom);
					loadmover(movlist, poziom);
                    loadimgmover (movlist);

                    fuel = fuelall;
					ciagsterowy = ciag * 3;
                    kat_b = 0;
                    kat_ob = 0;
                    grawitacja = ciag;
                    play_sample( silnik, 255, 127, 1000, 1 );
                    przemieszczenie[0] = 0;
                    przemieszczenie[1] = 0;
                    xfiz = x;
                    yfiz = y;
                }
                if (key[KEY_P]){
                   stop_sample( silnik );
                   pauza( bufor, napisy.pause, starfont, normfont );
                   play_sample( silnik, 255, 127, 1000, 1 );
                }
			    //blit( bufor, screen, 0, 0, 0, 0, 800, 600 );

                stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                            0, 0, SCREEN_W, SCREEN_H);

                /*rotate_sprite( bufor_fiz, ufo, 50, 50, itofix( kat_ob ));
                blit( bufor_fiz, screen, 0, 0, 0, 0, bufor_fiz->w, bufor_fiz->h );*/
//                 textprintf_ex( screen, font, 10, 20,  makecol(64, 255, 64), 0, "fuel: %d; all: %d", fuel, fuelall);
				clear_to_color( bufor_fiz, makecol( 0, 0, 0 ));
				clear_to_color( bufor, makecol( 0, 0, 0 ));
                boost::chrono::nanoseconds ns = boost::chrono::high_resolution_clock::now() - start;
                int resttime = 10 - ns.count()/tik_per_ms;
                textprintf_ex( screen, font, 10, 20,  makecol(64, 255, 64), 0, "time: %d", (resttime-10) *(-1));
                if(resttime > 0)
                    rest(resttime);

        	}
        	while (key[KEY_Q]){
                  rest(10);
            }
            destroy_bitmap(bufor_fiz);
            stop_sample( silnik );
			stop_sample(bummwav);
         }
   	     rest(100);
    }

    while(key[KEY_ESC]) rest(50);
	readkey();
	for (int movdel = 0; movdel < MOVNUM; movdel++) {
		if (movlist[movdel].conf.is_open()) movlist[movdel].conf.close();
		for (int movdelframe = 0; movdelframe < MOVFRAME; movdelframe++){
			if (movlist[movdel].frame[movdelframe] != NULL) destroy_bitmap( movlist[movdel].frame[movdelframe] );
		}
	}
    destroy_bitmap( bufor );
    destroy_bitmap( hud );
    destroy_bitmap( ufo );
    destroy_bitmap( mapa );
    destroy_bitmap( mapafront );
    destroy_bitmap( mapaback );
	destroy_bitmap( wybuch );
    destroy_bitmap( pipe );
    destroy_bitmap( ogien );
    destroy_bitmap( ogientlo );
    destroy_font(normfont);
    destroy_font(starfont);
    destroy_font(czytfont);
    destroy_sample( silnik );
    destroy_sample( bummwav );
	allegro_exit();
	return 0;
}
END_OF_MAIN()



BITMAP * tworzenie_okna_i_bufora (int szerokosc, int wysokosc, int windowed){
    //if (szerokosc < 800) szerokosc = 800;
    int depth;
    if ((depth = desktop_color_depth()) != 0) set_color_depth( depth );
    else set_color_depth( 32 ); // ilosc kolorów
    float wys600 = (float)wysokosc/600.00000;
    int szerbuf;
    if (wys600 == 0) szerbuf = szerokosc;
    else szerbuf = szerokosc/wys600;
    int wysbuf = 600;
    if(set_gfx_mode( windowed, szerokosc, wysokosc, 0, 0 )){
        if(set_gfx_mode( GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0 ))
            set_gfx_mode( GFX_AUTODETECT_WINDOWED, 400, 300, 0, 0 );//tworzenie okna
        szerbuf = 800;
    }
    set_palette( default_palette );
    clear_to_color( screen, makecol( 0, 0, 0 ));//czyszczenie okna
    if (szerbuf < 800) szerbuf = 800;
    return create_bitmap( szerbuf, wysbuf );//zwrot pointera do bufora
}

void obliczanie_sil (int ciag, int kat_a, int przyciaganie, double zwrot[]){
     // funkcja oblicza si³y dzia³aj¹ce na ufo; funkcja pobiera jako orgument tablicê globaln¹ czyli zbiór wskaŸników do zmiennych typu int, na których bospoœrednio operuje. W tych zmiennych nale¿y szukaæ zwróconej wartoœci.
     double horyzontalnie;
     double wertykalnie;
     double f = 0.8; //opor
     // dodac opory
     if(zwrot[0]>0) horyzontalnie = sin(kat_a * PI/180) * ciag -zwrot[0]*zwrot[0]*f;
     else horyzontalnie = sin(kat_a * PI/180) * ciag +zwrot[0]*zwrot[0]*f;
     if(zwrot[1]>0) wertykalnie = (cos(kat_a * PI/180) * ciag) - przyciaganie -zwrot[1]*zwrot[1]*f;
     else wertykalnie = (cos(kat_a * PI/180) * ciag) - przyciaganie +zwrot[1]*zwrot[1]*f;
     zwrot[0] += horyzontalnie*0.002;// x+ prawo x- lewo
     zwrot[1] += wertykalnie*0.002 ; // y+ dol y- gora
     textprintf_ex( screen, font, 10, 40,  makecol(64, 255, 64), 0, "x: %f; y: %f", zwrot[0], zwrot[1]);

}

void sterowanie(int &ciag, int &obrot){ // operuje bezpoœrednio na zmiennych okreslajacych ciag i obrot(kat obrotu)
    if( key[ KEY_LEFT ] ) ----obrot;
    if( key[ KEY_RIGHT ] ) ++++obrot;
    if( key[ KEY_UP ] ) ciag++;
    if( key[ KEY_DOWN ] ) ciag--;
    if( key[ KEY_1 ] ) ciag = 0;
    if( key[ KEY_2 ] ) ciag = 30;
    if( key[ KEY_3 ] ) ciag = 60;
    if( key[ KEY_4 ] ) ciag = 90;
}

void wskaznik (int x, int y, BITMAP * bufor, int col){ // wskazuje gdzie jest statek po opuszczeniu ekranu
	 if( x < 0 || y < 0 || x > bufor->w || y > bufor->h){
	 	 int texty = y;
	 	 texty *= -1;
         if( x < 0 ) x = 0;
         if( y < 0 ) y = 0;
         if( x > bufor->w ) x = bufor->w -20;
         if( y > bufor->h ) { texty += bufor->h; y = bufor->h-20; }
         texty /= 10;
		 rectfill( bufor, x+40, y, x+60, y+20, col);
         textprintf_ex( bufor, font, x+65, y+5, col, -1, "%d", texty );
     }
}

void odwrocenie ( int kat_ob, BITMAP * bufor, int xprze, int color){ // pokazuje ustawienie dzioba statku
     kat_ob = (kat_ob - 255) * (-1);
     kat_ob += 255/4;
     arc(bufor, 750+xprze, 50, itofix(kat_ob - 5), itofix(kat_ob + 5), 30, color);
}

void predkosc ( int ciag, BITMAP *bufor, int xprze, int color){ // prezêtuje prêdkoœæ
        int xdest = ciag *10;
        switch (xdest){
            case 0: xdest += 2; break;
            default: ;
        }
        rectfill(bufor, 2+xprze, 2, xprze + xdest, 17, color);
//        textprintf( bufor, font, 0, 0, makecol( 255, 128, 128 ), "speed");
}

void hudfuel(int fuel, int fuelall, BITMAP* bufor, int xprze, int color){
    int stan = (fuelall-fuel)*270/fuelall - 45;
    const int r = 50;
    const int xo = 450+xprze;
    const int yo = 50;
    const int xf = 415+xprze;
    const int yf = 85;
    if (fuel < 0) stan = 270 -45;
    if (stan < 0) stan += 360;
    int x = xo + r * cos(stan * -PI/180);
    int y = yo + r * sin(stan * -PI/180);
    line(bufor, xo, yo, x, y, color);
    floodfill(bufor, xf, yf, color);
}

void hudcolor (int colors[CONFCOL], BITMAP* hud){
    floodfill(hud, 100, 50, colors[4]);
    floodfill(hud, 318, 10, colors[4]);
    floodfill(hud, 790, 10, colors[4]);
    floodfill(hud, 790, 90, colors[4]);
    floodfill(hud, 10, 580, colors[4]);
    floodfill(hud, 720, 580, colors[4]);
    floodfill(hud, 300, 0, colors[0]);

    for (int i = 5; i <= 295; i += 10)
        floodfill(hud, i, 19, colors[0]);

    floodfill(hud, 450, 2, colors[1]);
    floodfill(hud, 750, 2, colors[2]);
}

void generatehud (BITMAP* hud, conf config, napi napisy, FONT* normfont, FONT* starfont){
				textprintf_ex( hud, starfont, 505, 40, config.colors[1], -1, "%s", napisy.fuel);
				textprintf_right_ex( hud, normfont, 395, 40, config.colors[1], -1, "%s", napisy.fuel);
				textprintf_ex( hud, starfont, 5, 40, config.colors[0], -1, "%s", napisy.power);
				textprintf_ex( hud, normfont, 5, 70, config.colors[0], -1, "%s", napisy.power);
				textprintf_right_ex( hud, starfont, 705, 5, config.colors[2], -1, "%s", napisy.angle);
				textprintf_right_ex( hud, normfont, 705, 70, config.colors[2], -1, "%s", napisy.angle);
                textprintf_ex( hud, normfont, 5, 540, config.colors[3], -1, "%s", napisy.scor);
				textprintf_right_ex( hud, starfont, 795, 540, config.colors[3], -1, "%s", napisy.scor);
}

void printonhud(BITMAP* hud, BITMAP* bufor, int xall, conf config, FONT* normfont, FONT* starfont,
               int ciag, int fuel, int fuelall, int y, int xsr, int kat_ob, int score){

				masked_blit( hud, bufor, 0, 0, xall, 0, 800, 600 );
				textprintf_ex( bufor, normfont, 5+xall, 570, config.colors[3], -1, "%d+%d",score,fuel);
				textprintf_right_ex( bufor, starfont, 795+xall, 570, config.colors[3], -1, "%d+%d",fuel,score);

                predkosc ( ciag, bufor, xall, config.colors[0]);
                hudfuel(fuel, fuelall, bufor, xall, config.colors[7]);
        		wskaznik ( xsr, y, bufor, config.colors[8]);
        		odwrocenie ( kat_ob, bufor, xall, config.colors[2]);

}

void zderzenia( int x, int y, int kat, int szerokosc, int wysokosc, int zwrot[], BITMAP * bufor){
    x += szerokosc/2;
    y += wysokosc/2;
    int i;
    int color;
/*    int hitbox[][2] = {
        { -26, -43 }, 50
        { 0, -43 }, 43
        { 26, -43 }, 50
        { -26, -30 }, 40
        { 0, -30 }, 30
        { 26, -30 }, 40
        { -30, 0 }, 30
        { 30, 0 }, 30
        { 51, 0}, 51
        { -22 , 30}, 37
        { 22 , 30} 37
    };*/
    int hitbox[11][2] = {
        { sin((kat + 210 - 180) * -PI/180) * 50, cos((kat + 210 - 180) * -PI/180) * 50 }, // kolo 1 dol
        { sin((kat - 360) * -PI/180) * 44, cos((kat - 360) * -PI/180) * 44 }, // kolo 2 dol
        { sin((kat + 150 - 180) * -PI/180) * 50, cos((kat + 150 - 180) * -PI/180) * 50 }, // kolo 3 dol
        { sin((kat + 220 - 180) * -PI/180) * 40, cos((kat + 220 - 180) * -PI/180) * 40 }, // kolo 1 gora
        { sin((kat - 360) * -PI/180) * 31, cos((kat - 360) * -PI/180) * 31 }, // kolo 2 gora
        { sin((kat + 140 - 180) * -PI/180) * 40, cos((kat + 140 - 180) * -PI/180) * 40 }, // kolo 3 gora
        { sin((kat + 270) * -PI/180) * 30, cos((kat + 270) * -PI/180) * 30 }, // prawy bok dol
        { sin((kat + 90) * -PI/180) * 30, cos((kat + 90) * -PI/180) * 30 }, // lewy bok dol
        { sin((kat - 180) * -PI/180) * 51, cos((kat - 180) * -PI/180) * 51}, // czub
        { sin((kat + 324 - 180) * -PI/180) * 37, cos((kat + 324 - 180) * -PI/180) * 37}, // prawy bok gora
        { sin((kat + 36 - 180) * -PI/180) * 37, cos((kat + 36 - 180) * -PI/180) * 37} // lewy bok gora
    };
    for( i = 0; i < 11; ++i ){
         hitbox[i][0] += x;
         hitbox[i][1] += y;
    }
    for( i = 0; i < 11; ++i ){
//        textprintf_ex( bufor, font, 10, 100 + 10 * i,  makecol(225, 225, 20 * i), 0, "%d:  %d", i, getpixel(bufor, hitbox[i][0], hitbox[i][1]));
//        putpixel(bufor, hitbox[i][0], hitbox[i][1], makecol(225, 255, 0));
        color = getpixel(bufor, hitbox[i][0], hitbox[i][1]);
        if(color == makecol( 106, 106, 106 )) color = 2;
        else if(color < 0) color = 0;
        else if(color > 0) color = 1;
        zwrot[i] = color;
    }
}

napi tekst(char name[]){
     fstream ftekst;
     ftekst.open(name, ios::in);
     const int koszl = 1024;
     int exit = 0;
     char naglowek;
     char kosz[koszl];
     int line = 0;
     napi tekst;
     if(ftekst.is_open() == true) textprintf_ex( screen, font, 10, 10,  makecol(64, 255, 64), 0, "otwarty" );
     while(exit != 1 && ftekst.is_open() && !key[KEY_ESC]){
           if( ftekst.fail() == true ) exit = 1;
           if( exit != 1) ftekst.get(naglowek);
           else naglowek = '_';
           textprintf_ex( screen, font, 10, 20,  makecol(64, 255, 64), 0, "naglowek lini: %d", line);
           if( naglowek == 'k' ) ftekst.getline(kosz,koszl);
           else if ( naglowek != '_' ) {
                while ( (naglowek != ':') && !key[KEY_ESC]){
                      ftekst.get(naglowek);
                }
                switch (line){
                case 0: ftekst.getline(tekst.nazwa, NAZWAL); break;
                case 1: ftekst.getline(tekst.mgra, MGRAL); break;
                case 2: ftekst.getline(tekst.mins, MINSL); break;
                case 3: ftekst.getline(tekst.mopt, MOPTL); break;
                case 4: ftekst.getline(tekst.mkon, MKONL); break;
                case 5: ftekst.getline(tekst.msco, MSCOL); break;
                case 6: ftekst.getline(tekst.instr1_1, INSTRLINEL); break;
                case 7: ftekst.getline(tekst.instr1_2, INSTRLINEL); break;
                case 8: ftekst.getline(tekst.instr1_3, INSTRLINEL); break;
                case 9: ftekst.getline(tekst.instr1_4, INSTRLINEL); break;
                case 10: ftekst.getline(tekst.instr1_5, INSTRLINEL); break;
                case 11: ftekst.getline(tekst.instr1_6, INSTRLINEL); break;
                case 12: ftekst.getline(tekst.instr1_7, INSTRLINEL); break;
                case 13: ftekst.getline(tekst.instr2_1, INSTRLINEL); break;
                case 14: ftekst.getline(tekst.instr2_2, INSTRLINEL); break;
                case 15: ftekst.getline(tekst.instr2_3, INSTRLINEL); break;
                case 16: ftekst.getline(tekst.instr2_4, INSTRLINEL); break;
                case 17: ftekst.getline(tekst.instr2_5, INSTRLINEL); break;
                case 18: ftekst.getline(tekst.instr2_6, INSTRLINEL); break;
                case 19: ftekst.getline(tekst.instr2_7, INSTRLINEL); break;
                case 20: ftekst.getline(tekst.instr3_1, INSTRLINEL); break;
                case 21: ftekst.getline(tekst.instr3_2, INSTRLINEL); break;
                case 22: ftekst.getline(tekst.instr3_3, INSTRLINEL); break;
                case 23: ftekst.getline(tekst.instr3_4, INSTRLINEL); break;
                case 24: ftekst.getline(tekst.instr3_5, INSTRLINEL); break;
                case 25: ftekst.getline(tekst.instr3_6, INSTRLINEL); break;
                case 26: ftekst.getline(tekst.instr3_7, INSTRLINEL); break;
                case 27: ftekst.getline(tekst.instr4_1, INSTRLINEL); break;
                case 28: ftekst.getline(tekst.instr4_2, INSTRLINEL); break;
                case 29: ftekst.getline(tekst.instr4_3, INSTRLINEL); break;
                case 30: ftekst.getline(tekst.instr4_4, INSTRLINEL); break;
                case 31: ftekst.getline(tekst.instr4_5, INSTRLINEL); break;
                case 32: ftekst.getline(tekst.instr4_6, INSTRLINEL); break;
                case 33: ftekst.getline(tekst.instr4_7, INSTRLINEL); break;
                case 34: ftekst.getline(tekst.instr1_n, INSTRLINEL); break;
                case 35: ftekst.getline(tekst.instr1_s, INSTRLINEL); break;
                case 36: ftekst.getline(tekst.instr2_n, INSTRLINEL); break;
                case 37: ftekst.getline(tekst.instr2_s, INSTRLINEL); break;
                case 38: ftekst.getline(tekst.instr3_n, INSTRLINEL); break;
                case 39: ftekst.getline(tekst.instr3_s, INSTRLINEL); break;
                case 40: ftekst.getline(tekst.instr4_n, INSTRLINEL); break;
                case 41: ftekst.getline(tekst.instr4_s, INSTRLINEL); break;
                case 42: ftekst.getline(tekst.power, POWERL); break;
                case 43: ftekst.getline(tekst.angle, ANGLEL); break;
                case 44: ftekst.getline(tekst.win, WINL); break;
                case 45: ftekst.getline(tekst.pause, PAUSEL); break;
                case 46: ftekst.getline(tekst.fuel, FUELL); break;
                case 47: ftekst.getline(tekst.scor, SCORL); break;
                case 48: ftekst.getline(tekst.hsnw1, HSNW1L); break;
                case 49: ftekst.getline(tekst.hsnw2, HSNW2L); break;
                case 50: ftekst.getline(tekst.hsname, HSNAMEL); break;
                case 51: ftekst.getline(tekst.hsnws1, HSNWS1L); break;
                case 52: ftekst.getline(tekst.hsnws2, HSNWS2L); break;
                case 53: ftekst.getline(tekst.hsvn, HSVNL); break;
                case 54: ftekst.getline(tekst.hsvs, HSVSL); break;
                case 55:
                case 56:
                case 57:
                case 58:
                case 59:
                case 60:
                case 61:
                case 62:
                case 63:
                case 64:
                case 65:
                case 67:
                case 68:
                case 69:
                case 70: ftekst.getline(tekst.opt0[line-55], OPTLINEL); break;
                case 71:
                case 72:
                case 73:
                case 74:
                case 75:
                case 76:
                case 77:
                case 78:
                case 79:
                case 80:
                case 81:
                case 82:
                case 83:
                case 84:
                case 85:
                case 86: ftekst.getline(tekst.opt_roz[line-71], OPTLINEL); break;
                case 87:
                case 88:
                case 89:
                case 90:
                case 91:
                case 92:
                case 93:
                case 94:
                case 95:
                case 96:
                case 97:
                case 98:
                case 99:
                case 100:
                case 101:
                case 102: ftekst.getline(tekst.opt_col[line-87], OPTLINEL); break;
                default: ftekst.getline(kosz,koszl); break;
                }
                line++;
           }
     }
     ftekst.close();
     return tekst;
}

int menu(napi tekst, FONT* norm, FONT* star, BITMAP* bufor, BITMAP* pipe, int xprze, int col){
    int kursor = 1;
    int lastg = 0;
    int lastd = 0;
    int color1;
    int color2;
    int color3;
    int color4;
    int color5;
    int color6;
    int colorw, wr, wb, wg;
    wr = 16;
    wg = 200;
    wb = 16;

	while(1){
        animpipe(pipe);
        //masked_blit(pipe, bufor, 0, 0, xprze, 0, pipe->w, pipe->h);
        masked_stretch_blit(pipe, bufor, 0, 0,
                            pipe->w, pipe->h,
                            0, 0, bufor->w, bufor->h);
        color1 = col;
        color2 = col;
        color3 = col;
        color4 = col;
        color5 = col;
        color6 = col;
        if(wr >= 16 && wr < 112 && wb == 16) {wr+=2; wg-=4;}
        else if(wb >= 16 && wb < 112 && wr == 112) {wb+=2; wg+=4;}
        else if(wr > 16 && wr <= 112 && wb == 112) {wr-=2; wg-=4;}
        else if(wb > 16 && wb <= 112 && wr == 16) {wb-=2; wg+=4;}
        colorw = makecol(wr,wg,wb);
        switch(kursor){
        case 1: color2 = colorw;break;
        case 2: color3 = colorw;break;
        case 3: color4 = colorw;break;
        case 4: color5 = colorw;break;
        case 5: color6 = colorw;break;
        }
        textprintf_centre_ex( bufor, star, bufor->w / 2, 10, color1, -1, "%s", tekst.nazwa);
        textprintf_centre_ex( bufor, star, bufor->w / 4, 210, color2, -1, "%s", tekst.mgra);
        textprintf_centre_ex( bufor, star, bufor->w / 4, 210+75*1, color3, -1, "%s", tekst.mins);
        textprintf_centre_ex( bufor, star, bufor->w / 4, 210+75*2, color4, -1, "%s", tekst.msco);
        textprintf_centre_ex( bufor, star, bufor->w / 4, 210+75*3, color5, -1, "%s", tekst.mopt);
        textprintf_centre_ex( bufor, star, bufor->w / 4, 210+75*4, color6, -1, "%s", tekst.mkon);
        textprintf_centre_ex( bufor, norm, bufor->w / 2, 110, color1, -1, "%s", tekst.nazwa);
        textprintf_centre_ex( bufor, norm, bufor->w / 2 + bufor->w / 4, 210, color2, -1, "%s", tekst.mgra);
        textprintf_centre_ex( bufor, norm, bufor->w / 2 + bufor->w / 4, 210+75*1, color3, -1, "%s", tekst.mins);
        textprintf_centre_ex( bufor, norm, bufor->w / 2 + bufor->w / 4, 210+75*2, color4, -1, "%s", tekst.msco);
        textprintf_centre_ex( bufor, norm, bufor->w / 2 + bufor->w / 4, 210+75*3, color5, -1, "%s", tekst.mopt);
        textprintf_centre_ex( bufor, norm, bufor->w / 2 + bufor->w / 4, 210+75*4, color6, -1, "%s", tekst.mkon);
        if( key[ KEY_UP ] ){
             if (lastg == 0){
                kursor--;
             }
             lastg++;
             lastd = 0;
        }
        if( key[ KEY_DOWN ] ){
             if (lastd == 0){
                kursor++;
             }
             lastd++;
             lastg = 0;
        }
        if ((lastg > 25 || lastd > 25 )|| (!key[ KEY_DOWN ] && !key[ KEY_UP ])){
           lastg = 0;
           lastd = 0;
        }
        if( kursor == 0 ) kursor = 5;
        if( kursor == 6 ) kursor = 1;
        rectfill(bufor, bufor->w/2-10, kursor * 75 + 135, bufor->w/2+10, kursor * 75 + 155, colorw);
        triangle(bufor, bufor->w/2-20, kursor * 75 + 145, bufor->w/2-10, kursor * 75 + 135, bufor->w/2-10, kursor * 75 + 155, colorw);
        triangle(bufor, bufor->w/2+20, kursor * 75 + 145, bufor->w/2+10, kursor * 75 + 135, bufor->w/2+10, kursor * 75 + 155, colorw);
        //blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
        stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                    0, 0, SCREEN_W, SCREEN_H);
        clear_to_color( bufor, makecol( 0, 0, 0 ));
        if( key[ KEY_ENTER ] ){
            while(key[ KEY_ENTER ])rest(50);
             return kursor;
        }
        if( key[ KEY_SPACE ] ){
            while(key[ KEY_SPACE ])rest(50);
            return kursor;
        }
        if( key[ KEY_ESC ] ) return KOMQUIT;
        if( key[KEY_E] ){
            while(key[ KEY_E ])rest(50);
            return KOMEDIT;
        }
        rest(10);
    }
}

void instrukcja(napi napisy, FONT* head, FONT* foot, FONT* norm, FONT* star, BITMAP* bufor, int c, int c2){
     int haline = 30;
     int strona = 1;
     const int lmarg = bufor->w/2 - 385;
     const int rmarg = bufor->w - lmarg;
     bool le = false;
     bool ri = false;
     BITMAP * tlo = load_png("png/instrt.png", default_palette); rgb_to_bgr(tlo);
     while( !key[ KEY_ESC ] && !key[ KEY_Q ] && strona < 5 && strona > 0){
           animinstrt(tlo);
           masked_stretch_blit(tlo, bufor, 0, 0, tlo->w, tlo->h, 0, 0, bufor->w, bufor->h);
           stretch_blit(tlo, bufor, 0, 0, 10, tlo->h, 0, 0, lmarg - 15, bufor->h);
           stretch_blit(tlo, bufor, tlo->w - 10, 0, 10, tlo->h, rmarg + 15, 0, bufor->w - rmarg + 15, bufor->h);
           //textprintf_right_ex( bufor, norm, rmarg, 10 , c, -1, "%d; %d", lmarg, rmarg);
           switch (strona){
           case 1:{
                textprintf_ex( bufor, head, lmarg, 10 , c2, -1, "%s", napisy.instr1_n);
                textprintf_ex( bufor, norm, lmarg, 10 + haline*1, c, -1, "%s", napisy.instr1_1);
                textprintf_ex( bufor, norm, lmarg, 10 + haline*2, c, -1, "%s", napisy.instr1_2);
                textprintf_ex( bufor, norm, lmarg, 10 + haline*3, c, -1, "%s", napisy.instr1_3);
                textprintf_ex( bufor, norm, lmarg, 10 + haline*4, c, -1, "%s", napisy.instr1_4);
                textprintf_ex( bufor, norm, lmarg, 10 + haline*5, c, -1, "%s", napisy.instr1_5);
                textprintf_ex( bufor, norm, lmarg, 10 + haline*6, c, -1, "%s", napisy.instr1_6);
                textprintf_ex( bufor, norm, lmarg, 10 + haline*7, c, -1, "%s", napisy.instr1_7);
                textprintf_right_ex( bufor, foot, rmarg, 15 + haline*8, c2, -1, "%s", napisy.instr1_s);
                textprintf_ex( bufor, star, lmarg, 100 + haline*7, c2, -1, "%s", napisy.instr1_n);
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 1), c, -1, "%s", napisy.instr1_1);
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 2), c, -1, "%s", napisy.instr1_2);
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 3), c, -1, "%s", napisy.instr1_3);
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 4), c, -1, "%s", napisy.instr1_4);
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 5), c, -1, "%s", napisy.instr1_5);
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 6), c, -1, "%s", napisy.instr1_6);
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 7), c, -1, "%s", napisy.instr1_7);
                textprintf_right_ex( bufor, star, rmarg, 100 + haline*(7 + 8), c2, -1, "%s", napisy.instr1_s);
                }break;
           case 2:{
                textprintf_ex( bufor, head, lmarg, 10 , c2, -1, "%s", napisy.instr2_n );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*1, c, -1, "%s", napisy.instr2_1 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*2, c, -1, "%s", napisy.instr2_2 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*3, c, -1, "%s", napisy.instr2_3 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*4, c, -1, "%s", napisy.instr2_4 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*5, c, -1, "%s", napisy.instr2_5 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*6, c, -1, "%s", napisy.instr2_6 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*7, c, -1, "%s", napisy.instr2_7 );
                textprintf_right_ex( bufor, foot, rmarg, 15 + haline*8, c2, -1, "%s", napisy.instr2_s );
                textprintf_ex( bufor, star, lmarg, 100 + haline*7, c2, -1, "%s", napisy.instr2_n );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 1), c, -1, "%s", napisy.instr2_1 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 2), c, -1, "%s", napisy.instr2_2 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 3), c, -1, "%s", napisy.instr2_3 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 4), c, -1, "%s", napisy.instr2_4 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 5), c, -1, "%s", napisy.instr2_5 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 6), c, -1, "%s", napisy.instr2_6 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 7), c, -1, "%s", napisy.instr2_7 );
                textprintf_right_ex( bufor, star, rmarg, 100 + haline*(7 + 8), c2, -1, "%s", napisy.instr2_s );
                }break;
           case 3:{
                textprintf_ex(  bufor,  head,  lmarg,  10  ,  c2,  -1,  "%s",  napisy.instr3_n );
                textprintf_ex(  bufor,  norm,  lmarg,  10  +  haline*1,  c,  -1,  "%s",  napisy.instr3_1 );
                textprintf_ex(  bufor,  norm,  lmarg,  10  +  haline*2,  c,  -1,  "%s",  napisy.instr3_2 );
                textprintf_ex(  bufor,  norm,  lmarg,  10  +  haline*3,  c,  -1,  "%s",  napisy.instr3_3 );
                textprintf_ex(  bufor,  norm,  lmarg,  10  +  haline*4,  c,  -1,  "%s",  napisy.instr3_4 );
                textprintf_ex(  bufor,  norm,  lmarg,  10  +  haline*5,  c,  -1,  "%s",  napisy.instr3_5 );
                textprintf_ex(  bufor,  norm,  lmarg,  10  +  haline*6,  c,  -1,  "%s",  napisy.instr3_6 );
                textprintf_ex(  bufor,  norm,  lmarg,  10  +  haline*7,  c,  -1,  "%s",  napisy.instr3_7 );
                textprintf_right_ex(  bufor,  foot,  rmarg,  15  +  haline*8,  c2,  -1,  "%s",  napisy.instr3_s );
                textprintf_ex(  bufor,  star,  lmarg,  100  +  haline*7,  c2,  -1,  "%s",  napisy.instr3_n );
                textprintf_ex(  bufor,  star,  lmarg,  100  +  haline*(7  +  1),  c,  -1,  "%s",  napisy.instr3_1 );
                textprintf_ex(  bufor,  star,  lmarg,  100  +  haline*(7  +  2),  c,  -1,  "%s",  napisy.instr3_2 );
                textprintf_ex(  bufor,  star,  lmarg,  100  +  haline*(7  +  3),  c,  -1,  "%s",  napisy.instr3_3 );
                textprintf_ex(  bufor,  star,  lmarg,  100  +  haline*(7  +  4),  c,  -1,  "%s",  napisy.instr3_4 );
                textprintf_ex(  bufor,  star,  lmarg,  100  +  haline*(7  +  5),  c,  -1,  "%s",  napisy.instr3_5 );
                textprintf_ex(  bufor,  star,  lmarg,  100  +  haline*(7  +  6),  c,  -1,  "%s",  napisy.instr3_6 );
                textprintf_ex(  bufor,  star,  lmarg,  100  +  haline*(7  +  7),  c,  -1,  "%s",  napisy.instr3_7 );
                textprintf_right_ex(  bufor,  star,  rmarg,  100  +  haline*(7  +  8),  c2,  -1,  "%s",  napisy.instr3_s );
                }break;
           case 4:{
                textprintf_ex( bufor, head, lmarg, 10 , c2, -1, "%s", napisy.instr3_n );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*1, c, -1, "%s", napisy.instr4_1 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*2, c, -1, "%s", napisy.instr4_2 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*3, c, -1, "%s", napisy.instr4_3 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*4, c, -1, "%s", napisy.instr4_4 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*5, c, -1, "%s", napisy.instr4_5 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*6, c, -1, "%s", napisy.instr4_6 );
                textprintf_ex( bufor, norm, lmarg, 10 + haline*7, c, -1, "%s", napisy.instr4_7 );
                textprintf_right_ex( bufor, foot, rmarg, 15 + haline*8, c2, -1, "%s", napisy.instr4_s );
                textprintf_ex( bufor, star, lmarg, 100 + haline*7, c2, -1, "%s", napisy.instr4_n );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 1), c, -1, "%s", napisy.instr4_1 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 2), c, -1, "%s", napisy.instr4_2 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 3), c, -1, "%s", napisy.instr4_3 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 4), c, -1, "%s", napisy.instr4_4 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 5), c, -1, "%s", napisy.instr4_5 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 6), c, -1, "%s", napisy.instr4_6 );
                textprintf_ex( bufor, star, lmarg, 100 + haline*(7 + 7), c, -1, "%s", napisy.instr4_7 );
                textprintf_right_ex( bufor, star, rmarg, 100 + haline*(7 + 8), c2, -1, "%s", napisy.instr4_s );
                }break;
           }
           if (key[KEY_RIGHT] && ri == false){strona++; ri = true;}
           if (key[KEY_LEFT] && le == false){strona--; le = true;}
           if (!key[KEY_RIGHT]) ri = false;
           if (!key[KEY_LEFT]) le = false;
           //blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
           stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h, 0, 0, SCREEN_W, SCREEN_H);
           clear_to_color( bufor, makecol( 0, 0, 0 ));
           rest(10);
     }
     destroy_bitmap(tlo);
}

void draw_map(BITMAP * bufor, BITMAP * mapa, int x, int y){
     masked_blit( mapa, bufor, x, y, 0, 0, bufor->w, bufor->h );
}

void grub(int x, int y, BITMAP * bufor){
     BITMAP * grub = load_png( "png/grubmini.png", default_palette); rgb_to_bgr(grub);
     BITMAP * grubmaxi = load_png( "png/grub.png", default_palette); rgb_to_bgr(grubmaxi);
     masked_blit( grub, bufor, 0, 0, x, y, x + 100, y + 100 );
//   blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
     stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                 0, 0, SCREEN_W, SCREEN_H);
     while(!key[KEY_ESC] && !key[KEY_Q] && !key[KEY_ENTER]) rest(50);
     while(key[KEY_ESC] || key[KEY_Q] || key[KEY_ENTER]) rest(50);
     clear_to_color(bufor, makecol(0,0,0));
     masked_blit( grubmaxi, bufor, 0, 0, (bufor->w - grubmaxi->w)/2, 0, grubmaxi->w, grubmaxi->h );
     stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                 0, 0, SCREEN_W, SCREEN_H);
     while(!key[KEY_ESC] && !key[KEY_Q] && !key[KEY_ENTER]) rest(50);
     destroy_bitmap( grub );
     destroy_bitmap( grubmaxi );
     while(key[KEY_ESC] || key[KEY_Q] || key[KEY_ENTER]) rest(50);
}

void wygrana(BITMAP* bufor, char win[], FONT* star, FONT* norm){
     int color1 = makecol(64, 255, 64);
     int color2 = color1;
     textprintf_centre_ex( bufor, star, bufor->w / 2, bufor->h/2 + 20, color1, -1, "%s", win);
     textprintf_centre_ex( bufor, norm, bufor->w / 2, bufor->h/2 - 20, color2, -1, "%s", win);
//   blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
     stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                 0, 0, SCREEN_W, SCREEN_H);
     while(!key[KEY_ESC] && !key[KEY_Q] && !key[KEY_ENTER]) rest(50);
     while(key[KEY_ESC] && key[KEY_Q] && key[KEY_ENTER]) rest(50);
}

void pauza(BITMAP* bufor, char pause[], FONT* star, FONT* norm){
     int color1 = makecol(64, 255, 64);
     int color2 = color1;
     textprintf_centre_ex( bufor, star, bufor->w / 2, bufor->h/2 + 20, color1, -1, "%s", pause);
     textprintf_centre_ex( bufor, norm, bufor->w / 2, bufor->h/2 - 20, color2, -1, "%s", pause);
//   blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
     stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                 0, 0, SCREEN_W, SCREEN_H);
     while(key[KEY_P]) rest(50);
     while(!key[KEY_P]) rest(50);
     while(key[KEY_P]) rest(50);
}

BITMAP* level(int level, int &x, int &y, int &grawitacja, int &fuel){
     char nazwa[PLANGHT];
     char conf[PLANGHT];
     char znak;
     int color;
     sprintf(nazwa,"png/map/%d/map.png", level);
     sprintf(conf,"png/map/%d/conf.txt", level);
     BITMAP* map;
     map = load_png( nazwa, default_palette ); rgb_to_bgr(map);

     /*color = getpixel(map, 0, 599);
     grawitacja = getr(color) -127;
     x = getg(color)*10 - 400;
     y = getb(color)*3;*/
     fstream fconfig (conf, ios::in);

     fconfig.get(znak);
     while (znak < 48 || znak > 57){
        fconfig.get(znak);
     }
     grawitacja = znak - 48;
     fconfig.get(znak);
     grawitacja = grawitacja * 10 + znak - 48;
     fconfig.get(znak);
     if (znak == '-') grawitacja *= -1;

     fconfig.get(znak);
     while (znak < 48 || znak > 57){
        fconfig.get(znak);
     }
     x = znak - 48;
     for(int i = 1; i < 5; i++){
        fconfig.get(znak);
        x = x*10 + znak-48;
     }
     x -= 400;
     fconfig.get(znak);
     while (znak < 48 || znak > 57){
        fconfig.get(znak);
     }
     y = znak - 48;
     for(int i = 1; i < 3; i++){
        fconfig.get(znak);
        y = y*10 + znak-48;
     }
     fconfig.get(znak);
     while (znak < 48 || znak > 57){
        fconfig.get(znak);
     }
     fuel = znak - 48;
     for(int i = 1; i < 6; i++){
        fconfig.get(znak);
        fuel = fuel*10 + znak-48;
     }
     fconfig.close();
     return map;
}

BITMAP* levelback(int level){
     char nazwa[PLANGHT];
     int color;
     sprintf(nazwa,"png/map/%d/mapback.png", level);
     BITMAP* mapb;
     mapb = load_png( nazwa, default_palette ); rgb_to_bgr(mapb);
     return mapb;
}

BITMAP* levelfront(int level){
     char nazwa[PLANGHT];
     int color;
     sprintf(nazwa,"png/map/%d/mapfront.png", level);
     BITMAP* mapf;
     mapf = load_png( nazwa, default_palette ); rgb_to_bgr(mapf);
     return mapf;
}

void dzwiek_silnik(int ciag, SAMPLE * silnik){
     adjust_sample( silnik, 255, 127, 1000/15 * ciag, 1 );
}

void trafienie_g(int hit, BITMAP* bufor, int x, int y, int kat){
     char nazwa[PLANGHT];
     hit++;
     sprintf(nazwa,"png/hitbox/hitbox%d.png", hit);
     BITMAP* traf = NULL;
     traf = load_png( nazwa, default_palette ); rgb_to_bgr(traf);
     if (traf != NULL) rotate_sprite( bufor, traf, x, y, itofix( kat ));
     destroy_bitmap( traf );
}

void rgb_to_bgr(BITMAP * bitmapa){
    if (bitmap_color_depth(screen) != 32) return;
	int color;
    int cb;
    int cr;
    for (int iii = 0; iii < bitmapa->w; ++iii){
	   	for (int jjj = 0; jjj < bitmapa->h; ++jjj){
			color = _getpixel32(bitmapa, iii, jjj);
			cb = getb(color);
			cr = getr(color);
			color = makecol(cb, getg(color), cr);
			putpixel(bitmapa, iii, jjj, color);
		}
	}
}

void animpipe(BITMAP * bitmapa){
	int color;
    int cb;
    int cr;
    int cg;
    for (int iii = 0; iii < bitmapa->w; ++iii){
	   	for (int jjj = 0; jjj < bitmapa->h; ++jjj){
			color = getpixel(bitmapa, iii, jjj);
			cb = getb(color);
			if (cb != 255){
                cr = getr(color);
                cg = getg(color);
                if (cr >= 155 && cr < 225 && cb == 155 && cg == 155) cr++;
                else if (cg >= 155 && cg < 225 && cr == 225 && cb == 155) cg++;
                else if (cb >= 155 && cb < 225 && cr == 225 && cg == 225) cb++;
                else if (cr > 155 && cr <= 225 && cb == 225 && cg == 225) cr--;
                else if (cg > 155 && cg <= 225 && cr == 155 && cb == 225) cg--;
                else if (cb > 155 && cb <= 225 && cr == 155 && cg == 155) cb--;
                color = makecol(cr, cg, cb);
                putpixel(bitmapa, iii, jjj, color);
			} else jjj = 590;
		}
	}
}

void animinstrt(BITMAP * bitmapa){
	int color;
    int cb;
    int cr;
    int cg;
    for (int iii = 0; iii < bitmapa->h; ++iii){
	   	for (int jjj = 0; jjj < bitmapa->w; ++jjj){
			color = getpixel(bitmapa, jjj, iii);
			cb = getb(color);
			if (cb != 255){
                cr = getr(color);
                cg = getg(color);
                if (cr >= 155 && cr < 225 && cb == 155 && cg == 155) cr++;
                else if (cg >= 155 && cg < 225 && cr == 225 && cb == 155) cg++;
                else if (cb >= 155 && cb < 225 && cr == 225 && cg == 225) cb++;
                else if (cr > 155 && cr <= 225 && cb == 225 && cg == 225) cr--;
                else if (cg > 155 && cg <= 225 && cr == 155 && cb == 225) cg--;
                else if (cb > 155 && cb <= 225 && cr == 155 && cg == 155) cb--;
                color = makecol(cr, cg, cb);
                putpixel(bitmapa, jjj, iii, color);
			} else jjj = 789;
		}
	}
}

void printmover(mover lista[], int x, int y, BITMAP * bufor, int z){
	 int num = 0;
	 while (lista[num].istnieje == true && num < MOVNUM){
		   if (lista[num].z == z){
			  if ( lista[num].frame[lista[num].numframe] != NULL )
	 	   	  	 rotate_sprite( bufor, lista[num].frame[lista[num].numframe], lista[num].x - x, lista[num].y - y, itofix( lista[num].kat));
  		   }
		   ++num; //allegro_message("%s\n%s", lista[num].path.c_str(),lista[num].img.c_str()); rest(100);
     }
}

void refreshmover(mover lista[]){
    /*STRUKTURA PLIKU CONF
        ilosc znakow:
        [1] -- numer obrazka
        [2-6] -- wspolrzedna x (5 cyfry zapisu dziesietnego)
        [7-12] -- wspolrzedna y (5 cyfry zapisu dziesietnego)
        [13] -- wspolrzedna z (1 cyfra)
        [14-16] -- kat obrotu (3 cyfry zapisu dziesietnego kata {0-255} )

        [1-16] blok 1 klatki
        [17-31] blok 2 klatki
    */
	 int num = 0;
     char litera;
     int znak = 0;
	 while (lista[num].istnieje == true && num < MOVNUM){
  	 	   if (lista[num].conf.is_open()){
  		   	  lista[num].conf.seekg(lista[num].num);
			  lista[num].conf.get(litera);
			  lista[num].numframe = litera - 48;
			  lista[num].conf.get(litera);
			  lista[num].x = 0;
			  for (int i = 0; i < 5; ++i){
			  	  lista[num].x = lista[num].x * 10 + (litera - 48);
			  	  lista[num].conf.get(litera);
			  }
			  lista[num].y = 0;
			  for (int i = 0; i < 5; ++i){
			  	  lista[num].y = lista[num].y * 10 + (litera - 48);
			  	  lista[num].conf.get(litera);
			  }
			  lista[num].z = litera - 48;
			  lista[num].kat = 0;
			  for (int i = 0; i < 3; ++i){
			  	  lista[num].conf.get(litera);
			  	  lista[num].kat = lista[num].kat * 10 + (litera - 48);
			  }
			  lista[num].num = lista[num].conf.tellg();
			  lista[num].conf.seekg(0, ios_base::end);
  	  		  if (((int)lista[num].conf.tellg() - lista[num].num) < 15) lista[num].num = 0;
           }
           ++num;
     }
}

void loadmover(mover lista[], int level){
     char nazwa[PLANGHT];
     sprintf(nazwa,"png/map/%d/movers.txt", level);
     fstream fmovers (nazwa, ios::in);
//     fmovers.open(nazwa.c_str(), ios::in);
     int exit = 0;
     char litera;
     int znak = 0;
     int num = 0;
     int startpath;
     int endpath;
     int n = 0;
     while(exit != 1 && fmovers.is_open() &&  num < MOVNUM){
		   fmovers.get(litera);
           if( litera == '[') {
		   	   znak = 1;
		   	   startpath = fmovers.tellg();
 			   fmovers.get(litera);
			   while ( litera != ']'){
 	   	 	   		 fmovers.get(litera);
	 		         znak++;
		       }
		       endpath = fmovers.tellg();
		       fmovers.seekg(startpath);
      		   if (znak < PLANGHT) fmovers.get(lista[num].path, znak);
		       if (lista[num].conf.is_open()) lista[num].conf.close();
		       lista[num].conf.open(lista[num].path, ios::in);
			   //allegro_message("%s\n%d; %d; %d", lista[num].path.c_str(), startpath, endpath, znak);
			   lista[num].istnieje = true;
			   lista[num].num = 0;
			   fmovers.seekg(endpath);
			   while (litera != '(')
			   		fmovers.get(litera);
			   if ( litera == '(') {
  	   		   	  znak = 1;
		   	   	  startpath = fmovers.tellg();
 			   	  fmovers.get(litera);
			   	  while ( litera != ')'){
 	   	 	   		 	fmovers.get(litera);
	 		         	znak++;
      			  }
		       	  endpath = fmovers.tellg();
		       	  fmovers.seekg(startpath);
      		   	  if (znak < PLANGHT) fmovers.get(lista[num].img, znak);
			   }
			   num++;
		   }
		   if( litera == '=' || fmovers.eof()) exit = 1;
           rest(1);
     }
	 lista[num].istnieje = false;
     rest(1);
     fmovers.close();
}

void loadimgmover (mover lista[]){
	 /*
	 otwieram plik "img" z listą imgmov
	 szukam pierwszej ścieżki
	 sprawdzam 1-erwszą bitmap* czy null jak tak to destroy_bitmap
	 wpisuje do pierwszej Bitmap*
	 powtarzam
	 zapentlam dopóki lista[num] istnieje
	 */
	 int num = 0;
     int exit = 0;
     char litera;
     int znak = 0;
     int imgnum = 0;
     int startpath;
     int endpath;
     fstream fmovimg;
     char tempimg[PLANGHT];
     while (exit == 0 && num < MOVNUM){
	 	   if ( lista[num].istnieje == false ) exit = 1;
	 	   else {
		   		fmovimg.open( lista[num].img, ios::in );
		   		imgnum = 0;
			  	while (fmovimg.is_open() && imgnum < MOVFRAME){
				   	  fmovimg.get(litera);
				   	  if( litera == '[') {
		   	   		  	  znak = 1;
		   	   			  startpath = fmovimg.tellg();
 			   			  fmovimg.get(litera);
			   			  while ( litera != ']'){
 	   	 	   		 	  		fmovimg.get(litera);
	 		         			znak++;
      					  }
						  endpath = fmovimg.tellg();
		       			  fmovimg.seekg(startpath);
				  		  if (znak < PLANGHT) fmovimg.get(tempimg, znak);
		       			  if (lista[num].frame[imgnum] != NULL) destroy_bitmap( lista[num].frame[imgnum] );
                		  lista[num].frame[imgnum] = load_png( tempimg, default_palette); rgb_to_bgr(lista[num].frame[imgnum]);
			   			  //allegro_message("%s\n%d; %d; %d", lista[num].img.c_str(), startpath, endpath, znak);
			   			  fmovimg.seekg(endpath);
			   			  imgnum++;
      			      }
      			      if( litera == '=' || fmovimg.eof() ){
			  	  		  imgnum = MOVFRAME+1;
			  	  		  fmovimg.close();
				  	  }
  		        }
		   }
		   num++;
     }
}

void fillmover(mover list[], int n){
	 for (int i = 0; i < n; i++){
		list[i].istnieje = false;
		for	(int k = 0; k < MOVFRAME; k++) list[i].frame[k] = NULL;
	 }
}

int do_sha1_file(char *name, unsigned char *out){
    FILE *f;
    unsigned char buf[8192];
    SHA_CTX sc;
    int err;

    f = fopen(name, "rb");
    if (f == NULL) {
        /* do something smart here: the file could not be opened */
        return -1;
    }
    SHA1_Init(&sc);
    for (;;) {
        size_t len;

        len = fread(buf, 1, sizeof buf, f);
        if (len == 0)
            break;
        SHA1_Update(&sc, buf, len);
    }
    err = ferror(f);
    fclose(f);
    if (err) {
        /* some I/O error was encountered; report the error */
        return -1;
    }
    SHA1_Final(out, &sc);
    return 0;
}

int do_sha1_map_pack(unsigned char *out, int alllevel){
    char mapp[PLANGHT];
    char conf[PLANGHT];
    char movp[PLANGHT];
    int level = 1;
    unsigned char sha1file[20];
    SHA_CTX sc;
    SHA1_Init(&sc);
    while (level <= alllevel) {
        sprintf(mapp,"png/map/%d/map.png", level);
        sprintf(conf,"png/map/%d/conf.txt", level);
        sprintf(movp,"png/map/%d/movers.txt", level);
        do_sha1_file(mapp, sha1file);
        SHA1_Update(&sc, sha1file, 20);
        do_sha1_file(conf, sha1file);
        SHA1_Update(&sc, sha1file, 20);
        do_sha1_file(movp, sha1file);
        SHA1_Update(&sc, sha1file, 20);
        level++;
    }
    SHA1_Final(out, &sc);
    return 0;
}

int CreateIfNotExists( const char * sFileName ) {
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
        char wypel[] = "0\nBRAK\n0\nBRAK\n0\nBRAK\n0\nBRAK\n0\nBRAK\n";
        nowyPlik.write(wypel, sizeof(wypel));
        nowyPlik.close();
        return 2; //udało się utworzyć plik
    }
    return 0; //nie da się utworzyć pliku i plik nie istnieje
}

void highscore(char name[], napi napisy, int score, BITMAP * bufor, FONT * star, FONT * norm, FONT * czyt){
    const int numrek = 6;/**< ilosc wpisow +1 */ /**^ funkcja do wpisywania nowych najlepszych wynikow ^*/
    const int lmarg = bufor->w/2 - 385;/**< prawy/lewy margines do pisania tekstu */
    const int rmarg = bufor->w - lmarg;
    char nazwa[PLANGHT];
    char hightab[numrek][2][SCORL];/**< tablica do ktorej zostana przepisane najlepsze wyniki */
    itoa(score, hightab[0][0], 10);
    strncpy( hightab[0][1], "                  ", sizeof(hightab[0][1]) - 1);
    hightab[0][1][strlen(hightab[0][1])] = '\0';/**< wypelninie indeksu [0] dla aktualnego wyniku */
    sprintf(nazwa,"high/%s.hig", name);
    int istnieje = CreateIfNotExists(nazwa);
    int wynik = numrek;
    if(istnieje != 0){
        fstream highfile (nazwa, ios::in);
//        if(istnieje == 1){ //wczytaj zapisane wyniki
            for(int i = 1; i < numrek; i++){
                highfile.getline(hightab[i][0], SCORL);
                highfile.getline(hightab[i][1], SCORL); /**< wczytanie pliku i najlepszych ponktow */
            }
//        }
        highfile.close();
        for (int i = 1; i < numrek; i++){/**< szukanie najwyzszego miejsca dla aktualnego wyniku */
                if(score > atoi(hightab[i][0])) {
                        wynik = i;
                        i = numrek;
                }
        }
        if (wynik != numrek){/**< jesli wynik nie jest najgorszy (zakfalifikowal sie) */
            // pobieranie nazwy gracza;
            char klawisz = ' ';
            int color1 = makecol(64, 255, 64);
            int color2 = makecol(240, 230, 64);
            int color3 = makecol(64, 255, 64);
            const int hline = 30;
            const int vch = 25;
            int numznak = 0;
            clear_keybuf();
            clear_to_color( bufor, makecol( 0, 0, 0 ));
//          blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
            stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                        0, 0, SCREEN_W, SCREEN_H);
            BITMAP * tlo = load_png("png/instrt.png", default_palette); rgb_to_bgr(tlo);
            while (!key[KEY_ENTER] && !key[KEY_ESC]){/**< pentla do zapisu nazwy wyniku */

                animinstrt(tlo);
                masked_stretch_blit(tlo, bufor, 0, 0, tlo->w, tlo->h, 0, 0, bufor->w, bufor->h);
                stretch_blit(tlo, bufor, 0, 0, 10, tlo->h, 0, 0, lmarg - 15, bufor->h);
                stretch_blit(tlo, bufor, tlo->w - 10, 0, 10, tlo->h, rmarg + 15, 0, bufor->w - rmarg + 15, bufor->h);

                 rest(10);/**V wyswietlanie stosownych napisow V*/
                 textprintf_centre_ex( bufor, norm, bufor->w / 2, 10, color1, -1, "%s", napisy.hsnw1);
                 textprintf_centre_ex( bufor, star, bufor->w / 2, 310, color1, -1, "%s", napisy.hsnw1);
                 textprintf_centre_ex( bufor, norm, bufor->w / 2, 10 + hline, color1, -1, "%d", wynik);
                 textprintf_centre_ex( bufor, star, bufor->w / 2, 310 + hline, color1, -1, "%d", wynik);
                 textprintf_centre_ex( bufor, norm, bufor->w / 2, 10 + hline*2, color1, -1, "%s", napisy.hsnw2);
                 textprintf_centre_ex( bufor, star, bufor->w / 2, 310 + hline*2, color1, -1, "%s", napisy.hsnw2);
                 textprintf_ex( bufor, norm, lmarg, 10 + hline*3, color1, -1, "%s", napisy.hsname);
                 textprintf_ex( bufor, star, lmarg, 310 + hline*3, color1, -1, "%s", napisy.hsname);
                 textprintf_right_ex( bufor, norm, rmarg, 10 + hline*5, color2, -1, "%s", napisy.hsnws1);
                 textprintf_right_ex( bufor, star, rmarg, 310 + hline*5, color2, -1, "%s", napisy.hsnws1);
                 textprintf_right_ex( bufor, norm, rmarg, 10 + hline*6, color2, -1, "%s", napisy.hsnws2);
                 textprintf_right_ex( bufor, star, rmarg, 310 + hline*6, color2, -1, "%s", napisy.hsnws2);
                 for(int n = 0; n < SCORL-1; n++){
                    textprintf_ex( bufor, czyt, lmarg +15 + vch*n, 10 + hline*4, color3, -1, "%c", hightab[0][1][n]);
                    textprintf_ex( bufor, star, lmarg +15 + vch*n, 310 + hline*4, color3, -1, "%c", hightab[0][1][n]);
                 }
                 textprintf_ex( bufor, norm, lmarg+15 + vch*numznak, 10 + hline*4, color1, -1, "%c", '_');
                 textprintf_ex( bufor, norm, lmarg+15 + vch*numznak, 310 + hline*4, color1, -1, "%c", '_');
//               blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
                 stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                             0, 0, SCREEN_W, SCREEN_H);
                 clear_to_color( bufor, makecol( 0, 0, 0 ));
                 klawisz = '\0';
                 if(keypressed())
                    klawisz = readkey();
                 if (klawisz == 8){
                    if ( numznak > 0 ) numznak--;
                    hightab[0][1][numznak] = ' ';
                 }
                 else if (klawisz && klawisz != 27 && klawisz != 13 && klawisz != 10){
                    hightab[0][1][numznak] = klawisz;
                    numznak++;
                 }
                 if (key[KEY_LEFT]) numznak--;
                 if (key[KEY_RIGHT]) numznak++;
                 while (key[KEY_LEFT] || key[KEY_RIGHT]) rest(50);
                 if (numznak < 0) numznak = 0;
                 if (numznak > SCORL-1) numznak = SCORL-1;
            }
            destroy_bitmap(tlo);
            while(key[KEY_ENTER]) rest(50);
            char buf[2][SCORL];
            for (int i = wynik; i < numrek; i++){/**< przesuwanie tablicy do zrobienia miejsca dla nowego wyniku */
                    strncpy(buf[0], hightab[i][0], sizeof(buf[0]) - 1);
                    strncpy(buf[1], hightab[i][1], sizeof(buf[1]) - 1);
                    buf[0][strlen(buf[0])]= '\0';
                    buf[1][strlen(buf[1])]= '\0';
                    strncpy(hightab[i][0], hightab[0][0], sizeof(hightab[i][0]) - 1);
                    strncpy(hightab[i][1], hightab[0][1], sizeof(hightab[i][1]) - 1);
                    hightab[i][0][strlen(hightab[i][0])]= '\0';
                    hightab[i][1][strlen(hightab[i][1])]= '\0';
                    strncpy( hightab[0][0], buf[0], sizeof(hightab[0][0]) - 1);
                    strncpy( hightab[0][1], buf[1], sizeof(hightab[0][1]) - 1);
                    hightab[0][0][strlen(hightab[0][0])]= '\0';
                    hightab[0][1][strlen(hightab[0][1])]= '\0';
            }
            highfile.open(nazwa, ios::out|ios::trunc);/**< przepisanie tablicy z programu jako nowa tablice dopliku */
            for(int n = 1; n < numrek; n++){
                for (int n2 = 0; n2 < 2; n2++){
                    for (int n3 = 0; (n3 < SCORL)&&(hightab[n][n2][n3] != '\0'); n3++) highfile.put(hightab[n][n2][n3]);
                    highfile.put('\n');
                }
            }
            highfile.close();
            viewscore(name, napisy, bufor, star, norm, czyt, wynik); /**< wypisanie najlepszych wynikow z wyroznieniem nowego */
        }
    }
}

void viewscore(char name[], napi napisy, BITMAP * bufor, FONT * star, FONT * norm, FONT * czyt, int wyr){
    char nazwa[PLANGHT];
    sprintf(nazwa,"high/%s.hig", name);
    int istnieje = CreateIfNotExists(nazwa);
    const int numrek = 5;
    const int lmarg = bufor->w/2 - 385;
    const int rmarg = bufor->w - lmarg;
    char hightab[numrek][2][SCORL];
    if(istnieje != 0){
        fstream highfile (nazwa, ios::in);
        for(int i = 0; i < numrek; i++){
            highfile.getline(hightab[i][0], SCORL);
            highfile.getline(hightab[i][1], SCORL);
        }
        highfile.close();
        int color1 = makecol(64, 255, 64);
        int color2 = makecol(240, 230, 64);
        int color3 = makecol(64, 255, 64);
        int color4 = makecol(64, 255, 64);
        int colorw, wr, wb, wg;
        wr = 16;
        wg = 200;
        wb = 16;
        BITMAP * tlo = load_png("png/instrt.png", default_palette); rgb_to_bgr(tlo);
        const int hline = 30;
        while( !key[ KEY_ESC ] && !key[ KEY_Q ]){
            animinstrt(tlo);
            masked_stretch_blit(tlo, bufor, 0, 0, tlo->w, tlo->h, 0, 0, bufor->w, bufor->h);
            stretch_blit(tlo, bufor, 0, 0, 10, tlo->h, 0, 0, lmarg - 15, bufor->h);
            stretch_blit(tlo, bufor, tlo->w - 10, 0, 10, tlo->h, rmarg + 15, 0, bufor->w - rmarg + 15, bufor->h);
            if (wyr != -1){
                if(wr >= 16 && wr < 112 && wb == 16) {wr+=2; wg-=4;}
                else if(wb >= 16 && wb < 112 && wr == 112) {wb+=2; wg+=4;}
                else if(wr > 16 && wr <= 112 && wb == 112) {wr-=2; wg-=4;}
                else if(wb > 16 && wb <= 112 && wr == 16) {wb-=2; wg+=4;}
                color4 = makecol(wr,wg,wb);
            }
            textprintf_centre_ex( bufor, norm, bufor->w / 2, 10, color1, -1, "%s", napisy.hsvn);
            textprintf_centre_ex( bufor, star, bufor->w / 2, 310, color1, -1, "%s", napisy.hsvn);
            textprintf_right_ex( bufor, norm, rmarg, 10 + hline*6, color2, -1, "%s", napisy.hsvs);
            textprintf_right_ex( bufor, star, rmarg, 310 + hline*6, color2, -1, "%s", napisy.hsvs);
            for(int i = 0; i < numrek; ++i){
                if ( i == wyr-1) colorw = color4;
                else colorw = color3;
                textprintf_ex( bufor, czyt, lmarg, 10 + hline*(i+1), colorw, -1, "%s", hightab[i][1]);
                textprintf_ex( bufor, star, lmarg, 310 + hline*(i+1), colorw, -1, "%s", hightab[i][1]);
                textprintf_right_ex( bufor, czyt, rmarg, 10 + hline*(i+1), colorw, -1, "%s", hightab[i][0]);
                textprintf_right_ex( bufor, star, rmarg, 310 + hline*(i+1), colorw, -1, "%s", hightab[i][0]);
            }

//          blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
            stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                        0, 0, SCREEN_W, SCREEN_H);
            clear_to_color( bufor, makecol( 0, 0, 0 ));
            rest(10);
        }
        destroy_bitmap(tlo);
        while (key[KEY_Q]) rest(50);
    }

}

conf readconf(){ //wczytuje plik config
    conf tempconf;
    fstream conffile ("conf.txt", ios::in);
    while(conffile.is_open() && !conffile.eof()){
        int len;
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
                int c[3];
                for (int i = 0; i < 3; ++i){
                    conffile.get(litera);
                    c[i] = litera - 48;
                    conffile.get(litera);
                    c[i] = c[i]*10 + litera - 48;
                    conffile.get(litera);
                    c[i] = c[i]*10 + litera - 48;
                }
                tempconf.colors[ind] = makecol(c[0],c[1],c[2]);
                break;
            //falltrough END
            case 865:
                while (litera != '[') conffile.get(litera);
                conffile.get(litera);
                tempconf.windowed = litera - 48;
                break;
        }
//        allegro_message("%s: %d",buf, param);
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

        sprintf(bufor, "hcpow[%03d%03d%03d]::\n?",getr(tempconf.colors[0]),getg(tempconf.colors[0]),getb(tempconf.colors[0]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcfue[%03d%03d%03d]::\n?",getr(tempconf.colors[1]),getg(tempconf.colors[1]),getb(tempconf.colors[1]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcang[%03d%03d%03d]::\n?",getr(tempconf.colors[2]),getg(tempconf.colors[2]),getb(tempconf.colors[2]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcsco[%03d%03d%03d]::\n?",getr(tempconf.colors[3]),getg(tempconf.colors[3]),getb(tempconf.colors[3]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hca[%03d%03d%03d]::\n?",getr(tempconf.colors[4]),getg(tempconf.colors[4]),getb(tempconf.colors[4]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "nca[%03d%03d%03d]::\n?",getr(tempconf.colors[5]),getg(tempconf.colors[5]),getb(tempconf.colors[5]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "ncns[%03d%03d%03d]::\n?",getr(tempconf.colors[6]),getg(tempconf.colors[6]),getb(tempconf.colors[6]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcfuw[%03d%03d%03d]::\n?",getr(tempconf.colors[7]),getg(tempconf.colors[7]),getb(tempconf.colors[7]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);

        sprintf(bufor, "hcwsk[%03d%03d%03d]::?",getr(tempconf.colors[8]),getg(tempconf.colors[8]),getb(tempconf.colors[8]));
        for(int i = 0; bufor[i] != '?'; ++i)
            conffile.put(bufor[i]);
    }
    conffile.close();
}

void pisanie(BITMAP* bufor, FONT* star, int color[OPTNUMLIN], int hline, char teksty[OPTNUMLIN][OPTLINEL+32], FONT* fonts[OPTNUMLIN]){
     for(int i = 0; i < OPTNUMLIN; ++i){
        textprintf_centre_ex( bufor, star, bufor->w / 4, bufor->h/40 + hline * i, color[i], -1, "%s", teksty[i]);
        textprintf_centre_ex( bufor, fonts[i], bufor->w / 2 + bufor->w / 4, bufor->h/40 + hline * i, color[i], -1, "%s", teksty[i]);
    }
}

BITMAP* options_roz(BITMAP* bufor, BITMAP* pipe, napi napisy, FONT * star, FONT * norm, FONT * czyt, conf tempconf){
    int kursor = 2;
    //zmienne do sprawdzania dlugosci trzymania klawisza
    int lastg = 0;
    int lastd = 0;
    bool change_res = false;
    char litera;

    const int hline = 30;
    int color[OPTNUMLIN];
    FONT* czcionki[OPTNUMLIN];
    int colorw, wr, wb, wg;
    wr = 16;
    wg = 200;
    wb = 16;
	while(!key[ KEY_ESC ] && !key[ KEY_Q ]){
        animpipe(pipe);
        masked_stretch_blit(pipe, bufor, 0, 0,
                            pipe->w, pipe->h,
                            0, 0, bufor->w, bufor->h);

        for (int i = 1; i < OPTNUMLIN-3; ++i) {
            color[i] = tempconf.colors[5];
            czcionki[i] = norm;
        }

        color[0] = tempconf.colors[6];
        czcionki[0] = norm;
        color[OPTNUMLIN-3] = tempconf.colors[6];
        czcionki[OPTNUMLIN-3] = czyt;
        color[OPTNUMLIN-2] = tempconf.colors[6];
        czcionki[OPTNUMLIN-2] = czyt;
        color[OPTNUMLIN-1] = tempconf.colors[6];
        czcionki[OPTNUMLIN-1] = norm;

        if(wr >= 16 && wr < 112 && wb == 16) {wr+=2; wg-=4;}
        else if(wb >= 16 && wb < 112 && wr == 112) {wb+=2; wg+=4;}
        else if(wr > 16 && wr <= 112 && wb == 112) {wr-=2; wg-=4;}
        else if(wb > 16 && wb <= 112 && wr == 16) {wb-=2; wg+=4;}
        colorw = makecol(wr,wg,wb);
        color[kursor] = colorw;
        if (change_res) color[kursor+1] = colorw;

        char teksty[OPTNUMLIN][OPTLINEL+32];
        for (int i = 0; i < OPTNUMLIN; ++i)
            strcpy(teksty[i], napisy.opt_roz[i]);

        teksty[1][0] = '\0';
        teksty[4][0] = '\0';
        teksty[7][0] = '\0';
        teksty[10][0] = '\0';

        sprintf(teksty[3],"[%d]", tempconf.resx);
        sprintf(teksty[6],"[%d]", tempconf.resy);
        teksty[9][tempconf.windowed+1] = ' ';

        pisanie(bufor, star, color, hline, teksty, czcionki);

        if( key[ KEY_ENTER ] || key[ KEY_SPACE ] ){
            if ( kursor == 2 || kursor == 5) {/**< jezeli kursor jest ustawiony na x lub y */
                while(key[ KEY_ENTER ] || key[ KEY_SPACE ])rest(50);
                if(change_res == false){ /**< to przestaw flage na edycje rozdzielczosci */
                    change_res = true;
                    switch (kursor){/**< i wyzeruj wartosc x lub y */
                    case 2: tempconf.resx = 0; break;
                    case 5: tempconf.resy = 0; break;
                    }
                }
                else change_res = false;
            }
        }

        if(change_res){
            int *res = NULL;
            switch (kursor){/**< ustalenie na podstawie kursora ktora dlugosc edytowac */
            case 2: res = &tempconf.resx; break;
            case 5: res = &tempconf.resy; break;
            }
            if(keypressed()){/**< nacisnieto klawisz */
                    litera = readkey();
                    litera = litera-48;/**< jesli bakspace to usun zad wielkosci */
                    if (litera == -40) *res *= 0.1;
                    else if (litera >= 0 && litera <= 9){/**< jesli liczba to dodaj zad wielkosci i dopisz na koncu liczbe */
                            *res = *res *10 +litera;
                    }
            }
        }
     else { /**< przeskoczenie elementow ktore moga zmienic miejsce kursora  */
        if( key[ KEY_UP ] ){
             if (lastg == 0){
                kursor--;
             }
             lastg++;
             lastd = 0;
        }
        if( key[ KEY_DOWN ] ){
             if (lastd == 0){
                kursor++;
             }
             lastd++;
             lastg = 0;
        }
        if( key[ KEY_LEFT ] ){
             if (kursor == 8){
                  tempconf.windowed = 2;
             }
        }
        if( key[ KEY_RIGHT ] ){
             if (kursor == 8){
                  tempconf.windowed = 0;
             }
        }
        if ((lastg > 25 || lastd > 25 )|| (!key[ KEY_DOWN ] && !key[ KEY_UP ])){
           lastg = 0;
           lastd = 0;
        }

        switch (kursor){
        case -1:
        case 0: kursor = 2; break;
        case 1: kursor = 12; break;
        case 3: kursor = 5; break;
        case 4: kursor = 2; break;
        case 6: kursor = 8; break;
        case 7: kursor = 5; break;
        case 9: kursor = 11; break;
        case 10: kursor = 8; break;
        case 13: kursor = 2; break;
        }
    }
        rectfill(bufor, bufor->w/2-10, kursor * hline + 15, bufor->w/2+10, (kursor+1) * hline + 15, colorw);
        triangle(bufor, bufor->w/2-20, kursor * hline + 15 + hline/2, bufor->w/2-10, kursor * hline + 15, bufor->w/2-10, (kursor+1) * hline + 15, colorw);
        triangle(bufor, bufor->w/2+20, kursor * hline + 15 + hline/2, bufor->w/2+10, kursor * hline + 15, bufor->w/2+10, (kursor+1) * hline + 15, colorw);
        //blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
        stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                    0, 0, SCREEN_W, SCREEN_H);
        clear_to_color( bufor, makecol( 0, 0, 0 ));
        if( key[ KEY_ENTER ] || key[ KEY_SPACE ] ){
            while(key[ KEY_ENTER ] || key[ KEY_SPACE ])rest(50);
              switch (kursor){
              case 11:
                  saveconf(tempconf);
                  destroy_bitmap(bufor);
                  bufor = tworzenie_okna_i_bufora(tempconf.resx, tempconf.resy, tempconf.windowed);
                  break;
              case 12: tempconf = readconf(); break;
              }
        }
        rest(10);
    }
    while(key[ KEY_Q ]) rest(10);
    return bufor;
}

BITMAP* options_kol(BITMAP* bufor, BITMAP* pipe, napi napisy, FONT * star, FONT * norm, FONT * czyt, conf tempconf, BITMAP* hud){
    int kursor = 2;
    //zmienne do sprawdzania dlugosci trzymania klawisza
    int lastg = 0;
    int lastd = 0;
    bool change_kol = false;
    bool leftkey = false;
    bool rightkey = false;
    char litera;
    int rgbedit = 1;
    BITMAP* hudtemp = create_bitmap(hud->w, hud->h);
    BITMAP* tekstbufor = create_bitmap(bufor->w*1.6, bufor->h*0.8);
    blit(hud, hudtemp, 0, 0, 0, 0, hud->w, hud->h);
    const int hline = 29;
    int color[OPTNUMLIN];
    FONT* czcionki[OPTNUMLIN];
    int colorw, wr, wb, wg;
    wr = 16;
    wg = 200;
    wb = 16;
	while(!key[ KEY_ESC ] && !key[ KEY_Q ]){
        animpipe(pipe);
        masked_stretch_blit(pipe, bufor, 0, 0,
                            pipe->w, pipe->h,
                            0, 0, bufor->w, bufor->h);

        for (int i = 1; i < OPTNUMLIN-1; ++i) {
            color[i] = tempconf.colors[5];
            if ( i >= 2 && i <= 10) color[i] = tempconf.colors[i-2];
            czcionki[i] = czyt;
        }

        color[0] = tempconf.colors[6];
        czcionki[0] = norm;
        color[OPTNUMLIN-1] = tempconf.colors[6];
        czcionki[OPTNUMLIN-1] = norm;

        if(wr >= 16 && wr < 112 && wb == 16) {wr+=2; wg-=4;}
        else if(wb >= 16 && wb < 112 && wr == 112) {wb+=2; wg+=4;}
        else if(wr > 16 && wr <= 112 && wb == 112) {wr-=2; wg-=4;}
        else if(wb > 16 && wb <= 112 && wr == 16) {wb-=2; wg+=4;}
        colorw = makecol(wr,wg,wb);
        //color[kursor] = colorw;

        char teksty[OPTNUMLIN][OPTLINEL+32];
        char temptekst[OPTLINEL+32];
        for (int i = 0; i < OPTNUMLIN; ++i){
            if(i >= 2 && i <= 10){
                if(kursor != i || !change_kol )
                    sprintf(temptekst,"%s[%03d,%03d,%03d]", napisy.opt_col[i], getr(tempconf.colors[i-2]), getg(tempconf.colors[i-2]), getb(tempconf.colors[i-2]));
                else if( wg <= 32 || (wg >= 80 && wg < 128) || wg >= 176 ){
                    switch (rgbedit){
                    case 1: sprintf(temptekst, "%s[   ,%03d,%03d]", napisy.opt_col[i], getg(tempconf.colors[i-2]), getb(tempconf.colors[i-2])); break;
                    case 2: sprintf(temptekst, "%s[%03d,   ,%03d]", napisy.opt_col[i], getr(tempconf.colors[i-2]), getb(tempconf.colors[i-2])); break;
                    case 3: sprintf(temptekst, "%s[%03d,%03d,   ]", napisy.opt_col[i], getr(tempconf.colors[i-2]), getg(tempconf.colors[i-2])); break;
                    }
                }
                else sprintf(temptekst, "%s[%03d,%03d,%03d]", napisy.opt_col[i], getr(tempconf.colors[i-2]), getg(tempconf.colors[i-2]), getb(tempconf.colors[i-2]));
                strcpy(teksty[i], temptekst);
            }
            else strcpy(teksty[i], napisy.opt_col[i]);
        }

        teksty[1][0] = '\0';
        teksty[11][0] = '\0';
        teksty[12][0] = '\0';

        clear_to_color(tekstbufor, makecol(255,0,255));
        pisanie(tekstbufor, star, color, hline, teksty, czcionki);
        rectfill(tekstbufor, tekstbufor->w/2-10, kursor * hline + 15, tekstbufor->w/2+10, (kursor+1) * hline + 15, colorw);
        triangle(tekstbufor, tekstbufor->w/2-20, kursor * hline + 15 + hline/2, tekstbufor->w/2-10, kursor * hline + 15, tekstbufor->w/2-10, (kursor+1) * hline + 15, colorw);
        triangle(tekstbufor, tekstbufor->w/2+20, kursor * hline + 15 + hline/2, tekstbufor->w/2+10, kursor * hline + 15, tekstbufor->w/2+10, (kursor+1) * hline + 15, colorw);
        masked_stretch_blit(tekstbufor, bufor, 0, 0, tekstbufor->w, tekstbufor->h,
                                               0, 0, bufor->w, bufor->h);

        if( key[ KEY_ENTER ] || key[ KEY_SPACE ] ){
            if ( kursor >= 2 && kursor <= 10) {/**< jezeli kursor jest ustawiony na kolor(y) */
                while(key[ KEY_ENTER ] || key[ KEY_SPACE ])rest(50);
                if(change_kol == false){ /**< to przestaw flage na edycje koloru */
                    change_kol = true;
                    rgbedit = 1;
                }
                else change_kol = false;
            }
        }

        if(change_kol){
            hudcolor(tempconf.colors, hudtemp);
            generatehud(hudtemp, tempconf, napisy, norm, star);
            printonhud (hudtemp, bufor, (bufor->w-800)*0.5, tempconf, norm, star, 15, 789, 789*2, -500, bufor->w*0.5 - 50, 20, 123456);
            if(keypressed()){/**< nacisnieto klawisz */
                    if( key[ KEY_LEFT ] && !leftkey){
                        rgbedit--;
                        leftkey = true;
                        if(rgbedit == 0) rgbedit = 1;
                    } else leftkey = false;
                    if( key[ KEY_RIGHT ]  && !rightkey){
                        rgbedit++;
                        rightkey = true;
                        if(rgbedit == 4) rgbedit = 3;
                    } else rightkey = false;
                    int r, g, b;
                    r = getr(tempconf.colors[kursor-2]);
                    g = getg(tempconf.colors[kursor-2]);
                    b = getb(tempconf.colors[kursor-2]);
                    int *skladkol = NULL;

                    switch (rgbedit){ /**< ustalanie zmienianego kolor */
                    case 1: skladkol = &r; break;
                    case 2: skladkol = &g; break;
                    case 3: skladkol = &b; break;
                    }
                    litera = readkey();
                    litera = litera-48;/**< jesli bakspace to usun zad wielkosci */
                    if (litera == -40) *skladkol *= 0.1;
                    else if (litera >= 0 && litera <= 9){/**< jesli liczba to dodaj zad wielkosci i dopisz na koncu liczbe */
                            *skladkol = *skladkol *10 +litera;
                    }
                    if (*skladkol > 255) *skladkol = 255;
                    tempconf.colors[kursor-2] = makecol(r,g,b);
            }
        }
     else { /**< przeskoczenie elementow ktore moga zmienic miejsce kursora  */
        if( key[ KEY_UP ] ){
             if (lastg == 0){
                kursor--;
             }
             lastg++;
             lastd = 0;
        }
        if( key[ KEY_DOWN ] ){
             if (lastd == 0){
                kursor++;
             }
             lastd++;
             lastg = 0;
        }
        if ((lastg > 25 || lastd > 25 )|| (!key[ KEY_DOWN ] && !key[ KEY_UP ])){
           lastg = 0;
           lastd = 0;
        }

        switch (kursor){
        case 1: kursor = 14; break;
        case 11: kursor = 13; break;
        case 12: kursor = 10; break;
        case 15: kursor = 2; break;
        }
    }
        stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                    0, 0, SCREEN_W, SCREEN_H);
        clear_to_color( bufor, makecol( 0, 0, 0 ));
        if( key[ KEY_ENTER ] || key[ KEY_SPACE ] ){
            while(key[ KEY_ENTER ] || key[ KEY_SPACE ])rest(50);
              switch (kursor){
              case 13:
                  saveconf(tempconf);
                  blit(hudtemp, hud, 0, 0, 0, 0, hud->w, hud->h);
                  break;
              case 14:
                  tempconf = readconf();
                  blit(hud, hudtemp, 0, 0, 0, 0, hud->w, hud->h);
                  break;
              }
        }
        rest(10);
    }
    destroy_bitmap(hudtemp);
    destroy_bitmap(tekstbufor);
    while(key[ KEY_Q ]) rest(10);
    return bufor;
}

BITMAP* options_main(BITMAP* bufor, BITMAP* pipe, napi napisy, FONT * star, FONT * norm, FONT * czyt, conf tempconf, BITMAP* hud){
    int kursor = 1;
    int lastg = 0;
    int lastd = 0;
    const int hline = 30;
    int color[OPTNUMLIN];
    FONT* fonts[OPTNUMLIN];
    int colorw, wr, wb, wg;
    wr = 16;
    wg = 200;
    wb = 16;
	while(!key[ KEY_ESC ] && !key[ KEY_Q ]){
        animpipe(pipe);
        masked_stretch_blit(pipe, bufor, 0, 0,
                            pipe->w, pipe->h,
                            0, 0, bufor->w, bufor->h);

        for (int i = 1; i < OPTNUMLIN-1; ++i)
            color[i] = tempconf.colors[5];

        color[0] = tempconf.colors[6];
        color[OPTNUMLIN-1] = tempconf.colors[6];

        if(wr >= 16 && wr < 112 && wb == 16) {wr+=2; wg-=4;}
        else if(wb >= 16 && wb < 112 && wr == 112) {wb+=2; wg+=4;}
        else if(wr > 16 && wr <= 112 && wb == 112) {wr-=2; wg-=4;}
        else if(wb > 16 && wb <= 112 && wr == 16) {wb-=2; wg+=4;}
        colorw = makecol(wr,wg,wb);
        color[kursor] = colorw;

        char teksty[OPTNUMLIN][OPTLINEL+32];
        for (int i = 0; i < OPTNUMLIN; ++i){
            strcpy(teksty[i], napisy.opt0[i]);
            fonts[i] = norm;
        }

        teksty[1][0] = '\0';
        teksty[2][0] = '\0';
        teksty[5][0] = '\0';
        teksty[7][0] = '\0';
        teksty[8][0] = '\0';
        teksty[11][0] = '\0';
        teksty[14][0] = '\0';

        sprintf(teksty[4],"[%dx%d][%d]", tempconf.resx, tempconf.resy, tempconf.windowed);
        sprintf(teksty[7],"[%s]", tempconf.lang);
        sprintf(teksty[13],"[\%]");


        pisanie(bufor, star, color, hline, teksty, fonts);

        if( key[ KEY_UP ] ){
             if (lastg == 0){
                kursor--;
             }
             lastg++;
             lastd = 0;
        }
        if( key[ KEY_DOWN ] ){
             if (lastd == 0){
                kursor++;
             }
             lastd++;
             lastg = 0;
        }
        if ((lastg > 25 || lastd > 25 )|| (!key[ KEY_DOWN ] && !key[ KEY_UP ])){
           lastg = 0;
           lastd = 0;
        }
        switch (kursor){
        case -1:
        case 0:
        case 1: kursor = 3; break;
        case 2: kursor = 12; break;
        case 4: kursor = 6; break;
        case 5: kursor = 3; break;
        case 7: kursor = 9; break;
        case 8: kursor = 6; break;
        case 10: kursor = 12; break;
        case 11: kursor = 9; break;
        case 13: kursor = 3; break;
        }
        rectfill(bufor, bufor->w/2-10, kursor * hline + 15, bufor->w/2+10, (kursor+1) * hline + 15, colorw);
        triangle(bufor, bufor->w/2-20, kursor * hline + 15 + hline/2, bufor->w/2-10, kursor * hline + 15, bufor->w/2-10, (kursor+1) * hline + 15, colorw);
        triangle(bufor, bufor->w/2+20, kursor * hline + 15 + hline/2, bufor->w/2+10, kursor * hline + 15, bufor->w/2+10, (kursor+1) * hline + 15, colorw);
        //blit( bufor, screen, 0, 0, 0, 0, 800, 600 );
        stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                    0, 0, SCREEN_W, SCREEN_H);
        clear_to_color( bufor, makecol( 0, 0, 0 ));
        if( key[ KEY_ENTER ] || key[ KEY_SPACE ] ){
            while(key[ KEY_ENTER ] || key[ KEY_SPACE ])rest(50);
              switch (kursor){
              case 3:
                  bufor = options_roz( bufor, pipe, napisy, star, norm, czyt, tempconf);
                  break;
              case 6: break;
              case 9:
                  bufor = options_kol( bufor, pipe, napisy, star, norm, czyt, tempconf, hud);
                  break;
              case 12: break;
              }
              tempconf = readconf();
        }
        rest(10);
    }
    return bufor;
}

int e_wybor_poz(BITMAP* bufor, FONT* czyt, int col, int maxlevel){
    int poziom = 1;
    int restTime = 100;
    int kier = 0;
    while(!key[KEY_ENTER]){
        clear_to_color( bufor, makecol( 0, 0, 0 ));
        textprintf_centre_ex( bufor, czyt, bufor->w / 4, bufor->h/40, col, -1, "Poziom: ");
        textprintf_centre_ex( bufor, czyt, bufor->w / 4, bufor->h/40+50, col, -1, "%d", poziom);
        if(key[KEY_UP]){
            --poziom;
            if(kier == 0){
                kier = 1;
                restTime = 100;
            }else restTime-=3;
        }else if(key[KEY_DOWN]){
            ++poziom;
            if(kier == 1){
                kier = 0;
                restTime = 100;
            }else restTime-=3;
        } else restTime = 100;
        if(poziom<1) poziom = maxlevel;
        if(poziom>maxlevel) poziom = 1;
        stretch_blit(bufor, screen, 0, 0, bufor->w, bufor->h,
                                    0, 0, SCREEN_W, SCREEN_H);
        if(restTime < 15) restTime = 15;
        rest(restTime);
    }
    while(key[KEY_ENTER]) rest(10);
    return poziom;
}

void e_loadmover(mover lista[], vector<e_mov> klatkilista[MOVNUM]){ //
	 int num = 0;
     char litera;
     int znak = 0;
     e_mov tempFram;
	 while (lista[num].istnieje == true && num < MOVNUM){
            if (lista[num].conf.is_open()){
                do{
                    lista[num].conf.seekg(lista[num].num);
                    lista[num].conf.get(litera);
                    tempFram.numframe = litera - 48;
                    lista[num].conf.get(litera);
                    tempFram.x = 0;
                    for (int i = 0; i < 5; ++i){
                        tempFram.x = tempFram.x * 10 + (litera - 48);
                        lista[num].conf.get(litera);
                    }
                    tempFram.y = 0;
                    for (int i = 0; i < 5; ++i){
                        tempFram.y = tempFram.y * 10 + (litera - 48);
                        lista[num].conf.get(litera);
                    }
                    tempFram.z = litera - 48;
                    tempFram.kat = 0;
                    for (int i = 0; i < 3; ++i){
                        lista[num].conf.get(litera);
                        tempFram.kat = tempFram.kat * 10 + (litera - 48);
                    }
                    lista[num].num = lista[num].conf.tellg();
                    lista[num].conf.seekg(0, ios_base::end);
                    klatkilista[num].push_back(tempFram);
                    if (((int)lista[num].conf.tellg() - lista[num].num) < 15) lista[num].num = 0;
                }while (lista[num].num != 0);
            }
           ++num;
     }
}

void e_refreshmover(mover lista[MOVNUM], vector<e_mov> klatkilista[MOVNUM], int liczniki[MOVNUM]){ //
	 int num = 0;
    short int temp = 0;
	 while (lista[num].istnieje == true && num < MOVNUM){
           if(liczniki[num]<0)liczniki[num]+=klatkilista[num].size();
           ++liczniki[num];
           if(liczniki[num]>=klatkilista[num].size()){
                liczniki[num]=0;
           }
           lista[num].x = klatkilista[num][liczniki[num]].x;
           lista[num].y = klatkilista[num][liczniki[num]].y;
           lista[num].z = klatkilista[num][liczniki[num]].z;
           lista[num].numframe = klatkilista[num][liczniki[num]].numframe;
           lista[num].kat = klatkilista[num][liczniki[num]].kat;
           //allegro_message("%d; %d; %d",temp,lista->frame, klatkilista[num][liczniki[num]].numframe);
           ++num;
     }
}

void e_savemover(mover lista[MOVNUM], vector<e_mov> klatkilista[MOVNUM]){
    int num = MOVNUM-1;
    int i;
    while(num >= 0){
        if(lista[num].istnieje == true){
            if(lista[num].conf.is_open()){
                lista[num].conf.close();
            }
            lista[num].conf.open(lista[num].path, ios::out);
            //allegro_message("%s\n%s", lista[num].path, lista[num].img);
            if(lista[num].conf.is_open()){
                for(i=0;i<klatkilista[num].size();++i){
                    lista[num].conf.fill('0');
                    lista[num].conf.width(1);
                    lista[num].conf<<klatkilista[num][i].numframe;
                    lista[num].conf.width(5);
                    lista[num].conf<<klatkilista[num][i].x;
                    lista[num].conf.width(5);
                    lista[num].conf<<klatkilista[num][i].y;
                    lista[num].conf.width(1);
                    lista[num].conf<<klatkilista[num][i].z;
                    lista[num].conf.width(3);
                    lista[num].conf<<klatkilista[num][i].kat;
                }
                lista[num].conf.close();
            }
        }
        --num;
    }
}
