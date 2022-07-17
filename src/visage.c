/*
 Visage w wersji dla C v1.0
 14.04.2004
 Tomasz Biela aka TeBe/Madteam 
 
 odczytuje pliki HIP, TIP, INP
 
& and
| or
^ xor
% reszta z dzielenia

!= rozne
|| alternatywa
&& i
 
*/

#include <stdio.h>
#include <peekpoke.h>
#include <string.h>

#define nmi    0xd40e	// NMI

#define kol712 0x02c8	// adresy komorek koloru
#define kol704 0x02c0
#define kol708 0x02c4
#define key    0x02fc

const adr1=0x6010;	// adres bufora dla pierwszej klatki
const adr2=0x9010;	// adres bufora dla drugiej klatki

const adr_gr9=0x6010;	// adresy buforow dla klatek TIP'a
const adr_gra=0x8010;
const adr_grb=0xa010;

unsigned char *header=(char*)0x9010;		// miejsce na naglowek pliku HIP, taki jak adr2

unsigned char *antic1=(char*)0x5800;		// tablice z programem ANTIC'a
unsigned char *antic2=(char*)0x5c00;

unsigned int j,a,b,c,d;
unsigned char x, y;

unsigned char temp[12];                         // bufor na tymczasowe dane
unsigned char filename[128], old_name[128];     // nazwa pliku
unsigned char *pch;

unsigned char test[][5]={".HIP",".INP",".RIP",".TIP",".CIN"};


/* LET'S GO */

void wait(void)
/* synchronizacja z ramka */
{
 unsigned char a=PEEK(0x14);
 while (PEEK(0x14)==a) { ; }
}


void blad(void)
{
 puts("I/O Error");
 memset(filename,' ',128);
}


void ustaw_kolory(void)
{
 POKE(0x02C0,0x00);					// paleta 9 kolorow
 POKE(0x02C1,0x02); POKE(0x02C2,0x04); POKE(0x02C3,0x06); POKE(0x02C4,0x08);
 POKE(0x02C5,0x0a); POKE(0x02C6,0x0c); POKE(0x02C7,0x0e); POKE(0x02C8,0x00);
}


void interlace(void)
{
/* interlace - mruganie dwoma ekranami .. ANTIC1 .. ANTIC2 .. ANTIC1 .. */
 while (!kbhit())
 {
  POKEW(0x0230,antic1); wait();
  POKEW(0x0230,antic2); wait();
 }
}


void show(void)
{
 POKE(0x22f,0x00);      // wylacz ekran (czarny)
 POKE(nmi,0x40);        // wylacz przerwanie DLI
}


void Antic_Creator_Tip(void)
/*
 ANTIC_CREATOR_TIP tworzy obraz dla formatu TIP
*/
{
// obraz #1
 a=adr_gr9; b=adr_gra+40; c=adr_grb;
 
 antic1[0]=antic2[0]=0x80;	// wywolanie przerwania DLI
 
 for (j=1; j<721; j+=12)
 {
  antic1[j]=antic1[j+3]=antic1[j+6]=antic1[j+9]=0x4f;
  antic1[j+1]=c & 0xff; antic1[j+2]=c >> 8; c+=40;
  antic1[j+4]=a & 0xff; antic1[j+5]=a >> 8; a+=80;
  antic1[j+7]=c & 0xff; antic1[j+8]=c >> 8; c+=40;
  antic1[j+10]=b & 0xff; antic1[j+11]=b >> 8; b+=80;
 }
(unsigned char *)a=antic2;
antic1[j-6]=0x41;                               // cofamy sie o dwie nadmiarowe linie, aby uniknac "skrzywienia" obrazu
antic1[j-5]=a & 0xff; antic1[j-4]=a >> 8;


// obraz #2
 a=adr_gr9+40; b=adr_gra; c=adr_grb;

 for (j=1; j<721; j+=12)
 {
  antic2[j]=antic2[j+3]=antic2[j+6]=antic2[j+9]=0x4f;
  antic2[j+1]=c & 0xff; antic2[j+2]=c >> 8; c+=40;
  antic2[j+4]=b & 0xff; antic2[j+5]=b >> 8; b+=80;
  antic2[j+7]=c & 0xff; antic2[j+8]=c >> 8; c+=40;
  antic2[j+10]=a & 0xff; antic2[j+11]=a >> 8; a+=80;
 }
(unsigned char *)a=antic1;
antic2[j-6]=0x41;
antic2[j-5]=a & 0xff; antic2[j-4]=a >> 8;
}



