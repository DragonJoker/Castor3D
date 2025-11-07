#include "FbxImporter/FbxImporterFile.hpp"

#include "FbxImporter/FbxMaterialImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>

namespace c3d_fbx
{
	//*********************************************************************************************

	namespace file
	{
		fbx::FbxManager * createFbxManager()
		{
			auto result = fbx::FbxManager::Create();
			if ( result )
			{
				c3d::log::debug << "Autodesk FBX SDK version " << fbx::FbxManager::GetVersion() << std::endl;

				//Create an IOSettings object. This object holds all import/export settings.
				auto ios = FbxIOSettings::Create( result, IOSROOT );
				result->SetIOSettings( ios );

				//Load plug-ins from the executable directory (optional)
				auto path = FbxGetApplicationDirectory();
				result->LoadPluginsDirectory( path.Buffer() );
			}
			else
			{
				c3d::log::error << cuT( "Error: Unable to create FBX manager.\n" );
			}

			return result;
		}

		fbx::FbxScene * loadScene( fbx::FbxManager * fbxManager, c3d::Path const & fileName )
		{
			if ( !fbxManager )
				return nullptr;

			auto strFileName = c3d::string::toMbString( fileName );

			// Get the file version number generate by the FBX SDK.
			int sdkMajor{};
			int sdkMinor{};
			int sdkRevision{};
			fbx::FbxManager::GetFileFormatVersion( sdkMajor, sdkMinor, sdkRevision );

			// Create an importer.
			auto fbxImporter = fbx::FbxImporter::Create( fbxManager, "" );

			// Initialize the importer by providing a filename.
			const bool importStatus = fbxImporter->Initialize( strFileName.c_str(), -1, fbxManager->GetIOSettings() );
			int fileMajor{};
			int fileMinor{};
			int fileRevision{};
			fbxImporter->GetFileVersion( fileMajor, fileMinor, fileRevision );
			fbx::FbxScene * fbxScene{};

			if ( !importStatus )
			{
				c3d::log::error << cuT( "Call to FbxImporter::Initialize() failed: " ) << c3d::makeString( fbxImporter->GetStatus().GetErrorString() ) << cuT( "\n" );

				if ( fbxImporter->GetStatus().GetCode() == fbx::FbxStatus::eInvalidFileVersion )
				{
					c3d::log::error << "FBX file format version for this FBX SDK is " << sdkMajor << "." << sdkMinor << "." << sdkRevision << "\n";
					c3d::log::error << "FBX file format version for file " << fileName << " is " << fileMajor << "." << fileMinor << "." << fileRevision << "\n";
				}
			}
			else
			{
				c3d::log::debug << "FBX file format version for this FBX SDK is " << sdkMajor << "." << sdkMinor << "." << sdkRevision << "\n";

				if ( fbxImporter->IsFBX() )
				{
					c3d::log::debug << "FBX file format version for file " << fileName << " is " << fileMajor << "." << fileMinor << "." << fileRevision << "\n";

					// Set the import states. By default, the import states are always set to true.
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_LINK, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_SHAPE, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_GOBO, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_AUDIO, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_CHARACTER, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_CONSTRAINT, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_MODEL, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_NORMAL, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_BINORMAL, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_TANGENT, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_VERTEXCOLOR, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_POLYGROUP, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_SMOOTHING, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_USERDATA, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_VISIBILITY, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_EDGECREASE, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_VERTEXCREASE, false );
					fbxManager->GetIOSettings()->SetBoolProp( IMP_FBX_HOLE, false );
				}

				//Create an FBX scene. This object holds most objects imported/exported from/to files.
				fbxScene = FbxScene::Create( fbxManager, c3d::toUtf8( fileName.getFileName() ).c_str() );

				if ( fbxScene )
				{
					if ( !fbxImporter->Import( fbxScene ) )
					{
						auto const & status = fbxImporter->GetStatus();
						c3d::log::error << cuT( "Import failed with status " ) << c3d::makeString( status.GetErrorString() ) << cuT( "\n" );
						fbx::FbxArray< fbx::FbxString * > errors;
						fbxImporter->GetStatus().GetErrorStringHistory( errors );
						for ( int i = 0; i < errors.Size(); ++i )
							if ( errors[i] )
								c3d::log::error << cuT( "    " ) << c3d::makeString( errors[i]->Buffer() ) << cuT( "\n" );

						fbxScene->Destroy();
						fbxScene = nullptr;
					}
				}
				else
				{
					c3d::log::error << cuT( "Error: Unable to create FBX scene.\n" );
				}
			}

			// Destroy the importer.
			fbxImporter->Destroy();

			return fbxScene;
		}
	}

	//*********************************************************************************************

	c3d::MbString const FbxImporterFile::Name = "FBX Importer";

	FbxImporterFile::FbxImporterFile( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
		: c3d_assimp::AssimpImporterFile{ engine, scene, path, parameters, progress }
		, m_fbxManager{ file::createFbxManager() }
		, m_fbxScene{ file::loadScene( m_fbxManager, path ) }
	{
		if ( isValid() )
			doPrelistMaterials();
	}

	FbxImporterFile::~FbxImporterFile()noexcept
	{
		if ( m_fbxScene )
			m_fbxScene->Destroy();
		if ( m_fbxManager )
			m_fbxManager->Destroy();
	}

	c3d::ImporterFileUPtr FbxImporterFile::create( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
	{
		return c3d::makeUniqueDerived< c3d::ImporterFile, FbxImporterFile >( engine, scene, path, parameters, progress );
	}

	c3d::StringArray FbxImporterFile::listMaterials()
	{
		c3d::StringArray result;
		if ( isValid() )
			for ( auto const & [name, _] : m_sceneData.materials )
				result.emplace_back( name );
		return result;
	}

	c3d::Vector< uint32_t > FbxImporterFile::listTextureAnimations( c3d::Material const & material
		, uint32_t pass )
	{
		c3d::Vector< uint32_t > result;
		return result;
	}

	c3d::MaterialImporterUPtr FbxImporterFile::createMaterialImporter()
	{
		return c3d::makeUniqueDerived< c3d::MaterialImporter, FbxMaterialImporter >( *getOwner() );
	}

	void FbxImporterFile::doPrelistMaterials()
	{
		FbxArray< fbx::FbxSurfaceMaterial * > materials;
		m_fbxScene->FillMaterialArray( materials );

		for ( int i = 0; i < materials.Size(); ++i )
		{
			fbx::FbxSurfaceMaterial * fbxMaterial = materials[i];
			auto name = getMaterialName( c3d::makeString( fbxMaterial->GetName() ) );
			m_sceneData.materials.try_emplace( name, fbxMaterial );
		}
	}

	//*********************************************************************************************
}
