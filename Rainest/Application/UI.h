/*
 * UI.h
 *
 * Created: 2014/6/30 14:02:01
 *  Author: ZY
 */ 


#ifndef UI_H_
#define UI_H_

#include "typedef.h"

extern  uchar updata_display_flag; //������ʾ 0-������ 1-����
extern uchar CurrentPage;//��ǰ��ʾҳ��ID
extern uchar NextPage;//�¸���ʾҳ��ID
extern uchar MCDwarning_flag;

void UI_Display(void);//LCD��ʾ����

#endif /* UI_H_ */