/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_CPU_INFORMATIONS___
#define ___CU_CPU_INFORMATIONS___

#include "CastorUtilsPrerequisites.hpp"

#include <ctime>

namespace Castor
{
	class CpuInformations
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API CpuInformations();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~CpuInformations();
		/**
		 *\~english
		 *\brief		Retrieves the cores count (physical + cores).
		 *\return		The count.
		 *\~french
		 *\brief		Récupère le nombre de coeurs (physiques + coeurs).
		 *\return		Le compte.
		 */
		CU_API uint8_t GetCoreCount();
		/**
		 *\~english
		 *\return		The MMX instructions support status.
		 *\~french
		 *\return		Le statut du support des instructions MMX.
		 */
		inline bool HasMMX()const
		{
			return m_mmx;
		}
		/**
		 *\~english
		 *\return		The SSE instructions support status.
		 *\~french
		 *\return		Le statut du support des instructions SSE.
		 */
		inline bool HasSSE()const
		{
			return m_sse;
		}
		/**
		 *\~english
		 *\return		The SSE2 instructions support status.
		 *\~french
		 *\return		Le statut du support des instructions SSE2.
		 */
		inline bool HasSSE2()const
		{
			return m_sse2;
		}
		/**
		 *\~english
		 *\return		The SSE3 instructions support status.
		 *\~french
		 *\return		Le statut du support des instructions SSE3.
		 */
		inline bool HasSSE3()const
		{
			return m_sse3;
		}

	private:
		std::string m_name;
		bool m_mmx;
		bool m_sse;
		bool m_sse2;
		bool m_sse3;
	};
}

#endif
