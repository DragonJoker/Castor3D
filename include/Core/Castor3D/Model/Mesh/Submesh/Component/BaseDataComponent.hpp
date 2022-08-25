/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BaseDataComponent_H___
#define ___C3D_BaseDataComponent_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <unordered_map>

namespace castor3d
{
	C3D_API void uploadBaseData( SubmeshFlag submeshData
		, Submesh const & submesh
		, castor::Point3fArray const & data );
	C3D_API void gatherBaseDataBuffer( SubmeshFlag submeshData
		, PipelineFlags const & flags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation
		, std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > & cache );

	template< SubmeshFlag SubmeshFlagT >
	class BaseDataComponentT
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
		explicit BaseDataComponentT( Submesh & submesh )
			: SubmeshComponent{ submesh, Name, Id }
		{
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		void gather( PipelineFlags const & flags
			, MaterialRPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, uint32_t & currentBinding
			, uint32_t & currentLocation )override
		{
			gatherBaseDataBuffer( SubmeshFlagT
				, flags
				, layouts
				, currentBinding
				, currentLocation
				, m_layouts );
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		SubmeshComponentSPtr clone( Submesh & submesh )const override
		{
			auto result = std::make_shared< BaseDataComponentT >( submesh );
			result->m_data = m_data;
			return std::static_pointer_cast< SubmeshComponent >( result );
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getSubmeshFlags
		 */
		SubmeshFlags getSubmeshFlags( Pass const * pass )const override
		{
			return SubmeshFlagT;
		}

		void setData( castor::Point3fArray const & data )
		{
			m_data = data;
		}

		castor::Point3fArray & getData()
		{
			return m_data;
		}

		castor::Point3fArray const & getData()const
		{
			return m_data;
		}

	private:
		bool doInitialise( RenderDevice const & device )override
		{
			return true;
		}

		void doCleanup( RenderDevice const & device )override
		{
			m_data.clear();
		}

		void doUpload()override
		{
			uploadBaseData( SubmeshFlagT , *getOwner(), m_data );
		}

	public:
		static uint32_t constexpr Id = getIndex( SubmeshFlagT ) - 1u;
		static castor::String const Name;


	private:
		castor::Point3fArray m_data;
		std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
	};

	template< SubmeshFlag SubmeshFlagT >
	castor::String const BaseDataComponentT< SubmeshFlagT >::Name{ []()
		{
			return castor3d::getName( castor3d::getData( SubmeshFlagT ) );
		}() };
}

#endif
