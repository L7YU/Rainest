/*
 * mqtt_operation.c
 *
 * Created: 2016/6/14 13:55:12
 *  Author: wjy
 */ 
#include <string.h>   // 16.7.8 implicit declaration of function ȱ��ͷ�ļ�
#include "typedef.h"
#include "event_handle.h"
#include "CMD_Ctr.h"
#include "MQTTPacket.h"
#include "mqtt-client.h"
#include "mqtt-packet.h"
#include "mqtt-unpack.h"
#include "transport.h"
#include "mqtt-data.h"
#include "wifi.h"
#include "uip-client.h"
#include <avr/wdt.h>

#define AT		0
#define MAC		1
#define WMODE	2

static unsigned char auto_restart_flag = 0;

static unsigned	int mq_connect_count = 0;
static unsigned char mq_send_buff[400] = {0};
static int mq_send_len = 0;
static unsigned  int ping_count = 0; 

static volatile unsigned char mq_send_state = MQ_START;      //mq������������״̬
static volatile unsigned char mq_send_state_temp = MQ_READY; // ���ڷ����������ʱ����ʱ���浱ǰ״̬

unsigned char mqtt_send_flag = 0;


/****************************************************************************
***                                                                       ***
***          APP���ýӿ�                                                  ***
***                                                                       ***
*****************************************************************************/
static void mq_getcmd(void)
{
	if(webclient_flag)
	{
		webclient_flag = 0;
		cmd_ctr();		// ���ͽӿ�
	}
}

static void handle_publish(unsigned char * buf,int buflen)       
{
	unsigned char * payload_in;
	int payloadlen_in = 0;
	unsigned char dup;
	int qos;
	unsigned char retained;
	unsigned short msgid;
	MQTTString receivedTopic;
	if(MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
	&payload_in, &payloadlen_in, buf, buflen) == 1)
	{
		mqclient_datahandler((char *)payload_in,payloadlen_in);  //16.7.7
	}else
	{
		check_command_flag = NeedCheckCmd;  //16.8.17
	}
}


/****************************************************************************
***                                                                       ***
***          �����豸����		                                          ***
***                                                                       ***
*****************************************************************************/
void set_wifi_restart(void)
{
	wifi_restart();
	mq_send_state = MQ_START;
	wifi_start_flag = 0;
	auto_restart_flag = 1;
}

void set_uip_restart(void)
{
	uip_client_restart();
	mq_send_state = MQ_START;
	wifi_start_flag = 0;
	auto_restart_flag = 1;
}

static void mqtt_start_wait(void)
{
	if(wifi_start_flag)
	{
		wifi_start_flag = 0;
		if(!auto_restart_flag)
		{
			mq_send_state = WIFI_AT;
		}
		else
		{
			if(net_mode == ETH_MODE)
			{
				mq_send_state =	SOCKET;
			}
			else if(net_mode == WIFI_MODE)
			{
				mq_send_state = MQ_CONNECT;
			}
		}
	}
}

static void mq_wifi_init(unsigned char flag)  //16.7.6_2 ��comparison between pointer and integer��
{
	if(webclient_flag)
	{
		webclient_flag = 0;
		if(flag == AT)
		{
			sent_wifi_at();
			if(check_baud() == 0)
			{
				mq_send_state = MQ_START;
			}else
			{
				mq_send_state = WIFI_WSMAC;
			}
			return;			
		}
		if(flag == MAC) 
		{
			if(sent_wifi_wsmac() == 1) {mq_send_state = WIFI_WMODE; return;}
		}
		if(flag == WMODE) {wifi_wmode(); mq_send_state = SOCKET; return;}
	}
}

static void mq_wifi_linkwait(void)    //16.8.3
{
	static int linkwait_count = 0;
	if(webclient_flag)
	{
		webclient_flag = 0;
		linkwait_count++;
		
		if(linkwait_count >= 20)
		{
			linkwait_count = 0;
			if(WIFI_MODE == net_mode)
			{
				set_wifi_restart();
				net_mode = WIFI_MODE;
			}
			else if(ETH_MODE == net_mode)
			{
				set_uip_restart();
				net_mode = WIFI_MODE;
			}
			auto_restart_flag = 0;
		}
	}
}
/****************************************************************************
***                                                                       ***
***           MQ���ݷ���                                                  ***
***                                                                       ***
*****************************************************************************/
static void mq_send_data(unsigned char * buff,unsigned int len)
{
	if(net_mode == WIFI_MODE)
	{
		wifi_transport_sendPacketBuffer(buff,len);
	}else if(net_mode == ETH_MODE)
	{
		eth_transport_sendPacketBuffer(buff,len);
	}
}

