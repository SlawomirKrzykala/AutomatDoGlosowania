/*
 * glosowanie.c
 *
 * Created: 2020-05-24 22:04:03
 * Author : Cichy
 */ 


#define RS 0
#define RW 1
#define E 2
#define LED 3
#define F_CPU 16000000

#define TIME_TO_CHANGE 5;

//Przyciski
#define PORT_BUTTON PORTD
#define DDR_BUTTON DDRD
#define PIN_BUTTON PIND
#define STARTSTOP_BUTTON 0b00100000
#define YES_BUTTON 0b00010000
#define ABST_BUTTON 0b00001000
#define NO_BUTTON 0b00000100


#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"
	

volatile uint16_t timeVoting=0;
volatile uint8_t buttonStatus = 0;
volatile uint8_t timeToChange=0;


void timer1_init()
{
		TCCR1B=(1<<CS12);
		TCNT1=3036;//ustawienie stanu pocz¹tkowego Timea 1
		TIMSK1=(1<<TOIE1);//zezwolenie na przerwanie
		sei();//globalne w³¹czenie przerwañ
}
void timer0_init()
{
	TCCR0B=(1<<CS00)|(1<<CS02);
	TCNT0=0;//ustawienie stanu pocz¹tkowego Timea 0
	TIMSK0=(1<<TOIE0);//zezwolenie na przerwanie
	sei();//globalne w³¹czenie przerwañ
}

void button_init()
{
	DDR_BUTTON&=~(STARTSTOP_BUTTON|YES_BUTTON|ABST_BUTTON|NO_BUTTON);
	PORT_BUTTON|=STARTSTOP_BUTTON|YES_BUTTON|ABST_BUTTON|NO_BUTTON;
}

ISR(TIMER1_OVF_vect)//Wektor przerwania od przepe³nienia Timea 1
{
	TCNT1=3036;//ustawienie stanu pocz¹tkowego Timea 1
	++timeVoting;
	if(timeToChange)--timeToChange;

}

void setPressButton(uint8_t btn)
{
	if (!(buttonStatus&btn))//czy pierwsze wykrycie
	{
			buttonStatus=btn;
			
	}
	else
	{
		if(!(~(buttonStatus)&0b00000011)){//czy juz 5 wykrycie
			buttonStatus=0b10000011|btn;
		}
		else{
			buttonStatus++;
		}
			
	}
}



ISR(TIMER0_OVF_vect)//Wektor przerwania od przepe³nienia Timea 0
{
	
	if(!(buttonStatus&0b11000000))
	{	
	if((~PIN_BUTTON)&STARTSTOP_BUTTON)//start/stop
	{
		setPressButton(0b00000100);	
		//LCD_writeText("P1");
	}
	else if ((~PIN_BUTTON)&YES_BUTTON)//tak
	{
		setPressButton(0b00001000);	
		//LCD_writeText("P2");
	}
	else if((~PIN_BUTTON)&ABST_BUTTON)//wstrz
	{
		setPressButton(0b00010000);	
		//LCD_writeText("P3");
	}
	else if ((~PIN_BUTTON)&NO_BUTTON)//nie
	{		
		setPressButton(0b00100000);	
		//LCD_writeText("P4");
	}
	}
	
	else if ((buttonStatus&0b01000000)&&(PIN_BUTTON&STARTSTOP_BUTTON)&&(PIN_BUTTON&YES_BUTTON)&&(PIN_BUTTON&ABST_BUTTON)&&(PIN_BUTTON&NO_BUTTON)) //zwolnienie przycisku
	{
		if(!(buttonStatus&0b00000011))
		{
			buttonStatus=0b00000000;
		}
		else
		{
			buttonStatus--;
		}
	}	
		
	TCNT0=0;
}

