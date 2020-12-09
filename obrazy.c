#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define MAX 999           /* Maksymalny rozmiar wczytywanego obrazu */
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

/*Funkcja zapisujaca tabele do pliku PGM*/
int zapisz(FILE *plik_wy,int obraz_pgm[][MAX],int *wymx, int *wymy, int szarosci) {
  int i, j;

  if (plik_wy==NULL){
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku.\n");
    return(0);
  }

  fprintf(plik_wy,"P2\n"); /*Zapisuje migidzne "P2"*/

  fprintf(plik_wy,"%d %d %d ",*wymx,*wymy,szarosci); /*Zapisuje parametry obrazu*/

  for(i=0;i<*wymy;i++){
    for(j=0;j<*wymx;j++){
      fprintf(plik_wy,"%d ",obraz_pgm[i][j]); /*Wypisuje wartosci tabeli*/
      fprintf(plik_wy,"\n");
    }
  }

  return 0; /*Zapisz zakonczone sukcesem*/
}

void czysc(){
  system("clear");
}

/*Funkcja wyswietlajaca plik tymczasowy*/
void wyswietl(char *n_pliku) {
  char polecenie[DL_LINII];      /* bufor pomocniczy do zestawienia polecenia */

  strcpy(polecenie,"display ");  /* konstrukcja polecenia postaci */
  strcat(polecenie,n_pliku);     /* display "nazwa_pliku" &       */
  strcat(polecenie," &");
  printf("%s\n",polecenie);      /* wydruk kontrolny polecenia */
  system(polecenie);             /* wykonanie polecenia        */
}

/*Funkcja tworzaca negatyw*/
int negatyw(int obraz_pgm[][MAX], int *wymx, int *wymy, int szarosci) {
  int i, j;

  for(i=0; i<*wymy; i++){
    for(j=0; j<*wymx; j++){
      obraz_pgm[i][j] = szarosci - obraz_pgm [i][j]; /*Zmieniam wartosci elementow tablicy na znajdujace sie po przecinwej stronie skali szarosci*/
    }
  }
  return 0;
}

/*Fukcja wykonujaca progowanie*/
int progowanie(int obraz_pgm[][MAX], int *wymx, int *wymy, int szarosci, int *wartosc) {
    int i, j;

      for (i=0; i<*wymy; i++){
        for (j=0; j<*wymx; j++){
          if(obraz_pgm[i][j]<=(szarosci**wartosc/100)){ /*Sprawdzam czy element tabeli jest mniejszy badz rowny od progu*/
            obraz_pgm[i][j]=0; /*Jesli tak, to przypisuje mu wartosc 0*/
          }
          else{
            obraz_pgm[i][j]=szarosci; /*Jesli nie, to przypisuje mu maksymalna wartosc szarosci*/
          }
        }
      }
    return 0;
  }

/*Funkjca tworzaca kontur obrazu*/
int konturowanie(int obraz_pgm[][MAX], int *wymx, int *wymy, int szarosci) {
  int i, j;

  for(i=0; i<*wymy; i++){
    for(j=0; j<*wymx; j++){
      if(i+1==*wymy){ /*Specjalny przypadek zapobiegajacy wczytaniu elementu nienalezacego do tablicy poza wymy*/
        obraz_pgm[i][j] = abs(obraz_pgm[i][j+1]-obraz_pgm[i][j]);
      }
      if(j+1==*wymx){ /*Specjalny przypadek zapobiegajacy wczytaniu elementu nienalezacego do tablicy poza wymx*/
        obraz_pgm[i][j] = abs(obraz_pgm[i+1][j]-obraz_pgm[i][j]);
      }
      else{ /*Pozostale przypadki*/
        obraz_pgm[i][j] = (abs(obraz_pgm[i+1][j]-obraz_pgm[i][j])+abs(obraz_pgm[i][j+1]-obraz_pgm[i][j]));
      }
    }
  }
  return 0;
}

