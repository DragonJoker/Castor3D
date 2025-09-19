#include "Castor3D/Model/Mesh/Submesh/Component/LineMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Binary/ChunkWriter.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, LineMapping )

namespace c3d
{
	//*********************************************************************************************

	template<>
	class TextWriter< LineMapping >
		: public TextWriterT< LineMapping >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< LineMapping >{ tabs }
		{
		}

		bool operator()( LineMapping const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, cuT( "lines" ) ) } )
			{
				result = true;
				for ( auto & value : object.getData().getFaces() )
				{
					StringStream stream;
					stream << value[0] << " " << value[1];
					result = result && write( file, cuT( "value " ), stream.str() );
				}
			}

			return result;
		}
	};

	//*********************************************************************************************

	namespace smshcompline
	{
		static constexpr SectionId sectionId = makeSectionName( 'S', 'M', 'S', 'H', 'L', 'N', 'M', 'P' );

		struct LineDistance
		{
			Array< uint32_t, 2u > m_index;
			double m_distance;
		};

		CU_DeclareVector( LineDistance, LineDist );

		struct Compare
		{
			bool operator()( LineDistance const & lhs
				, LineDistance const & rhs )const
			{
				return lhs.m_distance < rhs.m_distance;
			}
		};

		struct LineMappingContext
		{
			SubmeshContext * submesh;
			Vector< LineIndices > values;
		};

		static CU_ImplementAttributeParserNewBlock( parserSection, SubmeshContext, LineMappingContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
			{
				newBlockContext->submesh = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( sectionId )

		static CU_ImplementAttributeParserBlock( parserValue, LineMappingContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
			{
				Point2ui indices;
				params[0]->get( indices );
				auto & face = blockContext->values.emplace_back();
				face[0] = indices[0];
				face[1] = indices[1];
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLine, SubmeshContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( auto component = blockContext->submesh->createComponent< LineMapping >() )
			{
				Point2ui indices;
				params[0]->get( indices );
				component->getData().getFaces().emplace_back( indices[0], indices[1] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, LineMappingContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( !blockContext->values.empty() )
			{
				if ( auto component = blockContext->submesh->submesh->createComponent< LineMapping >() )
					component->getData().addLineGroup( blockContext->values );
			}
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	void LineMapping::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
		auto result = static_cast< ComponentData * >( data );
		result->m_lines = m_lines;
		result->m_cameraPosition = m_cameraPosition;
	}

	Line LineMapping::ComponentData::addLine( uint32_t a, uint32_t b )
	{
		Line result{ a, b };

		if ( auto size = m_submesh.getPointsCount();
			a < size && b < size )
		{
			m_lines.push_back( result );
		}
		else
		{
			throw std::range_error( "addLine - One or more index out of bound" );
		}

		return result;
	}

	void LineMapping::ComponentData::addLineGroup( LineIndices const * const begin
		, LineIndices const * const end )
	{
		for ( auto & line : makeArrayView( begin, end ) )
		{
			addLine( line.m_index[0], line.m_index[1] );
		}
	}

	void LineMapping::ComponentData::clearLines()
	{
		m_lines.clear();
	}

	void LineMapping::ComponentData::doCleanup( RenderDevice const & device )
	{
		m_lines.clear();
	}

	void LineMapping::ComponentData::doUpload( UploadData & uploader )
	{
		auto count = uint32_t( m_lines.size() * 2 );
		auto & offsets = m_submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshData::eIndex );

		if ( count && buffer.hasData() )
		{
			uploader.pushUpload( m_lines.data()
				, m_lines.size() * sizeof( Line )
				, buffer.getBuffer(), buffer.getOffset()
				, VertexIndexInputState );
		}
	}

	//*********************************************************************************************

	String const LineMapping::TypeName = C3D_MakeSubmeshIndexComponentName( "lines" );

	LineMapping::LineMapping( Submesh & submesh
		, BufferUsageFlags bufferUsageFlags )
		: IndexMapping{ submesh, TypeName
			, makeRawUnique< ComponentData >( submesh, bufferUsageFlags ) }
	{
	}

	void LineMapping::computeNormals( bool reverted )
	{
	}

	void LineMapping::computeTangents()
	{
	}

	SubmeshComponentUPtr LineMapping::clone( Submesh & submesh )const
	{
		auto result = makeUnique< LineMapping >( submesh );
		getData().copy( &result->getData() );
		return ptrRefCast< SubmeshComponent >( result );
	}

	uint32_t LineMapping::getCount()const
	{
		return getDataT< ComponentData >()->getCount();
	}

	void LineMapping::setCount( uint32_t value )
	{
		getDataT< ComponentData >()->setCount( value );
	}

	uint32_t LineMapping::getComponentsCount()const
	{
		return 2u;
	}

	bool LineMapping::doWriteText( String const & tabs
		, StringStream & file )const
	{
		return TextWriter< LineMapping >{ tabs }( *this, file );
	}

	bool LineMapping::doWriteBinary( BinaryChunk & chunk )const
	{
		auto count = getData().getCount();
		auto result = ChunkWriter< u32 >::write( 2u, ChunkType::eSubmeshIndexComponentCount, chunk )
			&& ChunkWriter< u32 >::write( count, ChunkType::eSubmeshIndexCount, chunk );

		if ( result )
		{
			auto & lines = getData().getFaces();
			auto const * data = std::bit_cast< LineIndices const * >( lines.data() );
			result = ChunkWriter< LineIndices >::write( data, data + count, ChunkType::eSubmeshIndices, chunk );
		}

		return result;
	}

	//*********************************************************************************************

	void LineMapping::Plugin::createParsers( AttributeParsers & result )const
	{
		BlockParserContextT< SubmeshContext > submeshContext{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };
		BlockParserContextT< smshcompline::LineMappingContext > sectionContext{ result, smshcompline::sectionId, CSCNSection::eSubmesh };

		submeshContext.addParser( "line", smshcompline::parserLine, { makeParameter< ParameterType::ePoint2U >() } );
		submeshContext.addPushParser( "lines", smshcompline::sectionId, smshcompline::parserSection );

		sectionContext.addParser( cuT( "value" ), smshcompline::parserValue, { makeParameter< ParameterType::ePoint2U >() } );
		sectionContext.addPopParser( cuT( "}" ), smshcompline::parserEnd );
	}

	void LineMapping::Plugin::createSections( StrSectionIdMap & sections )const
	{
		sections.try_emplace( SectionId( smshcompline::sectionId ), "lines" );
	}

	//*********************************************************************************************
}
