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
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device				Le render device.
		 *\param[in]	category			La catégorie de la passe.
		 *\param[in]	name				Le nom de la passe.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API ForwardRenderTechniquePass( RenderDevice const & device
			, castor::String const & category
			, castor::String const & name
			, SceneRenderPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniqueDesc );
		/**
		 *\~english
		 *\brief		Initialises the render pass.
		 *\param[in]	device		The GPU device.
		 *\param[in]	colourView	The target colour view.
		 *\param[in]	depthView	The target depth view.
		 *\param[in]	size		The pass dimensions.
		 *\param[in]	clear		Tells if the attaches must be cleared at the beginning of the pass.
		 *\~french
		 *\brief		Initialise la passe de rendu.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	colourView	La vue couleur cible.
		 *\param[in]	depthView	La vue profondeur cible.
		 *\param[in]	size		Les dimensions de la passe.
		 *\param[in]	clear		Dit si les attaches doivent être vidées au début de la passe.
		 */
		C3D_API void initialiseRenderPass( ashes::ImageView const & colourView
			, ashes::ImageView const & depthView
			, castor::Size const & size
			, bool clear );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[in]	device	The GPU device.
		 *\param[out]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	device	Le device GPU.
		 *\param[out]	toWait	Le sémaphore à attendre.
		 */
		C3D_API ashes::Semaphore const & render( ashes::Semaphore const & toWait );

	private:
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;

	protected:
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::CommandBufferPtr m_nodesCommands;
	};
}

#endif
