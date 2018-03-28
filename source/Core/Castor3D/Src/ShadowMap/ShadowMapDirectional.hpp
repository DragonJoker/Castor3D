/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapDirectional_H___
#define ___C3D_ShadowMapDirectional_H___

#include "Miscellaneous/GaussianBlur.hpp"
#include "ShadowMap/ShadowMap.hpp"

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
		 *\param[in]	engine	The engine.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	scene	La scène.
		 */
		ShadowMapDirectional( Engine & engine
			, Scene & scene );
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
		void render( renderer::Semaphore const & toWait )override;
		/**
		 *\copydoc		castor3d::ShadowMap::debugDisplay
		 */
		void debugDisplay( castor::Size const & size, uint32_t index )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Setters.
		*/
		/**@{*/
		inline renderer::TextureView const & getDepthView()const
		{
			return *m_depthView;
		}
		/**@}*/

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
		 *\copydoc		castor3d::ShadowMap::doGetPixelShaderSource
		 */
		glsl::Shader doGetPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;

	public:
		static renderer::Format constexpr VarianceFormat = renderer::Format::eR32G32_SFLOAT;
		static renderer::Format constexpr LinearDepthFormat = renderer::Format::eR32_SFLOAT;
		static renderer::Format constexpr RawDepthFormat = renderer::Format::eD24_UNORM_S8_UINT;

	private:
		renderer::TexturePtr m_depthTexture;
		renderer::TextureViewPtr m_depthView;
		renderer::FrameBufferPtr m_frameBuffer;
		std::unique_ptr< GaussianBlur > m_blur;
	};
}

#endif
