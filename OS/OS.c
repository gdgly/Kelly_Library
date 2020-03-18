/**************************************************************************/
/*!
    @file     LiteFXOS.cpp
    @author   The Firebrand Forge

    @section LICENSE

    Copyright (C) 2018 The Firebrand Forge Inc

    This program is free software: you can redistribute it and/or modify
    it under the terupdateInterval of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    @brief Simple thread manager.

	Limitations:
	Not a real context switch, no saving/restoring registers using the stack.
	Main loop cannot block.
	No thread scheduler, all threads are equal priority and must run to completion in sequence.
	Cannot time slice i.e cannot switch threads in the middle of execution

*/
/**************************************************************************/
#include "OS.h"

static volatile uint32_t * TimerCounter; // User app must provide millis timer.
static volatile uint32_t TimerFreq;			//1000 ticks per second using millis
static volatile uint32_t ThreadPeriodMax; //updateInterval max in between ticks
static volatile uint32_t ThreadPeriodMin; //updateInterval min in between ticks

inline static void ProcThread(LITE_FX_OS_THREAD_T * thread)
{
	if (thread->OneShot)
	{
		if (thread->TicksRemaining) // last update on 1 ticks remaining
		{
			thread->TicksRemaining--;
			if (thread->Function) thread->Function();
		}
		else
		{
			thread->Run = false;
			if (thread->OnComplete) thread->OnComplete();
		}
	}
	else
	{
		if (thread->Function) thread->Function();
	}
}

/**************************************************************************/
/*!
    @brief	Public function for processing threads.
    		Run this function inside the main loop.

    @param	thread pointer to the thread to be processed
    @return True if the function ran, otherwise false.
*/
/**************************************************************************/
bool LiteFXOS_ProcThread(LITE_FX_OS_THREAD_T * thread)
{
	if (!thread->Run)	return false;

	if(*TimerCounter - thread->LastUpdate < thread->UpdateInterval)	return 0;

	if(thread->Context && thread->LoadContext) thread->LoadContext(thread->Context);

	thread->LastUpdate = *TimerCounter;
	ProcThread(thread);
	return true;
}

void LiteFXOS_ProcThreadNRepeat(LITE_FX_OS_THREAD_T * thread, uint32_t n)
{
	if(thread->Context && thread->LoadContext) thread->LoadContext(thread->Context);

	for(uint32_t t = 0; t < n; t++)	ProcThread(thread);
}

/*-----------------------------------------------------------------------------
  Thread Init functions - assigns all parameters necessary to run
 *----------------------------------------------------------------------------*/
void LiteFXOS_InitThread(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t updateInterval, uint32_t lastUpdate, bool oneShot, uint32_t ticks, void(*onComplete)(void), void * memory, void (* loadMemory)(void*))
{
	thread->Function		= function;
	thread->UpdateInterval	= updateInterval;		//todo use SetPeriod function or should allow unbounded init?
	thread->LastUpdate		= lastUpdate;
	thread->Run				= false;
	thread->OneShot			= oneShot;
	thread->TicksRemaining 	= ticks;
	thread->Ticks		 	= ticks;
	thread->OnComplete 		= onComplete;
	thread->Context 		= memory;
	thread->LoadContext 	= loadMemory;
}

void LiteFXOS_InitThreadPeriodicArgPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t updateInterval)
{
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, 0, 0, 0, 0, 0);
}

void LiteFXOS_InitThreadPeriodicArgFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticksPerSecond)
{
	uint32_t updateInterval = TimerFreq / ticksPerSecond;
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, 0, 0, 0, 0, 0);
}

// e.g. wipe 2 leds at the rate of 500ms each
void LiteFXOS_InitThreadOneShotArgTicksPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void))
{
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, true, ticks, onComplete, 0, 0);
}

// e.g. wipe 2 leds at the rate of 5 fps
void LiteFXOS_InitThreadOneShotArgTicksFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	uint32_t updateInterval = TimerFreq / ticksPerSecond;
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, true, ticks, onComplete, 0, 0);
}

