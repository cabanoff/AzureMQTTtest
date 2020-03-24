/**
 * @file
 * simulator of messages from 100 sensors ##50 - 150
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <mqtt.h>
//#include <posix_sockets.h>
//#include <parse.h>

#define DEBUG_PUBLISH "[{\"key\":\"MartaRum\",\"value\":1}]"
#define MAX_JSON_SIZE 1000
#define MAX_MESSAGES 100
#define CURRENT sensorMess[messCounter]

/**
 * @brief The function saves published message
 * @param[in] message reference to received message.
 *
 * @param[in] size size of the received message.
 *
 * @returns none
 */
typedef struct{
    //char jsonStr[MAX_JSON_SIZE];    // 1 - activity1, 2 - Rum1, 3 - Chew1, 4 - Rest1
    char jsonStrAzure[MAX_JSON_SIZE]; //
    uint32_t deviceID;
    uint32_t messageID;
    uint8_t activity1;
    uint8_t activity2;
    uint8_t activity3;
    uint8_t activity4;
    uint8_t rumination1;
    uint8_t rumination2;
    uint8_t rumination3;
    uint8_t chewing1;
    uint8_t chewing2;
    uint8_t chewing3;
    uint8_t rest1;
    uint8_t rest2;
    uint8_t rest3;
    char time[20];
    char timeAzure[30];
    //uint8_t counter;
}sensorMess_t;

size_t messageSize;
sensorMess_t sensorMess[MAX_MESSAGES];           //reserv memory for 50 messages
volatile uint16_t messCounter = 0;
//volatile int waitMutex;
//char* messToSend = NULL;

/* --- PRIVATE FUNCTIONS DECLARATION ---------------------------------------- */
int getDevInfo(char*,sensorMess_t*);
//void formJsonStrings(sensorMess_t*);
void formJsonStringsAzure(sensorMess_t*);

void parse_init(void){
    //mutex initialization

    //waitMutex = 0;
}
void parse_deinit(void){
    //destroy mutex


}

 void parse_save(const char* message, size_t size)
 {
    char debMessage[MAX_JSON_SIZE];
    time_t timer;
    time(&timer);
    struct tm* tm_info = gmtime(&timer);
    struct timespec fetch_time;
    char timebuf[30];
    strftime(timebuf, 20, "%Y%m%d%H%M%S", tm_info);     /*get UTC time*/
    //pthread_mutex_lock(&mutex);
    //waitMutex = 1;
    memcpy(CURRENT.time,timebuf,20);  // save time

    /* local timestamp generation until we get accurate GPS time */
    //char fetch_timestamp[30];
    //printf("%s,\n\r",message);

    //struct tm * x;
	clock_gettime(CLOCK_REALTIME, &fetch_time);
	tm_info = gmtime(&(fetch_time.tv_sec));
    sprintf(timebuf,"%04i-%02i-%02iT%02i:%02i:%02i.%03liZ",\
    (tm_info->tm_year)+1900,(tm_info->tm_mon)+1,tm_info->tm_mday,tm_info->tm_hour,tm_info->tm_min,tm_info->tm_sec,(fetch_time.tv_nsec)/1000000); /* ISO 8601 format */
    memcpy(CURRENT.timeAzure,timebuf,30);  // save time

    char* startSensorInfo = strchr(message, ',') + 1; //next sympol after comma should be sensor message
    CURRENT.deviceID = 50 + size%100;
    CURRENT.messageID = 1 + size/100;
    uint32_t deviceInfo = getDevInfo(startSensorInfo,&CURRENT);
    if(deviceInfo == 0)return; //message is not vallid;


    //formJsonStrings(&CURRENT);
    formJsonStringsAzure(&CURRENT);
    //snprintf(debMessage,MAX_JSON_SIZE,"[{\"key\":\"MartaRum\",\"value\":1,\"datetime\":\"%s\"}]", timebuf);

    if(messCounter < (MAX_MESSAGES-1))messCounter++;
    //waitMutex = 0;
    //pthread_mutex_unlock(&mutex);
    //printf("%d %s,\n\r",messCounter,message);
 }


/**
 * @brief returns prepared message
 *
 * @param[in] message string for copy to,
 * @param[in] len - length of this string
 *
 * @returns message to be sent
 */
  char* parse_get_mess(char* message, unsigned int len)
  {

    if(messCounter){
        messCounter--;
        if(len > strlen(sensorMess[messCounter].jsonStrAzure))
        {
            memcpy(message,sensorMess[messCounter].jsonStrAzure,strlen(sensorMess[messCounter].jsonStrAzure)+1);
        }
        return(sensorMess[messCounter].jsonStrAzure);
        //return(sensorMess[messCounter].time);
    }
    return NULL;
  }

  /**
 * @brief returns prepared message for Azure
 *
 * @param[in] none
 *
 * @returns message to be sent
 */
  char* parse_get_mess_azure(void)
  {
    return(sensorMess[messCounter].jsonStrAzure);
        //return(sensorMess[messCounter].time);
  }


 /* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

 /**
 * @brief returns device ID in string
 *  first 4 bytes of message is device ID
 *  4bytes = 8 digits
 *  01000000 970F0000 00 01 9F000000 00 00 00000001000000000000000000000000
 * @param[in] message received from sensor,
 * @param[in] sensorMess struct for store sensor information,
 *
 * @returns 0 if not apropiate format of the input messsage
 */
#define IS_DIGIT(x)  ((x>='0')&&(x<='9'))||((x>='a')&&(x<='f'))||((x>='A')&&(x<='F'))


 int getDevInfo(char* message,sensorMess_t* sensorMess)
{

    /* find condition 1 byte N */

    sensorMess->chewing1 = rand()%2;
    sensorMess->rumination1 = rand()%2;
    sensorMess->rest1 = rand()%2;

    sensorMess->chewing2 = rand()%2;
    sensorMess->rumination2 = rand()%2;
    sensorMess->rest3 = rand()%2;

    sensorMess->chewing3 = rand()%2;
    sensorMess->rumination3 = rand()%2;
    sensorMess->rest3 = rand()%2;
    /*find activity 1*/

    sensorMess->activity1 = rand()%255;
    /*find activity 2*/

    sensorMess->activity2 = rand()%255;
    /*find activity 2*/

    sensorMess->activity3 = rand()%255;
    /*find activity 4*/
    sensorMess->activity4 = rand()%255;

    return 1;

}
/**
    forms json string
    {"_id":"5e1d0da752c6011229426371","temp":0,"deviceId":"2","bsId":"1","sim":false,
    "timestamp":"2019-12-26T00:47:31.662Z","sentToAzure":false,"sentToServer":false,
    "activity1":8,"activity2":30,"rumination":0,"chewing":0,"rest":1,
    "createAt":"2020-01-14T00:39:03.013Z","updateAt":"2020-01-14T00:39:03.013Z"}
*/
void formJsonStringsAzure(sensorMess_t* sensorMess)
{
    snprintf(sensorMess->jsonStrAzure,MAX_JSON_SIZE,\
    "{\"_id\":\"%d%x\",\"deviceId\":\"%d\",\"timestamp\":\"%s\",\"activity1\":%d,\
\"activity2\":%d,\"activity3\":%d,\"activity4\":%d,\"rumination\":%d,\"chewing\":%d,\"rest\":%d}",\
    sensorMess->deviceID,sensorMess->messageID,sensorMess->deviceID,sensorMess->timeAzure,sensorMess->activity1,\
    sensorMess->activity2,sensorMess->activity3,sensorMess->activity4,sensorMess->rumination1,sensorMess->chewing1,sensorMess->rest1);
}

