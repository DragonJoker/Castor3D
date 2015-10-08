#include "Mesh.hpp"

#include "Engine.hpp"
#include "MeshFactory.hpp"
#include "Submesh.hpp"
#include "Cone.hpp"
#include "Cylinder.hpp"
#include "Icosahedron.hpp"
#include "Cube.hpp"
#include "Plane.hpp"
#include "Sphere.hpp"
#include "Torus.hpp"
#include "Projection.hpp"
#include "Animation.hpp"

#include <Factory.hpp>
#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	Mesh::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< Mesh, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool Mesh::TextLoader::operator()( Mesh const & p_mesh, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "Writing Mesh " ) + p_mesh.GetName() );
		bool l_return = p_file.WriteText( cuT( "\t\tmesh \"" ) + p_mesh.GetName() + cuT( "\"\n\t\t{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\t\ttype custom\n" ) ) > 0;
		}

		if ( l_return )
		{
			for ( auto && l_submesh : p_mesh )
			{
				Submesh::TextLoader()( *l_submesh, p_file );
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	Mesh::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< Mesh >( p_path )
	{
	}

	bool Mesh::BinaryParser::Fill( Mesh const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_MESH );

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		for ( auto && l_submesh : p_obj )
		{
			Submesh::BinaryParser( m_path ).Fill( *l_submesh, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool Mesh::BinaryParser::Parse( Mesh & p_obj, BinaryChunk & p_chunk )const
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

	Mesh::Mesh( Engine * p_engine, String const & p_name )
		: Resource< Mesh >( p_name )
		, m_modified( false )
		, m_engine( p_engine )
	{
	}

	Mesh::Mesh( Engine * p_engine )
		: Mesh( p_engine, cuEmptyString )
	{
	}

	Mesh::~Mesh()
	{
		Cleanup();
	}

	void Mesh::Cleanup()
	{
		//vector::deleteAll( m_submeshes);
		m_submeshes.clear();
	}

	void Mesh::Initialise( MeshGenerator & p_generator, UIntArray const & p_faces, RealArray const & p_dimensions )
	{
		p_generator.Generate( *this, p_faces, p_dimensions );
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
		SubmeshSPtr l_pReturn;

		if ( p_index < m_submeshes.size() )
		{
			l_pReturn = m_submeshes[p_index];
		}

		return l_pReturn;
	}

	SubmeshSPtr Mesh::CreateSubmesh()
	{
		SubmeshSPtr l_submesh = std::make_shared< Submesh >( this, m_engine, GetSubmeshCount() );
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

	void Mesh::ComputeNormals( bool p_bReverted )
	{
		for ( auto && l_submesh : m_submeshes )
		{
			l_submesh->ComputeNormals( p_bReverted );
		}
	}

	void Mesh::GenerateBuffers()
	{
		for ( auto && l_submesh : m_submeshes )
		{
			l_submesh->GenerateBuffers();
		}
	}

	MeshSPtr Mesh::Clone( String const & p_name )
	{
		MeshSPtr l_clone = std::make_shared< Mesh >( m_engine, p_name );

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
}
