/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BaseDataComponent_H___
#define ___C3D_BaseDataComponent_H___

#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <unordered_map>

namespace c3d
{
	C3D_API void uploadBaseData( SubmeshData submeshData
		, Submesh const & submesh
		, Point4fArray const & data
		, Point4fArray const & up
		, UploadData & uploader );
	C3D_API void uploadBaseData( SubmeshData submeshData
		, Submesh const & submesh
		, Point3fArray const & data
		, Point4fArray & up
		, UploadData & uploader );
	C3D_API void gatherBaseDataBuffer( SubmeshData submeshData
		, ObjectBufferOffset const & bufferOffsets
		, PipelineFlags const & flags
		, ashes::BufferCRefArray & buffers
		, Vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation
		, HashMap< size_t, ashes::PipelineVertexInputStateCreateInfo > & cache );
	C3D_API void fillBaseSurfaceType( SubmeshData submeshData
		, sdw::type::IOStruct & type
		, uint32_t & index );
	C3D_API void fillBaseSurfaceType( SubmeshData submeshData
		, sdw::type::BaseStruct & type );
	C3D_API String getBaseDataComponentName( SubmeshData submeshData );
	C3D_API void createBaseDataParsers( SubmeshData submeshData
		, AttributeParsers & result );
	C3D_API void createBaseDataSections( SubmeshData submeshData
		, StrSectionIdMap & result );
	C3D_API bool writeBaseDataText( SubmeshData submeshData
		, Point3fArray const & data
		, String const & tabs
		, StringStream & file );
	C3D_API bool writeBaseDataText( SubmeshData submeshData
		, Point4fArray const & data
		, String const & tabs
		, StringStream & file );
	C3D_API bool writeBaseDataBinary( SubmeshData submeshData
		, Point3fArray const & data
		, BinaryChunk & chunk );
	C3D_API bool writeBaseDataBinary( SubmeshData submeshData
		, Point4fArray const & data
		, BinaryChunk & chunk );
	C3D_API void acceptBaseData( ConfigurationVisitorBase & vis
		, c3d::String const & name
		, size_t count );

	template< SubmeshData SubmeshDataT, typename DataT >
	class BaseDataComponentT
		: public SubmeshComponent
	{
	public:
		static constexpr SubmeshData MySubmeshData = SubmeshDataT;
		using Data = DataT;

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
			 *\copydoc		SubmeshComponentData::gather
			 */
			void gather( PipelineFlags const & flags
				, Pass const & pass
				, ObjectBufferOffset const & bufferOffsets
				, ashes::BufferCRefArray & buffers
				, Vector< uint64_t > & offsets
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
			 *\copydoc		SubmeshComponentData::copy
			 */
			void copy( SubmeshComponentDataRPtr data )const override
			{
				static_cast< ComponentData * >( data )->m_data = m_data;
			}

			void setData( Vector< DataT > const & data )
			{
				m_data = data;
				needsUpdate();
			}

			Vector< DataT > & getData()
			{
				needsUpdate();
				return m_data;
			}

			Vector< DataT > const & getData()const
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
			Vector< DataT > m_data;
			Point4fArray m_up;
			HashMap< size_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
		};

		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			using SubmeshComponentPlugin::SubmeshComponentPlugin;

			void createParsers( AttributeParsers & result )const override
			{
				createBaseDataParsers( SubmeshDataT, result );
			}

			void createSections( StrSectionIdMap & sections )const override
			{
				createBaseDataSections( SubmeshDataT, sections );
			}

			SubmeshComponentUPtr createComponent( Submesh & submesh )const override
			{
				return makeUniqueDerived< SubmeshComponent, BaseDataComponentT< SubmeshDataT, DataT > >( submesh );
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
				return makeRawUnique< SurfaceShader >();
			}
		};

		static SubmeshComponentPluginUPtr createPlugin( SubmeshComponentRegister const & submeshComponents )
		{
			return makeUniqueDerived< SubmeshComponentPlugin, Plugin >( submeshComponents );
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
				, makeRawUnique< ComponentData >( submesh ) }
		{
		}
		/**
		 *\copydoc		SubmeshComponent::clone
		 */
		SubmeshComponentUPtr clone( Submesh & submesh )const override
		{
			auto result = makeUnique< BaseDataComponentT >( submesh );
			getData().copy( &result->getData() );
			return ptrRefCast< SubmeshComponent >( result );
		}

		ComponentData & getData()const noexcept
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
			return *getDataT< ComponentData >();
#pragma GCC diagnostic pop
		}

		void accept( ConfigurationVisitorBase & vis )override
		{
			acceptBaseData( vis, TypeName, getData().getData().size() );
		}

	private:
		bool doWriteText( String const & tabs
			, StringStream & file )const override
		{
			return writeBaseDataText( SubmeshDataT, getData().getData(), tabs, file );
		}

		bool doWriteBinary( BinaryChunk & chunk )const override
		{
			return writeBaseDataBinary( SubmeshDataT, getData().getData(), chunk );
		}

	public:
		static String const TypeName;
	};

	template< SubmeshData SubmeshDataT, typename DataT >
	String const BaseDataComponentT< SubmeshDataT, DataT >::TypeName{ []()
		{
			return getBaseDataComponentName( SubmeshDataT );
		}() };
}

#endif
