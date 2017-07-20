#include "Mesh.hpp"

#include "Engine.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	bool BinaryWriter< Mesh >::DoWrite( Mesh const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		for ( auto submesh : p_obj )
		{
			result &= BinaryWriter< Submesh >{}.Write( *submesh, m_chunk );
		}

		if ( result && p_obj.m_skeleton )
		{
			result = BinaryWriter< Skeleton >{}.Write( *p_obj.m_skeleton, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Mesh >::DoParse( Mesh & p_obj )
	{
		bool result = true;
		SubmeshSPtr submesh;
		SkeletonSPtr skeleton;
		String name;
		BinaryChunk chunk;

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eName:
				result = DoParseChunk( name, chunk );

				if ( result )
				{
					p_obj.m_name = name;
				}

				break;

			case ChunkType::eSubmesh:
				submesh = std::make_shared< Submesh >( *p_obj.GetScene(), p_obj, p_obj.GetSubmeshCount() );
				result = BinaryParser< Submesh >{}.Parse( *submesh, chunk );

				if ( result )
				{
					p_obj.m_submeshes.push_back( submesh );
				}

				break;

			case ChunkType::eSkeleton:
				skeleton = std::make_shared< Skeleton >( *p_obj.GetScene() );
				result = BinaryParser< Skeleton >{}.Parse( *skeleton, chunk );

				if ( result )
				{
					p_obj.SetSkeleton( skeleton );
				}

				break;
			}
		}

		if ( result )
		{
			p_obj.ComputeContainers();
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
		Cleanup();
	}

	void Mesh::Cleanup()
	{
		Animable::CleanupAnimations();

		for ( auto submesh : m_submeshes )
		{
			submesh->Cleanup();
		}

		m_submeshes.clear();
	}

	void Mesh::ComputeContainers()
	{
		if ( m_submeshes.size() == 0 )
		{
			return;
		}

		uint32_t count = GetSubmeshCount();

		for ( uint32_t i = 0; i < count; i++ )
		{
			m_submeshes[i]->ComputeContainers();
		}

		Point3r min( m_submeshes[0]->GetCollisionBox().GetMin() );
		Point3r max( m_submeshes[0]->GetCollisionBox().GetMax() );

		for ( auto submesh : m_submeshes )
		{
			CubeBox const & box = submesh->GetCollisionBox();
			max[0] = std::max( box.GetMax()[0], max[0] );
			max[1] = std::max( box.GetMax()[1], max[1] );
			max[2] = std::max( box.GetMax()[2], max[2] );
			min[0] = std::min( box.GetMin()[0], min[0] );
			min[1] = std::min( box.GetMin()[1], min[1] );
			min[2] = std::min( box.GetMin()[2], min[2] );
		}

		m_box.Load( min, max );
		m_sphere.Load( m_box );
	}

	uint32_t Mesh::GetFaceCount()const
	{
		uint32_t nbFaces = 0;

		for ( auto submesh : m_submeshes )
		{
			nbFaces += submesh->GetFaceCount();
		}

		return nbFaces;
	}

	uint32_t Mesh::GetVertexCount()const
	{
		uint32_t nbFaces = 0;

		for ( auto submesh : m_submeshes )
		{
			nbFaces += submesh->GetPointsCount();
		}

		return nbFaces;
	}

	SubmeshSPtr Mesh::GetSubmesh( uint32_t p_index )const
	{
		SubmeshSPtr result;

		if ( p_index < m_submeshes.size() )
		{
			result = m_submeshes[p_index];
		}

		return result;
	}

	SubmeshSPtr Mesh::CreateSubmesh()
	{
		SubmeshSPtr submesh = std::make_shared< Submesh >( *GetScene(), *this, GetSubmeshCount() );
		m_submeshes.push_back( submesh );
		return submesh;
	}

	void Mesh::DeleteSubmesh( SubmeshSPtr & p_submesh )
	{
		auto it = std::find( m_submeshes.begin(), m_submeshes.end(), p_submesh );

		if ( it != m_submeshes.end() )
		{
			m_submeshes.erase( it );
			p_submesh.reset();
			it = m_submeshes.end();
		}
	}

	void Mesh::ComputeNormals( bool p_reverted )
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->ComputeNormals( p_reverted );
		}
	}

	void Mesh::Ref( MaterialSPtr p_material )
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->Ref( p_material );
		}
	}

	void Mesh::UnRef( MaterialSPtr p_material )
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->UnRef( p_material );
		}
	}

	void Mesh::SetSkeleton( SkeletonSPtr p_skeleton )
	{
		m_skeleton = p_skeleton;
	}

	MeshAnimation & Mesh::CreateAnimation( Castor::String const & p_name )
	{
		if ( !HasAnimation( p_name ) )
		{
			DoAddAnimation( std::make_shared< MeshAnimation >( *this, p_name ) );
		}

		return DoGetAnimation< MeshAnimation >( p_name );
	}
}
