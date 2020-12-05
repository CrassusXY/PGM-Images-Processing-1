#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define MAX 512            /* Maksymalny rozmiar wczytywanego obrazu */
#define DL_LINII 1024      /* Dlugosc buforow pomocniczych */

/************************************************************************************
 * Funkcja wczytuje obraz PGM z pliku do tablicy       	       	       	       	    *
 *										    *
 * \param[in] plik_we uchwyt do pliku z obrazem w formacie PGM			    *
 * \param[out] obraz_pgm tablica, do ktorej zostanie zapisany obraz		    *
 * \param[out] wymx szerokosc obrazka						    *
 * \param[out] wymy wysokosc obrazka						    *
 * \param[out] szarosci liczba odcieni szarosci					    *
 * \return liczba wczytanych pikseli						    *
 ************************************************************************************/

int czytaj(FILE *plik_we,int obraz_pgm[][MAX],int *wymx,int *wymy, int *szarosci) {
  char buf[DL_LINII];      /* bufor pomocniczy do czytania naglowka i komentarzy */
  int znak;                /* zmienna pomocnicza do czytania komentarzy */
  int koniec=0;            /* czy napotkano koniec danych w pliku */
  int i,j;

  /*Sprawdzenie czy podano prawid�owy uchwyt pliku */
  if (plik_we==NULL) {
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku\n");
    return(0);
  }

  /* Sprawdzenie "numeru magicznego" - powinien by� P2 */
  if (fgets(buf,DL_LINII,plik_we)==NULL)   /* Wczytanie pierwszej linii pliku do bufora */
    koniec=1;                              /* Nie udalo sie? Koniec danych! */

  if ( (buf[0]!='P') || (buf[1]!='2') || koniec) {  /* Czy jest magiczne "P2"? */
    fprintf(stderr,"Blad: To nie jest plik PGM\n");
    return(0);
  }

  /* Pominiecie komentarzy */
  do {
    if ((znak=fgetc(plik_we))=='#') {         /* Czy linia rozpoczyna sie od znaku '#'? */
      if (fgets(buf,DL_LINII,plik_we)==NULL)  /* Przeczytaj ja do bufora                */
	koniec=1;                   /* Zapamietaj ewentualny koniec danych */
    }  else {
      ungetc(znak,plik_we);                   /* Gdy przeczytany znak z poczatku linii */
    }                                         /* nie jest '#' zwroc go                 */
  } while (znak=='#' && !koniec);   /* Powtarzaj dopoki sa linie komentarza */
                                    /* i nie nastapil koniec danych         */

  /* Pobranie wymiarow obrazu i liczby odcieni szarosci */
  if (fscanf(plik_we,"%d %d %d",wymx,wymy,szarosci)!=3) {
    fprintf(stderr,"Blad: Brak wymiarow obrazu lub liczby stopni szarosci\n");
    return(0);
  }
  /* Pobranie obrazu i zapisanie w tablicy obraz_pgm*/
  for (i=0;i<*wymy;i++) {
    for (j=0;j<*wymx;j++) {
      if (fscanf(plik_we,"%d",&(obraz_pgm[i][j]))!=1) {
	fprintf(stderr,"Blad: Niewlasciwe wymiary obrazu\n");
	return(0);
      }
    }
  }
  return *wymx**wymy;   /* Czytanie zakonczone sukcesem    */
}                       /* Zwroc liczbe wczytanych pikseli */

int negatyw(int obraz_pgm[][MAX], int *wymx, int *wymy, int *szarosci) {
  int i, j;
  for(i=0; i<*wymy; i++){
    for(j=0; j<*wymx; j++){
      obraz_pgm[i][j] = szarosci - obraz_pgm [i][j];
    }
  }
}

