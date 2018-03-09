/*
 * data_rw.c
 *
 * Created: 2014/7/9 20:26:38
 *  Author: ZY
 */ 
#include "data_rw.h"
#include "btn_ctr.h"
#include "RTC.h"
#include "schedule_execute.h"
#include "wifi_receive.h"
#include "LCD.h"
#include <avr/wdt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

DevParam devparam;//�豸����
RainDelay rain_delay;
extern uchar user_band_flag;
extern uchar sentwater_flag;
extern SLink * water_used_link;
extern SLink *spray_link;

typedef struct epschedule_struct   //16.7.6 ����ṹ�壬����eeprom���ܼƻ�����ڵ��ʽ
{
	Schedule sch;                //���ܼƻ�����
	unsigned char nextzone;     //ָ����һ·��
	} EpSchedule,*pEpSchedule;

void set_mac_flag(unsigned char flag)
{
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)DevcodeFlagAddr, flag);
}

void read_spray_time_limit(void)
{
	eeprom_busy_wait();
	spray_time_limit = eeprom_read_word((uint16_t *)SprayLimitTimeAddr);
	eeprom_busy_wait();
	if (MaxManuTime < spray_time_limit)
	{
		spray_time_limit = MaxManuTime;
	}
}

void save_spray_time_limit(uint16_t howlong)
{
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)SprayLimitTimeAddr, howlong);
	eeprom_busy_wait();
}
 /************************************************************************/
 /* ��������get_devcode													 */
 /* ���ܣ�		��ȡ�豸��												 */
 /*������	��															 */
 /*����ֵ���豸��														 */
 /************************************************************************/
 void get_devcode(char * code)
 {
	eeprom_busy_wait();
	eeprom_read_block(code, (uint8_t *)DevcodeAddr, LenOfDevcodes);
	eeprom_busy_wait();
 }

 /************************************************************************/
 /* ��������get_serialcode*/																		
 /* ���ܣ�		��ȡ�����*/															
 /*������	��		*/																						
 /*����ֵ���豸��	*/																			
 /************************************************************************/
void get_serialcode( char * code ) 
{
	int i,j;
	char s[12];
	char p[37];
	unsigned long ultime = 0;	//�ƶ�ʱ��

	strcpy_P(p, PSTR("1234567890ABCDEFGHIJKLMNPQRSTUVWXYZ"));
	RTC_time_get();
	sprintf_P(s,PSTR("%d%d%d%d%d"),\
								current_time.mon,\
								current_time.mday,\
								current_time.hour,\
								current_time.min,\
								current_time.sec);
	sscanf_P(s,PSTR("%ld"),&ultime);
	
	srandom(ultime);
	
	for(i = 0; i < LenOfSerialCode; i++)
	{
		j = random()%35;
		code[i] = p[j];
	}
	eeprom_busy_wait();
	eeprom_write_block(serialcode, (uint8_t *)SerialCodeAddr, LenOfSerialCode);
}


/************************************************************************/
/* ��������read_code													*/
/* ���ܣ�		��ȡ�豸�ź������										*/
/*������	��															*/
/*����ֵ����															*/
/************************************************************************/
void read_code( void ) 
{
	get_devcode(devcode);
	get_serialcode(serialcode);
	if (!check_mac(devcode))
	{
		set_mac_flag(0);
	}
}

/************************************************************************/
/* ��������read_schedule												*/
/* ���ܣ�		��ȡ���ܼƻ�											*/
/*������	��															*/
/*����ֵ����															*/
/************************************************************************/
// void read_schedule( void )    //16.7.6 ����ʹ�ã�Ӱ����������
// {
// 	uchar i = 0;
// 	uchar num = 0;
// 	uint addr = 0;
// 	Schedule sch;
// 	
// 	addr = ScheduleNumAddr;
// 	eeprom_busy_wait();
// 	num = eeprom_read_byte((uint8_t *)addr);
// 	eeprom_busy_wait();
// 
// 	if( num > MaxScheduleNum)
// 	{
// 		num = 0;
// 	}
// 	
// 	if (NULL != sch_link)
// 	{
// 		for(i = 0; i < num; i++)
// 		{
// 			addr = Schedule1Addr + i * sizeof(Schedule);
// 			eeprom_read_block(&sch, (pSchedule)addr, sizeof(Schedule));
// 			eeprom_busy_wait();
// 			if (sch.zones > valves)
// 			{
// 				continue;
// 			}
// 			else
// 			{
// 				insert_sch(sch_link, sch);
// 			}
// 		}
// 	}
// }

