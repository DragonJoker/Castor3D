/*
See LICENSE file in root folder
*/
#ifndef ___CU_CONSOLE_H___
#define ___CU_CONSOLE_H___

#include "CastorUtilsPrerequisites.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Console info class, platform dependent.
	\~french
	\brief		Classe d'informations de console, dépendante de la plateforme.
	*/
	class ConsoleImpl
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		ConsoleImpl() = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~ConsoleImpl() = default;
		/**
		 *\~english
		 *\brief		Configures console for the given log type.
		 *\param[in]	p_logLevel	The log level.
		 *\~french
		 *\brief		Configure la console pour le niveau de log demandé.
		 *\param[in]	p_logLevel	Le niveau de log.
		 */
		virtual void beginLog( LogType p_logLevel ) = 0;
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked.
		 *\param[in]	p_toLog		The text to log.
		 *\param[in]	p_newLine	Tells if the line end character must be added.
		 *\~french
		 *\brief		Affiche un texte dans la console, ajoute le caractère de fin de ligne si demandé.
		 *\param[in]	p_toLog		Le texte à logger.
		 *\param[in]	p_newLine	Dit si le caractère de fin de ligne doit être ajouté.
		 */
		virtual void print( String const & p_toLog, bool p_newLine ) = 0;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Console base class.
	\~french
	\brief		Classe de base d'une console.
	*/
	class ProgramConsole
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		explicit ProgramConsole( bool p_showConsole );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ProgramConsole();
		/**
		 *\~english
		 *\brief		Configures console for the given log type.
		 *\param[in]	p_logLevel	The log level.
		 *\~french
		 *\brief		Configure la console pour le niveau de log demandé.
		 *\param[in]	p_logLevel	Le niveau de log.
		 */
		void beginLog( LogType p_logLevel );
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked.
		 *\param[in]	p_toLog		The text to log.
		 *\param[in]	p_newLine	Tells if the line end character must be added.
		 *\~french
		 *\brief		Affiche un texte dans la console, ajoute le caractère de fin de ligne si demandé.
		 *\param[in]	p_toLog		Le texte à logger.
		 *\param[in]	p_newLine	Dit si le caractère de fin de ligne doit être ajouté.
		 */
		void print( String const & p_toLog, bool p_newLine );

	protected:
		//!\~english	The platform specific console.
		//!\~french		La console spécifique à la plateforme.
		std::unique_ptr< ConsoleImpl > m_console;
	};
}

#endif
