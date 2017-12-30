/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_POINT_H___
#define ___C3D_SHADOW_MAP_PASS_POINT_H___

#include "ShadowMapPass.hpp"
#include "Render/Viewport.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation.
	\~french
	\brief		Implémentation du mappage d'ombres.
	*/
	class ShadowMapPassPoint
		: public ShadowMapPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	scene		The scene.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	scene		La scène.
		 *\param[in]	shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPassPoint( Engine & engine
			, Scene & scene
			, ShadowMap const & shadowMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassPoint();
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		void update( Camera const & camera
			, RenderQueueArray & queues
			, Light & light
			, uint32_t index )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::render
		 */
		void render( uint32_t index )override;

	protected:
		void doRenderNodes( SceneRenderNodes & p_nodes );

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		bool doInitialise( castor::Size const & p_size )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		void doUpdate( RenderQueueArray & p_queues )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::doPreparePipeline
		 */
		void doPreparePipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;

	public:
		static castor::String const ShadowMapUbo;
		static castor::String const WorldLightPosition;
		static castor::String const FarPlane;
		static uint32_t constexpr TextureSize = 1024;
		static uint32_t constexpr UboBindingPoint = 8u;

	private:
		//!\~english	The connection to light's node changed signal.
		//!\~french		La connexion au signal de changement du noeud de la source lumineuse.
		OnSceneNodeChangedConnection m_onNodeChanged;
		//!\~english	The projection matrix.
		//!\~french		La matrice de projection.
		castor::Matrix4x4r m_projection;
		//!\~english	The shadow map configuration data UBO.
		//!\~french		L'UBO de données de configuration de shadow map.
		UniformBuffer m_shadowConfig;
		//!\~english	The variable holding the world light position.
		//!\~french		La variable contenant la position monde de la lumière.
		Uniform3f & m_worldLightPosition;
		//!\~english	The variable holding the camera's far plane.
		//!\~french		La variable contenant la position du plan éloigné de la caméra.
		Uniform1f & m_farPlane;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!\~english	The view matrices for the render of each cube face.
		//!\~french		Les matrices vue pour le dessin de chaque face du cube.
		std::array< castor::Matrix4x4r, size_t( CubeMapFace::eCount ) > m_matrices;
	};
}

#endif
