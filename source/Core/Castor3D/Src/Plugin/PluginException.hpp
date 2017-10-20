/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PLUGIN_EXCEPTION_H___
#define ___C3D_PLUGIN_EXCEPTION_H___

#include "Castor3DPrerequisites.hpp"
#include "Exception/Exception.hpp"

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
		 *\param[in]	p_critical		Tells if the exception is critical
		 *\param[in]	p_description	The exception description
		 *\param[in]	p_file			The file
		 *\param[in]	p_function		The function
		 *\param[in]	p_line			The file line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_critical		Dit si l'exception est critique
		 *\param[in]	p_description	La description de l'exception
		 *\param[in]	p_file			Le fichier
		 *\param[in]	p_function		La fonction
		 *\param[in]	p_line			La ligne dans le fichier
		 */
		C3D_API PluginException( bool p_critical, std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line );
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

