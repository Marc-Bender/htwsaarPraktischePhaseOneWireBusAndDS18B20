/**
 * \file Timer2.h
 * \brief Contains the definitions for the usage of Timer2. This is widely (read almost-entirely) identical with the driver for the Timer0 which will therefor not be commented in any way.
 * Mostly this differs through the availability of certain prescalers. That are not present in Timer0.
 * The functionality for generating interrupts after a certain time is CPU Core Clock dependant and hardcoded. There is no calculation at runtime happening that takes the time period and then
 * computes the optimum configuration for the prescaler and the "stop-counting-at-value"-register. This has been done in a different project and created some respectable amount of code for calculating the 
 * ever same values. The CPU Core Clock is assumed to be 16MHz for those functions while the other functions in this module are clock agnostic.
**/

/***
 *** Enth�lt die Definitionen f�r den Umgang mit Timer2. Weitestgehend identisch mit timer0 der daher unerkl�rt bleibt. 
 *** Hauptunterschied liegt in den Verf�gbaren Prescalern. Timer2 hat einen Prescaler mehr als Timer0.
 *** Die Funktionalit�t f�r das generieren von interrupts nach einer fixen zeit ist cpu takt frequenz abh�ngig und hardcoded mit der annahme von einem 16mhz takt. 
 *** Es wurde keine funktionalit�t f�r das berechnen der entsprechenden werte zur laufzeit implementiert. Dies wurde in einem anderen projekt gemacht und hatte einen respektablen batzen code
 *** zur folge. Im �brigen ist es nicht zielf�hrend die ewig gleichen werte jedes mal aufs neue zu berechnen.
***/

#ifndef TIMER2_H_
#define TIMER2_H_

#include <avr/io.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "marcsLanguageExtension.h"

/***
 *** Z�hlt die Konfigurationsmodi f�r Timer2 auf. Wird gebraucht um die WGM bits zu konfigurieren. 
***/
typedef enum
{
	WaveformGenerationModeTimer2_Normal=0,
	WaveformGenerationModeTimer2_PWM_PhaseCorrect,
	WaveformGenerationModeTimer2_CTC,
	WaveformGenerationModeTimer2_FastPWM,
	WaveformGenerationModeTimer2_RESERVED0,
	WaveformGenerationModeTimer2_PWM_PhaseCorrect_To_OCRA,
	WaveformGenerationModeTimer2_RESERVED1,
	WaveformGenerationModeTimer2_FastPWM_To_OCRA,
} WaveformGenerationModeTimer2_t;

/***
 *** Z�hlt die verschiedenen Prescaler von Timer2 auf. 
***/
typedef enum
{
	PrescalerTimer2_NoClockSource=0,
	PrescalerTimer2_1,
	PrescalerTimer2_8,
	PrescalerTimer2_32,
	PrescalerTimer2_64,
	PrescalerTimer2_128,
	PrescalerTimer2_256,
	PrescalerTimer2_1024
}PrescalerTimer2_t;

void setWaveformGenerationModeTimer2(IN_PAR const WaveformGenerationModeTimer2_t waveformmode);
void setPrescalerTimer2(IN_PAR const PrescalerTimer2_t prescaler);
void enableTimer2OverflowInterrupt();
void stopTimer2();
void defineCallbackOnTimer2Overflow(IN_PAR const CallbackFunctionType callbackFunction);
void resetTimer2(void);
void enableTimer2CaptureCompareAInterrupt(void);
void defineCallbackOnTimer2CaptureCompareAMatch(IN_PAR const CallbackFunctionType callbackFunction);
void letTimer2GenerateInterruptEvery500us(void);
void letTimer2GenerateInterruptEvery750us(void);
void letTimer2GenerateInterruptEveryRoughly60us(void);

#endif /* TIMER2_H_ */
