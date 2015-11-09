#include "AssimpImporter.hpp"

#include <InitialiseEvent.hpp>
#include <ImporterPlugin.hpp>
#include <MaterialManager.hpp>
#include <Bone.hpp>
#include <MeshManager.hpp>
#include <SceneManager.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace C3dAssimp;

//*************************************************************************************************

namespace detail
{
	struct stBONE_NODE
	{
		Point3r ptTranslate;
		Point3r ptScale;
		Quaternion qRotate;
		bool bTranslate;
		bool bRotate;
		bool bScale;

		stBONE_NODE()
			:	bTranslate( false	)
			,	bRotate( false	)
			,	bScale( false	)
		{
		}
	};

	typedef std::map< real, stBONE_NODE > NodeTimeMap;

	const aiNodeAnim * FindNodeAnim( const aiAnimation * p_pAnimation, const String p_strNodeName )
	{
		const aiNodeAnim * l_return = NULL;

		for ( uint32_t i = 0; i < p_pAnimation->mNumChannels && !l_return; ++i )
		{
			const aiNodeAnim * l_pNodeAnim = p_pAnimation->mChannels[i];

			if ( string::string_cast< xchar >( l_pNodeAnim->mNodeName.data ) == p_strNodeName )
			{
				l_return = l_pNodeAnim;
			}
		}

		return l_return;
	}

	real FindFactor( NodeTimeMap const & p_mapNodes, NodeTimeMap::const_iterator p_itCur, NodeTimeMap::const_iterator & p_itPrv, NodeTimeMap::const_iterator & p_itNxt )
	{
		p_itPrv = p_itCur;
		p_itNxt = p_itCur;

		if ( p_itCur != p_mapNodes.begin() )
		{
			p_itPrv--;
		}

		p_itNxt++;

		if ( p_itNxt == p_mapNodes.end() )
		{
			p_itNxt = p_itCur;
		}

		real l_rDeltaTime = p_itNxt->first - p_itPrv->first;
		real l_rReturn = ( p_itCur->first - p_itPrv->first ) / l_rDeltaTime;
		ENSURE( l_rReturn >= 0.0f && l_rReturn <= 1.0f );
		return l_rReturn;
	}

	Point3r CalcTranslate( NodeTimeMap const & p_mapNodes, NodeTimeMap::const_iterator p_itCur )
	{
		NodeTimeMap::const_iterator l_itPrv;
		NodeTimeMap::const_iterator l_itNxt;
		real l_rFactor = FindFactor( p_mapNodes, p_itCur, l_itPrv, l_itNxt );
		const Point3r & l_ptStart = l_itPrv->second.ptTranslate;
		const Point3r & l_ptEnd = l_itNxt->second.ptTranslate;
		return l_ptStart + ( l_rFactor * ( l_ptEnd - l_ptStart ) );
	}

	Point3r CalcScale( NodeTimeMap const & p_mapNodes, NodeTimeMap::const_iterator p_itCur )
	{
		NodeTimeMap::const_iterator l_itPrv;
		NodeTimeMap::const_iterator l_itNxt;
		real l_rFactor = FindFactor( p_mapNodes, p_itCur, l_itPrv, l_itNxt );
		const Point3r & l_ptStart = l_itPrv->second.ptScale;
		const Point3r & l_ptEnd = l_itNxt->second.ptScale;
		return l_ptStart + ( l_rFactor * ( l_ptEnd - l_ptStart ) );
	}

	Quaternion CalcRotate( NodeTimeMap const & p_mapNodes, NodeTimeMap::const_iterator p_itCur )
	{
		NodeTimeMap::const_iterator l_itPrv;
		NodeTimeMap::const_iterator l_itNxt;
		real l_rFactor = FindFactor( p_mapNodes, p_itCur, l_itPrv, l_itNxt );
		const Quaternion & l_qStart = l_itPrv->second.qRotate;
		const Quaternion & l_qEnd = l_itNxt->second.qRotate;
		return l_qStart.Slerp( l_qEnd, l_rFactor );
	}
}

