/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredTransparentPass_H___
#define ___C3D_DeferredTransparentPass_H___

#include "ShadowMap/ShadowMapDirectional.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "ShadowMap/ShadowMapSpot.hpp"
#include "Technique/RenderTechniquePass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Enumerator of textures used in weighted blend OIT.
	\~french
	\brief		Enumération des textures utilisées lors du weighted blend OIT.
	*/
	enum class WbTexture
		: uint8_t
	{
		eDepth,
		eAccumulation,
		eRevealage,
		eVelocity,
		CASTOR_SCOPED_ENUM_BOUNDS( eDepth ),
	};
	//!\~english	The weighted blend pass result.
	//!\~french		Le résultat de la passe de weighted blend.
	using WeightedBlendTextures = std::array< ashes::TextureViewCRef, size_t( WbTexture::eCount ) >;
	/**
	 *\~english
	 *\brief		Retrieve the name for given texture enum value.
	 *\param[in]	texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le nom pour la valeur d'énumeration de texture.
	 *\param[in]	texture	La valeur.
	 *\return		Le nom.
	 */
	castor::String getTextureName( WbTexture texture );
	/**
	 *\~english
	 *\brief		Retrieve the pixel format for given texture enum value.
	 *\param[in]	texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le format de pixels pour la valeur d'énumeration de texture.
	 *\param[in]	texture	La valeur.
	 *\return		Le nom.
	 */
	ashes::Format getTextureFormat( WbTexture texture );
	/**
	 *\~english
	 *\brief		Retrieve the attachment point for given texture enum value.
	 *\param[in]	texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le point d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	texture	La valeur.
	 *\return		Le nom.
	 */
	ashes::ImageAspectFlags getTextureAttachmentPoint( WbTexture texture );
	/**
	 *\~english
	 *\brief		Retrieve the attachment index for given texture enum value.
	 *\param[in]	texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	texture	La valeur.
	 *\return		Le nom.
	 */
	uint32_t getTextureAttachmentIndex( WbTexture texture );

	class RenderTechnique;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Transparent nodes pass using Weighted Blend OIT.
	\~french
	\brief		Passe pour les noeuds transparents, utilisant le Weighted Blend OIT.
	*/
	class TransparentPass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	scene	The scene for this technique.
		 *\param[in]	camera	The camera for this technique (may be null).
		 *\param[in]	config	The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	scene	La scène pour cette technique.
		 *\param[in]	camera	La caméra pour cette technique (peut être nulle).
		 *\param[in]	config	La configuration du SSAO.
		 */
		TransparentPass( MatrixUbo const & matrixUbo
			, SceneCuller & culler
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TransparentPass();
		/**
		 *\~english
		 *\brief		Initialises the render pass.
		 *\~french
		 *\brief		Initialise la passe de rendu.
		 */
		void initialiseRenderPass( WeightedBlendTextures const & wbpResult );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		void update( RenderInfo & info
			, castor::Point2r const & jitter )override;
		/**
		 *\~english
		 *\brief		Renders transparent nodes.
		 *\~french
		 *\brief		Dessine les noeuds transparents.
		 */
		ashes::Semaphore const & render( ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline void setDepthFormat( ashes::Format value )
		{
			m_depthFormat = value;
		}
		/**@}*/

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		bool doInitialise( castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		ashes::DepthStencilState doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		ashes::ColourBlendState doCreateBlendState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateTextureBindings
		 */
		ashes::DescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		glsl::Shader doGetVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetLegacyPixelShaderSource
		 */
		glsl::Shader doGetLegacyPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		glsl::Shader doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		glsl::Shader doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & pipeline )const override;

	private:
		ashes::Format m_depthFormat;
		ashes::CommandBufferPtr m_nodesCommands;
		ashes::FrameBufferPtr m_frameBuffer;
	};
}

#endif
