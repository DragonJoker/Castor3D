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
		, ObjectBufferOffset const & bufferOffsets
		, PipelineFlags const & flags
		, ashes::BufferCRefArray & buffers
		, castor::Vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation
		, castor::UnorderedMap< size_t, ashes::PipelineVertexInputStateCreateInfo > & cache );
	C3D_API void fillBaseSurfaceType( SubmeshData submeshData
		, sdw::type::IOStruct & type
		, uint32_t & index );
	C3D_API void fillBaseSurfaceType( SubmeshData submeshData
		, sdw::type::BaseStruct & type );
	C3D_API castor::String getBaseDataComponentName( SubmeshData submeshData );

	template< SubmeshData SubmeshDataT, typename DataT >
	class BaseDataComponentT
		: public SubmeshComponent
	{
	public:
		struct SurfaceShader
			: public shader::SubmeshVertexSurfaceShader
		{
			C3D_API void fillSurfaceType( sdw::type::Struct & type
				, uint32_t * index )const override
			{
				if ( index )
				{
					fillBaseSurfaceType( SubmeshDataT, static_cast< sdw::type::IOStruct & >( type ), *index );
				}
				else
				{
					fillBaseSurfaceType( SubmeshDataT, static_cast< sdw::type::BaseStruct & >( type ) );
				}
			}
		};

		struct ComponentData
			: public SubmeshComponentData
		{
			using SubmeshComponentData::SubmeshComponentData;
			/**
			 *\copydoc		castor3d::SubmeshComponentData::gather
			 */
			void gather( PipelineFlags const & flags
				, Pass const & pass
				, ObjectBufferOffset const & bufferOffsets
				, ashes::BufferCRefArray & buffers
				, castor::Vector< uint64_t > & offsets
				, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
				, uint32_t & currentBinding
				, uint32_t & currentLocation )override
			{
				gatherBaseDataBuffer( SubmeshDataT
					, bufferOffsets
					, flags
					, buffers
					, offsets
					, layouts
					, currentBinding
					, currentLocation
					, m_layouts );
			}
			/**
			 *\copydoc		castor3d::SubmeshComponentData::copy
			 */
			void copy( SubmeshComponentDataRPtr data )const override
			{
				static_cast< ComponentData * >( data )->m_data = m_data;
			}

			void setData( castor::Vector< DataT > const & data )
			{
				m_data = data;
				needsUpdate();
			}

			castor::Vector< DataT > & getData()
			{
				needsUpdate();
				return m_data;
			}

			castor::Vector< DataT > const & getData()const
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
				uploadBaseData( SubmeshDataT, m_submesh, m_data, m_up, uploader );
			}

		private:
			castor::Vector< DataT > m_data;
			castor::Point4fArray m_up;
			castor::UnorderedMap< size_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
		};

		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			using SubmeshComponentPlugin::SubmeshComponentPlugin;

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

			shader::SubmeshVertexSurfaceShaderPtr createVertexSurfaceShader()const override
			{
				return castor::make_unique< SurfaceShader >();
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
			: SubmeshComponent{ submesh, TypeName
				, castor::make_unique< ComponentData >( submesh ) }
		{
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		SubmeshComponentUPtr clone( Submesh & submesh )const override
		{
			auto result = castor::makeUnique< BaseDataComponentT >( submesh );
			result->getData().copy( &getData() );
			return castor::ptrRefCast< SubmeshComponent >( result );
		}

		ComponentData & getData()const noexcept
		{
			return *getDataT< ComponentData >();
		}

	public:
		static castor::String const TypeName;
	};

	template< SubmeshData SubmeshDataT, typename DataT >
	castor::String const BaseDataComponentT< SubmeshDataT, DataT >::TypeName{ []()
		{
			return getBaseDataComponentName( SubmeshDataT );
		}() };
}

#endif