void devparam_init(void)
{
// 	devparam.hassensor = 0;
// 	devparam.sensorinterv = 6;
// 	devparam.minadjust = 100;
	devparam.mwspray = 1;
// 	devparam.zonecount = 8;
	devparam.timezone = 0;
// 	devparam.maxelectric = 1000;
	devparam.mastertag = 0;
	devparam.tem_unit_flag = 0;
// 	strcpy_P(devparam.fwver,PSTR("1.6"));
}
/************************************************************************/
/* ��������read_devparam												*/
/* ���ܣ�		��ȡ�豸����										    */
/*������	��															*/
/*����ֵ����															*/
/************************************************************************/
void read_devparam( void ) 
{
	eeprom_busy_wait();
	eeprom_read_block(&devparam, (pDevParam)DevParamAddr, sizeof(DevParam));
	eeprom_busy_wait();
	if (0xff == devparam.mwspray)
	{
		devparam_init();
		save_devparam();
	}
}

/************************************************************************/
/* ��������save_devparam												*/
/* ���ܣ�		��ȡ�豸����											*/
/*������	��															*/
/*����ֵ����															*/
/************************************************************************/
void save_devparam( void )
{
	eeprom_busy_wait();
	eeprom_write_block(&devparam, (pDevParam)DevParamAddr, sizeof(DevParam));
}

/************************************************************************/
/* ��������get_water_used												*/
/* ���ܣ���ȡ��ˮ��														*/
/*������WaterUsedStruct ��ˮʱ���·��									*/
/*����ֵ����															*/
/************************************************************************/
pWaterUsedStruct get_water_used(SLink *L)
{
	int i = 0;
	SLink *p = NULL;
	pWaterUsedStruct pwus = NULL;
	
	i = get_length(L);
	
	if (i)
	{
		p = get_addr(L,i);
		if (NULL != p)
		{
			pwus = (pWaterUsedStruct)(p->data);
		}
	}
	
	return pwus;
}


/************************************************************************/
/* ��������read_water_used			*/													
/* ���ܣ�		��ȡ��ˮ��		*/																			
/*������	��					*/																			
/*����ֵ����				*/																			
/************************************************************************/
void save_raindelay( void ) 
{
	eeprom_busy_wait();
	eeprom_write_block(&rain_delay, (pRainDelay)RainDelayAddr, sizeof(RainDelay));
}

void read_raindelay( void ) 
{
	int min_dif = 0;

	eeprom_busy_wait();
	eeprom_read_block(&rain_delay, (pRainDelay)RainDelayAddr, sizeof(RainDelay));
	eeprom_busy_wait();
	if ( rain_delay.delay_time > MaxRainDelayTime )
	{
		rain_delay.delay_time = 0;
		return;
	}
	
	min_dif = MinDif(current_time, rain_delay.begin_time);
	if( min_dif < 0 )
	{
		rain_delay.delay_time = 0;
	}
	else
	{
		rain_delay.delay_time = rain_delay.delay_time - min_dif;
		if (rain_delay.delay_time < 0)
		{
			rain_delay.delay_time = 0;
		}
	}
}

void read_manuwater_set( void ) 
{
	eeprom_busy_wait();
	eeprom_read_block(ManuValveTime, (uint16_t *)ManuWaterSetAddr, MaxValveManu*2);
	if (ManuValveTime[0] > MaxManuTime)
	{
		memset(ManuValveTime, 0,sizeof(int)*MaxValveManu);
		save_manuwater_set();
	}
}


// void save_schedule( SLink *L ) //16.7.6 ����ʹ��
// {
// 	uint addr = 0;
// 	SLink *p = L->next;
// 	Schedule sch;
// 	uchar i = 0;
// 	uchar j = 0;
// 	
// 	i = get_length(L);
// 	addr = ScheduleNumAddr;
// 	eeprom_busy_wait();
// 	eeprom_write_byte((uint8_t *)addr,i);
// 	
// 	while(NULL != p)
// 	{
// 		sch = *(pSchedule)(p->data);
// 		p = p->next;
// 		addr = Schedule1Addr + j * sizeof(Schedule);
// 		eeprom_busy_wait();
// 		eeprom_write_block(&sch, (pSchedule)addr, sizeof(Schedule));
// 		j++;
// 	}
// }

