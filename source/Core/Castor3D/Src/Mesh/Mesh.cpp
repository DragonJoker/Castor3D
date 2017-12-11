#include "Mesh.hpp"

#include "Engine.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	Mesh::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Mesh >{ tabs }
	{
	}

	bool Mesh::TextWriter::operator()( Mesh const & object
		, TextFile & file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing Mesh " ) + object.getName() );
		auto result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "mesh \"" ) + object.getName() + cuT( "\"\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\timport \"Meshes/" ) + object.getName() + cuT( ".cmsh\"\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		castor::TextWriter< Mesh >::checkError( result, "Mesh" );
		return result;
	}

	//*************************************************************************************************

	bool BinaryWriter< Mesh >::doWrite( Mesh const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( p_obj.getName(), ChunkType::eName, m_chunk );
		}

		for ( auto submesh : p_obj )
		{
			result &= BinaryWriter< Submesh >{}.write( *submesh, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Mesh >::doParse( Mesh & p_obj )
	{
		bool result = true;
		SubmeshSPtr submesh;
		SkeletonSPtr skeleton;
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					p_obj.m_name = name;
				}

				break;

			case ChunkType::eSubmesh:
				submesh = std::make_shared< Submesh >( *p_obj.getScene(), p_obj, p_obj.getSubmeshCount() );
				result = BinaryParser< Submesh >{}.parse( *submesh, chunk );

				if ( result )
				{
					p_obj.m_submeshes.push_back( submesh );
				}

				break;
			}
		}

		if ( result )
		{
			p_obj.computeContainers();
		}

		return result;
	}

	bool BinaryParser< Mesh >::doParse_v1_2( Mesh & p_obj )
	{
		bool result = true;
		SubmeshSPtr submesh;
		SkeletonSPtr skeleton;
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeleton:
				skeleton = std::make_shared< Skeleton >( *p_obj.getScene() );
				result = BinaryParser< Skeleton >{}.parse( *skeleton, chunk );

				if ( result )
				{
					p_obj.setSkeleton( skeleton );
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Mesh::Mesh( String const & p_name, Scene & p_scene )
		: Resource< Mesh >{ p_name }
		, Animable{ p_scene }
		, m_modified{ false }
	{
	}

	Mesh::~Mesh()
	{
		cleanup();
	}

	void Mesh::cleanup()
	{
		Animable::cleanupAnimations();

		for ( auto submesh : m_submeshes )
		{
			submesh->cleanup();
		}

		m_submeshes.clear();
	}

	void Mesh::updateContainers()
	{
		if ( !m_submeshes.empty() )
		{
			m_box = m_submeshes[0]->getBoundingBox();

			for ( auto i = 1u; i < m_submeshes.size(); ++i )
			{
				m_box = m_box.getUnion( m_submeshes[i]->getBoundingBox() );
			}

			m_sphere.load( m_box );
		}
	}

	void Mesh::computeContainers()
	{
		for ( auto & submesh : m_submeshes )
		{
			submesh->computeContainers();
		}

		updateContainers();
	}

	uint32_t Mesh::getFaceCount()const
	{
		uint32_t nbFaces = 0;

		for ( auto submesh : m_submeshes )
		{
			nbFaces += submesh->getFaceCount();
		}

		return nbFaces;
	}

	uint32_t Mesh::getVertexCount()const
	{
		uint32_t nbFaces = 0;

		for ( auto submesh : m_submeshes )
		{
			nbFaces += submesh->getPointsCount();
		}

		return nbFaces;
	}

	SubmeshSPtr Mesh::getSubmesh( uint32_t p_index )const
	{
		SubmeshSPtr result;

		if ( p_index < m_submeshes.size() )
		{
			result = m_submeshes[p_index];
		}

		return result;
	}

	SubmeshSPtr Mesh::createSubmesh()
	{
		SubmeshSPtr submesh = std::make_shared< Submesh >( *getScene(), *this, getSubmeshCount() );
		m_submeshes.push_back( submesh );
		return submesh;
	}

	void Mesh::deleteSubmesh( SubmeshSPtr & p_submesh )
	{
		auto it = std::find( m_submeshes.begin(), m_submeshes.end(), p_submesh );

		if ( it != m_submeshes.end() )
		{
			m_submeshes.erase( it );
			p_submesh.reset();
			it = m_submeshes.end();
		}
	}

	void Mesh::computeNormals( bool p_reverted )
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->computeNormals( p_reverted );
		}
	}

	void Mesh::setSkeleton( SkeletonSPtr p_skeleton )
	{
		m_skeleton = p_skeleton;
		m_skeleton->computeContainers( *this );
	}

	MeshAnimation & Mesh::createAnimation( castor::String const & p_name )
	{
		if ( !hasAnimation( p_name ) )
		{
			doAddAnimation( std::make_shared< MeshAnimation >( *this, p_name ) );
		}

		return doGetAnimation< MeshAnimation >( p_name );
	}
}