// time is in timer periods
// e.g. wipe 2 leds at the total time of 1200ms, fader of 100 ticks in 1200ms
void LiteFXOS_InitThreadOneShotArgTicksTime(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t ticks, uint32_t time, void(*onComplete)(void)) //time uinit in timer ticks
{
	uint32_t updateInterval = time / ticks;
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, true, ticks, onComplete, 0, 0);
}

// e.g. rainbow 1000ms at 60fps
void LiteFXOS_InitThreadOneShotArgTimeFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), uint32_t time, uint32_t ticksPerSecond, void(*onComplete)(void)) //time uinit in timer ticks
{
	uint32_t updateInterval = TimerFreq / ticksPerSecond;
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, true, time / updateInterval, onComplete, 0, 0);
}

void LiteFXOS_InitThreadPeriodicArgContextPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t updateInterval)
{
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, 0, 0, 0, memory, loadMemory);
}

void LiteFXOS_InitThreadPeriodicArgContextFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticksPerSecond)
{
	uint32_t updateInterval = TimerFreq / ticksPerSecond;
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, 0, 0, 0, memory, loadMemory);
}

void LiteFXOS_InitThreadOneShotArgContextTicksPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void))
{
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, true, ticks, onComplete, memory, loadMemory);
}

void LiteFXOS_InitThreadOneShotArgContextTicksFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	uint32_t updateInterval = TimerFreq / ticksPerSecond;
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, true, ticks, onComplete, memory, loadMemory);
}

void LiteFXOS_InitThreadOneShotArgContextTicksTime(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t time, void(*onComplete)(void)) //time uinit in timer ticks
{
	uint32_t updateInterval = time / ticks;
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, true, ticks, onComplete, memory, loadMemory);
}

void LiteFXOS_InitThreadOneShotArgContextTimeFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t time, uint32_t ticksPerSecond, void(*onComplete)(void)) //time uinit in timer ticks
{
	uint32_t updateInterval = TimerFreq / ticksPerSecond;
	LiteFXOS_InitThread(thread, function, updateInterval, *TimerCounter - updateInterval, true, time / updateInterval, onComplete, memory, loadMemory);
}

uint32_t LiteFXOS_ConvertMsTimeToTimerTime(uint32_t ms)
{
	//return TimerFreq;
}

/*-----------------------------------------------------------------------------
  Thread Start functions - assigns all parameters and run
 *----------------------------------------------------------------------------*/
void LiteFXOS_StartThread(LITE_FX_OS_THREAD_T * thread)
{
	thread->Run				= true;
	if (thread->OneShot) thread->TicksRemaining = thread->Ticks;
}

void LiteFXOS_StopThread(LITE_FX_OS_THREAD_T * thread)
{
	thread->Run				= false;
}

void LiteFXOS_StartThreadFunction(LITE_FX_OS_THREAD_T * thread, void (* function)(void))
{
	thread->Function		= function;
	thread->Run				= true;
	if (thread->OneShot) thread->TicksRemaining = thread->Ticks;
}

void LiteFXOS_StartThreadPeriodicArgContextPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t updateInterval)
{
	LiteFXOS_InitThreadPeriodicArgContextPeriod(thread, function, memory, loadMemory, updateInterval);
	thread->Run				= true;
}

void LiteFXOS_StartThreadPeriodicArgContextFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticksPerSecond)
{
	LiteFXOS_InitThreadPeriodicArgContextFreq(thread, function, memory, loadMemory, ticksPerSecond);
	thread->Run				= true;
}

void LiteFXOS_StartThreadOneShotArgContextTicksPeriod(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void))
{
	LiteFXOS_InitThreadOneShotArgContextTicksPeriod(thread, function, memory, loadMemory, ticks, updateInterval, onComplete);
	thread->Run				= true;
}

