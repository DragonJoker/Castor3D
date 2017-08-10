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
#ifndef ___CASTOR_BLOCK_TIMER___
#define ___CASTOR_BLOCK_TIMER___

#include "PreciseTimer.hpp"
#include "CastorUtils.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.5.0
	\date		24/04/2012
	\~english
	\brief		Helper class, used to time a block's execution time
	\remark		Call the macro CASTOR_TIME() at the beginning of a block to have a console output when leaving that block
	\~french
	\brief		Classe permettant de mesurer le temps d'exécution d'un bloc
	\remark		Appelez la macro CASTOR_TIME() au début d'un bloc pour avoir une entrée dans la console en sortie du bloc
	*/
	class BlockTimer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_szFunction	Caller function
		 *\param[in]	p_szFile		Function file
		 *\param[in]	p_uiLine		Function line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_szFunction	La fonction appelante
		 *\param[in]	p_szFile		Le fichier oà se trouve la fonction
		 *\param[in]	p_uiLine		La ligne dans la fonction
		 */
		CU_API BlockTimer( char const * p_szFunction, char const * p_szFile, uint32_t p_uiLine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~BlockTimer();

	private:
		String m_strFile;
		String m_strFunction;
		uint32_t const m_uiLine;
		PreciseTimer m_timer;
	};
}

#	define CASTOR_TIME() castor::BlockTimer timer##__LINE__( __FUNCTION__, __FILE__, __LINE__ )

#endif
