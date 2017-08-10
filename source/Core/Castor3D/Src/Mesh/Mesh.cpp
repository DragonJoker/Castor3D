#include "Mesh.hpp"

#include "Engine.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace castor;

namespace castor3d
{
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

		if ( result && p_obj.m_skeleton )
		{
			result = BinaryWriter< Skeleton >{}.write( *p_obj.m_skeleton, m_chunk );
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

		if ( result )
		{
			p_obj.computeContainers();
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

	void Mesh::computeContainers()
	{
		if ( m_submeshes.size() == 0 )
		{
			return;
		}

		uint32_t count = getSubmeshCount();

		for ( uint32_t i = 0; i < count; i++ )
		{
			m_submeshes[i]->computeContainers();
		}

		Point3r min( m_submeshes[0]->getCollisionBox().getMin() );
		Point3r max( m_submeshes[0]->getCollisionBox().getMax() );

		for ( auto submesh : m_submeshes )
		{
			CubeBox const & box = submesh->getCollisionBox();
			max[0] = std::max( box.getMax()[0], max[0] );
			max[1] = std::max( box.getMax()[1], max[1] );
			max[2] = std::max( box.getMax()[2], max[2] );
			min[0] = std::min( box.getMin()[0], min[0] );
			min[1] = std::min( box.getMin()[1], min[1] );
			min[2] = std::min( box.getMin()[2], min[2] );
		}

		m_box.load( min, max );
		m_sphere.load( m_box );
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

	void Mesh::ref( MaterialSPtr p_material )
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->ref( p_material );
		}
	}

	void Mesh::unref( MaterialSPtr p_material )
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->unref( p_material );
		}
	}

	void Mesh::setSkeleton( SkeletonSPtr p_skeleton )
	{
		m_skeleton = p_skeleton;
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
