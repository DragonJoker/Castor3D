/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CommandBuffer_HPP___
#define ___Renderer_CommandBuffer_HPP___
#pragma once

#include "Buffer/PushConstantsBuffer.hpp"
#include "Miscellaneous/BufferCopy.hpp"
#include "Miscellaneous/BufferImageCopy.hpp"
#include "Miscellaneous/ImageBlit.hpp"
#include "Miscellaneous/ImageCopy.hpp"
#include "RenderPass/ClearValue.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	A command buffer.
	*\~french
	*\brief
	*	Un tampon de commandes.
	*/
	class CommandBuffer
	{
	protected:
		CommandBuffer( CommandBuffer const & ) = delete;
		CommandBuffer & operator=( CommandBuffer const & ) = delete;
		CommandBuffer() = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] pool
		*	The parent command buffer pool.
		*\param[in] primary
		*	Tells if the command buffer is primary (\p true) or not (\p false).
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] pool
		*	Le pool de tampons de commandes.
		*\param[in] primary
		*	Dit si le tampon est un tampon de commandes primaire (\p true) ou secondaire (\p false).
		*/
		CommandBuffer( Device const & device
			, CommandPool const & pool
			, bool primary = true );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~CommandBuffer() = default;
		/**
		*\~english
		*\brief
		*	Starts recording the command buffer.
		*\param[in] flags
		*	The usage flags for the command buffer.
		*\return
		*	\p false on any problem.
		*\~french
		*\brief
		*	Démarre l'enregistrement du tampon de commandes.
		*\param[in] flags
		*	Les indicateurs de type de charge qui sera affectée au tampon.
		*\return
		*	\p false en cas d'erreur.
		*/
		virtual bool begin( CommandBufferUsageFlags flags = 0u )const = 0;
		/**
		*\~english
		*\brief
		*	Starts recording the command buffer as a secondary command buffer.
		*\param[in] flags
		*	The usage flags for the command buffer.
		*\param[in] inheritanceInfo
		*	The inheritance informations.
		*\return
		*	\p false on any error.
		*\~french
		*\brief
		*	Démarre l'enregistrement du tampon de commandes en tant que tampon secondaire.
		*\param[in] flags
		*	Les indicateurs de type de charge qui sera affectée au tampon.
		*\param[in] inheritanceInfo
		*	Les informations d'héritage.
		*\return
		*	\p false en cas d'erreur.
		*/
		virtual bool begin( CommandBufferUsageFlags flags
			, CommandBufferInheritanceInfo const & inheritanceInfo )const = 0;
		/**
		*\~english
		*\brief
		*	Ends the command buffer recording.
		*\return
		*	\p false on any error.
		*\~french
		*\brief
		*	Termine l'enregistrement du tampon de commandes.
		*\return
		*	\p false en cas d'erreur.
		*/
		virtual bool end()const = 0;
		/**
		*\~english
		*\brief
		*	Reset a command buffer to the initial state.
		*\param[in] flags
		*	The flags controlling the reset operation.
		*\return
		*	\p false on any error.
		*\~french
		*\brief
		*	Réinitialise le tampon de commandes et le met dans un état où il peut à nouveau enregistrer des commandes.
		*\param[in] flags
		*	Les indicateurs contrôlant le comportement de la réinitialisation du tampon.
		*\return
		*	\p false en cas d'erreur.
		*/
		virtual bool reset( CommandBufferResetFlags flags = 0u )const = 0;
		/**
		*\~english
		*\brief
		*	Begins a new render pass.
		*\param[in] renderPass
		*	The render pass to begin.
		*\param[in] frameBuffer
		*	The framebuffer containing the attachments that are used with the render pass.
		*\param[in] clearValues
		*	The clear values for each attachment that needs to be cleared.
		*\param[in] contents
		*	Specifies how the commands in the first subpass will be provided.
		*\~french
		*\brief
		*	Démarre une passe de rendu.
		*\param[in] renderPass
		*	La passe de rendu.
		*\param[in] frameBuffer
		*	Le tampon d'image affecté par le rendu.
		*\param[in] clearValues
		*	Les valeurs de vidage, une par attache de la passe de rendu.
		*\param[in] contents
		*	Indique la manière dont les commandes de la première sous-passe sont fournies.
		*/
		virtual void beginRenderPass( RenderPass const & renderPass
			, FrameBuffer const & frameBuffer
			, ClearValueArray const & clearValues
			, SubpassContents contents )const = 0;
		/**
		*\~english
		*\brief
		*	Transition to the next subpass of a render pass.
		*\param[in] contents
		*	Specifies how the commands in the next subpass will be provided.
		*\~french
		*\brief
		*	Passe à la sous-passe suivante.
		*\param[in] contents
		*	Indique la manière dont les commandes de la sous-passe suivante sont fournies.
		*/
		virtual void nextSubpass( SubpassContents contents )const = 0;
		/**
		*\~english
		*\brief
		*	End the current render pass.
		*\~french
		*\brief
		*	Termine la passe de rendu courante.
		*/
		virtual void endRenderPass()const = 0;
		/**
		*\~english
		*\brief
		*	Execute a secondary command buffer.
		*\param[in] commands
		*	The secondary command buffers, which are recorded to execute in the order they are listed in the array.
		*\~french
		*\brief
		*	Execute des tampons de commande secondaires, qui sont enregistrés pour être exécutés dans l'ordre du tableau.
		*\param[in] commands
		*	Les tampons de commandes.
		*/
		virtual void executeCommands( CommandBufferCRefArray const & commands )const = 0;
		/**
		*\~english
		*\brief
		*	Clear a color image.
		*\param[in] image
		*	The image to clear.
		*\param[in] colour
		*	The clear colour.
		*\~french
		*\brief
		*	Vide une image couleur.
		*\param[in] image
		*	L'image à vider.
		*\param[in] colour
		*	La couleur de vidage.
		*/
		virtual void clear( TextureView const & image
			, RgbaColour const & colour )const = 0;
		/**
		*\~english
		*\brief
		*	Clear a depth and/or stencil image.
		*\param[in] image
		*	The image to clear.
		*\param[in] value
		*	The clear value.
		*\~french
		*\brief
		*	Vide une image profondeur et/ou stencil.
		*\param[in] image
		*	L'image à vider.
		*\param[in] value
		*	La valeur de vidage.
		*/
		virtual void clear( renderer::TextureView const & image
			, renderer::DepthStencilClearValue const & value )const = 0;
		/**
		*\~english
		*\brief
		*	Clear regions within currently bound framebuffer attachments.
		*\param[in] clearAttachments
		*	The attachments to clear and the clear values to use.
		*\param[in] clearRects
		*	The regions within each selected attachment to clear.
		*\~french
		*\brief
		*	Vide une région des attaches du tampon d'images actuellement attaché.
		*\param[in] clearAttachments
		*	Les attaches à vider et les valeurs de nettoyage à utiliser.
		*\param[in] clearRects
		*	Les régions à nettoyer pour chaque attache sélectionnée.
		*/
		virtual void clearAttachments( ClearAttachmentArray const & clearAttachments
			, ClearRectArray const & clearRects ) = 0;
		/**
		*\~english
		*\brief
		*	Defines a memory dependency between commands that were submitted before it, and those submitted after it.
		*\param[in] after
		*	Specifies the pipeline stages that must be ended before the barrier.
		*\param[in] before
		*	Specifies the pipeline stages that can be started after the barrier.
		*\param[in] transitionBarrier
		*	Describes the transition.
		*\~french
		*\brief
		*	Met en place une barrière de transition d'état de tampon.
		*\param[in] after
		*	Les étapes devant être terminées avant l'exécution de la barrière.
		*\param[in] before
		*	Les étapes pouvant être commencées après l'exécution de la barrière.
		*\param[in] transitionBarrier
		*	La description de la transition.
		*/
		virtual void memoryBarrier( PipelineStageFlags after
			, PipelineStageFlags before
			, BufferMemoryBarrier const & transitionBarrier )const = 0;
		/**
		*\~english
		*\brief
		*	Defines a memory dependency between commands that were submitted before it, and those submitted after it.
		*\param[in] after
		*	Specifies the pipeline stages that must be ended before the barrier.
		*\param[in] before
		*	Specifies the pipeline stages that can be started after the barrier.
		*\param[in] transitionBarrier
		*	Describes the transition.
		*\~french
		*\brief
		*	Met en place une barrière de transition de layout d'image.
		*\param[in] after
		*	Les étapes devant être terminées avant l'exécution de la barrière.
		*\param[in] before
		*	Les étapes pouvant être commencées après l'exécution de la barrière.
		*\param[in] transitionBarrier
		*	La description de la transition.
		*/
		virtual void memoryBarrier( PipelineStageFlags after
			, PipelineStageFlags before
			, ImageMemoryBarrier const & transitionBarrier )const = 0;
		/**
		*\~english
		*\brief
		*	Binds a pipeline to the command buffer.
		*\param[in] pipeline
		*	The pipeline to be bound.
		*\param[in] bindingPoint
		*	Specifies whether to bind to the compute or graphics bind point.
		*\~french
		*\brief
		*	Active un pipeline: shaders, tests, états, ...
		*\param[in] pipeline
		*	Le pipeline à activer.
		*\param[in] bindingPoint
		*	Le point d'attache du pipeline.
		*/
		virtual void bindPipeline( Pipeline const & pipeline
			, PipelineBindPoint bindingPoint = PipelineBindPoint::eGraphics )const = 0;
		/**
		*\~english
		*\brief
		*	Binds a compute pipeline to the command buffer.
		*\param[in] pipeline
		*	The pipeline to be bound.
		*\param[in] bindingPoint
		*	Specifies whether to bind to the compute or graphics bind point.
		*\~french
		*\brief
		*	Active un pipeline de calcul.
		*\param[in] pipeline
		*	Le pipeline à activer.
		*\param[in] bindingPoint
		*	Le point d'attache du pipeline.
		*/
		virtual void bindPipeline( ComputePipeline const & pipeline
			, PipelineBindPoint bindingPoint = PipelineBindPoint::eCompute )const = 0;
		/**
		*\~english
		*\brief
		*	Binds vertex buffers to the command buffer.
		*\param[in] firstBinding
		*	The index of the first vertex input binding whose state is updated by the command.
		*\param[in] buffers
		*	The array of buffer handles.
		*\param[in] offsets
		*	The array of buffer offsets.
		*\~french
		*\brief
		*	Active des tampons de sommets sur le tampon de commandes.
		*\param[in] firstBinding
		*	L'indice de la première attache d'entrée de sommets dont l'état est mis à jour par la commande.
		*\param[in] buffers
		*	Un tableau de tampons de sommets.
		*\param[in] offsets
		*	Un tableau d'offsets dans les tampons.
		*/
		virtual void bindVertexBuffers( uint32_t firstBinding
			, BufferCRefArray const & buffers
			, UInt64Array offsets )const = 0;
		/**
		*\~english
		*\brief
		*	Binds an index buffer to the command buffer.
		*\param[in] buffer
		*	The buffer being bound.
		*\param[in] offset
		*	The starting offset in bytes within buffer used in index buffer address calculations.
		*\param[in] type
		*	Tells whether the indices are treated as 16 bits or 32 bits.
		*\~french
		*\brief
		*	Active un tampon d'indices sur le tampon de commandes.
		*\param[in] buffer
		*	Le tampon à activer.
		*\param[in] offset
		*	L'offset de départ en octets dans le tampon, utilisé pour le calcul des indices.
		*\param[in] type
		*	Dit si les indices sont traités en 16 bits ou 32 bits.
		*/
		virtual void bindIndexBuffer( BufferBase const & buffer
			, uint64_t offset
			, IndexType indexType )const = 0;
		/**
		*\~english
		*\brief
		*	Binds descriptor sets to the command buffer.
		*\param[in] descriptorSet
		*	The descriptor sets.
		*\param[in] layout
		*	The pipeline layout used to program the binding.
		*\param[in] bindingPoint
		*	Indicates whether the descriptor wil be used by graphics or compute pipeline.
		*\~french
		*\brief
		*	Active des descriptor sets.
		*\param[in] descriptorSet
		*	Les descriptor sets.
		*\param[in] layout
		*	Le layout de pipeline.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*\param[in] dynamicOffsets
		*	Les offsets des .
		*/
		virtual void bindDescriptorSets( DescriptorSetCRefArray const & descriptorSet
			, PipelineLayout const & layout
			, renderer::UInt32Array const & dynamicOffsets
			, PipelineBindPoint bindingPoint = PipelineBindPoint::eGraphics )const = 0;
		/**
		*\~english
		*\brief
		*	Defines the currently bound pipeline viewport.
		*\remarks
		*	This action is possible only if the viewport is dynamic, in the pipeline.
		*\param[in] viewport
		*	The viewport.
		*\~french
		*\brief
		*	Définit le viewport du pipeline.
		*\remarks
		*	Cette action n'est faisable que si le viewport est configuré comme dynamique.
		*\param[in] viewport
		*	Le viewport.
		*/
		virtual void setViewport( Viewport const & viewport )const = 0;
		/**
		*\~english
		*\brief
		*	Defines the currently bound pipeline scissor.
		*\remarks
		*	This action is possible only if the scissor is dynamic, in the pipeline.
		*\param[in] scissor
		*	The scissor.
		*\~french
		*\brief
		*	Définit le scissor du pipeline.
		*\remarks
		*	Cette action n'est faisable que si le scissor est configuré comme dynamique.
		*\param[in] scissor
		*	Le scissor.
		*/
		virtual void setScissor( Scissor const & scissor )const = 0;
		/**
		*\~english
		*\brief
		*	Draws some vertices.
		*\param[in] vtxCount
		*	The vertex count.
		*\param[in] instCount
		*	The instances count.
		*\param[in] firstVertex
		*	The first vertex index.
		*\param[in] firstInstance
		*	The first instance index.
		*\~french
		*\brief
		*	Dessine des sommets.
		*\param[in] vtxCount
		*	Nombre de sommets.
		*\param[in] instCount
		*	Nombre d'instances.
		*\param[in] firstVertex
		*	Index du premier sommet.
		*\param[in] firstInstance
		*	Index de la première instance.
		*/
		virtual void draw( uint32_t vtxCount
			, uint32_t instCount = 1u
			, uint32_t firstVertex = 0u
			, uint32_t firstInstance = 0u )const = 0;
		/**
		*\~english
		*\brief
		*	Draws some indexed vertices.
		*\param[in] indexCount
		*	The index count.
		*\param[in] instCount
		*	The instances count.
		*\param[in] firstIndex
		*	The first index index.
		*\param[in] vertexOffset
		*	The offset added to the vertex index, before indexing the vertex buffer.
		*\param[in] firstInstance
		*	The first instance index.
		*\~french
		*\brief
		*	Dessine des sommets.
		*\param[in] indexCount
		*	Nombre d'indices.
		*\param[in] instCount
		*	Nombre d'instances.
		*\param[in] firstIndex
		*	Index du premier indice.
		*\param[in] vertexOffset
		*	La valeur ajoutée à l'indice du sommet avant d'indexer le tampon de sommets.
		*\param[in] firstInstance
		*	Index de la première instance.
		*/
		virtual void drawIndexed( uint32_t indexCount
			, uint32_t instCount = 1u
			, uint32_t firstIndex = 0u
			, uint32_t vertexOffset = 0u
			, uint32_t firstInstance = 0u )const = 0;
		/**
		*\~english
		*\brief
		*	Perform an indirect draw.
		*\param[in] buffer
		*	The buffer containing draw parameters.
		*\param[in] offset
		*	The byte offset into \p buffer where parameters begin.
		*\param[in] drawCount
		*	The number of draws to execute, and can be zero.
		*\param[in] stride
		*	The byte stride between successive sets of draw parameters.
		*\~french
		*\brief
		*	Effectue un dessin indirect.
		*\param[in] buffer
		*	Le tampon contenant les paramètres de dessin.
		*\param[in] offset
		*	L'offset en octets dans \p buffer, où les paramètres commencent.
		*\param[in] drawCount
		*	Le nombre de dessins à effectuer, peut être zéro.
		*\param[in] stride
		*	Le stride en octets entre deux ensembles successifs de paramètres de dessin.
		*/
		virtual void drawIndirect( BufferBase const & buffer
			, uint32_t offset
			, uint32_t drawCount
			, uint32_t stride )const = 0;
		/**
		*\~english
		*\brief
		*	Perform an indexed indirect draw.
		*\param[in] buffer
		*	The buffer containing draw parameters.
		*\param[in] offset
		*	The byte offset into \p buffer where parameters begin.
		*\param[in] drawCount
		*	The number of draws to execute, and can be zero.
		*\param[in] stride
		*	The byte stride between successive sets of draw parameters.
		*\~french
		*\brief
		*	Effectue un dessin indirect indexé.
		*\param[in] buffer
		*	Le tampon contenant les paramètres de dessin.
		*\param[in] offset
		*	L'offset en octets dans \p buffer, où les paramètres commencent.
		*\param[in] drawCount
		*	Le nombre de dessins à effectuer, peut être zéro.
		*\param[in] stride
		*	Le stride en octets entre deux ensembles successifs de paramètres de dessin.
		*/
		virtual void drawIndexedIndirect( BufferBase const & buffer
			, uint32_t offset
			, uint32_t drawCount
			, uint32_t stride )const = 0;
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon vers une image.
		*\param[in] copyInfo
		*	Les informations de la copie.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	L'image destination.
		*\~english
		*\brief
		*	Copies data from a buffer to an image.
		*\param[in] copyInfo
		*	The copy informations.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination image.
		*/
		virtual void copyToImage( BufferImageCopyArray const & copyInfo
			, BufferBase const & src
			, Texture const & dst )const = 0;
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon vers une image.
		*\param[in] copyInfo
		*	Les informations de la copie.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	L'image destination.
		*\~english
		*\brief
		*	Copies data from a buffer to an image.
		*\param[in] copyInfo
		*	The copy informations.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination image.
		*/
		virtual void copyToBuffer( BufferImageCopyArray const & copyInfo
			, Texture const & src
			, BufferBase const & dst )const = 0;
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon vers un autre.
		*\param[in] copyInfo
		*	Les informations de la copie.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\~english
		*\brief
		*	Copies data from a buffer to another one.
		*\param[in] copyInfo
		*	The copy informations.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination buffer.
		*/
		virtual void copyBuffer( BufferCopy const & copyInfo
			, BufferBase const & src
			, BufferBase const & dst )const = 0;
		/**
		*\~french
		*\brief
		*	Copie les données d'une image vers une autre.
		*\param[in] copyInfo
		*	Les informations de la copie.
		*\param[in] src
		*	L'image source.
		*\param[in] dst
		*	L'image destination.
		*\~english
		*\brief
		*	Copies data from an image to another one.
		*\param[in] copyInfo
		*	The copy informations.
		*\param[in] src
		*	The source image.
		*\param[in] dst
		*	The destination image.
		*/
		virtual void copyImage( ImageCopy const & copyInfo
			, Texture const & src
			, ImageLayout srcLayout
			, Texture const & dst
			, ImageLayout dstLayout )const = 0;
		/**
		*\~french
		*\brief
		*	Copie des régions d'une image vers une autre.
		*\param[in] regions
		*	Les régions à blitter.
		*\param[in] srcImage
		*	L'image source.
		*\param[in] srcLayout
		*	Le layout de l'image source.
		*\param[in] dstImage
		*	L'image destination.
		*\param[in] dstLayout
		*	Le layout de l'image destination.
		*\param[in] filter
		*	Le filtre appliqué si la copie nécessite une mise à l'échelle.
		*\~english
		*\brief
		*	Copy regions of an image to another one.
		*\param[in] regions
		*	The regions to blit.
		*\param[in] srcImage
		*	The source image.
		*\param[in] srcLayout
		*	The source image layout.
		*\param[in] dstImage
		*	The destination image.
		*\param[in] dstLayout
		*	The source image layout.
		*\param[in] filter
		*	The filter applied if the blit requires scaling.
		*/
		virtual void blitImage( Texture const & srcImage
			, ImageLayout srcLayout
			, Texture const & dstImage
			, ImageLayout dstLayout
			, std::vector< ImageBlit > const & regions
			, Filter filter )const = 0;
		/**
		*\~english
		*\brief
		*	Resets a range of queries in a query pool.
		*\param[in] pool
		*	The query pool.
		*\param[in] firstQuery
		*	The first query index.
		*\param[in] queryCount
		*	The number of queries (starting at \p firstQuery).
		*\~french
		*\brief
		*	Réinitialise un intervalle de requêtes d'un pool.
		*\param[in] pool
		*	Le pool de requêtes.
		*\param[in] firstQuery
		*	L'index de la première requête.
		*\param[in] queryCount
		*	Le nombre de requêtes (à partir de \p firstQuery).
		*/
		virtual void resetQueryPool( QueryPool const & pool
			, uint32_t firstQuery
			, uint32_t queryCount )const = 0;
		/**
		*\~english
		*\brief
		*	Begins a query.
		*\param[in] pool
		*	The query pool.
		*\param[in] query
		*	The query index.
		*\param[in] flags
		*	The constraints on the types of queries that can be performed.
		*\~french
		*\brief
		*	Démarre un requête.
		*\param[in] pool
		*	Le pool de requêtes.
		*\param[in] query
		*	L'index de la requête.
		*\param[in] flags
		*	Les contraintes sur les types de requête qui peuvent être effectuées.
		*/
		virtual void beginQuery( QueryPool const & pool
			, uint32_t query
			, QueryControlFlags flags )const = 0;
		/**
		*\~english
		*\brief
		*	Ends a query.
		*\param[in] pool
		*	The query pool.
		*\param[in] query
		*	The query index.
		*\~french
		*\brief
		*	Termine une requête.
		*\param[in] pool
		*	Le pool de requêtes.
		*\param[in] query
		*	L'index de la requête.
		*/
		virtual void endQuery( QueryPool const & pool
			, uint32_t query )const = 0;
		/**
		*\~english
		*\brief
		*	Writes a timestamp to given query result.
		*\param[in] pipelineStage
		*	Specifies the stage of the pipeline.
		*\param[in] pool
		*	The query pool.
		*\param[in] query
		*	The query index.
		*\~french
		*\brief
		*	Ecrit un timestamp dans le résultat d'une requête.
		*\param[in] pipelineStage
		*	Définit l'étape du pipeline.
		*\param[in] pool
		*	Le pool de requêtes.
		*\param[in] query
		*	L'index de la requête.
		*/
		virtual void writeTimestamp( PipelineStageFlag pipelineStage
			, QueryPool const & pool
			, uint32_t query )const = 0;
		/**
		*\~french
		*\brief
		*	Met à jour les valeurs de push constants.
		*\param[in] layout
		*	Le layout de pipeline utilisé pour programmer la mise à jour des push constants.
		*\param[in] pcb
		*	Le tampon de push constants.
		*\~english
		*\brief
		*	Updates the values of push constants.
		*\param[in] layout
		*	The pipeline layout used to program the push constants updates.
		*\param[in] pcb
		*	The push constants buffer.
		*/
		virtual void pushConstants( PipelineLayout const & layout
			, PushConstantsBufferBase const & pcb )const = 0;
		/**
		*\~french
		*\brief
		*	Distribue des éléments de calcul.
		*\param[in] groupCountX, groupCountY, groupCountZ
		*	Le nombre de groupes de travail locaux à distribuer dans les dimensions X, Y, et Z.
		*\~english
		*\brief
		*	Dispatch compute work items.
		*\param[in] groupCountX, groupCountY, groupCountZ
		*	The number of local workgroups to dispatch to the X, Y, and Z dimensions.
		*/
		virtual void dispatch( uint32_t groupCountX
			, uint32_t groupCountY
			, uint32_t groupCountZ )const = 0;
		/**
		*\~english
		*\brief
		*	Dispatch compute work items using indirect parameters.
		*\param[in] buffer
		*	The buffer containing dispatch parameters.
		*\param[in] offset
		*	The byte offset into \p buffer where parameters begin.
		*\~french
		*\brief
		*	Distribue des éléments de calcul en utilisant des paramètres indirects.
		*\param[in] buffer
		*	Le tampon contenant les paramètres de distribution.
		*\param[in] offset
		*	L'offset en octets dans \p buffer, où les paramètres commencent.
		*/
		virtual void dispatchIndirect( BufferBase const & buffer
			, uint32_t offset )const = 0;
		/**
		*\~french
		*\brief
		*	Définit la largeur des lignes, si celle-ci est un état dynamique du pipeline.
		*\param[in] width
		*	La largeur voulue.
		*\~english
		*\brief
		*	Specifies the lines width, if this is a dynamic state in the pipeline.
		*\param[in] width
		*	The wanted width.
		*/
		virtual void setLineWidth( float width )const = 0;
		/**
		*\~french
		*\brief
		*	Définit la largeur des lignes, si celle-ci est un état dynamique du pipeline.
		*\param[in] constantFactor
		*	Le facteur contrôlant la valeur constante de profondeur ajoutée à chaque fragment.
		*\param[in] clamp
		*	Le décalage maximum (ou minimum) de profondeur d'un fragment.
		*\param[in] slopeFactor
		*	Le facteur appliqué à la pente d'un fragment lors du calcul de décalage de profondeur.
		*\~english
		*\brief
		*	Specifies the lines width, if this is a dynamic state in the pipeline.
		*\param[in] constantFactor
		*	The scalar factor controlling the constant depth value added to each fragment.
		*\param[in] clamp
		*	The maximum (or minimum) depth bias of a fragment.
		*\param[in] slopeFactor
		*	The scalar factor applied to a fragment’s slope in depth bias calculations.
		*/
		virtual void setDepthBias( float constantFactor
			, float clamp
			, float slopeFactor )const = 0;
		/**
		*\~english
		*\brief
		*	Binds a vertex buffer to the command buffer.
		*\param[in] binding
		*	The index of the vertex input binding whose state is updated by the command.
		*\param[in] buffers
		*	The array of buffer handles.
		*\param[in] offsets
		*	The array of buffer offsets.
		*\~french
		*\brief
		*	Active un tampon de sommets sur le tampon de commandes.
		*\param[in] firstBinding
		*	L'indice de l'attache d'entrée de sommets dont l'état est mis à jour par la commande.
		*\param[in] buffers
		*	Le tampon de sommets.
		*\param[in] offsets
		*	L'offsets dans le tampon.
		*/
		void bindVertexBuffer( uint32_t binding
			, BufferBase const & buffer
			, uint64_t offset )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon vers une image.
		*\param[in] copyInfo
		*	Les informations de la copie.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	L'image destination.
		*\~english
		*\brief
		*	Copies data from a buffer to an image.
		*\param[in] copyInfo
		*	The copy informations.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination image.
		*/
		void copyToImage( BufferImageCopy const & copyInfo
			, BufferBase const & src
			, Texture const & dst )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon vers une image.
		*\param[in] copyInfo
		*	Les informations de la copie.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	L'image destination.
		*\~english
		*\brief
		*	Copies data from a buffer to an image.
		*\param[in] copyInfo
		*	The copy informations.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination image.
		*/
		void copyToBuffer( BufferImageCopy const & copyInfo
			, Texture const & src
			, BufferBase const & dst )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	Le décalage dans le tampon source.
		*\~english
		*\brief
		*	Copies data from a buffer to another one.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination buffer.
		*\param[in] size
		*	The byte size of the data to copy.
		*\param[in] offset
		*	The offset in the source buffer.
		*/
		void copyBuffer( BufferBase const & src
			, BufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const;
		/**
		*\~english
		*\brief
		*	Binds a descriptor set to the command buffer.
		*\param[in] descriptorSet
		*	The descriptor set.
		*\param[in] layout
		*	The pipeline layout used to program the binding.
		*\param[in] bindingPoint
		*	Indicates whether the descriptor wil be used by graphics or compute pipeline.
		*\~french
		*\brief
		*	Active un descriptor set.
		*\param[in] descriptorSet
		*	Le descriptor set.
		*\param[in] layout
		*	Le layout de pipeline.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*/
		inline void bindDescriptorSet( DescriptorSet const & descriptorSet
			, PipelineLayout const & layout
			, renderer::UInt32Array const & dynamicOffsets
			, PipelineBindPoint bindingPoint = PipelineBindPoint::eGraphics )const
		{
			bindDescriptorSets( DescriptorSetCRefArray{ descriptorSet }
				, layout
				, dynamicOffsets
				, bindingPoint );
		}
		/**
		*\~english
		*\brief
		*	Binds a descriptor set to the command buffer.
		*\param[in] descriptorSet
		*	The descriptor set.
		*\param[in] layout
		*	The pipeline layout used to program the binding.
		*\param[in] bindingPoint
		*	Indicates whether the descriptor wil be used by graphics or compute pipeline.
		*\~french
		*\brief
		*	Active un descriptor set.
		*\param[in] descriptorSet
		*	Le descriptor set.
		*\param[in] layout
		*	Le layout de pipeline.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*/
		inline void bindDescriptorSet( DescriptorSet const & descriptorSet
			, PipelineLayout const & layout
			, PipelineBindPoint bindingPoint = PipelineBindPoint::eGraphics )const
		{
			bindDescriptorSets( DescriptorSetCRefArray{ descriptorSet }
				, layout
				, UInt32Array{}
				, bindingPoint );
		}
		/**
		*\~english
		*\brief
		*	Binds descriptor sets to the command buffer.
		*\param[in] descriptorSet
		*	The descriptor sets.
		*\param[in] layout
		*	The pipeline layout used to program the binding.
		*\param[in] bindingPoint
		*	Indicates whether the descriptor wil be used by graphics or compute pipeline.
		*\~french
		*\brief
		*	Active des descriptor sets.
		*\param[in] descriptorSet
		*	Les descriptor sets.
		*\param[in] layout
		*	Le layout de pipeline.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*/
		inline void bindDescriptorSets( DescriptorSetCRefArray const & descriptorSet
			, PipelineLayout const & layout
			, PipelineBindPoint bindingPoint = PipelineBindPoint::eGraphics )const
		{
			bindDescriptorSets( descriptorSet
				, layout
				, UInt32Array{}
				, bindingPoint );
		}
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	Le décalage dans le tampon source.
		*\~english
		*\brief
		*	Copies data from a buffer to another one.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination buffer.
		*\param[in] size
		*	The byte size of the data to copy.
		*\param[in] offset
		*	The offset in the source buffer.
		*/
		void copyBuffer( BufferBase const & src
			, VertexBufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	Le décalage dans le tampon source.
		*\~english
		*\brief
		*	Copies data from a buffer to another one.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination buffer.
		*\param[in] size
		*	The byte size of the data to copy.
		*\param[in] offset
		*	The offset in the source buffer.
		*/
		void copyBuffer( VertexBufferBase const & src
			, BufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	Le décalage dans le tampon source.
		*\~english
		*\brief
		*	Copies data from a buffer to another one.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination buffer.
		*\param[in] size
		*	The byte size of the data to copy.
		*\param[in] offset
		*	The offset in the source buffer.
		*/
		void copyBuffer( VertexBufferBase const & src
			, VertexBufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	Le décalage dans le tampon source.
		*\~english
		*\brief
		*	Copies data from a buffer to another one.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination buffer.
		*\param[in] size
		*	The byte size of the data to copy.
		*\param[in] offset
		*	The offset in the source buffer.
		*/
		void copyBuffer( BufferBase const & src
			, UniformBufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	Le décalage dans le tampon source.
		*\~english
		*\brief
		*	Copies data from a buffer to another one.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination buffer.
		*\param[in] size
		*	The byte size of the data to copy.
		*\param[in] offset
		*	The offset in the source buffer.
		*/
		void copyBuffer( UniformBufferBase const & src
			, BufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	Le décalage dans le tampon source.
		*\~english
		*\brief
		*	Copies data from a buffer to another one.
		*\param[in] src
		*	The source buffer.
		*\param[in] dst
		*	The destination buffer.
		*\param[in] size
		*	The byte size of the data to copy.
		*\param[in] offset
		*	The offset in the source buffer.
		*/
		void copyBuffer( UniformBufferBase const & src
			, UniformBufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'une image vers une autre.
		*\param[in] src
		*	L'image source.
		*\param[in] dst
		*	L'image destination.
		*\~english
		*\brief
		*	Copies data from an image to another one.
		*\param[in] src
		*	The source image.
		*\param[in] dst
		*	The destination image.
		*/
		void copyImage( TextureView const & src
			, TextureView const & dst )const;
		/**
		*\~french
		*\brief
		*	Met à jour les valeurs de push constants.
		*\param[in] layout
		*	Le layout de pipeline utilisé pour programmer la mise à jour des push constants.
		*\param[in] pcb
		*	Le tampon de push constants.
		*\~english
		*\brief
		*	Updates the values of push constants.
		*\param[in] layout
		*	The pipeline layout used to program the push constants updates.
		*\param[in] pcb
		*	The push constants buffer.
		*/
		template< typename T >
		inline void pushConstants( PipelineLayout const & layout
			, PushConstantsBuffer< T > const & pcb )const
		{
			pushConstants( layout, pcb.getBuffer() );
		}
	};
}

#endif