//*************************************************************************************************

C3D_Assimp_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Assimp_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_IMPORTER;
}

C3D_Assimp_API String GetName()
{
	return cuT( "ASSIMP Importer" );
}

C3D_Assimp_API ImporterPlugin::ExtensionArray GetExtensions()
{
	ImporterPlugin::ExtensionArray l_arrayReturn;
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "DAE" ), cuT( "Collada" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "BLEND" ), cuT( "Blender" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "BVH" ), cuT( "3 Biovision BVH" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "3DS" ), cuT( "3D Studio Max 3DS" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "ASE" ), cuT( "3D Studio Max ASE" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "OBJ" ), cuT( "Wavefront Object" ) ) );
	//l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) ) ); // Crashes on big meshes.
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "MD2" ), cuT( "Quake II" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "MD3" ), cuT( "Quake III" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "LWO" ), cuT( "LightWave Model" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "IFC" ), cuT( "IFC-STEP, Industry Foundation Classes" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "NFF" ), cuT( "Sense8 WorldToolkit" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "SMD" ), cuT( "Valve Model" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "VTA" ), cuT( "Valve Model" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "MDL" ), cuT( "3 Quake I" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "PK3" ), cuT( "Quake 3 BSP" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "MDC" ), cuT( "1 RtCW" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "MD5MESH" ), cuT( "Doom 3" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "XML" ), cuT( "Ogre XML Mesh" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "X" ), cuT( "DirectX X" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "Q3O" ), cuT( "Quick3D" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "Q3S" ), cuT( "Quick3D" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "RAW" ), cuT( "Raw Triangles" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "AC" ), cuT( "AC3D" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "STL" ), cuT( "Stereolithography" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "DXF" ), cuT( "Autodesk DXF" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "IRRMESH" ), cuT( "Irrlicht Mesh" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "IRR" ), cuT( "Irrlicht Scene" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "OFF" ), cuT( "Object File Format" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "TER" ), cuT( "Terragen Terrain" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "MDL" ), cuT( "3D GameStudio Model" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "HMP" ), cuT( "3D GameStudio Terrain" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "MS3D" ), cuT( "3 Milkshape 3D" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "LWS" ), cuT( "LightWave Scene" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "LXO" ), cuT( "Modo Model" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "CSM" ), cuT( "CharacterStudio Motion" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "COB" ), cuT( "TrueSpace" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "SCN" ), cuT( "TrueSpace" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "XGL" ), cuT( "2 XGL" ) ) );
	l_arrayReturn.push_back( ImporterPlugin::Extension( cuT( "ZGL" ), cuT( "2 XGL" ) ) );
	return l_arrayReturn;
}

C3D_Assimp_API void Create( Engine * p_engine, ImporterPlugin * p_pPlugin )
{
	ImporterSPtr l_pImporter = std::make_shared< AssimpImporter >( *p_engine );
	p_pPlugin->AttachImporter( l_pImporter );
}

C3D_Assimp_API void Destroy( ImporterPlugin * p_pPlugin )
{
	p_pPlugin->DetachImporter();
}

C3D_Assimp_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Assimp_API void OnUnload( Castor3D::Engine * p_engine )
{
}

//*************************************************************************************************

AssimpImporter::AssimpImporter( Engine & p_engine )
	: Importer( p_engine )
	, m_anonymous( 0 )
{
}

AssimpImporter::~AssimpImporter()
{
}

