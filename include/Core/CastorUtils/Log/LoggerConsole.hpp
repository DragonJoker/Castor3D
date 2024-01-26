/*
See LICENSE file in root folder
*/
#ifndef ___CU_LoggerConsole_H___
#define ___CU_LoggerConsole_H___

#include "CastorUtils/Design/NonCopyable.hpp"
#include "CastorUtils/Log/LogModule.hpp"

namespace castor
{
	class ConsoleImpl
		: public NonMovable
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
		virtual ~ConsoleImpl()noexcept = default;
		/**
		 *\~english
		 *\brief		Configures console for the given log type.
		 *\param[in]	logLevel	The log level.
		 *\~french
		 *\brief		Configure la console pour le niveau de log demandé.
		 *\param[in]	logLevel	Le niveau de log.
		 */
		virtual void beginLog( LogType logLevel ) = 0;
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked.
		 *\param[in]	toLog	The text to log.
		 *\param[in]	newLine	Tells if the line end character must be added.
		 *\~french
		 *\brief		Affiche un texte dans la console, ajoute le caractère de fin de ligne si demandé.
		 *\param[in]	toLog	Le texte à logger.
		 *\param[in]	newLine	Dit si le caractère de fin de ligne doit être ajouté.
		 */
		virtual void print( MbString const & toLog, bool newLine ) = 0;
	};

	class ProgramConsole
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		explicit ProgramConsole( bool showConsole );
		/**
		 *\~english
		 *\brief		Configures console for the given log type.
		 *\param[in]	logLevel	The log level.
		 *\~french
		 *\brief		Configure la console pour le niveau de log demandé.
		 *\param[in]	logLevel	Le niveau de log.
		 */
		void beginLog( LogType logLevel );
		/**
		 *\~english
		 *\brief		Prints a text in the console, adds the line end character if asked.
		 *\param[in]	toLog	The text to log.
		 *\param[in]	newLine	Tells if the line end character must be added.
		 *\~french
		 *\brief		Affiche un texte dans la console, ajoute le caractère de fin de ligne si demandé.
		 *\param[in]	toLog	Le texte à logger.
		 *\param[in]	newLine	Dit si le caractère de fin de ligne doit être ajouté.
		 */
		void print( MbString const & toLog, bool newLine );

	private:
		//!\~english	The platform specific console.
		//!\~french		La console spécifique à la plateforme.
		castor::RawUniquePtr< ConsoleImpl > m_console;
	};
}

#endif
