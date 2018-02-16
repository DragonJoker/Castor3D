/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Device_HPP___
#define ___Renderer_Device_HPP___
#pragma once

#include "Command/CommandPool.hpp"
#include "Command/Queue.hpp"
#include "Core/Connection.hpp"
#include "Pipeline/ColourBlendState.hpp"
#include "Pipeline/RasterisationState.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	The class containing the informations related to the logical device.
	*\remarks
	*	It creates most of the rendering API objects.
	*\~french
	*\brief
	*	Classe contenant les informations liées au GPU logique.
	*\remarks
	*	Elle crée la plupart des objets de rendu.
	*/
	class Device
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderer
		*	The Renderer instance.
		*\param[in] connection
		*	The connection to the application.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de Renderer.
		*\param[in] connection
		*	La connection à l'application.
		*/
		Device( Renderer const & renderer
			, Connection const & connection );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Device() = default;
		/**
		*\~english
		*\return
		*	The clip direction for the rendering API.
		*\~french
		*\return
		*	La direction de clipping pour l'API de rendu.
		*/
		ClipDirection getClipDirection()const;
		/**
		*\~english
		*\brief
		*	Enables the device's context (for OpenGL).
		*\~french
		*\brief
		*	Active le contexte du périphérique (pour OpenGL).
		*/
		void enable()const;
		/**
		*\~french
		*\brief
		*	Désctive le contexte du périphérique (pour OpenGL).
		*\~english
		*\brief
		*	Disables the device's context (for OpenGL).
		*/
		void disable()const;
		/**
		*\~french
		*\brief
		*	Crée une passe de rendu.
		*\param[in] attaches
		*	Les attaches voulues pour la passe.
		*\param[in] subpasses
		*	Les sous passes (au moins 1 nécéssaire).
		*\param[in] initialState
		*	L'état voulu en début de passe.
		*\param[in] finalState
		*	L'état voulu en fin de passe.
		*\param[in] samplesCount
		*	Le nombre d'échantillons (pour le multisampling).
		*\return
		*	La passe créée.
		*\~english
		*\brief
		*	Creates a render pass.
		*\param[in] attaches
		*	The attachments pixels formats.
		*\param[in] subpasses
		*	The rendersubpasses (at least one is necessary).
		*\param[in] initialState
		*	The state wanted at the beginning of the pass.
		*\param[in] finalState
		*	The state attained at the end of the pass.
		*\param[in] samplesCount
		*	The samples count (for multisampling).
		*\return
		*	The created pass.
		*/
		virtual RenderPassPtr createRenderPass( RenderPassAttachmentArray const & attaches
			, RenderSubpassPtrArray && subpasses
			, RenderPassState const & initialState
			, RenderPassState const & finalState
			, SampleCountFlag samplesCount = SampleCountFlag::e1 )const = 0;
		/**
		*\~french
		*\brief
		*	Crée une sous-passe de rendu.
		*\param[in] attaches
		*	Les attaches de la sous-passe.
		*\param[in] neededState
		*	L'état voulu pour l'exécution de cette sous passe.
		*\return
		*	La sous-passe créée.
		*\~english
		*\brief
		*	Creates a render subpass.
		*\param[in] attaches
		*	The subpass attachments.
		*\param[in] neededState
		*	The state wanted for this subpass execution.
		*\return
		*	The created subpass.
		*/
		virtual RenderSubpassPtr createRenderSubpass( RenderPassAttachmentArray const & attaches
			, RenderSubpassState const & neededState )const = 0;
		/**
		*\~french
		*\brief
		*	Crée un layout de sommets.
		*\param[in] bindingSlot
		*	Le point d'attache du tampon associé.
		*\param[in] stride
		*	La taille en octets séparant un élément du suivant, dans le tampon.
		*\param[in] inputRate
		*	La cadence d'entrée.
		*\return
		*	Le layout créé.
		*\~english
		*\brief
		*	Creates a vertex layout.
		*\param[in] bindingSlot
		*	The binding point for associated buffer.
		*\param[in] stride
		*	The byte size from one element to the next one, in the buffer.
		*\param[in] inputRate
		*	The input rate (for instantiation).
		*\return
		*	The created layout.
		*/
		virtual VertexLayoutPtr createVertexLayout( uint32_t bindingSlot
			, uint32_t stride
			, VertexInputRate inputRate = VertexInputRate::eVertex )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] setLayouts
		*	The descriptor sets layouts.
		*\param[in] pushConstantRanges
		*	The push constants ranges.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] setLayouts
		*	Les layouts des descripteurs du pipeline.
		*\param[in] pushConstantRanges
		*	Les intervalles de push constants.
		*\return
		*	Le layout créé.
		*/
		virtual PipelineLayoutPtr createPipelineLayout( DescriptorSetLayoutCRefArray const & setLayouts
			, PushConstantRangeCRefArray const & pushConstantRanges )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a geometry buffers.
		*\param[in] vbos
		*	The VBOs.
		*\param[in] vboOffsets
		*	The offset for the first vertex of each VBO.
		*\param[in] layouts
		*	The vertex layouts, one for each VBO.
		*\~french
		*\brief
		*	Crée tampon de géométries.
		*\param[in] vbos
		*	Les VBOs.
		*\param[in] vboOffsets
		*	L'offset du premier sommet pour chaque VBO.
		*\param[in] layouts
		*	Les layouts, un par vbo de \p vbos.
		*/
		virtual GeometryBuffersPtr createGeometryBuffers( VertexBufferCRefArray const & vbos
			, std::vector< uint64_t > vboOffsets
			, VertexLayoutCRefArray const & layouts )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a geometry buffers.
		*\param[in] vbos
		*	The VBOs.
		*\param[in] vboOffsets
		*	The offset for the first vertex of each VBO.
		*\param[in] layouts
		*	The vertex layouts, one for each VBO.
		*\param[in] ibo
		*	The IBO.
		*\param[in] iboOffset
		*	The offset of the first index in the IBO.
		*\param[in] type
		*	LThe index type.
		*\~french
		*\brief
		*	Crée tampon de géométries.
		*\param[in] vbos
		*	Les VBOs.
		*\param[in] vboOffsets
		*	L'offset du premier sommet pour chaque VBO.
		*\param[in] layouts
		*	Les layouts, un par vbo de \p vbos.
		*\param[in] ibo
		*	L'IBO.
		*\param[in] iboOffset
		*	L'offset du premier indice dans l'IBO.
		*\param[in] type
		*	Le type des indices.
		*/
		virtual GeometryBuffersPtr createGeometryBuffers( VertexBufferCRefArray const & vbos
			, std::vector< uint64_t > vboOffsets
			, VertexLayoutCRefArray const & layouts
			, BufferBase const & ibo
			, uint64_t iboOffset
			, IndexType type )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a descriptor set layout.
		*\param[in] bindings
		*	The layout bindings.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de set de descripteurs.
		*\param[in] bindings
		*	Les attaches du layout.
		*\return
		*	Le layout créé.
		*/
		virtual DescriptorSetLayoutPtr createDescriptorSetLayout( DescriptorSetLayoutBindingArray && bindings )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a texture.
		*\param[in] initialLayout
		*	The image initial layout.
		*\~french
		*\brief
		*	Crée une texture.
		*\param[in] initialLayout
		*	Le layout initial pour l'image.
		*/
		virtual TexturePtr createTexture( ImageLayout initialLayout = ImageLayout::eUndefined )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a sampler.
		*\param[in] wrapS, wrapT, wrapR
		*	The texture wrap modes.
		*\param[in] minFilter, magFilter
		*	The minification and magnification filters.
		*\param[in] mipFilter
		*	The mipmap filter.
		*\~french
		*\brief
		*	Crée un échantillonneur.
		*\param[in] wrapS, wrapT, wrapR
		*	Les modes de wrap de texture.
		*\param[in] minFilter, magFilter
		*	Les filtres de minification et magnification.
		*\param[in] mipFilter
		*	Le filtre de mipmap.
		*/
		virtual SamplerPtr createSampler( WrapMode wrapS
			, WrapMode wrapT
			, WrapMode wrapR
			, Filter minFilter
			, Filter magFilter
			, MipmapMode mipFilter = MipmapMode::eNone
			, float minLod = -1000.0f
			, float maxLod = 1000.0f
			, float lodBias = 0.0f
			, BorderColour borderColour = BorderColour::eFloatOpaqueBlack
			, float maxAnisotropy = 1.0f
			, CompareOp compareOp = CompareOp::eAlways )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a GPU buffer.
		*\param[in] size
		*	The buffer size.
		*\param[in] target
		*	The buffer usage flags.
		*\param[in] memoryFlags
		*	The buffer memory flags.
		*\~french
		*\brief
		*	Crée un tampon GPU.
		*\param[in] size
		*	La taille du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] memoryFlags
		*	Les indicateurs de mémoire du tampon.
		*/
		virtual BufferBasePtr createBuffer( uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags memoryFlags )const = 0;
		/**
		*\~french
		*\brief
		*	Crée une vue sur un tampon GPU.
		*\param[in] buffer
		*	Le tampon sur lequel la vue est créée.
		*\param[in] format
		*	Le format des pixels de la vue.
		*\param[in] offset
		*	Le décalage dans le tampon.
		*\param[in] range
		*	Le nombre d'éléments dans le tampon.
		*\~english
		*\brief
		*	Creates a view on a GPU buffer.
		*\param[in] buffer
		*	The buffer from which the view is created.
		*\param[in] format
		*	The view's pixels format.
		*\param[in] offset
		*	The offset in the buffer.
		*\param[in] range
		*	The number of elements from the buffer.
		*/
		virtual BufferViewPtr createBufferView( BufferBase const & buffer
			, PixelFormat format
			, uint32_t offset
			, uint32_t range )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a uniform buffer.
		*\param[in] count
		*	The buffer elements count.
		*\param[in] size
		*	The size of one element in the buffer.
		*\param[in] target
		*	The buffer usage flags.
		*\param[in] flags
		*	The buffer memory flags.
		*\~french
		*\brief
		*	Crée un tampon d'uniformes.
		*\param[in] count
		*	Le nombre d'éléments du tampon.
		*\param[in] size
		*	La taille d'un élément.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		virtual UniformBufferBasePtr createUniformBuffer( uint32_t count
			, uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags memoryFlags )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a swap chain.
		*\param[in] size
		*	The wanted dimensions.
		*\~french
		*\brief
		*	Crée une swap chain.
		*\param[in] size
		*	Les dimensions souhaitées.
		*/
		virtual SwapChainPtr createSwapChain( UIVec2 const & size )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a semaphore.
		*\~french
		*\brief
		*	Crée un sémaphore.
		*/
		virtual SemaphorePtr createSemaphore()const = 0;
		/**
		*\~english
		*\brief
		*	Creates a fence.
		*\param[in] flags
		*	The fence creation flags.
		*\~french
		*\brief
		*	Crée une barrière.
		*\param[in] flags
		*	Les indicateurs de création de la barrière.
		*/
		virtual FencePtr createFence( FenceCreateFlags flags = 0 )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a command buffer pool.
		*\param[in] queueFamilyIndex
		*	The family index of the queue to which the pool belongs.
		*\param[in] flags
		*	The command pool creation flags.
		*\~french
		*\brief
		*	Crée un pool de tampons de commandes.
		*\param[in] queueFamilyIndex
		*	L'index de la famille à laquelle appartient le pool.
		*\param[in] flags
		*	Les indicateurs de création du pool.
		*/
		virtual CommandPoolPtr createCommandPool( uint32_t queueFamilyIndex
			, CommandPoolCreateFlags const & flags = 0 )const = 0;
		/**
		*\~english
		*\brief
		*	Creates the shader program.
		*\~french
		*\brief
		*	Crée un programme shader.
		*/
		virtual ShaderProgramPtr createShaderProgram()const = 0;
		/**
		*\~english
		*\brief
		*	Creates a query pool.
		*\param[in] type
		*	The query type.
		*\param[in] count
		*	The number of queries managed by the pool.
		*\param[in] pipelineStatistics
		*	Specifies the counters to be returned in queries.
		*\~french
		*\brief
		*	Crée un pool de requêtes.
		*\param[in] type
		*	Le type de requête.
		*\param[in] count
		*	Le nombre de requêtes gérées par le pool.
		*\param[in] pipelineStatistics
		*	Définit les compteurs que les requêtes retournent.
		*/
		virtual QueryPoolPtr createQueryPool( QueryType type
			, uint32_t count
			, QueryPipelineStatisticFlags pipelineStatistics )const = 0;
		/**
		*\english
		*	Computes a perspective projection matrix.
		*\param[in] fovy
		*	The vertical aperture angle.
		*\param[in] aspect
		*	The width / height ratio.
		*\param[in] zNear
		*	The near plane position.
		*\param[in] zFar
		*	The far plane position.
		*\~french
		*\brief
		*	Calcule une matrice de projection en perspective.
		*\param[in] fovy
		*	L'angle d'ouverture verticale.
		*\param[in] aspect
		*	Le ratio largeur / hauteur.
		*\param[in] zNear
		*	La position du premier plan (pour le clipping).
		*\param[in] zNear
		*	La position de l'arrière plan (pour le clipping).
		*/
		virtual Mat4 perspective( Angle fovy
			, float aspect
			, float zNear
			, float zFar )const = 0;
		/**
		*\~english
		*\brief
		*	Computes an orthographic projection matrix.
		*\param[in] left, right
		*	The left and right planes position.
		*\param[in] top, bottom
		*	The top and bottom planes position.
		*\param[in] zNear, zFar
		*	The near and far planes position.
		*\~french
		*\brief
		*	Calcule une matrice de projection orthographique.
		*\param[in] left, right
		*	La position des plans gauche et droite.
		*\param[in] top, bottom
		*	La position des plans haut et bas.
		*\param[in] zNear, zFar
		*	La position des premier et arrière plans.
		*/
		virtual Mat4 ortho( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )const = 0;
		/**
		*\english
		*	Computes a perspective projection matrix with no far plane clipping.
		*\param[in] fovy
		*	The vertical aperture angle.
		*\param[in] aspect
		*	The width / height ratio.
		*\param[in] zNear
		*	The near plane position.
		*\~french
		*\brief
		*	Calcule une matrice de projection en perspective sans clipping
		*	d'arrière plan.
		*\param[in] fovy
		*	L'angle d'ouverture verticale.
		*\param[in] aspect
		*	Le ratio largeur / hauteur.
		*\param[in] zNear
		*	La position du premier plan (pour le clipping).
		*/
		Mat4 infinitePerspective( Angle fovy
			, float aspect
			, float zNear )const;
		/**
		*\~english
		*\brief
		*	Creates a geometry buffers.
		*\param[in] vbo
		*	The VBO.
		*\param[in] vboOffset
		*	The offset of the first vertex in the VBO.
		*\param[in] layout
		*	The vertex buffer layout.
		*\~french
		*\brief
		*	Crée tampon de géométries.
		*\param[in] vbo
		*	Le VBO.
		*\param[in] vboOffset
		*	L'offset du premier sommet dans le VBO.
		*\param[in] layout
		*	Le layout.
		*/
		GeometryBuffersPtr createGeometryBuffers( VertexBufferBase const & vbo
			, uint64_t vboOffset
			, VertexLayout const & layout )const;
		/**
		*\~english
		*\brief
		*	Creates a geometry buffers.
		*\param[in] vbo
		*	The VBO.
		*\param[in] vboOffset
		*	The offset of the first vertex in the VBO.
		*\param[in] layout
		*	The vertex buffer layout.
		*\param[in] ibo
		*	The IBO.
		*\param[in] iboOffset
		*	The offset of the first index in the VBO.
		*\param[in] type
		*	The index type.
		*\~french
		*\brief
		*	Crée tampon de géométries.
		*\param[in] vbo
		*	Le VBO.
		*\param[in] vboOffset
		*	L'offset du premier sommet dans le VBO.
		*\param[in] layout
		*	Le layout.
		*\param[in] ibo
		*	L'IBO.
		*\param[in] iboOffset
		*	L'offset du premier sommet dans l'IBO.
		*\param[in] type
		*	Le type des indices.
		*/
		GeometryBuffersPtr createGeometryBuffers( VertexBufferBase const & vbo
			, uint64_t vboOffset
			, VertexLayout const & layout
			, BufferBase const & ibo
			, uint64_t iboOffset
			, IndexType type )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout()const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] layout
		*	The descriptor set layout.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] layout
		*	Le layout des descripteurs utilisés par le pipeline.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( DescriptorSetLayout const & layout )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] pushConstantRange
		*	The push constants range.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] pushConstantRange
		*	L'intervalle de push constants.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( PushConstantRange const & pushConstantRange )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] layout
		*	The descriptor set layout.
		*\param[in] pushConstantRange
		*	The push constants range.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] layout
		*	Le layout des descripteurs utilisés par le pipeline.
		*\param[in] pushConstantRange
		*	L'intervalle de push constants.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( DescriptorSetLayout const & layout
			, PushConstantRange const & pushConstantRange )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] setLayouts
		*	The descriptor sets layouts.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] setLayouts
		*	Les layouts des descripteurs du pipeline.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( DescriptorSetLayoutCRefArray const & layouts )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] pushConstantRanges
		*	The push constants ranges.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] pushConstantRanges
		*	Les intervalles de push constants.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( PushConstantRangeCRefArray const & pushConstantRanges )const;
		/**
		*\~english
		*\brief
		*	Waits for the device to be idle.
		*\~french
		*\brief
		*	Attend que le périphérique soit inactif.
		*/
		virtual void waitIdle()const = 0;
		/**
		*\~english
		*\brief
		*	The version number.
		*\~french
		*\brief
		*	Le numéro de version.
		*/
		inline std::string const & getVersion()const
		{
			return m_version;
		}
		/**
		*\~english
		*\return
		*	The presentation queue.
		*\~french
		*\return
		*	La file de présentation.
		*/
		inline Queue const & getPresentQueue()const
		{
			return *m_presentQueue;
		}
		/**
		*\~english
		*\return
		*	The compute queue.
		*\~french
		*\return
		*	La file de calcul.
		*/
		inline Queue const & getComputeQueue()const
		{
			return *m_computeQueue;
		}
		/**
		*\~english
		*\return
		*	The graphics queue.
		*\~french
		*\return
		*	La file de dessin.
		*/
		inline Queue const & getGraphicsQueue()const
		{
			return *m_graphicsQueue;
		}
		/**
		*\~english
		*\return
		*	The command buffer pool for the presentation queue.
		*\~french
		*\return
		*	Le pool de tampons de commandes pour la file de présentation.
		*/
		inline CommandPool const & getPresentCommandPool()const
		{
			return *m_presentCommandPool;
		}
		/**
		*\~english
		*\return
		*	The command buffer pool for the compute queue.
		*\~french
		*\return
		*	Le pool de tampons de commandes pour la file de calcul.
		*/
		inline CommandPool const & getComputeCommandPool()const
		{
			return *m_computeCommandPool;
		}
		/**
		*\~english
		*\return
		*	The command buffer pool for the graphics queue.
		*\~french
		*\return
		*	Le pool de tampons de commandes pour la file de dessin.
		*/
		inline CommandPool const & getGraphicsCommandPool()const
		{
			return *m_graphicsCommandPool;
		}
		/**
		*\~english
		*\return
		*	The parent Renderer.
		*\~french
		*\return
		*	Le Renderer parent.
		*/
		inline Renderer const & getRenderer()const
		{
			return m_renderer;
		}
		/**
		*\~english
		*\return
		*	The number of nanoseconds it takes for timestamp value to be incremented by 1.
		*\~french
		*\return
		*	Le nombre de nanosecondes qu'il faut pour que la valeur d'un timestamp soit incrémentée de 1.
		*/
		inline float getTimestampPeriod()const
		{
			return m_timestampPeriod;
		}

	private:
		/**
		*\~french
		*\brief
		*	Active le contexte du périphérique (pour OpenGL).
		*\~english
		*\brief
		*	Enables the device's context (for OpenGL).
		*/
		virtual void doEnable()const = 0;
		/**
		*\~french
		*\brief
		*	Désactive le contexte du périphérique (pour OpenGL).
		*\~english
		*\brief
		*	Disables the device's context (for OpenGL).
		*/
		virtual void doDisable()const = 0;

	public:
		DeviceEnabledSignal onEnabled;
		DeviceDisabledSignal onDisabled;

	protected:
		Renderer const & m_renderer;
		QueuePtr m_presentQueue;
		QueuePtr m_computeQueue;
		QueuePtr m_graphicsQueue;
		CommandPoolPtr m_presentCommandPool;
		CommandPoolPtr m_computeCommandPool;
		CommandPoolPtr m_graphicsCommandPool;
		std::string m_version;
		float m_timestampPeriod;
	};
}

#endif