/************************************************************************/
/* ��������add_epschedule												*/
/* ���ܣ��������ܼƻ�������eeprom										*/
/*������L�� ���ܼƻ������ַ											*/
/*����ֵ��1�ɹ�  0��ʧ��												*/
/************************************************************************/
int add_epschedule(SLink *L,Schedule sch_new)           //16.7.6        
{
	uint addr = 0;
	unsigned int i = 0;
	SLink *p;
	EpSchedule epsch;
	memset(&epsch,0,sizeof(EpSchedule));
	
	i = get_length(L);                   //RAM��������Ϊ0,���µ�����·����Ϊ�׽ڵ�
	if(i == 0)
	{
		eeprom_busy_wait();
		eeprom_write_byte((uint8_t *)EpScheduleHeadAddr,sch_new.zones);
		eeprom_busy_wait();
	}else                               //��������Ȳ�Ϊ0���������β�ڵ��nextzoneָ���µĽڵ�
	{
		p = get_addr(L,i);
		epsch.sch = *(pSchedule)(p->data);
		epsch.nextzone = sch_new.zones;
		addr = EpSchedule1Addr + sizeof(EpSchedule) * (epsch.sch.zones - 1);
		eeprom_busy_wait();
		eeprom_write_block(&epsch,(pEpSchedule)addr,sizeof(EpSchedule));
		eeprom_busy_wait();
	}
	
	epsch.sch = sch_new;
	epsch.nextzone = 0;
	
	addr = EpSchedule1Addr + sizeof(EpSchedule) * (epsch.sch.zones - 1);
	eeprom_busy_wait();
	eeprom_write_block(&epsch,(pEpSchedule)addr,sizeof(EpSchedule));
	eeprom_busy_wait();

	return 1;
}

/************************************************************************/
/* ��������updata_epschedule											*/
/* ���ܣ����±�����eeprom��ָ��·�������ܼƻ�����						*/
/*������L�� ���ܼƻ������ַ	zone�� ָ��·��							*/
/*����ֵ��1�ɹ�  0��ʧ��												*/
/************************************************************************/
int updata_epschedule(SLink *L,Schedule sch_new)    //16.7.6  
{
	uint addr = 0;
	unsigned int j = 0;
	SLink *p;
	EpSchedule epsch_new;
	Schedule sch;
	memset(&epsch_new,0,sizeof(EpSchedule));
	memset(&sch,0,sizeof(Schedule));
												
	j = locate_zone(L,sch_new.zones);
	if(j == 0) return 0;
	
	epsch_new.sch = sch_new;
	
	j = j+1;
	p = get_addr(L,j);
	if(p != NULL)     //�������µĽڵ㲻��β�ڵ㣬������дnextzoneָ��·��             
	{
		sch = *(pSchedule)(p->data);
		epsch_new.nextzone = sch.zones;
	}	
	
	addr = EpSchedule1Addr + sizeof(EpSchedule)*(epsch_new.sch.zones - 1);
	eeprom_busy_wait();
	eeprom_write_block(&epsch_new,(pEpSchedule)addr,sizeof(EpSchedule));
	eeprom_busy_wait();

	return 1;                           
}

/************************************************************************/
/* ��������insert_epschedule											*/
/* ���ܣ��������ܼƻ�������eeprom										*/
/*������L�� ���ܼƻ������ַ											*/
/*����ֵ��1�ɹ�  0��ʧ��												*/
/************************************************************************/
void insert_epschedule(SLink *L, Schedule sch_new)  //16.7.6 
{
	int i = 0;

	i = locate_zone(L, sch_new.zones);
	if (i)
	{
		updata_epschedule(L,sch_new);  //16.7.6
	}
	else
	{
		add_epschedule(L,sch_new);
	}
}