/*Funkcja rozmywajaca obraz*/
int rozmycie(int obraz_pgm[][MAX], int tablica_pomocnicza[][MAX], int *wymx, int *wymy, int szarosci, int*promien) {
  int i,j;

  for(i=0; i<*wymy; i++){
    for(j=0; j<*wymx; j++){ 
      tablica_pomocnicza[i][j]=obraz_pgm[i][j]; /*Przypisuje do elementow tablicy pomocniczej wartosci odpowiadajacych elementow tablicy zawierajacej obraz*/
    }
  }

  if(*promien==1){ /*Przypadek, kiedy promien rozmycia wynosi 1*/
    for(i=0; i<*wymy; i++){
      for(j=0; j<*wymx; j++){
        if(j==0){ /*Przypadek poczatku wiersza, zapobiegajacy pobraniu nieistniejacych wartosci*/
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1])/2;
        }
        if(j+1==*wymx){ /*Przypadek konca wiersza, zapobiegajacy pobraniu nieistniejacych wartosci*/
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j])/2;
        }
        else{ /*Normalny przypadek. obliczam srednia z trzech sasiadujacych elementow tablicy pomocniczej i przypisuje ja do tablicy zawierajacej obraz*/
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1])/3;
        }
      }
    }
  }

  if(*promien==2){ /*Przypadek, kiedy promien rozmycia wynosi 2*/
    for(i=0; i<*wymy; i++){
      for(j=0; j<*wymx; j++){
        if(j==0){ /*Pierwszy przypadek poczatku wiersza, zapobiegajacy pobraniu nieistniejacych wartosci*/
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1]+tablica_pomocnicza[i][j+2])/3;
        }
        if(j==1){ /*Drugi przypadek poczatku wiersza, zapobiegajacy pobraniu nieistniejacych wartosci*/
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1]+tablica_pomocnicza[i][j+2])/4;
        }
        if(j+1==*wymx){ /*Pierwszy przypadek konca wiersza, zapobiegajacy pobraniu nieistniejacych wartosci*/
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-2]+tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j])/3;
        }
        if(j+2==*wymx){ /*Drugi przypadek konca wiersza, zapobiegajacy pobraniu nieistniejacych wartosci*/
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-2]+tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1])/4;
        }
        else{ /*Normalny przypadek. obliczam srednia z pieciu sasiadujacych elementow tablicy pomocniczej i przypisuje ja do tablicy zawierajacej obraz*/
          obraz_pgm[i][j]=(tablica_pomocnicza[i][j-2]+tablica_pomocnicza[i][j-1]+tablica_pomocnicza[i][j]+tablica_pomocnicza[i][j+1]+tablica_pomocnicza[i][j+2])/5;
        }
      }
    }
  }
  return 0;
}

