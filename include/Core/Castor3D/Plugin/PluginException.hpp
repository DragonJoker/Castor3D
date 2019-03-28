/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PLUGIN_EXCEPTION_H___
#define ___C3D_PLUGIN_EXCEPTION_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <CastorUtils/Exception/Exception.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		01/04/2011
	\~english
	\brief		Plugin loading exception
	\~french
	\brief		Exception de chargement de plug-in
	*/
	class PluginException
		: public castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	critical	Tells if the exception is critical
		 *\param[in]	description	The exception description
		 *\param[in]	file		The file
		 *\param[in]	function	The function
		 *\param[in]	line		The file line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	critical	Dit si l'exception est critique
		 *\param[in]	description	La description de l'exception
		 *\param[in]	file		Le fichier
		 *\param[in]	function	La fonction
		 *\param[in]	line		La ligne dans le fichier
		 */
		C3D_API PluginException( bool critical, std::string const & description, char const * file, char const * function, uint32_t line );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PluginException()throw();
		/**
		 *\~english
		 *\brief		Tells if the exception is critical
		 *\return		The status
		 *\~french
		 *\brief		Dit si l'exception est critique
		 *\return		Le statut
		 */
		inline bool isCritical()const
		{
			return m_critical;
		}

	private:
		bool m_critical;
	};
	//! helper macro to ease the use of PluginException
#	define CASTOR_PLUGIN_EXCEPTION( description, critical ) throw PluginException( critical, description, __FILE__, __FUNCTION__, __LINE__)
}

#endif

