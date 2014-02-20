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
#ifndef ___C3D_RenderTechnique___
#define ___C3D_RenderTechnique___

#include "Prerequisites.hpp"
#include <CastorUtils/Rectangle.hpp>

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
	\brief		Render technique factory
	\~french
	\brief		Fabrique de technique de rendu
	*/
	class C3D_API TechniqueFactory
	{
	private:
		DECLARE_MAP( Castor::String, RenderTechniqueBaseSPtr, RenderTechniquePtrStr );
		RenderTechniquePtrStrMap	m_mapRegistered;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		TechniqueFactory();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TechniqueFactory();
		/**
		 *\~english
		 *\brief		Creates a RenderTechnique from a technique name
		 *\param[in]	p_strName		The technique name
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\return		The created RenderTechnique
		 *\~french
		 *\brief		Crée une RenderTechnique à partir d'un nom de technique
		 *\param[in]	p_key			Le type d'objet
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		La RenderTechnique créée
		 */
		RenderTechniqueBaseSPtr Create( Castor::String const & p_strName, RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );
		/**
		 *\~english
		 *\brief		Registers an object type
		 *\param[in]	p_strName	The object type
		 *\~french
		 *\brief		Enregistre un type d'objet
		 *\param[in]	p_strName	Le type d'objet
		 */
		template< class Technique >
		void Register( Castor::String const & p_strName )
		{
			std::shared_ptr< Technique > l_pObj( new Technique );

			if( m_mapRegistered.find( p_strName ) == m_mapRegistered.end() )
			{
				m_mapRegistered.insert( std::make_pair( p_strName, std::static_pointer_cast< RenderTechniqueBase >( l_pObj ) ) );
			}
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render technique base class
	\remark		A render technique is the description of a way to render a render target
	\~french
	\brief		Classe de base d'une technique de rendu
	\remark		Une technique de rendu est la description d'une manière de rendre une cible de rendu
	*/
	class C3D_API RenderTechniqueBase
	{
	protected:
		//!\~english	The parent render target	\~french	La render target parente
		RenderTarget * m_pRenderTarget;
		//!\~english	The	engine	\~french	Le moteur
		Engine * m_pEngine;
		//!\~english	The	render system	\~french	Le render system
		RenderSystem * m_pRenderSystem;
		//!\~english	The	overlays blend state	\~french	L'état de mélange pour les overlays
		BlendStateWPtr m_wp2DBlendState;
		//!\~english	The	overlays depth stencil state	\~french	L'état de profoundeur et stenxil pour les overlays
		DepthStencilStateWPtr m_wp2DDepthStencilState;
		//!\~english	The	technique buffers size	\~french	Les dimensions des tampons de la technique
		Castor::Size m_size;
		//!\~english	The	technique blit rectangle	\~french	Le rectangle de blit de la technique
		Castor::Rect m_rect;
		//!\~english	The frame buffer	\~french	Le tampon d'image
		FrameBufferSPtr m_pFrameBuffer;
		//!\~english	The buffer receiving the color render	\~french	Le tampon recevant le rendu couleur
		DynamicTextureSPtr m_pColorBuffer;
		//!\~english	The buffer receiving the depth render	\~french	Le tampon recevant le rendu profondeur
		DepthRenderBufferSPtr m_pDepthBuffer;
		//!\~english	The attach between colour buffer and frame buffer	\~french	L'attache entre le tampon couleur et le tampon d'image
		TextureAttachmentSPtr m_pColorAttach;
		//!\~english	The attach between depth buffer and frame buffer	\~french	L'attache entre le tampon profondeur et le tampon d'image
		RenderBufferAttachmentSPtr m_pDepthAttach;

	protected:
		friend class TechniqueFactory;
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		RenderTechniqueBase();
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
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		RenderTechniqueBase( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

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
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true if ok
		 */
		bool Initialise();
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
		 *\brief		Clone function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction de clonage, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		virtual RenderTechniqueBaseSPtr Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )=0;

	protected:
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoCreate()=0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void DoDestroy()=0;
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true if ok
		 */
		virtual bool DoInitialise()=0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void DoCleanup()=0;
		/**
		 *\~english
		 *\brief		Render begin function
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de début de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoBeginRender()=0;
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
		virtual void DoEndRender()=0;

	private:
		virtual RenderTechniqueBaseSPtr Clone() { return nullptr; }
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Basic render technique class
	\~french
	\brief		Classe de technique de rendu basique
	*/
	class C3D_API DirectRenderTechnique : public RenderTechniqueBase
	{
	protected:
		friend class TechniqueFactory;
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		DirectRenderTechnique();
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
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		DirectRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DirectRenderTechnique();
		/**
		 *\~english
		 *\brief		Clone function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction de clonage, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		virtual RenderTechniqueBaseSPtr Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

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
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true if ok
		 */
		virtual bool DoInitialise();
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
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		virtual void DoEndRender();
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		MSAA Render technique class
	\remark		It creates a multisampled frame buffer and needed attachments,
				<br />it renders to the singlesampled frame buffer
				<br />then it blits this singlesampled frame buffer to the main one with appropriate resize
	\~french
	\brief		Classe de technique de rendu implémentant le MSAA
	\remark		On crée un tampon d'image supportant le multisampling avec les attaches nécessaires
				<br />on fait le rendu dans le tampon d'image singlesamplé
				<br />puis on blitte ce tampon d'image singlesamplé dans le principal avec le redimensionnement approprié
	*/
	class C3D_API MsaaRenderTechnique : public RenderTechniqueBase
	{
	protected:
		//!\~english	The multisampled frame buffer	\~french	Le tampon d'image multisamplé
		FrameBufferSPtr m_pMsFrameBuffer;
		//!\~english	The buffer receiving the multisampled color render	\~french	Le tampon recevant le rendu couleur multisamplé
		ColourRenderBufferSPtr m_pMsColorBuffer;
		//!\~english	The buffer receiving the multisampled depth render	\~french	Le tampon recevant le rendu profondeur multisamplé
		DepthRenderBufferSPtr m_pMsDepthBuffer;
		//!\~english	The attach between multisampled colour buffer and multisampled frame buffer	\~french	L'attache entre le tampon couleur multisamplé et le tampon multisamplé
		RenderBufferAttachmentSPtr m_pMsColorAttach;
		//!\~english	The attach between multisampled depth buffer and multisampled frame buffer	\~french	L'attache entre le tampon profondeur multisamplé et le tampon multisamplé
		RenderBufferAttachmentSPtr m_pMsDepthAttach;
		//!\~english	The samples count	\~french	Le nombre de samples
		int m_iSamplesCount;
		//!\~english	The bound frame buffer, depends on m_iSamplesCount	\~french	Le tampon d'image lié, dépend de m_iSamplesCount
		FrameBufferSPtr m_pBoundFrameBuffer;
		//!\~english	Multisampled rasteriser states	\~french	Etats du rasteriser pour le multisample
		RasteriserStateWPtr m_wpMsRasteriserState;
		
	protected:
		friend class TechniqueFactory;
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MsaaRenderTechnique();
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
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		MsaaRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MsaaRenderTechnique();
		/**
		 *\~english
		 *\brief		Clone function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_iSamplesCount	The wanted samples count
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction de clonage, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		virtual RenderTechniqueBaseSPtr Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

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
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true if ok
		 */
		virtual bool DoInitialise();
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
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		virtual void DoEndRender();
	};
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
	\brief		Classe de technique de rendu implémentant le SSAA
	\remark		On crée un tampon d'image supersamplé avec les attaches nécessaires
				<br />on fait le rendu dans le tampon d'image singlesamplé
				<br />puis on blitte ce tampon d'image singlesamplé dans le principal avec le redimensionnement approprié
	*/
	class C3D_API SsaaRenderTechnique : public RenderTechniqueBase
	{
	protected:
		//!\~english	The supersampled frame buffer	\~french	Le tampon d'image supersamplé
		FrameBufferSPtr m_pSsFrameBuffer;
		//!\~english	The buffer receiving the supersampled color render	\~french	Le tampon recevant le rendu couleur supersamplé
		ColourRenderBufferSPtr m_pSsColorBuffer;
		//!\~english	The buffer receiving the supersampled depth render	\~french	Le tampon recevant le rendu profondeur supersamplé
		DepthRenderBufferSPtr m_pSsDepthBuffer;
		//!\~english	The attach between supersampled colour buffer and supersampled frame buffer	\~french	L'attache entre le tampon couleur supersamplé et le tampon supersamplé
		RenderBufferAttachmentSPtr m_pSsColorAttach;
		//!\~english	The attach between supersampled depth buffer and supersampled frame buffer	\~french	L'attache entre le tampon profondeur supersamplé et le tampon supersamplé
		RenderBufferAttachmentSPtr m_pSsDepthAttach;
		//!\~english	The samples count	\~french	Le nombre de samples
		int m_iSamplesCount;
		//!\~english	The bound frame buffer, depends on m_iSamplesCount	\~french	Le tampon d'image lié, dépend de m_iSamplesCount
		FrameBufferSPtr m_pBoundFrameBuffer;
		//!\~english	Supersampled size	\~french	Taille supersamplée
		Castor::Size m_sizeSsaa;
		//!\~english	Supersampled rectangle	\~french	Rectangle supersamplé
		Castor::Rect m_rectSsaa;
		
	protected:
		friend class TechniqueFactory;
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
		 *\param[in]	p_params		Les paramètres de la technique
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
		 *\brief		Clone function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_iSamplesCount	The wanted samples count
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction de clonage, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		virtual RenderTechniqueBaseSPtr Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

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
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true if ok
		 */
		virtual bool DoInitialise();
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
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		virtual void DoEndRender();
	};

	class C3D_API DeferredShadingRenderTechnique : public RenderTechniqueBase
	{
	protected:
		struct stVERTEX
		{
			real m_coords[2];
			real m_texture[2];
		};
		typedef enum eDS_TEXTURE CASTOR_TYPE( uint8_t )
		{	eDS_TEXTURE_POSITION
		,	eDS_TEXTURE_DIFFUSE
		,	eDS_TEXTURE_NORMALS
		,	eDS_TEXTURE_TANGENT
		,	eDS_TEXTURE_BITANGENT
		,	eDS_TEXTURE_SPECULAR
		,	eDS_TEXTURE_COUNT
		}	eDS_TEXTURE;
		DECLARE_SMART_PTR( Point3rFrameVariable );
		//!\~english	The various textures	\~french	Les diverses textures
		DynamicTextureSPtr m_pDsTextures[eDS_TEXTURE_COUNT];
		//!\~english	The buffer receiving the depth colour	\~french	Le tampon recevant la couleur de la profondeur
		DepthRenderBufferSPtr m_pDsBufDepth;
		//!\~english	The deferred shading frame buffer	\~french	Le tampon d'image pour le deferred shading
		FrameBufferSPtr m_pDsFrameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer	\~french	Les attaches entre les texture et le tampon deferred shading
		TextureAttachmentSPtr m_pDsTexAttachs[eDS_TEXTURE_COUNT];
		//!\~english	The attach between depth texture and deferred shading frame buffer	\~french	L'attache entre la texture profondeur et le tampon deferred shading
		RenderBufferAttachmentSPtr m_pDsDepthAttach;
		//!\~english	The shader program used to render lights	\~french	Le shader utilisé pour rendre les lumières
		ShaderProgramBaseSPtr m_pDsShaderProgram;
		//!\~english	Buffer elements declaration	\~french	Déclaration des éléments d'un vertex
		BufferDeclarationSPtr m_pDeclaration;
		//!\~english	Vertex array (quad definition)	\~french	Tableau de vertex (définition du quad)
		std::array< BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!\~english	Geometry buffers holder	\~french	Conteneur de buffers de géométries
		GeometryBuffersSPtr m_pGeometryBuffers;
		//!	4 * [3(vertex position) + 2(texture coordinates)]
		stVERTEX m_pBuffer[6];
		//!\~english	The viewport used when rendering is done	\~french	Le viewport utilisé pour rendre la cible sur sa cible (fenêtre ou texture)
		ViewportSPtr m_pViewport;
		//!\~english	The shader variable containing the camera position	\~french	La variable de shader contenant la position de la caméra
		Point3rFrameVariableSPtr m_pShaderCamera;

	protected:
		friend class TechniqueFactory;
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Used only by TechniqueFactory
		 *\~french
		 *\brief		Constructeur
		 *\remark		Utilisé uniquement par TechniqueFactory
		 */
		DeferredShadingRenderTechnique();
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
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		DeferredShadingRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DeferredShadingRenderTechnique();
		/**
		 *\~english
		 *\brief		Clone function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction de clonage, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		virtual RenderTechniqueBaseSPtr Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

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
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true if ok
		 */
		virtual bool DoInitialise();
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
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		virtual void DoEndRender();
	};
}

#pragma warning( pop )

#endif