void Antic_Creator_INP (unsigned char gfx_mode)
/*
 obraz zaczyna sie od adresu $x010, dzieki czemu mamy 204 linie ciaglego obszaru
 ANTIC_CREATOR_INP tworzy obraz o wysokosci 200 lnii (bez przeplotu) dla formatu INP 
*/
{
// obraz #1
 a=adr1; b=adr2;
 
 antic1[0]=antic2[0]=antic1[1]=antic2[1]=0x70;          // puste linie dla centrowania
 antic1[2]=antic2[2]=0xa0;
 
 for (j=3; j<603; j+=3)
 {
   antic1[j]=antic2[j]=gfx_mode;

   antic1[j+1]=a & 0xff;
   antic1[j+2]=a >> 8;
   
   antic2[j+1]=b & 0xff;
   antic2[j+2]=b >> 8;

   a+=40; b+=40;
 }
(unsigned char *)a=antic2;
antic1[j]=0x41;
antic1[j+1]=a & 0xff; antic1[j+2]=a >> 8;

// obraz #2
(unsigned char *)a=antic1;
antic2[j]=0x41;
antic2[j+1]=a & 0xff; antic2[j+2]=a >> 8;
}


void Antic_Creator(unsigned char gfx_mode)
/*
 obraz zaczyna sie od adresu $x010, dzieki czemu mamy 204 linie ciaglego obszaru
 ANTIC_CREATOR tworzy obraz z przeplotem dla trybu HIP
*/
{
antic1[0]=antic2[0]=antic1[1]=antic2[1]=0x70;
antic1[2]=antic2[2]=0xa0;
 
// obraz #1
 a=adr1; b=adr2+40; 
 c=adr2; d=adr1+40;
 
 for (j=3; j<603; j+=6)
 {
   antic1[j]=antic1[j+3]=antic2[j]=antic2[j+3]=gfx_mode;

   antic1[j+1]=a & 0xff;
   antic1[j+2]=a >> 8;

   antic1[j+4]=b & 0xff;
   antic1[j+5]=b >> 8;

   antic2[j+1]=c & 0xff;
   antic2[j+2]=c >> 8;

   antic2[j+4]=d & 0xff;
   antic2[j+5]=d >> 8;

   a+=80; b+=80;
   c+=80; d+=80;
 }
(unsigned char *)a=antic2;
antic1[j]=0x41;
antic1[j+1]=a & 0xff; antic1[j+2]=a >> 8;

// obraz #2
(unsigned char *)a=antic1;
antic2[j]=0x41;
antic2[j+1]=a & 0xff; antic2[j+2]=a >> 8;
}


int load_HIP(void)
/*
 plik HIP ma dwa rozne formatu zapisu, oryginalny HARD'ow (16012 bajtow, 6 bajtow
 naglowka DOS'a + 8000 bajtow na klatke)
 i zapis z kolorami (16009 bajtow, po 8000 bajtow na klatke + 9 bajtow z informacja
 o kolorach
*/
{
FILE* plik = fopen(filename,"rb");

 if (plik==0) {blad(); return 0;}		// czy wystapil blad I/O		

 Antic_Creator(0x4f);			// tworz ANTIC dla grafiki HIP (z przeplotem)
 
 fread((unsigned char *)adr2,6,1,plik);		// sprawdzenie naglowka pliku
 if (header[0]==0xff && header[1]==0xff && header[2]==0x10 && header[3]==0x60)
 {
              /*****************/
              /*  HIP HARD'ow  */
              /*****************/

  fread((unsigned char *)adr1,8000,1,plik);	// 1 klatka  
  fread((unsigned char *)adr2,6,1,plik);	// omijamy 6 bajtow naglowka drugiej klatki
  fread((unsigned char *)adr2,8000,1,plik);	// 2 klatka
  fclose(plik);

  ustaw_kolory();
  asm("jsr $600");
  show();
  return 0;
 }
              /**************************/
              /*  HIP z paleta kolorow  */
              /**************************/

  fread((unsigned char *)adr2+6,7994,1,plik);	// 1 klatka
  fread((unsigned char *)adr1,8000,1,plik);	// 2 klatka
  fread((unsigned char *)kol704,9,1,plik);	// kolory
  fclose(plik);
  asm("jsr $600");
  show();
}


