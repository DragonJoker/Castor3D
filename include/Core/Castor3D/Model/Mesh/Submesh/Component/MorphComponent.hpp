/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphComponent_H___
#define ___C3D_MorphComponent_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
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
			, SubmeshFlags const & submeshFlags
			, MaterialRPtr material
			, TextureFlagsArray const & mask
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, uint32_t & currentLocation )override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentSPtr clone( Submesh & submesh )const override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		ProgramFlags getProgramFlags( Material const & material )const override
		{
			return ProgramFlag::eMorphing;
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getSubmeshFlags
		 */
		SubmeshFlags getSubmeshFlags()const override
		{
			return m_flags;
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr PosBindingPoint = 4u;
		C3D_API static uint32_t constexpr NmlBindingPoint = 5u;
		C3D_API static uint32_t constexpr TanBindingPoint = 6u;
		C3D_API static uint32_t constexpr TexBindingPoint = 7u;

	private:
		SubmeshFlags m_flags{};
		std::unordered_map< size_t, std::vector< ashes::PipelineVertexInputStateCreateInfo > > m_layouts;
	};
}

#endif
