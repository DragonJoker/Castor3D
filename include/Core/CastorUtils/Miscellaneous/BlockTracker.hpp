/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_BLOCK_TRACKER_H___
#define ___CASTOR_BLOCK_TRACKER_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Helper class, used to track entering and leaving a block
	\remark		Call the macro CU_Track() at the beginning of a block to have a console output when entering or leaving that block
	\~french
	\brief		Classe permettant de tracer les entrées et sorties de bloc
	\remark		Appelez la macro CU_Track() au début d'un bloc pour avoir une entrée dans la console en entrée et en sortie du bloc
	*/
	class BlockTracker
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
		CU_API ~BlockTracker();

	private:
		String m_strFile;
		String m_strFunction;
		uint32_t const m_uiLine;
	};
}

#	define CU_Track( fn ) castor::BlockTracker tracker##__LINE__( __FUNCTION__, __FILE__, __LINE__ )

#endif
