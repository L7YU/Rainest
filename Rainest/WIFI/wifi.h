/*
 * wifi.h
 *
 * Created: 2014/6/13 16:01:01
 *  Author: ZLF
 */ 


#ifndef WIFI_H_
#define WIFI_H_

extern unsigned char  wifi_start_flag;

#define WIFI_RST_ALT()  PORTD ^= (1 << PD5)

void wifi_init(void); //wifiģ���ʼ��
void wifi_restart(void); //wifiģ������
void sent_wifi_at(void); //����wifi atָ��
int sent_wifi_wsmac(void); //����wifi code��ȡָ��
void wifi_wmode(void); // �趨wifi�������
void send_wifi_socket(char * host,uint16_t port); //����wifi socketָ��
int get_wifi_baudrate(char * baud); //��ȡwifi������
void set_wifi_baudrate(char * baud); //����wifi������
int check_baud(void);   //wifi�����ʼ��


#endif /* WIFI_H_ */