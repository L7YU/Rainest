/*
 * data_rw.h
 *
 * Created: 2014/7/9 20:26:53
 *  Author: ZY
 */ 


#ifndef DATA_READ_H_
#define DATA_READ_H_

#include "typedef.h"
#include <stdint.h>

extern DevParam devparam;//�豸����
extern SLink *sch_link;
extern RainDelay rain_delay;

void read_param(void);//��ȡ����
void get_code(void);//����豸��������
//void read_schedule(void);//��ȡ���ܼƻ�            //16.7.6 ����ʹ��
//void save_schedule( SLink *L );//�������ܼƻ�
//void reset_sch_num(void);//�������ܼƻ�����
void get_devcode(char * code);//��ȡSIN
void get_serialcode( char * code );//��ȡ�����
void save_band_status(void);//�����û���״̬
pWaterUsedStruct get_water_used(SLink *L);//��ȡ����·������ͬ�������ˮ��
void reset_water_used_eeprom( uchar i );//����ĳ·��ˮ��
void save_raindelay( void );//�����ӳ�ʱ��
void save_manuwater_set(void);//�����ֶ���������
void recovery_system(void);//�ָ���������
void save_finetune( FineTune ft);//����΢������
void read_finetune(void);//��ȡ΢������
//void update_schedule(uchar i,Schedule NewSch);   //16.7.6 ����ʹ��
uchar get_del_zone(void);
void set_del_zone(uchar zone);
void reset_del_zone(uchar zone);
void save_devparam( void );
void reset_band_status( void );

void eeprom_read_water_used(SLink *L);
void eeprom_write_water_used(SLink *L, WaterUsedStruct wus);
void save_all_water_used(SLink *L);//������ˮ����EEPROM
void get_mac(unsigned char *mac);
void save_mac( char *mac );
unsigned char get_mac_flag(void);
void set_backlight_flag(unsigned char data);
int get_backlight_flag(void);
void set_timemode_flag(unsigned char timemode);
void get_timemode_flag(unsigned char * timemode);
int get_version_id(void);
void get_vesion_str(char *res) ;
void save_domain(char *serve);
void read_daomain(char *dest);
void read_spray_time_limit(void);
void save_spray_time_limit(uint16_t howlong);
void save_spray_link( SLink *L );
void eep_update_spray(Spray spr);
void eep_spray_delete_first(void);
void eep_save_spray(Spray spray);       //16.7.7  implicit declaration of function 'eep_save_spray'
void set_mac_flag(unsigned char flag);  //16.7.7
  
int add_epschedule(SLink *L,Schedule sch_new);                       //�������ܼƻ�            //16.7.6 �����
int updata_epschedule(SLink *L,Schedule sch_new); //����eeprom���ܼƻ�����
void insert_epschedule(SLink *L,Schedule sch_new);  //��������eeprom���ܼƻ�
int delete_epschedule(SLink * L,unsigned char zone);//ɾ��eeprom���ܼƻ�����
void read_epschedule(void);							//��ȡeeprom���ܼƻ�����
void reset_epschedule(void);						//���eeprom���ܼƻ�
#endif /* DATA_READ_H_ */