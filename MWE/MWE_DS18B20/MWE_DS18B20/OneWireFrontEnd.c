/**
 * \file OneWireFrontEnd.c
 * \brief Contains the implementation for the front end functions 
**/ 
/***
 *** implementierung f�r die funktionen der h�heren Abstraktionsebene des onewire protokolls.
***/
#include "OneWireFrontEnd.h"

#define BITS_PER_BYTE (8U)

/***
 *** Datentyp f�r die one wire bus kommandos die jeder slave verstehen muss.
***/
typedef enum
{
	OneWireRomCommands_ReadRom = 0x33,
	OneWireRomCommands_MatchRom = 0x55,
	OneWireRomCommands_SkipRom = 0xCC,
}OneWireRomCommands_t;

/***
 *** Interne Datenstrukur nicht un�hnlich der im code f�r den ds18b20 temperatursensor
***/
typedef struct
{
	const uint8_t * deviceId;
	const GPIOPin_t * onewirepin;
	CallbackFunctionType callback;
}OneWireFrontendInternalData_t;

OneWireFrontendInternalData_t internalDataOneWireFrontend;

void sendID(IN_PAR const bool startAtZero);

/***
 *** diese funktion wird auf gerufen wenn das n�chste byte der slave id gesendet werden soll. sie wird indirekt aufgerufen (als callback) wenn das schreiben des letzten bytes fertig ist.
 *** die funktion implementiert eine callback getriebene "schleife" in einer Quasi rekursiven art und weise. diese konzept wird in der code base mehrfach zufinden sein.
***/
/**
 * \brief this function is called when ever the next byte of a slave ID is to be sent. It is called indirectly as a callback when writing the previous byte has finished.
 * It implements a callback driven "loop" the quasi-recursive way. This is a concept you may find a number of times in this code base.
**/ 
void sendNextIDByte(void)
{
	sendID(false);
}

/***
 *** diese funktion sendet die id �ber den bus die sie in einer datenstruktur f�r den internen gebrauch findet. sie wird direkt von einer funktion in einer �bergeordeten statemachine
 *** oder indirekt aus dem callback des funktionsaufrufs der writebytetoonewirebus im inneren dieser funktion aufgerufen. Das sorgt f�r die quasi rekursive schleife (s. oben). 
 *** dieses vorgehen ist n�tig da alle funktionalit�t timer getrieben ist.
 *** im �brigen die letzte funktion in der statemachine f�r die funktionalit�ten in diesem modul die einen gewissen slave addressieren.
***/
/**
 * \brief this function sends out the ID it finds in an internal data structure. This function is called directly from another function in a higher level state machine or 
 * it is called indirectly from the callback of the writeByteToOneWireBus function call in this function. This basically creates a quasi-recursive loop based on callbacks which is 
 * necessary since the functions that are used here are all Timer driven somewhere down the rabbit hole.
 * This is also the last function in the state machines for the functionalities in this module that address a certain slave.
**/ 
void sendID(IN_PAR const bool startAtZero)
{
	static volatile uint8_t positionWithinID;
	
	if(startAtZero)
		positionWithinID=0;
	if (positionWithinID<SIZE_OF_ID)
		writeByteToOneWireBus(internalDataOneWireFrontend.deviceId[positionWithinID++],true,internalDataOneWireFrontend.onewirepin,&sendNextIDByte);
	else
		internalDataOneWireFrontend.callback();
}

/***
 *** Diese funktion wird indirekt aufgerufen wenn die initialisierungssequenz beendet ist und das match rom kommando gesendet wurde. sie startet die "schleife" die alle Slave id bytes
 *** sendet (was von der busspezifikation als n�chster schritt nach dem senden des match rom kommandos vorgesehen wird)
***/
/**
 * \brief this function is called indirectly when the initialization sequence is completed and the match rom command was sent. It starts the quasi-recursive loop that sends out the slave address 
 * that is mandated as a next step after sending a match rom command.
**/
void onMatchRomCommandSent(void)
{
	sendID(true);
}

/***
 *** Diese funktion wird aufgerufen wenn die initialisierungssequenz fertig ist und folglich ein match rom kommando gesendet werden kann (und auch soll)
 *** Sie sendet das Match Rom kommando �ber den bus. im �brigen ein wrapper wegen der bedingungen die in dieser code base an callback funktionen gestellt werden.
***/
/**
 * \brief this function is called when the initialization sequence is complete and thus a rom command is expected to be sent by the protocol.
 * It sends the match rom command to the bus. It also acts as a wrapper due to the requirements for callback functions in this code base.
**/ 
void onReadyToSendMatchRomCommand(void)
{
	writeByteToOneWireBus(OneWireRomCommands_MatchRom,true,internalDataOneWireFrontend.onewirepin, &onMatchRomCommandSent);
}

/***
 *** Analog dazu sendet diese Funktion das skip rom kommando
***/
/**
 * \brief conversely this sends the skip rom command after the initialization sequence reported finish.
**/ 
void onReadyToSendSkipRomCommand(void)
{
	writeByteToOneWireBus(OneWireRomCommands_SkipRom,true,internalDataOneWireFrontend.onewirepin, internalDataOneWireFrontend.callback);
}

