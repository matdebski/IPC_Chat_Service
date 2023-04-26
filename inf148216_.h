#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

//limity
#define ROOMS_LIMIT 5
#define USERS_LIMIT 5 
#define MAX_NAME_LENGTH 32
#define CHAT_MSG_LENGTH 50
#define CHAT_LIMIT 10 //limit wiadomosci na czacie

//komendy
#define REGISTER 1 //rejestracja
#define SERVER_LIST 2 //lista uzytkownikow
#define ROOMS 3 //lista pokoi
#define ROOM_LIST 4 //lista uzytownikow pokoju
#define ROOM_JOIN 5 //zapisanie do pokoju
#define ROOM_LEAVE 6 //wypisanie z pokoju
#define ROOM_CHAT 7 //pokazuje 10 ostatnich wiadomosci na kanale
#define ROOM_SEND 8 //wyslanie wiadomosci
#define PRIV_SEND 9//wyslanie prywatnej wiadomosci
#define PRIV_RCV 10//odbiera prywatne wiadomosci
#define UNREGISTER 11 //wylogwanie z servera
#define GET_ID 12
#define CHAT_SND 13



//Dane
struct User{
  
    int id;
    int reg_status;
    char name[MAX_NAME_LENGTH];
    char private_mail[CHAT_LIMIT][2*CHAT_MSG_LENGTH];
};

struct Room
{
    char name[MAX_NAME_LENGTH];
    int curr_users_num;
    int users[USERS_LIMIT];
    char chat[CHAT_LIMIT][2*CHAT_MSG_LENGTH]; 
};

struct Server
{
    int curr_users_num;
    int curr_rooms_num;
    struct User users[USERS_LIMIT];
    struct Room rooms[ROOMS_LIMIT];
};

//komunikaty
struct User_msg
{
    long cmd; //jaka komenda
    char txt[CHAT_MSG_LENGTH];
    int userId;
};

struct Server_msg
{   
    long userId; //do kogo wyslac komunikat
    char txt[2*CHAT_MSG_LENGTH];
    int r; // odpowiedz servera w zaleznosci od komendy wartosci ujemne kody bledow ,wartosc dodatnia 
            //to informacja ile komunikatow powiazanych z biezancym zadaniem jeszcze przyjdzie
            //zero oznacza poprawne wykonanie zadania
    
};

#define RED 31
#define GREEN 32
#define BLUE 36

