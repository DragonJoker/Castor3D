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
#ifndef ___C3D_DEFERRED_SHADING_RENDER_TECHNIQUE_H___
#define ___C3D_DEFERRED_SHADING_RENDER_TECHNIQUE_H___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Render/Viewport.hpp>
#include <Shader/UniformBuffer.hpp>

#include <LightPass.hpp>
#include <SsaoPass.hpp>

namespace deferred_msaa
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Deferred lighting Render technique class
	\remarks	Creates up to 6 render buffers (position, diffuse, normals, tangents, bitangents, specular)
				<br />it renders to these buffers (Geometry Pass)
				<br />then renders these buffers into the final one, adding the lights (Light Pass)
	\~french
	\brief		Classe de technique de rendu implémentant le Deferred lighting
	\remarks	On crée jusqu'à 6 tampons de rendu (position, diffuse, normales, tangentes, bitangentes, spéculaire)
				<br />on fait le rendu dans ces tampons (Geometry Pass)
				<br />puis on fait un rendu de ces tampons en ajoutant les lumières (Light Pass)
	*/
	class RenderTechnique
		: public Castor3D::RenderTechnique
	{
	public:
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
		RenderTechnique( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem & p_renderSystem, Castor3D::Parameters const & p_params );
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
		 *\param[in]	p_params		The technique parameters
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction d'instanciation, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_renderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		static Castor3D::RenderTechniqueSPtr CreateInstance( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem & p_renderSystem, Castor3D::Parameters const & p_params );

	protected:
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoInitialise
		 */
		bool DoInitialise( uint32_t & p_index )override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoRenderOpaque
		 */
		void DoRenderOpaque( Castor3D::RenderInfo & p_info )override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoRenderTransparent
		 */
		void DoRenderTransparent( Castor3D::RenderInfo & p_info )override;
		/**
		 *\copydoc		Castor3D::RenderTechnique::DoWriteInto
		 */
		bool DoWriteInto( Castor::TextFile & p_file )override;
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\return		\p false if anything went wrong (the technique is then not usable).
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\return		\p false si un problème est survenu, la technique est alors inutilisable.
		 */
		bool DoInitialiseDeferred( uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Initialises MSAA related stuff.
		 *\return		\p false if anything went wrong (the technique is then not usable).
		 *\~french
		 *\brief		Initialise les données liées au MSAA.
		 *\return		\p false si un problème est survenu, la technique est alors inutilisable.
		 */
		bool DoInitialiseMsaa();
		/**
		 *\~english
		 *\brief		Destroys deferred rendering related stuff.
		 *\~french
		 *\brief		Détruit les données liées au deferred rendering.
		 */
		void DoCleanupDeferred();
		/**
		 *\~english
		 *\brief		Destroys MSAA related stuff.
		 *\~french
		 *\brief		Détruit les données liées au MSAA.
		 */
		void DoCleanupMsaa();
		bool DoInitialiseGeometryPass( uint32_t & p_index );
		bool DoInitialiseLightPass();
		void DoCleanupGeometryPass();
		void DoCleanupLightPass();
		void DoUpdateSceneUbo();
		void DoRenderLights( Castor3D::LightType p_type
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, bool & p_first );

	public:
		static Castor::String const Type;
		static Castor::String const Name;

	private:
		using GeometryBufferTextures = std::array< Castor3D::TextureUnitUPtr, size_t( deferred_common::DsTexture::eCount ) >;
		using GeometryBufferAttachs = std::array< Castor3D::TextureAttachmentSPtr, size_t( deferred_common::DsTexture::eCount ) >;
		using LightPasses = std::array< std::unique_ptr< deferred_common::LightPass >, size_t( Castor3D::LightType::eCount ) >;

		//!\~english	The various textures.
		//!\~french		Les diverses textures.
		GeometryBufferTextures m_lightPassTextures;
		//!\~english	The depth buffer.
		//!\~french		Le tampon de profondeur.
		Castor3D::RenderBufferSPtr m_lightPassDepthBuffer;
		//!\~english	The deferred shading frame buffer.
		//!\~french		Le tampon d'image pour le deferred shading.
		Castor3D::FrameBufferSPtr m_geometryPassFrameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer.
		//!\~french		Les attaches entre les textures et le tampon deferred shading.
		GeometryBufferAttachs m_geometryPassTexAttachs;
		//!\~english	The attachment between depth buffer and deferred shading frame buffer.
		//!\~french		L'attache entre le tampon de profondeur et le tampon deferred shading.
		Castor3D::RenderBufferAttachmentSPtr m_geometryPassDepthAttach;
		//!\~english	The uniform buffer containing the scene data.
		//!\~french		Le tampon d'uniformes contenant les données de scène.
		Castor3D::UniformBuffer m_sceneUbo;
		//!\~english	The shader variable holding the camera position.
		//!\~french		La variable shader contenant la position de la caméra.
		Castor3D::Uniform3fSPtr m_cameraPos;
		//!\~english	The shader variable holding the camera far plane value.
		//!\~french		La variable shader contenant la valeur du plan éloigné de la caméra.
		Castor3D::Uniform1fSPtr m_cameraFarPlane;
		//!\~english	The shader variable holding fog type.
		//!\~french		La variable shader contenant le type de brouillard.
		Castor3D::Uniform1iSPtr m_fogType;
		//!\~english	The shader variable holding fog density.
		//!\~french		La variable shader contenant la densité du brouillard.
		Castor3D::Uniform1fSPtr m_fogDensity;
		//!\~english	The shader program used to render directional lights.
		//!\~french		Le shader utilisé pour rendre les lumières directionnelles.
		LightPasses m_lightPass;
		//!\~english	The shader program used to render directional lights.
		//!\~french		Le shader utilisé pour rendre les lumières directionnelles.
		LightPasses m_lightPassShadow;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		Castor3D::BufferDeclaration m_declaration;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		//!\~english	The viewport used when rendering is done.
		//!\~french		Le viewport utilisé pour rendre la cible sur sa cible (fenêtre ou texture).
		Castor3D::Viewport m_viewport;
		//!\~english	The multisampled frame buffer.
		//!\~french		Le tampon d'image multisamplé
		Castor3D::FrameBufferSPtr m_msaaFrameBuffer;
		//!\~english	The buffer receiving the multisampled color render.
		//!\~french		Le tampon recevant le rendu couleur multisamplé.
		Castor3D::ColourRenderBufferSPtr m_msaaColorBuffer;
		//!\~english	The buffer receiving the multisampled depth render.
		//!\~french		Le tampon recevant le rendu profondeur multisamplé.
		Castor3D::DepthStencilRenderBufferSPtr m_msaaDepthBuffer;
		//!\~english	The attach between multisampled colour buffer and multisampled frame buffer.
		//!\~french		L'attache entre le tampon couleur multisamplé et le tampon multisamplé.
		Castor3D::RenderBufferAttachmentSPtr m_msaaColorAttach;
		//!\~english	The attach between multisampled depth buffer and multisampled frame buffer.
		//!\~french		L'attache entre le tampon profondeur multisamplé et le tampon multisamplé.
		Castor3D::RenderBufferAttachmentSPtr m_msaaDepthAttach;
		//!\~english	The samples count.
		//!\~french		Le nombre de samples.
		int m_samplesCount;
		//!\~english	The technique blit rectangle.
		//!\~french		Le rectangle de blit de la technique.
		Castor::Rectangle m_rect;
		//!\~english	Tells if SSAO is to be used in lighting pass.
		//!\~french		Dit si le SSAO doit être utilisé dans la light pass.
		bool m_ssaoEnabled{ false };
		//!\~english	The SSAO pass.
		//!\~french		La passe SSAO.
		std::unique_ptr< deferred_common::SsaoPass > m_ssao;
	};
}

#endif
