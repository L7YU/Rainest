/*
 * typedef.h
 *
 * Created: 2014/7/2 17:02:20
 *  Author: ZY
 */ 

#ifndef TYPEDEF_H_
#define TYPEDEF_H_

#include <avr/io.h>

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef ul
#define ul unsigned long
#endif

//ϵͳʱ��(Hz)
#ifndef F_CPU
#define F_CPU              12000000UL
#endif

#define  MaxValve				48	//�豸���ɿ���·��
#define  MaxValveManu			48	//�ֶ�����·��
#define  LenOfDevcodes			12	//�豸�ų���
#define  LenOfConnuid			10	//�����볤��
#define  LenOfSerialCode		4	//����볤��
#define  LenOfDevid				8	//�豸ID����
#define  LenOfServe				25	//��������ַ����
#define  LenOfCid				15	//Cid����
#define  LenOfPath				51	//·����󳤶�
#define  MaxScheduleNum			36	//�����������ܼƻ���
#define  TimesOfSchedule		4	//�������ܼƻ�������ʱ����
#define  MaxManuTime			600	//��������ֶ�����ʱ��(min)
#define  MaxAutoTime			600	//���ʱ����ʱ��(min)
#define  MaxRainDelayTime		(72*60)	//����ӳ�����ʱ�� ��λ��Сʱ
#define  MaxtAdjustValue		150	//������΢����
#define  NumOfAdjustDays		5	//΢������������
#define  MaxWMSpray				3	//������ͬʱ����·��
#define  WaterUsedNum			(50-1) //������󱣴����ܼ�¼����
#define  MasterZone				12	//������
#define  SprayEepromNum			80

typedef  struct SprayTimely_Struct {
	uint whichvalve;
	uint howlong;
}SprayTimely,*pSprayTimely;


typedef struct Schedule_Struct{
	uchar upload_status;	//���ܼƻ��ϴ�״̬
	uchar is_work;	//���ܼƻ���ǰ״̬ 0-���� 1-ͣ��
	uchar is_weather;//�Ƿ������������ 0-���� 1-�����ã�Ϊ100%
	uchar weeks;//һ�����Ǽ�����,λ��־��0~6λ�ֱ��Ӧ��һ������
	uchar times[2 * TimesOfSchedule];//ÿ�����ܵ�ʱ��㣬10���Ʊ�ʾ�������ֽڱ�ʾһ��ʱ�䣬��4��ʱ��� 08 ��20
	uchar zones;//����·�� 1~12·
	uint howlong[TimesOfSchedule]; //ÿ����ʱ������ʱ��,��λ��min
	uchar mode;   //ִ��ģʽ��0-������ִ��  1-����ִ��
	float xishu; //����ʱ������ϵ��
	long days;	//�����Ʊ�ʾ�����λΪ��1��
	}Schedule,*pSchedule;
	
typedef struct 	DevParam_Struct{
// 	uchar hassensor;//�Ƿ��д�����
// 	uchar sensorinterv;//�������ϴ�����ʱ����,��λСʱ
// 	char fwver[4];//�汾��
	uchar mwspray;//�豸��ͬʱ���ܵ����·�� [1~3]
// 	uchar zonecount;//�豸�������ӵķ�����
	uchar timezone;//ʱ��ID
// 	uchar isweather;// ON/OFF weather system 0-by manual, 1-by weather
// 	int maxelectric;//�豸������
	uchar mastertag;//�Ƿ�����master����
	uchar tem_unit_flag;//�¶ȵ�λ
	uchar program_id;
	}DevParam,*pDevParam;
	
#ifndef _TM_DEFINED
#define _TM_DEFINED
typedef struct tm_struct {
	uchar sec;     // seconds after the minute - [0,59] 
	uchar min;    //  minutes after the hour - [0,59] 
	uchar hour;   //  hours since midnight - [0,23] 
	uchar mday;  //	day of the month - [1,31] 
	uchar mon;   // months since January - [1,12] 
	uchar year;    // years since 2000 
	uchar wday;  // days since Sunday - [1,7] 
}Time, *pTime;
#endif

typedef struct SchRemainTime{
	uchar hour;
	uchar min;
	uint zone;
}SchReTime;

