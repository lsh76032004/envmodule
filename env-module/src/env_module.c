#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <wiringPi.h>
#include <mcp3422.h>

#include "info.h"
#include "../include/parson.h"

#define GLOBAL "global"
#define PID		"env"
#define BUFFER_SIZE	1024

static char pid_buffer[BUFFER_SIZE];
static char cmd_buffer[BUFFER_SIZE];
static char level_buffer[BUFFER_SIZE];
//todo
enum CMD {
	HANDSHAKE = 0,
	GET_STATUS,
	ACT_BUZZER,
	ACT_LED,
};

/*****************************************************/
struct SV {
	int temp;
	int cds;
	int humi;
};

static struct SV env;
static int fd;			//Device Drive FD
static int level;		//LED BRIGHT LEVEL
static int buzzerRV;
static int ledRV;
#define LED_RATIO _IOW('c', 1, int)
#define BUZ_ON    _IO('c',2)

int calTemp(int temp) {
	double voltage, cel;
	voltage = (temp*5)/1024.;
	cel = (voltage-2.7)*(162/2.8) - 40;
	cel*=10;
	return (int)cel;
}

int calCds(int cds) {
	return (1000-(cds*1000/2048));
}

int calHumi(int humi, double temp) {
	double voltage = (humi*5)/1024.;
	double sRH = (voltage*(0.0062*5)) - 25.81;
	sRH /= (1.0546-(0.00216*temp));
	sRH*=-10;
	return (int)sRH;
}

void sensing()
{
	int cds = analogRead(400);
	int temp = analogRead(401);  // 센서로 부터 자료값을 받는다.
	int humidity = analogRead(402);
	env.temp = calTemp(temp);
	env.cds = calCds(cds);
	env.humi = calHumi(humidity, env.temp/10.);
}
/*
 *
input : full packet string(json format)
sample : {"key1","value1" : "key2","value2"}
 */
int read_json(char* json_packet)
{
	int ret = 0;
	/*Init json object*/
	JSON_Value *rootValue;
	JSON_Object *rootObject;

	rootValue = json_parse_string(json_packet);
	rootObject = json_value_get_object(rootValue);

	strncpy(pid_buffer, json_object_get_string(rootObject, "pid"), BUFFER_SIZE);
	strncpy(cmd_buffer, json_object_get_string(rootObject, "cmd"), BUFFER_SIZE);
	if(strcmp(cmd_buffer,"act_led")==0){
		strncpy(level_buffer, json_object_get_string(rootObject, "level"), BUFFER_SIZE);
		level = atoi(level_buffer);
	}
		


	if (strcmp(pid_buffer, PID) != 0 && strcmp(pid_buffer, GLOBAL) != 0) {
		return -1;
	}

	if (strcmp(cmd_buffer, "handshake") == 0) {
		ret = HANDSHAKE;
	}else if (strcmp(cmd_buffer, "get_status") == 0) {
		ret = GET_STATUS;
	}else if (strcmp(cmd_buffer, "act_buzzer") == 0) {
		ret = ACT_BUZZER;
	}else if (strcmp(cmd_buffer, "act_led") == 0) {
		ret = ACT_LED;
	}




	/*Get value function*/
	printf("[key : %s] [data : %s]\n", "pid", json_object_get_string(rootObject, "pid"));
	printf("[key : %s] [data : %s]\n", "cmd", json_object_get_string(rootObject, "cmd"));


	/*free memory*/
	json_value_free(rootValue);

	/* APPENDIX
	//Get array value 
	int i;
	JSON_Array *array = json_object_get_array(rootObject, "key#");
	for(i=0; i<json_array_get_count(array); i++)
	{
	printf("%s\n", json_array_get_string(array, i);	
	}
	 */

	return ret;
}

/*
   make json packet & print
 */
void response_handshake(char *send_buf)
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	char *buf;

	/*init empty json packet*/
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);

	sensing();
	/*add key & value*/
	json_object_set_string(rootObject, "pid", pid_buffer);
	json_object_set_string(rootObject, "cmd", cmd_buffer);
	json_object_set_string(rootObject, "module", PID);

	/*get full string of json packet */
	buf =  json_serialize_to_string(rootValue);
	strncpy(send_buf, buf, BUFFER_SIZE);
	printf("result json : %s\n", buf);

	//free memory
	json_free_serialized_string(buf);
	json_value_free(rootValue);
}

void response_status(char *send_buf)
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	char *buf;

	/*init empty json packet*/
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);

	sensing();
	/*add key & value*/
	json_object_set_string(rootObject, "pid", pid_buffer);
	json_object_set_string(rootObject, "cmd", cmd_buffer);
	if(strcmp(cmd_buffer, "get_status")==0){
		json_object_set_number(rootObject, "cds", env.cds);
		json_object_set_number(rootObject, "temp", env.temp);
		json_object_set_number(rootObject, "humi", env.humi);
	}
	else if(strcmp(cmd_buffer, "handshake")==0){
		json_object_set_string(rootObject, "module", PID);
	}
	else if(strcmp(cmd_buffer, "act_buzzer")==0){
		json_object_set_number(rootObject, "returnValue", buzzerRV );
	}
	else if(strcmp(cmd_buffer, "act_led")==0){
		json_object_set_number(rootObject, "returnValue", ledRV);
	}
	/*get full string of json packet */
	buf =  json_serialize_to_string(rootValue);
	strncpy(send_buf, buf, BUFFER_SIZE);
	printf("result json : %s\n", buf);

	//free memory
	json_free_serialized_string(buf);
	json_value_free(rootValue);
}

void env_module_init(struct info_t *info)
{

	fd = open("/dev/mydev",O_RDWR);
	if(fd == -1){
		perror("open");
		exit(-1);
	}


	printf("[%s] Env Module is initialized!!!\n", __func__);
	wiringPiSetup () ;
	mcp3422Setup (400, 0x6a, 0, 0) ;
}

void env_module(struct info_t *info)
{
	char send_buffer[BUFFER_SIZE];

	printf("[%s] RECV: %s!\n", __func__, info->receive_msg);

	int ret = read_json(info->receive_msg);
	if (ret < 0) {
		return;
	}

	switch (ret) {
		case HANDSHAKE:
			response_handshake(send_buffer);
			info->send(info,send_buffer);
			break;
		case GET_STATUS:
			response_status(send_buffer);

			info->send(info, send_buffer);
			break;
		case ACT_BUZZER:
			if(ioctl(fd,BUZ_ON)==0)
				buzzerRV = 1;
			else
				buzzerRV = 0;

			response_status(send_buffer);
			info->send(info, send_buffer);
			break;
		case ACT_LED:
			if(ioctl(fd,LED_RATIO,&level)==0)
				ledRV = 1;
			else 
				ledRV = 0;
			response_status(send_buffer);
			info->send(info, send_buffer);
			break;
		default:
			break;
	}
}
