/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredSsaoPass_H___
#define ___C3D_DeferredSsaoPass_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Passes/PassesModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

#include <RenderGraph/FrameGraphPrerequisites.hpp>

#define C3D_DebugSSAO 0

namespace castor3d
{
	class SsaoPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	linearisedDepth	The linearised depth buffer.
		 *\param[in]	gpResult		The geometry pass result.
		 *\param[in]	gpInfoUbo		The GBuffer configuration UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	linearisedDepth	Le depth buffer linéarisé.
		 *\param[in]	gpResult		Le résultat de la geometry pass.
		 *\param[in]	gpInfoUbo		L'UBO de configuration du GBuffer
		 */
		C3D_API SsaoPass( crg::FrameGraph & graph
			, RenderDevice const & device
			, crg::FramePass const & previousPass
			, castor::Size const & size
			, SsaoConfig & ssaoConfig
			, Texture const & depth
			, Texture const & normal
			, GpInfoUbo const & gpInfoUbo );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\return		The SSAO pass result.
		 *\~french
		 *\return		Le résultat de la passe SSAO.
		 */
		C3D_API Texture const & getResult()const;
		/**
		 *\~english
		 *\return		The bent normals.
		 *\~french
		 *\return		Les bent normals.
		 */
		C3D_API Texture const & getBentNormals()const;

		SsaoConfig const & getConfig()const
		{
			return m_ssaoConfig;
		}

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		RenderDevice const & m_device;
		SsaoConfig & m_ssaoConfig;
		Texture const & m_depth;
		Texture const & m_normal;
		GpInfoUbo const & m_gpInfoUbo;
		VkExtent2D m_size;
		MatrixUbo m_matrixUbo;
		LineariseDepthPassUPtr m_linearisePass;
		SsaoConfigUbo m_ssaoConfigUbo;
		SsaoRawAOPassUPtr m_rawAoPass;
		SsaoBlurPassUPtr m_horizontalBlur;
		SsaoBlurPassUPtr m_verticalBlur;
		crg::FramePass const * m_lastPass{};
	};
}

#endif
