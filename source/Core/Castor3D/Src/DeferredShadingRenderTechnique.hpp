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
#ifndef ___C3D_DEFERRED_SHADING_RENDER_TECHNIQUE_H___
#define ___C3D_DEFERRED_SHADING_RENDER_TECHNIQUE_H___

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
	\brief		Deferred lighting Render technique class
	\remark		Creates up to 6 render buffers (position, diffuse, normals, tangents, bitangents, specular)
				<br />it renders to these buffers (Geometry Pass)
				<br />then renders these buffers into the final one, adding the lights (Light Pass)
	\~french
	\brief		Classe de technique de rendu implémentant le Deferred lighting
	\remark		On crée jusqu'à 6 tampons de rendu (position, diffuse, normales, tangentes, bitangentes, spéculaire)
				<br />on fait le rendu dans ces tampons (Geometry Pass)
				<br />puis on fait un rendu de ces tampons en ajoutant les lumières (Light Pass)
	*/
	class C3D_API DeferredShadingRenderTechnique
		:	public RenderTechniqueBase
	{
	protected:
		typedef enum eDS_TEXTURE CASTOR_TYPE( uint8_t )
		{
			eDS_TEXTURE_POSITION,
			eDS_TEXTURE_DIFFUSE,
			eDS_TEXTURE_NORMALS,
			eDS_TEXTURE_TANGENT,
			eDS_TEXTURE_BITANGENT,
			eDS_TEXTURE_SPECULAR,
			eDS_TEXTURE_COUNT,
		}	eDS_TEXTURE;

		DECLARE_SMART_PTR( Point3rFrameVariable );

	protected:
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
		 *\brief		Instantiation function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction d'instanciation, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		static RenderTechniqueBaseSPtr CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

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
		virtual void DoEndRender();

	protected:
		//!\~english The various textures	\~french Les diverses textures
		DynamicTextureSPtr m_pDsTextures[eDS_TEXTURE_COUNT];
		//!\~english The buffer receiving the depth colour	\~french Le tampon recevant la couleur de la profondeur
		DepthStencilRenderBufferSPtr m_pDsBufDepth;
		//!\~english The deferred shading frame buffer	\~french Le tampon d'image pour le deferred shading
		FrameBufferSPtr m_pDsFrameBuffer;
		//!\~english The attachments between textures and deferred shading frame buffer	\~french Les attaches entre les texture et le tampon deferred shading
		TextureAttachmentSPtr m_pDsTexAttachs[eDS_TEXTURE_COUNT];
		//!\~english The attach between depth texture and deferred shading frame buffer	\~french L'attache entre la texture profondeur et le tampon deferred shading
		RenderBufferAttachmentSPtr m_pDsDepthAttach;
		//!\~english The shader program used to render lights	\~french Le shader utilisé pour rendre les lumières
		ShaderProgramBaseSPtr m_pDsShaderProgram;
		//!\~english Buffer elements declaration	\~french Déclaration des éléments d'un vertex
		BufferDeclarationSPtr m_pDeclaration;
		//!\~english Vertex array (quad definition)	\~french Tableau de vertex (définition du quad)
		std::array< BufferElementGroupSPtr, 4 > m_arrayVertex;
		//!\~english Geometry buffers holder	\~french Conteneur de buffers de géométries
		GeometryBuffersSPtr m_pGeometryBuffers;
		//!	4 * [3(vertex position) 2(texture coordinates)]
		Castor::real m_pBuffer[20];
		//!\~english The viewport used when rendering is done	\~french Le viewport utilisé pour rendre la cible sur sa cible (fenêtre ou texture)
		ViewportSPtr m_pViewport;
		//!\~english The shader variable containing the camera position	\~french La variable de shader contenant la position de la caméra
		Point3rFrameVariableSPtr m_pShaderCamera;
		//!\~english The depth stencil state used by the geometric pass	\~french Le DepthStencilState utilisé par la passe géométrique
		DepthStencilStateSPtr m_pDsGeometricState;
		//!\~english The depth stencil state used byt he lights pass	\~french Le DepthStencilState utilisé par la passe lumières
		DepthStencilStateSPtr m_pDsLightsState;
	};
}

#pragma warning( pop )

#endif
