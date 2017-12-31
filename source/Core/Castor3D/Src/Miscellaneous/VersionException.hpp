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
		 *\param[in]	p_given			The given version
		 *\param[in]	p_expected		The expected version
		 *\param[in]	p_strFile		The file
		 *\param[in]	p_strFunction	The function
		 *\param[in]	p_uiLine		The file line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_given			La version donnée
		 *\param[in]	p_expected		La version voulue
		 *\param[in]	p_strFile		Le fichier
		 *\param[in]	p_strFunction	La fonction
		 *\param[in]	p_uiLine		La ligne dans le fichier
		 */
		C3D_API VersionException( Version const & p_given, Version const & p_expected, char const * p_strFile, char const * p_strFunction, uint32_t p_uiLine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~VersionException() throw() {}
	};

#	define CASTOR_VERSION_EXCEPTION( p_given, p_expected ) throw castor3d::VersionException( p_given, p_expected, __FILE__, __FUNCTION__, __LINE__ )
}

#endif