SceneSPtr AssimpImporter::DoImportScene()
{
	DoImportMesh();
	SceneSPtr l_pScene;

	if ( m_pMesh )
	{
		m_pMesh->GenerateBuffers();
		l_pScene = GetOwner()->GetSceneManager().Create( cuT( "Scene_ASSIMP" ), *GetOwner(), cuT( "Scene_ASSIMP" ) );
		SceneNodeSPtr l_pNode = l_pScene->CreateSceneNode( m_pMesh->GetName(), l_pScene->GetObjectRootNode() );
		GeometrySPtr l_pGeometry = l_pScene->CreateGeometry( m_pMesh->GetName() );
		l_pGeometry->AttachTo( l_pNode );
		l_pGeometry->SetMesh( m_pMesh );
		m_pMesh.reset();
	}

	return l_pScene;
}

MeshSPtr AssimpImporter::DoImportMesh()
{
	String l_name = m_fileName.GetFileName();
	String l_meshName = l_name.substr( 0, l_name.find_last_of( '.' ) );
	m_pMesh = GetOwner()->GetMeshManager().Create( l_meshName, eMESH_TYPE_CUSTOM, UIntArray(), RealArray() );

	if ( !m_pMesh->GetSubmeshCount() )
	{
		SubmeshSPtr l_submesh;
		Assimp::Importer importer;
		uint32_t l_uiFlags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_FixInfacingNormals;
		String l_strNormals;
		bool l_bTangentSpace = false;
		xchar szBuffer[1024] = { 0 };

		if ( m_parameters.Get( cuT( "normals" ), szBuffer ) )
		{
			l_strNormals.assign( szBuffer );

			if ( l_strNormals == cuT( "smooth" ) )
			{
				l_uiFlags |= aiProcess_GenSmoothNormals;
			}
		}

		if ( m_parameters.Get( cuT( "tangent_space" ), l_bTangentSpace ) && l_bTangentSpace )
		{
			l_uiFlags |= aiProcess_CalcTangentSpace;
		}

		// And have it read the given file with some postprocessing
		aiScene const * l_pScene = importer.ReadFile( string::string_cast< char >( m_fileName ), l_uiFlags );

		if ( l_pScene )
		{
			SkeletonSPtr l_pSkeleton;

			if ( l_pScene->HasAnimations() )
			{
				l_pSkeleton = std::make_shared< Skeleton >( m_pMesh );
				m_pMesh->SetSkeleton( l_pSkeleton );
				l_pSkeleton->SetGlobalInverseTransform( Matrix4x4r( &l_pScene->mRootNode->mTransformation.Transpose().Inverse().a1 ) );
			}

			if ( l_pScene->HasMeshes() )
			{
				bool l_bCreate = true;

				for ( uint32_t i = 0; i < l_pScene->mNumMeshes; ++i )
				{
					if ( l_bCreate )
					{
						l_submesh = m_pMesh->CreateSubmesh();
					}

					l_bCreate = DoProcessMesh( l_pSkeleton, l_pScene->mMeshes[i], l_pScene, l_submesh );
				}

				if ( l_pScene->HasAnimations() && l_pSkeleton )
				{
					for ( uint32_t i = 0; i < l_pScene->mNumAnimations; ++i )
					{
						DoProcessAnimation( l_pSkeleton, l_pScene->mRootNode, l_pScene->mAnimations[i] );
					}
				}
			}
			else
			{
				GetOwner()->GetMeshManager().Remove( l_meshName );
				m_pMesh.reset();
			}

			importer.FreeScene();
		}
		else
		{
			// The import failed, report it
			Logger::LogError( std::stringstream() << "Scene import failed : " << importer.GetErrorString() );
			GetOwner()->GetMeshManager().Remove( l_meshName );
			m_pMesh.reset();
		}
	}

	MeshSPtr l_return( m_pMesh );
	m_pMesh.reset();
	return l_return;
}


