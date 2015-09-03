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
	Mesh::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Mesh, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
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
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_MESH );

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		for ( auto && l_submesh : p_obj )
		{
			Submesh::BinaryParser( m_path ).Fill( *l_submesh, l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool Mesh::BinaryParser::Parse( Mesh & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		SubmeshSPtr l_submesh;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NAME:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.m_name = l_name;
					}

					break;

				case eCHUNK_TYPE_SUBMESH:
					l_submesh = p_obj.CreateSubmesh();
					l_bReturn = Submesh::BinaryParser( m_path ).Parse( *l_submesh, l_chunk );
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Mesh::Mesh( Engine * p_pEngine, eMESH_TYPE p_eMeshType, String const & p_name )
		:	Resource< Mesh >( p_name )
		,	m_modified( false )
		,	m_factory( p_pEngine->GetMeshFactory() )
		,	m_pEngine( p_pEngine )
	{
		m_pMeshCategory = m_factory.Create( p_eMeshType );
		m_pMeshCategory->SetMesh( this );
	}

	Mesh::Mesh( Engine * p_pEngine, eMESH_TYPE p_eMeshType )
		:	Mesh( p_pEngine, p_eMeshType, cuEmptyString )
	{
	}

	Mesh::~Mesh()
	{
		Cleanup();
	}

	void Mesh::Cleanup()
	{
		//	vector::deleteAll( m_submeshes);
		m_submeshes.clear();
	}

	void Mesh::Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions )
	{
		m_pMeshCategory->Initialise( p_arrayFaces, p_arrayDimensions );
	}

	void Mesh::ComputeContainers()
	{
		if ( m_submeshes.size() == 0 )
		{
			return;
		}

		uint32_t l_uiCount = GetSubmeshCount();

		for ( uint32_t i = 0; i < l_uiCount; i++ )
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
		SubmeshSPtr l_submesh = std::make_shared< Submesh >( this, m_pEngine, GetSubmeshCount() );
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
		m_pMeshCategory->ComputeNormals( p_bReverted );
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
		MeshSPtr l_clone = std::make_shared< Mesh >( m_pEngine, GetMeshType(), p_name );

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
