/*
See LICENSE file in root folder
*/
#ifndef ___GC_CASTOR_APPLICATION_H___
#define ___GC_CASTOR_APPLICATION_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"
#include "GuiCommon/System/ImagesLoader.hpp"

#include <Castor3D/Miscellaneous/Version.hpp>

#include <memory>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5054 )
#include <wx/app.h>
#include <wx/intl.h>
#pragma warning( pop )

namespace GuiCommon
{
	/**
	\~english
	\brief		Base class for Castor applications, initialise the locale and the logger, takes care of command line parameters.
	\remarks	Command line options can be given with -abr, /abr, or --long.<br />
				Supported options are:
				<ul>
				<li>-h (--help)			Displays the help.
				<li>-l (--log)			Defines log level.
				<li>-f (--file)			Defines a scene file to load at startup.
				<li>-a (--validate)		Enables rendering API validation.
				<li>-u (--unlimited)	Disables FPS limit.
				</ul>
	\~french
	\brief		Classe de base pour les applications Castor, initialise la locale et le logger, prend en compte les paramètres en ligne de commande.
	\remarks	Les options en ligne de commande peuvent être passées ainsi : -abr, /abr, --long.<br />
				Les options supportées sont les suivantes :
				<ul>
				<li>-h (--help)			Affiche l'aide.
				<li>-l (--log)			Définit le niveau de log.
				<li>-f (--file)			Définit un fichier de scène à charger au lancement.
				<li>-a (--validate)		Active la validation via l'API de rendu.
				<li>-u (--unlimited)	Désactive la limite de FPS.
				</ul>
	*/
	class CastorApplication
		: public wxApp
	{
	public:
		struct Config
		{
			bool validate{ false };
			bool unlimFPS{ false };
			castor::LogType log{ castor::LogType::eInfo };
			uint32_t fixedFPS{ 60u };
			bool syncRender{ false };
			castor::String rendererName;
			uint32_t gpuIndex{ 0u };
			bool disableUpdateOptimisations{ false };
			castor::String fileName;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	internalName	The application name, used to look for language files and printed in logs.
		 *\param[in]	displayName		The displayed application name.
		 *\param[in]	steps			The initialisation steps count, used for splash screen.
		 *\param[in]	version			The application version.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	internalName	Le nom de l'application, utilisà pour rechercher les fichiers de langages, et àcrit dans les logs.
		 *\param[in]	displayName		Le nom de l'application, affiché dans les fenêtres.
		 *\param[in]	steps			Le nombre d'àtapes de l'initialisation, sert pour l'affichage du splash screen.
		 *\param[in]	version			La version de l'application.
		 */
		CastorApplication( castor::String internalName
			, castor::String displayName
			, uint32_t steps
			, castor3d::Version version
			, uint32_t wantedFPS
			, bool isCastorThreaded
			, castor::String rendererType = castor3d::RenderTypeUndefined );

		static void assertHandler( wxString const & file
			, int line
			, wxString const & func
			, wxString const & cond
			, wxString const & msg );
		/**
		 *\~english
		 *\return		The application name.
		 *\~french
		 *\return		Le nom de l'application.
		 */
		castor::String const & getInternalName()const
		{
			return m_internalName;
		}
		/**
		 *\~english
		 *\return		The application displayed name.
		 *\~french
		 *\return		Le nom de l'application, tel qu'affiché dans les fenêtres.
		 */
		castor::String const & getDisplayName()const
		{
			return m_displayName;
		}
		/**
		 *\~english
		 *\return		The file name given in command line with -f option.
		 *\~french
		 *\return		Le nom du fichier donné en ligne de commande via l'option -f.
		 */
		castor::String const & getFileName()const
		{
			return m_config.fileName;
		}
		/**
		 *\~english
		 *\return		The Castor3D engine.
		 *\~french
		 *\return		Le moteur Castor3D.
		 */
		castor3d::Engine * getCastor()const
		{
			return m_castor.get();
		}
		/**
		 *\~english
		 *\return		The unlimited FPS status.
		 *\~french
		 *\return		Le statut de FPS non limitées.
		 */
		bool isUnlimitedFps()const
		{
			return m_config.unlimFPS;
		}
		/**
		 *\~english
		 *\return		The splash screen.
		 *\~french
		 *\return		Le splash screen.
		 */
		SplashScreen * getSplashScreen()const
		{
			return m_splashScreen;
		}
		/**
		 *\~english
		 *\return		The application version.
		 *\~french
		 *\return		La version de l'application.
		 */
		castor3d::Version const & getVersion()const
		{
			return m_version;
		}

	protected:
		/**
		 *\~english
		 *\brief		Initialises the application.
		 *\remarks		Parses the command line, loads language file, displays splash screen, loads the plug-ins.
		 *\return		If false, the application will stop.
		 *\~french
		 *\brief		Initialise l'application.
		 *\remarks		Analyse la ligne de commande, chage le fichier de langage, affiche le splash screen, charge les plug-ins.
		 *\return		Si false, l'application s'arrêtera.
		 */
		bool OnInit()override;
		/**
		 *\~english
		 *\brief		Cleans up the application.
		 *\~french
		 *\brief		Nettoie l'application.
		 */
		int OnExit()override;
		void OnAssertFailure( const wxChar * file
			, int line
			, const wxChar * func
			, const wxChar * cond
			, const wxChar * msg )override;
		/**
		 *\~english
		 *\brief		Used to add application specific images to ImagesLoader.
		 *\~french
		 *\brief		Utilisà afin d'ajouter à ImagesLoader des images spécifiques à l'application.
		 */
		virtual void doLoadAppImages() = 0;
		/**
		 *\~english
		 *\brief		Should contain the application main frame initialisation code.
		 *\param[in]	splashScreen	The splash screen.
		 *\return		The main frame. If nullptr, the application will stop.
		 *\~french
		 *\brief		Devrait contenir le code d'initialisation de la fenêtre principale de l'application.
		 *\param[in]	splashScreen	Le splash screen.
		 *\return		La fenêtre principale. Si nullptr, l'application s'arrêtera.
		 */
		virtual wxWindow * doInitialiseMainFrame( SplashScreen & splashScreen ) = 0;

		bool doParseCommandLine();
		bool doInitialiseLocale( SplashScreen & splashScreen );
		bool doInitialiseCastor( SplashScreen & splashScreen );
		void doloadPlugins( SplashScreen & splashScreen );
		void doLoadImages( SplashScreen & splashScreen );
		void doCleanup();
		void doCleanupCastor();

	protected:
		ImagesLoader m_imagesLoader;
		castor::String m_internalName;
		castor::String m_displayName;
		std::shared_ptr< castor3d::Engine > m_castor;

	private:
		std::unique_ptr< wxLocale > m_locale;
		uint32_t m_steps;
		SplashScreen * m_splashScreen;
		castor3d::Version m_version;
		Config m_config;
	};
}

#endif
