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

using namespace castor3d;
using namespace castor;

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
			std::string components = faceVtx.substr( 0, index1 );
			uint32_t iv = string::toInt( components ) - 1;
			vit->m_pos[0] = allvtx[iv][0];
			vit->m_pos[1] = allvtx[iv][1];
			vit->m_pos[2] = allvtx[iv][2];

			++index1;
			size_t index2 = faceVtx.find( '/', index1 );
			components = faceVtx.substr( index1, index2 - index1 );

			if ( !components.empty() )
			{
				uint32_t ivt = string::toInt( components ) - 1;
				vit->m_tex[0] = alltex[ivt][0];
				vit->m_tex[1] = alltex[ivt][1];
			}

			if ( index2 != std::string::npos )
			{
				++index2;
				components = faceVtx.substr( index2 );

				if ( !components.empty() )
				{
					uint32_t ivn = string::toInt( components ) - 1;
					vit->m_nml[0] = allnml[ivn][0];
					vit->m_nml[1] = allnml[ivn][1];
					vit->m_nml[2] = allnml[ivn][2];
				}
			}
		}

		void doParseVec3( StringArray const & content, Point3rArray & array )
		{
			array.resize( content.size() );
			auto it = array.begin();

			for ( auto & line : content )
			{
				std::stringstream stream( line );
				std::string ident;
				stream >> ident;
				stream >> ( *it )[0] >> ( *it )[1] >> ( *it )[2];
				++it;
			}
		}

		void doParseVec2( StringArray const & content, Point2rArray & array )
		{
			array.resize( content.size() );
			auto it = array.begin();

			for ( auto & line : content )
			{
				std::stringstream stream( line );
				std::string ident;
				stream >> ident;
				stream >> ( *it )[0] >> ( *it )[1];
				++it;
			}
		}
	}

	ObjImporter::ObjImporter( Engine & engine )
		: Importer( engine )
		, m_collImages( engine.getImageCache() )
		, m_pFile( nullptr )
	{
	}

	ImporterUPtr ObjImporter::create( Engine & engine )
	{
		return std::make_unique< ObjImporter >( engine );
	}

	bool ObjImporter::doImportScene( Scene & scene )
	{
		auto mesh = scene.getMeshCache().add( cuT( "Mesh_OBJ" ) );
		bool result = doImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = scene.getSceneNodeCache().add( mesh->getName(), scene.getObjectRootNode() );
			GeometrySPtr geometry = scene.getGeometryCache().add( mesh->getName(), node, nullptr );
			geometry->setMesh( mesh );
			m_geometries.insert( { geometry->getName(), geometry } );
		}

		m_arrayLoadedMaterials.clear();
		m_arrayTextures.clear();
		return result;
	}

	bool ObjImporter::doImportMesh( Mesh & mesh )
	{
		bool result{ false };

		try
		{
			TextFile file( m_fileName, File::OpenMode::eRead );

			if ( file.isOk() )
			{
				m_pFile = &file;
				doReadObjFile( mesh );
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
			Logger::logWarning( std::stringstream() << "Encountered exception while importing mesh: " << exc.what() );
		}

		return result;
	}

	void ObjImporter::doAddTexture( String const & strValue
		, Pass & pass
		, TextureChannel channel )
	{
		Point3f offset( 0, 0, 0 );
		Point3f scale( 1, 1, 1 );
		Point3f turbulence( 0, 0, 0 );
		ImageSPtr pImage;
		String value( strValue );
		doParseTexParams( value, offset.ptr(), scale.ptr(), turbulence.ptr() );
		m_mapOffsets[&pass] = offset;
		m_mapScales[&pass] = scale;
		m_mapTurbulences[&pass] = turbulence;
		Logger::logDebug( StringStream() << cuT( "-	Texture :    " ) + value );
		Logger::logDebug( StringStream() << cuT( "-	Offset :     " ) << offset );
		Logger::logDebug( StringStream() << cuT( "-	Scale :      " ) << scale );
		Logger::logDebug( StringStream() << cuT( "-	Turbulence : " ) << turbulence );

		if ( !value.empty() )
		{
			auto texture = loadTexture( Path{ value }, pass, channel );

			if ( texture )
			{
				m_arrayTextures.push_back( texture->getTexture() );
			}
		}
	}

	void ObjImporter::doReadObjFile( Mesh & mesh )
	{
		std::ifstream file( m_fileName.c_str() );
		file.seekg( 0, std::ios::end );
		size_t size = file.tellg();
		file.seekg( 0, std::ios::beg );
		std::string content;
		content.resize( size + 1 );
		file.read( &content[0], size );
		auto lines = std::count( content.begin(), content.end(), '\n' );
		content.clear();
		file.clear();
		file.seekg( 0, std::ios::beg );
		std::string line;
		std::string mtlfile;
		uint32_t nf = 0u;
		uint32_t ntf = 0u;
		std::vector< uint32_t > faces;
		StringArray v;
		StringArray vt;
		StringArray vn;
		StringArray f;
		v.reserve( lines );
		vt.reserve( lines );
		vn.reserve( lines );
		f.reserve( lines );

		while ( std::getline( file, line ) )
		{
			string::trim( line );
			std::stringstream stream{ line };
			std::string ident;
			stream >> ident;

			if ( ident == "v" )
			{
				v.push_back( line );

				if ( ntf )
				{
					faces.push_back( ntf );
					ntf = 0u;
				}
			}
			else if ( ident == "vt" )
			{
				vt.push_back( line );
			}
			else if ( ident == "vn" )
			{
				vn.push_back( line );
			}
			else if ( ident == "f" )
			{
				f.push_back( line );
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
				f.push_back( line );

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
		if ( File::fileExists( m_filePath / mtlfile ) )
		{
			doReadMaterials( mesh, m_filePath / mtlfile );
		}
		else
		{
			Logger::logWarning( cuT( "Mtl file " ) + m_filePath / mtlfile + cuT( " doesn't exist" ) );
		}

		file.clear();
		file.seekg( 0, std::ios::beg );

		Point3rArray allvtx;
		doParseVec3( v, allvtx );
		v = StringArray{};
		Point2rArray alltex;
		doParseVec2( vt, alltex );
		vt = StringArray{};
		Point3rArray allnml;
		doParseVec3( vn, allnml );
		vn = StringArray{};

		std::cout << "    Vertex count: " << allvtx.size() << std::endl;
		std::cout << "    TexCoord count: " << alltex.size() << std::endl;
		std::cout << "    Normal count: " << allnml.size() << std::endl;
		std::cout << "    Group count: " << faces.size() << std::endl;

		InterleavedVertexArray vertex{ nf * 3 };
		std::vector< FaceIndices > index{ nf };
		auto vit = vertex.begin();
		auto fit = index.begin();
		auto facesit = faces.end();
		uint32_t vtxIndex{ 0u };
		std::string mtlname;

		for ( auto & line : f )
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
					doCreateSubmesh( mesh
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
					doCreateSubmesh( mesh
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
			doCreateSubmesh( mesh
				, mtlname
				, allnml.empty()
				, std::vector< FaceIndices > { index.begin(), fit }
				, InterleavedVertexArray{ vertex.begin(), vit } );
		}
	}

	void ObjImporter::doCreateSubmesh( castor3d::Mesh & mesh
		, castor::String const & mtlName
		, bool normals
		, std::vector< FaceIndices > && faces
		, castor3d::InterleavedVertexArray && vertex )
	{
		auto submesh = mesh.createSubmesh();
		submesh->setDefaultMaterial( mesh.getScene()->getMaterialView().find( mtlName ) );
		submesh->addPoints( vertex );
		submesh->addFaceGroup( faces );

		if ( normals )
		{
			submesh->computeNormals();
		}

		submesh->computeTangentsFromNormals();
	}

	void ObjImporter::doParseTexParams( String & strValue
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
					if ( doIsValidValue( strParam, strSrc, uint32_t( i + 1 ) ) )
					{
						value += xc;
					}
					else
					{
						// Les caractères suivants ne forment pas une valeur valide pour le param
						bValue = false;

						if ( strParam == cuT( "s" ) )
						{
							doParseTexParam( strParam + cuT( " " ) + value, scale );
						}
						else if ( strParam == cuT( "o" ) )
						{
							doParseTexParam( strParam + cuT( " " ) + value, offset );
						}
						else if ( strParam == cuT( "t" ) )
						{
							doParseTexParam( strParam + cuT( " " ) + value, turb );
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

	void ObjImporter::doParseTexParam( String const & strParam
		, float * values )
	{
		StringArray arraySplitted = string::split( strParam, cuT( " " ) );
		std::size_t uiMax = std::min< std::size_t >( 4, arraySplitted.size() );

		for ( std::size_t i = 1 ; i < uiMax ; i++ )
		{
			values[i - 1] = string::toFloat( arraySplitted[i] );
		}
	}

	bool ObjImporter::doIsValidValue( String const & strParam
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
					result = string::isFloating( arraySplitted[0] );
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
					result = string::isInteger( arraySplitted[0] );
				}
				else if ( strParam == cuT( "bm" ) )
				{
					result = string::isFloating( arraySplitted[0] );
				}
				else if ( strParam == cuT( "mm" ) )
				{
					result = string::isFloating( arraySplitted[0] );
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

	void ObjImporter::doReadMaterials( Mesh & mesh
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

		while ( matFile.isOk() )
		{
			matFile.readLine( strLine, 1000 );

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
					string::trim( string::toLowerCase( section ) );

					if ( section == cuT( "newmtl" ) )
					{
						// New material description
						if ( pass )
						{
							if ( fAlpha < 1.0 )
							{
								pass->setOpacity( fAlpha );
							}

							fAlpha = 1.0f;
						}

						if ( mesh.getScene()->getMaterialView().has( value ) )
						{
							material = mesh.getScene()->getMaterialView().find( value );
							REQUIRE( material->getType() == MaterialType::eLegacy );
							pass = material->getTypedPass< MaterialType::eLegacy >( 0u );
						}
						else
						{
							material.reset();
						}

						if ( !material )
						{
							material = mesh.getScene()->getMaterialView().add( value, MaterialType::eLegacy );
							material->createPass();
							pass = material->getTypedPass< MaterialType::eLegacy >( 0u );
							m_arrayLoadedMaterials.push_back( material );
						}

						pass->setTwoSided( true );
						bOpaFound = false;
						fAlpha = 1.0f;
						Logger::logDebug( cuT( "Material : " ) + value );
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
						pass->setDiffuse( castor::RgbColour::fromComponents( components[0], components[1], components[2]) );
					}
					else if ( section == cuT( "ks" ) )
					{
						// Specular colour
						StringStream stream( value );
						stream >> components[0] >> components[1] >> components[2];
						pass->setSpecular( castor::RgbColour::fromComponents( components[0], components[1], components[2] ) );
					}
					else if ( section == cuT( "tr" ) || section == cuT( "d" ) )
					{
						// Opacity
						if ( !bOpaFound )
						{
							bOpaFound = true;
							fAlpha = string::toFloat( value );
						}
					}
					else if ( section == cuT( "ns" ) )
					{
						// Shininess
						pass->setShininess( string::toFloat( value ) );
					}
					else if ( section == cuT( "map_kd" ) )
					{
						// Diffuse map
						doAddTexture( value, *pass, TextureChannel::eDiffuse );
					}
					else if ( section == cuT( "bump" ) || section == cuT( "map_bump" ) )
					{
						// Normal map
						doAddTexture( value, *pass, TextureChannel::eNormal );
					}
					else if ( section == cuT( "map_d" ) || section == cuT( "map_opacity" ) )
					{
						// Opacity map
						doAddTexture( value, *pass, TextureChannel::eOpacity );
					}
					else if ( section == cuT( "refl" ) )
					{
						// Reflection map
					}
					else if ( section == cuT( "map_ks" ) )
					{
						// Specular map
						doAddTexture( value, *pass, TextureChannel::eSpecular );
					}
					else if ( section == cuT( "map_ns" ) )
					{
						// Gloss/Shininess map
						doAddTexture( value, *pass, TextureChannel::eGloss );
					}
				}
			}
		}

		if ( pass && bOpaFound )
		{
			pass->setOpacity( fAlpha );
		}
	}
}
