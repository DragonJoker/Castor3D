/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQueue_H___
#define ___C3D_RenderQueue_H___

#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Multithreading/SpinMutex.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#if defined( CU_CompilerMSVC )
#	pragma warning( push )
#	pragma warning( disable:4503 )
#endif

namespace castor3d
{
	class RenderQueue
		: public castor::OwnedBy< RenderNodesPass >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	renderPass	The parent render pass.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	renderPass	La passe de rendu parente.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderQueue( RenderNodesPass & renderPass
			, SceneNode const * ignored );
		C3D_API ~RenderQueue();
		/**
		 *\~english
		 *\brief		Initialises the queue.
		 *\~french
		 *\brief		Initialise la file de rendu.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Tells the queue its GPU data is not valid anymore.
		 *\~french
		 *\brief		Dit à la queue que ses données GPU sont invalides.
		 */
		C3D_API void invalidate();
		/**
		 *\~english
		 *\brief		Cleans the queue up.
		 *\~french
		 *\brief		Nettoie la file de rendu.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief			Updates the render nodes.
		 *\param[in,out]	shadowMaps	Receives the shadow maps used in the render pass.
		 *\~french
		 *\brief			Met à jour les noeuds de rendu.
		 *\param[in,out]	shadowMaps	Reçoit les shadow maps utilisées par la passe de rendu.
		 */
		C3D_API void update( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer );
		/**
		 *\~english
		 *\brief			Updates the render nodes.
		 *\param[in,out]	shadowMaps	Receives the shadow maps used in the render pass.
		 *\param[in]		viewport	The viewport restraining the render pass.
		 *\param[in]		scissor		The scissors restraining the render pass.
		 *\~french
		 *\brief			Met à jour les noeuds de rendu.
		 *\param[in,out]	shadowMaps	Reçoit les shadow maps utilisées par la passe de rendu.
		 *\param[in]		viewport	Le viewport restreignant par la passe de rendu.
		 *\param[in]		scissor		Le scissor restreignant par la passe de rendu.
		 */
		C3D_API void update( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, VkViewport const & viewport
			, VkRect2D const & scissor );
		/**
		 *\~english
		 *\brief			Updates the render nodes.
		 *\param[in,out]	shadowMaps	Receives the shadow maps used in the render pass.
		 *\param[in]		scissor		The scissors restraining the render pass.
		 *\~french
		 *\brief			Met à jour les noeuds de rendu.
		 *\param[in,out]	shadowMaps	Reçoit les shadow maps utilisées par la passe de rendu.
		 *\param[in]		scissor		Le scissor restreignant par la passe de rendu.
		 */
		C3D_API void update( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, VkRect2D const & scissor );
		/**
		 *\~english
		 *\brief		Sets the node to be ignored in rendering.
		 *remarks		All objects attached to this node will be ignored
		 *\~french
		 *\brief		Définit le noeud qui sera ignoré lors du rendu.
		 *remarks		Tous les objets attachés à ce noeud seront ignorés.
		 */
		C3D_API void setIgnoredNode( SceneNode const & node );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool hasNodes()const;
		C3D_API bool needsInitialise()const;
		C3D_API RenderFilters getFilters()const;
		C3D_API ashes::CommandBuffer const & initCommandBuffer();
		C3D_API RenderCounts const & getVisibleCounts()const;

		bool isOutOfDate()const noexcept
		{
			return m_culledChanged || m_commandsChanged;
		}

		bool hasCommandBuffer()const noexcept
		{
			return m_pass.commandBuffer != nullptr;
		}
		
		ashes::CommandBuffer const & getCommandBuffer()const noexcept
		{
			CU_Require( hasCommandBuffer() );
			return *m_pass.commandBuffer;
		}

		QueueRenderNodes & getRenderNodes()const noexcept
		{
			CU_Require( m_renderNodes );
			return *m_renderNodes;
		}

		SceneCuller & getCuller()const noexcept
		{
			return m_culler;
		}

		SceneNode const * getIgnoredNode()const noexcept
		{
			return m_ignoredNode;
		}

		uint32_t getDrawCallsCount()const noexcept
		{
			return m_drawCalls;
		}
		/**@}*/

	private:
		void doInitialise( RenderDevice const & device
			, QueueData const & queueData );
		void doPrepareCommandBuffer();
		void doOnCullerCompute( SceneCuller const & culler );

	private:
		struct PassData
		{
			PassData()noexcept = default;
			PassData( PassData const & ) = delete;
			PassData & operator=( PassData && )noexcept = delete;
			PassData & operator=( PassData const & ) = delete;

			PassData( PassData && rhs )noexcept
				: initEvent{ std::move( rhs.initEvent ) }
				, initialised{ std::move( rhs.initialised ) }
				, commandBuffer{ std::move( rhs.commandBuffer ) }
				, renderPassAtInit{ std::move( rhs.renderPassAtInit ) }
			{
				rhs.initEvent = {};
				rhs.initialised = {};
				rhs.renderPassAtInit = {};
			}

			mutable castor::SpinMutex eventMutex;
			GpuFrameEvent * initEvent{};
			bool initialised{};
			ashes::CommandBufferPtr commandBuffer;
			VkRenderPass renderPassAtInit{};
		};

		SceneCuller & m_culler;
		SceneCullerSignalConnection m_onCullerCompute;
		SceneNode const * m_ignoredNode{ nullptr };
		QueueRenderNodesUPtr m_renderNodes;
		PassData m_pass;
		bool m_culledChanged{};
		bool m_fullSort{ true };
		bool m_commandsChanged{};
		std::atomic_bool m_invalidated{};
		castor::GroupChangeTracked< ashes::Optional< VkViewport > > m_viewport;
		castor::GroupChangeTracked< ashes::Optional< VkRect2D > > m_scissor;
		uint32_t m_drawCalls{};
	};
}

#if defined( CU_CompilerMSVC )
#	pragma warning( pop )
#endif

#endif
