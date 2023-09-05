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
		, castor::Point4fArray const & data
		, castor::Point4fArray & up
		, UploadData & uploader );
	C3D_API void uploadBaseData( SubmeshFlag submeshData
		, Submesh const & submesh
		, castor::Point3fArray const & data
		, castor::Point4fArray & up
		, UploadData & uploader );
	C3D_API void gatherBaseDataBuffer( SubmeshFlag submeshData
		, PipelineFlags const & flags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation
		, std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > & cache );

	template< SubmeshFlag SubmeshFlagT, typename DataT >
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
			, MaterialObs material
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
		SubmeshComponentUPtr clone( Submesh & submesh )const override
		{
			auto result = castor::makeUnique< BaseDataComponentT >( submesh );
			result->m_data = m_data;
			return castor::ptrRefCast< SubmeshComponent >( result );
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getSubmeshFlags
		 */
		SubmeshFlags getSubmeshFlags( Pass const * pass )const override
		{
			return SubmeshFlagT;
		}

		void setData( std::vector< DataT > const & data )
		{
			m_data = data;
		}

		std::vector< DataT > & getData()
		{
			return m_data;
		}

		std::vector< DataT > const & getData()const
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

		void doUpload( UploadData & uploader )override
		{
			uploadBaseData( SubmeshFlagT , *getOwner(), m_data, m_up, uploader );
		}

	public:
		static uint32_t constexpr Id = getIndex( SubmeshFlagT ) - 1u;
		static castor::String const Name;


	private:
		std::vector< DataT > m_data;
		castor::Point4fArray m_up;
		std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
	};

	template< SubmeshFlag SubmeshFlagT, typename DataT >
	castor::String const BaseDataComponentT< SubmeshFlagT, DataT >::Name{ []()
		{
			return castor3d::getName( castor3d::getData( SubmeshFlagT ) );
		}() };
}

#endif
