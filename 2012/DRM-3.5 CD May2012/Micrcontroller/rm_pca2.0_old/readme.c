/*
Project:		Digital Rate Meter
Client:		Kundian Chemical Industries KCI
Programmer:	Sajjad Hussain, S.E.
Developer:	Sajjad Hussain, S.E.

Specifications:
	
	Pulse
									___ 					 ___ 					  ___ 				   ___
								  |	|				   | 	 |				    |	  |				  | 	|
				______________|	|______________|	 |______________|	  |______________|	|_______________
				
				Ton	= > 2usec 	Actual Pulse
				Ton	  < 2usec 	Noise Pulse	(To be Filtered)
				
				Ton	Measured by	Timer1 Mode 2 and External Interrupt 1 pin (Gated Interrupt)
				Toff	Measured by PCA Timer and a Variable to Count no of Overflows
	
	Freuency Range
			
			
			0.001 Hz		to			1000 Hz
	
	
	Threshold Settings
	
			Counts (@ Xtal =	1MHz)
			<---------------------|------------------------------------|----------------------------->
			10^9						C1												 C2									10^3


			Counts (@ Xtal =	4MHz)
			<---------------------|------------------------------------|----------------------------->
			4x10^9					C1												 C2									4x10^3


			Frequency (Hz)
			<---------------------|------------------------------------|----------------------------->
			0.001						f1												 f2									1000
			
	
	Different Ranges
	
				Frequncy				Counts				Condition		LED		Bulb(BAP)		R1R2R3
			=====================================================================================
			1.	f < f1				C >= C1				Normal			-			Green				1 1 1
			2. f1 <= f < f2		C2 <= f < C1		Warning			AL1		Yellow			1 1 0
			3.	f >= f2				C < C2				Threat			AL2		Red				0 1 0
			4.	f < 0.001			C > 4x10^9			No Pulse			MF			Yellow			? ? ?
			5.																					Yellow			0 1 1		AL1(Anamoly)
			6.																					Yellow			x 0 x		AL2(Anamoly)

	Truth Table for Relays

				Condition		LED		Bulb(BAP)		R1R2R3
			=========================================================
			1.	Normal			-			Green				1 1 1
			2. AL1(MF)			AL1,MF	Yellow			0 1 1
			3.	AL1				AL1		Yellow			1 1 0
			4.	AL2				AL2		Red				0 1 0
			5.	AL2(MF)			AL2,MF	Yellow			x 0 x					0 0 0
			6. Start				MF			Yellow			x 0 x	(Supposed)	0 0 0
			

	Relay Position Noted During Visit to KCI	(07-02-2010)

	========================			========================
			KCI Rate Meter							ICCC Rate Meter
	========================			========================	
	D2 D1 D0									R1 R2 R3
	BF	S2	S1		BAP	Condition		S2	BF	S1		BAP	Condition
	========================			========================
	1	1	1		G		Normal			1	1	1		G		Normal
	1	0	1		Y		AL1(MF)			0	1	1		Y		AL1(MF)
	1	1	0		Y		AL1				1	1	0		Y		AL1
	1	0	0		R		AL2				0	1	0		R		AL2
	0	x	x		Y		AL2(MF)			x	0	x		Y		AL2(MF)

	Implementation of logarithmic Scale b/w frequency and DAC
	==========================================================
	
	|Iout
	|
	|           .
	|         .
	|       .
	|     .
	|   .
	|._ _ _ _ _ _ _ _ _ _ log(F)
	
	DAC Total Values:	256
	
	Count32 Range in log:	log(4e9) - log(4e3)	= 6
 	therefore step size for Count32 is S = 6/256
 	DAC Value = log(Count32)/S - log(4000)/S
 	is equvalently implented as
 	DAC Value = 10*[log(Count32)/S - log(4000)]/S
 	DAC Value = DAC_Value / 10;
 	
 	Vout = (Vmax-Vmin)*DAC Value / 256 + Vmin*255/256	in Volts
 	Iout = 3*Vout + 1;	in mA

			Counts (@ Xtal =	4MHz)
			<---------------------|------------------------------------|----------------------------->
			4x10^9					C1												 C2									4x10^3


			Frequency (Hz)
			<---------------------|------------------------------------|----------------------------->
			0.001						f1												 f2									1000
 	
			DAC Value (8 bit)
			<---------------------|------------------------------------|----------------------------->
			255						C1												 C2									0

			Vout (v)
			<---------------------|------------------------------------|----------------------------->
			1v							C1												 C2									5v


			Iout (mA)
			<---------------------|------------------------------------|----------------------------->
			4mA						f1												 f2									20mA


		Edges
		
		Edge 1		Pulse +	
		Edge 2		Pulse -
		Edge 3-20	Relays
		Edge 21		Recorder +
		Edge 22		Recorder -
		Edge 23-25	Unconnected
		Edge 26		+5v
		Edge 28		CC Led Anode
		Edge 29		Pressure Signal/ Air Valve Signal
		Edge 30-33	Ac input
		Edge 34		Unconnected
		
		26,27	Short			Are physically short circuited
		26,29 Short			MF is OFF
		26,29 Open			MF is ON
		28		+5V			CC is ON
		28		0V				CC is OFF
	*/ 

























































