bool AssimpImporter::DoProcessMesh( SkeletonSPtr p_pSkeleton, aiMesh const * p_pAiMesh, aiScene const * p_pAiScene, SubmeshSPtr p_pSubmesh )
{
	bool l_return = false;
	MaterialSPtr l_pMaterial;

	if ( p_pAiMesh->mMaterialIndex < p_pAiScene->mNumMaterials )
	{
		l_pMaterial = DoProcessMaterial( p_pAiScene->mMaterials[p_pAiMesh->mMaterialIndex] );
	}

	if ( p_pAiMesh->HasFaces() && p_pAiMesh->HasPositions() && l_pMaterial )
	{
		p_pSubmesh->SetDefaultMaterial( l_pMaterial );
		p_pSubmesh->Ref( l_pMaterial );
		stVERTEX_GROUP l_stVertices = { 0 };
		l_stVertices.m_uiCount = p_pAiMesh->mNumVertices;
#if CASTOR_USE_DOUBLE
		std::vector< real > vtx;
		std::vector< real > nml;
		std::vector< real > tan;
		std::vector< real > bin;
		std::vector< real > tex;
		vtx.reserve( p_pAiMesh->mNumVertices * 3 );

		for ( unsigned int i = 0; i < p_pAiMesh->mNumVertices; ++i )
		{
			vtx.push_back( p_pAiMesh->mVertices[i].x );
			vtx.push_back( p_pAiMesh->mVertices[i].y );
			vtx.push_back( p_pAiMesh->mVertices[i].z );
		}

		l_stVertices.m_pVtx = vtx.data();

		if ( p_pAiMesh->mNormals )
		{
			nml.reserve( p_pAiMesh->mNumVertices * 3 );

			for ( unsigned int i = 0; i < p_pAiMesh->mNumVertices; ++i )
			{
				nml.push_back( p_pAiMesh->mNormals[i].x );
				nml.push_back( p_pAiMesh->mNormals[i].y );
				nml.push_back( p_pAiMesh->mNormals[i].z );
			}

			l_stVertices.m_pNml = nml.data();
		}

		if ( p_pAiMesh->mTangents )
		{
			tan.reserve( p_pAiMesh->mNumVertices * 3 );

			for ( unsigned int i = 0; i < p_pAiMesh->mNumVertices; ++i )
			{
				tan.push_back( p_pAiMesh->mTangents[i].x );
				tan.push_back( p_pAiMesh->mTangents[i].y );
				tan.push_back( p_pAiMesh->mTangents[i].z );
			}

			l_stVertices.m_pTan = tan.data();
		}

		if ( p_pAiMesh->mBitangents )
		{
			bin.reserve( p_pAiMesh->mNumVertices * 3 );

			for ( unsigned int i = 0; i < p_pAiMesh->mNumVertices; ++i )
			{
				bin.push_back( p_pAiMesh->mBitangents[i].x );
				bin.push_back( p_pAiMesh->mBitangents[i].y );
				bin.push_back( p_pAiMesh->mBitangents[i].z );
			}

			l_stVertices.m_pBin = bin.data();
		}

		if ( p_pAiMesh->HasTextureCoords( 0 ) )
		{
			tex.reserve( p_pAiMesh->mNumVertices * 3 );

			for ( unsigned int i = 0; i < p_pAiMesh->mNumVertices; ++i )
			{
				tex.push_back( p_pAiMesh->mTextureCoords[0][i].x );
				tex.push_back( p_pAiMesh->mTextureCoords[0][i].y );
				tex.push_back( p_pAiMesh->mTextureCoords[0][i].z );
			}

			l_stVertices.m_pTex = tex.data();
		}

#else
		l_stVertices.m_pVtx = ( real * )p_pAiMesh->mVertices;
		l_stVertices.m_pNml = ( real * )p_pAiMesh->mNormals;
		l_stVertices.m_pTan = ( real * )p_pAiMesh->mTangents;
		l_stVertices.m_pBin = ( real * )p_pAiMesh->mBitangents;

		if ( p_pAiMesh->HasTextureCoords( 0 ) )
		{
			l_stVertices.m_pTex = ( real * )p_pAiMesh->mTextureCoords[0];
		}

#endif

		std::vector< stVERTEX_BONE_DATA > l_arrayBones( p_pAiMesh->mNumVertices );

		if ( p_pAiMesh->HasBones() && p_pSkeleton )
		{
			DoProcessBones( p_pSkeleton, p_pAiMesh->mBones, p_pAiMesh->mNumBones, l_arrayBones );
			l_stVertices.m_pBones = &l_arrayBones[0];
		}

		p_pSubmesh->AddPoints( l_stVertices );

		for ( uint32_t l_uiIndex = 0; l_uiIndex < p_pAiMesh->mNumFaces; l_uiIndex++ )
		{
			aiFace const & l_face = p_pAiMesh->mFaces[l_uiIndex];

			if ( l_face.mNumIndices == 3 )
			{
				p_pSubmesh->AddFace( l_face.mIndices[0], l_face.mIndices[1], l_face.mIndices[2] );
			}
		}

		if ( !l_stVertices.m_pNml )
		{
			p_pSubmesh->ComputeNormals( true );
		}
		else if ( !l_stVertices.m_pTan )
		{
			p_pSubmesh->ComputeTangentsFromNormals();
		}

		l_return = true;
	}

	return l_return;
}

