/*
 * event_handle.h
 *
 * Created: 2014/7/31 15:07:05
 *  Author: ZY
 */ 


#ifndef EVENT_HANDLE_H_
#define EVENT_HANDLE_H_

extern struct timer one_hour_timer ;
extern uchar half_sec_count;
extern uchar one_sec_count;
extern uint valves_temp;


void TimeCtr(void);//ʱ���¼�
void PushEvent(void);//��Ϣ����
void TimeCtr_init(void);//��ʱ��ʼ��
void send_water_data(void);
void upgrade_firmware(void);
void valves_changed(void);

#endif /* EVENT_HANDLE_H_ */