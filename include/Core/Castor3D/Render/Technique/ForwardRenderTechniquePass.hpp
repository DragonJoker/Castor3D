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
		 *\param[in]	parent				The parent technique.
		 *\param[in]	pass				The parent frame pass.
		 *\param[in]	context				The rendering context.
		 *\param[in]	graph				The runnable graph.
		 *\param[in]	device				The render device.
		 *\param[in]	typeName			The pass type name.
		 *\param[in]	category			The pass category.
		 *\param[in]	name				The pass name.
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent				La technique parente.
		 *\param[in]	pass				La frame pass parente.
		 *\param[in]	context				Le contexte de rendu.
		 *\param[in]	graph				Le runnable graph.
		 *\param[in]	device				Le render device.
		 *\param[in]	typeName			La nom du type de la passe.
		 *\param[in]	category			La catégorie de la passe.
		 *\param[in]	name				Le nom de la passe.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API ForwardRenderTechniquePass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & typeName
			, castor::String const & category
			, castor::String const & name
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;

	public:
		C3D_API static castor::String const Type;

	private:
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;
	};
}

#endif
