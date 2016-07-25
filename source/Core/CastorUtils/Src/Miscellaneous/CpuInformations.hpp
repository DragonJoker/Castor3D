/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CU_CPU_INFORMATIONS___
#define ___CU_CPU_INFORMATIONS___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		09/02/2016
	\~english
	\brief		Retrieves the basic CPU informations.
	\~french
	\brief		Récupère les informations sur le CPU.
	*/
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