MaterialSPtr AssimpImporter::DoProcessMaterial( aiMaterial const * p_pAiMaterial )
{
	MaterialSPtr l_return;
	PassSPtr l_pPass;
	MaterialManager & l_mtlManager = GetOwner()->GetMaterialManager();
	aiString l_mtlname;
	p_pAiMaterial->Get( AI_MATKEY_NAME, l_mtlname );
	String l_name = string::string_cast< xchar >( l_mtlname.C_Str() );
	aiColor3D l_clrAmbient( 1, 1, 1 );
	aiColor3D l_clrDiffuse( 1, 1, 1 );
	aiColor3D l_clrSpecular( 1, 1, 1 );
	aiColor3D l_clrEmissive( 1, 1, 1 );
	aiString l_ambTexName;
	aiString l_difTexName;
	aiString l_hgtTexName;
	aiString l_nmlTexName;
	aiString l_opaTexName;
	aiString l_shnTexName;
	aiString l_spcTexName;

	if ( l_name.empty() )
	{
		l_name = m_fileName.GetFileName() + string::to_string( m_anonymous++ );
	}

	l_return = l_mtlManager.Find( l_name );

	if ( !l_return )
	{
		float l_fOpacity = 1;
		float l_fShininess = 0.5f;
		float l_fShininessStrength = 1.0f;
		int l_iTwoSided = 0;
		l_return = l_mtlManager.Create( l_name, *GetOwner(), l_name );
		l_return->CreatePass();
		l_pPass = l_return->GetPass( 0 );
		p_pAiMaterial->Get( AI_MATKEY_COLOR_AMBIENT, l_clrAmbient );
		p_pAiMaterial->Get( AI_MATKEY_COLOR_DIFFUSE, l_clrDiffuse );
		p_pAiMaterial->Get( AI_MATKEY_COLOR_SPECULAR, l_clrSpecular );
		p_pAiMaterial->Get( AI_MATKEY_COLOR_EMISSIVE, l_clrEmissive );
		p_pAiMaterial->Get( AI_MATKEY_OPACITY, l_fOpacity );
		p_pAiMaterial->Get( AI_MATKEY_SHININESS, l_fShininess );
		p_pAiMaterial->Get( AI_MATKEY_SHININESS_STRENGTH, l_fShininessStrength );
		p_pAiMaterial->Get( AI_MATKEY_TWOSIDED, l_iTwoSided );
		p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_AMBIENT, 0 ), l_ambTexName );
		p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), l_difTexName );
		p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), l_nmlTexName );
		p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_HEIGHT, 0 ), l_hgtTexName );
		p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), l_opaTexName );
		p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), l_shnTexName );
		p_pAiMaterial->Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), l_spcTexName );

		if ( l_clrAmbient.IsBlack() && l_clrDiffuse.IsBlack() && l_clrSpecular.IsBlack() && l_clrEmissive.IsBlack() )
		{
			l_clrDiffuse.r = 1.0;
			l_clrDiffuse.g = 1.0;
			l_clrDiffuse.b = 1.0;
		}

		l_pPass->SetAmbient( Colour::from_components( l_clrAmbient.r, l_clrAmbient.g, l_clrAmbient.b, 1 ) );
		l_pPass->SetDiffuse( Colour::from_components( l_clrDiffuse.r, l_clrDiffuse.g, l_clrDiffuse.b, 1 ) );
		l_pPass->SetSpecular( Colour::from_components( l_clrSpecular.r * l_fShininessStrength, l_clrSpecular.g * l_fShininessStrength, l_clrSpecular.b * l_fShininessStrength, 1 ) );
		l_pPass->SetEmissive( Colour::from_components( l_clrEmissive.r, l_clrEmissive.g, l_clrEmissive.b, 1 ) );

		if ( l_fShininess > 0 )
		{
			l_pPass->SetShininess( l_fShininess );
		}

		l_pPass->SetAlpha( l_fOpacity );
		l_pPass->SetTwoSided( l_iTwoSided != 0 );

		if ( l_difTexName.length > 0 && std::string( l_difTexName.C_Str() ).find( "_Cine_" ) != String::npos && std::string( l_difTexName.C_Str() ).find( "/MI_CH_" ) != String::npos )
		{
			String l_strGlob = string::string_cast< xchar >( l_difTexName.C_Str() ) + cuT( ".tga" );
			string::replace( l_strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
			String l_strDiff = l_strGlob;
			String l_strNorm = l_strGlob;
			String l_strSpec = l_strGlob;
			String l_strOpac = l_strGlob;
			DoAddTexture( string::replace( l_strDiff, cuT( "_Cine_" ), cuT( "_D_" ) ), l_pPass, eTEXTURE_CHANNEL_DIFFUSE );
			DoAddTexture( string::replace( l_strNorm, cuT( "_Cine_" ), cuT( "_N_" ) ), l_pPass, eTEXTURE_CHANNEL_NORMAL );
			DoAddTexture( string::replace( l_strSpec, cuT( "_Cine_" ), cuT( "_S_" ) ), l_pPass, eTEXTURE_CHANNEL_SPECULAR );
			DoAddTexture( string::replace( l_strOpac, cuT( "_Cine_" ), cuT( "_A_" ) ), l_pPass, eTEXTURE_CHANNEL_OPACITY );
		}
		else
		{
			if ( l_ambTexName.length > 0 )
			{
				DoAddTexture( string::string_cast< xchar >( l_ambTexName.C_Str() ), l_pPass, eTEXTURE_CHANNEL_AMBIENT	);
			}

			if ( l_difTexName.length > 0 )
			{
				DoAddTexture( string::string_cast< xchar >( l_difTexName.C_Str() ), l_pPass, eTEXTURE_CHANNEL_DIFFUSE	);
			}

			if ( l_opaTexName.length > 0 )
			{
				DoAddTexture( string::string_cast< xchar >( l_opaTexName.C_Str() ), l_pPass, eTEXTURE_CHANNEL_OPACITY	);
			}

			if ( l_shnTexName.length > 0 )
			{
				DoAddTexture( string::string_cast< xchar >( l_shnTexName.C_Str() ), l_pPass, eTEXTURE_CHANNEL_GLOSS	);
			}

			if ( l_spcTexName.length > 0 )
			{
				DoAddTexture( string::string_cast< xchar >( l_spcTexName.C_Str() ), l_pPass, eTEXTURE_CHANNEL_SPECULAR	);
			}

			if ( l_nmlTexName.length > 0 )
			{
				DoAddTexture( string::string_cast< xchar >( l_nmlTexName.C_Str() ), l_pPass, eTEXTURE_CHANNEL_NORMAL	);

				if ( l_hgtTexName.length > 0 )
				{
					DoAddTexture( string::string_cast< xchar >( l_hgtTexName.C_Str() ), l_pPass, eTEXTURE_CHANNEL_HEIGHT	);
				}
			}
			else if ( l_hgtTexName.length > 0 )
			{
				DoAddTexture( string::string_cast< xchar >( l_hgtTexName.C_Str() ), l_pPass, eTEXTURE_CHANNEL_NORMAL	);
			}
		}
	}

	return l_return;
}

