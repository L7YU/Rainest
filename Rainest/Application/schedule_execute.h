/*
 * schedule_execute.h
 *
 * Created: 2014/7/11 15:42:16
 *  Author: ZY
 */ 


#ifndef SCHEDULE_EXECUTE_H_
#define SCHEDULE_EXECUTE_H_

#include "typedef.h"


void init_list( SLink **L);//�����ʼ��

void updata_sch( SLink *L, Schedule NewSch );//�������ܼƻ�
void insert_sch(SLink *L, Schedule sch);//�������ܼƻ� 
void del_sch( SLink *L, uchar zone );//ɾ�����ܼƻ�
void del_all_sch(SLink *L);// ɾ���������ܼƻ�
int locate_zone(SLink *L,uchar x);	//��ȡ·��Ϊx��λ��
pSchedule get_upload_sch_addr( SLink *L );	//��ȡ��Ҫ�ϴ������ܼƻ���ַ
void sch_inquire(SLink *L,Time tm);//�鿴���ܼƻ��Ƿ���Ҫִ��

void start_spray( uchar zones, uint howlong );	//���ܿ�


void add_spray(SLink *L, Spray spr);
void del_spray(SLink *L, uchar zone);
int locate_spray(SLink *L,uchar zone);
void clean_spray(SLink *L);

void del_first_water_used(SLink *L);
void del_last_water_used(SLink *L);
void add_water_used(SLink *L, WaterUsedStruct wus);

SLink* get_addr( SLink *L,int i );//��ȡλ��Ϊi��Ԫ��ָ��
int get_length( SLink *L );//��ȡ������
void clear_link(SLink *L);
int ins_elem( SLink *L, void * data, int i );
int schedule_is_legal(Schedule sch);
#endif /* SCHEDULE_EXECUT_H_ */