/************************************************************************/
/* ��������delete_epschedule											*/
/* ���ܣ�ɾ��eeprom��ָ��·�������ܼƻ�����								*/
/*������L�� ���ܼƻ������ַ	zone�� ָ��·��							*/
/*����ֵ��1�ɹ�  0��ʧ��												*/
/************************************************************************/
int delete_epschedule(SLink * L,unsigned char zone)    //16.7.6  
{
	uint addr = 0;
	SLink *p;
	unsigned int i;
	unsigned int j;
	EpSchedule epsch;
	Schedule sch = {0};
	
	i = locate_zone(L,zone);
	if(i == 0) return 0;
	
	j = i+1;              //ȡɾ���ڵ����һ�ڵ�
	i = i-1;			  //ȡɾ���ڵ����һ�ڵ�
	
	if(i == 0)           //�����һ�ڵ�Ϊ0 ��˵��ɾ������ͷ�ڵ�
	{
		p = get_addr(L,j);
		if(p == NULL)          //��һ�ڵ�Ϊ�գ�˵��ɾ�������һ���ڵ㣬ͷ·��Ӧ����Ϊ0
		{
			eeprom_busy_wait();
			eeprom_write_byte((uint8_t *)EpScheduleHeadAddr,0);
			eeprom_busy_wait();
			return 1;
		}
		sch = *(pSchedule)(p->data);  //��Ϊ�գ���ͷ·������Ϊ��һ�ڵ�·��
		eeprom_busy_wait();
		eeprom_write_byte((uint8_t *)EpScheduleHeadAddr,sch.zones);
		eeprom_busy_wait();
		return 1;		
	}
	
	p = get_addr(L,i);
	epsch.sch = *(pSchedule)(p->data);
	
	p = get_addr(L,j);        
	if(p == NULL)				//���Ҫ��һ�ڵ�Ϊ�գ���ɾ������β�ڵ㣬��һ�ڵ�ָ��0
	{		
		epsch.nextzone = 0;
		addr = EpSchedule1Addr + sizeof(EpSchedule) * (epsch.sch.zones - 1);
		eeprom_busy_wait();
		eeprom_write_block(&epsch,(pEpSchedule)addr,sizeof(EpSchedule));
		eeprom_busy_wait();
		return 1;
	}else                          //�����Ϊ�գ�����һ�ڵ��nextָ����һ�ڵ��·��
	{
		sch = *(pSchedule)(p->data);
		epsch.nextzone = sch.zones;
		addr = EpSchedule1Addr + sizeof(EpSchedule) * (epsch.sch.zones - 1);
		eeprom_busy_wait();
		eeprom_write_block(&epsch,(pEpSchedule)addr,sizeof(EpSchedule));
		eeprom_busy_wait();
		return 1;
	}
}

/************************************************************************/
/* ��������updata_epschedule											*/
/* ���ܣ���ȡeeprom�е����ܼƻ����ݣ������뵽RAM�е�sch_link			*/
/*������L�� ��															*/
/*����ֵ����															*/
/************************************************************************/
void read_epschedule(void)               //16.7.6  
{
	uint addr = 0;
	unsigned char head = 0;
	Schedule sch = {0};
	EpSchedule epsch;
			
	eeprom_busy_wait();
	head = eeprom_read_byte((uint8_t *)EpScheduleHeadAddr);
	eeprom_busy_wait();
	if(head > MaxValve)
	{
		head = 0;
	}
	if(head == 0) return;
	
	
	addr = EpSchedule1Addr + sizeof(EpSchedule) * (head - 1);
	eeprom_busy_wait();
	eeprom_read_block(&epsch,(pEpSchedule)addr,sizeof(EpSchedule));
	eeprom_busy_wait();
	while(1)
	{
		sch = epsch.sch;	
		insert_sch(sch_link, sch);	
		
		if(epsch.nextzone == 0) break;
		addr = EpSchedule1Addr + sizeof(EpSchedule) * (epsch.nextzone - 1);
		eeprom_busy_wait();
		eeprom_read_block(&epsch,(pEpSchedule)addr,sizeof(EpSchedule));	
		eeprom_busy_wait();
	}
}


/************************************************************************/
/* ��������reset_epschedule												*/
/* ���ܣ�ɾ��eeprom���������ܼƻ�����									*/
/*������L�� ���ܼƻ������ַ	zone�� ָ��·��							*/
/*����ֵ��1�ɹ�  0��ʧ��												*/
/************************************************************************/
void reset_epschedule(void)             //16.7.6  
{
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)EpScheduleHeadAddr,0);
	eeprom_busy_wait();
}

// void reset_sch_num(void)          //16.7.6  ����ʹ��
// {
// 	eeprom_busy_wait();
// 	eeprom_write_byte((uint8_t *)ScheduleNumAddr,0);
// 	eeprom_busy_wait();
// }

void save_band_status( void )
{
	uchar i = 0xaa;
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)BandStatusAddr,i);
}

void reset_band_status( void )
{
	uchar i = 0xff;
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)BandStatusAddr,i);
}


void read_band_status(void)
{
	uchar i = 0;
	eeprom_busy_wait();
	i = eeprom_read_byte((uint8_t *)BandStatusAddr);
	eeprom_busy_wait();
	if (0xaa == i)
	{
		user_band_flag = UserBandOk;
	}
}


