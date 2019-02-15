/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VERSION_EXCEPTION_H___
#define ___C3D_VERSION_EXCEPTION_H___

#include "Version.hpp"

#include <Exception/Exception.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		01/04/2011
	\~english
	\brief		Version comparison exception
	\~french
	\brief		Exception de comparaison de versions
	*/
	class VersionException
		: public castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	given		The given version
		 *\param[in]	expected	The expected version
		 *\param[in]	file		The file
		 *\param[in]	function	The function
		 *\param[in]	line		The file line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	given		La version donnée
		 *\param[in]	expected	La version voulue
		 *\param[in]	file		Le fichier
		 *\param[in]	function	La fonction
		 *\param[in]	line		La ligne dans le fichier
		 */
		C3D_API VersionException( Version const & given
			, Version const & expected
			, char const * file
			, char const * function
			, uint32_t line );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~VersionException() throw() {}
	};

#	define CASTOR_VERSION_EXCEPTION( given, expected ) throw castor3d::VersionException( given, expected, __FILE__, __FUNCTION__, __LINE__ )
}

#endif
