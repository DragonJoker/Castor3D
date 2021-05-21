/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ForwardRenderTechniquePass_H___
#define ___C3D_ForwardRenderTechniquePass_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

namespace castor3d
{
	class ForwardRenderTechniquePass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device				The render device.
		 *\param[in]	category			The pass category.
		 *\param[in]	name				The pass name.
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\param[in]	colourView			The target colour view.
		 *\param[in]	depthView			The target depth view.
		 *\param[in]	clear				Tells if the attaches must be cleared at the beginning of the pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device				Le render device.
		 *\param[in]	category			La catégorie de la passe.
		 *\param[in]	name				Le nom de la passe.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 *\param[in]	colourView			La vue couleur cible.
		 *\param[in]	depthView			La vue profondeur cible.
		 *\param[in]	clear				Dit si les attaches doivent être vidées au début de la passe.
		 */
		C3D_API ForwardRenderTechniquePass( crg::FramePass const & pass
			, crg::GraphContext const & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, SceneRenderPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;

	private:
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;
	};
}

#endif