void save_manuwater_set(void)
{
	eeprom_busy_wait();
	eeprom_write_block(ManuValveTime, (uint16_t *)ManuWaterSetAddr, MaxValveManu*2);
}

void save_finetune( FineTune ft )
{
	eeprom_busy_wait();
	eeprom_write_block(&ft,(pFineTune)FineTuneAddr, sizeof(FineTune));
	eeprom_busy_wait();
}

void finetune_init(void)
{
	memset(weather_adjust.minadjust, 100, sizeof(weather_adjust.minadjust));
	weather_adjust.adjust_mode = 0;
	save_finetune(weather_adjust);
}

void read_finetune( void )
{
	eeprom_busy_wait();
	eeprom_read_block(&weather_adjust,(pFineTune)FineTuneAddr, sizeof(FineTune));
	eeprom_busy_wait();
	if (weather_adjust.minadjust[0] > MaxtAdjustValue)
	{
		finetune_init();
	}
}

// void update_schedule( uchar i,Schedule NewSch )          //16.7.6  ����ʹ��
// {
// 	uint addr = 0;	
// 	
// 	addr = Schedule1Addr + (i-1) * sizeof(Schedule);
// 	eeprom_busy_wait();
// 	eeprom_write_block(&NewSch, (pSchedule)addr, sizeof(Schedule));
// }

void set_del_zone(uchar zone)
{
	int i = 1;
	SLink *p = NULL;
	pSchedule psch = NULL;
	uchar del_valves[6] = {0};
	
	if (zone > MaxValve)
	{
		return;
	}
	
	i = locate_zone(sch_link,zone);
	p = get_addr(sch_link,i);
	if (NULL != p)
	{
		psch = (pSchedule)p->data;
		if(Uploaded == psch->upload_status)				
		{
			eeprom_busy_wait();
			eeprom_read_block(del_valves,(uint8_t *)DeleteSchAddr, sizeof(del_valves));
			eeprom_busy_wait();
			del_valves[(zone-1)/8] &= ~(1<<((zone-1)%8));
			eeprom_write_block(del_valves,(uint8_t *)DeleteSchAddr,sizeof(del_valves));
			eeprom_busy_wait();
		}
	}
}

void reset_del_zone(uchar zone)
{
	uchar del_valves[6] = {0};
	
	if(zone > MaxValve)
	{
		return;
	}
	eeprom_busy_wait();
	eeprom_read_block(del_valves,(uint8_t *)DeleteSchAddr, sizeof(del_valves));
	eeprom_busy_wait();
	del_valves[(zone-1)/8] |= 1<<((zone-1)%8);
	eeprom_write_block(del_valves,(uint8_t *)DeleteSchAddr,sizeof(del_valves));
	eeprom_busy_wait();
}

uchar get_del_zone(void)
{
	int i = 0;
	int j = 0;
	uchar k = 0;
	uchar del_valves[6] = {0};
	eeprom_busy_wait();
	eeprom_read_block(del_valves,(uint8_t *)DeleteSchAddr, sizeof(del_valves));
	eeprom_busy_wait();
	
	for (i=0; i<6;i++)
	{
		for (j=0;j<8;j++)
		{
			k++;
			if (k > valves)
			{
				return 0;
			}
			if (!(del_valves[i] & (1<<j)))
			{
				return k;
			}
			
			if (48 == k)
			{
				return 0;
			}
		}
	}
	
	return k;
}

void recovery_system(void)
{
	int i = 0;
	LCD_clear();
	LCD_show_strP(3,3,PSTR("System Recovery"));
	
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)BandStatusAddr, 0);
	
	for (i = DevcodeFlagAddr; i < (EpSchedule1Addr+sizeof(EpSchedule)); i++)    //16.7.6  �޸�Ϊ��EpSchedule1Addr��
	{
		eeprom_busy_wait();
		eeprom_write_byte((uint8_t *)i, 0);
	}
	
	devparam_init();
	eeprom_busy_wait();
	eeprom_write_block(&devparam,(pDevParam)DevParamAddr, sizeof(DevParam));
	eeprom_busy_wait();
	
	uchar del_valves[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	eeprom_write_block(del_valves,(uint8_t *)DeleteSchAddr,sizeof(del_valves));
	eeprom_busy_wait();
	
	eeprom_write_byte((uint8_t *)WaterUsedHead, 0);
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)WaterUsedTag, 0);
	eeprom_busy_wait();

	eeprom_write_word((uint16_t *)SprayLimitTimeAddr, 0xffff);
	eeprom_busy_wait();
	
	finetune_init();
	
	while(1);
}

