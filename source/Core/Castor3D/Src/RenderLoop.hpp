/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_RENDER_LOOP_H___
#define ___C3D_RENDER_LOOP_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/10/2015
	\version	0.8.0
	\~english
	\brief		Render loop base class.
	\~french
	\brief		Classe de base d'une boucle de rendu.
	*/
	class RenderLoop
		: public Castor::OwnedBy< Engine >
		, public Castor::Aligned< CASTOR_ALIGN_OF( Castor::Point3r ) >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_wantedFPS		The wanted FPS count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_renderSystem	Le système de rendu.
		 *\param[in]	p_wantedFPS		Le nombre voulu du FPS.
		 */
		C3D_API RenderLoop( Engine & p_engine, RenderSystem * p_renderSystem, uint32_t p_wantedFPS );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~RenderLoop();
		/**
		 *\~english
		 *\brief		Starts render loop.
		 *\remarks		Use only with an asynchronous render loop.
		 *\~french
		 *\brief		Commence le rendu.
		 *\remark		A utiliser uniquement avec une boucle de rendu asynchrone.
		 */
		C3D_API void StartRendering();
		/**
		 *\~english
		 *\brief		Renders one frame.
		 *\remarks		Use only with a synchronous render loop.
		 *\~french
		 *\brief		Dessine une image.
		 *\remark		A utiliser uniquement avec une boucle de rendu synchrone.
		 */
		C3D_API void RenderSyncFrame();
		/**
		 *\~english
		 *\brief		Ends the render.
		 *\remarks		Use only with an asynchronous render loop.
		 *\~french
		 *\brief		Termine le rendu.
		 *\remark		A utiliser uniquement avec une boucle de rendu asynchrone.
		 */
		C3D_API void EndRendering();
		/**
		 *\~english
		 *\brief		Retrieves the wanted frame time.
		 *\return		The time, in seconds.
		 *\~french
		 *\brief		Récupère le temps voulu pour une frame.
		 *\return		Le temps, en secondes.
		 */
		C3D_API double GetFrameTime();
		/**
		 *\~english
		 *\return		The wanted refresh rate.
		 *\~french
		 *\return		La vitesse de rafraichissement.
		 */
		C3D_API uint32_t GetWantedFps();
		/**
		 *\~english
		 *\brief		Creates a render context.
		 *\remarks		If the main context has not been created yet, the first created context will become the main context.
						For asynchronous render loop, this means that the calling thread won't have associated context, preventing calls to RenderSyncFrame
		 *\param[in]	p_window	The render window used to initialise the render context, receives the context.
		 *\~french
		 *\brief		Crée un contexte de rendu.
		 *\remarks		Si le contexte principal n'a pas encore été créé, le premier contexte créé deviendra le contexte principal.
						Pour la boucles de rendu asynchrone, cela signifie que le thread appelant cette fonction sera sans contexte associé, prévenant l'appel de RenderSyncFrame.
		 *\param[in]	p_window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 */
		C3D_API void CreateContext( RenderWindow & p_window );
		/**
		 *\~english
		 *\brief		Show or hide debug overlays.
		 *\param[in]	p_show	The status.
		 *\~french
		 *\brief		Affiche ou cache les incrustations de débogage.
		 *\param[in]	p_show	Le statut.
		 */
		C3D_API void ShowDebugOverlays( bool p_show );

	protected:
		/**
		 *\~english
		 *\brief		Asks for render context creation.
		 *\param[in]	p_window	The render window used to initialise the render context, receives the context.
		 *\return		The created context, or the existing one.
		 *\~french
		 *\brief		Demande la création du contexte de rendu.
		 *\param[in]	p_window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 *\return		Le contexte créé, ou l'existant.
		 */
		C3D_API ContextSPtr DoCreateContext( RenderWindow & p_window );
		/**
		 *\~english
		 *\brief		Starts threaded render loop.
		 *\~french
		 *\brief		Commence le rendu threadé.
		 */
		C3D_API void DoRenderFrame();
		/**
		 *\~english
		 *\brief		Starts threaded render loop.
		 *\~french
		 *\brief		Commence le rendu threadé.
		 */
		C3D_API virtual void DoStartRendering() = 0;
		/**
		 *\~english
		 *\brief		Renders one frame, only if not in render loop.
		 *\~french
		 *\brief		Rend une image, uniquement hors de la boucle de rendu.
		 */
		C3D_API virtual void DoRenderSyncFrame() = 0;
		/**
		 *\~english
		 *\brief		Ends the render, cleans up engine.
		 *\remark		Ends the threaded render loop, if any.
		 *\~french
		 *\brief		Termine le rendu, nettoie le moteur.
		 *\remark		Arrête la boucle de rendu threadé, si elle existe.
		 */
		C3D_API virtual void DoEndRendering() = 0;
		/**
		 *\~english
		 *\brief		Asks for main render context creation.
		 *\param[in]	p_window	The render window used to initialise the render context, receives the context.
		 *\~french
		 *\brief		Demande la création du contexte de rendu principal.
		 *\param[in]	p_window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 */
		C3D_API virtual ContextSPtr DoCreateMainContext( RenderWindow & p_window ) = 0;

	private:
		void DoGpuStep( uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount );
		void DoCpuStep();

	protected:
		//!\~english  The current RenderSystem.	\~french Le RenderSystem courant.
		RenderSystem * m_renderSystem;
		//!\~english The wanted FPS, used in threaded render mode.	\~french Le nombre de FPS souhaité, utilisé en rendu threadé.
		uint32_t m_wantedFPS;
		//!\~english The wanted time for a frame.	\~french Le temps voulu pour une frame.
		double m_frameTime;
		//!\~english The debug overlays.	\~french Les incrustations de débogage.
		std::unique_ptr< DebugOverlays > m_debugOverlays;
	};
}

#undef DECLARE_MANAGED_MEMBER

#endif
