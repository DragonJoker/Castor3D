/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapDirectional_H___
#define ___C3D_ShadowMapDirectional_H___

#include "Castor3D/Miscellaneous/GaussianBlur.hpp"
#include "Castor3D/ShadowMap/ShadowMap.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation for directional lights.
	\~french
	\brief		Implémentation du mappage d'ombres pour les lumières directionnelles.
	*/
	class ShadowMapDirectional
		: public ShadowMap
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	scene		The scene.
		 *\param[in]	cascades	The cascades count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	scene		La scène.
		 *\param[in]	cascades	Le nombre de cascades.
		 */
		ShadowMapDirectional( Engine & engine
			, Scene & scene
			, uint32_t cascades );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ShadowMapDirectional();
		/**
		 *\copydoc		castor3d::ShadowMap::update
		 */
		void update( Camera const & camera
			, RenderQueueArray & queues
			, Light & light
			, uint32_t index )override;
		/**
		 *\copydoc		castor3d::ShadowMap::render
		 */
		ashes::Semaphore const & render( ashes::Semaphore const & toWait
			, uint32_t index )override;
		/**
		 *\copydoc		castor3d::ShadowMap::debugDisplay
		 */
		void debugDisplay( ashes::RenderPass const & renderPass
			, ashes::FrameBuffer const & frameBuffer
			, castor::Size const & size, uint32_t index )override;

	private:
		void doInitialiseDepth();
		void doInitialiseFramebuffers();
		/**
		 *\copydoc		castor3d::ShadowMap::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::ShadowMap::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::ShadowMap::doUpdateFlags
		 */
		void doUpdateFlags( PipelineFlags & flags )const override;
		/**
		 *\copydoc		castor3d::ShadowMap::getVertexShaderSource
		 */
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::ShadowMap::doGetPixelShaderSource
		 */
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		static ashes::Format constexpr VarianceFormat = ashes::Format::eR32G32_SFLOAT;
		static ashes::Format constexpr LinearDepthFormat = ashes::Format::eR32_SFLOAT;
		static ashes::Format constexpr RawDepthFormat = ashes::Format::eD24_UNORM_S8_UINT;

	private:
		struct FrameBuffer
		{
			ashes::FrameBufferPtr frameBuffer;
			ashes::TextureViewPtr depthView;
			ashes::TextureViewPtr varianceView;
			ashes::TextureViewPtr linearView;
			std::unique_ptr< GaussianBlur > blur;
		};
		ashes::CommandBufferPtr m_commandBuffer;
		CameraSPtr m_camera;
		ashes::TexturePtr m_depthTexture;
		std::vector< FrameBuffer > m_frameBuffers;
		ShadowType m_shadowType;
		uint32_t m_cascades;
	};
}

#endif
