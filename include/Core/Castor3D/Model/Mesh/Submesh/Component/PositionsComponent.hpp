/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PositionsComponent_H___
#define ___C3D_PositionsComponent_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <unordered_map>

namespace castor3d
{
	class PositionsComponent
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
		C3D_API explicit PositionsComponent( Submesh & submesh );
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( ShaderFlags const & shaderFlags
			, SubmeshFlags const & submeshFlags
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
		 *\copydoc		castor3d::SubmeshComponent::getSubmeshFlags
		 */
		SubmeshFlags getSubmeshFlags( Material const & material )const override
		{
			return getDataFlags();
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getDataFlags
		 */
		SubmeshFlags getDataFlags()const override
		{
			return SubmeshFlag::ePositions;
		}

		void setData( std::vector< castor::Point3f > const & data )
		{
			m_data = data;
		}

		std::vector< castor::Point3f > & getData()
		{
			return m_data;
		}

		std::vector< castor::Point3f > const & getData()const
		{
			return m_data;
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr BindingPoint = 0u;

	private:
		std::vector< castor::Point3f > m_data;
		std::unordered_map< uint32_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
	};
}

#endif