int main() {
  int obraz[MAX][MAX]; /*Tablica do ktorej zostanie wczytany obraz*/
  int wymx,wymy,odcieni; /*Paremetry obrazu. Jego wymiary i skala szarosci*/
  int odczytano = 0; /*int pomagajacy w sprawdzeniu, czy wczytano poprawnie obraz do tablicy*/
  FILE *plik;
  char nazwa[100]; /*char pomagajacy przy operacji zapisywania i wczytywania*/
  int opcja; /*int uzywany przy wyborze opcji w menu*/
  int poziom; /*int z poziomem w operacji progowania*/
  int promien; /*int z promieniem w operacji rozmycia*/
  int tablica[MAX][MAX]; /*Tablica pomocnicza przy operacji rozmywania*/

  while (opcja!=8) /*Menu*/
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
    case 1: /*Opcja wywolujaca wczytania pliku*/
      printf("Podaj nazwe pliku:\n");
      scanf("%s",nazwa); /*Pobieram nazwe pliku do wczytania*/
      plik=fopen(nazwa,"r"); /*Otwieram ten plik w trybie do czytania*/
      if (plik != NULL) { /*Sprawdzam, czy plik jest prawidlowy*/
        odczytano = czytaj(plik,obraz,&wymx,&wymy,&odcieni); /*Wywoluje funkcje czytaj oraz przypisuje jej wynik, do odczytano*/
        fclose(plik); /*Zamykam plik*/
        czysc(); /*Czyszczenie*/
        printf("Plik odczytany poprawnie\n\n");
      }
      else{ /*Sytuacja, gdy plik nie jest poprawny*/
        czysc(); 
        printf("Plik nie odczytany poprawnie\n\n");
      }
      break;

    case 2:
      if(odczytano != 0){ /*Sprawdzam, czy jest wczytany plik*/
        printf("Podaj pod jaka nazwa mam zapisac plik:\n"); /*Sytacja, gdy plik jest wczytany*/
        scanf("%s",nazwa); /*Wczytuje nazwe pod jaka zapisac plik*/
        plik=fopen(nazwa,"w"); /*Otwieram plik do trybie zapisu*/
        zapisz(plik,obraz,&wymx,&wymy,odcieni); /*Wywoluje funkcje zapisz*/
        fclose(plik); /*Zamykam plik*/
        czysc();
        printf("Zapisano\n\n");
      }
      else
      {
        czysc(); /*Sytacja, gdy plik nie jest wczytany*/
        printf("Brak pliku do zapisania\n\n");
      }  
      break;

    case 3:
      if (odczytano != 0){ /*Sprawdzam, czy jest wczytany plik*/
        plik=fopen("tmp.pgm","w"); /*Otwieram plik tymczasowy do zapisu*/
        zapisz(plik,obraz,&wymx,&wymy,odcieni); /*Zapisuje go*/
        fclose(plik); /*zamykam go*/
        wyswietl("tmp.pgm"); /*Wywoluje funcje zapisz*/
        czysc();
        printf("Obraz wyswietlony poprawnie\n\n");
      }
      else{ /*Sytuacja, gdy plik nie jest wczytany*/
        czysc();
        printf("Brak pliku do wyswietlenia\n\n");
      }
      break;

    case 4:
      if (odczytano != 0){ /*Sprawdzam, czy jest wczytany plik*/
        negatyw(obraz,&wymx,&wymy,odcieni);
        czysc();
        printf("Negatyw wykonany poprawnie\n\n");
      }
      else{ /*Sytuacja, gdy plik nie jest wczytany*/
        czysc();
        printf("Brak pliku do operacji negatywu\n\n");
      }
      break;

    case 5:
      if(odczytano != 0){ /*Sprawdzam, czy jest wczytany plik*/
        printf("Podaj poziom progowania: ");
        scanf("%d",&poziom); /*Wczytuje poziom od jakiego ma byc progowanie*/
        if(poziom>=0||poziom<=100){ /*Sprawdzam, czy poziom progowania jest poprawny*/
        progowanie(obraz,&wymx,&wymy,odcieni,&poziom); /*Wywoluje operacje progowania*/
        czysc();
        printf("Progowanie wykonane poprawnie\n\n");
        }
        else{ /*Sytuacja, gdy uzytkownik podal niewlasciwy poziom progowania*/
          czysc();
          printf("Bledny poziom progowania.\n\n");
        }
      }
      else{ /*Sytuacja, gdy plik nie jest wczytany*/
        czysc();
        printf("Brak pliku do progowania\n\n");
      }
      break;

    case 6:
      if (odczytano != 0){ /*Sprawdzam, czy jest wczytany plik*/
        konturowanie(obraz,&wymx,&wymy,odcieni); /*Wywoluje operacje konturowania*/
        czysc();
        printf("Konturowanie wykonane poprawnie\n\n");
      }
      else{ /*Sytuacja, gdy plik nie jest wczytany*/
        czysc();
        printf("Brak pliku do konturowania\n\n");
      }
      break;

    case 7:
      if (odczytano != 0){ /*Sprawdzam, czy jest wczytany plik*/
        printf("Podaj promien rozmycia(1 lub 2): ");
        scanf("%d",&promien); /*Wczytuje promien rozmycia*/
        if(promien == 1 || promien == 2){ /*Sprawdzam promien rozmycia*/
          rozmycie(obraz,tablica,&wymx,&wymy,odcieni,&promien); /*Wywoluje operacje rozmycia*/
          czysc();
          printf("Konturowanie wykonane poprawnie\n\n");
        }
        else{ /*Sytaucja. gdy uzytkownik podal bledny promien*/
          czysc();
          printf("Bledny promien\n\n");
        }
      }
      else{ /*Sytuacja, gdy plik nie jest wczytany*/
        czysc();
        printf("Brak pliku do rozmycia\n\n");
      }
      break;

    case 8: /*Opcja konczaca dzialanie programu*/
      czysc();
      printf ("Program zakonczyl dzialanie\n\n");
      break;
    
    default: /*Ostrzezenie o wybraniu opcji spoza zdefiniowanej w menu*/
      czysc(); 
      printf("Nie ma takiej pozycji w menu\n\n");
      break;
    }
  }
}


/*Lukasz Walczak 259278




*/
