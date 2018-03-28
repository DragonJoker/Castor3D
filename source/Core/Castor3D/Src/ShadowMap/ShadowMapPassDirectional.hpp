/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___
#define ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___

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
	class ShadowMapPassDirectional
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
		C3D_API ShadowMapPassDirectional( Engine & engine
			, Scene & scene
			, ShadowMap const & shadowMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassDirectional();
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		void update( Camera const & camera
			, RenderQueueArray & queues
			, Light & light
			, uint32_t index )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::updateDeviceDependent
		 */
		void updateDeviceDependent( uint32_t index )override;
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
		 *\copydoc		castor3d::RenderPass::doFillDescriptor
		 */
		void doFillDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillDescriptor
		 */
		void doFillDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		void doUpdate( RenderQueueArray & queues )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::doPreparePipeline
		 */
		void doPreparePipeline( ShaderProgramSPtr program
			, renderer::VertexLayoutCRefArray const & layouts
			, PipelineFlags const & flags )override;

	public:
		static castor::String const ShadowMapUbo;
		static castor::String const FarPlane;
		static uint32_t constexpr UboBindingPoint = 10u;
		static uint32_t constexpr TextureSize = 2048;

		struct Configuration
		{
			float farPlane;
		};

	private:
		CameraSPtr m_camera;
		renderer::UniformBufferPtr< Configuration > m_shadowConfig;
		float m_farPlane;
		castor::Matrix4x4r m_view;
	};
}

#endif
