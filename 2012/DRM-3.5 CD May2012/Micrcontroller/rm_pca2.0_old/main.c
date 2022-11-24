/*				Digital Rate Meter Ranging
            Programmer: Sajjad Hussain S.E.
            Crystal Frequency	=	24 MHz
            Internal Clock		=	4  MHz
            Micocontroller		= 	AT89C51RC2
            Dated: 	30-03-2010
*/
#include "at89c51rc2.h"
#include "ratemeter.h"
#include <intrins.h>
#include <math.h>
// Main Program Starts Here
void main()
{
	unsigned int dly;
	CKCON0 = 0x01;				// 6 Clock Intstruction Cycle
	CSDAC = 1;
	for (dly=0;dly<30000;dly++)
		_nop_();
	// Default Threshold (Counts)
	Thresh[0] = 400000;		// equals to 10 Hz
	Thresh[1] = 40000;			// equals to 100 Hz
	// Timer 1 in  mode 2 with hardware gate
	TMOD =0xA1;
	TH1 = 0x00;	
	TL1 = 0x00;	
	TR1=1;		
		
   // Set Timer2 used for 12.5 msec @ 4MHz
   // Used to Update display every One Second	(12.5 msec * 80 = 1000 msec)
   T2MOD = 0x01;
	T2CON = 0x00;
	TH2 = 0x3C;TL2 = 0xB0;
	RCAP2H = 0x3C;RCAP2L = 0xB0;
	TR2 = 1;
	ET2 = 1;
	EA = 1;
	// Set PCA Module and PCA Interrupt
	CCON = 0x00;
	CMOD = 0x01;
	CH = 0;
	CL = 0;
	CCAPM0 = 0x11;// Capture -ve Edge
	CR = 1;
	EC = 1;
	//	Keypad Interrupt
	EX0 = 1;
	IT0 = 1;
	// Default led's and relays position
	P2 = 0x00;
	LED1_MF  =  LED_ON;
	LED2_AL1 =  LED_OFF;
	LED3_AL2 =  LED_OFF;
	LED4_CC  =  LED_OFF;
	RELAY1_S2 = 0;
	RELAY2_MF = 0;
	RELAY3_S1 = 0;
   // Initial Screen
	LcdInit();
	LcdGotoXY(1,1);
	LcdWriteStr("IMP/S:000.000");
	LcdGotoXY(2,3);
	LcdWriteStr("Th1: 10");
	LcdGotoXY(3,3);
	LcdWriteStr("Th2: 100");
	LcdGotoXY(4,1);
	LcdWriteStr("Anomaly ");
	isInit = 1;
	//Running Forever
	while(1)
	{	
		switch(state)
		{
		 	case 8:
		 		next = 9;
		 		break;
		 	case 9:
		 		next = 10;
		 		break;
		 	case 10:
		 		state = 11;
		 		next = 11;
		 		break;
		 	case 11:
				if(PulseWidth>5)	// > 2usec
				{
					TF16_Old = TF16; 
					TF16 = 0;
					CL = 0; CH = 0;
					Capture16 = (capHigh<<8) + capLow;
					Count32 = TF16_Old;
					Count32 = (Count32<<16) + Capture16;
				}
				state = 12;
		 		break;
		 	case 12:
		 		if(CntMSec==80) //One Second
		 		{
					CntMSec = 0;
					state = 13;
				}
				break;
			case 13:
				// Calculate Running Counter
				rCapture16 = (CH<<8) + CL;
				rCount32 = TF16;
				rCount32 = (rCount32<<16) + rCapture16;
				
				LcdGotoXY(1,7);LcdWriteStr("        ");
				if(Count32 < rCount32)//	Handling frequncy less than 1Hz
					Count32 = rCount32;
				// Check the Edge 29 for MF
				if(Edge29)// Pressure Signal Absent 
					command = command | 0x01;
				else // Pressure Signal is Pressent
				{	
					command = command & 0xFE;
				}
				// Check the Range of Frequency, in which You are
				if(Count32 < 4000)
				{
					Count32 = 4000;
					command = command | 0x06;	//AL1 = 1 AL2 = 1
				}
				else if (Count32 <= Thresh[1])
				{
					command = command | 0x04;	//AL2 = 1
				}
				else if (Count32 <= Thresh[0])
				{
				 	command = command | 0x02;	//AL1 = 1
					command = command & 0xFB;	//AL2 = 0
				}
				else if (Count32 < 4000000000)
				{
				 	command = command & 0xFD;	//AL1 = 0
					command = command & 0xFB;	//AL2 = 0
				}
				else
				{
					command = command | 0x01;
					Count32 = 4000000000;
				}
				// Display the Frequency and Output to DAC
				factor = log10(Count32)*2560/6;
				factor = (factor - 1536)/10;
				if (factor>255) factor = 255;
				WriteDAC((unsigned char)(factor));
				LcdGotoXY(1,7);CountsToFreq(Count32);
				/*LcdGotoXY(2,1);LongToAscii(Count32);*/
				LcdGotoXY(4,1);
				state = command;
				break;
		 	case 0:
		 		LcdWriteStr("Normal  ");  //A   Green Led
		 		//Putc(command+48);
		 		LED1_MF =  LED_OFF;
				LED2_AL1 =  LED_OFF;
				LED3_AL2 =  LED_OFF;
				RELAY1_S2 = 1;	RELAY2_MF = 1;	RELAY3_S1 = 1;
				state = 12;
		 		break;
		 	case 1:
		 		LcdWriteStr("Anomaly ");
		 		//Putc(command+48);
				LED2_AL1 =  LED_OFF;
				LED3_AL2 =  LED_OFF;
				LED1_MF  =  LED_ON;
				RELAY1_S2 = 0;	RELAY2_MF = 0;	RELAY3_S1 = 0;
				state = 8;
		 		break;
		 	case 2:
		 		LcdWriteStr("Alarm1  ");
		 		//Putc(command+48);
		 		LED1_MF =  LED_OFF;
				LED2_AL1 =  LED_ON;
				LED3_AL2 =  LED_OFF;
				RELAY1_S2 = 1;	RELAY2_MF = 1;	RELAY3_S1 = 0;
				state = 12;
		 		break;
		 	case 3:
		 		LcdWriteStr("Anomaly1");
		 		//Putc(command+48);
				LED1_MF  =  LED_ON;
				LED2_AL1 =  LED_ON;
				LED3_AL2 =  LED_OFF;
				RELAY1_S2 = 0;	RELAY2_MF = 1;	RELAY3_S1 = 1;
				state = 12;
		 		break;
		 	case 4:
		 	case 6:
		 		LcdWriteStr("Alarm2  ");
		 		//Putc(command+48);
				LED1_MF  =  LED_OFF;
				LED2_AL1 =  LED_ON;
				LED3_AL2 =  LED_ON;
				RELAY1_S2 = 0;	RELAY2_MF = 1;	RELAY3_S1 = 0;
				state = 12;
		 		break;
		 	case 5:
		 	case 7:
		 		LcdWriteStr("Anomaly2");
		 		//Putc(command+48);
				LED1_MF  =  LED_ON;
				LED2_AL1 =  LED_ON;
				LED3_AL2 =  LED_ON;
				RELAY1_S2 = 0;	RELAY2_MF = 0;	RELAY3_S1 = 0;
				state = 12;
		 		break;
		}//switch(state)
	}//while
}//main
// Interrupt Routines
void PCATimers() interrupt 6  using 1
{
	if(CF)
	{
		CF = 0;
		TF16++;
	}//CF
	else	if(CCF0 & isInit)
	{
		CCF0 = 0;
		capLow  = CCAP0L; 
		capHigh = CCAP0H;
		PulseWidth = TL1;	// Save the Pulse Width Value
		TL1 = 0;				// Reset for Next Pulse Width
		state = next;
	}
}
void Timer2()	interrupt 5
{
	TF2 = 0;
	CntMSec++;
}

