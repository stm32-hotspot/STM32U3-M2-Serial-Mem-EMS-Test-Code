/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

// header file
#ifndef MB2095_NET_h
#define MB2095_NET_h

typedef enum { // The component names 
NoNetComponent = 0, // no compoent, end of component cluster
B1, /* 2425755-1 */ 
CN1, /* USB_TYPE_C_Receptacle */ 
CN2, /* Header 7x2 */ 
CN3, /* Header 16x1 */ 
CN4, /* Header 16x1 */ 
CN5, /* Header 4x1 */ 
CN6, /* BTB010_061R */ 
CN7, /* Header 4x1 */ 
CN8, /* Header 16x1 */ 
CN9, /* BTB010_061R */ 
J1, /* MICROMOD */ 
J2, /* MICROMOD */ 
JP1, /* Jumper_Header 2x1 */ 
LD1, /* LED_green */ 
LD2, /* LED_red */ 
LD3, /* LED_orange */ 
LD4, /* LED_green */ 
U1 /* MCU */, /* STM32U385VGI6Q */ 
U2, /* LD39100 */ 
U3, /* USBLC6-2SC6 */ 
U4, /* ESD-TRANSIL */ 
U5, /* ESDALC6V1W5 */ 
U6, /* ESDALC6V1W5 */ 
U7, /* VL53L4CX */ 
U8, /* LIS2DU12 */ 
U9, /* STHS34PF80 */ 
U10, /* TSZ122IST */ 
U11, /* M95P32-IXMNT/E */ 
U12, /* ESD-TRANSIL */ 
} component_reference_t;

#define J1_3		PA_12	// USB_P 
#define U3_IO1		PA_12	// USB_P 
#define J1_5		PA_11	// USB_N 
#define U3_IO2		PA_11	// USB_N 
#define J1_59		PE_15	// PE15 
#define J1_61		PE_14	// PE14 
#define J1_57		PE_13	// PE13 
#define J2_34		PE_12	// PE12 
#define U11_D		PE_12	// PE12 
#define J2_56		PE_11	// PE11 
#define U11_Sn		PE_11	// PE11 
#define J2_32		PE_10	// PE10 
#define U11_CLK		PE_10	// PE10 
#define J1_46		PE_8	// PE8 
#define J1_54		PE_7	// PE7 
#define J1_56		PE_6	// PE6 
#define J2_54		PE_3	// PE3 
#define J1_58		PE_2	// PE2 
#define J1_42		PE_0	// PE0 
#define J1_47		PD_15	// PD15 
#define J1_32		PD_14	// PD14 
#define CN7_3		PD_13	// PD13 
#define J2_16		PD_13	// PD13 
#define U7_SDA		PD_13	// PD13 
#define U8_SDA		PD_13	// PD13 
#define U9_SDA		PD_13	// PD13 
#define CN7_2		PD_12	// PD12 
#define J2_18		PD_12	// PD12 
#define U7_SCL		PD_12	// PD12 
#define U8_SCL		PD_12	// PD12 
#define U9_SCL		PD_12	// PD12 
#define CN3_11		PD_11	// PD11 
#define J2_52		PD_7	// PD7 
#define J1_10		PD_5	// PD5 
#define J2_44		PD_4	// PD4 
#define J1_62		PD_2	// PD2 
#define J1_43		PD_1	// PD1 
#define J1_41		PD_0	// PD0 
#define J1_60		PC_12	// PC12 
#define J1_70		PC_11	// PC11 
#define J1_68		PC_10	// PC10 
#define J1_66		PC_9	// PC9 
#define J1_64		PC_8	// PC8 
#define J1_44		PC_7	// PC7 
#define J2_42		PC_6	// PC6 
#define U11_Hn		PC_6	// PC6 
#define J2_48		PC_3	// PC3 
#define J2_46		PC_2	// PC2 
#define CN3_10		PC_1	// PC1 
#define CN4_6		PC_1	// PC1 
#define J1_34		PC_1	// PC1 
#define CN3_6		PC_0	// PC0 
#define J1_38		PC_0	// PC0 
#define CN3_12		PB_15	// PB15 
#define LD1_		PB_15	// PB15 
#define CN3_13		PB_14	// PB14 
#define CN4_8		PB_14	// PB14 
#define LD2_		PB_14	// PB14 
#define CN4_9		PB_13	// PB13 
#define J1_15		PB_13	// PB13 
#define J1_19		PB_11	// PB11 
#define CN4_5		PB_10	// PB10 
#define J1_17		PB_10	// PB10 
#define CN4_3		PB_9	// PB9 
#define J1_51		PB_9	// PB9 
#define CN4_4		PB_8	// PB8 
#define J1_48		PB_8	// PB8 
#define CN3_15		PB_7	// PB7 
#define CN4_7		PB_6	// PB6 
#define J1_53		PB_6	// PB6 
#define J2_58		PB_5	// PB5 
#define CN4_14		PB_4	// PB4 
#define CN4_13		PB_3	// PB3 
#define J1_8		PB_3	// PB3 
#define J1_14		PB_2	// PB2 
#define CN4_12		PB_1	// PB1 
#define J1_13		PB_1	// PB1 
#define J2_36		PB_0	// PB0 
#define U11_Q		PB_0	// PB0 
#define J1_55		PA_15	// PA15 
#define J1_21		PA_14	// PA14 
#define J1_23		PA_13	// PA13 
#define CN4_11		PA_10	// PA10 
#define CN5_3		PA_10	// PA10 
#define J1_20		PA_10	// PA10 
#define CN4_10		PA_9	// PA9 
#define CN5_4		PA_9	// PA9 
#define J1_22		PA_9	// PA9 
#define CN3_14		PA_8	// PA8 
#define LD3_		PA_8	// PA8 
#define J2_38		PA_7	// PA7 
#define U11_Wn		PA_7	// PA7 
#define J1_12		PA_6	// PA6 
#define CN3_8		PA_5	// PA5 
#define J2_6		PA_5	// PA5 
#define CN3_4		PA_4	// PA4 
#define J2_4		PA_4	// PA4 
#define J2_8		PA_3	// PA3 
#define J1_40		PA_2	// PA2 
#define J2_12		PA_1	// PA1 
#define J2_10		PA_0	// PA0 
#define J2_3		PH_0	// OSC_IN 
#define J1_11		PH_3	// BOOT0 
#define TP2_1		PH_3	// BOOT0 

#endif