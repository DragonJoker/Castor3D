/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphComponent_H___
#define ___C3D_MorphComponent_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <unordered_map>

namespace castor3d
{
	class MorphComponent
		: public SubmeshComponent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		C3D_API explicit MorphComponent( Submesh & submesh );
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( ShaderFlags const & shaderFlags
			, ProgramFlags const & programFlags
			, MaterialRPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, uint32_t instanceMult
			, TextureFlagsArray const & mask
			, uint32_t & currentLocation )override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentSPtr clone( Submesh & submesh )const override;

		GpuBufferOffsetT< InterleavedVertex > const & getAnimationBuffer()const
		{
			return m_animBuffer;
		}

		GpuBufferOffsetT< InterleavedVertex > & getAnimationBuffer()
		{
			return m_animBuffer;
		}

		ashes::StagingBuffer & getStagingBuffer()const
		{
			return *m_stagingBuffer;
		}

		ashes::CommandBuffer & getCommandBuffer()const
		{
			return *m_commandBuffer;
		}

		InterleavedVertexArray & getData()
		{
			return m_data;
		}

		ProgramFlags getProgramFlags( MaterialRPtr material )const override
		{
			return ProgramFlag::eMorphing;
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr BindingPoint = 1u;

	private:
		GpuBufferOffsetT< InterleavedVertex > m_animBuffer;
		std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > m_animLayouts;
		InterleavedVertexArray m_data;
		ashes::StagingBufferPtr m_stagingBuffer;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::FencePtr m_fence;
	};
}

#endif
