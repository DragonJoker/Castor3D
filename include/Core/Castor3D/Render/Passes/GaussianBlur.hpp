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
		C3D_API GaussianBlur( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, castor::String const & category
			, castor::String const & prefix
			, crg::ImageViewIdArray const & views
			, crg::ImageViewId const & intermediateView
			, uint32_t kernelSize );
		C3D_API GaussianBlur( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, castor::String const & category
			, castor::String const & prefix
			, crg::ImageViewIdArray const & views
			, uint32_t kernelSize );
		C3D_API GaussianBlur( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, castor::String const & category
			, castor::String const & prefix
			, crg::ImageViewId const & view
			, uint32_t kernelSize );
		C3D_API GaussianBlur( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, castor::String const & category
			, castor::String const & prefix
			, crg::ImageViewId const & view
			, crg::ImageViewId const & intermediateView
			, uint32_t kernelSize );
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
		crg::FramePass const & getLastPass()const
		{
			CU_Require( m_lastPass );
			return *m_lastPass;
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

	private:
		crg::ImageViewIdArray m_sources;
		RenderDevice const & m_device;
		crg::FramePass const * m_lastPass;
		castor::String m_prefix;
		VkExtent2D m_size;
		VkFormat m_format;
		crg::ImageViewId m_intermediateView;
		UniformBufferOffsetT< Configuration > m_blurUbo;
		std::vector< float > m_kernel;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShaderX;
		ShaderModule m_pixelShaderY;
		ashes::PipelineShaderStageCreateInfoArray m_stagesX;
		ashes::PipelineShaderStageCreateInfoArray m_stagesY;
	};
}

#endif
