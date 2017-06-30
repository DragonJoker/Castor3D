/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_DeferredTransparentPass_H___
#define ___C3D_DeferredTransparentPass_H___

#include "ShadowMap/ShadowMapDirectional.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "ShadowMap/ShadowMapSpot.hpp"
#include "Technique/RenderTechniquePass.hpp"
#include "Technique/Transparent/DepthPass.hpp"

namespace Castor3D
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
	Castor::String GetTextureName( WbTexture p_texture );
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
	Castor::PixelFormat GetTextureFormat( WbTexture p_texture );
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
	AttachmentPoint GetTextureAttachmentPoint( WbTexture p_texture );
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
	uint32_t GetTextureAttachmentIndex( WbTexture p_texture );

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
		 *\~english
		 *\brief		Render function.
		 *\param[out]	p_info		Receives the render informations.
		 *\param[out]	p_shadows	Tells if the scene has shadow producing light sources.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	p_info		Reçoit les informations de rendu.
		 *\param[out]	p_shadows	Dit si la scène a des lumières produisant des ombres.
		 */
		void Render( RenderInfo & p_info, bool p_shadows )override;
		/**
		 *\copydoc		Castor3D::RenderPass::InitialiseShadowMaps
		 */
		bool InitialiseShadowMaps()override;
		/**
		 *\copydoc		Castor3D::RenderPass::CleanupShadowMaps
		 */
		void CleanupShadowMaps()override;
		/**
		 *\copydoc		Castor3D::RenderPass::UpdateShadowMaps
		 */
		void UpdateShadowMaps( RenderQueueArray & p_queues )override;
		/**
		 *\copydoc		Castor3D::RenderPass::RenderShadowMaps
		 */
		void RenderShadowMaps()override;
		/**
		 *\copydoc		Castor3D::RenderTechniquePass::AddShadowProducer
		 */
		C3D_API void AddShadowProducer( Light & p_light )override;
		/**
		 *\~english
		 *\return		The directional light shadow map.
		 *\~french
		 *\return		La map d'ombre de la lumière directionnelle.
		 */
		inline ShadowMapDirectional & GetDirectionalShadowMap()
		{
			return m_directionalShadowMap;
		}
		/**
		 *\~english
		 *\return		The spot lights shadow map.
		 *\~french
		 *\return		La map d'ombre des lumières projecteur.
		 */
		inline ShadowMapSpot & GetSpotShadowMap()
		{
			return m_spotShadowMap;
		}
		/**
		 *\~english
		 *\return		The point lights shadow map.
		 *\~french
		 *\return		La map d'ombre des lumières ponctuelles.
		 */
		inline ShadowMapPoint & GetPointShadowMaps()
		{
			return m_pointShadowMap;
		}

	private:
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetDepthMaps
		 */
		void DoGetDepthMaps( DepthMapArray & p_depthMaps )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareFrontPipeline
		 */
		void DoPrepareFrontPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareBackPipeline
		 */
		void DoPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetVertexShaderSource
		 */
		GLSL::Shader DoGetVertexShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, bool p_invertNormals )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPixelShaderSource
		 */
		GLSL::Shader DoGetPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, ComparisonFunc p_alphaFunc )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdatePipeline
		 */
		void DoUpdatePipeline( RenderPipeline & p_pipeline )const override;
		void DoCompletePipeline( PipelineFlags const & p_flags
			, RenderPipeline & p_pipeline );

	private:
		//!\~english	The shadow map used for directional lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type directionnelles.
		ShadowMapDirectional m_directionalShadowMap;
		//!\~english	The shadow map used for spot lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type spot.
		ShadowMapSpot m_spotShadowMap;
		//!\~english	The shadow map used for pont lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type point.
		ShadowMapPoint m_pointShadowMap;
	};
}

#endif