static void mq_socket(void)
{
	if(webclient_flag)
	{
		webclient_flag = 0;
		int res = 0;
		
		if(net_mode == WIFI_MODE)
		{
			if(wifi_transport_open()) mq_send_state = WIFI_RESTART;  //WIFIģʽ�£�����wifiģ��ͻ��Զ�����TCP������ֱ����ת��WIFI_RESTART
		}else if(net_mode == ETH_MODE)
		{
			res = eth_transport_open();  //����1����socket���ӳɹ�������-1����ʱ
			
			if(res == 1) mq_send_state = MQ_CONNECT;
			if(res == -1) mqclient_timedout();
		}
	}
}

static void mq_connect(void)
{
	if(webclient_flag)
	{
		webclient_flag = 0;
		unsigned char buff[100] = {0};
		unsigned int len = 0;
		
		len = connect_packet(buff,sizeof(buff));
		mq_send_data(buff,len);
		
		mq_connect_count++;
		
		if(mq_connect_count > 6)
		{
			mq_connect_count = 0;
			mqclient_timedout();
		}
	}
}

static void mq_subscribe(void)
{
	if(webclient_flag)
	{
		webclient_flag = 0;
		unsigned char buff[100] = {0};
		unsigned int len = 0;
		
		len = subscribe_packet(buff,sizeof(buff));
		mq_send_data(buff,len);
		
		mq_connect_count++;
		if(mq_connect_count > 6)
		{
			mq_connect_count = 0;
			mqclient_timedout();
		}
	}
}

static void mq_publish(void)
{
	if(webclient_flag)
	{
		webclient_flag = 0;
		mq_send_data(mq_send_buff,mq_send_len);
	}
}

static void mq_ping() //����ΪPINGSEND��������������ΪPINGINIT�����ʼ�������������������0��������������-1
{
	unsigned char buff[50] = {0};
	int len = 0;
		
	if(webclient_flag)
	{
		webclient_flag = 0;
			
		if(ping_count > 6)   //����6��δ�յ���Ӧ,��ΪMQ�Ͽ�
		{
			ping_count = 0;
			mqclient_timedout();
			return;
		}
		else
		{
			len = ping_packet(buff);
			mq_send_data(buff,len);
			ping_count++;   //�ȴ�pingesp��Ӧ������ÿ����1�μ�һ
			return;
		}
	}
}


void mqtt_send_data(void)
{
	switch(mq_send_state)
	{
		case MQ_START:  //��������׼����ʼ���ȴ�5�����ң�ȷ��wifi�������
		mqtt_start_wait();
		break;
			
		case WIFI_AT:   //wifiģ���ʼ��������atָ������޸Ĳ�����
		mq_wifi_init(AT);
		break;
			
		case WIFI_WSMAC: //��ȡwifi�е�code
		mq_wifi_init(MAC);
		break;
			
		case WIFI_WMODE: //�趨wifi�����繤������
		mq_wifi_init(WMODE);
		break;
			
		case SOCKET:  //wifi����̫����mqtt����������socket����
		mq_socket();
		break;
			
		case MQ_CONNECT: //��¼mqtt������
		mq_connect();
		break;
			
		case MQ_SUB:   //������ע��mqtt��Ϣ����
		mq_subscribe();
		break;
			
		case MQ_READY: //��������Ԥ��״̬�������Ҫ�ϴ�����
		mq_getcmd();
		break;
			
		case MQ_PUB:  //������Ϣ
		mq_publish();
		break;
			
		case MQ_PING://�������
		mq_ping();
		break;
			
		case WIFI_RESTART: //wifi����
		set_wifi_restart();
		break;
			
		case UIP_RESTART: //��̫������
		set_uip_restart();
		break;
		
		case WIFI_LINKWAIT:  //�ȴ�wifiģ�齨��link���� //16.8.3
		mq_wifi_linkwait();
		break;
		
		default:
		break;
	}
	wdt_reset();
}


