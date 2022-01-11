
void uintToTabChar(uint8_t num, uint8_t *tabChar, uint8_t size)
{
	
	do
	{
		tabChar[--size]=(num%10)+'0';
		num=num/10;
	}while (size!=0);
}

void timeToTabChar(uint16_t s, uint8_t *tabChar)
{
	
	uint8_t div=s/3600;
	uint8_t tabCharTmp[2];
	uintToTabChar(div, tabCharTmp,2);
	tabChar[0]=tabCharTmp[0];
	tabChar[1]=tabCharTmp[1];
	tabChar[2]=58;
	
	s-=3600*div;
	div=s/60;
	uintToTabChar(div, tabCharTmp,2);
	tabChar[3]=tabCharTmp[0];
	tabChar[4]=tabCharTmp[1];
	tabChar[5]=58;
	
	s-=60*div;
	uintToTabChar(s, tabCharTmp,2);
	tabChar[6]=tabCharTmp[0];
	tabChar[7]=tabCharTmp[1];
	tabChar[8]='\0';
}

void TWI_init(void)
{
	TWBR = 16;
	TWCR = (1<<TWEA);
}

void TWI_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

void TWI_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ((TWCR & (1<<TWSTO)));
}

void TWI_write(uint8_t byte)
{
	TWDR = byte;
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
}

void LCD_write(uint8_t data, uint8_t rs)
{
	uint8_t co=0;
	co |= (rs<<RS)|(1<<E)|(1<<LED) | (data & 0xf0);
	TWI_write(co);
	co &= ~(1<<E);
	TWI_write(co);
	co = 0;
	co |= (rs<<RS)|(1<<E)|(1<<LED) | ((data<<4) & 0xf0);
	TWI_write(co);
	co &= ~(1<<E);
	TWI_write(co);
}

void LCD_init(void)
{
	TWI_start();
	TWI_write(0x4e);// adres do zapisu
	LCD_write(0x33,0);
	LCD_write(0x32,0);
	LCD_write(0x28,0);
	LCD_write(0x06,0);
	LCD_write(0x0c,0);
	LCD_write(0x01,0);
	TWI_stop();
	_delay_ms(2);
}

void LCD_writeText(uint8_t *napis)
{
	uint8_t i=0;
	TWI_start();
	TWI_write(0x4e);
	while(napis[i]!='\0')
	{
		if(napis[i]=='\n')
		{
			LCD_write(0xc0,0);
			++i;
		}
		LCD_write(napis[i++],1);
	}
	TWI_stop();
	
}

void LCD_writeNumber(uint8_t num)
{
	uint8_t numTabChar[8]={'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
	uint8_t n=0;
	uint8_t numTMP=num;
	
	do
	{
		numTMP/=10;
		++n;
	}while (numTMP);
	uintToTabChar(num, numTabChar, n);
	LCD_writeText(numTabChar);
	
}

void LCD_writeTime(uint16_t time)
{
	uint8_t timeTabChar[8];
	timeToTabChar(time, timeTabChar);
	LCD_writeText(timeTabChar);
}

void LCD_setPosition(uint8_t position)
{
	TWI_start();
	TWI_write(0x4e);// adres do zapisu
	LCD_write(position, 0);
	TWI_stop();
}

void LCD_clear()
{
	TWI_start();
	TWI_write(0x4e);// adres do zapisu
	LCD_write(0x01, 0);
	TWI_stop();
	_delay_ms(2);

}