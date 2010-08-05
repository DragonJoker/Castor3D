/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___STRING_CONVERTER_H___
#define ___STRING_CONVERTER_H___

#include <sstream>
#include <vector>
#include <string.h>

#ifndef d_dll
#define d_dll
#endif

#include "Macros.h"

namespace General
{ namespace Utils
{
	const size_t MAX_INT_LEN = 33;

	/**
	 * Non-deprecated version of itoa
	 */
	inline char * itoa( int p_value, size_t & p_len, int p_base = 10)
	{
		static char l_buffer[MAX_INT_LEN] = { 0 };
		p_len = 0;
	
		if (p_base < 2 || p_base > 36)
		{
			return l_buffer;
		}
	
		if (p_value == 0)
		{
			l_buffer[0] = '0';
			p_len = 1;
			return l_buffer;
		}
	
		size_t l_indexBuffer = MAX_INT_LEN - 2;
		char l_signe = 0;
	
		if (p_value < 0)
		{
			unsigned int l_value = static_cast <unsigned int> ( -p_value);
			l_signe = '-';
	
			for ( ; l_value && l_indexBuffer ; -- l_indexBuffer, l_value /= p_base)
			{
				l_buffer[l_indexBuffer] = "0123456789abcdefghijklmnopqrstuvwxyz"[ l_value % p_base ];
				p_len ++;
			}
		}
		else
		{
			for ( ; p_value && l_indexBuffer ; -- l_indexBuffer, p_value /= p_base)
			{
				l_buffer[l_indexBuffer] = "0123456789abcdefghijklmnopqrstuvwxyz"[ p_value % p_base ];
				p_len ++;
			}
		}
	
		if (l_signe != 0)
		{
			l_buffer[l_indexBuffer] = l_signe;
			p_len ++;
		}
		else
		{
			l_indexBuffer ++;
		}
	
		return & l_buffer[l_indexBuffer];
	}
	/**
	 * Non-deprecated version of uitoa
	 */
	inline char * uitoa( unsigned int p_value, size_t & p_len, int p_base = 10)
	{
		static char l_buffer[MAX_INT_LEN] = { 0 };
		p_len = 0;
	
		if (p_base < 2 || p_base > 36)
		{
			return l_buffer;
		}
	
		if (p_value == 0)
		{
			l_buffer[0] = '0';
			p_len = 1;
			return l_buffer;
		}
	
		size_t l_indexBuffer = MAX_INT_LEN - 2;
	
		for ( ; p_value && l_indexBuffer ; -- l_indexBuffer, p_value /= p_base)
		{
			l_buffer[l_indexBuffer] = "0123456789abcdefghijklmnopqrstuvwxyz"[ p_value % p_base ];
			p_len ++;
		}
	
		return & l_buffer[++ l_indexBuffer];
	}
}
}

#endif
