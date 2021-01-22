/*
nexus433
Copyright (C) 2018 aquaticus

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <condition_variable>
#include <thread>
#include <future>
#include <gpiod.h>
#include <condition_variable>
#include "istorage.h"
//#include "diag.h"

typedef uint16_t word;

class Decoder
{
public:
	typedef enum _STATES
	{
		STATE_UNKNOWN,
		STATE_PULSE_START,
		STATE_PULSE_END,
	} STATES;

	Decoder(IStorage &s, int line, int tolerance, int precision) : m_Storage(s),
																   m_Line(line),
																   m_ToleranceUs(tolerance),
																   m_ResolutionUs(precision)
	{
		m_pThread = NULL;
		m_ErrorStop = false;
	}

	bool Decode(word delta);

protected:
	void ThreadFunc();

	IStorage &m_Storage;
	std::promise<void> m_StopFlag;
	std::thread *m_pThread;

	STATES m_State = STATE_UNKNOWN;
	uint8_t m_BitCount = 0;
	uint64_t m_Bits = 0;
	int m_Line;
	int m_ToleranceUs;	// +/- tolerance in microseconds, default 150
	int m_ResolutionUs; // how long go to sleep in microseconds. Lower number (0 best) better precision but higher system load
	std::atomic<bool> m_ErrorStop;
	std::future<void> m_Future;
};
