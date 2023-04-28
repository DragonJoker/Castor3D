/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryInjectionPass_HPP___
#define ___C3D_GeometryInjectionPass_HPP___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderAST/Shader.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>
#include <RenderGraph/RunnablePasses/PipelineHolder.hpp>

namespace castor3d
{
	class GeometryInjectionPass
		: public castor::Named
		, public crg::RenderPass
	{
	public:
		enum Idx : uint32_t
		{
			LightsIdx,
			RsmNormalsIdx,
			RsmPositionIdx,
			LpvGridUboIdx,
			LpvLightUboIdx,
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	lightType	The light source type.
		 *\param[in]	gridSize	The grid dimensions.
		 *\param[in]	rsmSize		The reflective shadow map dimensions.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	gridSize	Les dimensions de la grille.
		 *\param[in]	rsmSize		Les dimensions de la reflective shadow map.
		 */
		C3D_API GeometryInjectionPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, LightType lightType
			, uint32_t gridSize
			, uint32_t rsmSize );
		/**
		 *\~english
		 *\brief		Constructor for point lights.
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	face		The cube shadow map face.
		 *\param[in]	gridSize	The grid dimensions.
		 *\param[in]	rsmSize		The reflective shadow map dimensions.
		 *\~french
		 *\brief		Constructeur pour les sources omnidirectionnelles.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	face		La face de la cube shadow map.
		 *\param[in]	gridSize	Les dimensions de la grille.
		 *\param[in]	rsmSize		Les dimensions de la reflective shadow map.
		 */
		C3D_API GeometryInjectionPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, CubeMapFace face
			, uint32_t gridSize
			, uint32_t rsmSize );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\brief		Creates the geometry injection pass result.
		 *\param[in]	resources	The render graph resources handler.
		 *\param[in]	device		The GPU device.
		 *\param[in]	prefix		The pass name's prefix.
		 *\param[in]	index		The layer index.
		 *\param[in]	gridSize	The grid dimensions.
		 *\~french
		 *\brief		Crée le résultat de la passe d'injection de géométries.
		 *\param[in]	resources	Le gestionnaire de ressources du render graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	prefix		Le préfixe du nom de la passe.
		 *\param[in]	index		L'indice de la layer.
		 *\param[in]	gridSize	Les dimensions de la grille.
		 */
		static TexturePtr createResult( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::String const & prefix
			, uint32_t index
			, uint32_t gridSize );

	private:
		void doSubInitialise( uint32_t index );
		void doSubRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		class PipelineHolder
		{
		public:
			PipelineHolder( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, crg::pp::Config config
				, uint32_t lpvSize );

			void initialise( VkRenderPass renderPass
				, uint32_t index );
			void recordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index );

		protected:
			void doCreatePipeline( uint32_t index );

		private:
			crg::PipelineHolder m_holder;
			uint32_t m_lpvSize;
			VkRenderPass m_renderPass;
		};

	private:
		RenderDevice const & m_device;
		uint32_t m_rsmSize;
		GpuBufferOffsetT< NonTexturedQuad::Vertex > m_vertexBuffer;
		ShaderModule m_vertexShader;
		ShaderModule m_geometryShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		PipelineHolder m_holder;
	};
}

#endif
