/*
 * _74HC595N.c
 *
 * Created: 2014/5/14 13:11:06
 *  Author: ZLF
 */

#include "schedule_execute.h"
#include "HC595N.h"
#include "event_handle.h"
#include "data_rw.h"
#include "exboard.h"
#include <avr/io.h>

#ifndef F_CPU
#define F_CPU              12000000UL
#endif
#include <util/delay.h>

extern volatile unsigned int spray_count;
extern volatile unsigned char spray_flag;			//���ܱ�־
extern DevParam devparam;//�豸����
extern SLink * water_used_link;

long long spray_state = 0;		//����״̬                //16.7.5 Ϊ�˿��Ա�ʾ36·���ϵķ��ţ��˴��������޸�Ϊlong long
long long spray_queue_state = 0;		//�Ŷ�״̬
long long spray_array_state = 0; 
long long spray_array_state_temp = 0;

/************************************************************************/
/* ��������HC595N_write													*/
/* ���ܣ�����ֱ�ӿ��ƣ�����0x01����1·����2~8·��0x80����8·����1~7·	*/
/* ������data  ֱ�ӿ��Ʋ���												*/
/* ����ֵ����															*/
/************************************************************************/
void HC595N_write(unsigned int spray_state)
{		
	PORTC |= (1 << PC6);
	
	for(int i = 0;i < 16;i++)
	{
		if(spray_state & 0x8000)
		{
			PORTC |= (1 << HC595_DAT);
		}
		else
		{
			PORTC &= ~(1 << HC595_DAT);
		}
		_delay_us(2);
		PORTC |= (1 << HC595_SH);
		_delay_us(2);
		PORTC &= ~(1 << HC595_SH);
		spray_state = spray_state << 1;
	}
	PORTB |= (1 << HC595_ST);
	_delay_us(2);
	PORTB &= ~(1 << HC595_ST);
	
	PORTC &= ~(1 << PC6);
}

void valve_open(SLink *L)
{
// 	unsigned int spray_state = 0;
	
// 	if (zone > 0)
// 	{
// 		if (0 != devparam.mastertag)
// 		{
// 			spray_state |= (1 << 11);
// 		}
// 		
// 		if (zone < 13)
// 		{
// 			spray_state |= (1 << (zone - 1));
// 		}
// 		else
// 		{
// 			_delay_ms(5);
// 			exboard_open(zone - 12);
// 		}
// 		HC595N_write(spray_state);
// 	}
	
	
	int i = 0;
	unsigned int spray_state = 0;
	SLink *p = NULL;
	pSpray pspr = NULL;
	uint8_t valves[10] = {0};
	int len = 0;
	
	p = L->next;
	
	while((NULL != p) && (i < devparam.mwspray))
	{
		pspr = (pSpray)(p->data);
		
		if (0 != devparam.mastertag)
		{
			spray_state |= (1 << 11);
		}

		if (pspr->zone < 13)
		{
			spray_state |= (1 << (pspr->zone - 1));
		}
		else
		{
// 			_delay_ms(5);
// 			exboard_open(pspr->zone - 12);
			valves[len] = pspr->zone - 12;
			len++;
		}
		
		p = p->next;
		i++;
	}
	HC595N_write(spray_state);
	if (len > 0)
	{
		exboard_open(&valves[0], len);
	}
}

void valve_close(unsigned char zone)
{
	HC595N_write(0);
	
	if (zone > 12)
	{
		_delay_ms(5);
		exboard_close(zone - 12);
	}	
}

/************************************************************************/
/* ��������HC595N_init													*/
/* ���ܣ����ų�ʼ��														*/
/* ��������																*/
/* ����ֵ����															*/
/************************************************************************/
void HC595N_init(void)
{
	DDRB |= (1 << HC595_EN1)|(1 << HC595_ST);
	DDRC |= (1 << HC595_DAT)|(1 << HC595_SH);
	PORTB &= ~(1 << HC595_EN1)|(1 << HC595_ST);	//�������
	PORTC &= ~(1 << HC595_DAT)|(1 << HC595_SH);
	HC595N_write(0);
}



/************************************************************************/
/* ��������spray_open													*/
/* ���ܣ���·���Ŵ򿪶�������											*/
/* ������number  ��number·   second  ���ܵ�ʱ�䣬��s��					*/
/* ����ֵ����															*/
/************************************************************************/
void spray_open(void)
{
	spray_count = 0;
// 	TIMSK1 |= (1 << OCIE1B);
}

/************************************************************************/
/* ��������spray_close													*/
/* ���ܣ���·���Źرն�������											*/
/* ������number  ��number·��											*/
/* ����ֵ����															*/
/************************************************************************/
void spray_close_all(SLink *L)
{
// 	ClearLink(L);
	
	SLink *p = NULL;
	pSpray pspr = NULL;
	
	p = L->next;
	
	while(NULL != p)
	{
		pspr = (pSpray)(p->data);
		pspr->work_status = ExeCompleted;
		p = p->next;
	}

// 	spray_count = 0;
// 	HC595N_write(0, 0);
}

void spray_close_zone(SLink *L, uchar zone)
{
	int i = 0;
	SLink *p;
	pSpray pspr = NULL;
	
	i = locate_spray(L,zone);
	if (i)
	{
		p = get_addr(L,i);
		if (NULL != p)
		{
			pspr = (pSpray)(p->data);
			pspr->work_status = ExeCompleted;
		}
	}
}


