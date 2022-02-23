/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SecondaryUVComponent_H___
#define ___C3D_SecondaryUVComponent_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <unordered_map>

namespace castor3d
{
	class SecondaryUVComponent
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
		C3D_API explicit SecondaryUVComponent( Submesh & submesh );
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( ShaderFlags const & shaderFlags
			, ProgramFlags const & programFlags
			, MaterialRPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, TextureFlagsArray const & mask
			, uint32_t & currentLocation )override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentSPtr clone( Submesh & submesh )const override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		ProgramFlags getProgramFlags( MaterialRPtr material )const override
		{
			return ProgramFlag::eSecondaryUV;
		}

		C3D_API void addTexcoords( std::vector< castor::Point3f > const & uvs );

		GpuBufferOffsetT< castor::Point3f > const & getAnimationBuffer()const
		{
			return m_buffer;
		}

		GpuBufferOffsetT< castor::Point3f > & getAnimationBuffer()
		{
			return m_buffer;
		}

		std::vector< castor::Point3f > & getData()
		{
			return m_data;
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr BindingPoint = 4u;

	private:
		GpuBufferOffsetT< castor::Point3f > m_buffer;
		std::vector< castor::Point3f > m_data;
		std::unordered_map< uint32_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
	};
}

#endif