typedef struct SentWater_Struct{
	uint howlong;
	uint zone;	
	uchar type; //0��ʾ��ʱ���ܣ�1�ֶ����ܣ�2���ܼƻ�,3������
	uchar adjust;//΢��ֵ
	Time add_time;//��ʼִ������ʱ���¼
	Time end_time;//��������ʱ���¼
}WaterUsedStruct,*pWaterUsedStruct;

typedef struct Spray_Struct{
	uchar zone;//����·��
	uchar adjust;//΢��ֵ
	uchar type; //0��ʾ��ʱ���ܣ�1�ֶ����ܣ�2���ܼƻ���3������
	uchar work_status;	//���ܼƻ���ǰ״̬ 0-δִ�� 1-����ִ�� 2-ִ�����
	uint howlong;//����ʱ��(�ݼ�)
	uint src_time;//����ԭ�趨ʱ��
	Time add_time;//�������ʱ���¼
}Spray,*pSpray;

typedef struct RainDelay_Struct{
	int delay_time;
	Time begin_time;
}RainDelay, *pRainDelay;

typedef struct FineTune_Struct{
		uchar minadjust[5];
		uchar adjust_mode;		//By Weather or by manual, λ��ʾ��
										//�ӵ���λ��ʼ������λ�����α�ʾ1~5���΢��ģʽ
		Time  set_t;					//����ʱ��
}FineTune, *pFineTune;
	
typedef struct Weather_Struct{
	int tempminF;
	int tempmaxF;
	int tempminC;
	int tempmaxC;
	uchar raining;
	}Weather;
	
typedef struct node{
	void* data;
	struct node *next;
}SLink;

/************************************************************************/
/*                                           CMD						*/
/************************************************************************/
#define	ConnCloud				1		//�Ʒ�������
#define	SendDevcodes			2		//�豸�ϴ�������
#define	GetDevStatus			3		//����豸����״̬
#define	SendZoneNum				4		//�豸�ϴ����õ�·�����(û����)
#define  GetCommand				5		//��ȡapp��ָ��
#define  SetDevParam			6		//�豸���ò���
#define	GetDevParam				7		//��ȡ�豸����
#define	UpDateDevParam			8		//�޸��豸����
#define  GetUserInfo			9		//��ȡ�û���Ϣ
#define	GetNewSoft				10		//�豸���������
// #define  GetSchF				11		//������ܼƻ�(��FlagΪ����)
#define  GetDateTime			12		//�豸��ȡ������ʱ��
#define	SetWater				13		//�豸�ϴ���ˮ��
// #define	SetHumidity			14		//�豸�ϴ�����ʪ��
#define  GetSchS				15		//������ܼƻ�(��IDΪ����)
#define  GetDomain				16		//�豸��ѯ����
#define  SendDevStatus			17		//�ϴ���ť����״̬
#define  SendDevEvent			18		//�ϴ����ܿ�״̬
#define  GetWeather				19		//��ȡ����״̬
#define  SetRainDelay			20		//�ϴ��ӳ�����
#define  SetSchWeeks			21		//�ϴ����ܼƻ�-����
#define  SetSchTimes			22		//�ϴ����ܼƻ�-��ʱʱ��
#define  DeleteSch				23		//ɾ�����ܼƻ�
#define  SetAdjust				24		//�ϴ�΢������
#define  GetAdjust				25		//��ȡ΢������
#define  SentZoneTime			26		//����·��ʱ��
#define  SentSensor				27		//���ʹ�����ֵ
#define  SchSetDays				28		//����������
#define	 SetSch					29		//�ϴ����ܼƻ�
#define  ReturnCid			    30      //����CID
#define  SetMaster				31		//����Master
#define  SetDevType				32		//�����豸�ͺ�
#define  CmdFree				33      //�����ƶ˲鿴���޶�ʧ����  //16.8.17
#define  ReturnExecuted			34		//����ִ�гɹ�ȷ�Ϸ���		//16.8.19


/************************************************************************/
/*                                   Mode	Select						*/
/************************************************************************/
#define	 SystemCheckMode			0
#define  AutoMode					4
#define  OffMode					5
#define	 ManuWaterMode				3
#define  Configration				6
#define  ScheduleView				1
#define  RainDelayView				2
#define  WeatherAdjust				8
#define  DeviceParam				9
#define  SpecialFeatures			7

