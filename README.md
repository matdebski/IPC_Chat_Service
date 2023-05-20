# IPC_Chat_Service

Aplikacja pozwalająca na komunikację (chat) użytkowników, odbywającą się poprzez wymianę wiadomości pomiędzy nimi. Projekt składa się z 2 programów: klienta i serwera. Każdy klient może wysyłać i otrzymywać wiadomości do/od jednego z użytkowników systemu, lub do/od grupy składającej się z wielu użytkowników. W wymianie wiadomości pomiędzy klientami pośredniczy serwer.


## Przebieg komunikacji:
Komunikacja odbywa się za pomocą jednej kolejki. Po nadaniu ID(unikalne dla każdego użytkownika) użytkownicy odbierają komunikaty typu ID ze strukturą Server_msg,
a wysyłają komunikaty typu 1-13 w zależności od wykonywanego zadania (zadania opisane w **inf148216_.h**) ze strukturą User_msg. Wysyłane  przez strukture komunikaty przekazują ID użytkownika serverowi.
Dzięki temu server wie do kogo wysłać odpowiedź zwrotną.Po odebraniu odpowiedzi program ją interpretuje i wyświetla rezultat na ekranie.

Server wysyła komunikaty o typie=ID docelowego użytkownika ze strukturą Server_msg a odbiera User_msg o typu 1-13 w zależności od komendy(wyjątkiem jest komenda GET_ID gdzie server odsyła komunikat ze strukturą User_msg).
Wysłane przez strukture komunikaty  przekazują niezbędne informacje, które potem są interpretowane i wyświetlane użytkownikowi.

## Struktury
**DANE**
```c
struct User{
    	int id						- id użytkownika przydzielane jeszcze przed rejestracją 
    	int reg_status;					-jeśli uzytkownik zarejestrowany to =1 jeśli nie =0
    	char name[MAX_NAME_LENGTH]; 			- nazwa użytkownika
    char private_mail[CHAT_LIMIT][2*CHAT_MSG_LENGTH];	-wiadomości prywatne wysłane do uzytkownika
};

struct Room
{
    char name[MAX_NAME_LENGTH];				-nazwa pokoju
    int curr_users_num;					-aktualna liczba użytkowników w pokoju
    int users[USERS_LIMIT];				-tablica informująca czy użytkownik jest w pokoju(users[i]=1) czy go nie ma (users[i]=0)
    char chat[CHAT_LIMIT][2*CHAT_MSG_LENGTH]; 		-czat prowadzony w ramach pokoju
};

struct Server
{
    int curr_users_num;					-aktualna liczba zarejestrowanych użytkowników
    int curr_rooms_num;					-aktualna liczba zarejestrowanych pokoi
    struct User users[USERS_LIMIT];			-użytkownicy
    struct Room rooms[ROOMS_LIMIT];			-pokoje
};
```
**KOMUNIKATY**
```c
struct User_msg
{
    long cmd; //jaka komenda				-nr zadania
    char txt[CHAT_MSG_LENGTH];				-txt wpisany przez użytkownika
    int userId;						-id użytkownika
};

struct Server_msg
{   
    long userId; 					-do kogo server ma wysłac komunikat
    char txt[2*CHAT_MSG_LENGTH];			-odpowiedź servera
    int r; 	 					-status wykonania komendy, w zależności od komendy, ujemne wartości oznaczają błąd, 
							0- poprawne wykonanie zadania,
							wartości dodanie- ile komunikatów powiązanych z zadaniem jeszcze przyjedzie np. dla komendy SERVER_LIST
							jest to ilość uzytkowników zalogowanych na serwer.	
};							błąd może wyst
```

## Instrukcja
Kompilacja:

> gcc inf148216_k.c -o inf148216_k <br>
> gcc inf148216_s.c -o inf148216_s

### Uruchamianie:

1) Najpierw trzeba uruchomić server:
> ./inf148216_s <nr_kolejki>

np. ./inf148216_s 123

2) Następnie można już uruchamiać klientów:

> ./inf148216_k <nr_kolejki> 

np. ./inf148216_k 123

Numer kolejki powinien być taki sam.

3) Aby zakończyć prace użytkownik musi się wylogować cmd: 11

4) Aby wyłączyć server należy skorzystać z kombinacji klawiszy ctr+c
przed następnym uruchomieniem trzeba usunąć kolejke komendą:
ipcrm -q msqid lub ipcrm -a
msqid można odczytać za pomocą komendy: ipcs -q

### Opis plików:
 
inf148216_s.c - plik odpowiada za działanie servera, osługuje komunikaty od użytkownków, przechowuje informacje o wszystkich uzytkownikach i pokojach

inf148216_k.c - plik odpowiada za interfejs uzytkownika, obsługuje i interpretuje komunikaty od servera

inf148216_.h - plik przechowuje potrzebne struktury i stałe
