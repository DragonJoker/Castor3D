/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScatteringPass_H___
#define ___C3D_SubsurfaceScatteringPass_H___

#include "Render/Viewport.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Texture/TextureUnit.hpp"
#include "Technique/Opaque/LightPass.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		26/07/2017
	\~english
	\brief		Gaussian blur pass.
	\~french
	\brief		Passe flou gaussien.
	*/
	class SubsurfaceScatteringPass
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	gpInfoUbo		The geometry pass UBO.
		 *\param[in]	sceneUbo		The scene UBO.
		 *\param[in]	textureSize		The render area dimensions.
		 *\param[in]	gp				The geometry pass result.
		 *\param[in]	lightDiffuse	The light pass diffuse result.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	gpInfoUbo		L'UBO de la passe géométrique.
		 *\param[in]	sceneUbo		L'UBO de la scène.
		 *\param[in]	textureSize		Les dimensions de la zone de rendu.
		 *\param[in]	gp				Le résultat de la geometry pass.
		 *\param[in]	lightDiffuse	Le résultat diffus de la light pass.
		 */
		C3D_API SubsurfaceScatteringPass( Engine & engine
			, GpInfoUbo & gpInfoUbo
			, SceneUbo & sceneUbo
			, castor::Size const & textureSize
			, GeometryPassResult const & gp
			, TextureUnit const & lightDiffuse );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SubsurfaceScatteringPass();
		/**
		*\~english
		*\brief		Applies Subsurface scattering.
		*\~french
		*\brief		Applique le Subsurface scattering.
		*/
		C3D_API void prepare( renderer::CommandBuffer const & commandBuffer )const;
		/**
		 *\~english
		 *\brief		Applies Subsurface scattering.
		 *\~french
		 *\brief		Applique le Subsurface scattering.
		 */
		C3D_API void render()const;
		/**
		 *\~english
		 *\brief		Dumps the results on the screen.
		 *\param[in]	size	The dump dimensions.
		 *\~french
		 *\brief		Dumpe les résultats sur l'écran.
		 *\param[in]	size	Les dimensions d'affichage.
		 */
		C3D_API void debugDisplay( castor::Size const & size )const;

		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		void doBlur( GeometryPassResult const & gp
			, TextureUnit const & source
			, TextureUnit const & destination
			//, TextureAttachmentSPtr attach
			, castor::Point2r const & direction )const;
		void doCombine( GeometryPassResult const & gp
			, TextureUnit const & source )const;

	public:
		static castor::String const Config;
		static castor::String const Step;
		static castor::String const Correction;
		static castor::String const PixelSize;

		struct Configuration
		{
			renderer::Vec2 blurPixelSize;
			float blurCorrection;
		};

	private:
		class Blur
			: private RenderQuad
		{
		public:
			Blur( RenderSystem & renderSystem
				, castor::Size const & size
				, GpInfoUbo & gpInfoUbo
				, SceneUbo & sceneUbo
				, renderer::UniformBuffer< Configuration > const & blurUbo
				, GeometryPassResult const & gp
				, TextureUnit const & source
				, TextureUnit const & destination
				, bool isVertic );
			void prepareFrame( renderer::CommandBuffer const & commandBuffer )const;

		private:
			void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
				, renderer::DescriptorSet & descriptorSet )override;

		private:
			RenderSystem & m_renderSystem;
			GeometryPassResult const & m_geometryBufferResult;
			GpInfoUbo & m_gpInfoUbo;
			SceneUbo & m_sceneUbo;
			renderer::UniformBuffer< Configuration > const & m_blurUbo;
			renderer::RenderPassPtr m_renderPass;
			renderer::FrameBufferPtr m_frameBuffer;
		};

		class Combine
			: private RenderQuad
		{
		public:
			explicit Combine( RenderSystem & renderSystem
				, castor::Size const & size
				, GeometryPassResult const & gp
				, TextureUnit const & source
				, std::array< TextureUnit, 3u > const & blurResults
				, TextureUnit const & destination );
			void prepareFrame( renderer::CommandBuffer const & commandBuffer )const;

		private:
			void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
				, renderer::DescriptorSet & descriptorSet )override;

		private:
			RenderSystem & m_renderSystem;
			GeometryPassResult const & m_geometryBufferResult;
			TextureUnit const & m_source;
			std::array< TextureUnit, 3u > const & m_blurResults;
			renderer::RenderPassPtr m_renderPass;
			renderer::FrameBufferPtr m_frameBuffer;
		};

	private:
		castor::Size m_size;
		renderer::UniformBufferPtr< Configuration > m_blurUbo;
		TextureUnit m_intermediate;
		std::array< TextureUnit, 3u > m_blurResults;
		TextureUnit m_result;
		Blur m_blurX[3];
		Blur m_blurY[3];
		Combine m_combine;
	};
}

#endif
