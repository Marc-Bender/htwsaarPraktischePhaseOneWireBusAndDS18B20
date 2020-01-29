/**
 * \file OneWireFrontEnd.h
 * \brief Contains the more front end functions for working with the One Wire Bus and most importantly writing drivers for various devices.
 * There is an important notice on how the One Wire functionality is implemented in this code base in the Header with the Low Level stuff. It must be obeyed at all times when using these front end functions.
 * Also everything thats true there is true here as well.
**/ 
/***
 *** Enth�lt eher auf den nutzer gerichtete funktionen f�r das arbeiten mit dem onewirebus und f�r das schreiben von treibern f�r verschiedene busger�te. Anmerkung im Header der untergeordneten funktionalit�ten beachten!!!
***/
#ifndef ONEWIREFRONTEND_H_
#define ONEWIREFRONTEND_H_

	#include "OneWireLowLevel.h"

	#define SIZE_OF_ID (8U)

	void sendSkipRomCommand(IN_PAR const GPIOPin_t * const onewirepin, IN_PAR const CallbackFunctionType onSkip);
	void sendMatchRomCommand(IN_PAR const GPIOPin_t * const onewirepin, IN_PAR const uint8_t * const deviceID, IN_PAR const CallbackFunctionType onMatch);
	void writeByteToOneWireBus(IN_PAR const uint8_t byte, IN_PAR const bool startAtZero, IN_PAR const GPIOPin_t * const onewirepin, IN_PAR const CallbackFunctionType onByteSent);
	void readByteFromOneWireBus(OUT_PAR uint8_t * const byte, IN_PAR const bool startAtZero, IN_PAR const GPIOPin_t * const onewirepin, IN_PAR const CallbackFunctionType onByteRead);

#endif