#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Binary/BinaryMorphComponent.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

CU_ImplementSmartPtr( c3d, MorphComponent )

namespace c3d
{
	//*********************************************************************************************

	template<>
	class TextWriter< MorphComponent >
		: public TextWriterT< MorphComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< MorphComponent >{ tabs }
		{
		}

		using TextWriterT< MorphComponent >::write;

		bool write( StringStream & file
			, String const & name
			, Point3fArray const & data )
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = true;
				for ( auto & value : data )
					result = result && write( file, cuT( "value" ), value );
			}

			return result;
		}

		bool write( StringStream & file
			, String const & name
			, Point4fArray const & data )
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = true;
				for ( auto & value : data )
					result = result && write( file, cuT( "value" ), value );
			}

			return result;
		}

		bool operator()( MorphComponent const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, cuT( "morph_targets" ) ) } )
			{
				result = true;

				for ( auto const & it : object.getData().getMorphTargetsBuffers() )
				{
					result = false;
					if ( auto bufferBlock{ beginBlock( file, cuT( "buffer" ) ) } )
					{
						result = true;
						if ( result && !it.positions.empty() )
							result = write( file, cuT( "positions" ), it.positions );
						if ( result && !it.normals.empty() )
							result = write( file, cuT( "normals" ), it.normals );
						if ( result && !it.tangents.empty() )
							result = write( file, cuT( "tangents" ), it.tangents );
						if ( result && !it.bitangents.empty() )
							result = write( file, cuT( "bitangents" ), it.bitangents );
						if ( result && !it.texcoords0.empty() )
							result = write( file, cuT( "texcoords0" ), it.texcoords0 );
						if ( result && !it.texcoords1.empty() )
							result = write( file, cuT( "texcoords1" ), it.texcoords1 );
						if ( result && !it.texcoords2.empty() )
							result = write( file, cuT( "texcoords2" ), it.texcoords2 );
						if ( result && !it.texcoords3.empty() )
							result = write( file, cuT( "texcoords3" ), it.texcoords3 );
						if ( result && !it.colours.empty() )
							result = write( file, cuT( "colours" ), it.colours );
					}
				}
			}

			return result;
		}
	};

	//*********************************************************************************************

	namespace smshcompmorph
	{
		enum class Section
			: SectionId
		{
			eMorph = makeSectionName( 'S', 'M', 'S', 'H', 'M', 'R', 'P', 'H' ),
			eBuffer = makeSectionName( 'S', 'M', 'H', 'M', 'P', 'B', 'U', 'F' ),
			ePositions = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'P', 'O' ),
			eNormals = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'N', 'L' ),
			eTangents = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'T', 'G' ),
			eBitangents = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'B', 'T' ),
			eTexcoords0 = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'T', '0' ),
			eTexcoords1 = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'T', '1' ),
			eTexcoords2 = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'T', '2' ),
			eTexcoords3 = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'T', '3' ),
			eColours = makeSectionName( 'S', 'H', 'M', 'P', 'B', 'F', 'C', 'L' ),
		};


		static void computeBoundingBox( SubmeshAnimationBuffer & buffer )
		{
			if ( buffer.positions.empty() )
			{
				return;
			}

			auto & points = buffer.positions;
			Point3f min{ points[0] };
			Point3f max{ points[0] };

			if ( points.size() > 1 )
			{
				for ( auto const & vertex : makeArrayView( &points[1], points.data() + points.size() ) )
				{
					Point3f cur{ vertex };
					max[0] = std::max( cur[0], max[0] );
					max[1] = std::max( cur[1], max[1] );
					max[2] = std::max( cur[2], max[2] );
					min[0] = std::min( cur[0], min[0] );
					min[1] = std::min( cur[1], min[1] );
					min[2] = std::min( cur[2], min[2] );
				}
			}

			buffer.boundingBox.load( min, max );
		}

		static Pair< MorphFlags, uint32_t > computeMorphFlags( SubmeshAnimationBuffer const & buffer )
		{
			MorphFlags flags{};
			uint32_t count{};

			if ( !buffer.positions.empty() )
			{
				flags |= MorphFlag::ePositions;
				++count;
			}

			if ( !buffer.normals.empty() )
			{
				flags |= MorphFlag::eNormals;
				++count;
			}

			if ( !buffer.tangents.empty() )
			{
				flags |= MorphFlag::eTangents;
				++count;
			}

			if ( !buffer.bitangents.empty() )
			{
				flags |= MorphFlag::eBitangents;
				++count;
			}

			if ( !buffer.texcoords0.empty() )
			{
				flags |= MorphFlag::eTexcoords0;
				++count;
			}

			if ( !buffer.texcoords1.empty() )
			{
				flags |= MorphFlag::eTexcoords1;
				++count;
			}

			if ( !buffer.texcoords2.empty() )
			{
				flags |= MorphFlag::eTexcoords2;
				++count;
			}

			if ( !buffer.texcoords3.empty() )
			{
				flags |= MorphFlag::eTexcoords3;
				++count;
			}

			if ( !buffer.colours.empty() )
			{
				flags |= MorphFlag::eColours;
				++count;
			}

			return { flags, count };
		}

		struct MorphContext
		{
			SubmeshContext * submesh;
			MorphComponentUPtr component;
			SubmeshAnimationBuffer buffer;
		};

		static CU_ImplementAttributeParserNewBlock( parserMorph, SubmeshContext, MorphContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
			{
				newBlockContext->submesh = blockContext;
				newBlockContext->component = makeUnique< MorphComponent >( *blockContext->submesh );
			}
		}
		CU_EndAttributePushNewBlock( Section::eMorph )

			static CU_ImplementAttributeParserBlock( parserBuffer, MorphContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
				blockContext->buffer = {};
		}
		CU_EndAttributePushBlock( Section::eBuffer, blockContext )

		static CU_ImplementAttributeParserBlock( parserPositions, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::ePositions, blockContext )

		static CU_ImplementAttributeParserBlock( parserPosition, MorphContext )
		{
			params[0]->get( blockContext->buffer.positions.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPositionsEnd, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserNormals, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::eNormals, blockContext )

		static CU_ImplementAttributeParserBlock( parserNormal, MorphContext )
		{
			params[0]->get( blockContext->buffer.normals.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNormalsEnd, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserTangents, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::eTangents, blockContext )

		static CU_ImplementAttributeParserBlock( parserTangent, MorphContext )
		{
			params[0]->get( blockContext->buffer.tangents.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTangentsEnd, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserBitangents, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::eBitangents, blockContext )

		static CU_ImplementAttributeParserBlock( parserBitangent, MorphContext )
		{
			params[0]->get( blockContext->buffer.bitangents.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBitangentsEnd, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserTexcoords0, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::eTexcoords0, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexcoord0, MorphContext )
		{
			params[0]->get( blockContext->buffer.texcoords0.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexcoords0End, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserTexcoords1, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::eTexcoords1, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexcoord1, MorphContext )
		{
			params[0]->get( blockContext->buffer.texcoords1.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexcoords1End, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserTexcoords2, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::eTexcoords2, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexcoord2, MorphContext )
		{
			params[0]->get( blockContext->buffer.texcoords2.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexcoords2End, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserTexcoords3, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::eTexcoords3, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexcoord3, MorphContext )
		{
			params[0]->get( blockContext->buffer.texcoords3.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexcoords3End, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserColours, MorphContext )
		{
			if ( !blockContext->component )
				CU_ParsingError( cuT( "No component initialised." ) );
		}
		CU_EndAttributePushBlock( Section::eColours, blockContext )

		static CU_ImplementAttributeParserBlock( parserColour, MorphContext )
		{
			params[0]->get( blockContext->buffer.colours.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserColoursEnd, MorphContext )
		{
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserBufferEnd, MorphContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
				blockContext->component->getData().addMorphTarget( move( blockContext->buffer ) );
			blockContext->buffer = {};
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserEnd, MorphContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
				blockContext->submesh->submesh->addComponent( ptrRefCast< SubmeshComponent >( blockContext->component ) );
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	void MorphComponent::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
		auto result = static_cast< ComponentData * >( data );
		result->m_flags = m_flags;
		result->m_targetDataCount = m_targetDataCount;
		result->m_targets = m_targets;
	}

	void MorphComponent::ComponentData::addMorphTarget( SubmeshAnimationBuffer data )
	{
		if ( m_flags == MorphFlags{} )
		{
			auto [flags, count] = smshcompmorph::computeMorphFlags( data );
			m_flags = flags;
			m_targetDataCount = count;
		}

		CU_Require( smshcompmorph::computeMorphFlags( data ).first == m_flags );
		smshcompmorph::computeBoundingBox( data );
		m_targets.emplace_back( c3d::move( data ) );
	}

	bool MorphComponent::ComponentData::doInitialise( RenderDevice const & device )
	{
		auto vertexCount = m_submesh.getPointsCount();

		if ( auto size = m_targetDataCount * vertexCount * MaxMorphTargets;
			!m_buffer || size > m_buffer.getCount() )
		{
			m_buffer = device.bufferPool->getBuffer< Point4f >( BufferUsageFlags::eStorageBuffer
				, size
				, MemoryPropertyFlags::eDeviceLocal );
		}

		return true;
	}

	void MorphComponent::ComponentData::doCleanup( RenderDevice const & device )
	{
		if ( m_buffer )
		{
			device.bufferPool->putBuffer( m_buffer );
			m_buffer = {};
		}
	}

	void MorphComponent::ComponentData::doUpload( UploadData & uploader )
	{
		if ( !m_buffer )
		{
			return;
		}

		uint32_t index{};
		auto vertexCount = m_submesh.getPointsCount();
		auto bufferIt = m_buffer.getData().begin();
		auto stride = m_targetDataCount * MaxMorphTargets;

		while ( index < vertexCount )
		{
			auto bufIt = bufferIt;

			for ( auto & target : m_targets )
			{
				if ( auto posIt = target.positions.begin();
					posIt != target.positions.end() )
				{
					posIt += index;
					*bufIt = Point4f{ *posIt };
					++bufIt;
				}

				if ( auto nmlIt = target.normals.begin();
					nmlIt != target.normals.end() )
				{
					nmlIt += index;
					*bufIt = Point4f{ *nmlIt };
					++bufIt;
				}

				if ( auto tanIt = target.tangents.begin();
					tanIt != target.tangents.end() )
				{
					tanIt += index;
					*bufIt = Point4f{ *tanIt };
					++bufIt;
				}

				if ( auto binIt = target.bitangents.begin();
					binIt != target.bitangents.end() )
				{
					binIt += index;
					*bufIt = Point4f{ *binIt };
					++bufIt;
				}

				if ( auto tx0It = target.texcoords0.begin();
					tx0It != target.texcoords0.end() )
				{
					tx0It += index;
					*bufIt = Point4f{ *tx0It };
					++bufIt;
				}

				if ( auto tx1It = target.texcoords1.begin();
					tx1It != target.texcoords1.end() )
				{
					tx1It += index;
					*bufIt = Point4f{ *tx1It };
					++bufIt;
				}

				if ( auto tx2It = target.texcoords2.begin();
					tx2It != target.texcoords2.end() )
				{
					tx2It += index;
					*bufIt = Point4f{ *tx2It };
					++bufIt;
				}

				if ( auto tx3It = target.texcoords3.begin();
					tx3It != target.texcoords3.end() )
				{
					tx3It += index;
					*bufIt = Point4f{ *tx3It };
					++bufIt;
				}

				if ( auto colIt = target.colours.begin();
					colIt != target.colours.end() )
				{
					colIt += index;
					*bufIt = Point4f{ *colIt };
					++bufIt;
				}
			}

			bufferIt += stride;
			++index;
		}

		m_buffer.upload( uploader, VertexShaderReadState );
	}

	//*********************************************************************************************

	String const MorphComponent::TypeName = C3D_MakeSubmeshComponentName( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName
			, makeRawUnique< ComponentData >( submesh ) }
	{
	}

	SubmeshComponentUPtr MorphComponent::clone( Submesh & submesh )const
	{
		auto result = makeUnique< MorphComponent >( submesh );
		getData().copy( &result->getData() );
		return ptrRefCast< SubmeshComponent >( result );
	}

	bool MorphComponent::doWriteText( String const & tabs
		, StringStream & file )const
	{
		return TextWriter< MorphComponent >{ tabs }( *this, file );
	}

	bool MorphComponent::doWriteBinary( BinaryChunk & chunk )const
	{
		return BinaryWriter< MorphComponent >{}.write( *this, chunk );
	}

	//*********************************************************************************************

	void MorphComponent::Plugin::createParsers( AttributeParsers & result )const
	{
		BlockParserContextT< SubmeshContext > submeshContext{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };
		BlockParserContextT< smshcompmorph::MorphContext > morphContext{ result, smshcompmorph::Section::eMorph, CSCNSection::eSubmesh };
		BlockParserContextT< smshcompmorph::MorphContext > bufferContext{ result, smshcompmorph::Section::eBuffer, smshcompmorph::Section::eMorph };
		BlockParserContextT< smshcompmorph::MorphContext > positionsContext{ result, smshcompmorph::Section::ePositions, smshcompmorph::Section::eBuffer };
		BlockParserContextT< smshcompmorph::MorphContext > normalsContext{ result, smshcompmorph::Section::eNormals, smshcompmorph::Section::eBuffer };
		BlockParserContextT< smshcompmorph::MorphContext > tangentsContext{ result, smshcompmorph::Section::eTangents, smshcompmorph::Section::eBuffer };
		BlockParserContextT< smshcompmorph::MorphContext > bitangentsContext{ result, smshcompmorph::Section::eBitangents, smshcompmorph::Section::eBuffer };
		BlockParserContextT< smshcompmorph::MorphContext > texcoords0Context{ result, smshcompmorph::Section::eTexcoords0, smshcompmorph::Section::eBuffer };
		BlockParserContextT< smshcompmorph::MorphContext > texcoords1Context{ result, smshcompmorph::Section::eTexcoords1, smshcompmorph::Section::eBuffer };
		BlockParserContextT< smshcompmorph::MorphContext > texcoords2Context{ result, smshcompmorph::Section::eTexcoords2, smshcompmorph::Section::eBuffer };
		BlockParserContextT< smshcompmorph::MorphContext > texcoords3Context{ result, smshcompmorph::Section::eTexcoords3, smshcompmorph::Section::eBuffer };
		BlockParserContextT< smshcompmorph::MorphContext > coloursContext{ result, smshcompmorph::Section::eColours, smshcompmorph::Section::eBuffer };

		submeshContext.addPushParser( "morph_targets", smshcompmorph::Section::eMorph, smshcompmorph::parserMorph );

		morphContext.addPushParser( cuT( "buffer" ), smshcompmorph::Section::eBuffer, smshcompmorph::parserBuffer );
		morphContext.addPopParser( cuT( "}" ), smshcompmorph::parserEnd );

		bufferContext.addPushParser( cuT( "positions" ), smshcompmorph::Section::ePositions, smshcompmorph::parserPositions );
		bufferContext.addPushParser( cuT( "normals" ), smshcompmorph::Section::eNormals, smshcompmorph::parserNormals );
		bufferContext.addPushParser( cuT( "tangents" ), smshcompmorph::Section::eTangents, smshcompmorph::parserTangents );
		bufferContext.addPushParser( cuT( "bitangents" ), smshcompmorph::Section::eBitangents, smshcompmorph::parserBitangents );
		bufferContext.addPushParser( cuT( "texcoords0" ), smshcompmorph::Section::eTexcoords0, smshcompmorph::parserTexcoords0 );
		bufferContext.addPushParser( cuT( "texcoords1" ), smshcompmorph::Section::eTexcoords1, smshcompmorph::parserTexcoords1 );
		bufferContext.addPushParser( cuT( "texcoords2" ), smshcompmorph::Section::eTexcoords2, smshcompmorph::parserTexcoords2 );
		bufferContext.addPushParser( cuT( "texcoords3" ), smshcompmorph::Section::eTexcoords3, smshcompmorph::parserTexcoords3 );
		bufferContext.addPushParser( cuT( "colours" ), smshcompmorph::Section::eColours, smshcompmorph::parserColours );
		bufferContext.addPopParser( cuT( "}" ), smshcompmorph::parserBufferEnd );

		positionsContext.addParser( cuT( "value" ), smshcompmorph::parserPosition, { makeParameter< ParameterType::ePoint3F >() } );
		positionsContext.addPopParser( cuT( "}" ), smshcompmorph::parserPositionsEnd );

		normalsContext.addParser( cuT( "value" ), smshcompmorph::parserNormal, { makeParameter< ParameterType::ePoint3F >() } );
		normalsContext.addPopParser( cuT( "}" ), smshcompmorph::parserNormalsEnd );

		tangentsContext.addParser( cuT( "value" ), smshcompmorph::parserTangent, { makeParameter< ParameterType::ePoint4F >() } );
		tangentsContext.addPopParser( cuT( "}" ), smshcompmorph::parserTangentsEnd );

		bitangentsContext.addParser( cuT( "value" ), smshcompmorph::parserBitangent, { makeParameter< ParameterType::ePoint3F >() } );
		bitangentsContext.addPopParser( cuT( "}" ), smshcompmorph::parserBitangentsEnd );

		texcoords0Context.addParser( cuT( "value" ), smshcompmorph::parserTexcoord0, { makeParameter< ParameterType::ePoint3F >() } );
		texcoords0Context.addPopParser( cuT( "}" ), smshcompmorph::parserTexcoords0End );

		texcoords1Context.addParser( cuT( "value" ), smshcompmorph::parserTexcoord1, { makeParameter< ParameterType::ePoint3F >() } );
		texcoords1Context.addPopParser( cuT( "}" ), smshcompmorph::parserTexcoords1End );

		texcoords2Context.addParser( cuT( "value" ), smshcompmorph::parserTexcoord2, { makeParameter< ParameterType::ePoint3F >() } );
		texcoords2Context.addPopParser( cuT( "}" ), smshcompmorph::parserTexcoords2End );

		texcoords3Context.addParser( cuT( "value" ), smshcompmorph::parserTexcoord3, { makeParameter< ParameterType::ePoint3F >() } );
		texcoords3Context.addPopParser( cuT( "}" ), smshcompmorph::parserTexcoords3End );

		coloursContext.addParser( cuT( "value" ), smshcompmorph::parserColour, { makeParameter< ParameterType::ePoint3F >() } );
		coloursContext.addPopParser( cuT( "}" ), smshcompmorph::parserColoursEnd );
	}

	void MorphComponent::Plugin::createSections( StrSectionIdMap & sections )const
	{
		sections.try_emplace( SectionId( smshcompmorph::Section::eMorph ), "morph_targets" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eBuffer ), "buffer" );
		sections.try_emplace( SectionId( smshcompmorph::Section::ePositions ), "positions" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eNormals ), "normals" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eTangents ), "tangents" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eBitangents ), "bitangents" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eTexcoords0 ), "texcoords0" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eTexcoords1 ), "texcoords1" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eTexcoords2 ), "texcoords2" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eTexcoords3 ), "texcoords3" );
		sections.try_emplace( SectionId( smshcompmorph::Section::eColours ), "colours" );
	}

	//*********************************************************************************************
}
