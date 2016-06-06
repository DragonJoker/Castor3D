#include "ObjImporter.hpp"

#include "ObjGroup.hpp"

#include <Colour.hpp>
#include <Image.hpp>

#include <Engine.hpp>
#include <GeometryManager.hpp>
#include <MaterialManager.hpp>
#include <MeshManager.hpp>
#include <SceneManager.hpp>
#include <SceneNodeManager.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Manager/ManagerView.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Obj
{
	ObjImporter::ObjImporter( Engine & p_pEngine )
		: Importer( p_pEngine )
		, m_collImages( p_pEngine.GetImageManager() )
		, m_pFile( nullptr )
	{
	}

	SceneSPtr ObjImporter::DoImportScene()
	{
		SceneSPtr l_scene = GetEngine()->GetSceneManager().Create( cuT( "Scene_OBJ" ), *GetEngine() );
		MeshSPtr l_mesh = DoImportMesh( *l_scene );

		if ( l_mesh )
		{
			SceneNodeSPtr l_node = l_scene->GetSceneNodeManager().Create( l_mesh->GetName(), l_scene->GetObjectRootNode() );
			GeometrySPtr l_geometry = l_scene->GetGeometryManager().Create( l_mesh->GetName(), l_node );
			l_geometry->AttachTo( l_node );

			for ( auto && l_submesh : *l_mesh )
			{
				GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
			}

			l_geometry->SetMesh( l_mesh );
		}

		m_arrayLoadedMaterials.clear();
		m_arrayTextures.clear();
		return l_scene;
	}

	MeshSPtr ObjImporter::DoImportMesh( Scene & p_scene )
	{
		MeshSPtr l_return;

		try
		{
			TextFile l_file( m_fileName, File::eOPEN_MODE_READ );

			if ( l_file.IsOk() )
			{
				m_pFile = &l_file;
				l_return = DoReadObjFile( p_scene );
				m_arrayLoadedMaterials.clear();
				m_arrayTextures.clear();
			}

			if ( m_pThread )
			{
				m_pThread->join();
				m_pThread.reset();
			}
		}
		catch ( std::exception & exc )
		{
			Logger::LogWarning( std::stringstream() << "Encountered exception while importing mesh: " << exc.what() );
		}

		return l_return;
	}

	void ObjImporter::DoAddTexture( String const & p_strValue, PassSPtr p_pPass, TextureChannel p_channel )
	{
		Point3f l_offset( 0, 0, 0 );
		Point3f l_scale( 1, 1, 1 );
		Point3f l_turbulence( 0, 0, 0 );
		ImageSPtr l_pImage;
		String l_strValue( p_strValue );
		DoParseTexParams( l_strValue, l_offset.ptr(), l_scale.ptr(), l_turbulence.ptr() );
		m_mapOffsets[p_pPass] = l_offset;
		m_mapScales[p_pPass] = l_scale;
		m_mapTurbulences[p_pPass] = l_turbulence;
		Logger::LogDebug( StringStream() << cuT( "-	Texture :    " ) + l_strValue );
		Logger::LogDebug( StringStream() << cuT( "-	Offset :     " ) << l_offset );
		Logger::LogDebug( StringStream() << cuT( "-	Scale :      " ) << l_scale );
		Logger::LogDebug( StringStream() << cuT( "-	Turbulence : " ) << l_turbulence );

		if ( !l_strValue.empty() )
		{
			auto l_texture = LoadTexture( Path{ l_strValue }, *p_pPass, p_channel );

			if ( l_texture )
			{
				m_arrayTextures.push_back( l_texture->GetTexture() );
			}
		}
	}

	MeshSPtr ObjImporter::DoReadObjFile( Scene & p_scene )
	{
		String l_name = m_fileName.GetFileName();
		String l_meshName = l_name.substr( 0, l_name.find_last_of( '.' ) );
		MeshSPtr l_return = p_scene.GetMeshView().Create( l_meshName, eMESH_TYPE_CUSTOM );
		String l_strSection;
		String l_strValue;
		String l_strLine;
		String l_strFace;
		StringArray l_arrayValues;
		StringArray l_arrayIndex;
		StringArray l_arraySplitted;
		StringArray l_arrayFace;
		MaterialSPtr l_pMaterial;
		UvArray l_arrayAllTex;
		NormalArray l_arrayAllNml;
		VertexArray l_arrayAllVtx;
		FaceArrayGrpMap::iterator l_itGroup;
		FaceIndices l_face;
		stVERTEX l_vertex;
		stUV l_uv;
		stNORMAL l_normal;
		stGROUP * l_pGroup = NULL;
		auto & l_mtlManager = p_scene.GetMaterialView();
		FaceArray * l_pArrayIndex = NULL;

		while ( m_pFile->IsOk() )
		{
			m_pFile->ReadLine( l_strLine, 1000 );

			if ( !l_strLine.empty() )
			{
				l_arraySplitted = string::split( l_strLine, cuT( " " ), 1 );

				if ( l_arraySplitted.size() >= 1 )
				{
					l_strSection = l_arraySplitted[0];

					if ( l_arraySplitted.size() > 1 )
					{
						l_strValue = l_arraySplitted[1];
					}
					else
					{
						l_strValue.clear();
					}

					string::trim( l_strValue );
					string::trim( l_strSection );

					if ( l_strSection == cuT( "mtllib" ) )
					{
						// Material description file
						if ( File::FileExists( m_filePath / l_strValue ) )
						{
							DoReadMaterials( p_scene, m_filePath / l_strValue );
						}
					}
					else if ( l_strSection == cuT( "usemtl" ) )
					{
						// Material
						l_pMaterial = l_mtlManager.Find( l_strValue );

						if ( l_pGroup && l_pMaterial )
						{
							if ( !l_pGroup->m_mapVtxIndex.empty() )
							{
								DoCreateSubmesh( l_return, l_pGroup );
								delete l_pGroup;
								l_pGroup = new stGROUP;
								l_pGroup->m_strName = cuT( "" );
								l_itGroup = l_pGroup->m_mapIdx.find( l_pGroup->m_uiGroupId );
								l_pArrayIndex = &( l_itGroup->second );
							}

							l_pGroup->m_pMaterial = l_pMaterial;
						}
					}
					else if ( l_strSection == cuT( "group" ) || l_strSection == cuT( "g" ) )
					{
						// Submesh
						if ( l_strValue.empty() )
						{
							l_strValue = cuT( "(null)" );
						}

						if ( l_pGroup )
						{
							DoCreateSubmesh( l_return, l_pGroup );
							delete l_pGroup;
							l_pGroup = NULL;
						}

						// We create the group then put the group arrays into it
						l_pGroup = new stGROUP;
						l_pGroup->m_strName = l_strValue;
						l_pGroup->m_pMaterial = l_pMaterial;
						l_itGroup = l_pGroup->m_mapIdx.find( l_pGroup->m_uiGroupId );
						l_pArrayIndex = &( l_itGroup->second );
					}
					else if ( l_strSection == cuT( "v" ) )
					{
						// Vertex position
						StringStream l_stream( l_strValue );
						l_stream >> l_vertex.m_val[0] >> l_vertex.m_val[1] >> l_vertex.m_val[2];
						l_arrayAllVtx.push_back( l_vertex );
					}
					else if ( l_strSection == cuT( "vt" ) )
					{
						// Vertex UV
						StringStream l_stream( l_strValue );
						l_stream >> l_uv.m_val[0] >> l_uv.m_val[1];
						l_arrayAllTex.push_back( l_uv );
					}
					else if ( l_strSection == cuT( "vn" ) )
					{
						// Vertex Normal
						StringStream l_stream( l_strValue );
						l_stream >> l_normal.m_val[0] >> l_normal.m_val[1] >> l_normal.m_val[2];
						l_arrayAllNml.push_back( l_normal );
					}
					else if ( l_strSection == cuT( "s" ) )
					{
						// Smoothing Group
						if ( !l_pGroup )
						{
							l_pGroup = new stGROUP;
							l_pGroup->m_pMaterial = l_pMaterial;
							l_itGroup = l_pGroup->m_mapIdx.find( l_pGroup->m_uiGroupId );
							l_pArrayIndex = &( l_itGroup->second );
						}
					}
					else if ( l_strSection == cuT( "f" ) )
					{
						// Face indices
						if ( !l_pGroup )
						{
							l_pGroup = new stGROUP;
							l_pGroup->m_pMaterial = l_pMaterial;
							l_itGroup = l_pGroup->m_mapIdx.find( l_pGroup->m_uiGroupId );
							l_pArrayIndex = &( l_itGroup->second );
						}

						String l_strName = string::lower_case( l_pGroup->m_strName );

						if ( l_strName.find( cuT( "gundummy" ) ) == String::npos && l_strName.find( cuT( "bip01" ) ) == String::npos && l_strName.find( cuT( "fcfx_" ) ) == String::npos && l_strName.find( cuT( "bbone_" ) ) == String::npos )
						{
							l_arrayFace = string::split( l_strValue, cuT( " " ) );
							uint32_t l_uiV1, l_uiV2, l_uiV3;

							if ( l_arrayFace.size() == 3 )
							{
								// Face represented by 3 vertices
								l_uiV1 = DoTreatFace( l_face, 0, l_arrayFace[0], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								l_uiV2 = DoTreatFace( l_face, 1, l_arrayFace[2], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								l_uiV3 = DoTreatFace( l_face, 2, l_arrayFace[1], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								FaceIndices l_indices = { l_uiV1, l_uiV2, l_uiV3 };
								l_pGroup->m_arrayFaces.push_back( l_indices );
								l_pArrayIndex->push_back( l_face );
							}
							else if ( l_arrayFace.size() == 4 )
							{
								// Face represented by 4 vertices
								l_uiV1 = DoTreatFace( l_face, 0, l_arrayFace[0], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								l_uiV2 = DoTreatFace( l_face, 1, l_arrayFace[2], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								l_uiV3 = DoTreatFace( l_face, 2, l_arrayFace[1], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								FaceIndices l_indices1 = { l_uiV1, l_uiV2, l_uiV3 };
								l_pGroup->m_arrayFaces.push_back( l_indices1 );
								l_pArrayIndex->push_back( l_face );
								l_uiV1 = DoTreatFace( l_face, 0, l_arrayFace[0], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								l_uiV2 = DoTreatFace( l_face, 1, l_arrayFace[3], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								l_uiV3 = DoTreatFace( l_face, 2, l_arrayFace[2], l_pGroup, l_arrayAllVtx, l_arrayAllNml, l_arrayAllTex );
								FaceIndices l_indices2 = { l_uiV1, l_uiV2, l_uiV3 };
								l_pGroup->m_arrayFaces.push_back( l_indices2 );
								l_pArrayIndex->push_back( l_face );
							}
						}
					}
				}
			}
		}

		if ( l_pGroup )
		{
			DoCreateSubmesh( l_return, l_pGroup );
			delete l_pGroup;
			l_pGroup = NULL;
		}

		clear_container( l_arrayAllVtx );
		clear_container( l_arrayAllNml );
		clear_container( l_arrayAllTex );
		Logger::LogDebug( cuT( "LastLine : " ) + l_strSection + cuT( " " ) + l_strValue );
		return l_return;
	}

	uint32_t ObjImporter::DoRetrieveIndex( String & p_strIndex, uint32_t p_uiSize )
	{
		string::trim( p_strIndex );
		int l_iIndex = string::to_int( p_strIndex );
		uint32_t l_uiReturn;

		if ( l_iIndex < 0 )
		{
			l_uiReturn = uint32_t( p_uiSize ) + l_iIndex;
		}
		else
		{
			l_uiReturn = uint32_t( l_iIndex ) - 1;
		}

		return l_uiReturn;
	}

	uint32_t ObjImporter::DoTreatFace( FaceIndices & p_face, uint32_t p_uiIndex, String const & p_strFace, stGROUP * p_pGroup, Obj::VertexArray const & p_arrayVtx, NormalArray const & p_arrayNml, UvArray const & p_arrayTex )
	{
		//	VertexSPtr l_return;
		String l_strFace( p_strFace );
		StringArray l_arrayIndex;
		uint32_t l_uiIndex = 0;
		UIntUIntMap::iterator l_it;
		string::replace( l_strFace, cuT( "//" ), cuT( "/ /" ) );
		l_arrayIndex = string::split( l_strFace, cuT( "/" ) );

		if ( l_arrayIndex.size() > 0 )
		{
			// Face description seems to contain at least positions
			l_uiIndex = DoRetrieveIndex( l_arrayIndex[0], uint32_t( p_arrayVtx.size() ) );
			l_it = p_pGroup->m_mapVtxIndex.find( l_uiIndex );

			if ( l_it == p_pGroup->m_mapVtxIndex.end() )
			{
				// Vertex hasn't been inserted yet, so we insert it, to have good index, relative to the group's vertex
				p_pGroup->m_mapVtxIndex.insert( std::make_pair( l_uiIndex, uint32_t( p_pGroup->m_arrayVtx.size() ) ) );
				p_pGroup->m_arrayVtx.push_back( p_arrayVtx[l_uiIndex] );
				l_it = p_pGroup->m_mapVtxIndex.find( l_uiIndex );
			}

			p_face.m_index[p_uiIndex] = l_it->second;
			p_pGroup->m_arraySubVtx.push_back( p_arrayVtx[l_uiIndex] );
			//l_return = m_pSubmesh->AddPoint( p_arrayVtx[l_uiIndex].m_val[0], p_arrayVtx[l_uiIndex].m_val[1], p_arrayVtx[l_uiIndex].m_val[2] );

			if ( l_arrayIndex.size() >= 2 )
			{
				// It seems there are more than only vertex index
				string::trim( l_arrayIndex[1] );

				if ( !l_arrayIndex[1].empty() )
				{
					// We treat texture coordinates
					l_uiIndex = DoRetrieveIndex( l_arrayIndex[1], uint32_t( p_arrayTex.size() ) );
					p_pGroup->m_arrayTex.push_back( p_arrayTex[l_uiIndex] );
					//l_return->SetTexCoord( p_arrayTex[l_uiIndex].m_val[0], p_arrayTex[l_uiIndex].m_val[1] );
					stUVW l_uvw = { p_arrayTex[l_uiIndex].m_val[0], p_arrayTex[l_uiIndex].m_val[1], 0.0 };
					p_pGroup->m_arraySubTex.push_back( l_uvw );
				}

				if ( l_arrayIndex.size() >= 3 )
				{
					p_pGroup->m_bHasNormals = true;
					// It seems there are normals index
					string::trim( l_arrayIndex[2] );

					if ( !l_arrayIndex[2].empty() )
					{
						// We treat the found normal
						l_uiIndex = DoRetrieveIndex( l_arrayIndex[2], uint32_t( p_arrayNml.size() ) );
						p_pGroup->m_arrayNml.push_back( p_arrayNml[l_uiIndex] );
						//l_return->SetNormal( p_arrayNml[l_uiIndex].m_val[0], p_arrayNml[l_uiIndex].m_val[1], p_arrayNml[l_uiIndex].m_val[2] );
						p_pGroup->m_arraySubNml.push_back( p_arrayNml[l_uiIndex] );
					}
				}
			}

			l_uiIndex = uint32_t( p_pGroup->m_arraySubVtx.size() ) - 1;
		}

		//	return l_return;
		return l_uiIndex;
	}

	void ObjImporter::DoCreateSubmesh( Castor3D::MeshSPtr p_pMesh, stGROUP * p_pGroup )
	{
		if ( m_pThread )
		{
			m_pThread->join();
			m_pThread.reset();
		}

		if ( p_pGroup )
		{
			if ( p_pGroup->m_arraySubVtx.size() )
			{
				String l_strName = string::lower_case( p_pGroup->m_strName );

				if ( l_strName.find( cuT( "gundummy" ) ) == String::npos && l_strName.find( cuT( "bip01" ) ) == String::npos && l_strName.find( cuT( "fcfx_" ) ) == String::npos && l_strName.find( cuT( "bbone_" ) ) == String::npos && p_pGroup->m_arrayVtx.size() )
				{
					if ( p_pGroup->m_pMaterial )
					{
						struct stTHREAD_CONTEXT
						{
							String m_strName;
							SubmeshSPtr m_pSubmesh;
							MaterialSPtr m_pMaterial;
							VertexArray m_arrayVtx;
							NormalArray m_arrayNml;
							UvwArray m_arrayUvw;
							FaceArray m_arrayFaces;
						};
						stTHREAD_CONTEXT l_context
						{
							l_strName,
							p_pMesh->CreateSubmesh(),
							p_pGroup->m_pMaterial,
							std::move( p_pGroup->m_arraySubVtx ),
							std::move( p_pGroup->m_arraySubNml ),
							std::move( p_pGroup->m_arraySubTex ),
							std::move( p_pGroup->m_arrayFaces )
						};
						m_pThread = std::make_shared< std::thread >( [this]( stTHREAD_CONTEXT p_context )
						{
							Point3f l_ptOffset;
							Point3f l_ptScale;
							Point3f l_ptTurb;
							Point3f l_ptDefaultOffset( 0, 0, 0 );
							Point3f l_ptDefaultScale( 1, 1, 1 );
							Point3f l_ptDefaultTurb( 0, 0, 0 );
							String l_strName;
							VertexSPtr l_pVertex;
							Point2r l_ptTex;
							Coords3r l_ptNml;
							Coords3r l_ptTan;
							Logger::LogDebug( cuT( "Submesh :         " ) + p_context.m_strName );
							Logger::LogDebug( cuT( "-	Vertices :    " ) + string::to_string( uint32_t( p_context.m_arrayVtx.size() ) ) );
							Logger::LogDebug( cuT( "-	Material :    " ) + p_context.m_pMaterial->GetName() );
							// Valid because for each pass of each material we have an entry in those 3 maps
							p_context.m_pSubmesh->SetDefaultMaterial( p_context.m_pMaterial );
							l_ptOffset = m_mapOffsets[p_context.m_pMaterial->GetPass( 0 )];
							l_ptScale = m_mapScales[p_context.m_pMaterial->GetPass( 0 )];
							l_ptTurb = m_mapTurbulences[p_context.m_pMaterial->GetPass( 0 )];
							std::vector< Castor3D::InterleavedVertex > l_submesh{ p_context.m_arrayVtx.size() };

							for ( auto & l_vertex : l_submesh )
							{
								std::memcpy( l_vertex.m_pos.data(), p_context.m_arrayVtx[0].m_val, sizeof( l_vertex.m_pos ) );

								if ( p_context.m_arrayUvw.size() == p_context.m_arrayVtx.size() )
								{
									// if texture coordinates are available, we apply modifiers
									for ( UvwArray::iterator l_it = p_context.m_arrayUvw.begin() ; l_it != p_context.m_arrayUvw.end() ; ++l_it )
									{
										l_it->m_val[0] = ( l_it->m_val[0] + l_ptOffset[0] ) * l_ptScale[0];
										l_it->m_val[1] = ( l_it->m_val[1] + l_ptOffset[1] ) * l_ptScale[1];
										l_it->m_val[2] = ( l_it->m_val[2] + l_ptOffset[2] ) * l_ptScale[2];
									}

									std::memcpy( l_vertex.m_tex.data(), p_context.m_arrayUvw[0].m_val, sizeof( l_vertex.m_tex ) );
								}

								if ( p_context.m_arrayNml.size() == p_context.m_arrayVtx.size() )
								{
									std::memcpy( l_vertex.m_nml.data(), p_context.m_arrayNml[0].m_val, sizeof( l_vertex.m_nml ) );
								}
							}

							p_context.m_pSubmesh->AddPoints( l_submesh );
							p_context.m_pSubmesh->AddFaceGroup( p_context.m_arrayFaces );

							if ( !p_context.m_arrayNml.empty() && p_context.m_arrayNml.size() == p_context.m_arrayVtx.size() )
							{
								p_context.m_pSubmesh->ComputeTangentsFromNormals();
							}
							else
							{
								p_context.m_pSubmesh->ComputeNormals();
							}
						}, l_context );
						p_pGroup->m_arraySubVtx.clear();
						p_pGroup->m_arraySubNml.clear();
						p_pGroup->m_arraySubTex.clear();
						p_pGroup->m_arrayFaces.clear();
					}
				}
			}
		}
	}

	void ObjImporter::DoParseTexParams( String & p_strValue, float * p_offset, float * p_scale, float * p_turb )
	{
		String l_strSrc( p_strValue );
		String l_strParam;
		String l_strValue;
		bool l_bParam = false;
		bool l_bValue = false;
		xchar l_char;
		p_strValue.clear();

		for ( std::size_t i = 0 ; i < l_strSrc.size() ; i++ )
		{
			l_char = l_strSrc[i];

			if ( l_bParam )
			{
				// On a rencontré un tiret, on est en train de récupérer le param
				if ( l_char == cuT( ' ' ) )
				{
					// on est sur un espace, on a fini de récupérer le param
					l_bValue = true;
					l_bParam = false;
				}
				else
				{
					l_strParam += l_char;
				}
			}
			else if ( l_bValue )
			{
				// On a fini de récupérer le param, on est en train de récupérer les valeurs
				if ( l_char == cuT( ' ' ) )
				{
					// On est sur un espace, on vérifie le caractère suivant pour savoir, en fonction du param si c'est une valeur ou pas
					if ( DoIsValidValue( l_strParam, l_strSrc, uint32_t( i + 1 ) ) )
					{
						l_strValue += l_char;
					}
					else
					{
						// Les caractères suivants ne forment pas une valeur valide pour le param
						l_bValue = false;

						if ( l_strParam == cuT( "s" ) )
						{
							DoParseTexParam( l_strParam + cuT( " " ) + l_strValue, p_scale );
						}
						else if ( l_strParam == cuT( "o" ) )
						{
							DoParseTexParam( l_strParam + cuT( " " ) + l_strValue, p_offset );
						}
						else if ( l_strParam == cuT( "t" ) )
						{
							DoParseTexParam( l_strParam + cuT( " " ) + l_strValue, p_turb );
						}
					}
				}
				else
				{
					l_strValue += l_char;
				}
			}
			else if ( l_char == cuT( '-' ) )
			{
				// on est sur un tiret, on commence à récupérer le param
				l_strParam.clear();
				l_bParam = true;
			}
			else
			{
				// On n'est nulle part, on ajoute le caractère au résultat
				p_strValue += l_strSrc.substr( i );
				i = l_strSrc.size();
			}
		}
	}

	void ObjImporter::DoParseTexParam( String const & p_strParam, float * p_values )
	{
		StringArray l_arraySplitted = string::split( p_strParam, cuT( " " ) );
		std::size_t l_uiMax = std::min< std::size_t >( 4, l_arraySplitted.size() );

		for ( std::size_t i = 1 ; i < l_uiMax ; i++ )
		{
			p_values[i - 1] = string::to_float( l_arraySplitted[i] );
		}
	}

	bool ObjImporter::DoIsValidValue( String const & p_strParam, String const & p_strSrc, uint32_t p_uiIndex )
	{
		bool l_bReturn = false;
		StringArray l_arraySplitted;

		if ( p_uiIndex < p_strSrc.size() )
		{
			l_arraySplitted = string::split( p_strSrc.substr( p_uiIndex ), cuT( " " ), 2 );

			if ( l_arraySplitted.size() > 1 )
			{
				if ( p_strParam == cuT( "s" ) || p_strParam == cuT( "o" ) || p_strParam == cuT( "t" ) )
				{
					l_bReturn = string::is_floating( l_arraySplitted[0] );
				}
				else if ( p_strParam == cuT( "blendu" ) || p_strParam == cuT( "blendv" ) || p_strParam == cuT( "cc" ) || p_strParam == cuT( "clamp" ) )
				{
					l_bReturn = ( l_arraySplitted[0] == cuT( "on" ) || l_arraySplitted[0] == cuT( "off" ) );
				}
				else if ( p_strParam == cuT( "texres" ) )
				{
					l_bReturn = string::is_integer( l_arraySplitted[0] );
				}
				else if ( p_strParam == cuT( "bm" ) )
				{
					l_bReturn = string::is_floating( l_arraySplitted[0] );
				}
				else if ( p_strParam == cuT( "mm" ) )
				{
					l_bReturn = string::is_floating( l_arraySplitted[0] );
				}
				else if ( p_strParam == cuT( "imfchan" ) )
				{
					l_bReturn = ( l_arraySplitted[0] == cuT( "r" ) || l_arraySplitted[0] == cuT( "g" ) || l_arraySplitted[0] == cuT( "b" ) || l_arraySplitted[0] == cuT( "m" ) || l_arraySplitted[0] == cuT( "l" ) || l_arraySplitted[0] == cuT( "z" ) );
				}
			}
		}

		return l_bReturn;
	}

	void ObjImporter::DoReadMaterials( Scene & p_scene, Path const & p_pathMatFile )
	{
		String l_strLine;
		String l_strSection;
		String l_strValue;
		StringArray l_arraySplitted;
		PassSPtr l_pPass;
		MaterialSPtr l_pMaterial;
		float l_components[3];
		float l_fAlpha = 1.0f;
		bool l_bOpaFound = false;
		auto & l_mtlManager = p_scene.GetMaterialView();
		TextFile l_matFile( p_pathMatFile, File::eOPEN_MODE_READ );

		while ( l_matFile.IsOk() )
		{
			l_matFile.ReadLine( l_strLine, 1000 );

			if ( !l_strLine.empty() )
			{
				l_arraySplitted = string::split( l_strLine, cuT( " " ), 1 );

				if ( l_arraySplitted.size() >= 1 )
				{
					l_strSection = l_arraySplitted[0];

					if ( l_arraySplitted.size() > 1 )
					{
						l_strValue = l_arraySplitted[1];
					}
					else
					{
						l_strValue.clear();
					}

					string::trim( l_strValue );
					string::trim( string::to_lower_case( l_strSection ) );

					if ( l_strSection == cuT( "newmtl" ) )
					{
						// New material description
						if ( l_pPass )
						{
							if ( l_fAlpha < 1.0 )
							{
								l_pPass->SetAlpha( l_fAlpha );
							}

							l_fAlpha = 1.0f;
						}

						if ( l_mtlManager.Has( l_strValue ) )
						{
							l_pMaterial = l_mtlManager.Find( l_strValue );
						}
						else
						{
							l_pMaterial.reset();
						}

						if ( !l_pMaterial )
						{
							l_pMaterial = l_mtlManager.Create( l_strValue, *GetEngine() );
							m_arrayLoadedMaterials.push_back( l_pMaterial );
						}

						l_pPass = l_pMaterial->CreatePass();
						l_pPass->SetTwoSided( true );
						l_bOpaFound = false;
						l_fAlpha = 1.0f;
						Logger::LogDebug( cuT( "Material : " ) + l_strValue );
					}
					else if ( l_strSection == cuT( "illum" ) )
					{
						// Illumination description
					}
					else if ( l_strSection == cuT( "ka" ) )
					{
						// Ambient colour
						StringStream l_stream( l_strValue );
						l_stream >> l_components[0] >> l_components[1] >> l_components[2];
						l_pPass->SetAmbient( Castor::Colour::from_components( l_components[0], l_components[1], l_components[2], real( 1.0 ) ) );
					}
					else if ( l_strSection == cuT( "kd" ) )
					{
						// Diffuse colour
						StringStream l_stream( l_strValue );
						l_stream >> l_components[0] >> l_components[1] >> l_components[2];
						l_pPass->SetDiffuse( Castor::Colour::from_components( l_components[0], l_components[1], l_components[2], real( 1.0 ) ) );
					}
					else if ( l_strSection == cuT( "ks" ) )
					{
						// Specular colour
						StringStream l_stream( l_strValue );
						l_stream >> l_components[0] >> l_components[1] >> l_components[2];
						l_pPass->SetSpecular( Castor::Colour::from_components( l_components[0], l_components[1], l_components[2], real( 1.0 ) ) );
					}
					else if ( l_strSection == cuT( "tr" ) || l_strSection == cuT( "d" ) )
					{
						// Opacity
						if ( !l_bOpaFound )
						{
							l_bOpaFound = true;
							l_fAlpha = string::to_float( l_strValue );
						}
					}
					else if ( l_strSection == cuT( "ns" ) )
					{
						// Shininess
						l_pPass->SetShininess( string::to_float( l_strValue ) );
					}
					else if ( l_strSection == cuT( "map_kd" ) )
					{
						// Diffuse map
						DoAddTexture( l_strValue, l_pPass, TextureChannel::Diffuse );
					}
					else if ( l_strSection == cuT( "bump" ) || l_strSection == cuT( "map_bump" ) )
					{
						// Normal map
						DoAddTexture( l_strValue, l_pPass, TextureChannel::Normal );
					}
					else if ( l_strSection == cuT( "map_d" ) || l_strSection == cuT( "map_opacity" ) )
					{
						// Opacity map
						DoAddTexture( l_strValue, l_pPass, TextureChannel::Opacity );
					}
					else if ( l_strSection == cuT( "refl" ) )
					{
						// Reflection map
					}
					else if ( l_strSection == cuT( "map_ks" ) )
					{
						// Specular map
						DoAddTexture( l_strValue, l_pPass, TextureChannel::Specular );
					}
					else if ( l_strSection == cuT( "map_ka" ) )
					{
						// Ambient map
						DoAddTexture( l_strValue, l_pPass, TextureChannel::Ambient );
					}
					else if ( l_strSection == cuT( "map_ns" ) )
					{
						// Gloss/Shininess map
						DoAddTexture( l_strValue, l_pPass, TextureChannel::Gloss );
					}
				}
			}
		}

		if ( l_pPass && l_bOpaFound )
		{
			l_pPass->SetAlpha( l_fAlpha );
		}
	}
}
