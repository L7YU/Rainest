
#ifndef CMD_Ctr_H_
#define CMD_Ctr_H_

extern unsigned char set_adjust_flag;
extern unsigned char get_adjust_flag;
extern unsigned char get_weather_flag;
extern unsigned char set_raindelay_flag;
extern unsigned char set_sch_flag;
extern unsigned char net_connect_flag ;	//��������״̬�� 0-δ���ӻ������жϣ� 1-������
extern unsigned char user_band_flag;//�û��Ƿ�󶨱�־ 0-δ�󶨣� 1-�豸�����ɹ�
extern unsigned char conn_cloud_flag ;//�Ƿ������ƶ˳ɹ���־ 0-δ���ӣ� 1-�����ƶ˳ɹ�
extern unsigned char send_devcodes_flag ;//�Ƿ��ϴ��豸��������ƶ˳ɹ���0-δ�ϴ�  1-�ϴ��ɹ�
extern unsigned char get_param_flag ;//�Ƿ��ȡ���� 0-δȡ�ã�1-��ȡ�ɹ�
extern unsigned char get_time_flag;	//�Ƿ��ȡ���ƶ�ʱ�䣬0-δȡ�ã�1-��ȡ�ɹ�
extern unsigned char sentwater_flag;//�Ƿ�����ˮ��Ҫ�ϴ����ƶˣ�1-��Ҫ�ϴ���0-û��Ҫ�ϴ�����ˮ��
extern unsigned char get_domain_flag ;//��ѯ������־ 1-��ȡ�ɹ� 0-δ��ȡ��
extern unsigned char send_work_value_flag; //������ťѡ��ģʽ��־ 0-��Ҫ���� 1-������
extern unsigned char send_spary_status; //���͵�ǰ����·��״̬��־ 0-��Ҫ���� 1-������
extern unsigned char sent_sensor_flag;//���ʹ�������־  0-���ϴ�  1-�ϴ�
extern unsigned char set_del_zone_flag;
extern unsigned char get_schedule_flag;
extern unsigned char sent_zone_time_flag;
extern unsigned char sent_sensor_state;
extern unsigned char get_devstatus_flag;
extern unsigned char return_cid_flag;
extern unsigned char sent_master_state;
extern unsigned char sent_device_type_flag;
extern unsigned char sent_zone_num_flag;
extern unsigned char check_command_flag;  //16.8.17
extern unsigned char return_executed_flag; //16.8.19
//extern const char  mainserve[];

void cmd_ctr(void);//�������̿���
//void set_mainserve(void);

#endif /* CMD_Ctr_H_ */