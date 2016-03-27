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
#ifndef ___C3D_MSAA_RENDER_TECHNIQUE_H___
#define ___C3D_MSAA_RENDER_TECHNIQUE_H___

#include <RenderTechnique.hpp>

#ifndef _WIN32
#	define C3D_MsaaTechnique_API
#else
#	ifdef MsaaTechnique_EXPORTS
#		define C3D_MsaaTechnique_API __declspec(dllexport)
#	else
#		define C3D_MsaaTechnique_API __declspec(dllimport)
#	endif
#endif

namespace Msaa
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		MSAA Render technique class
	\remarks	It creates a multisampled frame buffer and needed attachments,
				<br />it renders to the singlesampled frame buffer
				<br />then it blits this singlesampled frame buffer to the main one with appropriate resize
	\~french
	\brief		Classe de technique de rendu implémentant le MSAA
	\remarks	On crée un tampon d'image supportant le multisampling avec les attaches nécessaires
				<br />on fait le rendu dans le tampon d'image singlesamplé
				<br />puis on blitte ce tampon d'image singlesamplé dans le principal avec le redimensionnement approprié
	*/
	class RenderTechnique
		:	public Castor3D::RenderTechnique
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderTarget	The render target for this technique
		 *\param[in]	p_renderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderTarget	La render target pour cette technique
		 *\param[in]	p_renderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		RenderTechnique( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem * p_renderSystem, Castor3D::Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RenderTechnique();
		/**
		 *\~english
		 *\brief		Instantiation function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_renderSystem	The render system
		 *\param[in]	p_iSamplesCount	The wanted samples count
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction d'instanciation, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_renderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		static Castor3D::RenderTechniqueSPtr CreateInstance( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem * p_renderSystem, Castor3D::Parameters const & p_params );

	protected:
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoCreate();
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void DoDestroy();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_index	The base texture index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_index	L'index de texture de base
		 *\return		\p true if ok
		 */
		virtual bool DoInitialise( uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void DoCleanup();
		/**
		 *\~english
		 *\brief		Render begin function
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de début de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoBeginRender();
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_scene		The scene to render
		 *\param[in]	p_camera	The camera through which the scene is viewed
		 *\param[in]	p_frameTime	The time elapsed since last frame was rendered
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de rendu
		 *\param[in]	p_scene		La scène à rendre
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue
		 *\param[in]	p_frameTime	Le temps écoulé depuis le rendu de la dernière frame
		 *\return		\p true si tout s'est bien passé
		 */
		virtual void DoRender( Castor3D::RenderTechnique::stSCENE_RENDER_NODES & p_nodes, Castor3D::Camera & p_camera, uint32_t p_frameTime );
		/**
		 *\~english
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		virtual void DoEndRender();

	protected:
		//!\~english The multisampled frame buffer	\~french Le tampon d'image multisamplé
		Castor3D::FrameBufferSPtr m_msFrameBuffer;
		//!\~english The buffer receiving the multisampled color render	\~french Le tampon recevant le rendu couleur multisamplé
		Castor3D::ColourRenderBufferSPtr m_pMsColorBuffer;
		//!\~english The buffer receiving the multisampled depth render	\~french Le tampon recevant le rendu profondeur multisamplé
		Castor3D::DepthStencilRenderBufferSPtr m_pMsDepthBuffer;
		//!\~english The attach between multisampled colour buffer and multisampled frame buffer	\~french L'attache entre le tampon couleur multisamplé et le tampon multisamplé
		Castor3D::RenderBufferAttachmentSPtr m_pMsColorAttach;
		//!\~english The attach between multisampled depth buffer and multisampled frame buffer	\~french L'attache entre le tampon profondeur multisamplé et le tampon multisamplé
		Castor3D::RenderBufferAttachmentSPtr m_pMsDepthAttach;
		//!\~english The samples count	\~french Le nombre de samples
		int m_samplesCount;
		//!\~english Multisampled rasteriser states	\~french Etats du rasteriser pour le multisample
		Castor3D::RasteriserStateWPtr m_wpMsRasteriserState;
		//!\~english The technique blit rectangle	\~french Le rectangle de blit de la technique
		Castor::Rectangle m_rect;
	};
}

#endif
