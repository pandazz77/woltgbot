#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <telebot.h>
#include "wol.h"

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

int main(int argc, char *argv[]){
    wol_header_t *currentWOLHeader = (wol_header_t *) malloc( sizeof( wol_header_t ));
    strncpy( currentWOLHeader->remote_addr, REMOTE_ADDR, ADDR_LEN );

    char *token;
    int users[12];
    unsigned int ucount = 0; // users count

    // parse args
    for(int i=0;i<argc;i++){
        if(!strcmp(argv[i],"-t") || !strcmp(argv[i],"--token")){
            token = argv[i+1];
        } else if (!strcmp(argv[i],"-u") || !strcmp(argv[i],"--users")){
            char *pt;
            pt = strtok(argv[i+1],",");
            while(pt!=NULL){
                users[ucount++] = atoi(pt);
                pt = strtok(NULL,",");
            }
        }
    }
    printf("Welcome to WakeOnLan bot\n");
    puts(token);
    for(int i=0;i<ucount;i++){
        printf("%d\n",users[i]);
    }

    telebot_handler_t handle;
    if (telebot_create(&handle, token) != TELEBOT_ERROR_NONE){
        printf("Telebot create failed\n");
        return -1;
    }

    telebot_user_t me;
    if (telebot_get_me(handle, &me) != TELEBOT_ERROR_NONE){
        printf("Failed to get bot information\n");
        telebot_destroy(handle);
        return -1;
    }

    printf("ID: %d\n", me.id);
    printf("First Name: %s\n", me.first_name);
    printf("User Name: %s\n", me.username);

    telebot_put_me(&me);

    int index, count, offset = -1;
    telebot_error_e ret;
    telebot_message_t message;
    telebot_update_type_e update_types[] = {TELEBOT_UPDATE_TYPE_MESSAGE};

    while (1){
        telebot_update_t *updates;
        ret = telebot_get_updates(handle, offset, 20, 0, update_types, 0, &updates, &count);
        if (ret != TELEBOT_ERROR_NONE)
            continue;
        printf("Number of updates: %d\n", count);
        for (index = 0; index < count; index++){
            message = updates[index].message;
            if (message.text){
                
                bool authorized = false;
                for(int i=0;i<ucount;i++){
                    if(users[i]==message.from->id){
                        char str[4096];
                        
                        if (strstr(message.text, "/start")){
                            snprintf(str, SIZE_OF_ARRAY(str), "Hello %s | Authorized", message.from->first_name);
                        } else if (strstr(message.text,"/wol")){
                            int sock;
                            if((sock=startupSocket())<0){
                                exit(EXIT_FAILURE);
                            }
                            while((currentWOLHeader->mac_addr = nextAddrFromArg(message.text,SIZE_OF_ARRAY(message.text)))!=NULL){
                                if(sendWOL(currentWOLHeader,sock)<0){
                                    snprintf(str,SIZE_OF_ARRAY(str),"Error occured during sending the WOL magic packet for mac address: %s ...!\n", currentWOLHeader->mac_addr->mac_addr_str );
                                    fprintf(stderr,str);
                                }
                                free(currentWOLHeader->mac_addr);
                            }
                            close(sock);
                            snprintf(str,SIZE_OF_ARRAY(str),"Magic packet has been sent.");
                        } else {
                            snprintf(str,SIZE_OF_ARRAY(str),"UNKNOWN COMMAND: `%s`",message.text);
                        }

                        ret = telebot_send_message(handle,message.chat->id,str,"HTML",false,false,updates[index].message.message_id,"");
                        authorized = true;
                        break;
                    }
                }

                if(!authorized){
                    ret = telebot_send_message(handle,message.chat->id,"You are not authorized","HTML",false,false,updates[index].message.message_id,"");
                }

                printf("%s: %s \n", message.from->first_name, message.text);

                if (ret != TELEBOT_ERROR_NONE){
                    printf("Failed to send message: %d \n", ret);
                }
            }
            offset = updates[index].update_id + 1;
        }
        telebot_put_updates(updates, count);

        sleep(1);
    }

    telebot_destroy(handle);

    return 0;
}