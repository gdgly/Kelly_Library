/*
 * Math.c
 *
 *  Created on: 2018��11��13��
 *      Author: Administrator
 */

#include "Function.h"
#include "Variable.h"
#include "Platform.h"

/*********************************************************************************
 Function:Calculate_Percent(void)
 Description:Calculate Divisor
 Parameters:
 Return:Divisor
*********************************************************************************/
APP_SECTION uint8_t Calculate_Percent(uint8_t a,uint8_t b,uint8_t c)
{
  return ((uint16_t)((uint8_t)(a-b)*100)/c);
}
/****************************************/
/*Function: �����Ŵ���                 */
/*��ڲ���������������������           */
/*���ڲ������������������             */
/****************************************/
APP_SECTION uint16_t sqrt_16(uint32_t M)
{
  uint16_t N, i;
  uint32_t tmp, ttp;   // �����ѭ������

  if (M == 0)               // ��������Ϊ0���������ҲΪ0
    return 0;

  N = 0;
  tmp = (M >> 30);          // ��ȡM�����λ��B[m-1]B[m-2]
  M <<= 2;

  if (tmp >= 1)              //��B[m-1]B[m-2]>=1, ��N���λΪ1
  {
    N ++;                 // �����ǰλΪ1������ΪĬ�ϵ�0
    tmp -= N;
  }
  for (i=15; i>0; i--)      // ��ʣ���15λ
  {
    N <<= 1;              // ����һλ
    tmp <<= 2;
    tmp += (M >> 30);     // ����
    ttp = N;
    ttp = (ttp<<1)+1;
    M <<= 2;
    if (tmp >= ttp)       // �������
    {
      tmp -= ttp;
      N ++;
    }
  }
  return N;
}

/****************************************/
/*Function: ��ͨ�˲���                 */
/*��ڲ�����        */
/*���ڲ�����            */
/****************************************/
APP_SECTION int32_t PT_Function(int32_t input,int32_t *Int,uint32_t times)
{
  int32_t i=0,j=0,k=0;
  i = 1 << times;
  i = i - 1;
  k= 16 - times;
  j = input << k;

  *Int = (int32_t)(((*Int) * i) + j) >> times;
  return((int32_t)((*Int) >> k));
}

/****************************************/
/*Function: u8SearchNear ���ַ������������                 */
/*��ڲ�����        */
/*���ڲ�����            */
/****************************************/
APP_SECTION uint16_t u8SearchNear(uint16_t u16Input_Data,uint16_t *u16Data_Array,uint16_t u8Size_of_Array)
{
  uint16_t Arr_Head=0,Arr_End=0,Index=0;

  Arr_Head = 0;
  Arr_End  = u8Size_of_Array - 1;

  if(u16Input_Data < u16Data_Array[0])
  {
    Index = 0;
  }
  else if(u16Input_Data > u16Data_Array[Arr_End])
  {
    Index = Arr_End;
  }
  else
  {
    while(Arr_Head <= Arr_End)
    {
      Index = (uint16_t)((Arr_Head + Arr_End) >> 1);

      if(u16Input_Data < u16Data_Array[Index])
      {
        Arr_End = Index - 1;
      }
      else if(u16Input_Data > u16Data_Array[Index])
      {
        Arr_Head = Index + 1;
      }
      else
      {
        break;
      }
    }

    if(u16Input_Data > u16Data_Array[Index])
    {
      if( (u16Data_Array[Index + 1] - u16Input_Data) < (u16Input_Data - u16Data_Array[Index]) )
      {
        Index ++;
      }
    }
    else if(u16Input_Data < u16Data_Array[Index])
    {
      if( (u16Input_Data - u16Data_Array[Index - 1]) < (u16Data_Array[Index] - u16Input_Data) )
      {
        Index --;
      }
    }

  }

  return Index;
}

/****************************************/
/*Function: u8SearchSides ���ַ����ұ߽�����                 */
/*��ڲ�����        */
/*���ڲ�����            */
/****************************************/
APP_SECTION uint16_t u8SearchSides(uint16_t u16Input_Data,uint16_t *u16Data_Array,uint16_t u8Size_of_Array)
{
	uint16_t Arr_Head=0,Arr_End=0,Index=0,Index_low=0,Index_up=0;

	Arr_Head = 0;
	Arr_End  = u8Size_of_Array - 1;

	if(u16Input_Data <= u16Data_Array[0])
	{
		Index = 0;
		Index_low = Index;
		Index_up = Index + 1;
	}
	else if(u16Input_Data >= u16Data_Array[Arr_End])
	{
		Index = Arr_End;
		Index_low = Index - 1;
		Index_up = Index;
	}
	else
	{
		while(Arr_Head <= Arr_End)
		{
			Index = (uint16_t)((Arr_Head + Arr_End) >> 1);

			if(u16Input_Data < u16Data_Array[Index])
			{
				Arr_End = Index - 1;
			}
			else if(u16Input_Data > u16Data_Array[Index])
			{
				Arr_Head = Index + 1;
			}
			else
			{
				break;
			}
		}

		if(u16Input_Data >= u16Data_Array[Index])
		{
			Index_low = Index;
			Index_up = Index + 1;
		}
		else
		{
			Index_low = Index - 1;
			Index_up = Index;
		}

	}

	return (uint16_t)((uint16_t)(Index_low << 8) + Index_up);
}