int main(void)
{ 
	uint8_t status=0;// 0 - czek na start |
					// 1- czek na glos | 2 - czek na glos & 1start | 3 - czek na glos & 2start | 4 - czek na glos & 3start >>>5-wyniki |
					// 5 - oddany glos tak | 6 - oddany glos wstrz | 7 - oddany glos nie |
					// 8 - wyniki | 9 - wyniki & 1start | 10 - wyniki & 2start | 11 - wyniki & 3start >>>0-czek na start |

	uint8_t votes[3]={0,0,0};//tak,wstrz,nie
	timer0_init();
	TWI_init();
	LCD_init();
	button_init();

	

	
	while (1)
	{
		switch (status)
		{	

			case 0:	
				LCD_setPosition(0x81);
				LCD_writeText("Czekam na start...");
				LCD_setPosition(0xc1);
				LCD_writeText("created by Cichy");
				if(!((~buttonStatus)&0b10000100))
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					timeVoting=0;
					timer1_init();
					votes[0]=0;
					votes[1]=0;
					votes[2]=0;
					status=1;
					LCD_clear();
				}
				else if(!((~buttonStatus)&0b10000000))
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
				}
			break;
				

			case 1:
			case 2:
			case 3:
			case 4:
				LCD_setPosition(0x81);				
				LCD_writeText("Czekam na akcje...");

				LCD_setPosition(0xc3);
				LCD_writeText("Trwa: ");
				LCD_writeTime(timeVoting);
				
				if(!((~buttonStatus)&0b10001000))//g³os tak
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=5;
					LCD_clear();
					timeToChange=6;


				}
				else if(!((~buttonStatus)&0b10010000))//g³os wstrzymany
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=6;
					LCD_clear();
					timeToChange=TIME_TO_CHANGE;

				}
				else if(!((~buttonStatus)&0b10100000))//g³os nie
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=7;
					LCD_clear();
					timeToChange=TIME_TO_CHANGE;

				}
				else if(!((~buttonStatus)&0b10000100))//przycisk start
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					if(++status>=4)
					{
						TIMSK1=(0<<TOIE1);//ZATRZYMANIE CZASU
						status=8;
						LCD_clear();					
					}					
				}
			break;


			case 5://glos na tak
				LCD_setPosition(0x84);				
				LCD_writeText("Wybrano: TAK");
				LCD_setPosition(0xc1);
				LCD_writeText("Czas na zmiane:  ");
				LCD_writeNumber(timeToChange);
				if(!((~buttonStatus)&0b10010000))//zmiana na wstrzymany
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=6;
					timeToChange=TIME_TO_CHANGE;
					LCD_clear();

				}
				else if(!((~buttonStatus)&0b10100000))//zmiana na nie
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=7;
					timeToChange=TIME_TO_CHANGE;
					LCD_clear();

				}
				else if(!((~buttonStatus)&0b10000000))
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
				}
				if(timeToChange==0)
				{
					++votes[0];
					status=1;
					LCD_clear();
				}
			break;


			case 6://glos na wstrzymano
				LCD_setPosition(0x80);				
				LCD_writeText("Wybrano:  WSTRZYMANO");
				LCD_setPosition(0xc1);
				LCD_writeText("Czas na zmiane:  ");
				LCD_writeNumber(timeToChange);
				if(!((~buttonStatus)&0b10001000))//zmiana na tak
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=5;
					timeToChange=TIME_TO_CHANGE;
					LCD_clear();

				}
				else if(!((~buttonStatus)&0b10100000))//zmiana na nie
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=7;
					timeToChange=TIME_TO_CHANGE;
					LCD_clear();

				}
				else if(!((~buttonStatus)&0b10000000))
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
				}
				if(timeToChange==0)
				{
					++votes[1];
					status=1;
					LCD_clear();
				}
			break;


			case 7://glos na nie
				LCD_setPosition(0x84);				
				LCD_writeText("Wybrano: NIE");
				LCD_setPosition(0xc1);
				LCD_writeText("Czas na zmiane:  ");
				LCD_writeNumber(timeToChange);
				if(!((~buttonStatus)&0b10001000))//zmiana na tak
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=5;
					timeToChange=TIME_TO_CHANGE;
					LCD_clear();

				}
				else if(!((~buttonStatus)&0b10010000))//zmiana na wstrzymany
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					status=6;
					timeToChange=TIME_TO_CHANGE;
					LCD_clear();

				}
				else if(!((~buttonStatus)&0b10000000))
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
				}
				if(timeToChange==0)
				{
					++votes[2];
					status=1;
					LCD_clear();
				}
			break;



			case 8:
			case 9:
			case 10:
			case 11:
				LCD_setPosition(0x80);				
				LCD_writeText("TAK=");
				LCD_writeNumber(votes[0]);
				
				LCD_setPosition(0x87);
				LCD_writeText("WS=");
				LCD_writeNumber(votes[1]);
				LCD_setPosition(0x8D);
				LCD_writeText("NIE=");
				LCD_writeNumber(votes[2]);


				LCD_setPosition(0xc3);
				LCD_writeText("Czas: ");
				LCD_writeTime(timeVoting);
				if(!((~buttonStatus)&0b10000100))//przycisk start
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
					if(++status>=11)
					{
						status=0;
						LCD_clear();
					}
				}
				else if(!((~buttonStatus)&0b10000000))
				{
					buttonStatus|=0b01000000;
					buttonStatus&=0b01111111;
				}
			break;


		}
	}
}