int progowanie(int obraz_pgm[][MAX], int *wymx, int *wymy, int szarosci) {
    int i, j;
    int wartosc, wybor;


    printf("Wybierz opcję progowania:");
    printf("1 - Progowanie\n 2 - Progowanie czerni\n 3 - Progowanie bieli\n 4 - Powrot do menu\n");
    scanf("%d",wybor);

    switch(wybor){
      case 1:
      printf("Podaj poziom progowania:");
      scanf("%d",wartosc);
      if(wartosc>=0&&wartosc<=100){
        for (i=0; i<*wymy; i++){
            for (j=0; j<*wymx; j++){
              if(obraz_pgm[i][j]<=(szarosci*wartosc/100)){
                obraz_pgm[i][j]=0;
              }
              else{
                obraz_pgm[i][j]=szarosci;
              }
            }
        }
        return 0;
      }
      else{
        return 0;
      }

      case 2:
      printf("Podaj poziom progowania czerni:");
      scanf("%d",wartosc);
      if(wartosc>=0&&wartosc<=100){
        for (i=0; i<*wymy; i++){
            for (j=0; j<*wymx; j++){
              if(obraz_pgm[i][j]<=(szarosci*wartosc/100)){
                obraz_pgm[i][j]=0;
              }
              else{
                obraz_pgm[i][j]=obraz_pgm[i][j];
              }
            }
        }
        return 0;
      }
      else{
        return 0;
      }

      case 3:
      printf("Podaj poziom progowania bieli:");
      scanf("%d",wartosc);
      if(wartosc>=0&&wartosc<=100){
        for (i=0; i<*wymy; i++){
            for (j=0; j<*wymx; j++){
              if(obraz_pgm[i][j]<=(szarosci*wartosc/100)){
                obraz_pgm[i][j]=obraz_pgm[i][j];
              }
              else{
                obraz_pgm[i][j]=szarosci;
              }
            }
        }
        return 0;
      }
      else{
        return 0;
      }

      case 4:
      return 0;

      default:
      printf("Wybrano niewlasciwa opcje!");
    }
}

int konturowanie(int obraz_pgm[][MAX], int *wymx, int *wymy, int szarosci) {
  int i, j;
    for(i=0; i<*wymy; i++){
      for(j=0; j<*wymx; j++){
        obraz_pgm[i][j] = (abs(obraz_pgm[i+1][j]-obraz_pgm[i][j])+abs(obraz_pgm[i][j+1]-obraz_pgm[i][j]));
      }
    }
}

void wyswietl(char *n_pliku) {
  char polecenie[DL_LINII];      /* bufor pomocniczy do zestawienia polecenia */

  strcpy(polecenie,"display ");  /* konstrukcja polecenia postaci */
  strcat(polecenie,n_pliku);     /* display "nazwa_pliku" &       */
  strcat(polecenie," &");
  printf("%s\n",polecenie);      /* wydruk kontrolny polecenia */
  system(polecenie);             /* wykonanie polecenia        */
}


int main() {
  int obraz[MAX][MAX] ;
  int wymx,wymy,odcieni;

  int odczytano = 0;
  FILE *plik;
  char nazwa[100];

  int opcja;

  while (opcja!=0)
  {
  printf("Menu:\n");
  printf("1 - Wczytaj\n");
  printf("2 - Zapisz\n");
  printf("3 - Wyswietl\n");
  printf("4 - Negatyw\n");
  printf("5 - Progowanie\n"); 
  printf("6 - Konturowanie\n");
  printf("7 - Rozmycie\n");  
  printf("0.Zakoncz\n");
  printf("Twój wybór:");
  scanf("%d", &opcja);

  switch (opcja)
    {
    case 1:
      printf("Podaj nazwe pliku:\n");
      scanf("%s",nazwa);
      plik=fopen(nazwa,"r");
      if (plik != NULL) {
        odczytano = czytaj(plik,obraz,&wymx,&wymy,&odcieni);
        fclose(plik);
      }
    case 2:
    case 3:
      if (odczytano != 0)
      wyswietl(nazwa);
      return odczytano;
    case 4:
      negatyw(obraz,&wymx,&wymy,odcieni);
    case 5:
      progowanie(obraz,&wymx,&wymy,odcieni);
    case 6:
    case 7:
    case 0:
    }
  }
}