void AssimpImporter::DoProcessBones( SkeletonSPtr p_pSkeleton, aiBone ** p_pBones, uint32_t p_uiCount, std::vector< stVERTEX_BONE_DATA > & p_arrayVertices )
{
	for ( uint32_t i = 0; i < p_uiCount; ++i )
	{
		aiBone * l_pAiBone = p_pBones[i];
		String l_name = string::string_cast< xchar >( l_pAiBone->mName.C_Str() );
		uint32_t l_uiIndex;

		if ( m_mapBoneByID.find( l_name ) == m_mapBoneByID.end() )
		{
			BoneSPtr l_pBone = std::make_shared< Bone >( *p_pSkeleton );
			l_pBone->SetName( l_name );
			l_pBone->SetOffsetMatrix( Matrix4x4r( &l_pAiBone->mOffsetMatrix.Transpose().a1 ) );
			l_uiIndex = uint32_t( m_arrayBones.size() );
			m_arrayBones.push_back( l_pBone );
			m_mapBoneByID[l_name] = l_uiIndex;
			p_pSkeleton->AddBone( l_pBone );
		}
		else
		{
			l_uiIndex = m_mapBoneByID[l_name];
		}

		for ( uint32_t j = 0; j < l_pAiBone->mNumWeights; ++j )
		{
			uint32_t l_uiVertexId = l_pAiBone->mWeights[j].mVertexId;
			real l_rWeight = real( l_pAiBone->mWeights[j].mWeight );
			p_arrayVertices[l_uiVertexId].AddBoneData( l_uiIndex, l_rWeight );
		}
	}
}

