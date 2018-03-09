#include "UI.h"
#include "data_rw.h"
#include "timer.h"
#include "json.h"
#include "CMD_Ctr.h"
#include "tapdev.h"
#include "HC595N.h"
#include "uip.h"
#include "uip_arp.h"
#include "LCD.h"
#include "event_handle.h"
#include "btn_ctr.h"
#include "USART0.h"
#include "usart1.h"  //16.7.7
#include "RTC.h"
#include "button.h"
#include "data_rw.h"
#include "resolv.h"
#include "rw_buffer.h"
#include "mqtt-client.h"
#include "mqtt-data.h"
#include "exboard.h"
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#ifndef F_CPU
#define F_CPU              12000000UL
#endif
#include <util/delay.h>

	
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#define BackLightCloseTime  (300)	//N���޲������ر��� 300*2

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */


#define BACK_LIGHT_OFF()  do{PORTD |= (1 << PD4);}while(0)	//������
#define BACK_LIGHT_ON()   do{PORTD &= ~(1 << PD4);}while(0)		//�ر���


volatile unsigned int arp_count;			//10s��ʱ������
volatile unsigned int periodic_count;		//0.5s��ʱ������
volatile unsigned char arp_flag = 0;		//10s��ʱ��־λ
volatile unsigned char periodic_flag = 0;	//0.5s��ʱ��־λ
volatile unsigned int tm_count = 0;		//ʱ���ۼ���(ÿ0,5���1)

volatile unsigned int spray_count;			//���ܼ���
volatile unsigned char spray_flag;			//���ܱ�־
volatile unsigned char sch_time_flag = 0;		//���ܼƻ�ʱ�䵽���־

volatile unsigned int webclient_count;		//������ʼ�����
volatile unsigned char webclient_flag;		//������ʱ�־λ
volatile unsigned int wifi_path_count;
volatile unsigned char wifi_path_flag;

volatile unsigned int back_light_count;		//�������

volatile unsigned char button_flag = 1;		//������ʶ

unsigned char net_mode = WIFI_MODE;	//����ģʽ
volatile unsigned int visit_cloud_time = Fast;//���ʽӿ�ʱ��

volatile unsigned char work_value = 1;		//8����ģʽ
volatile unsigned char operate_value;	//4��������
volatile unsigned char spray_value = 1;	//�˸�����
volatile unsigned char sensor_value = 0;	//������ֵ

unsigned char timemode_flag; //ʱ��ģʽѡ�� 16.9.22

static unsigned int net_fail_count = 0; //����ʧ�ܴ���
//static unsigned char eth_state = ETH_DISCONN;  //16.7.6_2


/*---------------------------------------------------------------------------*/
// char serve[LenOfServe];
 char content[LenOfPath];

char devcode[LenOfDevcodes+1];	//�豸���к�
char serialcode[LenOfSerialCode+1]; //�豸�����
char connuid[LenOfConnuid+1];//������
// char is_have_sensor = 0;
unsigned char mac[6] = {0};
volatile Time current_time;
unsigned int spray_time_limit = 0;

int valves = 12; //����·��

SLink *sch_link = NULL;
SLink *spray_link = NULL;
SLink *water_used_link = NULL;

	
//���ð汾��Ϣ	
int version = 217;

void set_backlight(void)
{
	int flag = 0;
	flag = get_backlight_flag();
	if (1 == flag)
	{
		BACK_LIGHT_OFF(); 
		set_backlight_flag(0);
	}
	else
	{
		BACK_LIGHT_ON();
	}
}

