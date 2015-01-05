#include "Geometry.hpp"
#include "Mesh.hpp"
#include "Submesh.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"
#include "Engine.hpp"
#include "Material.hpp"
#include "Pass.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	bool Geometry::TextLoader::operator()( Geometry const & p_geometry, TextFile & p_file )
	{
		Logger::LogMessage( cuT( "Writing Geometry " ) + p_geometry.GetName() );
		bool l_bReturn = p_file.WriteText( cuT( "\tobject \"" ) + p_geometry.GetName() + cuT( "\"\n\t{\n" ) ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = MovableObject::TextLoader()( p_geometry, p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = Mesh::TextLoader()( *p_geometry.GetMesh(), p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\tmaterials\n\t\t{\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			for ( SubmeshPtrArrayConstIt l_it = p_geometry.GetMesh()->Begin(); l_it != p_geometry.GetMesh()->End(); ++l_it )
			{
				l_bReturn = p_file.WriteText( cuT( "\t\t\t\tmaterial \"" ) + p_geometry.GetMaterial( *l_it )->GetName() + cuT( "\"\n" ) ) > 0;
			}
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Geometry::BinaryParser::BinaryParser( Path const & p_path )
		:	MovableObject::BinaryParser( p_path )
	{
	}

	bool Geometry::BinaryParser::Fill( Geometry const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_GEOMETRY );

		if ( l_bReturn )
		{
			l_bReturn = MovableObject::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_bReturn && p_obj.GetMesh() )
		{
			l_bReturn = DoFillChunk( p_obj.GetMesh()->GetName(), eCHUNK_TYPE_GEOMETRY_MESH, l_chunk );

			if ( l_bReturn )
			{
				uint32_t l_id = 0;

				for ( SubmeshPtrArrayConstIt l_it = p_obj.GetMesh()->Begin(); l_it != p_obj.GetMesh()->End(); ++l_it )
				{
					l_bReturn = DoFillChunk( l_id, eCHUNK_TYPE_GEOMETRY_MATERIAL_ID, l_chunk );
					l_bReturn = DoFillChunk( p_obj.GetMaterial( *l_it )->GetName(), eCHUNK_TYPE_GEOMETRY_MATERIAL_NAME, l_chunk );
					l_id++;
				}
			}
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool Geometry::BinaryParser::Parse( Geometry & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		String l_name;
		uint32_t l_id = 0;
		MaterialManager & l_mtlManager = p_obj.GetScene()->GetEngine()->GetMaterialManager();

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_MOVABLE_NODE:
					l_bReturn = MovableObject::BinaryParser( m_path ).Parse( p_obj, l_chunk );
					break;

				case eCHUNK_TYPE_GEOMETRY_MESH:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetMesh( p_obj.GetScene()->GetEngine()->GetMeshManager().find( l_name ) );
					}

					break;

				case eCHUNK_TYPE_GEOMETRY_MATERIAL_ID:
					l_bReturn = DoParseChunk( l_id, l_chunk );
					break;

				case eCHUNK_TYPE_GEOMETRY_MATERIAL_NAME:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetMaterial( p_obj.GetMesh()->GetSubmesh( l_id ), l_mtlManager.find( l_name ) );
					}

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

	Geometry::Geometry( Scene * p_pScene, MeshSPtr p_mesh, SceneNodeSPtr p_sn, String const & p_name )
		:	MovableObject( p_pScene, ( SceneNode * )p_sn.get(), p_name, eMOVABLE_TYPE_GEOMETRY )
		,	m_mesh( p_mesh )
		,	m_changed( true )
		,	m_listCreated( false )
		,	m_visible( true )
	{
	}

	Geometry::~Geometry()
	{
		Cleanup();
	}

	void Geometry::Cleanup()
	{
	}

	void Geometry::CreateBuffers( uint32_t & p_nbFaces, uint32_t & p_nbVertex )
	{
		if ( !m_listCreated )
		{
			Cleanup();
			MeshSPtr l_pMesh = GetMesh();

			if ( l_pMesh )
			{
				uint32_t l_nbFaces = l_pMesh->GetFaceCount();
				uint32_t l_nbVertex = l_pMesh->GetVertexCount();
				p_nbFaces += l_nbFaces;
				p_nbVertex += l_nbVertex;
				l_pMesh->ComputeContainers();
				uint32_t l_nbSubmeshes = l_pMesh->GetSubmeshCount();
				m_listCreated = l_nbSubmeshes > 0;
				Logger::LogMessage( cuT( "Geometry::CreateBuffers - NbVertex : %d, NbFaces : %d" ), l_nbVertex, l_nbFaces );
				m_listCreated = l_pMesh->GetSubmeshCount() > 0;
			}

			std::for_each( m_submeshesMaterials.begin(), m_submeshesMaterials.end(), [&]( std::pair< SubmeshSPtr, MaterialSPtr > p_pair )
			{
				uint32_t l_uiProgramFlags = p_pair.first->GetProgramFlags();

				if ( p_pair.first->GetRefCount( p_pair.second ) > 1 )
				{
					l_uiProgramFlags |= ePROGRAM_FLAG_INSTANCIATION;
				}

				if ( GetScene()->GetEngine()->GetRenderSystem()->GetCurrentContext()->IsDeferredShadingSet() )
				{
					l_uiProgramFlags |= ePROGRAM_FLAG_DEFERRED;
				}

				SkeletonSPtr l_pSkeleton = p_pair.first->GetSkeleton();

				if ( p_pair.second )
				{
					std::for_each( p_pair.second->Begin(), p_pair.second->End(), [&]( PassSPtr p_pPass )
					{
						if ( !p_pPass->HasShader() )
						{
							ShaderProgramBaseSPtr l_pProgram = GetScene()->GetEngine()->GetShaderManager().GetAutomaticProgram( p_pPass->GetTextureFlags(), l_uiProgramFlags );
							l_pProgram->Initialise();
						}
					} );
				}
			} );
		}
	}

	void Geometry::Render()
	{
		uint32_t l_nbSubmeshes;
		SubmeshSPtr l_submesh;
		MeshSPtr l_pMesh = GetMesh();

		if ( !m_listCreated )
		{
			uint32_t l_nbFaces = 0, l_nbVertex = 0;
			CreateBuffers( l_nbFaces, l_nbVertex );
		}

		if ( m_visible && l_pMesh && m_listCreated )
		{
			l_nbSubmeshes = static_cast< uint32_t >( l_pMesh->GetSubmeshCount() );
			std::for_each( l_pMesh->Begin(), l_pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				p_pSubmesh->Render();
			} );
		}
	}

	void Geometry::SetMesh( MeshSPtr p_pMesh )
	{
		m_submeshesMaterials.clear();
		m_mesh = p_pMesh;

		if ( p_pMesh )
		{
			m_strMeshName = p_pMesh->GetName();

			for ( SubmeshPtrArrayIt l_it = p_pMesh->Begin(); l_it != p_pMesh->End(); ++l_it )
			{
				SetMaterial( *l_it, ( *l_it )->GetDefaultMaterial() );
			}
		}
		else
		{
			m_strMeshName = cuEmptyString;
		}
	}

	void Geometry::SetMaterial( SubmeshSPtr p_submesh, MaterialSPtr p_material )
	{
		MeshSPtr l_mesh = GetMesh();

		if ( l_mesh )
		{
			SubmeshPtrArrayIt l_it = std::find( l_mesh->Begin(), l_mesh->End(), p_submesh );

			if ( l_it != l_mesh->End() )
			{
				m_submeshesMaterials[p_submesh] = p_material;
			}
			else
			{
				CASTOR_EXCEPTION( "Couldn't retrive the submesh in mesh's submeshes" );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "No mesh" );
		}
	}

	MaterialSPtr Geometry::GetMaterial( SubmeshSPtr p_submesh )const
	{
		MaterialSPtr l_return;
		std::map< SubmeshSPtr, MaterialSPtr >::const_iterator l_it = m_submeshesMaterials.find( p_submesh );

		if ( l_it != m_submeshesMaterials.end() )
		{
			l_return = l_it->second;
		}
		else
		{
			Logger::LogError( cuT( "Geometry::GetMaterial - Wrong submesh" ) );
		}

		return l_return;
	}
}
