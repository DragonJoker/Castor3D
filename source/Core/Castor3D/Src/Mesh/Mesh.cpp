#include "Mesh.hpp"

#include "Engine.hpp"
#include "Mesh/MeshFactory.hpp"
#include "Mesh/Submesh.hpp"
#include "Animation/Skeleton.hpp"
#include "Animation/Animation.hpp"

#include <Factory.hpp>
#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	Mesh::BinaryWriter::BinaryWriter( Path const & p_path )
		: Castor3D::BinaryWriter< Mesh >{ p_path }
	{
	}

	bool Mesh::BinaryWriter::DoWrite( Mesh const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, p_chunk );
		}

		for ( auto && l_submesh : p_obj )
		{
			Submesh::BinaryWriter( m_path ).Write( *l_submesh, p_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	Mesh::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Mesh >{ p_path }
	{
	}

	bool Mesh::BinaryParser::DoParse( Mesh & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		SubmeshSPtr l_submesh;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NAME:
					l_return = DoParseChunk( l_name, l_chunk );

					if ( l_return )
					{
						p_obj.m_name = l_name;
					}

					break;

				case eCHUNK_TYPE_SUBMESH:
					l_submesh = p_obj.CreateSubmesh();
					l_return = Submesh::BinaryParser( m_path ).Parse( *l_submesh, l_chunk );
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Mesh::Mesh( String const & p_name, Engine & p_engine )
		: Resource< Mesh >( p_name )
		, OwnedBy< Engine >( p_engine )
		, m_modified( false )
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

		Point3r l_min( m_submeshes[0]->GetCubeBox().GetMin() );
		Point3r l_max( m_submeshes[0]->GetCubeBox().GetMax() );

		for ( auto && l_submesh : m_submeshes )
		{
			CubeBox const & l_box = l_submesh->GetCubeBox();

			if ( l_box.GetMin()[0] < l_min[0] )
			{
				l_min[0] = l_box.GetMin()[0];
			}

			if ( l_box.GetMin()[1] < l_min[1] )
			{
				l_min[1] = l_box.GetMin()[1];
			}

			if ( l_box.GetMin()[2] < l_min[2] )
			{
				l_min[2] = l_box.GetMin()[2];
			}

			if ( l_box.GetMax()[0] > l_max[0] )
			{
				l_max[0] = l_box.GetMax()[0];
			}

			if ( l_box.GetMax()[1] > l_max[1] )
			{
				l_max[1] = l_box.GetMax()[1];
			}

			if ( l_box.GetMax()[2] > l_max[2] )
			{
				l_max[2] = l_box.GetMax()[2];
			}
		}

		m_box.Load( l_min, l_max );
		m_sphere.Load( m_box );
	}

	uint32_t Mesh::GetFaceCount()const
	{
		uint32_t l_nbFaces = 0;

		for ( auto && l_submesh : m_submeshes )
		{
			l_nbFaces += l_submesh->GetFaceCount();
		}

		return l_nbFaces;
	}

	uint32_t Mesh::GetVertexCount()const
	{
		uint32_t l_nbFaces = 0;

		for ( auto && l_submesh : m_submeshes )
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
		SubmeshSPtr l_submesh = std::make_shared< Submesh >( *GetEngine(), this, GetSubmeshCount() );
		m_submeshes.push_back( l_submesh );
		return l_submesh;
	}

	void Mesh::DeleteSubmesh( SubmeshSPtr & p_submesh )
	{
		auto && l_it = std::find( m_submeshes.begin(), m_submeshes.end(), p_submesh );

		if ( l_it != m_submeshes.end() )
		{
			m_submeshes.erase( l_it );
			p_submesh.reset();
			l_it = m_submeshes.end();
		}
	}

	void Mesh::ComputeNormals( bool p_reverted )
	{
		for ( auto && l_submesh : m_submeshes )
		{
			l_submesh->ComputeNormals( p_reverted );
		}
	}

	MeshSPtr Mesh::Clone( String const & p_name )
	{
		MeshSPtr l_clone = std::make_shared< Mesh >( p_name, *GetEngine() );

		for ( auto && l_submesh : m_submeshes )
		{
			l_clone->m_submeshes.push_back( l_submesh->Clone() );
		}

		l_clone->ComputeContainers();
		return l_clone;
	}

	void Mesh::Ref( MaterialSPtr p_material )
	{
		for ( auto && l_submesh : m_submeshes )
		{
			l_submesh->Ref( p_material );
		}
	}

	void Mesh::UnRef( MaterialSPtr p_material )
	{
		for ( auto && l_submesh : m_submeshes )
		{
			l_submesh->UnRef( p_material );
		}
	}

	void Mesh::SetSkeleton( SkeletonSPtr p_skeleton )
	{
		m_skeleton = p_skeleton;

		m_skeleton->TraverseHierarchy( []( BoneSPtr p_bone )
		{
			if ( p_bone->GetParent() )
			{
				p_bone->SetFinalTransformation( p_bone->GetParent()->GetFinalTransformation() * p_bone->GetOffsetMatrix() );
			}
			else
			{
				p_bone->SetFinalTransformation( p_bone->GetOffsetMatrix() );
			}
		} );
	}
}
