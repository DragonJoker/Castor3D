/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GaussianBlur_H___
#define ___C3D_GaussianBlur_H___

#include "RenderToTexture/RenderQuad.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		26/07/2017
	\~english
	\brief		Gaussian blur pass.
	\~french
	\brief		Passe flou gaussien.
	*/
	class GaussianBlur
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	texture		The texture.
		 *\param[in]	textureSize	The render area dimensions.
		 *\param[in]	format		The pixel format for the textures to blur.
		 *\param[in]	kernelSize	The kernel coefficients count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	texture		La texture.
		 *\param[in]	textureSize	Les dimensions de la zone de rendu.
		 *\param[in]	format		Le format de pixel des textures à flouter.
		 *\param[in]	kernelSize	Le nombre de coefficients du kernel.
		 */
		C3D_API GaussianBlur( Engine & engine
			, renderer::TextureView const & texture
			, renderer::Extent2D const & textureSize
			, renderer::Format format
			, uint32_t kernelSize );
		/**
		 *\~english
		 *\brief		Blurs given texture.
		 *\~french
		 *\brief		Applique le flou sur la texture.
		 */
		C3D_API void blur();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		inline renderer::RenderPass const & getBlurXPass()const
		{
			REQUIRE( m_blurXPass );
			return *m_blurXPass;
		}

		inline renderer::RenderPass const & getBlurYPass()const
		{
			REQUIRE( m_blurYPass );
			return *m_blurYPass;
		}

		inline renderer::FrameBuffer const & getBlurXFrameBuffer()const
		{
			REQUIRE( m_blurXFbo );
			return *m_blurXFbo;
		}

		inline renderer::FrameBuffer const & getBlurYFrameBuffer()const
		{
			REQUIRE( m_blurYFbo );
			return *m_blurYFbo;
		}

		inline renderer::CommandBuffer const & getBlurXCommandBuffer()const
		{
			return m_blurXQuad.getCommandBuffer();
		}

		inline renderer::CommandBuffer const & getBlurYCommandBuffer()const
		{
			return m_blurYQuad.getCommandBuffer();
		}
		/**@}*/

	private:
		bool doInitialiseBlurXProgram( Engine & engine );
		bool doInitialiseBlurYProgram( Engine & engine );

	public:
		static castor::String const Config;
		static castor::String const Coefficients;
		static castor::String const CoefficientsCount;
		static castor::String const TextureSize;
		static constexpr uint32_t MaxCoefficients{ 60u };

	private:
		struct Configuration
		{
			castor::Point2f textureSize;
			uint32_t blurCoeffsCount;
			std::array< castor::Point4f, 15 > blurCoeffs; // We then allow for 60 coeffs max.
		};

		class RenderQuad
			: public castor3d::RenderQuad
		{
		public:
			RenderQuad( RenderSystem & renderSystem
				, renderer::TextureView const & src
				, renderer::TextureView const & dst
				, renderer::UniformBuffer< Configuration > const & blurUbo
				, renderer::Format format
				, renderer::Extent2D const & size );

		private:
			virtual void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
				, renderer::DescriptorSet & descriptorSet );

			renderer::TextureView const & m_srcView;
			renderer::TextureView const & m_dstView;
			renderer::UniformBuffer< Configuration > const & m_blurUbo;
		};

		renderer::TextureView const & m_source;
		renderer::Extent2D m_size;
		renderer::Format m_format;
		TextureUnit m_intermediate;

		std::vector< float > m_kernel;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::UniformBufferPtr< Configuration > m_blurUbo;
		renderer::RenderPassPtr m_blurXPass;
		renderer::FrameBufferPtr m_blurXFbo;
		RenderQuad m_blurXQuad;
		renderer::RenderPassPtr m_blurYPass;
		renderer::FrameBufferPtr m_blurYFbo;
		RenderQuad m_blurYQuad;


	};
}

#endif
