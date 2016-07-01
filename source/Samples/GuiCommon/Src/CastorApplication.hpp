/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GC_CASTOR_APPLICATION_H___
#define ___GC_CASTOR_APPLICATION_H___

#include "GuiCommonPrerequisites.hpp"

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
				<li>opengl"		Dàfinit l'API de rendu à OpenGl (la fenàtre de choix du renderer ne sera pas affichàe).
				<li>directx		Dàfinit l'API de rendu à Direct3D 11 (la fenàtre de choix du renderer ne sera pas affichàe).
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
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_internalName	Le nom de l'application, utilisà pour rechercher les fichiers de langages, et àcrit dans les logs.
		 *\param[in]	p_displayName	Le nom de l'application, affichà dans les fenàtres.
		 *\param[in]	p_steps			Le nombre d'àtapes de l'initialisation, sert pour l'affichage du splash screen.
		 */
		CastorApplication( Castor::String const & p_internalName, Castor::String const & p_displayName, uint32_t p_steps );
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
		 *\return		Le nom de l'application, tel qu'affichà dans les fenàtres.
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
		 *\brief		Devrait contenir le code d'initialisation de la fenàtre principale de l'application.
		 *\param[in]	p_splashScreen	Le splash screen.
		 *\return		La fenàtre principale. Si nullptr, l'application s'arràtera.
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
	};
}

#endif
