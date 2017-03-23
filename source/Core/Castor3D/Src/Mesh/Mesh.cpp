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
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		for ( auto l_submesh : p_obj )
		{
			l_return &= BinaryWriter< Submesh >{}.Write( *l_submesh, m_chunk );
		}

		if ( l_return && p_obj.m_skeleton )
		{
			l_return = BinaryWriter< Skeleton >{}.Write( *p_obj.m_skeleton, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< Mesh >::DoParse( Mesh & p_obj )
	{
		bool l_return = true;
		SubmeshSPtr l_submesh;
		SkeletonSPtr l_skeleton;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eName:
				l_return = DoParseChunk( l_name, l_chunk );

				if ( l_return )
				{
					p_obj.m_name = l_name;
				}

				break;

			case ChunkType::eSubmesh:
				l_submesh = std::make_shared< Submesh >( *p_obj.GetScene(), p_obj, p_obj.GetSubmeshCount() );
				l_return = BinaryParser< Submesh >{}.Parse( *l_submesh, l_chunk );

				if ( l_return )
				{
					p_obj.m_submeshes.push_back( l_submesh );
				}

				break;

			case ChunkType::eSkeleton:
				l_skeleton = std::make_shared< Skeleton >( *p_obj.GetScene() );
				l_return = BinaryParser< Skeleton >{}.Parse( *l_skeleton, l_chunk );

				if ( l_return )
				{
					p_obj.SetSkeleton( l_skeleton );
				}

				break;
			}
		}

		if ( l_return )
		{
			p_obj.ComputeContainers();
		}

		return l_return;
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

		for ( auto l_submesh : m_submeshes )
		{
			l_submesh->Cleanup();
		}

		m_submeshes.clear();
	}

	void Mesh::ComputeContainers()
	{
		if ( m_submeshes.size() == 0 )
		{
			return;
		}

		uint32_t l_count = GetSubmeshCount();

		for ( uint32_t i = 0; i < l_count; i++ )
		{
			m_submeshes[i]->ComputeContainers();
		}

		Point3r l_min( m_submeshes[0]->GetCollisionBox().GetMin() );
		Point3r l_max( m_submeshes[0]->GetCollisionBox().GetMax() );

		for ( auto l_submesh : m_submeshes )
		{
			CubeBox const & l_box = l_submesh->GetCollisionBox();
			l_max[0] = std::max( l_box.GetMax()[0], l_max[0] );
			l_max[1] = std::max( l_box.GetMax()[1], l_max[1] );
			l_max[2] = std::max( l_box.GetMax()[2], l_max[2] );
			l_min[0] = std::min( l_box.GetMin()[0], l_min[0] );
			l_min[1] = std::min( l_box.GetMin()[1], l_min[1] );
			l_min[2] = std::min( l_box.GetMin()[2], l_min[2] );
		}

		m_box.Load( l_min, l_max );
		m_sphere.Load( m_box );
	}

	uint32_t Mesh::GetFaceCount()const
	{
		uint32_t l_nbFaces = 0;

		for ( auto l_submesh : m_submeshes )
		{
			l_nbFaces += l_submesh->GetFaceCount();
		}

		return l_nbFaces;
	}

	uint32_t Mesh::GetVertexCount()const
	{
		uint32_t l_nbFaces = 0;

		for ( auto l_submesh : m_submeshes )
		{
			l_nbFaces += l_submesh->GetPointsCount();
		}

		return l_nbFaces;
	}

	SubmeshSPtr Mesh::GetSubmesh( uint32_t p_index )const
	{
		SubmeshSPtr l_return;

		if ( p_index < m_submeshes.size() )
		{
			l_return = m_submeshes[p_index];
		}

		return l_return;
	}

	SubmeshSPtr Mesh::CreateSubmesh()
	{
		SubmeshSPtr l_submesh = std::make_shared< Submesh >( *GetScene(), *this, GetSubmeshCount() );
		m_submeshes.push_back( l_submesh );
		return l_submesh;
	}

	void Mesh::DeleteSubmesh( SubmeshSPtr & p_submesh )
	{
		auto l_it = std::find( m_submeshes.begin(), m_submeshes.end(), p_submesh );

		if ( l_it != m_submeshes.end() )
		{
			m_submeshes.erase( l_it );
			p_submesh.reset();
			l_it = m_submeshes.end();
		}
	}

	void Mesh::ComputeNormals( bool p_reverted )
	{
		for ( auto l_submesh : m_submeshes )
		{
			l_submesh->ComputeNormals( p_reverted );
		}
	}

	void Mesh::Ref( MaterialSPtr p_material )
	{
		for ( auto l_submesh : m_submeshes )
		{
			l_submesh->Ref( p_material );
		}
	}

	void Mesh::UnRef( MaterialSPtr p_material )
	{
		for ( auto l_submesh : m_submeshes )
		{
			l_submesh->UnRef( p_material );
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