AnimationSPtr AssimpImporter::DoProcessAnimation( SkeletonSPtr p_pSkeleton, aiNode * p_node, aiAnimation * p_pAnimation )
{
	String l_name = string::string_cast< xchar >( p_pAnimation->mName.C_Str() );

	if ( l_name.empty() )
	{
		l_name = m_fileName.GetFileName();
	}

	AnimationSPtr l_pAnimation = p_pSkeleton->CreateAnimation( l_name );
	real l_rTicksPerSecond = real( p_pAnimation->mTicksPerSecond != 0 ? p_pAnimation->mTicksPerSecond : 25.0 );
	DoProcessAnimationNodes( l_pAnimation, l_rTicksPerSecond, p_pSkeleton, p_node, p_pAnimation, nullptr );
	return l_pAnimation;
}

void AssimpImporter::DoProcessAnimationNodes( AnimationSPtr p_pAnimation, real p_rTicksPerSecond, SkeletonSPtr p_pSkeleton, aiNode * p_node, aiAnimation * p_paiAnimation, MovingObjectBaseSPtr p_object )
{
	String l_name = string::string_cast< xchar >( p_node->mName.data );
	const aiNodeAnim * l_pNodeAnim = ::detail::FindNodeAnim( p_paiAnimation, l_name );
	Matrix4x4r l_mtxNode( &p_node->mTransformation/*.Transpose()*/.a1 );
	MovingObjectBaseSPtr l_pObject;

	if ( l_pNodeAnim )
	{
		BonePtrArrayConstIt l_itBone = std::find_if( p_pSkeleton->Begin(), p_pSkeleton->End(), [&]( BoneSPtr p_pBone )
		{
			return p_pBone->GetName() == l_name;
		} );

		if ( l_itBone != p_pSkeleton->End() )
		{
			l_pObject = p_pAnimation->AddMovingObject( *l_itBone );
		}
		else
		{
			l_pObject = p_pAnimation->AddMovingObject();
		}

		// We treat translations
		for ( uint32_t i = 0; i < l_pNodeAnim->mNumPositionKeys; ++i )
		{
			Point3r l_ptTranslate( l_pNodeAnim->mPositionKeys[i].mValue.x, l_pNodeAnim->mPositionKeys[i].mValue.y, l_pNodeAnim->mPositionKeys[i].mValue.z );
			l_pObject->AddTranslateKeyFrame( real( l_pNodeAnim->mPositionKeys[i].mTime ) / p_rTicksPerSecond )->SetValue( l_ptTranslate );
		}

		// Then we treat scalings
		for ( uint32_t i = 0; i < l_pNodeAnim->mNumScalingKeys; ++i )
		{
			Point3r l_ptScale( l_pNodeAnim->mScalingKeys[i].mValue.x, l_pNodeAnim->mScalingKeys[i].mValue.y, l_pNodeAnim->mScalingKeys[i].mValue.z );
			l_pObject->AddScaleKeyFrame( real( l_pNodeAnim->mScalingKeys[i].mTime ) / p_rTicksPerSecond )->SetValue( l_ptScale );
		}

		// And eventually the rotations
		for ( uint32_t i = 0; i < l_pNodeAnim->mNumRotationKeys; ++i )
		{
			Quaternion l_qRotate;
			l_qRotate.FromRotationMatrix( Matrix3x3r( &l_pNodeAnim->mRotationKeys[i].mValue.GetMatrix().Transpose().a1 ) );
			l_pObject->AddRotateKeyFrame( real( l_pNodeAnim->mRotationKeys[i].mTime ) / p_rTicksPerSecond )->SetValue( l_qRotate );
		}
	}

	if ( !l_pObject )
	{
		l_pObject = p_pAnimation->AddMovingObject();
	}

	if ( p_object )
	{
		p_object->AddChild( l_pObject );
	}

	if ( !l_pObject->HasKeyFrames() )
	{
		l_pObject->SetNodeTransform( l_mtxNode );
	}

	for ( uint32_t i = 0 ; i < p_node->mNumChildren ; i++ )
	{
		DoProcessAnimationNodes( p_pAnimation, p_rTicksPerSecond, p_pSkeleton, p_node->mChildren[i], p_paiAnimation, l_pObject );
	}
}