void eeprom_write_water_used(SLink *L, WaterUsedStruct wus)
{
	uchar head = 0;
	uchar tag = 0;
	
	eeprom_busy_wait();
	head = eeprom_read_byte((uint8_t *)WaterUsedHead);
	eeprom_busy_wait();
	tag = eeprom_read_byte((uint8_t *)WaterUsedTag);
	eeprom_busy_wait();
	if ((head > WaterUsedNum) || (tag > WaterUsedNum))
	{
		head = 0;
		tag = 0;
	}
	
	eeprom_write_block(&wus, (pWaterUsedStruct)(WaterUsed1Addr + (tag * sizeof(WaterUsedStruct))), sizeof(WaterUsedStruct));
	eeprom_busy_wait();
	
	if(head == tag)
	{
		tag++;
		if (tag > WaterUsedNum)
		{
			tag = 0;
		}
	}
	else if (head > tag)
	{
		tag++;
		if (tag == head)
		{
			head++;
		}
		if (head > WaterUsedNum)
		{
			head = 0;
		}
	}
	else if (head < tag)
	{
		tag++;
		
		if (tag > WaterUsedNum)
		{
			tag = 0;
		}
		if (tag == head)
		{
			head++;
		}
	}
	
	eeprom_write_byte((uint8_t *)WaterUsedHead, head);
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)WaterUsedTag, tag);
	eeprom_busy_wait();
}

void eeprom_read_water_used(SLink *L)
{
	uchar head = 0;
	uchar tag = 0;
	int i = 0;
	WaterUsedStruct wus;
	pWaterUsedStruct pwus = NULL;
	
	i = get_length(L);
	if (i)
	{
		return;
	}
	
	eeprom_busy_wait();
	head = eeprom_read_byte((uint8_t *)WaterUsedHead);
	eeprom_busy_wait();
	tag = eeprom_read_byte((uint8_t *)WaterUsedTag);
	eeprom_busy_wait();
	if ((head > WaterUsedNum) || (tag > WaterUsedNum))
	{
		head = 0;
		tag = 0;
	}
	
	if (head == tag)
	{
		return;
	}
	else
	{
		if (tag == 0)
		{
			tag = WaterUsedNum;
		}
		else
		{
			tag--;
		}
		
		eeprom_read_block(&wus, (pWaterUsedStruct)(WaterUsed1Addr + tag * sizeof(WaterUsedStruct)), sizeof(WaterUsedStruct));
		eeprom_busy_wait();

		pwus = (pWaterUsedStruct)malloc(sizeof(WaterUsedStruct));
		if (pwus != NULL)
		{
			*pwus = wus;
			ins_elem(L,pwus,1);
		}
	}
	
	eeprom_write_byte((uint8_t *)WaterUsedHead, head);
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)WaterUsedTag, tag);
	eeprom_busy_wait();
}

/************************************************************************/
/* ��������save_water_used*/																			 
/* ���ܣ�������ˮ��*/																					
/*��������*/																							
/*����ֵ����*/																						
/************************************************************************/
void save_all_water_used( SLink *L )
{
	SLink *p = L->next;
	pWaterUsedStruct pwus = NULL;

	while(NULL != p)
	{
		pwus = (pWaterUsedStruct)(p->data);
		eeprom_write_water_used(L, *pwus);
		p = p->next;
	}
}

void save_mac( char *mac )
{
	eeprom_busy_wait();
	eeprom_write_block(mac, (uint8_t *)DevcodeAddr, LenOfDevcodes);
	set_mac_flag(1);
}

unsigned char mac_justice(unsigned char temp)
{
	switch(temp)
	{
		case '0':return 0x00;
		case '1':return 0x01;
		case '2':return 0x02;
		case '3':return 0x03;
		case '4':return 0x04;
		case '5':return 0x05;
		case '6':return 0x06;
		case '7':return 0x07;
		case '8':return 0x08;
		case '9':return 0x09;
		case 'a':return 10;
		case 'b':return 11;
		case 'c':return 12;
		case 'd':return 13;
		case 'e':return 14;
		case 'f':return 15;
		case 'A':return 10;
		case 'B':return 11;
		case 'C':return 12;
		case 'D':return 13;
		case 'E':return 14;
		case 'F':return 15;
		default:return 0x00;
	}
}

unsigned char get_mac_flag(void)
{
	unsigned char flag = 0;
	eeprom_busy_wait();
	flag = eeprom_read_byte((uint8_t *)DevcodeFlagAddr);
	eeprom_busy_wait();
	return flag;
}



