/*
 * json.h
 *
 * Created: 2014/6/4 10:20:01
 *  Author: ZY
 */ 


#ifndef JSON_H_
#define JSON_H_

#include "typedef.h"
#include "schedule_execute.h"

extern Weather weather;
extern pSchedule sch_upload;
extern uchar delete_zone;
extern const char  urlhead[];

void parse_cloud_API(const char *data ,const unsigned int len);//����JSONָ�����
int Js_parser_array( char *res, const char *buffer, char *key);//��ȡJSONָ���е�����
int Js_parser_value_int( int *res, const char *buffer, char *key);//��ȡJSONָ���е�����
int Js_parser_value_char( unsigned char *res,const char *buffer, char *key);//��ȡJSONָ���е�����
int Js_parser_string(char *res,const char *buffer, char *key);//��ȡJSONָ���е��ַ���
unsigned long prarser_time(char * const buffer,char *key);//��ȡJSONָ���е��ƶ�ʱ��
Time prarser_datetime(char * const buffer,char *key );//��ȡJSONָ���е��ƶ�ʱ��
unsigned int tobit(char * const whichvalve);//������·���������ʾת��Ϊ16λλ��ʾ
int parser_open_valve(char * const buffer);//������ˮ������
int parser_close_valve(char * const buffer);//������ˮ������
int parser_schedule(Schedule *psch, char * const buffer );//�������ܼƻ�����
int parser_dev_param(pDevParam devparam, char * const buffer );//�����豸����
void setw_handle( void );//�ϴ���ˮ���ɹ�����
void get_cmd( unsigned char flag);//��ȡ�����ƶ˽ӿ�ָ��
void get_cid(char * const data);

#endif /* JSON_H_ */