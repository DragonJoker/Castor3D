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
#ifndef ___C3D_RENDER_TECHNIQUE_H___
#define ___C3D_RENDER_TECHNIQUE_H___

#include "Castor3DPrerequisites.hpp"
#include <Rectangle.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

#if !defined( NDEBUG )
#	define DEBUG_BUFFERS	0
#else
#	define DEBUG_BUFFERS	0
#endif

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render technique base class
	\remarks	A render technique is the description of a way to render a render target
	\~french
	\brief		Classe de base d'une technique de rendu
	\remarks	Une technique de rendu est la description d'une manière de rendre une cible de rendu
	*/
	class C3D_API RenderTechniqueBase
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The technique name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom de la technique
		 */
		RenderTechniqueBase( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The technique name
		 *\param[in]	p_renderTarget	The render target for this technique
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom de la technique
		 *\param[in]	p_renderTarget	La render target pour cette technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		RenderTechniqueBase( Castor::String const & p_name, RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RenderTechniqueBase();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		bool Create();
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		void Destroy();
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
		bool Initialise( uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Render begin function
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de début de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		bool BeginRender();
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
		 *\param[in]	p_scene			La scène à rendre
		 *\param[in]	p_camera		La caméra à travers laquelle la scène est vue
		 *\param[in]	p_ePrimitives	Le mode d'affichage
		 *\param[in]	p_dFrameTime	Le temps écoulé depuis le rendu de la dernière frame
		 *\return		\p true si tout s'est bien passé
		 */
		bool Render( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime );
		/**
		 *\~english
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		void EndRender();
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux flags donnés
		 *\param[in]	p_uiProgramFlags	Combinaison de ePROGRAM_FLAG
		 */
		Castor::String GetVertexShaderSource( uint32_t p_uiProgramFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		Castor::String GetPixelShaderSource( uint32_t p_uiFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the technique name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de la technique
		 *\return		La valeur
		 */
		inline Castor::String const & GetName()const
		{
			return m_name;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoCreate() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void DoDestroy() = 0;
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
		virtual bool DoInitialise( uint32_t & p_index ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Render begin function
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de début de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoBeginRender() = 0;
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
		 *\param[in]	p_scene			La scène à rendre
		 *\param[in]	p_camera		La caméra à travers laquelle la scène est vue
		 *\param[in]	p_ePrimitives	Le mode d'affichage
		 *\param[in]	p_dFrameTime	Le temps écoulé depuis le rendu de la dernière frame
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime );
		/**
		 *\~english
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		virtual void DoEndRender() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader correspondant aux flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 */
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_uiProgramFlags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		virtual Castor::String DoGetPixelShaderSource( uint32_t p_uiFlags )const = 0;

	protected:
		//!\~english The technique name	\~french Le nom de la technique
		Castor::String m_name;
		//!\~english The parent render target	\~french La render target parente
		RenderTarget * m_pRenderTarget;
		//!\~english The	engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english The	render system	\~french Le render system
		RenderSystem * m_renderSystem;
		//!\~english The	overlays blend state	\~french L'état de mélange pour les overlays
		BlendStateWPtr m_wp2DBlendState;
		//!\~english The	overlays depth stencil state	\~french L'état de profoundeur et stenxil pour les overlays
		DepthStencilStateWPtr m_wp2DDepthStencilState;
		//!\~english The	technique buffers size	\~french Les dimensions des tampons de la technique
		Castor::Size m_size;
		//!\~english The	technique blit rectangle	\~french Le rectangle de blit de la technique
		Castor::Rectangle m_rect;
		//!\~english The sampler used by the textures	\~french L'échantillonneur utilisé par les textures
		SamplerSPtr m_sampler;
		//!\~english The frame buffer	\~french Le tampon d'image
		FrameBufferSPtr m_pFrameBuffer;
		//!\~english The buffer receiving the color render	\~french Le tampon recevant le rendu couleur
		DynamicTextureSPtr m_pColorBuffer;
		//!\~english The buffer receiving the depth render	\~french Le tampon recevant le rendu profondeur
		DepthStencilRenderBufferSPtr m_pDepthBuffer;
		//!\~english The attach between colour buffer and frame buffer	\~french L'attache entre le tampon couleur et le tampon d'image
		TextureAttachmentSPtr m_pColorAttach;
		//!\~english The attach between depth buffer and frame buffer	\~french L'attache entre le tampon profondeur et le tampon d'image
		RenderBufferAttachmentSPtr m_pDepthAttach;
	};
}

#pragma warning( pop )

#endif
