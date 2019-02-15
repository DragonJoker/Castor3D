/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapPoint_H___
#define ___C3D_ShadowMapPoint_H___

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/ShadowMap/ShadowMap.hpp"

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
	class ShadowMapPoint
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
		ShadowMapPoint( Engine & engine
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ShadowMapPoint();
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
		C3D_API ashes::TextureView const & getView( uint32_t index )const override;
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La map d'ombres.
		 */
		inline TextureUnit & getTexture()
		{
			return m_shadowMap;
		}
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La map d'ombres.
		 */
		inline TextureUnit const & getTexture()const
		{
			return m_shadowMap;
		}

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
		void doUpdateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::ShadowMap::getVertexShaderSource
		 */
		ShaderPtr doGetVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		castor3d::ShadowMap::doGetPixelShaderSource
		 */
		ShaderPtr doGetPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const override;

	public:
		static ashes::Format constexpr VarianceFormat = ashes::Format::eR32G32_SFLOAT;
		static ashes::Format constexpr LinearDepthFormat = ashes::Format::eR32_SFLOAT;
		static ashes::Format constexpr RawDepthFormat = ashes::Format::eD24_UNORM_S8_UINT;

	private:
		struct FrameBuffer
		{
			ashes::FrameBufferPtr frameBuffer;
			ashes::TextureViewPtr varianceView;
			ashes::TextureViewPtr linearView;
		};
		struct PassData
		{
			ashes::CommandBufferPtr commandBuffer;
			ashes::TexturePtr depthTexture;
			ashes::TextureViewPtr depthView;
			std::array< FrameBuffer, 6u > frameBuffers;
			ashes::SemaphorePtr finished;
			ShadowType shadowType;
			ashes::TextureViewPtr varianceView;
			ashes::TextureViewPtr linearView;
		};
		std::vector< PassData > m_passesData;
	};
}

#endif
