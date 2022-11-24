/*				Digital Rate Meter Ranging
            Programmer: Sajjad Hussain S.E.
            Crystal Frequency	=	24 MHz
            Internal Clock		=	4  MHz
            Micocontroller		= 	AT89C51RC2
            Dated: 	30-03-2010
            Modification:	09-08-2012
            24-05-2012	Calibration
            28-05-2012	Averaging, Frequnecy Comparision instead of Counter
*/
#include "at89c51rc2.h"
#include "ratemeter.h"
#include <intrins.h>
#include <math.h>
// Main Program Starts Here
void main()
{
	unsigned int dly;
	RELAY1_S2 = 1;//0
	RELAY2_MF = 0;
	RELAY3_S1 = 1;//0
	

	//CSDAC = 1;
	for (dly=0;dly<30000;dly++)
		_nop_();

	CKCON0 = 0x01;				// 6 Clock Intstruction Cycle
	AUXR 	= AUXR | 0xFD;		// xram
	// Default Threshold (Counts)
	Thresh[0] = 10.0;//400000;		// equals to 10 Hz
	Thresh[1] = 100.0;//40000;			// equals to 100 Hz
	// Initilize
	T2MOD = 0x00;
	EA = 0;
	TMOD = 0x11;
	P2 = 0x00;
   // Initial Screen
	for (dly=0;dly<50;dly++)
		Delay10ms();
	LcdInit();
	LcdGotoXY(1,1);
	LcdWriteStr("IMP/S: 000.000");
	LcdGotoXY(2,3);
	LcdWriteStr("Th1: 010.000");
	LcdGotoXY(3,3);
	LcdWriteStr("Th2: 100.000");
	LcdGotoXY(4,1);
	LcdWriteStr("No Pulse, MF");
	LcdGotoXY(4,15);
	LcdWriteStr("01");
	
	for (dly=0;dly<50;dly++)
		Delay10ms();
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

	LED_MF  =  LED_ON;
	LED_AL1 =  LED_OFF;
	LED_AL2 =  LED_OFF;
	LED_CC  =  LED_OFF;
	RELAY1_S2 = 1;//0
	RELAY2_MF = 0;
	RELAY3_S1 = 1;//0
 	isInit = 1;
	//Running Forever
	while(isInit)
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
				command = 0;
				if(Edge29)// Pressure Signal Absent 
					command = command | 0x01;
				else // Pressure Signal is Pressent
				{	
					command = command & 0xFE;
				}
				
				LcdGotoXY(1,7); CountsToFreq(Count32);
				LcdGotoXY(4,1);
				state = command;
				break;
		 	case 0:
		 		LcdWriteStr("No Alarm     ");
		 		//Putc(command+48);
		 		LED_MF =  LED_OFF;
				LED_AL1 =  LED_OFF;
				LED_AL2 =  LED_OFF;
				RELAY1_S2 = 1;	RELAY2_MF = 1;	RELAY3_S1 = 1;
				state = 12;
		 		break;
		 	case 1:
		 		LcdWriteStr("No Alarm, MF ");
		 		//Putc(command+48);
				LED_AL1 =  LED_OFF;
				LED_AL2 =  LED_OFF;
				LED_MF  =  LED_ON;
				RELAY1_S2 = 1;	RELAY2_MF = 0;	RELAY3_S1 = 1;
				//state = 8;
				state = 12;
		 		break;
		 	case 2:
		 		LcdWriteStr("Alarm-1     ");
		 		//Putc(command+48);
		 		LED_MF =  LED_OFF;
				LED_AL1 =  LED_ON;
				LED_AL2 =  LED_OFF;
				RELAY1_S2 = 1;	RELAY2_MF = 1;	RELAY3_S1 = 0;
				state = 12;
		 		break;
		 	case 3:
		 		LcdWriteStr("Alarm-1, MF ");
		 		//Putc(command+48);
				LED_MF  =  LED_ON;
				LED_AL1 =  LED_ON;
				LED_AL2 =  LED_OFF;
				RELAY1_S2 = 1;	RELAY2_MF = 0;	RELAY3_S1 = 0;//011
				state = 12;
		 		break;
		 	case 4:
		 	case 6:
		 		LcdWriteStr("Alarm-2     ");
		 		//Putc(command+48);
				LED_MF  =  LED_OFF;
				LED_AL1 =  LED_ON;
				LED_AL2 =  LED_ON;
				RELAY1_S2 = 0;	RELAY2_MF = 1;	RELAY3_S1 = 0;
				state = 12;
		 		break;
		 	case 5:
		 	case 7:
		 		LcdWriteStr("Alarm-2, MF ");
		 		//Putc(command+48);
				LED_MF  =  LED_ON;
				LED_AL1 =  LED_ON;
				LED_AL2 =  LED_ON;
				RELAY1_S2 = 0;	RELAY2_MF = 0;	RELAY3_S1 = 0;
				state = 12;
		 		break;
		 	case 16:
		 	case 17:
		 		LcdWriteStr("No Pulse, MF");
		 		//Putc(command+48);
				LED_AL1 =  LED_OFF;
				LED_AL2 =  LED_OFF;
				LED_MF  =  LED_ON;
				RELAY1_S2 = 1;	RELAY2_MF = 0;	RELAY3_S1 = 1;
				state = 8;
		 		break;
		}//switch(state)
		if(Edge28==0)// +5 Present at Edge 28
			LED_CC = 1;
		else
			LED_CC = 0;
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
				LcdGotoXY(selThresh+2,7);
				LcdWriteStr("       ");
				LcdGotoXY(selThresh+2,7);
				D = 0; r = 0; Point = 0; Num = 1; 
				break;
			case 'L':
				TR2 = 0;
				selThresh = 0;
				LcdGotoXY(2,7);
				LcdWriteStr("         ");
				LcdGotoXY(2,7);
				LcdWriteCmd(0x0D);
				break;
			case 'U':
				TR2 = 0;
				selThresh = 1;
				LcdGotoXY(3,7);
				LcdWriteStr("         ");
				LcdGotoXY(3,7);
				LcdWriteCmd(0x0D);
				break;
			case 'Y':
				LcdWriteCmd(0x0C);
				switch(selThresh)
				{
					case 0:
						LcdGotoXY(2,7);

						//if ((4000000*Num/D) >= 4000000000)	// Limit Exceed
						if ((D/Num) <= 0.001)
						{
							Thresh[0] = 0.001;//4000000000;
						}
						//else if ((4000000*Num/D) < Thresh[1] ) //Not Logical
						else if ((D/Num)> Thresh[1])
						{
							Thresh[0] = Thresh[0];
						}
						else
							Thresh[0] = D/Num;//4000000*Num/D;

						DispFrequency(Thresh[0],0);//CountsToFreq(Thresh[0],0);
						break;
					case 1:
						LcdGotoXY(3,7);
						//if ((4000000*Num/D) > Thresh[0]) //Not Logical
						if (( D/Num) < Thresh[0])
						{
							Thresh[1] = Thresh[1];
						}
						//else if ((4000000*Num/D) <= 4000)	// Limit Exceed
						else if ((D/Num) >= 1000)
						{
							Thresh[1] = 1000;//4000;
						}
						else
							Thresh[1] = D/Num;//4000000*Num/D;

						DispFrequency(Thresh[1],0);//CountsToFreq(Thresh[1],0);
						break;
				}
				D = 0; r = 0; Point = 0; Num = 1; selThresh =2;
				TR2 =1;
				break;
			case 'M':
				break;
			default:
				if(Point<2 && r<7 && selThresh<2)
				{
					if(Point==1)
						Num = Num*10;

					if(Key==0xFE)
					{
						Point++;
						/*if(Point>1)
						{
							LcdGotoXY(4,1);
							LcdWriteStr("Error   ");
						}*/
					}
					else
						D = D*10 + Key;
					LcdGotoXY(selThresh+2,7+r);
					r++;
					Putc(Key+48);
				}//Point
				break;
		}
	}
}
































