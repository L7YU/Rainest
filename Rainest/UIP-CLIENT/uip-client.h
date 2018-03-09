/*
 * mqtt_client.h
 *
 * Created: 2016/4/28 15:34:50
 *  Author: Administrator
 */ 


#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

#include "uipopt.h"

struct mqtt_state {
	int timer;
	char * data;
};


typedef struct mqtt_state uip_tcp_appstate_t;

#define UIP_APPCALL mqtt_appcall
void mqtt_appcall(void); 

int uip_resolv_connect(char * host,uint16_t port);
int uip_socket_connect(char * host); //��̫��socket����
void uip_client_restart(void);//uip��������
void uip_send_data(unsigned char * buf,int len);//uip��������
void mqtt_uip_newdata(uint8_t *data, uint16_t len);//uip��������



#endif /* MQTT-CLIENT_H_ */