void get_mac(unsigned char *mac)
{
	unsigned char i = 0;
	unsigned char j = 0;
	char mac_temp[12] = {0};
	int flag = 0;
	flag = get_mac_flag();
	if (1 == flag)
	{
		eeprom_read_block(mac_temp, (uint8_t *)DevcodeAddr, LenOfDevcodes);
		eeprom_busy_wait();
		for(i = 0;i < 6;i++)
		{
			mac[i] = (mac_justice(mac_temp[j]) << 4) + mac_justice(mac_temp[j + 1]);
			j = j + 2;
		}
	}
}

void set_backlight_flag(unsigned char data)
{
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)BackLightAddr, data);
}

int get_backlight_flag(void)
{
	int flag = 0;
	eeprom_busy_wait();
	flag = eeprom_read_byte((uint8_t *)BackLightAddr);
	eeprom_busy_wait();
	return flag;
}

void set_timemode_flag(unsigned char timemode)
{
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)TimeModeAddr,timemode);
}

void get_timemode_flag(unsigned char * timemode)
{
	unsigned char timemode_temp = 0;
	eeprom_busy_wait();
	timemode_temp = eeprom_read_byte((uint8_t *)TimeModeAddr);
	if(timemode_temp != TimeMode_12 && timemode_temp != TimeMode_24){*timemode = TimeMode_24;}
	else{*timemode = timemode_temp;}	
}

int get_version_id(void)
{
	return version;	
}

void get_vesion_str(char *res)    //167=>1.6.7
{
	int version_id = 0;
	
	version_id = get_version_id();
	
	int unit = version_id%10;
	int ten = version_id/10%10;
	int hundred = version_id/100%10;
	
	sprintf_P(res, PSTR("%d.%d.%d"), hundred, ten, unit);
}

void save_domain(char *serve)
{
	eeprom_busy_wait();
	eeprom_write_block(serve, (uint8_t *)ServeAddr, strlen(serve)+1);
	eeprom_busy_wait();
}

void read_daomain(char *dest)
{
	eeprom_busy_wait();
	eeprom_read_block(dest, (uint8_t *)ServeAddr, LenOfServe);
	eeprom_busy_wait();
}


void eep_save_spray(Spray spray)
{
	uchar head = 0;
	uchar tag = 0;
	
	eeprom_busy_wait();
	head = eeprom_read_byte((uint8_t *)SparyHeadAddr);
	eeprom_busy_wait();
	tag = eeprom_read_byte((uint8_t *)SparyTagAddr);
	eeprom_busy_wait();
	if ((head > SprayEepromNum) || (tag > SprayEepromNum))
	{
		head = 0;
		tag = 0;
	}
	
	eeprom_write_block(&spray, (pSpray)(SparyFirstAddr + (tag * sizeof(Spray))), sizeof(Spray));
	eeprom_busy_wait();
	
	if(head == tag)
	{
		tag++;
		if (tag > SprayEepromNum)
		{
			tag = 0;
		}
	}
	else if (head > tag)
	{
		tag++;
		if (tag == head)
		{
			head++;
		}
		if (head > SprayEepromNum)
		{
			head = 0;
		}
	}
	else if (head < tag)
	{
		tag++;
		
		if (tag > SprayEepromNum)
		{
			tag = 0;
		}
		if (tag == head)
		{
			head++;
		}
	}
	
	eeprom_write_byte((uint8_t *)SparyHeadAddr, head);
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)SparyTagAddr, tag);
	eeprom_busy_wait();
}

void eep_read_spray(SLink *L)
{
	uchar head = 0;
	uchar tag = 0;
	int i = 0;
	Spray spray;
	pSpray pspray = NULL;
	
	i = get_length(L);
	if (NULL == L)
	{
		init_list(&spray_link);
	}
	
	eeprom_busy_wait();
	head = eeprom_read_byte((uint8_t *)SparyHeadAddr);
	eeprom_busy_wait();
	tag = eeprom_read_byte((uint8_t *)SparyTagAddr);
	eeprom_busy_wait();
	if ((head > SprayEepromNum) || (tag > SprayEepromNum))
	{
		head = 0;
		tag = 0;
	}
	
	if (head == tag)
	{
		return;
	}
	else
	{
		while(head != tag)
		{
			eeprom_read_block(&spray, (pSpray)(SparyFirstAddr + head * sizeof(Spray)), sizeof(Spray));
			eeprom_busy_wait();

			pspray = (pSpray)malloc(sizeof(Spray));
			if (pspray != NULL)
			{
				*pspray = spray;
				i =get_length(L)+1;
				ins_elem(L,pspray,i);
			}
			else
			{
				// 				USART0_send("\r\n malloc error \r\n");
			}
			
			head++;
			if (head > SprayEepromNum)
			{
				head = 0;
			}
		}
	}
}

