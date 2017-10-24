﻿/*
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
		CASTOR_SCOPED_ENUM_BOUNDS( eDepth ),
	};
	//!\~english	The weighted blend pass result.
	//!\~french		Le résultat de la passe de weighted blend.
	using WeightedBlendPassResult = std::array< TextureUnitUPtr, size_t( WbTexture::eCount ) >;
	/**
	 *\~english
	 *\brief		Retrieve the name for given texture enum value.
	 *\param[in]	p_texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le nom pour la valeur d'énumeration de texture.
	 *\param[in]	p_texture	La valeur.
	 *\return		Le nom.
	 */
	castor::String getTextureName( WbTexture p_texture );
	/**
	 *\~english
	 *\brief		Retrieve the pixel format for given texture enum value.
	 *\param[in]	p_texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le format de pixels pour la valeur d'énumeration de texture.
	 *\param[in]	p_texture	La valeur.
	 *\return		Le nom.
	 */
	castor::PixelFormat getTextureFormat( WbTexture p_texture );
	/**
	 *\~english
	 *\brief		Retrieve the attachment point for given texture enum value.
	 *\param[in]	p_texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le point d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	p_texture	La valeur.
	 *\return		Le nom.
	 */
	AttachmentPoint getTextureAttachmentPoint( WbTexture p_texture );
	/**
	 *\~english
	 *\brief		Retrieve the attachment index for given texture enum value.
	 *\param[in]	p_texture	The value.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère l'indice d'attache pour la valeur d'énumeration de texture.
	 *\param[in]	p_texture	La valeur.
	 *\return		Le nom.
	 */
	uint32_t getTextureAttachmentIndex( WbTexture p_texture );

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
		 *\param[in]	p_scene		The scene for this technique.
		 *\param[in]	p_camera	The camera for this technique (may be null).
		 *\param[in]	p_config	The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_scene		La scène pour cette technique.
		 *\param[in]	p_camera	La caméra pour cette technique (peut être nulle).
		 *\param[in]	p_config	La configuration du SSAO.
		 */
		TransparentPass( Scene & p_scene
			, Camera * p_camera
			, SsaoConfig const & p_config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TransparentPass();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		void render( RenderInfo & p_info
			, ShadowMapLightTypeArray & shadowMaps
			, castor::Point2r const & jitter )override;

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareFrontPipeline
		 */
		void doPrepareFrontPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareBackPipeline
		 */
		void doPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
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
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		glsl::Shader doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		glsl::Shader doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & p_pipeline )const override;
		void doCompletePipeline( PipelineFlags const & p_flags
			, RenderPipeline & p_pipeline );
	};
}

#endif
