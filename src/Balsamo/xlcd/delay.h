/* Change the following definition to match the instruction frequency 
of the application in MIPS (MHz) */
#ifndef Fcy
#define Fcy  16000000
#endif

#define Delay200uS_count  (Fcy * 0.0002) / 1080
#define Delay_1mS_Cnt     (Fcy * 0.001) / 2950
#define Delay_2mS_Cnt     (Fcy * 0.002) / 2950
#define Delay_5mS_Cnt     (Fcy * 0.005) / 2950
#define Delay_15mS_Cnt    (Fcy * 0.015) / 2950
#define Delay_1S_Cnt      (Fcy * 1) / 2950
