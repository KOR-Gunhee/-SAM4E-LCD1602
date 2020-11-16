#include "sam.h"

#define PORTA_Ena(data)		PIOA->PIO_PER=(1<<data);	//PIOA I/O 사용 설정
#define PORTA_Out(data)		PIOA->PIO_OER=(1<<data);	//PIOA 출력 사용 설정
#define PORTA_In(data)		PIOA->PIO_ODR=(1<<data);	//PIOA 입력 사용 설정
#define PORTA_Set(data)		PIOA->PIO_SODR=(1<<data);	//PIOA data set 설정
#define PORTA_Stat			PIOA->PIO_PDSR;				//PIOA data 읽어오기
#define PORTD_Ena(data)		PIOD->PIO_PER=(1<<data);	//PIOD I/O 사용 설정
#define PORTD_Out(data)		PIOD->PIO_OER=(1<<data);	//PIOA 출력 사용 설정
#define PORTD_In(data)		PIOD->PIO_ODR=(1<<data);	//PIOD 입력 사용 설정
#define PORTD_Set(data)		PIOD->PIO_SODR=(1<<data);	//PIOD data set 설정


void PIO_Init()
{
	//PORTA 초기화
	PMC->PMC_PCER0|=(1<<ID_PIOA);
	PORTA_Ena(0x0F);
	PORTA_In(0x0F);
	
	//PORTD 초기화
	PMC->PMC_PCER0|=(1<<ID_PIOD);
	PORTD_Ena(0xFF);
	PORTD_In(0xFF);
}

void MyDelay(volatile uint32_t delay)
{
	while(delay)
	{
		delay--;
	}
}
void MyDelay2(volatile uint32_t delay1,volatile uint32_t delay2)
{
	while(delay1)
	{
		MyDelay(delay2);
		delay1--;
	}
}

//텍스트 LCD로 부터 상태(명령)를 읽는 함수
unsigned char LCD_rCommand()
{
	unsigned char temp=1;

	PORTD_In(0xFF);

	PORTA_Set(0x06);	//명령 읽기 동작 시작	(0Bit clr, 1Bit set, 2Bit Set)
	MyDelay2(1000,10);

	temp=PORTA_Stat;		//명령 읽기
	MyDelay2(1000,10);

	PORTA_Set(0x02);	//명령 읽기 동작 끝		(0Bit clr, 1Bit set, 2Bit clr)

	PORTD_Out(0xFF);
	MyDelay2(1000,10);

	return temp;
}

//텍스트 LCD의 비지 플래그 상태를 확인하는 함수
char LCD_BusyCheck(unsigned char temp)
{
	if (temp & 0x80) return 1;
	else return 0;
}

//텍스트 LCD에 명령을 출력하는 함수 - 단, 비지플래그 체크하지 않음
void LCD_wCommand(char cmd)
{
	PORTA_Set(0x04);	//명령 쓰기 동작 시작	(0Bit clr, 1Bit clr, 2Bit set)

	PORTD_Set(cmd);
	MyDelay2(1000,10);

	PORTA_Set(0x00);	//명령 쓰기 동작 끝		(0Bit clr, 1Bit clr, 2Bit clr)
	MyDelay2(1000,10);
}

//텍스트 LCD에 명령을 출력하는 함수 - 단, 비지플래그 체크 함
void LCD_wBCommande(char cmd)
{
	while(LCD_BusyCheck(LCD_rCommand()))
	MyDelay2(1000,10);

	PORTA_Set(0x04);	//명령 쓰기 동작 시작	(0Bit clr, 1Bit clr, 2Bit set)

	PORTD_Set(cmd);
	MyDelay2(1000,10);

	PORTA_Set(0x00);	//명령 쓰기 동작 끝		(0Bit clr, 1Bit clr, 2Bit clr)
	MyDelay2(1000,10);

}

//텍스트 LCD를 초기화하는 함수
void LCD_Init()
{
	MyDelay2(1000,100000);
	
	LCD_wCommand(0x30);		//비지 플래그를 체크하지 않는 Function Set
	MyDelay2(1000,10000);

	LCD_wCommand(0x30);		//비지 플래그를 체크하지 않는 Function Set
	MyDelay2(1000,200000);

	LCD_wCommand(0x30);		//비지 플래그를 체크하지 않는 Function Set
	MyDelay2(1000,200000);

	LCD_wBCommande(0x38);	//비지 플래그를 체크하는 Function Set
	LCD_wBCommande(0x0c);	//비지 플래그를 체크하는 Display On/Off Control
	LCD_wBCommande(0x01);	//비지 플래그를 체크하는 Clear Display
}

//텍스트 LCD에 1바이트 데이터를 출력하는 함수
void LCD_wData(char dat)
{
	while(LCD_BusyCheck(LCD_rCommand()))
	MyDelay2(1000,10);

	PORTA_Set(0x00000005);	//데이터 쓰기 동작 시작		(0Bit set, 1Bit clr, 2Bit set)
	
	PORTD_Set(dat);
	MyDelay2(1000,10);

	PORTA_Set(0x00000001);	//데이터 쓰기 동작 끝		(0Bit set, 1Bit clr, 2Bit clr)
	MyDelay2(1000,10);
}

//텍스트 LCD에 문자열을 출력하는 함수
void LCD_wString(char *str)
{
	while(*str)
	LCD_wData(*str++);
}

int main(void)
{
	SystemInit();
	WDT->WDT_MR|=(1<<15);

	PIO_Init();
	LCD_Init();

	LCD_wBCommande(0x80|0x00);	//DDRAM Address = 0 설정
	LCD_wString("HELLO WORLD!");	//텍스트 LCD 문자열 출력

	LCD_wBCommande(0x80|0x40);	//DDRAM Address = 0 설정
	LCD_wString("GOOD BYE!");	//WESNET 문자열 출력


	return 1;
	while (1)
	{
	}
	
}