/***
 *** Diese funktion startet die statemachine die das matchrom kommand und die obligatorische id sendet. Weil Rom Commandos nicht beliebig gesendet werden k�nnen sorgt diese funktion auch daf�r
 *** das eine Initialisierungssequenz vorher geht.
***/
/**
 * \brief this function starts a state machine that sends out a match rom command and the obligatory deviceID afterwards. Since Rom Commands can not be sent willy-nilly as per protocol this function kicks off an initialization sequence first.
**/ 
__attribute__((optimize("O3"))) void sendMatchRomCommand(IN_PAR const GPIOPin_t * const onewirepin, IN_PAR const uint8_t * const deviceID, IN_PAR const CallbackFunctionType onMatch)
{
	internalDataOneWireFrontend.onewirepin = onewirepin;
	internalDataOneWireFrontend.callback = onMatch;
	internalDataOneWireFrontend.deviceId = deviceID;
	
	generateInitializationSequence(internalDataOneWireFrontend.onewirepin, &onReadyToSendMatchRomCommand);
}

/***
 *** Analog dazu f�r die Skip Rom seite der Welt
***/
/**
 * \brief Conversely this does something very similar for the Skip Rom side of things. Just that since broadcast actions are not used as it stands this code is not used.
**/ 
void sendSkipRomCommand(IN_PAR const GPIOPin_t * const onewirepin, IN_PAR const CallbackFunctionType onSkip)
{
	internalDataOneWireFrontend.onewirepin = onewirepin;
	internalDataOneWireFrontend.callback = onSkip;
	
	generateInitializationSequence(internalDataOneWireFrontend.onewirepin, &onReadyToSendSkipRomCommand);
}
/***
 *** Teil der "schleife" zum lesen von bytes vom one wire bus
***/
/**
 * \brief this function is part of the quasi-recursive-loop for reading bytes from the One Wire Bus.
**/ 
void readNextBitWrapper(void)
{
	readByteFromOneWireBus(NULL, false, NULL, NULL); //< since only the boolean flag is actually used when false is passed in the other parameters can take arbitrary values that will be discarded anyways; so pass NULL
}

/***
 *** diese funktion liest ganze bytes vom one wire bus und startet die "schleife"
***/
/**
 * \brief this function reads entire bytes from the One Wire Bus and starts a quasi-recursive-loop
**/ 
__attribute__((optimize("O3"), hot)) void readByteFromOneWireBus(OUT_PAR uint8_t * const byte, IN_PAR const bool startAtZero, IN_PAR const GPIOPin_t * const onewirepin, IN_PAR const CallbackFunctionType onByteRead)
{
	static uint8_t * lastByte;
	static uint8_t posistionWithinByte;
	static CallbackFunctionType lastCallback;
	static const GPIOPin_t * lastOneWirePin;
	static bool readBit;
	
	// if run the first time with startAtZero = false then this will produce undefined behavior
	// do not use the internalData data object here since this will be used by the other functions in the subordinate state machines!!
	if(startAtZero)
	{
		posistionWithinByte = 0;
		lastByte = byte;
		*byte=0; // ensure that the byte that holds the result is initially zero so that there are no ones in there that a simple or can not override
		lastCallback = onByteRead;
		lastOneWirePin = onewirepin;
	}
	else
	{
		*lastByte |= ((readBit==1) ? 1 : 0 ) << posistionWithinByte;
		posistionWithinByte++;
	}
	
	if (posistionWithinByte < BITS_PER_BYTE)
		readBitFromOneWire(lastOneWirePin, &readBit, &readNextBitWrapper);
	else
		lastCallback();
}

/***
 *** Analog dazu f�r das schreiben auf den Bus
***/
/**
 * \brief conversely for the writing side of things
**/ 
void writeNextBitWrapper(void)
{
	writeByteToOneWireBus(0, false, NULL, NULL);
}

/**
 * \brief conversely for the writing side of things
**/ 
__attribute__((optimize("O3"), hot)) void writeByteToOneWireBus(IN_PAR const uint8_t byte, IN_PAR const bool startAtZero, IN_PAR const GPIOPin_t * const onewirepin, IN_PAR const CallbackFunctionType onByteSent)
{
	static volatile uint8_t lastByte;
	static volatile uint8_t positionWithinByte;
	static volatile CallbackFunctionType lastCallback;
	static const GPIOPin_t * lastOneWirePin;
	
	// if run the first time with startAtZero = false then this will produce undefined behavior
	// do not use the internalData data object here since this will be used by the other functions in the subordinate state machines!!
	if(startAtZero)
	{
		positionWithinByte = 0;
		lastByte = byte;
		lastCallback = onByteSent;
		lastOneWirePin = onewirepin;
	}

	if (positionWithinByte < BITS_PER_BYTE)
	{
		if(lastByte & (1<<(positionWithinByte++)))
			write1BitToOneWire(lastOneWirePin, &writeNextBitWrapper);
		else
			write0BitToOneWire(lastOneWirePin, &writeNextBitWrapper);
	}
	else
	lastCallback();
}
