/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_SPOT_H___
#define ___C3D_SHADOW_MAP_PASS_SPOT_H___

#include "ShadowMapPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation for spot lights.
	\~french
	\brief		Implémentation du mappage d'ombres pour les lumières spot.
	*/
	class ShadowMapPassSpot
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
		C3D_API ShadowMapPassSpot( Engine & engine
			, Scene & scene
			, ShadowMap const & shadowMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassSpot();
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
		/**
		 *\~english
		 *\return		The camera.
		 *\~french
		 *\return		La caméra.
		 */
		inline CameraSPtr getCamera()const
		{
			return m_camera;
		}

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		bool doInitialise( castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		void doUpdate( RenderQueueArray & queues )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::doPreparePipeline
		 */
		void doPreparePipeline( renderer::ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;

	public:
		static castor::String const ShadowMapUbo;
		static castor::String const FarPlane;
		static uint32_t constexpr UboBindingPoint = 8u;
		static uint32_t constexpr TextureSize = 512;

	private:
		//!\~english	The camera created from the light.
		//!\~french		La caméra créée à partir de la lumière.
		CameraSPtr m_camera;
		//!\~english	The shadow map configuration data UBO.
		//!\~french		L'UBO de données de configuration de shadow map.
		UniformBuffer m_shadowConfig;
		//!\~english	The variable holding the camera's far plane.
		//!\~french		La variable contenant la position du plan éloigné de la caméra.
		Uniform1f & m_farPlane;
		//!\~english	The view matrix.
		//!\~french		La matrice vue.
		castor::Matrix4x4r m_view;
	};
}

#endif
