#include "inf148216_.h"

int get_usr_index(struct Server *server,int usr_id)
{
    int index_user;
    for(int i=0;i<USERS_LIMIT;i++)
    {
        if(server->users[i].id==usr_id && server->users[i].reg_status==1){
             index_user=i;
             return index_user;
        }
    }
    return -1;
    
}

int get_usr_undex_by_name(struct Server *server,char* name)
{
    int index_user;
    for(int i=0;i<USERS_LIMIT;i++)
    {
        if((strcmp(server->users[i].name,name)==0) && server->users[i].reg_status==1)
        {
             index_user=i;
             return index_user;
        }
    }
    return -1;
}

int get_room_index(struct Server *server,char* name)
{
    int index_room;
    
    for(int i=0;i<ROOMS_LIMIT;i++)
    {
        if(strcmp(name,server->rooms[i].name)==0 && server->rooms[i].curr_users_num>0)
        {   
            index_room=i;
            return i;
        }
    }
    return -1;
}

int reg(struct Server *server,struct User_msg *msgU)
{
    int index;

    if(server->curr_users_num==5)return -1;

    for(int i=0;i<USERS_LIMIT;i++)
    {
        if(server->users[i].reg_status==0)continue;

        if(strcmp(msgU->txt,server->users[i].name)==0)
            return -2;
    }

    for(int i=0;i<USERS_LIMIT;i++){
        if(server->users[i].reg_status==0){index=i;break;}
    }
    server->users[index].id=msgU->userId;
    server->users[index].reg_status=1;
    strcpy(server->users[index].name,msgU->txt);

    for(int i=0;i<CHAT_LIMIT;i++) server->users[index].private_mail[i][0]='\0'; 
    

    printf("%s zostal pomyslnie zarejestrowany jego id: %d\n",server->users[index].name,server->users[index].id);
    server->curr_users_num++;
    return 0;

}

int unreg(struct Server *server,struct User_msg *msgU)
{
    int index_user=get_usr_index(server,msgU->userId);
    server->curr_users_num--;
    server->users[index_user].reg_status=0;

    for(int i=0;i<ROOMS_LIMIT;i++)
    {
        if(server->rooms[i].users[index_user]==1)
        {
            server->rooms[i].users[index_user]=0;
            server->rooms[i].curr_users_num--;
            if(server->rooms[i].curr_users_num==0) 
            {
            server->curr_rooms_num--;
            }
        }
    }
    
    return 0;
}

void show_users_list(int msgid,struct Server *server,struct User_msg *msgU)
{   
    struct Server_msg msgS;
    msgS.userId=msgU->userId;
    msgS.r=server->curr_users_num;
    msgsnd(msgid,&msgS,sizeof(msgS),0);

    for(int i=0;i<USERS_LIMIT;i++)
    {
     if(server->users[i].reg_status==1)
     {
        msgS.userId=msgU->userId;
        strcpy(msgS.txt,server->users[i].name);
        msgsnd(msgid,&msgS,sizeof(msgS),0);
     }
    }

}

void show_rooms(int msgid,struct Server *server,struct User_msg *msgU)
{
   struct Server_msg msgS;
    msgS.userId=msgU->userId;
    msgS.r=server->curr_rooms_num;
    msgsnd(msgid,&msgS,sizeof(msgS),0);

    if(msgS.r==0) return;
    
    for(int i=0;i<ROOMS_LIMIT;i++)
    {
        if(server->rooms[i].curr_users_num>0)
        {
            strcpy(msgS.txt,server->rooms[i].name);
            msgsnd(msgid,&msgS,sizeof(msgS),0);
        }
    }
    

}

void rcv_private_msgs(int msgid,struct Server *server,struct User_msg *msgU)
{
    
    int index_user=get_usr_index(server,msgU->userId);
    
    struct Server_msg msgS;
    msgS.userId=msgU->userId;
    
    for(int i=0;i<CHAT_LIMIT;i++)
    {
        msgS.userId=msgU->userId;
        strcpy(msgS.txt,server->users[index_user].private_mail[i]);
        msgsnd(msgid,&msgS,sizeof(msgS),0);
     }
}

