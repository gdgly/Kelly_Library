/*
 * FOC.h
 *
 *  Created on: 2018Äê8ÔÂ22ÈÕ
 *      Author: Administrator
 */

#ifndef FOC_H_
#define FOC_H_

#include "typedefine.h"

typedef struct{
    uint32_t  qAngle;
    int32_t   qSin;
    int32_t   qCos;
    }SIN_COS_STR;
extern SIN_COS_STR FOC_SinCos1,FOC_SinCos2;

typedef struct{
    int32_t  Ia;
    int32_t  Ib;
    int32_t	 IdRef;
    int32_t	 IqRef;
    int32_t  Id;
    int32_t  Iq;
    }I_CS_STR;
extern I_CS_STR ParkParm_I1,ParkParm_I2;

typedef struct{
    uint32_t Us_Q15;
		int32_t  Vd;
    int32_t  Vq;
    int32_t  V1;
    int32_t  V2;
    int32_t  V3;
    }V_CS_STR;
extern V_CS_STR ParkParm_V1,ParkParm_V2;

typedef struct{
    uint32_t  Ta;
    uint32_t  Tb;
    uint32_t  Tc;
    }SVGEN_STR;
extern SVGEN_STR SVGen;

typedef struct{
    uint32_t	dloop;
		uint32_t  Mod;
    uint32_t  Max;
    uint32_t  PE70;
    uint32_t  Min;
    uint32_t  Cha;
    uint32_t  Chb;
    uint32_t  Chc;
    }PWM_STR;
extern PWM_STR PWM1,PWM2;

APP_SECTION extern void SinCos(SIN_COS_STR *pSinCos, int32_t *sin_tb);
APP_SECTION extern void ClarkePark(I_CS_STR *pIcs, SIN_COS_STR *pSinCos);
APP_SECTION extern void CalcRefVec(V_CS_STR *pVcs, SIN_COS_STR *pSinCos);
APP_SECTION extern void CalcSVGen(V_CS_STR *pVcs, PWM_STR *pPWM);
APP_SECTION extern void CalcTimes(uint32_t t1,uint32_t t2,uint32_t pwm_mod,SVGEN_STR *pSvgen);

#endif /* FOC_H_ */