/************************************************************************/
/*                                  Operate Mode						*/
/************************************************************************/
#define  Operate1			1
#define  Operate2			2
#define  Operate3			3
#define	 Operate4			4

/************************************************************************/
/*                                  Time Mode						*/
/************************************************************************/
#define  TimeMode_12		12
#define  TimeMode_24		24

/************************************************************************/
/*                                  EEPROM Addr							*/
/************************************************************************/
#define  IsHaveSensor			 0
#define  BackLightAddr			(IsHaveSensor + sizeof(char))
#define  TimeModeAddr			(BackLightAddr + sizeof(char))  //16.9.21 ʱ��ģʽ��ַ
#define  ServeAddr				(TimeModeAddr + sizeof(char))
#define  BandStatusAddr			(ServeAddr + sizeof(char) * LenOfServe)		//�豸��״̬ 1���ֽ�
#define  DevcodeAddr			(BandStatusAddr+sizeof(char))
#define  DevcodeFlagAddr		(DevcodeAddr +sizeof(char) * LenOfDevcodes)
#define  SerialCodeAddr			(DevcodeFlagAddr + sizeof(char))	//6���ֽ�
#define  DevParamAddr			(SerialCodeAddr+sizeof(char)*LenOfSerialCode)	//�豸������ַ(17���ֽ�)
#define  RainDelayAddr			(DevParamAddr+sizeof(DevParam))
#define  ManuWaterSetAddr		(RainDelayAddr+sizeof(RainDelay))	//�ֶ��������ñ���
#define  FineTuneAddr			(ManuWaterSetAddr + MaxValveManu*2)//΢�����ñ���
#define  DeleteSchAddr			(FineTuneAddr + sizeof(FineTune))	//��¼ɾ����·������16·
#define	 EpScheduleHeadAddr		(DeleteSchAddr+ 6)                 //���ܼƻ�����·��    //16.7.6 ��ַ�����޸�
#define  EpSchedule1Addr		(EpScheduleHeadAddr + 1)			//���ܼƻ��׵�ַ

// #define	 ScheduleNumAddr		(DeleteSchAddr+ 6)	//���ܼƻ�������ַ
// #define	 Schedule1Addr			(ScheduleNumAddr + 1)	//���ܼƻ��׵�ַ

#define  SparyHeadAddr			(EpSchedule1Addr + sizeof(EpSchedule) * (MaxValve+1))  
#define  SparyTagAddr			(SparyHeadAddr + 1)
#define  SparyFirstAddr			(SparyTagAddr + 1)

#define  WaterUsedHead			(SparyFirstAddr + sizeof(Spray)*(SprayEepromNum+1))
#define  WaterUsedTag			(WaterUsedHead + 1)
#define  WaterUsed1Addr			(WaterUsedTag + 1)


#define  SprayLimitTimeAddr		(E2END - 4)
#define	 FirstExeFlagAddr		(E2END-1)
#define  BootFlag				E2END

/************************************************************************/
/*                                   Net_Connect_Flag					*/
/************************************************************************/
#define  NetDisConnected			0
#define  NetConnected				1

/************************************************************************/
/*                                   Conn_Cloud_Flag					*/
/************************************************************************/
#define  DisConnCloud				0
#define  ConnCloudOk				1
#define  ConnCloudOff				2    //2016.9.16  

/************************************************************************/
/*                                   Send_Devcodes_Flag					*/
/************************************************************************/
#define  DisSendDevcodes			0
#define  SendDevcodesOk				1

/************************************************************************/
/*                                   User_Band_Flag						*/
/************************************************************************/
#define  DisBanded						0
#define  UserBandOk						1

/************************************************************************/
/*                                   Get_Time_Flag						*/
/************************************************************************/
#define  GetTimeFailed				1
#define	 GetTimeOK					0

/************************************************************************/
/*                                   WateringFlag						*/
/************************************************************************/
#define  WateringOFF					0
#define	 WateringStrat					1

/************************************************************************/
/*                                   Get_Param_Flag						*/
/************************************************************************/
#define  DisGetParam					1
#define	 GetParamOK						0

/************************************************************************/
/*                                   SentWater_Flag						*/
/************************************************************************/
#define  NoDataToSend					0
#define	 ReadyToSend					1

/************************************************************************/
/*                                   Updata_Display_Flag				*/
/************************************************************************/
#define  UpdataDisplayOk					0
#define	 UpdataDisplay						1