int load_TIP(void)
/*
 plik TIP sklada sie z 9 bajtowego naglowka
 0..2 TIP
 4..5 szerokosc w pixlach i wysokosc
 potem 3 bloki po 40*wysokosc bajtow
*/
{
unsigned int w,ofs;
FILE* plik =  fopen(filename,"rb");

 if (plik==0) {blad(); return 0;}		// czy wystapil blad I/O

 Antic_Creator_Tip();				// tworz ANTIC dla grafiki TIP

 fread((unsigned char *)adr2,9,1,plik);		// sprawdzenie naglowka pliku (adr2=header)
 if (header[0]=='T' && header[1]=='I' && header[2]=='P')
 {
  y=header[6];                  // wysokosc TIP'a
  if (y>119) {y=119;}           // maksymalna wysoksoc TIP'a to 119
  
  ofs=((119-y) >> 1)*40;        // ofset=(119-wysokosc) / 2
  w=y*40;
  
  a=adr_gr9+ofs;
  b=adr_gra+ofs;
  c=adr_grb+ofs;

  fread(a,w,1,plik);	// 1 klatka
  fread(b,w,1,plik);	// 2 klatka
  fread(c,w,1,plik);	// 3 klatka
  fclose(plik);
  
  ustaw_kolory();
  asm("jsr $603");
  show();
  return 0;
 }
// tutaj opcjonalnie komunikat "to nie jest naglowek TIP'a"
}


int load_INP()
/*
 plik INP sklada sie z 2 obrazow pod 8000 bajtow kazdy
 sa one zapisane z przeplotem
 koncowe 4 bajty pliku INP to informacja o kolorach (712,708,709,710) tak samo jak w MIC'u
*/
{
FILE* plik =  fopen(filename,"rb");

 if (plik==0) {blad(); return 0;}		// czy wystapil blad I/O

 Antic_Creator_INP(0x4e);	// tworz ANTIC dla grafiki INP

 fread((unsigned char *)adr1,8000,1,plik);	// 1 klatka
 fread((unsigned char *)adr2,8000,1,plik);	// 2 klatka
 
 fread((unsigned char *)kol712,1,1,plik);	// kolor tla
 fread((unsigned char *)kol708,3,1,plik);	// pozostale 3-y kolory

 fclose(plik);

 interlace();
}


/* M A I N */

int main (void)
{
POKE(0x0052,0x02);	                // ustaw margines

memmove((unsigned char*)&temp[0],(unsigned char*)0x022f,1);      // zapamietaj ANTIC i kolory
memmove((unsigned char*)&temp[1],(unsigned char*)0x0230,2);
memmove((unsigned char*)&temp[3],(unsigned char*)kol704,9);

puts("Visage C(6502) version 1.0");
puts("(Press Q or QUIT to exit program)");

start:

puts("");
puts("File name:"),gets(filename);		// pobierz nazwe pliku

strupr(filename);                       	// zamien na wielkie literki

if (strlen(filename)==0) {*filename=*old_name;} // jesli brak nazwy to wprowadz stara
*old_name=*filename;

if (strcmp(filename,"QUIT")==0 || strcmp(filename,"Q")==0) return 0;    // wyjscie z programu przez "quit" lub "q"

pch=strrchr(filename,'.');			// znajdz ostatnia kropke w nazwie

y=255;
for (x=0; x<5; x++)
{
 if (strcmp(pch,test[x])==0) {y=x;}     // sprawdz czy rozszerzenie pliku jest akceptowane
}

 memset((unsigned char *)adr_gr9,0x00,140*40);          // wyczysc obszary w ktorych znajduja sie klatki
 memset((unsigned char *)adr_gra,0x00,140*40);          // ale nie skasuj obszaru $bc40...
 memset((unsigned char *)adr_grb,0x00,140*40);
 POKE(key,0xff);

switch (y)
{
 case 0: load_HIP();
 	 break;

 case 1: load_INP();
 	 break;

 case 3: load_TIP();
         break;

 default: puts("Unknown file format."); break;
}

POKE(key,0xff);			// kasuje nacisniety klawisz

memmove((unsigned char*)0x22f,(unsigned char*)&temp[0],1);      // oddaj zapamietane wartosci
memmove((unsigned char*)0x230,(unsigned char*)&temp[1],2);
memmove((unsigned char*)kol704,(unsigned char*)&temp[3],9);

goto start;

return 0;
}
