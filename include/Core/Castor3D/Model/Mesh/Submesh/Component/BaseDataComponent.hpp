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
	C3D_API void uploadBaseData( SubmeshData submeshData
		, Submesh const & submesh
		, castor::Point4fArray const & data
		, castor::Point4fArray & up
		, UploadData & uploader );
	C3D_API void uploadBaseData( SubmeshData submeshData
		, Submesh const & submesh
		, castor::Point3fArray const & data
		, castor::Point4fArray & up
		, UploadData & uploader );
	C3D_API void gatherBaseDataBuffer( SubmeshData submeshData
		, PipelineFlags const & flags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation
		, std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > & cache );

	template< SubmeshData SubmeshDataT, typename DataT >
	class BaseDataComponentT
		: public SubmeshComponent
	{
	public:
		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			explicit Plugin( SubmeshComponentRegister const & submeshComponents )
				: SubmeshComponentPlugin{ submeshComponents }
			{
			}

			SubmeshComponentUPtr createComponent( Submesh & submesh )const override
			{
				return castor::makeUniqueDerived< SubmeshComponent, BaseDataComponentT< SubmeshDataT, DataT > >( submesh );
			}

			SubmeshComponentFlag getPositionFlag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::ePositions ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getNormalFlag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eNormals ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getTangentFlag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eTangents ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getBitangentFlag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eBitangents ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getTexcoord0Flag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eTexcoords0 ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getTexcoord1Flag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eTexcoords1 ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getTexcoord2Flag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eTexcoords2 ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getTexcoord3Flag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eTexcoords3 ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getColourFlag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eColours ? getComponentFlags() : 0u;
			}

			SubmeshComponentFlag getVelocityFlag()const noexcept override
			{
				return SubmeshDataT == SubmeshData::eVelocity ? getComponentFlags() : 0u;
			}
		};

		static SubmeshComponentPluginUPtr createPlugin( SubmeshComponentRegister const & submeshComponents )
		{
			return castor::makeUniqueDerived< SubmeshComponentPlugin, Plugin >( submeshComponents );
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		explicit BaseDataComponentT( Submesh & submesh )
			: SubmeshComponent{ submesh, TypeName }
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
			gatherBaseDataBuffer( SubmeshDataT
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
			uploadBaseData( SubmeshDataT , *getOwner(), m_data, m_up, uploader );
		}

	public:
		static castor::String const TypeName;


	private:
		std::vector< DataT > m_data;
		castor::Point4fArray m_up;
		std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
	};

	template< SubmeshData SubmeshDataT, typename DataT >
	castor::String const BaseDataComponentT< SubmeshDataT, DataT >::TypeName{ []()
		{
			return castor3d::getName( SubmeshDataT );
		}() };
}

#endif
