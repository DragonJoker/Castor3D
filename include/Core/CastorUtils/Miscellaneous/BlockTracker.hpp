/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_BLOCK_TRACKER_H___
#define ___CASTOR_BLOCK_TRACKER_H___

#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"

namespace castor
{
	class BlockTracker
		: public NonMovable
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
		 *\param[in]	file		Le fichier où se trouve la fonction
		 *\param[in]	line		La ligne dans la fonction
		 */
		CU_API BlockTracker( char const * function
			, char const * file
			, uint32_t line );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~BlockTracker()noexcept;

	private:
		String m_strFile;
		String m_strFunction;
		uint32_t const m_uiLine;
	};
}

#define CU_Track() castor::BlockTracker CU_Join( tracker, __LINE__ ){ __FUNCTION__, __FILE__, uint32_t( __LINE__ ) }

#endif