void AssimpImporter::DoAddTexture( String const & p_strPath, PassSPtr p_pPass, eTEXTURE_CHANNEL p_channel )
{
	if ( p_pPass )
	{
		TextureUnitSPtr l_pTexture;
		Path l_pathImage = m_filePath / p_strPath;

		if ( !File::FileExists( l_pathImage ) )
		{
			l_pathImage = m_filePath / cuT( "Texture" ) / p_strPath;
		}

		if ( File::FileExists( l_pathImage ) )
		{
			l_pTexture = p_pPass->AddTextureUnit();

			try
			{
				l_pTexture->SetAutoMipmaps( true );

				if ( !l_pTexture->LoadTexture( l_pathImage ) )
				{
					p_pPass->DestroyTextureUnit( l_pTexture->GetIndex() - 1 );
					l_pTexture.reset();
				}
				else
				{
					l_pTexture->SetChannel( p_channel );
				}
			}
			catch ( ... )
			{
				p_pPass->DestroyTextureUnit( l_pTexture->GetIndex() - 1 );
				l_pTexture.reset();
				Logger::LogWarning( cuT( "Error encountered while loading texture file " ) + p_strPath );
			}
		}
		else
		{
			Logger::LogWarning( cuT( "Couldn't load texture file " ) + p_strPath );
		}
	}
}

//*************************************************************************************************