/****************************************************************************
***                                                                       ***
***           MQ���ݽ���                                                  ***
***                                                                       ***
*****************************************************************************/

static void handle_puback(unsigned char * buf,int len)
{
	if(mq_send_state == MQ_PUB)
	{
		if(unpack_puback(buf,len))
		{
			mq_packetid++;
			mq_send_len = 0;
			memset(mq_send_buff,0,sizeof(mq_send_buff));
			mq_send_state = MQ_READY;
		}
	}
}

static void handle_pingresp(void)
{
	if(mq_send_state == MQ_PING)
	{
		mqclient_connected();
		ping_count = 0;
		mq_send_state = mq_send_state_temp;
	}
}


static void handle_connack(unsigned char * buf,int buflen)
{
	if(mq_send_state == MQ_CONNECT)
	{
		if(unpack_connack(buf,buflen))
		{
			mq_connect_count = 0;
			mq_send_state = MQ_SUB;
		}
	}
}

static void handle_suback(unsigned char * buf,int buflen)
{
	if(mq_send_state == MQ_SUB)
	{
		if(unpack_suback(buf,buflen))
		{
			mqclient_connected();
			mq_connect_count = 0;
			if(mq_send_len == 0)    //16.7.4 ��ӣ������δ����������ݣ����Ƚ��뷢��״̬
			{
				mq_send_state = MQ_READY;
			}
			else{
				mq_send_state = MQ_PUB;
			}
		}
	}
}

static void handle_errdata(void)      //16.8.17
{
	check_command_flag = NeedCheckCmd;  
}

void mqtt_read_packet(void)
{
	unsigned char buf[400] = {0};
	int buflen = sizeof(buf);
	int mqtype;
	
	mqtype = MQTTPacket_read(buf,buflen,transport_getdata); //��ȡ���绺�棬�ж���Ϣ����
	
	switch(mqtype)
	{
		case PUBLISH:       //����������Ϣ
		handle_publish(buf,buflen);//����������Ϣ
		break;
		
		case PUBACK:		//������Ϣ�ɹ�����
		handle_puback(buf,buflen);//���ͻ����ʼ������
		break;
		
		case PINGRESP:        //�������ɹ�����
		handle_pingresp();	  //���ص�ǰ״̬�������������
		break;
		
		case CONNACK:          //��¼mqtt�������ɹ�
		handle_connack(buf,buflen);//��ת��ע��mqtt����
		break;
		
		case SUBACK:			//ע��mqtt��Ϣ���гɹ�
		handle_suback(buf,buflen);//��ת����������͹���״̬
		break;
		
		case NOUARTDATA:   //16.8.17
		break;
				
		default:
		handle_errdata();   //16.8.17
		break;
	}
}


/****************************************************************************
***                                                                       ***
***           �ⲿ�ӿ�	                                                  ***
***                                                                       ***
*****************************************************************************/
//��Ҫ���͵�����ͨ��Э�鴦�������ڷ��ͻ�����
void send_mq_publish(unsigned char * payload)
{	
	mq_send_len = publish_packet(mq_send_buff,payload);	
	webclient_flag = 1;
	mq_send_state = MQ_PUB;
}

void send_mq_app_publish(unsigned char * payload)
{	
	mq_send_len = app_publish_packet(mq_send_buff,payload);
	webclient_flag = 1;
	mq_send_state = MQ_PUB;
}

int mq_send_ping(void) //��MQ��ת���������ģʽ������1:��ת�ɹ�  ����0��ʧ��  
{
	if((mq_send_state == MQ_READY) || (mq_send_state == MQ_PUB))
	{
		mq_send_state_temp = mq_send_state;
		mq_send_state = MQ_PING;
		ping_count = 0;
		return 1;
	}
	return 0;
}


void wifimac_reload(void)
{
	wifi_restart();
	auto_restart_flag = 0;
	net_mode = WIFI_MODE;
	mq_send_state = MQ_START;
}

void set_wifi_stalink(void)    //16.8.3
{
	mq_send_state = WIFI_LINKWAIT;	
	NET_LED_OFF();
	net_connect_flag = NetDisConnected;
}