void LiteFXOS_StartThreadOneShotArgContextTicksFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	LiteFXOS_InitThreadOneShotArgContextTicksFreq(thread, function, memory, loadMemory, ticks, ticksPerSecond, onComplete);
	thread->Run				= true;
}

void LiteFXOS_StartThreadOneShotArgContextTicksTime(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t ticks, uint32_t updateInterval, void(*onComplete)(void))
{
	LiteFXOS_InitThreadOneShotArgContextTicksTime(thread, function, memory, loadMemory, ticks, updateInterval, onComplete);
	thread->Run				= true;
}

void LiteFXOS_StartThreadOneShotArgContextTimeFreq(LITE_FX_OS_THREAD_T * thread, void (*function)(void), void * memory, void (* loadMemory)(void*), uint32_t updateInterval, uint32_t ticksPerSecond, void(*onComplete)(void))
{
	LiteFXOS_InitThreadOneShotArgContextTimeFreq(thread, function, memory, loadMemory, updateInterval, ticksPerSecond, onComplete);
	thread->Run				= true;
}


/*-----------------------------------------------------------------------------
  Thread Set functions - Configures thread data members
 *----------------------------------------------------------------------------*/
void LiteFXOS_SetThreadStart(LITE_FX_OS_THREAD_T * thread)
{
	thread->Run				= true;
}

void LiteFXOS_SetThreadStop(LITE_FX_OS_THREAD_T * thread)
{
	thread->Run				= false;
}

void LiteFXOS_SetThreadFunction(LITE_FX_OS_THREAD_T * thread, void (* function)(void))
{
	thread->Function		= function;
}

void LiteFXOS_SetThreadContext(LITE_FX_OS_THREAD_T * thread, void * memory, void (* loadMemory)(void*))
{
	thread->Context 		= memory;
	thread->LoadContext 	= loadMemory;
}

void LiteFXOS_SetThreadPeriod(LITE_FX_OS_THREAD_T * thread, uint16_t updateInterval)
{
	if (updateInterval < ThreadPeriodMin)	thread->UpdateInterval = ThreadPeriodMin;
	if (updateInterval > ThreadPeriodMax)	thread->UpdateInterval = ThreadPeriodMax;
	else									thread->UpdateInterval = updateInterval;
}

void LiteFXOS_SetThreadFreq(LITE_FX_OS_THREAD_T * thread, uint16_t ticksPerSecond)
{
	if 		(ticksPerSecond > TimerFreq/ThreadPeriodMin)	thread->UpdateInterval = TimerFreq / ThreadPeriodMin;
	else if (ticksPerSecond < TimerFreq/ThreadPeriodMax)	thread->UpdateInterval = TimerFreq / ThreadPeriodMax;
	else													thread->UpdateInterval = TimerFreq / ticksPerSecond;
}

void LiteFXOS_SetThreadOneShotTicks(LITE_FX_OS_THREAD_T * thread, uint32_t ticks)
{
	thread->OneShot		= true;
	thread->TicksRemaining 	= ticks;
}

void LiteFXOS_SetThreadOneShotOnComplete(LITE_FX_OS_THREAD_T * thread, void(*onComplete)(void))
{
	thread->OneShot		= true;
	thread->OnComplete 		= onComplete;
}

void LiteFXOS_SetThreadOneShotTicksPeriod(LITE_FX_OS_THREAD_T * thread, uint32_t ticks, uint32_t updateInterval)
{
	thread->OneShot		= true;
	thread->TicksRemaining 	= ticks;
	LiteFXOS_SetThreadPeriod(thread, updateInterval);
}

void LiteFXOS_SetThreadOneShotTicksFreq(LITE_FX_OS_THREAD_T * thread, uint32_t ticks, uint32_t ticksPerSecond)
{
	thread->OneShot		= true;
	thread->TicksRemaining 	= ticks;
	LiteFXOS_SetThreadPeriod(thread, TimerFreq/ticksPerSecond);
}

