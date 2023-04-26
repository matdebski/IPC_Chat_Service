
#include "inf148216_.h"
#include <unistd.h>

void color_print(char* txt,int color)
{
    printf("%c[1m%c[%dm %s",0x1B,0x1B,color,txt);
    printf("%c[0m",0x1B);
}

void help()
{
    printf("\nWybierz polecenie z listy:\n");
    printf("1 - pokaz instrukcje obslugi\n");
    printf("2 - wyswietl uzytkownikow zalogowanych na server \n");
    printf("3 - wyswietl dostepne pokoje\n");
    printf("4 - wyswietl uzytkownikow z wybranego pokoju\n");
    printf("5 - dolacz do wybranego pokoju\n");
    printf("6 - opusc wybrany pokoj\n");
    printf("7 - pokaz wiadomosci z czatu wybranego pokoju\n");
    printf("8 - wyslij wiadomosc na czat do wybranego pokoju\n");
    printf("9 - wyslij wiadomosc prywatna do wybrane uzytkownika\n");
    printf("10 - odbierz wiadomosci prywatne\n");
    printf("11 - wyloguj sie z servera\n\n");

}

void reg(struct User *klient,int msgid)
{

        struct Server_msg msgS;
        struct User_msg msgU;
        printf("Witaj w komunikatorze Gadu-Gadu, zacznijmy od rejestracji ;)\n");
        while(klient->reg_status==0)
        {
            color_print("Podaj nazwe uzytkownika: ",BLUE);
            scanf("%s",klient->name);
            
            strcpy(msgU.txt,klient->name);
            msgU.userId=klient->id;
            msgU.cmd=REGISTER;
            msgsnd(msgid,&msgU,sizeof(msgU),0);
            msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);

            if(msgS.r==-1){
                color_print("Server jest pelen poczekaj az zwolni sie miejsce na serverze!\n",RED);
            }
            else if(msgS.r==-2){
                color_print("Uzytkownik \"",RED);
                color_print(klient->name,RED);
                color_print("\" jest juz na serverze! Sprobuj ponownie z inna nazwa uzytkownika!\n",RED);
            }
            else{
                color_print("Gratulacje udalo ci sie pomyslnie zarejestrowac!\n",GREEN);
                klient->reg_status=1;
            }
        }

}

void unreg(struct User *klient,int msgid)
{
    struct Server_msg msgS;
    struct User_msg msgU;

    msgU.userId=klient->id;
    msgU.cmd=UNREGISTER;
    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
    exit(0);
}

void show_users_list(struct User *klient,int msgid)
{
    int size;
    struct User_msg msgU;
    struct Server_msg msgS;

    msgU.userId=klient->id;
    msgU.cmd=SERVER_LIST;

    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);

    size=msgS.r;
    printf("Aktywni uzytkownicy: \n");
    for(int i=0;i<size;i++)
    {
        msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
        printf( "-%s\n",msgS.txt);
    }
}

void show_rooms_list(struct User *klient,int msgid)
{
    int size;
    struct User_msg msgU;
    struct Server_msg msgS;

    msgU.userId=klient->id;
    msgU.cmd=ROOMS;

    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);

    size=msgS.r;
    if(size==0)
    {
        color_print("Jeszcze nie utworzono zadnych pokoi!\n",RED);
        return;
    }

    printf("Zarejestrowane pokoje: \n");
    for(int i=0;i<size;i++)
    {
        msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
        printf( "- %s\n",msgS.txt);
    }
    
}

void room_join(struct User *klient,int msgid)
{
    struct User_msg msgU;
    struct Server_msg msgS;
    msgU.userId=klient->id;
    msgU.cmd=ROOM_JOIN;

    color_print("Podaj nazwe pokoju do ktorego chcesz dolaczyc:\n",BLUE);
    scanf("%s",msgU.txt);
    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
    if(msgS.r==-1) color_print("Juz jestes w tym pokoju!\n",RED);
    else if(msgS.r==-2) color_print("Osiagnieto limit pokoi!Dolacz do istniejacego juz pokoju!\n",RED);
    else color_print("Zostales pomyslnie zapisany do wybranego pokoju!\n",GREEN);

}

void room_leave(struct User *klient,int msgid)
{
    struct User_msg msgU;
    struct Server_msg msgS;
    msgU.userId=klient->id;
    msgU.cmd=ROOM_LEAVE;
    color_print("Podaj nazwe pokoju do ktory chcesz opuscic:\n",BLUE);
    scanf("%s",msgU.txt);
    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);

    if(msgS.r==-1) color_print("Nie mozesz opuscic tego pokoju ,poniewaz on nie istnieje!\n",RED);
    else if(msgS.r==-2) color_print("Nie mozesz opuscic tego pokoju ,poniewaz w nim nie jestes!\n",RED);
    else color_print("Opusciles pokoj!\n",GREEN);
}

void show_users_in_room(struct User *klient,int msgid)
{
    int size;
    struct User_msg msgU;
    struct Server_msg msgS;
    char tmp[MAX_NAME_LENGTH];
    msgU.userId=klient->id;
    msgU.cmd=ROOM_LIST;
    color_print("Podaj nazwe pokoju: ",BLUE);
    scanf("%s",tmp);
    strcpy(msgU.txt,tmp);

    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);

    size=msgS.r;

    if(size==-1)
    {
        color_print("Nie ma takiego pokoju!\n",RED);
        return;
    }

    printf("Uzytkownicy:\n");
    for(int i=0;i<size;i++)
    {
        msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
        printf( "-%s\n",msgS.txt);
    }
}

