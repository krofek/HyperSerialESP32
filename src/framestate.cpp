#include "framestate.h"

#include "led_controller.h"

FrameState frameState;

void FrameState::init(byte input)
{
	currentLed = 0;
	count = input * 0x100;
	CRC = input;
	fletcher1 = 0;
	fletcher2 = 0;
	fletcherExt = 0;
	position = 0;
}

uint8_t FrameState::getCRC()
{
	return CRC;
}

uint16_t FrameState::getCount()
{
	return count;
}

uint16_t FrameState::getFletcher1()
{
	return fletcher1;
}

uint16_t FrameState::getFletcher2()
{
	return fletcher2;
}

uint16_t FrameState::getFletcherExt()
{
	return (fletcherExt != 0x41) ? fletcherExt : 0xaa;
}

uint16_t FrameState::getCurrentLedIndex()
{
	return currentLed++;
}

void FrameState::setProtocolVersion2(bool newVer)
{
	protocolVersion2 = newVer;
}

bool FrameState::isProtocolVersion2()
{
	return protocolVersion2;
}

void FrameState::setState(AwaProtocol newState)
{
	state = newState;
}

AwaProtocol FrameState::getState()
{
	return state;
}

void FrameState::computeCRC(byte input)
{
	count += input;
	CRC = CRC ^ input ^ 0x55;
}

void FrameState::addFletcher(byte input)
{
	fletcher1 = (fletcher1 + (uint16_t)input) % 255;
	fletcher2 = (fletcher2 + fletcher1) % 255;
	fletcherExt = (fletcherExt + (input ^ (position++))) % 255;
}