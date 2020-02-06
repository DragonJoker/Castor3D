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
#ifndef ___C3D_SSAA_RENDER_TECHNIQUE_H___
#define ___C3D_SSAA_RENDER_TECHNIQUE_H___

#include "RenderTechnique.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		SSAA Render technique class
	\remark		It creates a supersampled frame buffer and needed attachments,
				<br />it renders to the singlesampled frame buffer
				<br />then it blits this singlesampled frame buffer to the main one with appropriate resize
	\~french
	\brief		Classe de technique de rendu impl�mentant le SSAA
	\remark		On cr�e un tampon d'image supersampl� avec les attaches n�cessaires
				<br />on fait le rendu dans le tampon d'image singlesampl�
				<br />puis on blitte ce tampon d'image singlesampl� dans le principal avec le redimensionnement appropri�
	*/
	class C3D_API SsaaRenderTechnique
		:	public RenderTechniqueBase
	{
	protected:
		//!\~english The supersampled frame buffer	\~french Le tampon d'image supersampl�
		FrameBufferSPtr m_pSsFrameBuffer;
		//!\~english The buffer receiving the supersampled color render	\~french Le tampon recevant le rendu couleur supersampl�
		ColourRenderBufferSPtr m_pSsColorBuffer;
		//!\~english The buffer receiving the supersampled depth render	\~french Le tampon recevant le rendu profondeur supersampl�
		DepthStencilRenderBufferSPtr m_pSsDepthBuffer;
		//!\~english The attach between supersampled colour buffer and supersampled frame buffer	\~french L'attache entre le tampon couleur supersampl� et le tampon supersampl�
		RenderBufferAttachmentSPtr m_pSsColorAttach;
		//!\~english The attach between supersampled depth buffer and supersampled frame buffer	\~french L'attache entre le tampon profondeur supersampl� et le tampon supersampl�
		RenderBufferAttachmentSPtr m_pSsDepthAttach;
		//!\~english The samples count	\~french Le nombre de samples
		int m_iSamplesCount;
		//!\~english The bound frame buffer, depends on m_iSamplesCount	\~french Le tampon d'image li�, d�pend de m_iSamplesCount
		FrameBufferSPtr m_pBoundFrameBuffer;
		//!\~english Supersampled size	\~french Taille supersampl�e
		Castor::Size m_sizeSsaa;
		//!\~english Supersampled rectangle	\~french Rectangle supersampl�
		Castor::Rectangle m_rectSsaa;

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		SsaaRenderTechnique();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderTarget	The render target for this technique
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderTarget	La render target pour cette technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les param�tres de la technique
		 */
		SsaaRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~SsaaRenderTechnique();
		/**
		 *\~english
		 *\brief		Instantiation function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_iSamplesCount	The wanted samples count
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction d'instanciation, utilis�e par la fabrique pour cr�er des objets d'un type donn�
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les param�tres de la technique
		 *\return		Un cl�ne de cet objet
		 */
		static RenderTechniqueBaseSPtr CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

	protected:
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de cr�ation
		 *\return		\p true si tout s'est bien pass�
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
		 *\brief		Fonction de d�but de rendu
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool DoBeginRender();
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_scene			The scene to render
		 *\param[in]	p_camera		The camera through which the scene is viewed
		 *\param[in]	p_ePrimitives	The display mode
		 *\param[in]	p_dFrameTime	The time elapsed since last frame was rendered
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de rendu
		 *\param[in]	p_scene			La sc�ne � rendre
		 *\param[in]	p_camera		La cam�ra � travers laquelle la sc�ne est vue
		 *\param[in]	p_ePrimitives	Le mode d'affichage
		 *\param[in]	p_dFrameTime	Le temps �coul� depuis le rendu de la derni�re frame
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime );
		/**
		 *\~english
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		virtual void DoEndRender();
	};
}

#pragma warning( pop )

#endif