void show_chat_in_room(struct User *klient,int msgid)
{
    struct User_msg msgU;
    struct Server_msg msgS;
    char tmp[MAX_NAME_LENGTH];
    msgU.userId=klient->id;
    msgU.cmd=ROOM_CHAT;
    color_print("Podaj nazwe pokoju: ",BLUE);
    scanf("%s",tmp);
    strcpy(msgU.txt,tmp);

    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);

    if(msgS.r==-1)
    {
        color_print("Nie ma takiego pokoju!\n",RED);
        return;
    }
    else if (msgS.r==-2)
    {
        color_print("Nie jestes zarejestrowany do tego pokoju! Nie mozesz zobaczyc jego czatu!\n",RED);
        return;
    }

    printf("Czat:\n");
    printf("##############################\n");
    for(int i=0;i<CHAT_LIMIT;i++)
    {
        msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
        printf( "%s\n",msgS.txt);
    }
    printf("##############################\n\n");
}

void send_msg(struct User *klient,int msgid)
{
    struct User_msg msgU;
    struct Server_msg msgS;
    char tmp[CHAT_MSG_LENGTH];
    msgU.userId=klient->id;
    msgU.cmd=ROOM_SEND;

    color_print("Podaj nazwe pokoju do ktorego chcesz wyslac wiadomosc: ",BLUE);
    scanf("%s",msgU.txt);

    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
    if(msgS.r==-1)
    {
        color_print("Nie ma takiego pokoju!\n",RED);
        return;
    }
    else if(msgS.r==-2)
    {
        color_print("Nie jestes zarejestrowany do tego pokoju!\n",RED);
        return;
    }

    else color_print("Twoja wiadomosc: \n",BLUE);
    scanf(" %50[^\n]",tmp);

    msgU.userId=klient->id;
    msgU.cmd=CHAT_SND;
    strcpy(msgU.txt,tmp);

    msgsnd(msgid,&msgU,sizeof(msgU),0);
    color_print("Wyslano wiadomosc!\n",GREEN);
}

void rcv_private_mail(struct User *klient,int msgid)
{
    struct User_msg msgU;
    struct Server_msg msgS;
    msgU.userId=klient->id;
    msgU.cmd=PRIV_RCV;

    msgsnd(msgid,&msgU,sizeof(msgU),0);

    printf("Wiadomosci prywatne:\n");
    printf("##############################\n");
    for(int i=0;i<CHAT_LIMIT;i++)
    {
        msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
        printf( "%s\n",msgS.txt);
    }
    printf("##############################\n\n");
}

void send_private_msg(struct User *klient,int msgid)
{
    struct User_msg msgU;
    struct Server_msg msgS;
    char tmp[CHAT_MSG_LENGTH];
    msgU.userId=klient->id;
    msgU.cmd=PRIV_SEND;

    color_print("Podaj nazwe uzytkownika do ktorego chcesz wyslac wiadomosc: ",BLUE);
    scanf("%s",msgU.txt);

    if(strcmp(msgU.txt,klient->name)==0)
    {
        color_print("Probujesz wyslac wiadomosc sam do siebie :)!\n",RED);
        return;
    }

    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgS,sizeof(msgS),klient->id,0);
    if(msgS.r==-1)
    {
        color_print("Nie ma takiego uzytkownika!\n",RED);
        return;
    }

    else color_print("Twoja wiadomosc: \n",BLUE);
    scanf(" %50[^\n]",tmp);
    
    msgU.userId=klient->id;
    msgU.cmd=CHAT_SND;
    strcpy(msgU.txt,tmp);

    msgsnd(msgid,&msgU,sizeof(msgU),0);
    color_print("Wyslano wiadomosc!\n",GREEN);
}

int main(int argc,char * argv[])
{
    if(argc!=2){ color_print("Niepoprawny format komendy! Sprobuj \"./klient <nr_servera>\"\n",RED);exit(1);}
    
    int server_id=atoi(argv[1]);
    int cmd;
    char c,cmd_s[MAX_NAME_LENGTH];
    struct User klient;   
    
    klient.reg_status=0;
    int msgid = msgget(server_id, IPC_CREAT | 0644);
    
    
    
    if(msgid == -1) {
        perror("Utworzenie kolejki komunikatów");
        exit(1);
    }
    struct User_msg msgU;
    msgU.cmd=GET_ID;
    msgsnd(msgid,&msgU,sizeof(msgU),0);
    msgrcv(msgid,&msgU,sizeof(msgU),GET_ID,0);

    klient.id=msgU.userId;    
    reg(&klient,msgid);
    help();
    while(1)
    { 
        color_print("Twoja komenda: ",BLUE);
        
        scanf(" %50[^\n]",cmd_s);
        cmd=atoi(cmd_s);
        
        switch (cmd)
        {
        case 1:
            help();
            break;
        case SERVER_LIST:
            show_users_list(&klient,msgid);
            break;
        case ROOMS:
            show_rooms_list(&klient,msgid);
        break;
            
        case ROOM_LIST:
            show_users_in_room(&klient,msgid);
            break;
        case ROOM_JOIN:
            room_join(&klient,msgid);
            break;
        case ROOM_LEAVE:
            room_leave(&klient,msgid);
            break;
        case ROOM_CHAT:
            show_chat_in_room(&klient,msgid);
            break;
        case ROOM_SEND:
            send_msg(&klient,msgid);
            break;
        case PRIV_SEND:
            send_private_msg(&klient,msgid);
            break;
        case PRIV_RCV:
            rcv_private_mail(&klient,msgid);
            break;
        case UNREGISTER:
            unreg(&klient,msgid);
            break;
        default:
            color_print("Nieznana komenda! Wybierz 1 aby wyswietlic pomoc!\n",RED);
            break;
        }
    }
        
return 0;
}
