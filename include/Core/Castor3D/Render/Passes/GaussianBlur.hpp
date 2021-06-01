/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GaussianBlur_H___
#define ___C3D_GaussianBlur_H___

#include "PassesModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	using TextureViewCRef = std::reference_wrapper< TextureView const >;
	using TextureViewCRefArray = std::vector< TextureViewCRef >;

	class GaussianBlur
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	device		The GPU device.
		 *\param[in]	prefix		The pass name's prefix.
		 *\param[in]	view		The texture.
		 *\param[in]	kernelSize	The kernel coefficients count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	prefix		Le préfixe du nom de la passe.
		 *\param[in]	view		La texture.
		 *\param[in]	kernelSize	Le nombre de coefficients du kernel.
		 */
		C3D_API GaussianBlur( crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, castor::String const & prefix
			, crg::ImageViewId const & view
			, uint32_t kernelSize );
		C3D_API ~GaussianBlur();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		ashes::RenderPass const & getRenderPass()const
		{
			CU_Require( m_renderPass );
			return *m_renderPass;
		}

		ShaderModule const & getBlurXVertexModule()const
		{
			return m_blurX.vertexShader;
		}

		ShaderModule const & getBlurXPixelModule()const
		{
			return m_blurX.pixelShader;
		}

		ShaderModule const & getBlurYVertexModule()const
		{
			return m_blurY.vertexShader;
		}

		ShaderModule const & getBlurYPixelModule()const
		{
			return m_blurY.pixelShader;
		}

		ast::Shader const & getBlurXVertexShader()const
		{
			CU_Require( m_blurX.vertexShader.shader );
			return *m_blurX.vertexShader.shader;
		}

		ast::Shader const & getBlurXPixelShader()const
		{
			CU_Require( m_blurX.pixelShader.shader );
			return *m_blurX.pixelShader.shader;
		}

		ast::Shader const & getBlurYVertexShader()const
		{
			CU_Require( m_blurY.vertexShader.shader );
			return *m_blurY.vertexShader.shader;
		}

		ast::Shader const & getBlurYPixelShader()const
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

		struct BlurPass
		{
			ShaderModule vertexShader;
			ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray stages;
			bool isHorizontal;

			BlurPass( crg::FrameGraph & graph
				, crg::FramePass const *& previousPass
				, RenderDevice const & device
				, castor::String const & name
				, crg::ImageViewId const & input
				, crg::ImageViewId const & output
				, UniformBufferOffsetT< GaussianBlur::Configuration > const & blurUbo
				, VkFormat format
				, VkExtent2D const & textureSize
				, bool isHorizontal );
		};

	private:
		crg::ImageViewId const & m_source;
		RenderDevice const & m_device;
		crg::FramePass const * m_previousPass;
		castor::String m_prefix;
		VkExtent2D m_size;
		VkFormat m_format;
		crg::ImageId m_intermediate;
		crg::ImageViewId m_intermediateView;
		ashes::RenderPassPtr m_renderPass;
		UniformBufferOffsetT< Configuration > m_blurUbo;
		std::vector< float > m_kernel;
		BlurPass m_blurX;
		BlurPass m_blurY;
	};
}

#endif
