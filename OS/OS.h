/**************************************************************************/
/*!
    @file     LiteFXOS.h
    @author   The Firebrand Forge

    @section LICENSE

    Copyright (C) 2018 The Firebrand Forge Inc

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
/**************************************************************************/
#ifndef SOURCE_LITEFXOS_H_
#define SOURCE_LITEFXOS_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct LightFXOSThread
{
	//static volatile uint32_t *	TimerCounter;
	void (*Function)(void); 		// Current FX to run

	uint32_t	UpdateInterval;		// number of timer ticks (milliseconds) between updates
	uint32_t 	LastUpdate; 		// last update time
	bool 		Run;				// Enable or disable

	bool 		OneShot;			// one-time or repeats
	uint32_t 	TicksRemaining;
	uint32_t 	Ticks;
	void (*OnComplete)(void);

	void * Context;					// option to load context before running
	void (* LoadContext)(void * context);
//	void (* SaveContext)(void*);
//	uint32_t 	StartDelay;
} LITE_FX_OS_THREAD_T;

void LiteFXOS_InitModuleTimerMillis(uint32_t * timerCounter);
//void LiteFXOS_SetTimerCounter(uint32_t updateInterval);
bool LiteFXOS_ProcThread(LITE_FX_OS_THREAD_T * thread);
void LiteFXOS_ProcThreadNRepeat(LITE_FX_OS_THREAD_T * thread, uint32_t n);

void LiteFXOS_InitThreadPeriodicArgContextPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t updateInterval);
void LiteFXOS_InitThreadPeriodicArgContextFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticksPerSecond);
void LiteFXOS_InitThreadOneShotArgContextTicksPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void));
void LiteFXOS_InitThreadOneShotArgContextTicksFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void));
void LiteFXOS_InitThreadOneShotArgContextTicksTime(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void));
void LiteFXOS_InitThreadOneShotArgContextTimeFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t updateInterval, uint32_t ticksPerSecond, void(*onComplete)(void));

void LiteFXOS_StartThreadPeriodicArgContextPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t updateInterval);
void LiteFXOS_StartThreadPeriodicArgContextFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticksPerSecond);
void LiteFXOS_StartThreadOneShotArgContextTicksPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void));
void LiteFXOS_StartThreadOneShotArgContextTicksFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void));
void LiteFXOS_StartThreadOneShotArgContextTicksTime(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void));
void LiteFXOS_StartThreadOneShotArgContextTimeFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t updateInterval, uint32_t ticksPerSecond, void(*onComplete)(void));

void LiteFXOS_SetThreadStart(LITE_FX_OS_THREAD_T * thread);
void LiteFXOS_SetThreadStop(LITE_FX_OS_THREAD_T * thread);
void LiteFXOS_SetThreadContext(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*));
void LiteFXOS_SetThreadFunction(LITE_FX_OS_THREAD_T * thread, void (* function)(void));
void LiteFXOS_SetThreadPeriod(LITE_FX_OS_THREAD_T * thread, uint16_t updateInterval);
void LiteFXOS_SetThreadFreq(LITE_FX_OS_THREAD_T * thread, uint16_t ticksPerSecond);
void LiteFXOS_SetThreadOneShotTicks(LITE_FX_OS_THREAD_T * thread, uint32_t ticks);
void LiteFXOS_SetThreadOneShotOnComplete(LITE_FX_OS_THREAD_T * thread, void(*onComplete)(void));
void LiteFXOS_SetThreadOneShotTicksPeriod(LITE_FX_OS_THREAD_T * thread, uint32_t ticks, uint32_t updateInterval);
void LiteFXOS_SetThreadOneShotTicksFreq(LITE_FX_OS_THREAD_T * thread, uint32_t ticks, uint32_t ticksPerSecond);
void LiteFXOS_SetThreadOneShotTicksTime(LITE_FX_OS_THREAD_T * thread, uint32_t ticks, uint32_t time);
void LiteFXOS_SetThreadOneShotTimeFreq(LITE_FX_OS_THREAD_T * thread, uint32_t time, uint32_t ticksPerSecond);

void LiteFXOS_SetThreadFaster(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc);
void LiteFXOS_SetThreadSlower(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec);
void LiteFXOS_SetThreadFasterArgContextBoundary(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc, uint8_t freqMin, uint8_t freqMax, bool boundaryBehavior);
void LiteFXOS_SetThreadSlowerArgContextBoundary(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec, uint8_t freqMin, uint8_t freqMax, bool boundaryBehavior);

#endif /* SOURCE_LITEFXOS_H_ */
