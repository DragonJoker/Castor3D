/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapSpot_H___
#define ___C3D_ShadowMapSpot_H___

#include "Castor3D/Miscellaneous/GaussianBlur.hpp"
#include "Castor3D/ShadowMap/ShadowMap.hpp"

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
	class ShadowMapSpot
		: public ShadowMap
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	scene	La scène.
		 */
		ShadowMapSpot( Engine & engine
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ShadowMapSpot();
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
		C3D_API ashes::TextureView const & getLinearView( uint32_t index )const override;
		C3D_API ashes::TextureView const & getVarianceView( uint32_t index )const override;

	private:
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
		struct PassData
		{
			ashes::CommandBufferPtr commandBuffer;
			ashes::FrameBufferPtr frameBuffer;
			ashes::SemaphorePtr finished;
			ashes::TexturePtr depthTexture;
			ashes::TextureViewPtr depthView;
			std::unique_ptr< GaussianBlur > blur;
		};
		std::vector< PassData > m_passesData;
	};
}

#endif
