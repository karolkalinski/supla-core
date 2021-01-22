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

#include <wiringPi.h>
#include <stdio.h>

#include "decoder.h"
#include "diag.h"
#include <atomic>
#include <thread>
#include <chrono>

//tolerance +/- microseconds
#define LENGTH(x, len) (x < len + m_ToleranceUs && x > len - m_ToleranceUs)

// microseconds
#define PULSE_HIGH_LEN 500
#define PULSE_LOW_ZERO 1000
#define PULSE_LOW_ONE 2000
#define SYNC_GAP_LEN 4000

// Packet consist of 36 bits
#define PACKET_BITS_COUNT 36


bool Decoder::Decode( word delta)
{   int input = digitalRead(5);
    bool result = false;
    bool risingEdge = input == 1;
	bool fallingEdge = !risingEdge;
  
	switch (m_State)
	{
	case STATE_UNKNOWN:
		if (risingEdge)
		{
			m_State = STATE_PULSE_START;
		}
		break;

	case STATE_PULSE_START:
		if (fallingEdge && LENGTH(delta, PULSE_HIGH_LEN))
		{
			m_State = STATE_PULSE_END;
		}
		else
		{
			m_State = STATE_UNKNOWN;
		}
		if (m_BitCount > 0)
		{
			printf("M_bitCount %d \n", m_BitCount);
		}
		break;

	case STATE_PULSE_END:
		if (risingEdge)
		{
			if (LENGTH(delta, PULSE_LOW_ONE))
			{
				// "1"
				m_Bits |= (1ULL << (PACKET_BITS_COUNT - 1 - m_BitCount));
				m_BitCount++;

				m_State = STATE_PULSE_START;
			}
			else if (LENGTH(delta, PULSE_LOW_ZERO))
			{
				// "0" (no need to clear bit -- all bits are initially 0)
				m_BitCount++;
				m_State = STATE_PULSE_START;
			}
			else
			{
				m_BitCount = 0;
				m_Bits = 0;
				m_State = STATE_PULSE_START;
			}
		}
		else
		{
			m_State = STATE_UNKNOWN;
		}
		break;
	}

	if (PACKET_BITS_COUNT == m_BitCount)
	{
		printf("Dodano \n");
		m_Storage.Add(m_Bits);
		m_BitCount = 0;
		m_Bits = 0;
		result = true;
	}

	if (STATE_UNKNOWN == m_State)
	{
		m_BitCount = 0;
		m_Bits = 0;
	}

	return result;
}

