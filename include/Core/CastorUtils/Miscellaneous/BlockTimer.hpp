/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_BLOCK_TIMER___
#define ___CASTOR_BLOCK_TIMER___

#include "CastorUtils/Miscellaneous/PreciseTimer.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"

namespace castor
{
	class BlockTimer
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
		 *\param[in]	file		Le fichier oà se trouve la fonction
		 *\param[in]	line		La ligne dans la fonction
		 */
		CU_API BlockTimer( char const * file
			, char const * function
			, String name
			, uint32_t line );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~BlockTimer()noexcept;

	private:
		std::string_view m_file;
		std::string_view m_function;
		String m_name;
		uint32_t m_line;
		PreciseTimer m_timer;
	};
}

#define CU_TimeNoFile( Name ) castor::BlockTimer CU_Join( timer, __LINE__ ){ "", __FUNCTION__, Name, uint32_t( __LINE__ ) }
#define CU_TimeEx( Name ) castor::BlockTimer CU_Join( timer, __LINE__ ){ __FILE__, __FUNCTION__, Name, uint32_t( __LINE__ ) }
#define CU_Time() castor::BlockTimer CU_Join( timer, __LINE__ ){ __FILE__, __FUNCTION__, "", uint32_t( __LINE__ ) }

#endif
