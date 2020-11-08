/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PluginException_H___
#define ___C3D_PluginException_H___

#include "PluginModule.hpp"

#include <CastorUtils/Exception/Exception.hpp>

namespace castor3d
{
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
}

//! helper macro to ease the use of PluginException
#define CASTOR_PLUGIN_EXCEPTION( description, critical ) throw castor3d::PluginException{ critical, description, __FILE__, __FUNCTION__, uint32_t( __LINE__ ) }

#endif

