/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_POINT_H___
#define ___C3D_SHADOW_MAP_PASS_POINT_H___

#include "ShadowMapPass.hpp"
#include "Render/Viewport.hpp"

#include <Buffer/UniformBuffer.hpp>

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
		 *\copydoc		castor3d::ShadowMapPass::updateDeviceDependent
		 */
		void updateDeviceDependent( uint32_t index )override;

	protected:
		void doUpdateNodes( SceneRenderNodes & nodes );

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
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		void doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		void doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
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
		static castor::String const WorldLightPosition;
		static castor::String const FarPlane;
		static uint32_t constexpr TextureSize = 1024;
		static uint32_t constexpr UboBindingPoint = 10u;

		struct Configuration
		{
			castor::Point3f worldLightPosition;
			float farPlane;
		};

	private:
		OnSceneNodeChangedConnection m_onNodeChanged;
		castor::Matrix4x4r m_projection;
		renderer::UniformBufferPtr< Configuration > m_shadowConfig;
		Viewport m_viewport;
		std::array< castor::Matrix4x4r, size_t( CubeMapFace::eCount ) > m_matrices;
	};
}

#endif
