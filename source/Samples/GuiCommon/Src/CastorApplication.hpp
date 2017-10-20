/*
See LICENSE file in root folder
*/
#ifndef ___GC_CASTOR_APPLICATION_H___
#define ___GC_CASTOR_APPLICATION_H___

#include "GuiCommonPrerequisites.hpp"

#include <Miscellaneous/Version.hpp>

#include <memory>

#include <wx/app.h>
#include <wx/intl.h>

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		16/09/2015
	\version	0.8.0
	\~english
	\brief		Base class for Castor applications, initialise the locale and the logger, takes care of command line parameters.
	\argv		T	Must derivate from wxApp.
	\remarks	Command line options can be given with -abr, /abr, or --long
				<br />Supported options are:
				<ul>
				<li>h (--help)	Displays the help
				<li>l (--log)	Defines log level
				<li>f (--file)	Defines a scene file to load at startup
				<li>opengl"		Defines the renderer to OpenGl (the renderer selection dialog will not be displayed)
				<li>directx" )	Defines the renderer to Direct3D 11 (the renderer selection dialog will not be displayed)
				</ul>
	\~french
	\brief		Classe de base pour les applications Castor, initialise la locale et le logger, prend en compte les paramètres en ligne de commande.
	\args		T	doit àtre un descendant de wxApp.
	\remarks	Les options en ligne de commande peuvent àtre passàes ainsi : -abr, /abr, --long.
				<br />Les options supportàes sont les suivantes :
				<ul>
				<li>h (--help)	Affiche l'aide.
				<li>l (--log)	Dàfinit le niveau de log.
				<li>f (--file)	Dàfinit un fichier de scène à charger au lancement.
				<li>opengl"		Dàfinit l'API de rendu à OpenGl (la fenêtre de choix du renderer ne sera pas affichée).
				<li>directx		Dàfinit l'API de rendu à Direct3D 11 (la fenêtre de choix du renderer ne sera pas affichée).
				</ul>
	*/
	class CastorApplication
		: public wxApp
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_internalName	The application name, used to look for language files and printed in logs.
		 *\param[in]	p_displayName	The displayed application name.
		 *\param[in]	p_steps			The initialisation steps count, used for splash screen.
		 *\param[in]	p_version		The application version.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_internalName	Le nom de l'application, utilisà pour rechercher les fichiers de langages, et àcrit dans les logs.
		 *\param[in]	p_displayName	Le nom de l'application, affiché dans les fenêtres.
		 *\param[in]	p_steps			Le nombre d'àtapes de l'initialisation, sert pour l'affichage du splash screen.
		 *\param[in]	p_version		La version de l'application.
		 */
		CastorApplication( castor::String const & p_internalName
						   , castor::String const & p_displayName
						   , uint32_t p_steps
						   , castor3d::Version const & p_version
						   , castor::String const & p_rendererType = castor3d::RENDERER_TYPE_UNDEFINED );
		/**
		 *\~english
		 *\return		The application name.
		 *\~french
		 *\return		Le nom de l'application.
		 */
		inline castor::String const & getInternalName()const
		{
			return m_internalName;
		}
		/**
		 *\~english
		 *\return		The application displayed name.
		 *\~french
		 *\return		Le nom de l'application, tel qu'affiché dans les fenêtres.
		 */
		inline castor::String const & getDisplayName()const
		{
			return m_displayName;
		}
		/**
		 *\~english
		 *\return		The file name given in command line with -f option
		 *\~french
		 *\return		Le nom du fichier donné en ligne de commande via l'option -f
		 */
		inline castor::String const & getFileName()const
		{
			return m_fileName;
		}
		/**
		 *\~english
		 *\return		The renderer type given in command line, castor3d::RENDERER_TYPE_UNDEFINED if none was given
		 *\~french
		 *\return		Le type d'API de rendu donné en ligne de commande, castor3d::RENDERER_TYPE_UNDEFINED si aucun n'a àtà donné
		 */
		inline castor::String const & getRendererType()const
		{
			return m_rendererType;
		}
		/**
		 *\~english
		 *\return		The Castor3D engine
		 *\~french
		 *\return		Le moteur Castor3D
		 */
		inline castor3d::Engine * getCastor()const
		{
			return m_castor;
		}
		/**
		 *\~english
		 *\return		The splash screen
		 *\~french
		 *\return		Le splash screen
		 */
		inline SplashScreen * getSplashScreen()const
		{
			return m_splashScreen;
		}
		/**
		 *\~english
		 *\return		The application version.
		 *\~french
		 *\return		La version de l'application.
		 */
		inline castor3d::Version const & getVersion()const
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
		 *\return		Si false, l'application s'arràtera.
		 */
		virtual bool OnInit();
		/**
		 *\~english
		 *\brief		Cleans up the application.
		 *\~french
		 *\brief		Nettoie l'application.
		 */
		virtual int OnExit();
		/**
		 *\~english
		 *\brief		Used to add application specific images to ImagesLoader
		 *\~french
		 *\brief		Utilisà afin d'ajouter à ImagesLoader des images spécifiques à l'application
		 */
		virtual void doLoadAppImages() = 0;
		/**
		 *\~english
		 *\brief		Should contain the application main frame initialisation code.
		 *\param[in]	p_splashScreen	The splash screen.
		 *\return		The main frame. If nullptr, the application will stop.
		 *\~french
		 *\brief		Devrait contenir le code d'initialisation de la fenêtre principale de l'application.
		 *\param[in]	p_splashScreen	Le splash screen.
		 *\return		La fenêtre principale. Si nullptr, l'application s'arràtera.
		 */
		virtual wxWindow * doInitialiseMainFrame( SplashScreen * p_splashScreen ) = 0;

		bool doParseCommandLine();
		bool doInitialiseLocale( SplashScreen & p_splashScreen );
		bool doInitialiseCastor( SplashScreen & p_splashScreen );
		void doloadPlugins( SplashScreen & p_splashScreen );
		void doLoadImages( SplashScreen & p_splashScreen );
		void doCleanup();
		void doCleanupCastor();

	protected:
		castor::String m_internalName;
		castor::String m_displayName;
		castor3d::Engine * m_castor;

	private:
		castor::String m_fileName;
		castor::String m_rendererType;
		std::unique_ptr< wxLocale > m_locale;
		uint32_t m_steps;
		SplashScreen * m_splashScreen;
		castor3d::Version m_version;
	};
}

#endif
