/*
 * IIC.c
 *
 * Created: 2014/5/14 18:58:14
 *  Author: ZLF
 */ 

#include <avr/io.h>
#include "IIC.h"

#ifndef F_CPU
#define F_CPU              12000000UL
#endif
#include <util/delay.h>

void IIC_SCK_H(void){PORTC |= (1 << PC0);}	//ʱ���źŸ�
void IIC_SCK_L(void){PORTC &= ~(1 << PD0);}	//ʱ���źŵ�
void IIC_SDA_H(void){PORTC |= (1 << PC1);}	//�����źŸ�
void IIC_SDA_L(void){PORTC &= ~(1 << PC1);}	//�����źŵ�

/************************************************************************/
/*Description:һ��I2Cʱ������                                            */
/************************************************************************/
void IIC_clock(void)
{
	_delay_us(2);
	IIC_SCK_H();
	_delay_us(2);
	IIC_SCK_L();
}

/************************************************************************/
/*Description:I2C����ʼ�ź�                                              */
/************************************************************************/
void IIC_start(void)
{
	IIC_SDA_H();
	IIC_SCK_H();
	_delay_us(8);
	IIC_SDA_L();
	_delay_us(8);
	IIC_SCK_L();
	_delay_us(8);
}

/************************************************************************/
/*Description:I2C�Ľ����ź�                                                  */
/************************************************************************/
void IIC_stop(void)
{
	IIC_SDA_L();
	IIC_SCK_H();
	_delay_us(8);
	IIC_SDA_H();
	_delay_us(8);
}

/************************************************************************/
/*Description:I2C��Ӧ���ź�                                              */
/*Back:��Ӧ�𷵻�0����Ӧ�𷵻�1                                           */
/************************************************************************/
unsigned char IIC_response(void)	//Ӧ���ź�
{
	unsigned char flag = 0;
	DDRC &= ~(1 << PC1);
	if(PINC & 0x02)
	{
		flag = 1;
	}
	else
	{
		IIC_SCK_H();
		_delay_us(2);
		IIC_SCK_L();
		_delay_us(2);
	}
	DDRC |= (1 << PC1);
	return flag;
}

/************************************************************************/
/*Description:һ��������I2C����Ӧ                                        */
/************************************************************************/
void IIC_not_response(void)
{
	IIC_SDA_H();
	_delay_us(2);
	IIC_clock();
}

/************************************************************************/
/*Description:I2C��д�ֽڲ���                                            */
/*Parameter:��д����ֽ�                                                 */
/************************************************************************/
void IIC_write(unsigned char temp)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	
	dat = temp;
	for(i = 0;i < 8;i++)
	{
		if(dat & 0x80)
		{
			IIC_SDA_H();
		}
		else
		{
			IIC_SDA_L();
		}
		IIC_clock();
		
		dat = dat << 1;
	}
	IIC_SDA_H();
	
	while(IIC_response());
}

/************************************************************************/
/*Description:I2C��һ���ֽ�                                              */
/*Back:�������ֽ�                                                        */
/************************************************************************/
unsigned char IIC_read(void)
{
	unsigned char i = 0;
	char dat = 0;
	
	DDRC &= ~(1 << PC1);
	for(i = 0;i < 8;i++)
	{
		dat = dat << 1;
		IIC_SCK_H();
		if(PINC & 0x02)
		{
			dat |= 0x01;
		}
		else
		{
			dat &= 0xFE;
		}
		IIC_SCK_L();
		_delay_us(2);
	}
	DDRC |= (1 << PC1);
	return dat;
}

/************************************************************************/
/*Description:��ָ��������ָ���Ĵ���дһ��ָ����ֵ������345��5883          */
/*Parameter:�豸��ַ���Ĵ�����ַ���Ĵ���������                             */
/************************************************************************/
void IIC_reg_write(unsigned char address,unsigned char reg,unsigned char dat)
{
	IIC_start();
	IIC_write(address << 1);
	IIC_write(reg);
	IIC_write(dat);
	IIC_stop();
}

/************************************************************************/
/*Description:��ָ��������һ���Ĵ���ֵ������345��5883                     */
/*Parameter:�豸��ַ���Ĵ�����ַ                                          */
/*Back:������ֵ                                                          */
/************************************************************************/
char IIC_reg_read(unsigned char address,unsigned char reg)
{
	char dat = 0;
	
	IIC_start();
	IIC_write(address << 1);
	IIC_write(reg);
	IIC_start();
	IIC_write((address << 1)|(0x01));
	dat = IIC_read();
	//dat = IIC_read();
	IIC_not_response();
	IIC_stop();
	return dat;
}