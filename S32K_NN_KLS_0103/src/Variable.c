/*
 * Variable.c
 *
 *  Created on: 2017��8��21��
 *      Author: ted
 */

#include "Variable.h"
/**********************************************************************************
                                		�궨����
**********************************************************************************/

CAL_SECTION const uint8_t Default_Cal_data[CAL_DATA_SIZE]=
{
	  /******************�궨��0~127����������������*********************/
	  /*************����������0~86****************/
	  //�������ͺ�0~19
		'K','L','S','7',			          //0,1,2,Module Name,8Bytes: ASCII
		'2','1','8','N', 		            //3,4,5,6,7,Module Name,8Bytes: ASCII
		'b','z','b','b',			          //8,9,10,11,�ͻ�����
	  0x17,0x26,0x03,0x97,		        //12,13,14,15,Serial Number,4Bytes BCD
	  0x02,0x06,0x00,0x01,            //16,17,18,19,Software Version,2Bytes BCD,Level & Number
	  //��������������20~86
	  0x01,			            	        //20	Function ����������,0x01������̤���ֹ(��־Ϊ1ʱ��Ч,��KBL�෴),0x02�˳�ɲ����̤���ֹ,0x04�յ���̤���ֹ(��־Ϊ1ʱ��Ч),0x08��������ʹ��,0x10����������0x20��������ʹ�ܣ�0x40����ʹ�ܣ�0x80Boostʹ��
	  0x06,			            	        //21	Function ����������,0x01��ȫ����,0x02�����߼��ߵ�ƽ,0x04�궨ģʽ,0x08Ӳ������KIM,0x10��������������,0x20�����������,0x40����ʹ��,0x80�ı�����ת����
	  0x0c,														//22  Function ����������
		0,				                      //23	Startup Wait Time,valid value from 1 to 20,
	  0,72,				                    //24,25	Controller Voltage,valid value from 24 to 144,
	  0,2,				                    //26,27	Low voltage,valid value from 18 to 100
	  0,80,			             	        //28,29	Over voltage,valid value from 18 to 100
		0,0,														//30,31,Ԥ����ѹ
		0,0,0,0,												//32,33,34,35	(R)

	  0x02,0x58,                      //36,37 HS_Rated_I���������ƶ����,50~600
	  0x01,0x99,             	        //38,39	���������ƶ������Ӧ��ADֵ,��ֹ�޸�,����ADCģ�����λ�������仯,409(10λ)~2048(12λ)
	  0x02,0x00,				              //40,41	Phase A1 Zero current,474(10λ)~2200(12λ)
	  0x02,0x00,			                //42,43	Phase B1 Zero current,474(10λ)~2200(12λ)
		0x02,0x00,			                //44,45	Phase C1 Zero current,474(10λ)~2200(12λ)
	  0x02,0x00,											//46,47 Phase A2 Zero current,474(10λ)~2200(12λ)
	  0x02,0x00,											//48,49 Phase B2 Zero current,474(10λ)~2200(12λ)
	  0x02,0x00,											//50,51 Phase C2 Zero current,474(10λ)~2200(12λ)
	  100,				                    //52	Current percent,valid value from 20 to 100
	  100,				                    //53	˫������������������ٷֱ�,20~100
	  100,                            //54  ��ѹ�������(ĸ�ߵ����ٷֱ�)�ٷֱ�20~100
	  0,															//55,	(R)

	  0x55,				                    //56	Identify Motor Parameters Enable Flag,0x55 quit,0xAA entry
	  110,                            //57	PCB High Temperature Shutdown Controller Temperature 1 Byte
	  90,                             //58	PCB Shutdown Controller Resume Temperature 	1 Byte
	  100,                            //59	PCB High Temperature weaken Temperature	1 Byte
	  50,                             //60	PCB Used For High_Temp_Divisor[(110-100)/50],High Temperature weaken percent	1 Byte
	  50,                             //61	PCB Output weaken Temperature 	1 Byte
	  35,                             //62	PCB Used For Output_Weak_Divisor[(100-80)/35],Output weaken  percent	1 Byte
	  0,                              //63	PCB High Measure Temperature revise Temperature	1 Byte
	  30,                             //64	PCB Used For High_Measure_divisor[(50-0)/30)],High Measure Temperature revise percent 1 Byte
	  -40,                            //65	PCB =216,Low Measure Temperature revise Temperature	1 Byte
	  70,                             //66	PCB Used For Low_Measure_divisor[(0-(-40))/40],Low Measure Temperature revise percent 1 Byte
	  0,                              //67	PCB Temperature Trim,	1 Byte,	signed byte
	  80,				                      //68	PCB High Measure Weaken Start Temperature   1Byte
	  50,		                          //69	HL Temperature Trim
	  110, 				                    //70	HL High Temperature Shutdown Controller Temperature 1 Byte
	  80,				                      //71	HL High Measure Weaken Start Temperature   1Byte
	  85,				                      //72	HL High Temperature weaken percent
	  90,				                    	//73	HL Shutdown Controller Resume Temperature 	1 Byte
	  -20,														//74 CAL_LV_REVISE_PCB_L_TEMP,��ѹ��������PCB�¶�
	  20,              								//75,CAL_LV_REVISE_PCB_H_TEMP,��ѹ��������PCB�¶�
	  80,															//76,CAL_LV_REVISE_PERCENT,��ѹ��������PCB�¶ȶ�Ӧ�ĵ�ѹֵ�ٷֱ�
	  0,															//77 CAL_HL_REVISE_PCB_L_TEMP,�ߵͶ��¶���������PCB�¶�
	  20,              								//78 CAL_HL_REVISE_PCB_H_TEMP,�ߵͶ��¶���������PCB�¶�
	  60,															//79 INFO_HL_REVISE_WEAK_L_TEMP,�ߵͶ��¶���������PCB�¶ȶ�Ӧ�ĸߵͶ�������ʼ���¶�ֵ
		0,                    					//80,(R)
	  0,															//81,(R)
	  0x01,0x17,											//82,83,CAL_WHEEL_DIAMETER ���ְ뾶mm
	  0x02,0x74,											//84,85,CAL_SPD_DOWN_SCALE	���ٱ�*100
	  0x0,                            //86	0xFF is BootLoader mode,other byte not BootLoader mode
		0,0,0,0,0,0,0,0,0,0,						//87~96	(R)
		0,0,0,0,0,0,0,0,0,0,						//97~106	(R)
		0,0,0,0,0,0,0,0,0,0,						//107~116	(R)
		0,0,0,0,0,0,0,0,0,0,						//117~126	(R)
	  10,  	                					//127, PWMƵ�ʣ�20Ϊ20K������Ϊ10K	(R)
	  /**********************�궨��128~255���Ʋ���*****************/
	  2,				                      //128	Control Mode,0:OpenLoop, 1:Speed Mode,2:Torque Mode
	  0,                              //129,	�Ͷ˱����ٷֱ�
	  95,                             //130,	�߶˱����ٷֱ�
	  2,				                      //131	TPS Type,0:NO,1:0-5V,2:1-4V,3:0-5K
	  10,				                      //132	TPS Low Deadzone,valid value from 0 to 40 percent
	  70,			              	        //133	TPS High Deadzone,valid value from 60 to 100 percent
	  30,				                      //134	TPS Map Forward
	  20,				                      //135	TPS Map Reverse
	  0,				                      //136	BRK Type,0:NO,1:0-5V,2:1-4V,3:0-5K
	  20,             	    	        //137	BRK Low Deadzone,valid value from 0 to 40 percent
	  80,				                      //138	BRK High Deadzone,valid value from 60 to 100 percent
	  0,				                      //139	(R)BRK Map 1
	  0x03,0xE8,              		    //140,141	Max Output Frequency,valid value from 0 to 300Hz
	  0x0F,0xA0,			                //142,143	Max Motor RPM,valid value from 0 to 65535 RPM
	  100,				                    //144	HForward Speed Limit,valid value from 30 to 100 Percent
	  50,			                        //145 HReverse Speed Limit,valid value from 30 to 100 Percent
	  100,				                    //146	MForward Speed Limit,valid value from 30 to 100 Percent
	  50,			                        //147 MReverse Speed Limit,valid value from 30 to 100 Percent
	  100,				                    //148	LForward Speed Limit,valid value from 30 to 100 Percent
	  50,			                        //149 LReverse Speed Limit,valid value from 30 to 100 Percent
	  20,                             //150 COMP_PERCENT����-������������ٷֱ�0~100
	  0,				                      //151	(R)
	  0x07,0xD0,                      //152,153 dQrefqKp_T�ٶȻ�����ϵ��Kp,0~32767
	  0x00,0x3C,                      //154,155 dQrefqKi_T�ٶȻ�����ϵ��Ki,0~32767
	  0x00,0x00,                      //156,157 dQrefqKd_T�ٶȻ�΢��ϵ��Kd,0~32767
	  0x03,0xE8,                      //158,159 dQrefqErrMax�ٶȻ�����ź�Err�޷�ֵ,50~4095
	  0,50,                           //160,161 IVTBRK_Min_RPM�������������ٶ���Сֵ,0~65535
	  0x00,0x00,                      //162,163,BRK_SPEED_LIMIT �ٶȴ��ڴ˱궨ֵ�Ż���뷢���ƶ�״̬����Χ50~500
	  0,															//164,BRK_AD_Brakingɲ��ģ�����ƶ������ٷֱȣ���Χ0~50
	  10,                             //165 BRK_SW_Brakingɲ�������ƶ������ٷֱ�,0~100
	  0,                              //166,RLS_TPS Braking Percent����̤���ƶ�ת��Ϊ����ƶ�ת�صİٷֱȣ���Χ0~50
	  0,                              //167,NTL Braking Percent���յ��ƶ�ת��Ϊ����ƶ�ת�صİٷֱȣ���Χ0~50
	  0,                              //168 IVTBRK Current Limit Percent�����������������,0~100
		0,0,0,0,0,0,			     					//169,170,171,172,173,174,	(R)
	  10,             	            	//175 T_Accel_Time���Ž���ʱ��, valid value from 0 to 200,��Ӧ0~20s
	  5,				                      //176	T_ACC_Release_Time�����ͷ�ʱ��, valid value from 0 to 200,��Ӧ0~20s
	  5,				                      //177	T_Brake_Timeɲ������ʱ��, valid value from 0 to 200,��Ӧ0~20s
	  1,			              	        //178	T_BRK_Release_Timeɲ���ͷ�ʱ��, valid value from 0 to 200,��Ӧ0~20s
	  0,				                      //179	(R)
		0x01,0xF4,                      //180,181 dQqKp������Q����ϵ��Kp,0~32767
		0x00,0x0A,                      //182,183 dQqKi������Q����ϵ��Ki,0~32767
		0x7F,0xFF,                      //184,185 dQqOutMax������Q����޷�ֵ,32767~56756
		0x05,0xDC,                      //186,187 dDqKp������D����ϵ��Kp,0~32767
		0x00,0x1E,                      //188,189 dDqKi������D����ϵ��Ki,0~32767
		0x7F,0xFF,                      //190,191 dDqOutMax������D����޷�ֵ,23170~32767
	  0x00,0x00,                      //192,193 dUqKp��ѹ������ϵ��Kp,0~32767
	  0x00,0x00,                      //194,195 dUqKi��ѹ������ϵ��Ki,0~32767
	  0x00,0x00,                      //196,197 dUqErrMax��ѹ������ź�Err�޷�ֵ,50~4095
	  0x01,0x90,                      //198,199,ģʽ�л���ת������ֵ
	  0x00,0xC8,											//200,201,ģʽ�л���ת������ֵ
	  0x07,0xD0,											//202,203 ��������ʱ������Q����ϵ��Kp
	  0x00,0x3C,                      //204,205 ��������ʱ������Q����ϵ��Ki
	  0x13,0x88,											//206,207 ��������ʱ������D����ϵ��Kp
	  0x00,0x64,											//208,209 ��������ʱ������D����ϵ��Ki
		0,0,0,0,0,0,0,0,0,0,						//210~219	(R)
		0,0,0,0,0,0,0,0,0,0,						//220~229	(R)
		0,0,0,0,0,0,0,0,0,0,						//230~239	(R)
		0,0,0,0,0,0,0,0,0,0,						//240~249	(R)
		0,0,0,0,0,100,										//250~255	(R)
	/**********************�궨��256~383���Ʋ���,ƫ��128*****************/
	  2,				                      //128	Control Mode,0:OpenLoop, 1:Speed Mode,2:Torque Mode
	  0,                              //129,	�Ͷ˱����ٷֱ�
	  95,                             //130,	�߶˱����ٷֱ�
	  2,				                      //131	TPS Type,0:NO,1:0-5V,2:1-4V,3:0-5K
	  10,				                      //132	TPS Low Deadzone,valid value from 0 to 40 percent
	  70,			              	        //133	TPS High Deadzone,valid value from 60 to 100 percent
	  30,				                      //134	TPS Map Forward
	  20,				                      //135	TPS Map Reverse
	  0,				                      //136	BRK Type,0:NO,1:0-5V,2:1-4V,3:0-5K
	  20,             	    	        //137	BRK Low Deadzone,valid value from 0 to 40 percent
	  80,				                      //138	BRK High Deadzone,valid value from 60 to 100 percent
	  0,				                      //139	(R)BRK Map 1
	  0x03,0xE8,              		    //140,141	Max Output Frequency,valid value from 0 to 300Hz
	  0x0F,0xA0,			                //142,143	Max Motor RPM,valid value from 0 to 65535 RPM
	  100,				                    //144	HForward Speed Limit,valid value from 30 to 100 Percent
	  50,			                        //145 HReverse Speed Limit,valid value from 30 to 100 Percent
	  100,				                    //146	MForward Speed Limit,valid value from 30 to 100 Percent
	  50,			                        //147 MReverse Speed Limit,valid value from 30 to 100 Percent
	  100,				                    //148	LForward Speed Limit,valid value from 30 to 100 Percent
	  50,			                        //149 LReverse Speed Limit,valid value from 30 to 100 Percent
	  20,                             //150 COMP_PERCENT����-������������ٷֱ�0~100
	  0,				                      //151	(R)
	  0x07,0xD0,                      //152,153 dQrefqKp_T�ٶȻ�����ϵ��Kp,0~32767
	  0x00,0x3C,                      //154,155 dQrefqKi_T�ٶȻ�����ϵ��Ki,0~32767
	  0x00,0x00,                      //156,157 dQrefqKd_T�ٶȻ�΢��ϵ��Kd,0~32767
	  0x03,0xE8,                      //158,159 dQrefqErrMax�ٶȻ�����ź�Err�޷�ֵ,50~4095
	  0,50,                           //160,161 IVTBRK_Min_RPM�������������ٶ���Сֵ,0~65535
	  0x00,0x00,                      //162,163,BRK_SPEED_LIMIT �ٶȴ��ڴ˱궨ֵ�Ż���뷢���ƶ�״̬����Χ50~500
	  0,															//164,BRK_AD_Brakingɲ��ģ�����ƶ������ٷֱȣ���Χ0~50
	  10,                             //165 BRK_SW_Brakingɲ�������ƶ������ٷֱ�,0~100
	  0,                              //166,RLS_TPS Braking Percent����̤���ƶ�ת��Ϊ����ƶ�ת�صİٷֱȣ���Χ0~50
	  0,                              //167,NTL Braking Percent���յ��ƶ�ת��Ϊ����ƶ�ת�صİٷֱȣ���Χ0~50
	  0,                              //168 IVTBRK Current Limit Percent�����������������,0~100
		0,0,0,0,0,0,			     					//169,170,171,172,173,174,	(R)
	  10,             	            	//175 T_Accel_Time���Ž���ʱ��, valid value from 0 to 200,��Ӧ0~20s
	  5,				                      //176	T_ACC_Release_Time�����ͷ�ʱ��, valid value from 0 to 200,��Ӧ0~20s
	  5,				                      //177	T_Brake_Timeɲ������ʱ��, valid value from 0 to 200,��Ӧ0~20s
	  1,			              	        //178	T_BRK_Release_Timeɲ���ͷ�ʱ��, valid value from 0 to 200,��Ӧ0~20s
	  0,				                      //179	(R)
		0x01,0xF4,                      //180,181 dQqKp������Q����ϵ��Kp,0~32767
		0x00,0x0A,                      //182,183 dQqKi������Q����ϵ��Ki,0~32767
		0x7F,0xFF,                      //184,185 dQqOutMax������Q����޷�ֵ,32767~56756
		0x05,0xDC,                      //186,187 dDqKp������D����ϵ��Kp,0~32767
		0x00,0x1E,                      //188,189 dDqKi������D����ϵ��Ki,0~32767
		0x7F,0xFF,                      //190,191 dDqOutMax������D����޷�ֵ,23170~32767
	  0x00,0x00,                      //192,193 dUqKp��ѹ������ϵ��Kp,0~32767
	  0x00,0x00,                      //194,195 dUqKi��ѹ������ϵ��Ki,0~32767
	  0x00,0x00,                      //196,197 dUqErrMax��ѹ������ź�Err�޷�ֵ,50~4095
	  0x01,0x90,                      //198,199,ģʽ�л���ת������ֵ
	  0x00,0xC8,											//200,201,ģʽ�л���ת������ֵ
	  0x07,0xD0,											//202,203 ��������ʱ������Q����ϵ��Kp
	  0x00,0x3C,                      //204,205 ��������ʱ������Q����ϵ��Ki
	  0x13,0x88,											//206,207 ��������ʱ������D����ϵ��Kp
	  0x00,0x64,											//208,209 ��������ʱ������D����ϵ��Ki
		0,0,0,0,0,0,0,0,0,0,						//210~219	(R)
		0,0,0,0,0,0,0,0,0,0,						//220~229	(R)
		0,0,0,0,0,0,0,0,0,0,						//230~239	(R)
		0,0,0,0,0,0,0,0,0,0,						//240~249	(R)
		0,0,0,0,0,100,										//250~255	(R)
		/**********************�궨��384~447�������*****************/
	  0,51,			            	        //384,385	Nominal Voltage,valid value from 0 to 65535 V
	  0,40,                           //386,387 NomCurrent,0~65535
	  0,234,				                  //388,389	Nominal Frequency,valid value from 0 to 300Hz
	  0x0D,0xAC,		                  //390,391	Nominal Motor RPM,valid value from 0 to 65535 RPM
	  0,186,				                  //392,393	Max Frequency,valid value from 0 to 300Hz
	  0x14,0xB4,		                  //394,395	Max Motor RPM,valid value from 0 to 65535 RPM
	  0,0,                          	//396,397 �������1
	  0,0,                          	//398,399 �������1
	  0,0,                          	//400,401 �������1
	  0,0,                          	//402,403 �������1
	  8,				                      //404	Motor Poles,valid value from 2 to 32
	  0,                              //405	Motor use the temperature cut output function,0 not use ,1 is use KTY84-150
	  130,                            //406	Motor High Temperature cut output
	  110,                            //407	Motor High TemperatureResume Temperature
	  100,												  	//408 Motor High Measure Weaken Start Temperature   1Byte
	  0,													    //409 Motor High Temperature weaken percent
	  2,				                      //410	Encoder Type,0:NO,1:Quadrature Encoder,2:Hall,3:��ת��ѹ��,4:���Ի���
	  0x00,0x40,			                //411,412	Encoder Pulses,valid value from 0 to 65535 pulses
	  0,															//413,��������Ƶϵ��
	  0,															//414,����������򣬱�֤������������䷽��һ�£�0��ֹ��1ʹ�ܣ�����Ϊ��ʶ����״̬
	  2,															//415,���伫�������������*2
	  0x1F,0xC1,			              	//416,417 ��ת��ѹ����ʼ�Ƕ�
	  0x01,0xFC,											//418,419,	���Ի������(R)
	  0x01,0x9A,											//420,421,	���Ի�����ֵ(R)
	  0x03,0xCC,											//422,423,	���Ի����߶˱����(R)
	  0x00,0x32,			      					//424,425,	���Ի����Ͷ˱����(R)
	  5,                              //426 �����Ƕ�0�����ֵ
	  4,                              //427 �����Ƕ�60�����ֵ
	  6,                              //428 �����Ƕ�120�����ֵ
	  2,                              //429 �����Ƕ�180�����ֵ
	  3,                              //430 �����Ƕ�240�����ֵ
	  1,                              //431 �����Ƕ�300�����ֵ
	  5,                              //432 ǰ��״̬����A�����ص���ʱ�Ļ���ֵ
	  2,                              //433 ǰ��״̬����A�½��ص���ʱ�Ļ���ֵ
	  6,                              //434 ����״̬����A�����ص���ʱ�Ļ���ֵ
	  1,			     	                  //435 ����״̬����A�½��ص���ʱ�Ļ���ֵ
	  0,                              //436�����Ƕ�ƫ������0���30�㣬Ĭ��Ϊ0��
	  0,                              //437
	  0,                              //438
	  0,                              //439
	  0,                              //440
	  0,                              //441
	  0,                              //442
	  0,                              //443
	  0,                              //444
	  0,                              //445
	  0,                              //446
	  100,                            //447
		/**********************�궨��448~511���������ƫ��64*****************/
	  0,51,			            	        //384,385	Nominal Voltage,valid value from 0 to 65535 V
	  0,40,                           //386,387 NomCurrent,0~65535
	  0,234,				                  //388,389	Nominal Frequency,valid value from 0 to 300Hz
	  0x0D,0xAC,		                  //390,391	Nominal Motor RPM,valid value from 0 to 65535 RPM
	  0,186,				                  //392,393	Max Frequency,valid value from 0 to 300Hz
	  0x14,0xB4,		                  //394,395	Max Motor RPM,valid value from 0 to 65535 RPM
	  0,0,                          	//396,397 �������1
	  0,0,                          	//398,399 �������1
	  0,0,                          	//400,401 �������1
	  0,0,                          	//402,403 �������1
	  8,				                      //404	Motor Poles,valid value from 2 to 32
	  0,                              //405	Motor use the temperature cut output function,0 not use ,1 is use KTY84-150
	  130,                            //406	Motor High Temperature cut output
	  110,                            //407	Motor High TemperatureResume Temperature
	  100,												  	//408 Motor High Measure Weaken Start Temperature   1Byte
	  0,													    //409 Motor High Temperature weaken percent
	  2,				                      //410	Encoder Type,0:NO,1:Quadrature Encoder,2:Hall,3:��ת��ѹ��,4:���Ի���
	  0x00,0x40,			                //411,412	Encoder Pulses,valid value from 0 to 65535 pulses
	  0,															//413,��������Ƶϵ��
	  0,															//414,����������򣬱�֤������������䷽��һ�£�0��ֹ��1ʹ�ܣ�����Ϊ��ʶ����״̬
	  2,															//415,���伫�������������*2
	  0x1F,0xC1,			              	//416,417 ��ת��ѹ����ʼ�Ƕ�
	  0x01,0xFC,											//418,419,	���Ի������(R)
	  0x01,0x9A,											//420,421,	���Ի�����ֵ(R)
	  0x03,0xCC,											//422,423,	���Ի����߶˱����(R)
	  0x00,0x32,			      					//424,425,	���Ի����Ͷ˱����(R)
	  5,                              //426 �����Ƕ�0�����ֵ
	  4,                              //427 �����Ƕ�60�����ֵ
	  6,                              //428 �����Ƕ�120�����ֵ
	  2,                              //429 �����Ƕ�180�����ֵ
	  3,                              //430 �����Ƕ�240�����ֵ
	  1,                              //431 �����Ƕ�300�����ֵ
	  5,                              //432 ǰ��״̬����A�����ص���ʱ�Ļ���ֵ
	  2,                              //433 ǰ��״̬����A�½��ص���ʱ�Ļ���ֵ
	  6,                              //434 ����״̬����A�����ص���ʱ�Ļ���ֵ
	  1,			     	                  //435 ����״̬����A�½��ص���ʱ�Ļ���ֵ
	  0,                              //436�����Ƕ�ƫ������0���30�㣬Ĭ��Ϊ0��
	  0,                              //437
	  0,                              //438
	  0,                              //439
	  0,                              //440
	  0,                              //441
	  0,                              //442
	  0,                              //443
	  0,                              //444
	  0,                              //445
	  0,                              //446
	  100                             //447
		/* CAN Calibration Data*/
};

