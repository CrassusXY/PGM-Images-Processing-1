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

int zapisz(FILE *plik_wy,int obraz_pgm[][MAX],int *wymx, int *wymy, int szarosci) {
  int i, j;

  if (plik_wy==NULL){
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku.\n");
    return(0);
  }

  fprintf(plik_wy,"P2\n");

  fprintf(plik_wy,"%d %d %d ",*wymx,*wymy,szarosci);

  for(i=0;i<*wymy;i++){
    for(j=0;j<*wymx;j++){
      fprintf(plik_wy,"%d ",obraz_pgm[i][j]);
      fprintf(plik_wy,"\n");
    }
  }

  return 1;
}

void wyswietl(char *n_pliku) {
  char polecenie[DL_LINII];      /* bufor pomocniczy do zestawienia polecenia */

  strcpy(polecenie,"display ");  /* konstrukcja polecenia postaci */
  strcat(polecenie,n_pliku);     /* display "nazwa_pliku" &       */
  strcat(polecenie," &");
  printf("%s\n",polecenie);      /* wydruk kontrolny polecenia */
  system(polecenie);             /* wykonanie polecenia        */
}

int negatyw(int obraz_pgm[][MAX], int *wymx, int *wymy, int szarosci) {
  int i, j;

  for(i=0; i<*wymy; i++){
    for(j=0; j<*wymx; j++){
      obraz_pgm[i][j] = szarosci - obraz_pgm [i][j];
    }
  }
  return 0;
}

int progowanie(int obraz_pgm[][MAX], int *wymx, int *wymy, int szarosci, int *wartosc) {
    int i, j;

      for (i=0; i<*wymy; i++){
        for (j=0; j<*wymx; j++){
          if(obraz_pgm[i][j]<=(szarosci**wartosc/100)){
            obraz_pgm[i][j]=0;
          }
          else{
            obraz_pgm[i][j]=szarosci;
          }
        }
      }
    return 0;
  }

int konturowanie(int obraz_pgm[][MAX], int *wymx, int *wymy, int szarosci) {
  int i, j;

  for(i=0; i<*wymy; i++){
    for(j=0; j<*wymx; j++){
      if(i+1==*wymy){
        obraz_pgm[i][j] = abs(obraz_pgm[i][j+1]-obraz_pgm[i][j]);
      }
      if(j+1==*wymx){
        obraz_pgm[i][j] = abs(obraz_pgm[i+1][j]-obraz_pgm[i][j]);
      }
      else{
        obraz_pgm[i][j] = (abs(obraz_pgm[i+1][j]-obraz_pgm[i][j])+abs(obraz_pgm[i][j+1]-obraz_pgm[i][j]));
      }
    }
  }
  return 0;
}

int rozmycie(int obraz_pgm[][MAX], int tablica_pomocnicza[][MAX], int *wymx, int *wymy, int szarosci, int*promien) {
  int i,j;

  for(i=0; i<*wymy; i++){
    for(j=0; j<*wymx; j++){
      tablica_pomocnicza[i][j]=obraz_pgm[i][j];
    }
  }

  if(*promien==1){
    for(i=0; i<*wymy; i++){
      for(j=0; j<*wymx; j++){
        if(j==0){
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1])/2;
        }
        if(j+1==*wymx){
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j])/2;
        }
        else{
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1])/3;
        }
      }
    }
  }

  if(*promien==2){
    for(i=0; i<*wymy; i++){
      for(j=0; j<*wymx; j++){
        if(j==0){
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1]+tablica_pomocnicza[i][j+2])/3;
        }
        if(j==1){
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1]+tablica_pomocnicza[i][j+2])/4;
        }
        if(j+1==*wymx){
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-2]+tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j])/3;
        }
        if(j+2==*wymx){
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-2]+tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1])/4;
        }
        else{
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-2]+tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1]+tablica_pomocnicza[i][j+2])/5;
        }
      }
    }
  }
  return 0;
}

