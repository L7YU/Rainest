/*
 * schedule_execute.c
 *
 * Created: 2014/7/11 15:41:57
 *  Author: ZY
 */ 
#include "data_rw.h"
#include "schedule_execute.h"
#include "HC595N.h"
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

extern SLink *sch_link ;
extern SLink * spray_link;
extern FineTune weather_adjust;

/************************************************************************/
/* ��������start_spray													*/
/* ���ܣ�ִ�����ܿ�														*/
/*������zones: ·��(λ��ʾ) howlong��ʱ��								*/
/*����ֵ����															*/
/************************************************************************/
void start_spray( uchar zones, uint howlong ) 
{
//	uchar i = 0;   //16.7.7 unused
	Spray spr;
	
	if(OffMode == work_value) 
		return;//OFFģʽ�£�������
		
	spr.add_time = current_time;
	spr.adjust = 100;
	spr.type = SparyByApp;
	
	if (zones <= MaxValve)
	{
		spr.zone = zones;
		spr.howlong = howlong;
		spr.src_time = howlong;
		spr.work_status = NoExecute;
		add_spray(spray_link, spr);
	}
}


/************************************************************************/
/* ��������schedule_exe													*/
/* ���ܣ�ִ�е������ܼƻ�												*/
/*������sch: ���ܼƻ�  tm��ʱ��											*/
/*����ֵ����															*/
/************************************************************************/
void ScheduleExe( Schedule sch, Time tm)
{		
	uchar i = 0;
	float howlong = 0;
	float adjust = 0;
	Spray spr;
	
// 	if(rain_delay.delay_time) return;//���ڈ����ӳ����ܣ��˳���ѯ���ܼƻ�
	if ((((sch.weeks >> (tm.wday-1)) &1) && (sch.mode == BY_WEEKS)) || \
	    (((sch.days >> (tm.mday-1)) &1) && (sch.mode == BY_DAYS)))		//����ƥ���������ƥ��
	{
		for (i = 0; i < TimesOfSchedule; i++)	//ƥ���ĸ�ʱ��
		{
			if (sch.times[2*i] == tm.hour)	//Сʱƥ��
			{
				if (sch.times[2*i + 1] == tm.min)	//����ƥ��
				{
					if ((sch.howlong[i] > 0) && (sch.howlong[i] <= spray_time_limit))
					{
						adjust = weather_adjust.minadjust[0];
						howlong = sch.howlong[i];
						howlong = howlong * 60;//����ת������
						
						if((sensor_value == SensorOn) || (adjust == 0) || (rain_delay.delay_time))
						{
							spr.howlong = 0;
							spr.src_time = (uint)howlong;
							if (sensor_value == SensorOn)
							{
								spr.type = SparyBySensor;
							}
							else if(adjust == 0)
							{
								spr.type = SparyBySch;
							}
							else
							{
								spr.type = SparyByDelay;
							}
						}
						else
						{
							if (!sch.is_weather)
							{
								adjust = adjust / 100;
// 								howlong *= adjust;	//���ܼƻ���Ҫ����΢����������ʱ��
								howlong = howlong * adjust * sch.xishu;	//���ܼƻ���Ҫ����΢����������ʱ��
							}
							spr.howlong = (uint)howlong;
							spr.src_time = spr.howlong;
							spr.type = SparyBySch;
						}
						
						spr.zone =sch.zones;
						spr.add_time = tm;
						spr.adjust = weather_adjust.minadjust[0];
						spr.work_status = NoExecute;
						
						if((0 != devparam.mastertag) && (spr.zone == MasterZone)) //masterģʽ�£�12·��ִ��
						{
							continue;
						}
						add_spray(spray_link, spr);
					}
					else
					{
						//ʱ�䲻�Ϸ���ִ�в���
// 						if (sch.howlong[i] > spray_time_limit)
// 						{
// 							LCD_show_number(0,0,spray_time_limit);
// 						}
					}
				}
			}
		}	
	}			
}

/************************************************************************/
/* ��������InitList														*/
/* ���ܣ������ʼ��														*/
/*��������																*/
/*����ֵ������ָ���ַ													*/
/************************************************************************/
void init_list( SLink **L)
{
	*L = (SLink *)malloc(sizeof(SLink));
	if (NULL != *L)
	{
		(*L)->data = NULL;
		(*L)->next = NULL;
	}
}