/************************************************************************/
/* ��������bit_count													*/
/* ���ܣ�ͳ�ƶ�������1�ĸ���											*/
/* ������n  ����Ķ�������												*/
/* ����ֵ�� c ����n��1�ĸ���											*/
/************************************************************************/
int bit_count(unsigned int n)
{
	unsigned int c =0 ;
	for (c =0; n; ++c)
	{
		n &= (n -1) ; // ������λ��1
	}
	return c ;
}

long long get_spray_state(SLink *L)   //16.7.5 �޸���������Ϊlong long
{
	long long spray_state = 0;
	SLink *p = NULL;
	Spray spr;
	
	p = L->next;
	if(NULL != p)
	{
		spr = *(pSpray)(p->data);
		spray_state |= (long long)1 << (spr.zone - 1);
	}
	return spray_state;
}

long long get_spray_array_state(SLink *L)	// 16.7.5 �޸���������Ϊ long long
{
	long long spray_array_state = 0;
	SLink *p = NULL;
	Spray spr;
	
	p = L->next;
	while(NULL != p)
	{
		spr = *(pSpray)(p->data);
		spray_array_state |= (long long)1 << (spr.zone - 1);
		p = p->next;
	}
	return spray_array_state;
}

// unsigned int get_queue_state(SLink *L)					16.7.5 �˴�δ��ʹ�ã�����
// {
// 	unsigned int spray_state = 0;		//����״̬
// 	unsigned int spray_queue_state = 0;		//�Ŷ�״̬
// 	unsigned int spray_array_state = 0;
// 	
// 	spray_array_state = get_spray_array_state(L);
// 	spray_state = get_spray_state(L);
// 	spray_queue_state = (~spray_state) & spray_array_state;
// 	return spray_queue_state;
// }


void spary_exe(SLink *L)
{
	int i = 0;
	SLink *p = NULL;
	pSpray pspr = NULL;
	static int count = 0;
	int flag = 0;
	
	p = L->next;
	
	while(NULL != p)
	{
		pspr = (pSpray)(p->data);
		
		if ((0 == i) && (ExeCompleted != pspr->work_status))
		{
			if ((pspr->howlong > 0) && (pspr->howlong <= (spray_time_limit * 60)))
			{
				pspr->howlong --;
				count++;
				
				pspr->work_status = Executing;
				if (pspr->howlong <= 0)
				{
					pspr->work_status = ExeCompleted;
				}
				if (count%60 == 0)
				{
					eep_update_spray(*pspr);
				}
// 				valve_open(pspr->zone);
			}
			else
			{
				pspr->work_status = ExeCompleted;
				count = 0;
			}
		}
		
		if (ExeCompleted == pspr->work_status)
		{
			if (0 == i)
			{
				eep_spray_delete_first();
				valve_close(pspr->zone);
			}
			else
			{
				flag = 1;
			}
			add_spray_to_waterused(pspr);
			del_spray(L, pspr->zone);
		}
		
		p = p->next;
		i++;
	}
	
	if (flag)
	{
		save_spray_link(L);
	}
}


void spary_exe2(SLink *L)
{
	int i = 0;
	SLink *p = NULL;
	pSpray pspr = NULL;
	static int count = 0;
	int flag = 0;
	
	p = L->next;
	
	while(NULL != p)
	{
		pspr = (pSpray)(p->data);
		
		if ((i < devparam.mwspray) && (ExeCompleted != pspr->work_status))
		{
			if ((pspr->howlong > 0) && (pspr->howlong <= (spray_time_limit * 60)))
			{
				pspr->howlong --;
				
				
				pspr->work_status = Executing;
				if (pspr->howlong <= 0)
				{
					pspr->work_status = ExeCompleted;
				}
				
				if (0 == i)
				{
					count++;
				}
				
				if (count%60 == 0)
				{
					eep_update_spray(*pspr);
				}
// 				valve_open(pspr->zone);
			}
			else
			{
				pspr->work_status = ExeCompleted;
				count = 0;
			}
		}
		
		if (ExeCompleted == pspr->work_status)
		{
			if (i < devparam.mwspray)
			{
// 				eep_spray_delete_first();
				valve_close(pspr->zone);
			}
			
			if (0 == i)
			{
				eep_spray_delete_first();
			}
			else
			{
				flag = 1;
			}
			
			add_spray_to_waterused(pspr);
			del_spray(L, pspr->zone);
		}
		
		p = p->next;
		i++;
	}
	
	if (flag)
	{
		save_spray_link(L);
	}
}

void add_spray_to_waterused(pSpray pspr)
{
	WaterUsedStruct wus;
	
	wus.zone = pspr->zone;
	wus.howlong = pspr->src_time - pspr->howlong;
	wus.adjust = pspr->adjust;
	wus.add_time = pspr->add_time;
	wus.type = pspr->type;
	wus.end_time = current_time;
	
	add_water_used(water_used_link, wus);
	send_water_data();
	
}

void spray_detection( SLink * L )
{
	
	if(spray_flag)
	{
		spray_state = 0;
		spray_queue_state = 0;
		spray_array_state = 0;
		spray_flag = 0;
		
		clean_spray(L);
		
		spray_state = get_spray_state(L);
		spray_array_state = get_spray_array_state(L);
		spray_queue_state = (~spray_state) & spray_array_state;

		
		
		spary_exe2(L);

		valve_open(L);
// 		HC595N_write(spray_state, spray_array_state);
	}
}