CAL_SECTION const uint16_t Curve_Data[256]=
{
		100,100,95,90,85,80,75,70,60,50,40,30,20,20,20,20,//����Iq
		50,50,50,48,45,40,35,30,25,20,15,10,10,10,10,10,//����Id
	  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,//����Iq
		50,50,50,48,45,40,35,30,25,20,15,10,10,10,10,10,//����Id
	  7000,7000,7000,7500,8000,9000,10000,11500,13000,14500,17000,19000,19000,19000,19000,19000,//ת��Tr
	  60,60,60,65,70,75,80,85,90,95,100,110,120,130,130,130,//ת������
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

APP_SECTION const int32_t sinetable[] = {
0,1608,3212,4808,6393,7962,9512,11039,
12540,14010,15446,16846,18205,19520,20787,22005,
23170,24279,25330,26319,27245,28106,28898,29621,
30273,30852,31357,31785,32138,32413,32610,32728,
32767,32728,32610,32413,32138,31785,31357,30852,
30273,29621,28898,28106,27245,26319,25330,24279,
23170,22005,20787,19520,18205,16846,15446,14010,
12540,11039,9512,7962,6393,4808,3212,1608,
0,-1608,-3212,-4808,-6393,-7962,-9512,-11039,
-12540,-14010,-15446,-16846,-18205,-19520,-20787,-22005,
-23170,-24279,-25330,-26319,-27245,-28106,-28898,-29621,
-30273,-30852,-31357,-31785,-32138,-32413,-32610,-32728,
-32767,-32728,-32610,-32413,-32138,-31785,-31357,-30852,
-30273,-29621,-28898,-28106,-27245,-26319,-25330,-24279,
-23170,-22005,-20787,-19520,-18205,-16846,-15446,-14010,
-12540,-11039,-9512,-7962,-6393,-4808,-3212,-1608
};

APP_SECTION const uint8_t Led_Blinking[16]=
{
  0x11, 0x12, 0x13, 0x14,
  0x21, 0x22, 0x23, 0x24,
  0x31, 0x32, 0x33, 0x34,
  0x41, 0x42, 0x43, 0x44
};

APP_SECTION const int8_t Temperature_Array[32]=
{
	115,93, 80, 72, 64, 59, 54, 50,
	46, 43, 39, 36, 33, 30, 28, 25,
	22, 20, 17, 14, 12, 9,  6,  3,
	0,  -4, -8,-13,-18,-25,-38,-55
};

APP_SECTION const signed char HL_Temp_Array[172]={
130,129,128,127,127,126,125,124,123,123,
122,122,121,121,120,120,119,119,118,118,
117,117,116,116,115,114,114,113,113,112,
111,111,110,110,109,109,108,108,107,107,
106,106,105,105,104,104,103,103,102,102,
101,101, 100,100,99,99,98,98,97,97,
97,96,96,95,95,95,94,94,93,93,
92,92,91,91,90,90,89,89,88,88,
87,87,86,86,85,85,84,84,83,83,
82,82,81,81,80,80,79,79,78,78,
77,77,76,76,75,74,74,73,73,72,
71,71,70,69,69,68,68,67,66,66,
65,64,64,63,63,62,61,61,60,59,
59,58,57,56,56,55,54,53,53,52,
51,50,50,49,48,47,46,45,44,43,
42,41,40,39,38,36,35,34,33,31,
30,28,26,24,22,19,17,15,13,10,5,0
};

APP_SECTION const uint32_t ID_table[16]=
{
	0x001eef05,0x0000011e,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff
};
//8λAD��Ӧ���¶�
//const int16_t KTY84_Temp_Array[15]={-28,-8,10,28,46,65,83,103,125,148,173,200,230,260,300};
//10λAD��Ӧ���¶�
APP_SECTION const int16_t KTY84_Temp_Array[15]={-30,-11,7,24,43,61,81,100,121,144,168,194,223,254,293};

//10λAD��Ӧ���¶�
APP_SECTION const int16_t KTY83_Temp_Array[12]={-30,-15,0,16,32,50,68,88,110,135,160,190};

APP_SECTION const uint16_t X_Coords[X_POINTS] =
{
	0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115
};

APP_SECTION const uint16_t IR_Coff[X_POINTS] =
{
		336,326,316,306,296,286,276,268,260,252,246,240,236,232,228,224,220,216,212,208,206,204,202,200
};

CAL_RAMSCT uint8_t Cal_data[CAL_RAM_SIZE];//reserve for Calibration Data

/*********************************************************************************
                                 ����,����,��־λ
 *********************************************************************************/
volatile uint8_t NTL_Error,Reset_Err,Regen_OverVolt_Err,B_OverVolt_Err;

volatile uint8_t ATD_Value_u8[ATD_CHAN];

volatile uint16_t ATD_Value_u16[ATD_CHAN];

volatile uint16_t Temp_CF,Index_low,Index_up;

volatile uint8_t Wait_Timer;

uint8_t Task500us_En;

volatile uint32_t test4,Timer_500us;

volatile uint8_t M_En,Global_En=0,Regen_En,Stop_Flag;

volatile uint8_t WithFoot,Releas_BRK_DisHP,Neutral_DisHP,PCB_En;

volatile uint8_t Rev_Mode,New_Rev_Mode,Neutral_Flag;

volatile uint8_t SW_ON,SW_OFF;

volatile int8_t Ref_Direction;

volatile uint8_t StandstillCount1;
/*********************************************************************************
                                     ̤�����
 *********************************************************************************/
volatile uint16_t HS_Rated_I,Current_Max_AD,Current_Max_Q;

TORGUE_STR  TPSTorque;
TORGUE_STR  BRKTorque;

/*********************************************************************************
                                       ����
 *********************************************************************************/
volatile Error_STR Alarm_Err;

volatile Error_STR8 Error_Struct0,Error_Struct1;

volatile uint8_t Error_Code;

volatile uint32_t Check_sum_Err;
/*********************************************************************************
 *                                                                               *
 *                                   ʸ������                                    *
 *                                                                               *
 *********************************************************************************/
tPIParm  PIParmQref1;
tPIParm  PIParmQ1;
tPIParm  PIParmD1;
tPIParm  PIParmU;

PSC_STR Ctrl_1;

MOTOR_STR Motor1;

OBSERVER_STR Obser1;

HALL_STR Hall_Str1;

volatile int16_t  Real_OutMax;

volatile uint16_t  Drive_OutMax,Regen_OutMax;

volatile int16_t Tr_MTemp1;

volatile int16_t Id_Min,Id_delta;