/************************************************************************/
/*                                   Get_Domain_Flag					*/
/************************************************************************/
#define  DisGetDomain						0
#define	 GetDomainOK						1

/************************************************************************/
/*                                   Send_work_value_Flag				*/
/************************************************************************/
#define  SentWorkMode						0
#define	 SentWorkModeOK						1

/************************************************************************/
/*                                   Send_Spary_Status_Flag				*/							 
/************************************************************************/
#define  SentSparyStatus					0
#define	 SentSparyStatusOK					1

/************************************************************************/
/*                                   sent_zone_time_flag				*/
/************************************************************************/
#define  NeedSentZoneTime					0
#define	 SentZoneTimeOK						1

/************************************************************************/
/*                                   sent_sensor_flag					*/									 
/************************************************************************/
#define  SentSensorStatusOk					0
#define	 SentSensorStatus					1

/************************************************************************/
/*                                   get_weather_flag					*/									 
/************************************************************************/
#define  NoGetWeather							0
#define	 GetWeatherOk							1

/************************************************************************/
/*                                   set_raindelay_flag					*/									
/************************************************************************/
#define  SetRainDelayOK						0
#define	 NeedSetRainDelay					1

/************************************************************************/
/*                                   isweather							*/
/************************************************************************/
#define  AdjustByManual						0
#define	 AdjustByWeather					1

/************************************************************************/
/*                                   set_adjust_flag		 			 */
/************************************************************************/
#define  SetAdjustOK							0
#define	 NeedSetAdjust							1

/************************************************************************/
/*                                   get_adjust_flag		 			*/
/************************************************************************/
#define  DisGetAdjust							0
#define	 GetAdjustOK								1
/************************************************************************/
/*                                   Get_Schedule_Flag					*/
/************************************************************************/
#define  GetScheduleOK					0
#define  UpdataAllSch					1

/************************************************************************/
/*                                   set_sch_weeks_flag					*/
/************************************************************************/
#define  SetSchOK								0
#define	 NeedSetSch								1

/************************************************************************/
/*                                   adjust_mode						*/
/************************************************************************/
#define  ByWeather									0
#define	 ByManual									1

/************************************************************************/
/*                                   sensor_value						*/
/************************************************************************/
#define  NeedSentSensor								0
#define	 SentSensorOk								1
#define  SensorOff									0
#define  SensorOn									1

/************************************************************************/
/*                                   sch_upload_status					*/
/************************************************************************/
#define  NoUpload									0
#define	 Uploading									1
#define  Uploaded									2

/************************************************************************/
/*                                   work_status						*/
/************************************************************************/
#define  NoExecute									0
#define	 Executing									1
#define  ExeCompleted								2

/************************************************************************/
/*                                   spray_value						*/
/************************************************************************/
#define KEY1								1	//zone1~4
#define KEY2								2  //zone5~8
#define KEY3								3  //zone9~12

/************************************************************************/
/*                                  return_cid						    */
/************************************************************************/
#define NeedReturnCid						1
#define ReturnCidOK						    2

/************************************************************************/
/*                                  return_executed					    */
/************************************************************************/
#define NeedReturnExecuted					1
#define ReturnExecutedOK					2


/************************************************************************/
/*                                  check_command					    */ //16.8.17
/************************************************************************/
#define NeedCheckCmd					1
#define CheckCmdOk						2

/************************************************************************/
/*                                   page		ID						*/
/************************************************************************/
#define  ConnNetPage				0
#define	 ConnCloudPage				1
#define  UserBandPage				2
#define  AutoModePage				3
#define  WaterAutoPage				4
#define  ScheduleMainPage			5
#define  ManuWaterPage				6
#define  ManuWaterStartPage			7
#define  RainDelayPage				8
#define  ConfigPage					9
#define  OffModePage				10
#define  FineTunePage				11
#define  DeviceParamPage			12
#define  SetupDateTimePage			13
#define  WifiAccessPage				14
#define  RegSmartDevPage			15
#define  SchSelValvePage			16
#define  SchSetTimePage				17
#define  SchSetWeeksPage			18
#define  SpecialFeatPage			19
#define  SchDelPage					20
#define  SchPlsSetTimePage			21
#define  SchPlsSetWeekPage			22
#define  SchNoPlanPage				23
#define  SchConfirmCoverPage		24
#define  MasterConfigPage			25
#define  SchSetDaysPage				26
#define  SchSelModePage				27
#define  SchSprayLimitPage			28
#define  ManuWaterLimitPage			29

