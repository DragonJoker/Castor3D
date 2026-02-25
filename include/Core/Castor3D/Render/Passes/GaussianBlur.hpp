/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GaussianBlur_H___
#define ___C3D_GaussianBlur_H___

#include "PassesModule.hpp"

#include "Castor3D/Shader/Ubos/Ubo.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <RenderGraph/RunnablePass.hpp>

#include <ShaderAST/Shader.hpp>

namespace c3d
{
	using TextureViewCRef = ReferenceWrapper< TextureView const >;
	using TextureViewCRefArray = Vector< TextureViewCRef >;

	class GaussianBlur
		: public OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph				The render graph.
		 *\param[in]	device				The GPU device.
		 *\param[in]	prefix				The pass name's prefix.
		 *\param[in]	attach				The source and destination texture.
		 *\param[in]	intermediateView	The view receiving the intermediate blur.
		 *\param[in]	kernelSize			The kernel coefficients count.
		 *\param[in]	isEnabled			The enabled status retrieval callback.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph				Le render graph.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	prefix				Le préfixe du nom de la passe.
		 *\param[in]	attach				La texture source et destination.
		 *\param[in]	intermediateView	La texture recevant le blur intermédiaire.
		 *\param[in]	kernelSize			Le nombre de coefficients du kernel.
		 *\param[in]	isEnabled			Le callback de récupération du statut d'activation.
		 */
		C3D_API GaussianBlur( crg::FramePassGroup & graph
			, RenderDevice const & device
			, String const & prefix
			, crg::Attachment const & attach
			, crg::ImageViewId const & intermediateView
			, uint32_t kernelSize
			, crg::RunnablePass::IsEnabledCallback const & isEnabled = crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph			The render graph.
		 *\param[in]	device			The GPU device.
		 *\param[in]	prefix			The pass name's prefix.
		 *\param[in]	attach			The source and destination texture.
		 *\param[in]	kernelSize		The kernel coefficients count.
		 *\param[in]	isEnabled		The enabled status retrieval callback.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph			Le render graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	prefix			Le préfixe du nom de la passe.
		 *\param[in]	attach			La texture source et destination.
		 *\param[in]	kernelSize		Le nombre de coefficients du kernel.
		 *\param[in]	isEnabled		Le callback de récupération du statut d'activation.
		 */
		C3D_API GaussianBlur( crg::FramePassGroup & graph
			, RenderDevice const & device
			, String const & prefix
			, crg::Attachment const & attach
			, uint32_t kernelSize
			, crg::RunnablePass::IsEnabledCallback const & isEnabled = crg::RunnablePass::IsEnabledCallback( []() { return true; } ) );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph			The render graph.
		 *\param[in]	device			The GPU device.
		 *\param[in]	prefix			The pass name's prefix.
		 *\param[in]	texture			The source and destination texture.
		 *\param[in]	kernelSize		The kernel coefficients count.
		 *\param[in]	isEnabled		The enabled status retrieval callback.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph			Le render graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	prefix			Le préfixe du nom de la passe.
		 *\param[in]	texture			La texture source et destination.
		 *\param[in]	kernelSize		Le nombre de coefficients du kernel.
		 *\param[in]	isEnabled		Le callback de récupération du statut d'activation.
		 */
		C3D_API GaussianBlur( crg::FramePassGroup & graph
			, RenderDevice const & device
			, String const & prefix
			, c3d::Texture & texture
			, uint32_t kernelSize
			, crg::RunnablePass::IsEnabledCallback const & isEnabled = crg::RunnablePass::IsEnabledCallback( []() { return true; } ) );
		/**
		 *\copydoc		RenderTechniquePass::accept
		 */
		C3D_API void accept( ConfigurationVisitorBase & visitor )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		crg::Attachment const & getResultAttach()const noexcept
		{
			CU_Require( m_lastAttach );
			return *m_lastAttach;
		}
		/**@}*/

	public:
		C3D_API static MbString const Config;
		C3D_API static MbString const Coefficients;
		C3D_API static MbString const CoefficientsCount;
		C3D_API static MbString const TextureSize;
		C3D_API static constexpr uint32_t MaxCoefficients{ 60u };

		struct Configuration
		{
			Point2f textureSize;
			uint32_t blurCoeffsCount;
			uint32_t dump; // to keep a 16 byte alignment.
			Array< Point4f, GaussianBlur::MaxCoefficients / 4u > blurCoeffs; // We then allow for 60 coeffs max, to have a 256 bytes struct.
		};

	private:
		crg::Attachment const & m_source;
		RenderDevice const & m_device;
		crg::Attachment const * m_lastAttach;
		String m_prefix;
		Extent2D m_size;
		PixelFormat m_format;
		crg::ImageViewId m_intermediateView;
		UboT< Configuration > m_blurUbo;
		Vector< float > m_kernel;
		ProgramModule m_shaderX;
		ProgramModule m_shaderY;
		ashes::PipelineShaderStageCreateInfoArray m_stagesX;
		ashes::PipelineShaderStageCreateInfoArray m_stagesY;
	};
}

#endif
