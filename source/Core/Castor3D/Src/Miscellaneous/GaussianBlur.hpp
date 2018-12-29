/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GaussianBlur_H___
#define ___C3D_GaussianBlur_H___

#include "RenderToTexture/RenderQuad.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>

#include <Design/OwnedBy.hpp>
#include <ShaderWriter/Shader.hpp>

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
			, ashes::TextureView const & texture
			, ashes::Extent2D const & textureSize
			, ashes::Format format
			, uint32_t kernelSize );
		/**
		 *\~english
		 *\brief		Blurs given texture.
		 *\~french
		 *\brief		Applique le flou sur la texture.
		 */
		C3D_API ashes::Semaphore const & blur( ashes::Semaphore const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		inline ashes::RenderPass const & getRenderPass()const
		{
			REQUIRE( m_renderPass );
			return *m_renderPass;
		}

		inline ashes::FrameBuffer const & getBlurXFrameBuffer()const
		{
			REQUIRE( m_blurXFbo );
			return *m_blurXFbo;
		}

		inline ashes::FrameBuffer const & getBlurYFrameBuffer()const
		{
			REQUIRE( m_blurYFbo );
			return *m_blurYFbo;
		}

		inline ashes::CommandBuffer const & getBlurXCommandBuffer()const
		{
			return m_blurXQuad.getCommandBuffer();
		}

		inline ashes::CommandBuffer const & getBlurYCommandBuffer()const
		{
			return m_blurYQuad.getCommandBuffer();
		}

		inline sdw::Shader const & getBlurXVertexShader()const
		{
			REQUIRE( m_blurXVertexShader.shader );
			return *m_blurXVertexShader.shader;
		}

		inline sdw::Shader const & getBlurXPixelShader()const
		{
			REQUIRE( m_blurXPixelShader.shader );
			return *m_blurXPixelShader.shader;
		}

		inline sdw::Shader const & getBlurYVertexShader()const
		{
			REQUIRE( m_blurYVertexShader.shader );
			return *m_blurYVertexShader.shader;
		}

		inline sdw::Shader const & getBlurYPixelShader()const
		{
			REQUIRE( m_blurYPixelShader.shader );
			return *m_blurYPixelShader.shader;
		}
		/**@}*/

	private:
		bool doInitialiseBlurXProgram();
		bool doInitialiseBlurYProgram();

	public:
		C3D_API static castor::String const Config;
		C3D_API static castor::String const Coefficients;
		C3D_API static castor::String const CoefficientsCount;
		C3D_API static castor::String const TextureSize;
		C3D_API static constexpr uint32_t MaxCoefficients{ 60u };

		struct Configuration
		{
			castor::Point2f textureSize;
			uint32_t blurCoeffsCount;
			uint32_t dump; // to keep a 16 byte alignment.
			std::array< castor::Point4f, GaussianBlur::MaxCoefficients / 4u > blurCoeffs; // We then allow for 60 coeffs max, to have a 256 bytes struct.
		};

	private:
		class RenderQuad
			: public castor3d::RenderQuad
		{
		public:
			RenderQuad( RenderSystem & renderSystem
				, ashes::TextureView const & src
				, ashes::TextureView const & dst
				, ashes::UniformBuffer< Configuration > const & blurUbo
				, ashes::Format format
				, ashes::Extent2D const & size );

		private:
			virtual void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
				, ashes::DescriptorSet & descriptorSet );

			ashes::TextureView const & m_srcView;
			ashes::TextureView const & m_dstView;
			ashes::UniformBuffer< Configuration > const & m_blurUbo;
		};

		ashes::TextureView const & m_source;
		ashes::Extent2D m_size;
		ashes::Format m_format;
		TextureUnit m_intermediate;

		std::vector< float > m_kernel;
		ashes::CommandBufferPtr m_horizCommandBuffer;
		ashes::CommandBufferPtr m_verticCommandBuffer;
		ashes::SemaphorePtr m_horizSemaphore;
		ashes::SemaphorePtr m_verticSemaphore;
		ashes::UniformBufferPtr< Configuration > m_blurUbo;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_blurXFbo;
		ShaderModule m_blurXVertexShader;
		ShaderModule m_blurXPixelShader;
		RenderQuad m_blurXQuad;
		ashes::RenderPassPtr m_blurYPass;
		ashes::FrameBufferPtr m_blurYFbo;
		ShaderModule m_blurYVertexShader;
		ShaderModule m_blurYPixelShader;
		RenderQuad m_blurYQuad;


	};
}

#endif