void Keypad_One() interrupt 0
{
	P0=0xFF;
	EN = 0;CS=0;
	Key = P0 & 0x0F;
	CS=1;EN = 1;
	if(isInit)
	{
		Key = Mapping[Key];
		switch(Key)
		{
			case 'C':
				LcdGotoXY(selThresh+2,8);
				LcdWriteStr("       ");
				LcdGotoXY(selThresh+2,8);
				D = 0; r = 0; Point = 0; Num = 1; 
				break;
			case 'L':
				TR2 = 0;
				selThresh = 0;
				LcdGotoXY(2,8);
				LcdWriteStr("       ");
				LcdGotoXY(2,8);
				LcdWriteCmd(0x0D);
				break;
			case 'U':
				TR2 = 0;
				selThresh = 1;
				LcdGotoXY(3,8);
				LcdWriteStr("       ");
				LcdGotoXY(3,8);
				LcdWriteCmd(0x0D);
				break;
			case 'Y':
				LcdWriteCmd(0x0C);
				Thresh[selThresh] = 4000000*Num/D;
				D = 0; r = 0; Point = 0; Num = 1; selThresh =2;
				TR2 =1;
				break;
			case '#':
				break;
			default:
				if(Point<2 && r<7 && selThresh<2)
				{
					if(Point==1)
						Num = Num*10;

					if(Key==0xFE)
					{
						Point++;
						if(Point>1)
						{
							LcdGotoXY(4,1);
							LcdWriteStr("Error   ");
						}
					}
					else
						D = D*10 + Key;
					LcdGotoXY(selThresh+2,8+r);
					r++;
					Putc(Key+48);
				}//Point
				break;
		}
	}
}

