/************************************************************************/
/* ��������GetLength													*/
/* ���ܣ���ȡ������													*/
/*������L�� �����ַ													*/
/*����ֵ��																*/
/************************************************************************/
int get_length( SLink *L )
{
	int i = 0;
	SLink *p = L->next;
	while(p != NULL)
	{
		p = p->next;
		i++;
	}
	return i;
}

/************************************************************************/
/* ��������LocateZone													*/
/* ���ܣ���ȡ���ܼƻ�IDΪx��λ��										*/
/*������L�� �����ַ	x��Zoneֵ										*/
/*����ֵ��i����������λ��  0��ʧ�ܣ�û�и�Ԫ��							*/
/************************************************************************/
int locate_zone( SLink *L,uchar x )
{
	int i = 1;
	SLink *p = L->next;
	pSchedule psch;

	while (p != NULL /*&& (x != psch->zones)*/)
	{
		psch = (pSchedule)(p->data);
		if (x == psch->zones)
		{
			break;
		}
		p = p->next;
		i++;
	}

	if (p == NULL)
	{
		return 0;
	}
	else return i;
}


/************************************************************************/
/* ��������InsElem														*/
/* ���ܣ�����һ��Ԫ��													*/
/*������L�� �����ַ	x�����ܼƻ� i������λ��							*/
/*����ֵ��1���ɹ� 0��ʧ��												*/
/************************************************************************/
int ins_elem( SLink *L, void * data, int i )
{
	int j = 1;
	SLink *p = L;
	SLink *s;
	
	if (i<1 || i>(get_length(L)+1))
	{
		return 0;
	}
	else
	{
		s = (SLink *)malloc(sizeof(SLink));
		if (s != NULL)
		{
			s->data = data;
			s->next = NULL;
			
			while(j<i)
			{
				p = p->next;
				j++;
			}
			s->next = p->next;
			p->next = s;
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

/************************************************************************/
/* ��������DelElem														*/
/* ���ܣ�ɾ��һ��Ԫ��													*/
/*������L�� �����ַ	i��ɾ��λ��										*/
/*����ֵ��1���ɹ� 0��ʧ��												*/
/************************************************************************/
int DelElem( SLink *L, int i )
{
	int j = 1;
	SLink *p = L;
	SLink *q ;
	if ( i<1 || i>get_length(L) )
	{
		return 0;
	}
	else
	{
		while( j<i)
		{
			p = p->next;
			j++;
		}
		q = p->next;
		p->next = q->next;

		free(q->data);
		q->data = NULL;
		free(q);
		q = NULL;
		return 1;
	}
}

/************************************************************************/
/* ��������GetAdrr														*/
/* ���ܣ���ȡλ��Ϊi��Ԫ��ָ��											*/
/*������L�� �����ַ	i��Ԫ��λ��										*/
/*����ֵ��p���ɹ������ظ�Ԫ�ص�ַ 0��ʧ��								*/
/************************************************************************/
SLink* get_addr( SLink *L, int i )
{
	int j = 1;
	SLink *p = L->next;
	while( j<i )
	{
		if(NULL == p)
			break;
		p = p->next;
		j++;
	}
	return p;
}

/************************************************************************/
/* ��������UpdataSch													*/
/* ���ܣ��������ܼƻ�													*/
/*������NewSch���µ����ܼƻ�											*/
/*����ֵ����															*/
/************************************************************************/
void updata_sch( SLink *L, Schedule NewSch )
{
	int i = 0;
	SLink *p = NULL;
	i = locate_zone(L, NewSch.zones);
	if (i)
	{
		p = get_addr(L,i);
		*(pSchedule)(p->data) = NewSch;
//		update_schedule(i, NewSch);
	}
}

/************************************************************************/
/* ��������InsertSch													*/
/* ���ܣ����һ���µ����ܼƻ�											*/
/*������sch�����ܼƻ�,L: �����׵�ַ									    */
/*����ֵ����															*/
/************************************************************************/
void insert_sch(SLink *L, Schedule sch)  
{                                                             
	int i = 0;
	SLink *p = NULL;
	pSchedule psch = NULL;

	i = locate_zone(L, sch.zones);
	if (i)
	{
		p = get_addr(L,i);
		*(pSchedule)(p->data) = sch;
	}
	else
	{
		psch = (pSchedule)malloc(sizeof(Schedule));
		if (psch != NULL)
		{
			*psch = sch;
			i =get_length(L)+1;
			if(!ins_elem(L,psch,i))
			{
				free(psch);
				psch = NULL;
			}
		}
	}
}


/************************************************************************/
/* ��������DelSch														*/
/* ���ܣ�ɾ�����ܼƻ�													*/
/*������zone�����ܼƻ���zone											*/
/*����ֵ����															*/
/************************************************************************/
void del_sch( SLink *L, uchar zone )
{
	int i = 0;
	i = locate_zone(L,zone);
	if (i > 0)
	{
		delete_epschedule(L,zone); //16.7.6 ��ע�⣬ɾ�����ܼƻ�ʱ��Ϊ����ͬ��eeprom�е����ݣ����뽫delete_epschedule������DelElem֮ǰ
		DelElem(L,i);
//		save_schedule(L);
	}
}

void del_all_sch(SLink *L)
{
	clear_link(L);
//	reset_sch_num();
	reset_epschedule();  //16.7.6
}

/************************************************************************/
/* ��������SchInquire													*/
/* ���ܣ��鿴���ܼƻ��Ƿ���Ҫִ��										*/
/*������L�����ܼƻ�����		tm����ǰʱ��								*/
/*����ֵ����															*/
/************************************************************************/
void sch_inquire( SLink *L,Time tm )
{
	SLink *p = L->next;
	Schedule sch;
	
	while(NULL != p)
	{
		sch = *(pSchedule)(p->data);
		ScheduleExe(sch, tm);
		p = p->next;
	}
}

/************************************************************************/
/* ��������GetUploadSchAddr												*/
/* ���ܣ���ȡδ�ϴ������ܼƻ�											*/
/*������L�����ܼƻ�����													*/
/*����ֵ�����ܼƻ�ָ��													*/
/************************************************************************/
pSchedule get_upload_sch_addr( SLink *L )
{
	SLink *p = L->next;
	pSchedule psch = NULL;

	while(p != NULL)
	{
		psch = (pSchedule)(p->data);
		
		if(Uploaded != psch->upload_status)
		{
			return	psch;
		}
		else
		{
			p = p->next;
		}
	}
	return psch;
}

/************************************************************************/
/* ��������LocateSpray													*/
/* ���ܣ�����·����ȡ����λ��											*/
/*������L�����ܼƻ�����													*/
/*����ֵ����·��λ��													*/
/************************************************************************/
int locate_spray( SLink *L,uchar zone )
{
	int i = 1;
	SLink *p = L->next;
	pSpray psch;

	while (p != NULL)
	{
		psch = (pSpray)(p->data);
		if (zone == psch->zone)
		{
			break;
		}
		p = p->next;
		i++;
	}

	if (p == NULL)
	{
		return 0;
	}
	else return i;
}

/************************************************************************/
/* ��������AddSpray														*/
/* ���ܣ���Ӽ�ʱ����													*/
/*������L����ʱ�������� spr����ʱ���ܽṹ��								*/
/*����ֵ����															*/
/************************************************************************/
void add_spray( SLink *L, Spray spr )
{
	int i = 0;
	SLink *p = NULL;
	pSpray pspr = NULL;
			
	i = locate_spray(L, spr.zone);
	if (i)
	{
		p = get_addr(L,i);
		pspr = (pSpray)(p->data);
		pspr->work_status = ExeCompleted;
		
		add_spray_to_waterused(pspr);//��ӵ���ˮ����
		DelElem(L, i);//ɾ�������ܣ���ʡ�ڴ�
		
		if ((spr.zone > 0) && (spr.zone <= MaxValve))
		{
			pspr = (pSpray)malloc(sizeof(Spray));
			if (pspr != NULL)
			{
				*pspr = spr;
				if(!ins_elem(L,pspr,i))
				{
					free(pspr);
					pspr = NULL;
				}
			}
			eep_update_spray(spr);
		}
	}
	else
	{
		if ((spr.zone > 0) && (spr.zone <= MaxValve))
		{
			pspr = (pSpray)malloc(sizeof(Spray));
			if (pspr != NULL)
			{
				*pspr = spr;
				i =get_length(L)+1;
				if(!ins_elem(L,pspr,i))
				{
				
					free(pspr);
					pspr = NULL;
				}
			}
			eep_save_spray(spr);
			spray_open();
		}
	}	
}

/************************************************************************/
/* ��������DelSpray														*/
/* ���ܣ�ɾ����ʱ����													*/
/*������L����ʱ�������� zone����ʱ���ܵ�·��							*/
/*����ֵ����															*/
/************************************************************************/
void del_spray( SLink *L, uchar zone )
{
	int i = 0;
	i = locate_spray(L,zone);
	if (i)
	{
		DelElem(L,i);
	}
}

/************************************************************************/
/* ��������CleanSpray													*/
/* ���ܣ��������ʱ��Ϊ0��·��											*/
/*������L����ʱ��������													*/
/*����ֵ����															*/
/************************************************************************/
void clean_spray( SLink *L )
{
	SLink *p = L->next;
	pSpray pspr = NULL;

	while(NULL != p)
	{
		pspr = (pSpray)(p->data);
		if (((MaxManuTime * 60ul) < pspr->howlong) || (pspr->zone > MaxValve) || (pspr->zone < 1)) //16.7.8   "integer overflow in expression"  
		{
			del_spray(L, pspr->zone);
			p = L->next;
			continue;
		}
		p = p->next;
	}
}

/************************************************************************/
/* ��������ClearLink													*/
/* ���ܣ��������														*/
/*������L����ʱ��������													*/
/*����ֵ����															*/
/************************************************************************/
void clear_link( SLink *L )
{
	int i = 0;
	SLink *p;
	i = get_length(L);
	while(i)
	{
		p = get_addr(L,i);
		if (NULL != p)
		{
			DelElem(L, i);
		}
		i--;
	}
}

static void AddWaterUsedToLink(SLink *L, WaterUsedStruct wus)
{
	int i = 0;
	pWaterUsedStruct pwus = NULL;

	i = get_length(L) + 1;
	
	pwus = (pWaterUsedStruct)malloc(sizeof(WaterUsedStruct));
	if (pwus != NULL)
	{
		*pwus = wus;
		if(!(ins_elem(L,pwus,i)))
		{
			free(pwus);
			pwus = NULL;
		}
	}
}

void add_water_used( SLink *L, WaterUsedStruct wus )
{
	int i = 0;
	SLink *p = NULL;
	pWaterUsedStruct pwus = NULL;
	
	i = get_length(L);
	
	if (i)
	{
		p = get_addr(L,i);
		if(NULL != p)
		{
			pwus = (pWaterUsedStruct)(p->data);
			if((pwus->zone == wus.zone) && (!memcmp(&(pwus->add_time), &wus.add_time, sizeof(Time))))
			{
				pwus->howlong = wus.howlong;
				pwus->end_time = wus.end_time;
			}
			else 
			{
				 AddWaterUsedToLink(L, wus);
				 i = get_length(L);
				 if(i > 1) //��������3��������ʱ�ѵ�1��д��EEPROM
				 {
					 p = get_addr(L,1);
					 if(NULL != p)
					 {
						 pwus = (pWaterUsedStruct)(p->data);
						 eeprom_write_water_used(L, *pwus);
						 del_first_water_used(L);
					 }
				 }
			}
		}
	}
	else
	{
		AddWaterUsedToLink(L, wus);
	}
}

void del_first_water_used( SLink *L )
{
	int i = 0;
		
	i = get_length(L);
	if (i)
	{
		DelElem(L,1);
	}
}

void del_last_water_used( SLink *L )
{
	int i = 0;
	
	i = get_length(L);
	if (i)
	{
		DelElem(L,i);
	}
}

int schedule_is_legal(Schedule sch)
{
	if ((sch.zones < 1) || (sch.zones > MaxValve))
	{
		return 0;
	}
	
	
	for (int i = 0; i < TimesOfSchedule; i++)
	{
		if (sch.howlong[i] > MaxManuTime)
		{
			return 0;
		}
	}
	
	for (int i = 0; i < (2 * TimesOfSchedule); i++)
	{
		if (i%2 == 0)
		{
			if (sch.times[i] > 23)
			{
				return 0;
			}
			
		}
		else
		{
			if (sch.times[i] > 59)
			{
				return 0;
			}
		}
	}
	
	if (sch.mode > BY_DAYS)
	{
		return 0;
	}
	
	if (sch.xishu > 5)
	{
		return 0;
	}
	
	return 1;
}