void send_private_msg(int msgid,struct Server *server,struct User_msg *msgU)
{
    char date[100];
    char nickname[MAX_NAME_LENGTH];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char msg[2*CHAT_MSG_LENGTH];
    

    int index_user_from=get_usr_index(server,msgU->userId);
    int index_user_to=get_usr_undex_by_name(server,msgU->txt);
    
    struct Server_msg msgS;
    struct User_msg msgU2;
    msgS.userId=msgU->userId;

    
    strftime(date, sizeof(date)-1, "<%d.%m.%Y %H:%M>[", t);
    strcpy(msg,date);
    strcat(msg,server->users[index_user_from].name);
    strcat(msg,"]: ");
    
    if(index_user_to==-1) //nie ma takiego uzytkownika
    {
        msgS.r=-1;
        msgsnd(msgid,&msgS,sizeof(msgS),0);
        return;
    }

    msgS.r=0;
    msgsnd(msgid,&msgS,sizeof(msgS),0);//przesyla wiadomosc ze wszystko jest w porzadku
    msgrcv(msgid,&msgU2,sizeof(msgU2),CHAT_SND,0);

    strcat(msg,msgU2.txt);

    for(int i=0;i<CHAT_LIMIT-1;i++) {strcpy(server->users[index_user_to].private_mail[i],server->users[index_user_to].private_mail[i+1]);}

    strcpy(server->users[index_user_to].private_mail[CHAT_LIMIT-1],msg);
}

//funkcje do pokoi

void show_users_in_room(int msgid,struct Server *server,struct User_msg *msgU)
{
    int index_room=get_room_index(server,msgU->txt);
    
    struct Server_msg msgS;
    msgS.userId=msgU->userId;
    
    if(index_room==-1) //nie ma takiego pokoju
    {
        msgS.r=-1;
        msgsnd(msgid,&msgS,sizeof(msgS),0);
        return;
    }
    msgS.r=server->rooms[index_room].curr_users_num;
    msgsnd(msgid,&msgS,sizeof(msgS),0);

    for(int i=0;i<USERS_LIMIT;i++)
    {
     if(server->rooms[index_room].users[i]==1)
     {
        msgS.userId=msgU->userId;
        strcpy(msgS.txt,server->users[i].name);
        msgsnd(msgid,&msgS,sizeof(msgS),0);
     }
    }

}

int join_room(struct Server *server,struct User_msg *msgU)
{
    int index_user=get_usr_index(server,msgU->userId);
    int index_room=get_room_index(server,msgU->txt);
    //sprawdza czy pokoj istnieje i czy uzytkownik juz nie jest w tym pokoju
    if(index_room!=-1)
    {
        if(server->rooms[index_room].users[index_user]==1) return -1; //uzytkownik juz jest w tym pokoju
        else{
            server->rooms[index_room].users[index_user]=1; 
            server->rooms[index_room].curr_users_num++;
            return 0;
            } 
    }
    else{
    //jesli nie ma pokoju sprawdza czy nie osiagnieto limitu i szuka pierwszego wolnego indeksu
    if(server->curr_rooms_num==ROOMS_LIMIT) return -2;
    
    for(int i=0;i<ROOMS_LIMIT;i++)
    {
        if(server->rooms[i].curr_users_num==0)
        {
           strcpy(server->rooms[i].name,msgU->txt);
           server->rooms[i].users[index_user]=1;
           server->rooms[i].curr_users_num++;
           server->curr_rooms_num++;
            for(int j=0;j<CHAT_LIMIT;j++)server->rooms[i].chat[j][0]='\0';
           
           return 0; 
        }
    }
    }

}

int leave_room(struct Server *server, struct User_msg *msgU)
{
    int index_user=get_usr_index(server,msgU->userId);
    int index_room=get_room_index(server,msgU->txt);
    
    if(index_room==-1)return -1; //pokoj nie istnieje

    if(server->rooms[index_room].users[index_user]==0) return -2; //nie bylo go w tym pokoju

    server->rooms[index_room].users[index_user]=0;
    server->rooms[index_room].curr_users_num--;

    if(server->rooms[index_room].curr_users_num==0){
        server->curr_rooms_num--;
    }

    return 0;
}

void show_chat_in_room(int msgid,struct Server *server,struct User_msg *msgU)
{
    int index_room=get_room_index(server,msgU->txt);
    int index_user=get_usr_index(server,msgU->userId);
    
    struct Server_msg msgS;
    msgS.userId=msgU->userId;
    
    if(index_room==-1) //nie ma takiego pokoju
    {
        msgS.r=-1;
        msgsnd(msgid,&msgS,sizeof(msgS),0);
        return;
    }
    if(server->rooms[index_room].users[index_user]==0)//uzytkownik nie jest w tym pokoju zatem nie moze wyswietlic czatu
    {
        msgS.r=-2;
        msgsnd(msgid,&msgS,sizeof(msgS),0);
        return;
    }

    msgS.r=0;
    msgsnd(msgid,&msgS,sizeof(msgS),0);//przesyla wiadomosc ze wszystko jest w porzadku

    for(int i=0;i<CHAT_LIMIT;i++)
    {
        msgS.userId=msgU->userId;
        strcpy(msgS.txt,server->rooms[index_room].chat[i]);
        msgsnd(msgid,&msgS,sizeof(msgS),0);
     }
    }
  