/************************************************************************/
/*                                  Weeks								*/
/************************************************************************/
#define  MON			1
#define  TUES			2
#define  WED			3
#define  THURS			4
#define	 FRI			5
#define  SAT			6
#define  SUN			7

/************************************************************************/
/*                                  Config_Confirm_Key					*/
/************************************************************************/
#define  CANCEL						0
#define  CONFIRM					1

/************************************************************************/
/*                                  visit_cloud_time_flag				*/
/************************************************************************/
#define  Fast						Sec_3
#define  Slow						Sec_6
#define  VerySlow					Sec_10

/************************************************************************/
/*                                  Setup Date Time						*/
/************************************************************************/
#define  SetHour					0
#define  SetMin						1
#define  SetTimeMode				2
#define  SetWday					3
#define  SetMon						4
#define  SetMday					5
#define  SetYear					6


/************************************************************************/
/*                                  time interrupt						*/						
/************************************************************************/
#define  Sec_half							100
#define  Sec_1								200
#define  Sec_2								400
#define  Sec_3								600
#define  Sec_4								800
#define  Sec_5								1000
#define  Sec_6								1200
#define  Sec_8								1600
#define  Sec_7								1400
#define  Sec_10								2000
#define  Sec_15								3000

/************************************************************************/
/*                                       error type						*/					
/************************************************************************/
#define  SetDevcodeErr			5000
#define  SetnoUidErr			5102
#define  DevBoundedErr			5104
#define  GetWeatherErr			5105
#define  GetTimeUidErr			5200
#define  DevidErr				5800
#define  DevIsolatedErr			5801
#define  GetCdUidErr			5802
#define  GetStatusUidErr		6200
#define  GetDomainUidErr		6600
#define  GetWeatherUidErr		7001
#define  DelSchIDErr			7302
#define  SentSchErr				7501

/************************************************************************/
/*                                      ��������  						*/
/************************************************************************/
#define	 SparyByApp			0
#define  SparyByManu		1
#define	 SparyBySch			2
#define  SparyBySensor		3
#define  SparyByDelay		4
#define  SparyUndef			5

/************************************************************************/
/*                                      ETH STATE						*/
/************************************************************************/
#define ETH_DISCONN		0
#define ETH_CONN		1

/************************************************************************/
/*                                      NET MODE						*/
/************************************************************************/
#define WIFI_MODE	 0
#define ETH_MODE	 1

/************************************************************************/
/*                                    SCHDEULE MODE						*/
/************************************************************************/
#define BY_WEEKS	 0
#define BY_DAYS		 1

/************************************************************************/
/*                                    GET DEVSTATUS						*/
/************************************************************************/
#define DisGet		0
#define OkGet		1

/************************************************************************/
/*                                    COMM FLAG							*/
/************************************************************************/
#define SENT_FAILED	 0
#define SENT_OK		 1

/************************************************************************/
/*                                  PROMGRAM ID							*/
/************************************************************************/
#define PROMGRAM_A	 1
#define PROMGRAM_B	 2
#define PROMGRAM_C	 3
#define PROMGRAM_D	 4

//extern char serve[LenOfServe];//������IP
extern char content[LenOfPath];//WEB��ַ
extern char devcode[LenOfDevcodes+1];	//�豸���к�
extern char serialcode[LenOfSerialCode+1]; //�豸�����
extern char connuid[LenOfConnuid+1];//l������

extern volatile unsigned char webclient_flag;
extern volatile unsigned char work_value ;	//��ť����ѡ��
extern volatile unsigned char operate_value ;//�ĸ�����
extern volatile unsigned char spray_value ;//�˸�����
extern volatile unsigned char sensor_value;	//������ֵ

extern volatile unsigned int visit_cloud_time;//���ʽӿ�ʱ��

extern DevParam devparam;//�豸����

extern Time volatile current_time;//��ǰʱ��

extern int version;//�汾��

extern unsigned char net_mode;

extern unsigned int spray_time_limit;

extern int valves; //����·��

extern unsigned char timemode_flag;
#endif /* TYPEDEF_H_ */