void LiteFXOS_SetThreadOneShotTicksTime(LITE_FX_OS_THREAD_T * thread, uint32_t ticks, uint32_t updateInterval)
{
	thread->OneShot		= true;
	thread->TicksRemaining 	= ticks;
	LiteFXOS_SetThreadPeriod(thread, updateInterval/ticks);
}

void LiteFXOS_SetThreadOneShotTimeFreq(LITE_FX_OS_THREAD_T * thread, uint32_t updateInterval, uint32_t ticksPerSecond)
{
	thread->OneShot		= true;
	LiteFXOS_SetThreadPeriod(thread, TimerFreq/ticksPerSecond);
	thread->TicksRemaining 	= updateInterval / thread->UpdateInterval; //should round up or down?
}

/*-----------------------------------------------------------------------------
  Thread Set functions - change run rate
 *----------------------------------------------------------------------------*/
void LiteFXOS_SetThreadFaster(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc)
{
	uint32_t freqCurrent = TimerFreq / thread->UpdateInterval;
	uint32_t freqMax = TimerFreq / ThreadPeriodMin;

	if(freqCurrent + freqInc > freqMax)		freqCurrent = freqMax;
	else									freqCurrent += freqInc;

	thread->UpdateInterval = TimerFreq / freqCurrent;
}

void LiteFXOS_SetThreadSlower(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec)
{
	uint32_t freqCurrent = TimerFreq / thread->UpdateInterval;
	uint32_t freqMin = TimerFreq / ThreadPeriodMax;

	if(freqCurrent - freqDec < freqMin)		freqCurrent = freqMin;
	else									freqCurrent -= freqDec;

	thread->UpdateInterval = TimerFreq / freqCurrent;
}

void LiteFXOS_SetThreadFasterArgContextBoundary(LITE_FX_OS_THREAD_T * thread, uint8_t freqInc, uint8_t freqMin, uint8_t freqMax, bool boundaryBehavior)
{
	uint32_t freqCurrent = TimerFreq / thread->UpdateInterval;

	if(freqCurrent + freqInc > freqMax)
	{
		if (boundaryBehavior)	freqCurrent = freqMax;
		else					freqCurrent = freqMin;
	}
	else
	{
		freqCurrent += freqInc;
	}

	thread->UpdateInterval = TimerFreq / freqCurrent;
}

void LiteFXOS_SetThreadSlowerArgContextBoundary(LITE_FX_OS_THREAD_T * thread, uint8_t freqDec, uint8_t freqMin, uint8_t freqMax, bool boundaryBehavior)
{
	uint32_t freqCurrent = TimerFreq / thread->UpdateInterval;

	if(freqCurrent - freqDec < freqMin)
	{
		if (boundaryBehavior)		freqCurrent = freqMin;
		else						freqCurrent = freqMax;
	}
	else
	{
		freqCurrent -= freqDec;
	}

	thread->UpdateInterval = TimerFreq / freqCurrent;
}

/*-----------------------------------------------------------------------------
  Module Init
 *----------------------------------------------------------------------------*/
/**************************************************************************/
/*!
    @brief	Easy init for ms base timer

    @param	pointer to TimerCounter
    @return void
*/
/**************************************************************************/
void LiteFXOS_InitMillis(uint32_t * timerCounter)
{
	TimerCounter = timerCounter;
	TimerFreq = 1000;
	ThreadPeriodMax = 10000; //10 seconds
	ThreadPeriodMin = 1;
}

void LiteFXOS_Init(uint32_t * timerCounter, uint32_t timerFreq, uint32_t threadPeriodMin, uint32_t threadPeriodMax)
{
	TimerCounter = timerCounter;
	TimerFreq = timerFreq;
	ThreadPeriodMax = threadPeriodMax; //in timer tick units
	ThreadPeriodMin = threadPeriodMin; //in timer tick units
}
