/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RadianceComputer_H___
#define ___C3D_RadianceComputer_H___

#include "Texture/TextureUnit.hpp"

#include <array>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Computes the radiance map from an environment map.
	\~french
	\brief		Calcule la texture de radiance pour une texture d'environnement donnée.
	*/
	class RadianceComputer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render size.
		 *\param[in]	srcTexture	The cube texture source.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		La taille du rendu.
		 *\param[in]	srcTexture	La texture cube source.
		 */
		C3D_API explicit RadianceComputer( Engine & engine
			, castor::Size const & size
			, renderer::Texture const & srcTexture );
		/**
		 *\~english
		 *\brief		Computes the radiance map.
		 *\~french
		 *\brief		Calcule la texture de radiance.
		 */
		C3D_API void render();
		/**
		 *\~english
		 *\return		The radiance map.
		 *\~french
		 *\return		La texture de radiance.
		 */
		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		/**
		 *\~english
		 *\brief		Creates the radiance computer shader program.
		 *\~french
		 *\brief		Crée le programme shader de calcul de la radiance.
		 */
		renderer::ShaderProgram & doCreateProgram();

	private:
		struct RenderPass
		{
			renderer::TextureViewPtr srcView;
			renderer::TextureViewPtr dstView;
			renderer::DescriptorSetPtr descriptorSet;
			renderer::RenderPassPtr renderPass;
			renderer::FrameBufferPtr frameBuffer;
			renderer::PipelinePtr pipeline;
		};

		using RenderPasses = std::array< RenderPass, 6 >;
		RenderSystem & m_renderSystem;
		TextureUnit m_result;
		NonTexturedCube m_vertexData;
		SamplerSPtr m_sampler;
		renderer::VertexBufferPtr< NonTexturedCube > m_vertexBuffer;
		renderer::GeometryBuffersPtr m_geometryBuffers;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::UniformBufferPtr< renderer::Mat4 > m_configUbo;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		RenderPasses m_renderPasses;
		renderer::CommandBufferPtr m_commandBuffer;
	};
}

#endif
