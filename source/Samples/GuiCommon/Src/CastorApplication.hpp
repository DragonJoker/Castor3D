/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
	\args		T	Doit àtre un descendant de wxApp.
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
		CastorApplication( Castor::String const & p_internalName, Castor::String const & p_displayName, uint32_t p_steps, Castor3D::Version const & p_version );
		/**
		 *\~english
		 *\return		The application name.
		 *\~french
		 *\return		Le nom de l'application.
		 */
		inline Castor::String const & GetInternalName()const
		{
			return m_internalName;
		}
		/**
		 *\~english
		 *\return		The application displayed name.
		 *\~french
		 *\return		Le nom de l'application, tel qu'affiché dans les fenêtres.
		 */
		inline Castor::String const & GetDisplayName()const
		{
			return m_displayName;
		}
		/**
		 *\~english
		 *\return		The file name given in command line with -f option
		 *\~french
		 *\return		Le nom du fichier donnà en ligne de commande via l'option -f
		 */
		inline Castor::String const & GetFileName()const
		{
			return m_fileName;
		}
		/**
		 *\~english
		 *\return		The renderer type given in command line, Castor3D::RENDERER_TYPE_UNDEFINED if none was given
		 *\~french
		 *\return		Le type d'API de rendu donnà en ligne de commande, Castor3D::RENDERER_TYPE_UNDEFINED si aucun n'a àtà donnà
		 */
		inline Castor::String const & GetRendererType()const
		{
			return m_rendererType;
		}
		/**
		 *\~english
		 *\return		The Castor3D engine
		 *\~french
		 *\return		Le moteur Castor3D
		 */
		inline Castor3D::Engine * GetCastor()const
		{
			return m_castor;
		}
		/**
		 *\~english
		 *\return		The splash screen
		 *\~french
		 *\return		Le splash screen
		 */
		inline SplashScreen * GetSplashScreen()const
		{
			return m_splashScreen;
		}
		/**
		 *\~english
		 *\return		The application version.
		 *\~french
		 *\return		La version de l'application.
		 */
		inline Castor3D::Version const & GetVersion()const
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
		virtual void DoLoadAppImages() = 0;
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
		virtual wxWindow * DoInitialiseMainFrame( SplashScreen * p_splashScreen ) = 0;

		bool DoParseCommandLine();
		bool DoInitialiseLocale( SplashScreen & p_splashScreen );
		bool DoInitialiseCastor( SplashScreen & p_splashScreen );
		void DoLoadPlugins( SplashScreen & p_splashScreen );
		void DoLoadImages( SplashScreen & p_splashScreen );
		void DoCleanup();
		void DoCleanupCastor();

	protected:
		Castor::String m_internalName;
		Castor::String m_displayName;
		Castor3D::Engine * m_castor;

	private:
		Castor::String m_fileName;
		Castor::String m_rendererType;
		std::unique_ptr< wxLocale > m_locale;
		uint32_t m_steps;
		SplashScreen * m_splashScreen;
		Castor3D::Version m_version;
	};
}

#endif