void ethernet_receive(void)
{
	int i;
	if(uip_len > 0)
	{
		//eth_try = 0;
		if(BUF->type == htons(UIP_ETHTYPE_IP))		//ΪIP��  //16.7.7  dereferencing type-punned pointer will break strict-aliasing rules  
		{
			uip_arp_ipin();		//IP���ݰ�������
			uip_input();		//ʵ��Ϊuip_process��IP�����Ĵ�����
			if(uip_len > 0)
			{
				uip_arp_out();	//����ARP��
				tapdev_send();	//�����ͳ�
			}
		}
		else if(BUF->type == htons(UIP_ETHTYPE_ARP))		//ΪARP��
		{
			uip_arp_arpin();	//ARP���ݰ�������
			if(uip_len > 0)
			{
				tapdev_send();	//ARP��Ӧ
			}
		}
	}
	else if(periodic_flag == 1) //0.5�붨ʱ
	{
		periodic_flag = 0;
		for(i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);	//uip���ڴ�����
			if(uip_len > 0)		//��������Ҫ����
			{
				uip_arp_out();
				tapdev_send();
			}
		}
		#if UIP_UDP		//���֧��UDP
		for(i = 0; i < UIP_UDP_CONNS; i++)
		{
			
			uip_udp_periodic(i);
			if(uip_len > 0)
			{
				uip_arp_out();
				tapdev_send();
			}
		}
		#endif /* UIP_UDP */
		if(arp_flag == 1)	//10s��ʱ
		{
			arp_flag = 0;
			uip_arp_timer();	//Call the ARP timer function every 10 seconds.
		}
	}
}

void move_interrupts(int mode)
{
	unsigned char temp;
	
	if (0 == mode)
	{
		/* GET MCUCR*/
		temp = MCUCR;
		/* Enable change of Interrupt Vectors */
		MCUCR = temp|(1<<IVCE);
		/* Move interrupts to Start Flash section */
		MCUCR = temp & (~(1<<IVSEL));
	}
	else if(1 == mode)
	{
		/* GET MCUCR*/
		temp = MCUCR;
		/* Enable change of Interrupt Vectors */
		MCUCR = temp|(1<<IVCE);
		/* Move interrupts to Boot Flash section */
		MCUCR = temp|(1<<IVSEL);
	}
}


int main(void) 
{
	wdt_disable();
	wdt_enable(WDTO_4S);
	wdt_reset();
	
	move_interrupts(0);
 	_delay_ms(50);
	
	DDRA |= (1 << PA0);		//LED
	PORTA |= (1 << PA0);
	DDRD |= (1 << PD4);		//���� 
	
	LCD_init();	//lcd
	get_timemode_flag(&timemode_flag); // ��ȡʱ��ģʽ
	UI_Display();
	USART0_init();	//����
	usart1_init();
	buffer_init();
	get_mac(mac);	
	timer_init();	//�޸ĵĳ�ʼ��
	button_init();	//����
	
	RTC_init();
	HC595N_init();	//����
	
	uip_setethaddr(mac);		//SetMacAddress
	tapdev_init();	//��̫����ʼ��
	 
	uip_init();	//uip��ʼ��
	uip_arp_init();	//�����ʼ��
	
 	dhcpc_init(&mac, 6);	//DHCP
	
	resolv_init();		//DNS������ʼ�� //16.9.6
	resolv_query(mainserver);	//��ҳ������
	
	init_list(&sch_link);
	init_list(&spray_link);
	init_list(&water_used_link);
	read_param();
//	set_mainserve();
	TimeCtr_init();
	sensor_init();
	 while(1)
	 {		
		wdt_reset();
		
		spray_detection(spray_link);		//���ܼ��
		if (sch_time_flag)
		{
			sch_time_flag = 0;
			sch_inquire(sch_link, current_time);
		}
		if(button_detection())			//�������
		{
			back_light_count = 0;
			set_backlight();
 			updata_display_flag = UpdataDisplay;
		}
		sensor_value = get_sensor_state();
		
		TimeCtr(); 
		PushEvent();
 		btn_ctr();	//������������
		UI_Display();	//LCD��ʾ
		mqtt_read_packet(); //��ȡ������ջ������mqtt��
		mqtt_send_data();   //��Ⲣ����mqtt
		
 		if(net_mode == ETH_MODE)
 		{
			uip_len = tapdev_read();	//����̫���������ذ���
			ethernet_receive();
		}
		exboard_receive();
	}
	return 0;
}