void save_spray_link( SLink *L )
{
	SLink *p = L->next;
	Spray spr;
	uchar head = 0;
	uchar tag = 0;
	
	eeprom_busy_wait();
	head = eeprom_read_byte((uint8_t *)SparyHeadAddr);
	eeprom_busy_wait();
	tag = eeprom_read_byte((uint8_t *)SparyTagAddr);
	eeprom_busy_wait();
	if ((head > SprayEepromNum) || (tag > SprayEepromNum))
	{
		head = 0;
		tag = 0;
	}
	
	head = tag;
	
	while(NULL != p)
	{
		spr = *(pSpray)(p->data);
		p = p->next;
		
		eeprom_busy_wait();
		eeprom_write_block(&spr, (pSpray)(SparyFirstAddr + (tag * sizeof(Spray))), sizeof(Spray));
		eeprom_busy_wait();
		
		if(head == tag)
		{
			tag++;
			if (tag > SprayEepromNum)
			{
				tag = 0;
			}
		}
		else if (head > tag)
		{
			tag++;
			if (tag == head)
			{
				head++;
			}
			if (head > SprayEepromNum)
			{
				head = 0;
			}
		}
		else if (head < tag)
		{
			tag++;
			
			if (tag > SprayEepromNum)
			{
				tag = 0;
			}
			if (tag == head)
			{
				head++;
			}
		}
	}
	
	eeprom_write_byte((uint8_t *)SparyHeadAddr, head);
	eeprom_busy_wait();
	eeprom_write_byte((uint8_t *)SparyTagAddr, tag);
	eeprom_busy_wait();
}

void eep_update_spray(Spray spr)
{
	uchar head = 0;
	uchar tag = 0;
	Spray spray;
	
	eeprom_busy_wait();
	head = eeprom_read_byte((uint8_t *)SparyHeadAddr);
	eeprom_busy_wait();
	tag = eeprom_read_byte((uint8_t *)SparyTagAddr);
	eeprom_busy_wait();
	if ((head > SprayEepromNum) || (tag > SprayEepromNum))
	{
		head = 0;
		tag = 0;
	}
	
	if (head == tag)
	{
		return;
	}
	else
	{
		
		while(head != tag)
		{
			eeprom_busy_wait();
			eeprom_read_block(&spray, (pSpray)(SparyFirstAddr + head * sizeof(Spray)), sizeof(Spray));
			eeprom_busy_wait();
			
			if (spray.zone == spr.zone)
			{
				eeprom_write_block(&spr, (pSpray)(SparyFirstAddr + (head * sizeof(Spray))), sizeof(Spray));
				eeprom_busy_wait();
				return;
			}
			
			head++;
			if (head > SprayEepromNum)
			{
				head = 0;
			}
		}
	}
}

void eep_spray_delete_first(void)
{
	uchar head = 0;
	uchar tag = 0;
	
	eeprom_busy_wait();
	head = eeprom_read_byte((uint8_t *)SparyHeadAddr);
	eeprom_busy_wait();
	tag = eeprom_read_byte((uint8_t *)SparyTagAddr);
	eeprom_busy_wait();
	if ((head > SprayEepromNum) || (tag > SprayEepromNum))
	{
		head = 0;
		tag = 0;
		eeprom_write_byte((uint8_t *)SparyHeadAddr, head);
		eeprom_busy_wait();
		eeprom_write_byte((uint8_t *)SparyTagAddr, tag);
		eeprom_busy_wait();
		return;
	}
	
	head++;
	if (head > SprayEepromNum)
	{
		head = 0;
	}
	eeprom_write_byte((uint8_t *)SparyHeadAddr, head);
	eeprom_busy_wait();
}

void read_param( void )
{
	read_band_status();//��ȡ�û���״̬
	read_code();//��ȡ�豸�ź�������
	read_epschedule();//��ȡ���ܼƻ�
	read_devparam();//��ȡ�豸����
	read_raindelay();//��ȡ�ӳ�����ʱ��
	read_manuwater_set();//��ȡ�ֶ���������
	read_finetune();//��ȡ΢������
	read_spray_time_limit();
	eep_read_spray(spray_link);
}
