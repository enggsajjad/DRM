#define Putc LcdWriteChar
#define  LED_ON			1
#define  LED_OFF		0
sbit BS  = P0^7;
sbit RS = P3^5;
sbit RW = P3^6;

sbit WR1_DAC = P1^0;
sbit Edge29 = P1^1;
sbit CSDAC = P1^5;
sbit EN = P1^6;
sbit CS =	P1^7;

sbit LED1_MF = P2^7;
sbit LED2_AL1 = P2^6;
sbit LED3_AL2 = P2^5;
sbit LED4_CC = P2^4;
sbit RELAY1_S2 = P2^2;
sbit RELAY2_MF = P2^1;
sbit RELAY3_S1 = P2^0;

unsigned char CntMSec=0;
unsigned char Key;
unsigned char ccon=0;
unsigned int TF16=0;
unsigned int TF16_Old=0;
unsigned int Capture16=0;
unsigned int PulseWidth=0;
unsigned char capLow;
unsigned char capHigh;
unsigned long Count32;
unsigned long Thresh[2];
unsigned int rCapture16=0;
unsigned long rCount32;
unsigned long Num = 1;
unsigned long D;
unsigned char selThresh=2;
char r;
unsigned char Point;
//bit NegEdge;
bit isInit=0;
//unsigned char Hex[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
unsigned char Mapping[] = {1,2,3,0x43,4,5,6,0x4C,7,8,9,0x55,0x23,0,0xFE,0x59};
unsigned char dac_value=0;
unsigned char state = 100;
unsigned char command = 0;
unsigned char next = 8;
long factor;
//Function Prototypes
void LcdInit();
void DelayUs(void);
void LcdClear(void);
void LcdWriteStr(char *var);
void LcdWriteCmd(unsigned char c);
void WriteDAC(unsigned char DACIn);
void LcdWriteChar(unsigned char var);
void LongToAscii(unsigned long Value);
void LcdGotoXY(unsigned char r,unsigned char c); 

void WriteDAC(unsigned char DACIn) 
{
	EN = 0;
	P0 = DACIn;
	CSDAC=0;WR1_DAC = 0;
	WR1_DAC = 1;CSDAC=1;
	EN = 1;
}
void CountsToFreq(unsigned long C)
{
	unsigned long Q,R;
	Q = 4000000/C;
	R = 4000000%C;
	LongToAscii(Q);
	R = R*1000;
	Q = R/C;
	if (Q<10) LcdWriteStr(".00");
	else if(Q<100) LcdWriteStr(".0");
	else Putc('.');
	LongToAscii(Q);
}
// Function Definitions
void LongToAscii(unsigned long Value)
{
 //Handles Upto 10 Digits
 bit isDivisible=0;
 unsigned long Divisor = 1000000000;
 unsigned char k;
 unsigned char Q;
 if(Value==0)	
 {
 	Putc('0');
 	return;
 }
 for(k=0;k<10;k++)
 {
 	if (Divisor<=Value) isDivisible=1;
	if(isDivisible)
 	{
  	   Q = (unsigned char )(Value/Divisor);
  		LcdWriteChar(Q+48); 
 		Value = Value - (Q)*Divisor;
	}//End if (isDivisible)
	Divisor = Divisor/10;
 }//End for
}
void DelayUs(void)
{
	TL0 = 0x90;TH0 = 0xFE;//200 usec at 22.1184MHz
	TR0  = 1;
	while(!TF0);
	TR0  = 0;
	TF0  = 0;
}

// 8bit LCD Interface
void LcdInit()
{
   LcdWriteCmd(0x38);	//Function Set 0x38
   LcdWriteCmd(0x38);	//Function Set
   LcdWriteCmd(0x38);	//Function Set
	LcdWriteCmd(0x06);	//Entry Mode Set 0x06
   LcdWriteCmd(0x0C);	//Display On  Off Control 0x0C
}

void LcdBusy()
{
	BS   = 1;			//Make D7th bit of LCD as i/p
   EN   = 1;         //Make port pin as o/p
   RS   = 0;         //Selected command register
   RW   = 1;         //We are reading
   while(BS)
   {   					//read busy flag again and again till it becomes 0 Enable H->L
   	EN   = 0;
      EN   = 1;
   }
}
void LcdWriteCmd(unsigned char var)
{
	P0 = var;      	//Commands to be Written
   RS   = 0;        	//Selected command register
   RW   = 0;        	//We are writing in instruction register
   EN   = 1;        	//Enable H->L
   EN   = 0;
   LcdBusy();      //Wait for LCD to process the command
}
void LcdWriteChar(unsigned char var)
{
	P0 = var;      	//Data/Character to be Written
   RS   = 1;         //Selected data register
   RW   = 0;         //We are writing
   EN   = 1;         //Enable H->L
   EN   = 0;
   LcdBusy();      //Wait for LCD to process the command
}
void LcdWriteStr(char *var)
{
	while(*var)       //till string ends send characters one by one
   	LcdWriteChar(*var++);
}

void LcdGotoXY(unsigned char row, unsigned char col)
{
	switch (row)
	{
		case 1: LcdWriteCmd(0x80 + col - 1); break;
		case 2: LcdWriteCmd(0xc0 + col - 1); break;
		case 3: LcdWriteCmd(0x90 + col - 1); break;	//0x90 for 4x20
		case 4: LcdWriteCmd(0xd0 + col - 1); break;	//0xd0 for 4x20
		default: break;
	}
}

void LcdClear()
{
 unsigned char h;
 LcdWriteCmd(0x01);
 for(h=0;h<100;h++)
		DelayUs();
}