void send_msg(int msgid,struct Server *server,struct User_msg *msgU)
{
    char date[100];
    char nickname[MAX_NAME_LENGTH];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char msg[2*CHAT_MSG_LENGTH];
    

    int index_room=get_room_index(server,msgU->txt);
    int index_user=get_usr_index(server,msgU->userId);
    
    struct Server_msg msgS;
    struct User_msg msgU2;
    msgS.userId=msgU->userId;

    
    strftime(date, sizeof(date)-1, "<%d.%m.%Y %H:%M>[", t);
    strcpy(msg,date);
    strcat(msg,server->users[index_user].name);
    strcat(msg,"]: ");
    
    if(index_room==-1) //nie ma takiego pokoju
    {
        msgS.r=-1;
        msgsnd(msgid,&msgS,sizeof(msgS),0);
        return;
    }
    if(server->rooms[index_room].users[index_user]==0)//uzytkownik nie jest w tym pokoju zatem nie moze wyswietlic czatu
    {
        msgS.r=-2;
        msgsnd(msgid,&msgS,sizeof(msgS),0);
        return;
    }

    msgS.r=0;
    msgsnd(msgid,&msgS,sizeof(msgS),0);//przesyla wiadomosc ze wszystko jest w porzadku
    msgrcv(msgid,&msgU2,sizeof(msgU2),CHAT_SND,0);

    strcat(msg,msgU2.txt);


 

    for(int i=0;i<CHAT_LIMIT-1;i++) {strcpy(server->rooms[index_room].chat[i],server->rooms[index_room].chat[i+1]);}

    strcpy(server->rooms[index_room].chat[CHAT_LIMIT-1],msg);

}



int main(int argc,char * argv[])
{
    if(argc!=2){ printf("Niepoprawny format komendy! Sprobuj \"./server <nr_servera>\"");exit(1);}
    
    int server_id=atoi(argv[1]);
    int tmp_id=2000;
    struct Server server;
    struct User_msg msg_user;
    struct Server_msg msg_server;
    server.curr_users_num=0;
    server.curr_rooms_num=0;
    
    int msgid = msgget(server_id, IPC_CREAT | 0644); 
    for(int i=0;i<USERS_LIMIT;i++) server.users[i].reg_status=0;
    
    for(int i=0;i<ROOMS_LIMIT;i++)
    {
        server.rooms[i].curr_users_num=0;
        for(int j=0;j<USERS_LIMIT;j++) server.rooms[i].users[j]=0;
    } 

    if(msgid == -1) {
        perror("Utworzenie kolejki komunikatów");
        exit(1);
    } 

    while(1)
    {
        msgrcv(msgid, &msg_user, sizeof(msg_user) , -12, 0);
        msg_server.userId=msg_user.userId;
        printf("msg cmd : %ld\n",msg_user.cmd);
        switch (msg_user.cmd)
        {
        case REGISTER:
            msg_server.r=reg(&server,&msg_user);
            msgsnd(msgid,&msg_server,sizeof(msg_server),0);
            break;
        case SERVER_LIST:
                show_users_list(msgid,&server,&msg_user);
            break;
        case ROOMS:
            show_rooms(msgid,&server,&msg_user);
            break;
        case ROOM_LIST:
            show_users_in_room(msgid,&server,&msg_user);
            break;
        case ROOM_JOIN:
            msg_server.r=join_room(&server,&msg_user);
            msgsnd(msgid,&msg_server,sizeof(msg_server),0);
            break;
        case ROOM_LEAVE:
                msg_server.r=leave_room(&server,&msg_user);
                msgsnd(msgid,&msg_server,sizeof(msg_server),0);
            break;
        case ROOM_CHAT:
            show_chat_in_room(msgid,&server,&msg_user);
            break;
        case ROOM_SEND:
            send_msg(msgid,&server,&msg_user);
            break;
        case PRIV_SEND:
            send_private_msg(msgid,&server,&msg_user);
            break;
        case PRIV_RCV:
            rcv_private_msgs(msgid,&server,&msg_user);
            break;
        case UNREGISTER:
            msg_server.r=unreg(&server,&msg_user);
            msgsnd(msgid,&msg_server,sizeof(msg_server),0);
            break;
        case GET_ID:
            msg_user.userId=tmp_id;
            tmp_id++;
            msgsnd(msgid,&msg_user,sizeof(msg_user),0);
            break;
        default:
            printf("server nie rozpoznal komendy");
            break;
        }
    }
 return 0;   
}
