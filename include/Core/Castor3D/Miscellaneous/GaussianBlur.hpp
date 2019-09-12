/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GaussianBlur_H___
#define ___C3D_GaussianBlur_H___

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/RenderToTexture/RenderQuad.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

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
			, ashes::ImageView const & texture
			, VkExtent2D const & textureSize
			, VkFormat format
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
			CU_Require( m_renderPass );
			return *m_renderPass;
		}

		inline ashes::FrameBuffer const & getBlurXFrameBuffer()const
		{
			CU_Require( m_blurX.fbo );
			return *m_blurX.fbo;
		}

		inline ashes::FrameBuffer const & getBlurYFrameBuffer()const
		{
			CU_Require( m_blurY.fbo );
			return *m_blurY.fbo;
		}

		inline ashes::CommandBuffer const & getBlurXCommandBuffer()const
		{
			return m_blurX.quad.getCommandBuffer();
		}

		inline ashes::CommandBuffer const & getBlurYCommandBuffer()const
		{
			return m_blurY.quad.getCommandBuffer();
		}

		inline ShaderModule const & getBlurXVertexModule()const
		{
			return m_blurX.vertexShader;
		}

		inline ShaderModule const & getBlurXPixelModule()const
		{
			return m_blurX.pixelShader;
		}

		inline ShaderModule const & getBlurYVertexModule()const
		{
			return m_blurY.vertexShader;
		}

		inline ShaderModule const & getBlurYPixelModule()const
		{
			return m_blurY.pixelShader;
		}

		inline sdw::Shader const & getBlurXVertexShader()const
		{
			CU_Require( m_blurX.vertexShader.shader );
			return *m_blurX.vertexShader.shader;
		}

		inline sdw::Shader const & getBlurXPixelShader()const
		{
			CU_Require( m_blurX.pixelShader.shader );
			return *m_blurX.pixelShader.shader;
		}

		inline sdw::Shader const & getBlurYVertexShader()const
		{
			CU_Require( m_blurY.vertexShader.shader );
			return *m_blurY.vertexShader.shader;
		}

		inline sdw::Shader const & getBlurYPixelShader()const
		{
			CU_Require( m_blurY.pixelShader.shader );
			return *m_blurY.pixelShader.shader;
		}
		/**@}*/

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

		class RenderQuad
			: public castor3d::RenderQuad
		{
		public:
			RenderQuad( RenderSystem & renderSystem
				, ashes::ImageView const & src
				, ashes::ImageView const & dst
				, UniformBuffer< Configuration > const & blurUbo
				, VkFormat format
				, VkExtent2D const & size );

		private:
			virtual void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
				, ashes::DescriptorSet & descriptorSet );

			ashes::ImageView const & m_srcView;
			ashes::ImageView const & m_dstView;
			UniformBuffer< Configuration > const & m_blurUbo;
		};

		struct BlurPass
		{
			RenderQuad quad;
			ShaderModule vertexShader;
			ShaderModule pixelShader;
			ashes::SemaphorePtr semaphore;
			ashes::FrameBufferPtr fbo;
			ashes::CommandBufferPtr commandBuffer;

			BlurPass( Engine & engine
				, ashes::ImageView const & input
				, ashes::ImageView const & output
				, UniformBuffer< GaussianBlur::Configuration > const & blurUbo
				, VkFormat format
				, VkExtent2D const & textureSize
				, ashes::RenderPass const & renderPass
				, bool isHorizontal );

		private:
			ashes::CommandBufferPtr doGenerateCommandBuffer( RenderDevice const & device
				, ashes::RenderPass const & renderPass );
		};

	private:
		ashes::ImageView const & m_source;
		VkExtent2D m_size;
		VkFormat m_format;
		TextureUnit m_intermediate;
		ashes::RenderPassPtr m_renderPass;
		UniformBufferUPtr< Configuration > m_blurUbo;
		std::vector< float > m_kernel;
		BlurPass m_blurX;
		BlurPass m_blurY;
	};
}

#endif