int main() {
  int obraz[MAX][MAX] ;
  int wymx,wymy,odcieni;
  int odczytano = 0;
  FILE *plik;
  char nazwa[100];
  int opcja,poziom,promien;
  int tablica[MAX][MAX];

  while (opcja!=8)
  {
  printf("    Menu:\n");
  printf("1 - Wczytaj\n");
  printf("2 - Zapisz\n");
  printf("3 - Wyswietl\n");
  printf("4 - Negatyw\n");
  printf("5 - Progowanie\n"); 
  printf("6 - Konturowanie\n");
  printf("7 - Rozmycie\n");  
  printf("8 - Zakoncz\n");
  printf("\nTwój wybór:");
  scanf("%d", &opcja);
  printf("\n");

  switch (opcja)
    {
    case 1:
      printf("Podaj nazwe pliku:\n");
      scanf("%s",nazwa);
      plik=fopen(nazwa,"r");
      if (plik != NULL) {
        odczytano = czytaj(plik,obraz,&wymx,&wymy,&odcieni);
        fclose(plik);
        system("clear");
        printf("Plik odczytany poprawnie\n\n");
      }
      else{
        system("clear");
        printf("Plik nie odczytany poprawnie\n\n");
      }
      break;

    case 2:
      if(odczytano != 0){
        printf("Podaj pod jaka nazwa mam zapisac plik:\n");
        scanf("%s",nazwa);
        plik=fopen(nazwa,"w");
        zapisz(plik,obraz,&wymx,&wymy,odcieni);
        fclose(plik);
        system("clear");
        printf("Zapisano\n\n");
      }
      else
      {
        system("clear");
        printf("Brak pliku do zapisania\n\n");
      }  
      break;

    case 3:
      if (odczytano != 0){
        plik=fopen("tmp.pgm","w");
        zapisz(plik,obraz,&wymx,&wymy,odcieni);
        fclose(plik);
        wyswietl("tmp.pgm");
        system("clear");
        printf("Obraz wyswietlony poprawnie\n\n");
      }
      else{
        system("clear");
        printf("Brak pliku do wyswietlenia\n\n");
      }
      break;

    case 4:
      if (odczytano != 0){
        negatyw(obraz,&wymx,&wymy,odcieni);
        system("clear");
        printf("Negatyw wykonany poprawnie\n\n");
      }
      else{
        system("clear");
        printf("Brak pliku do operacji negatywu\n\n");
      }
      break;

    case 5:
      if(odczytano != 0){
        printf("Podaj poziom progowania: ");
        scanf("%d",&poziom);
        if(poziom>=0||poziom<=100){
        progowanie(obraz,&wymx,&wymy,odcieni,&poziom);
        system("clear");
        printf("Progowanie wykonane poprawnie\n\n");
        }
        else{
          system("clear");
          printf("Bledny poziom progowania.\n\n");
        }
      }
      else{
        system("clear");
        printf("Brak pliku do progowania\n\n");
      }
      break;

    case 6:
      if (odczytano != 0){
        konturowanie(obraz,&wymx,&wymy,odcieni);
        system("clear");
        printf("Konturowanie wykonane poprawnie\n\n");
      }
      else{
        system("clear");
        printf("Brak pliku do konturowania\n\n");
      }
      break;

    case 7:
      if (odczytano != 0){
        printf("Podaj promien rozmycia(1 lub 2): ");
        scanf("%d",&promien);
        if(promien == 1 || promien == 2){
          rozmycie(obraz,tablica,&wymx,&wymy,odcieni,&promien);
          system("clear");
          printf("Konturowanie wykonane poprawnie\n\n");
        }
        else{
          system("clear");
          printf("Bledny promien\n\n");
        }
      }
      else{
        system("clear");
        printf("Brak pliku do rozmycia\n\n");
      }
      break;

    case 8:
      system("clear");
      printf ("Program zakonczyl dzialanie\n\n");
      break;
    
    default:
      system("clear");
      printf("Nie ma takiej pozycji w menu\n\n");
      break;
    }
  }
}