ISR(TIMER1_COMPA_vect)	//��ʱ��1A�жϴ�����
{
	arp_count++;
	periodic_count++;
	
	webclient_count++;
	wifi_path_count++;
	
	if(arp_count >= Sec_10)
	{
		arp_flag = 1;
		arp_count = 0;
	}
	if(periodic_count >= Sec_half)
	{
		tm_count++;
		periodic_flag = 1;
		periodic_count = 0;
		
		RTC_time_get();
	}
	if(webclient_count >= Sec_1)  
	{
		webclient_flag = 1;
		webclient_count = 0;
		back_light_count++;
		if(back_light_count > BackLightCloseTime)
		{
			BACK_LIGHT_OFF();
		}
	}
}

ISR(TIMER1_COMPB_vect)	//��ʱ��1B�жϴ����������ڷ���
{
	spray_count++;
	
	if(spray_count >= Sec_1)
	{
		spray_count = 0;
		spray_flag = 1;
		if (0 == current_time.sec)
		{
			sch_time_flag = 1;
		}
	}
}

ISR(PCINT0_vect)	//�����жϴ�����
{
	button_flag = 1;
}

ISR(PCINT2_vect)	//�����жϴ�����
{
	button_flag = 2;
}


ISR(PCINT3_vect)
{
	button_flag = 3;
}

ISR(PCINT1_vect)
{
	button_flag = 4;
}

/************************************************************************/
/* ��������resolv_found													*/
/* ���ܣ���ҳ�������ļ�����												*/
/* ������nameΪ��ַ   ipaddrΪIP										*/
/* ����ֵ����															*/
/************************************************************************/
void resolv_found(char *name, u16_t *ipaddr)	//��ҳ�������ļ�����
{
    if(ipaddr != NULL) 
	{
		uip_resolv_connect(mainserver,mq_port);
	}
}

/************************************************************************/
/* ��������mqclient_connected											*/
/* ���ܣ��������ӳɹ�													*/
/* ��������																*/
/* ����ֵ����															*/
/************************************************************************/
void mqclient_connected(void)
{
	NET_LED_ON();
	LCD_show_strP(0, 20, PSTR("@"));
	net_connect_flag = NetConnected;
	net_fail_count = 0;
}

/************************************************************************/
/* ��������mqclient_timedout											*/
/* ���ܣ��������ӳ�ʱ													*/
/* ��������																*/
/* ����ֵ����															*/
/************************************************************************/
void mqclient_timedout( void )
{
	NET_LED_OFF();
	net_connect_flag = NetDisConnected;
	net_fail_count++;
	
	if(WIFI_MODE == net_mode)
	{
		set_wifi_restart();
		
		if(net_fail_count > 2)
		{
			net_fail_count = 0;
			net_mode = ETH_MODE;
		}
	}
	else if(ETH_MODE == net_mode)
	{
		set_uip_restart();             
		
		if(net_fail_count > 2)
		{
			net_fail_count = 0;
			net_mode = WIFI_MODE;
		}
	}
}

/************************************************************************/
/* ��������mqclient_datahandler										*/
/* ���ܣ��������ݴ���													*/
/* ������data�����յ�������  len�����ݳ���								*/
/* ����ֵ����															*/
/************************************************************************/
void mqclient_datahandler( char *data, u16_t len )
{
	u16_t i = 0;
	char *data_temp = data;
	
	if(len)
	{
		while(i < len)
		{
			i++;
			data_temp++;
		}
		
		*data_temp = 0;
		
		if (NULL != data)
		{
			parse_cloud_API(data, len);
		}
	}
}


void MY_UDP_APPCALL(void)
{
	switch (uip_udp_conn->rport)
	{
		case HTONS(67):
		dhcpc_appcall();
		break;
		case HTONS(68):
		dhcpc_appcall();
		break;
		case HTONS(53):
		resolv_appcall();
		break;
		default: break;
	}
}

