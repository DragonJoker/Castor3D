#include "ObjImporter.hpp"

#include "ObjGroup.hpp"

#include <Graphics/Colour.hpp>
#include <Graphics/Image.hpp>

#include <Engine.hpp>
#include <Cache/GeometryCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Obj
{
	namespace
	{
		void doParseFaceVertex( std::string const & faceVtx
			, Point3rArray const & allvtx
			, Point2rArray const & alltex
			, Point3rArray const & allnml
			, InterleavedVertexArray::iterator vit )
		{
			size_t index1 = faceVtx.find( '/' );
			std::string component = faceVtx.substr( 0, index1 );
			uint32_t iv = string::to_int( component ) - 1;
			vit->m_pos[0] = allvtx[iv][0];
			vit->m_pos[1] = allvtx[iv][1];
			vit->m_pos[2] = allvtx[iv][2];

			++index1;
			size_t index2 = faceVtx.find( '/', index1 );
			component = faceVtx.substr( index1, index2 - index1 );

			if ( !component.empty() )
			{
				uint32_t ivt = string::to_int( component ) - 1;
				vit->m_tex[0] = alltex[ivt][0];
				vit->m_tex[1] = alltex[ivt][1];
			}

			if ( index2 != std::string::npos )
			{
				++index2;
				component = faceVtx.substr( index2 );

				if ( !component.empty() )
				{
					uint32_t ivn = string::to_int( component ) - 1;
					vit->m_nml[0] = allnml[ivn][0];
					vit->m_nml[1] = allnml[ivn][1];
					vit->m_nml[2] = allnml[ivn][2];
				}
			}
		}
	}

	ObjImporter::ObjImporter( Engine & engine )
		: Importer( engine )
		, m_collImages( engine.GetImageCache() )
		, m_pFile( nullptr )
	{
	}

	ImporterUPtr ObjImporter::Create( Engine & engine )
	{
		return std::make_unique< ObjImporter >( engine );
	}

	bool ObjImporter::DoImportScene( Scene & scene )
	{
		auto mesh = scene.GetMeshCache().Add( cuT( "Mesh_OBJ" ) );
		bool result = DoImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = scene.GetSceneNodeCache().Add( mesh->GetName(), scene.GetObjectRootNode() );
			GeometrySPtr geometry = scene.GetGeometryCache().Add( mesh->GetName(), node, nullptr );
			geometry->SetMesh( mesh );
			m_geometries.insert( { geometry->GetName(), geometry } );
		}

		m_arrayLoadedMaterials.clear();
		m_arrayTextures.clear();
		return result;
	}

	bool ObjImporter::DoImportMesh( Mesh & mesh )
	{
		bool result{ false };

		try
		{
			TextFile file( m_fileName, File::OpenMode::eRead );

			if ( file.IsOk() )
			{
				m_pFile = &file;
				DoReadObjFile( mesh );
				m_arrayLoadedMaterials.clear();
				m_arrayTextures.clear();
			}

			if ( m_pThread )
			{
				m_pThread->join();
				m_pThread.reset();
			}

			result = true;
		}
		catch ( std::exception & exc )
		{
			Logger::LogWarning( std::stringstream() << "Encountered exception while importing mesh: " << exc.what() );
		}

		return result;
	}

	void ObjImporter::DoAddTexture( String const & strValue
		, Pass & pass
		, TextureChannel channel )
	{
		Point3f offset( 0, 0, 0 );
		Point3f scale( 1, 1, 1 );
		Point3f turbulence( 0, 0, 0 );
		ImageSPtr pImage;
		String value( strValue );
		DoParseTexParams( value, offset.ptr(), scale.ptr(), turbulence.ptr() );
		m_mapOffsets[&pass] = offset;
		m_mapScales[&pass] = scale;
		m_mapTurbulences[&pass] = turbulence;
		Logger::LogDebug( StringStream() << cuT( "-	Texture :    " ) + value );
		Logger::LogDebug( StringStream() << cuT( "-	Offset :     " ) << offset );
		Logger::LogDebug( StringStream() << cuT( "-	Scale :      " ) << scale );
		Logger::LogDebug( StringStream() << cuT( "-	Turbulence : " ) << turbulence );

		if ( !value.empty() )
		{
			auto texture = LoadTexture( Path{ value }, pass, channel );

			if ( texture )
			{
				m_arrayTextures.push_back( texture->GetTexture() );
			}
		}
	}

	void ObjImporter::DoReadObjFile( Mesh & mesh )
	{
		std::ifstream file( m_fileName.c_str() );
		std::string line;
		std::string mtlfile;
		uint32_t nv = 0u;
		uint32_t nvt = 0u;
		uint32_t nvn = 0u;
		uint32_t nf = 0u;
		uint32_t ntf = 0u;
		uint32_t ng = 0u;
		std::vector< uint32_t > faces;

		while ( std::getline( file, line ) )
		{
			string::trim( line );
			std::stringstream stream{ line };
			std::string ident;
			stream >> ident;

			if ( ident == "v" )
			{
				if ( ntf )
				{
					faces.push_back( ntf );
					ntf = 0u;
				}

				++nv;
			}
			else if ( ident == "vt" )
			{
				++nvt;
			}
			else if ( ident == "vn" )
			{
				++nvn;
			}
			else if ( ident == "f" )
			{
				auto count = 0u;

				for ( uint32_t i = 0u; i < 4u; i++ )
				{
					std::string faceVtx;
					stream >> faceVtx;

					if ( !faceVtx.empty() )
					{
						++count;
					}
				}

				++nf;
				++ntf;

				if ( count == 4u )
				{
					++nf;
					++ntf;
				}
			}
			else if ( ident == "g" || ident == "usemtl" )
			{
				if ( ntf )
				{
					faces.push_back( ntf );
					ntf = 0u;
				}
			}
			else if ( ident == "mtllib" )
			{
				mtlfile = line.substr( line.find_first_of( " " ) + 1 );
				string::trim( mtlfile );
			}
		}

		if ( ntf )
		{
			faces.push_back( ntf );
		}

		// Material description file
		if ( File::FileExists( m_filePath / mtlfile ) )
		{
			DoReadMaterials( mesh, m_filePath / mtlfile );
		}
		else
		{
			Logger::LogWarning( cuT( "Mtl file " ) + m_filePath / mtlfile + cuT( " doesn't exist" ) );
		}

		file.clear();
		file.seekg( 0, std::ios::beg );

		Point3rArray allvtx( nv );
		Point2rArray alltex( nvt );
		Point3rArray allnml( nvn );
		Point3rArray::iterator vtxit = allvtx.begin();
		Point2rArray::iterator texit = alltex.begin();
		Point3rArray::iterator nmlit = allnml.begin();

		std::cout << "    Vertex count: " << nv << std::endl;
		std::cout << "    TexCoord count: " << nvt << std::endl;
		std::cout << "    Normal count: " << nvn << std::endl;
		std::cout << "    Group count: " << faces.size() << std::endl;

		while ( std::getline( file, line ) )
		{
			string::trim( line );
			std::stringstream stream( line );
			std::string ident;
			stream >> ident;

			if ( ident == "v" )
			{
				stream >> ( *vtxit )[0] >> ( *vtxit )[1] >> ( *vtxit )[2];
				++vtxit;
			}
			else if ( ident == "vt" )
			{
				stream >> ( *texit )[0] >> ( *texit )[1];
				++texit;
			}
			else if ( ident == "vn" )
			{
				stream >> ( *nmlit )[0] >> ( *nmlit )[1] >> ( *nmlit )[2];
				++nmlit;
			}
		}

		file.clear();
		file.seekg( 0, std::ios::beg );

		InterleavedVertexArray vertex{ nf * 3 };
		std::vector< FaceIndices > index{ nf };
		auto vit = vertex.begin();
		auto fit = index.begin();
		auto facesit = faces.end();
		uint32_t vtxIndex{ 0u };
		std::string mtlname;

		while ( std::getline( file, line ) )
		{
			string::trim( line );
			std::stringstream stream( line );
			std::string ident;
			stream >> ident;

			if ( ident == "g" )
			{
				if ( facesit == faces.end() )
				{
					facesit = faces.begin();
				}
				else
				{
					DoCreateSubmesh( mesh
						, mtlname
						, allnml.empty()
						, std::vector< FaceIndices >{ index.begin(), fit }
						, InterleavedVertexArray{ vertex.begin(), vit } );
					vit = vertex.begin();
					fit = index.begin();
					vtxIndex = 0u;
					++facesit;
				}

				std::cout << "    Group faces count: " << *facesit << std::endl;
			}
			else if ( ident == "usemtl" )
			{
				if ( vertex.begin() != vit )
				{
					DoCreateSubmesh( mesh
						, mtlname
						, allnml.empty()
						, std::vector< FaceIndices >{ index.begin(), fit }
						, InterleavedVertexArray{ vertex.begin(), vit } );
					vit = vertex.begin();
					fit = index.begin();
					vtxIndex = 0u;
					++facesit;
				}

				stream >> mtlname;
			}
			else if ( ident == "f" )
			{
				auto count = 0u;

				for ( uint32_t i = 0u; i < 4u; i++ )
				{
					std::string faceVtx;
					stream >> faceVtx;

					if ( !faceVtx.empty() )
					{
						doParseFaceVertex( faceVtx
							, allvtx
							, alltex
							, allnml
							, vit );
						++vit;
						++count;
					}
				}

				if ( count == 3u )
				{
					fit->m_index[0] = vtxIndex++;
					fit->m_index[1] = vtxIndex++;
					fit->m_index[2] = vtxIndex++;
					++fit;
				}
				else
				{
					auto vtx1 = vtxIndex++;
					auto vtx2 = vtxIndex++;
					auto vtx3 = vtxIndex++;
					auto vtx4 = vtxIndex++;

					fit->m_index[0] = vtx1;
					fit->m_index[1] = vtx2;
					fit->m_index[2] = vtx3;
					++fit;

					fit->m_index[0] = vtx1;
					fit->m_index[1] = vtx3;
					fit->m_index[2] = vtx4;
					++fit;
				}
			}
		}

		if ( vit != vertex.begin()
			&& fit != index.begin() )
		{
			DoCreateSubmesh( mesh
				, mtlname
				, allnml.empty()
				, std::vector< FaceIndices > { index.begin(), fit }
				, InterleavedVertexArray{ vertex.begin(), vit } );
		}
	}

	void ObjImporter::DoCreateSubmesh( Castor3D::Mesh & mesh
		, Castor::String const & mtlName
		, bool normals
		, std::vector< FaceIndices > && faces
		, Castor3D::InterleavedVertexArray && vertex )
	{
		auto submesh = mesh.CreateSubmesh();
		submesh->SetDefaultMaterial( mesh.GetScene()->GetMaterialView().Find( mtlName ) );
		submesh->AddPoints( vertex );
		submesh->AddFaceGroup( faces );

		if ( normals )
		{
			submesh->ComputeNormals();
		}

		submesh->ComputeTangentsFromNormals();
	}

	void ObjImporter::DoParseTexParams( String & strValue
		, float * offset
		, float * scale
		, float * turb )
	{
		String strSrc( strValue );
		String strParam;
		String value;
		bool bParam = false;
		bool bValue = false;
		strValue.clear();

		for ( std::size_t i = 0 ; i < strSrc.size() ; i++ )
		{
			auto xc = strSrc[i];

			if ( bParam )
			{
				// On a rencontré un tiret, on est en train de récupérer le param
				if ( xc == cuT( ' ' ) )
				{
					// on est sur un espace, on a fini de récupérer le param
					bValue = true;
					bParam = false;
				}
				else
				{
					strParam += xc;
				}
			}
			else if ( bValue )
			{
				// On a fini de récupérer le param, on est en train de récupérer les valeurs
				if ( xc == cuT( ' ' ) )
				{
					// On est sur un espace, on vérifie le caractère suivant pour savoir, en fonction du param si c'est une valeur ou pas
					if ( DoIsValidValue( strParam, strSrc, uint32_t( i + 1 ) ) )
					{
						value += xc;
					}
					else
					{
						// Les caractères suivants ne forment pas une valeur valide pour le param
						bValue = false;

						if ( strParam == cuT( "s" ) )
						{
							DoParseTexParam( strParam + cuT( " " ) + value, scale );
						}
						else if ( strParam == cuT( "o" ) )
						{
							DoParseTexParam( strParam + cuT( " " ) + value, offset );
						}
						else if ( strParam == cuT( "t" ) )
						{
							DoParseTexParam( strParam + cuT( " " ) + value, turb );
						}
					}
				}
				else
				{
					value += xc;
				}
			}
			else if ( xc == cuT( '-' ) )
			{
				// on est sur un tiret, on commence à récupérer le param
				strParam.clear();
				bParam = true;
			}
			else
			{
				// On n'est nulle part, on ajoute le caractère au résultat
				strValue += strSrc.substr( i );
				i = strSrc.size();
			}
		}
	}

	void ObjImporter::DoParseTexParam( String const & strParam
		, float * values )
	{
		StringArray arraySplitted = string::split( strParam, cuT( " " ) );
		std::size_t uiMax = std::min< std::size_t >( 4, arraySplitted.size() );

		for ( std::size_t i = 1 ; i < uiMax ; i++ )
		{
			values[i - 1] = string::to_float( arraySplitted[i] );
		}
	}

	bool ObjImporter::DoIsValidValue( String const & strParam
		, String const & strSrc
		, uint32_t index )
	{
		bool result = false;
		StringArray arraySplitted;

		if ( index < strSrc.size() )
		{
			arraySplitted = string::split( strSrc.substr( index ), cuT( " " ), 2 );

			if ( arraySplitted.size() > 1 )
			{
				if ( strParam == cuT( "s" ) || strParam == cuT( "o" ) || strParam == cuT( "t" ) )
				{
					result = string::is_floating( arraySplitted[0] );
				}
				else if ( strParam == cuT( "blendu" )
					|| strParam == cuT( "blendv" )
					|| strParam == cuT( "cc" )
					|| strParam == cuT( "clamp" ) )
				{
					result = ( arraySplitted[0] == cuT( "on" ) || arraySplitted[0] == cuT( "off" ) );
				}
				else if ( strParam == cuT( "texres" ) )
				{
					result = string::is_integer( arraySplitted[0] );
				}
				else if ( strParam == cuT( "bm" ) )
				{
					result = string::is_floating( arraySplitted[0] );
				}
				else if ( strParam == cuT( "mm" ) )
				{
					result = string::is_floating( arraySplitted[0] );
				}
				else if ( strParam == cuT( "imfchan" ) )
				{
					result = ( arraySplitted[0] == cuT( "r" )
						|| arraySplitted[0] == cuT( "g" )
						|| arraySplitted[0] == cuT( "b" )
						|| arraySplitted[0] == cuT( "m" )
						|| arraySplitted[0] == cuT( "l" )
						|| arraySplitted[0] == cuT( "z" ) );
				}
			}
		}

		return result;
	}

	void ObjImporter::DoReadMaterials( Mesh & mesh
		, Path const & pathMatFile )
	{
		String strLine;
		String section;
		String value;
		StringArray arraySplitted;
		LegacyPassSPtr pass;
		MaterialSPtr material;
		float components[3];
		float fAlpha = 1.0f;
		bool bOpaFound = false;
		TextFile matFile( pathMatFile, File::OpenMode::eRead );

		while ( matFile.IsOk() )
		{
			matFile.ReadLine( strLine, 1000 );

			if ( !strLine.empty() )
			{
				arraySplitted = string::split( strLine, cuT( " " ), 1 );

				if ( arraySplitted.size() >= 1 )
				{
					section = arraySplitted[0];

					if ( arraySplitted.size() > 1 )
					{
						value = arraySplitted[1];
					}
					else
					{
						value.clear();
					}

					string::trim( value );
					string::trim( string::to_lower_case( section ) );

					if ( section == cuT( "newmtl" ) )
					{
						// New material description
						if ( pass )
						{
							if ( fAlpha < 1.0 )
							{
								pass->SetOpacity( fAlpha );
							}

							fAlpha = 1.0f;
						}

						if ( mesh.GetScene()->GetMaterialView().Has( value ) )
						{
							material = mesh.GetScene()->GetMaterialView().Find( value );
							REQUIRE( material->GetType() == MaterialType::eLegacy );
							pass = material->GetTypedPass< MaterialType::eLegacy >( 0u );
						}
						else
						{
							material.reset();
						}

						if ( !material )
						{
							material = mesh.GetScene()->GetMaterialView().Add( value, MaterialType::eLegacy );
							material->CreatePass();
							pass = material->GetTypedPass< MaterialType::eLegacy >( 0u );
							m_arrayLoadedMaterials.push_back( material );
						}

						pass->SetTwoSided( true );
						bOpaFound = false;
						fAlpha = 1.0f;
						Logger::LogDebug( cuT( "Material : " ) + value );
					}
					else if ( section == cuT( "illum" ) )
					{
						// Illumination description
					}
					else if ( section == cuT( "kd" ) )
					{
						// Diffuse colour
						StringStream stream( value );
						stream >> components[0] >> components[1] >> components[2];
						pass->SetDiffuse( Castor::Colour::from_components( components[0], components[1], components[2], real( 1.0 ) ) );
					}
					else if ( section == cuT( "ks" ) )
					{
						// Specular colour
						StringStream stream( value );
						stream >> components[0] >> components[1] >> components[2];
						pass->SetSpecular( Castor::Colour::from_components( components[0], components[1], components[2], real( 1.0 ) ) );
					}
					else if ( section == cuT( "tr" ) || section == cuT( "d" ) )
					{
						// Opacity
						if ( !bOpaFound )
						{
							bOpaFound = true;
							fAlpha = string::to_float( value );
						}
					}
					else if ( section == cuT( "ns" ) )
					{
						// Shininess
						pass->SetShininess( string::to_float( value ) );
					}
					else if ( section == cuT( "map_kd" ) )
					{
						// Diffuse map
						DoAddTexture( value, *pass, TextureChannel::eDiffuse );
					}
					else if ( section == cuT( "bump" ) || section == cuT( "map_bump" ) )
					{
						// Normal map
						DoAddTexture( value, *pass, TextureChannel::eNormal );
					}
					else if ( section == cuT( "map_d" ) || section == cuT( "map_opacity" ) )
					{
						// Opacity map
						DoAddTexture( value, *pass, TextureChannel::eOpacity );
					}
					else if ( section == cuT( "refl" ) )
					{
						// Reflection map
					}
					else if ( section == cuT( "map_ks" ) )
					{
						// Specular map
						DoAddTexture( value, *pass, TextureChannel::eSpecular );
					}
					else if ( section == cuT( "map_ns" ) )
					{
						// Gloss/Shininess map
						DoAddTexture( value, *pass, TextureChannel::eGloss );
					}
				}
			}
		}

		if ( pass && bOpaFound )
		{
			pass->SetOpacity( fAlpha );
		}
	}
}
