/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_BLOCK_TIMER___
#define ___CASTOR_BLOCK_TIMER___

#include "CastorUtils/Miscellaneous/PreciseTimer.hpp"

namespace castor
{
	class BlockTimer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	function	Caller function
		 *\param[in]	file		Function file
		 *\param[in]	line		Function line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	function	La fonction appelante
		 *\param[in]	file		Le fichier oà se trouve la fonction
		 *\param[in]	line		La ligne dans la fonction
		 */
		CU_API BlockTimer( char const * function
			, char const * file
			, uint32_t line );
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

#define CU_Time() castor::BlockTimer timer##__LINE__{ __FUNCTION__, __FILE__, uint32_t( __LINE__ ) }

#endif
