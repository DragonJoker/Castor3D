#include "FbxImporter/FbxImporterFile.hpp"

#include "FbxImporter/FbxMaterialImporter.hpp"

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

		c3d::StringView getName( fbx::EFbxType v )
		{
			switch ( v )
			{
			case fbx::EFbxType::eFbxUndefined: return cuT( "Undefined" );
			case fbx::EFbxType::eFbxChar: return cuT( "Char" );
			case fbx::EFbxType::eFbxUChar: return cuT( "UChar" );
			case fbx::EFbxType::eFbxShort: return cuT( "Short" );
			case fbx::EFbxType::eFbxUShort: return cuT( "UShort" );
			case fbx::EFbxType::eFbxUInt: return cuT( "UInt" );
			case fbx::EFbxType::eFbxLongLong: return cuT( "LongLong" );
			case fbx::EFbxType::eFbxULongLong: return cuT( "ULongLong" );
			case fbx::EFbxType::eFbxHalfFloat: return cuT( "HalfFloat" );
			case fbx::EFbxType::eFbxBool: return cuT( "Bool" );
			case fbx::EFbxType::eFbxInt: return cuT( "Int" );
			case fbx::EFbxType::eFbxFloat: return cuT( "Float" );
			case fbx::EFbxType::eFbxDouble: return cuT( "Double" );
			case fbx::EFbxType::eFbxDouble2: return cuT( "Double2" );
			case fbx::EFbxType::eFbxDouble3: return cuT( "Double3" );
			case fbx::EFbxType::eFbxDouble4: return cuT( "Double4" );
			case fbx::EFbxType::eFbxDouble4x4: return cuT( "Double4x4" );
			case fbx::EFbxType::eFbxEnum: return cuT( "Enum" );
			case fbx::EFbxType::eFbxEnumM: return cuT( "EnumM" );
			case fbx::EFbxType::eFbxString: return cuT( "String" );
			case fbx::EFbxType::eFbxTime: return cuT( "Time" );
			case fbx::EFbxType::eFbxReference: return cuT( "Reference" );
			case fbx::EFbxType::eFbxBlob: return cuT( "Blob" );
			case fbx::EFbxType::eFbxDistance: return cuT( "Distance" );
			case fbx::EFbxType::eFbxDateTime: return cuT( "DateTime" );
			case fbx::EFbxType::eFbxTypeCount: return cuT( "TypeCount" );
			default: return cuT( "Unknown" );
			}
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxHalfFloat const & v )
		{
			stream << v.value();
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxString const & v )
		{
			stream << v.Buffer();
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxColor const & v )
		{
			stream << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3];
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxDouble2 const & v )
		{
			stream << v[0] << ", " << v[1];
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxDouble3 const & v )
		{
			stream << v[0] << ", " << v[1] << ", " << v[2];
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxDouble4 const & v )
		{
			stream << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3];
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxDouble4x4 const & v )
		{
			stream << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3];
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxTime const & v )
		{
			stream << v.GetHourCount() << ":" << v.GetMinuteCount() << ":" << v.GetSecondCount() << " (" << v.GetMilliSeconds() << ")";
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxDistance const & v )
		{
			stream << v.value() << " " << v.unitName();
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxDateTime const & v )
		{
			stream << v.toString();
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxBlob const & v )
		{
			stream << "blob(" << v.Size() << ")";
			return stream;
		}

		c3d::OutputStream & operator<<( c3d::OutputStream & stream, fbx::FbxPropertyFlags::EFlags const & v )
		{
			if ( v == fbx::FbxPropertyFlags::eNone )
			{
				stream << cuT( "None" );
				return stream;
			}

			c3d::String sep;
			auto append = [&stream, &sep]( c3d::StringView s )
				{
					stream << sep << s;
					sep = cuT( "|" );
				};

			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eStatic ) )
				append( cuT( "Static" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eAnimatable ) )
				append( cuT( "Animatable" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eAnimated ) )
				append( cuT( "Animated" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eImported ) )
				append( cuT( "Imported" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUserDefined ) )
				append( cuT( "UserDefined" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eHidden ) )
				append( cuT( "Hidden" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eNotSavable ) )
				append( cuT( "NotSavable" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eLockedMember0 ) )
				append( cuT( "LockedMember0" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eLockedMember1 ) )
				append( cuT( "LockedMember1" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eLockedMember2 ) )
				append( cuT( "LockedMember2" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eLockedMember3 ) )
				append( cuT( "LockedMember3" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eMutedMember0 ) )
				append( cuT( "MutedMember0" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eMutedMember1 ) )
				append( cuT( "MutedMember1" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eMutedMember2 ) )
				append( cuT( "MutedMember2" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eMutedMember3 ) )
				append( cuT( "MutedMember3" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUIDisabled ) )
				append( cuT( "UIDisabled" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUIGroup ) )
				append( cuT( "UIGroup" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUIBoolGroup ) )
				append( cuT( "UIBoolGroup" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUIExpanded ) )
				append( cuT( "UIExpanded" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUINoCaption ) )
				append( cuT( "UINoCaption" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUIPanel ) )
				append( cuT( "UIPanel" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUILeftLabel ) )
				append( cuT( "UILeftLabel" ) );
			if ( c3d::checkFlag( v, fbx::FbxPropertyFlags::eUIHidden ) )
				append( cuT( "UIHidden" ) );

			return stream;
		}

		c3d::String getValue( fbx::FbxProperty const & fbxProperty
			, fbx::FbxDataType const & fbxDataType )
		{
			auto stream = c3d::makeStringStream();
			switch ( fbxDataType.GetType() )
			{
			case fbx::EFbxType::eFbxChar:
				stream << fbxProperty.Get< char >();
				break;
			case fbx::EFbxType::eFbxUChar:
				stream << fbxProperty.Get< unsigned char >();
				break;
			case fbx::EFbxType::eFbxShort:
				stream << fbxProperty.Get< short >();
				break;
			case fbx::EFbxType::eFbxUShort:
				stream << fbxProperty.Get< unsigned short >();
				break;
			case fbx::EFbxType::eFbxUInt:
				stream << fbxProperty.Get< unsigned int >();
				break;
			case fbx::EFbxType::eFbxLongLong:
				stream << fbxProperty.Get< long long >();
				break;
			case fbx::EFbxType::eFbxULongLong:
				stream << fbxProperty.Get< unsigned long long >();
				break;
			case fbx::EFbxType::eFbxHalfFloat:
				stream << fbxProperty.Get< fbx::FbxHalfFloat >();
				break;
			case fbx::EFbxType::eFbxBool:
				stream << fbxProperty.Get< bool >();
				break;
			case fbx::EFbxType::eFbxInt:
				stream << fbxProperty.Get< int >();
				break;
			case fbx::EFbxType::eFbxFloat:
				stream << fbxProperty.Get< float >();
				break;
			case fbx::EFbxType::eFbxDouble:
				stream << fbxProperty.Get< double >();
				break;
			case fbx::EFbxType::eFbxDouble2:
				stream << fbxProperty.Get< fbx::FbxDouble2 >();
				break;
			case fbx::EFbxType::eFbxDouble3:
				stream << fbxProperty.Get< fbx::FbxDouble3 >();
				break;
			case fbx::EFbxType::eFbxDouble4:
				stream << fbxProperty.Get< fbx::FbxDouble4 >();
				break;
			case fbx::EFbxType::eFbxDouble4x4:
				stream << fbxProperty.Get< fbx::FbxDouble4x4 >();
				break;
			case fbx::EFbxType::eFbxEnum:
				stream << fbxProperty.Get< fbx::FbxEnum >();
				break;
			case fbx::EFbxType::eFbxEnumM:
				stream << fbxProperty.Get< fbx::FbxEnum >();
				break;
			case fbx::EFbxType::eFbxString:
				stream << fbxProperty.Get< fbx::FbxString >();
				break;
			case fbx::EFbxType::eFbxTime:
				stream << fbxProperty.Get< fbx::FbxTime >();
				break;
			case fbx::EFbxType::eFbxReference:
				stream << fbxProperty.Get< fbx::FbxReference >();
				break;
			case fbx::EFbxType::eFbxBlob:
				stream << fbxProperty.Get< fbx::FbxBlob >();
				break;
			case fbx::EFbxType::eFbxDistance:
				stream << fbxProperty.Get< fbx::FbxDistance >();
				break;
			case fbx::EFbxType::eFbxDateTime:
				stream << fbxProperty.Get< fbx::FbxDateTime >();
				break;
			case fbx::EFbxType::eFbxTypeCount:
				break;
			default:
				stream << cuT( "Unknown" );
				break;
			}
			return stream.str();
		}

		c3d::String getEnums( fbx::FbxProperty const & fbxProperty )
		{
			auto stream = c3d::makeStringStream();
			c3d::String sep;
			for ( int i = 0; i < fbxProperty.GetEnumCount(); ++i )
			{
				stream << sep << fbxProperty.GetEnumValue( i );
				sep = cuT( ", " );
			}
			return stream.str();
		}

		c3d::String displayProperty( fbx::FbxProperty const & fbxProperty
			, c3d::String const & indent )
		{
			auto fbxDataType = fbxProperty.GetPropertyDataType();
			auto stream = c3d::makeStringStream();
			stream << indent << "[" << fbxProperty.GetName() << "]:" << std::endl
				<< indent << "  - Flags [" << fbxProperty.GetFlags() << "]" << std::endl
				<< indent << "  - DataType [" << fbxDataType.GetName() << "], " << getName( fbxDataType.GetType() ) << std::endl
				<< indent << "  - DataValue [" << getValue( fbxProperty, fbxDataType ) << "]" << std::endl
				<< indent << "  - EnumList [" << getEnums( fbxProperty ) << "]" << std::endl;
			if ( auto fbxChild = fbxProperty.GetChild();
				fbxChild.IsValid() )
				stream << indent << " - Child " << displayProperty( fbxProperty.GetChild(), indent + "  " );
			return stream.str();
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
			doPrelistMaterials( parameters );
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

	void FbxImporterFile::doPrelistMaterials( c3d::Parameters const & parameters )
	{
		FbxArray< fbx::FbxSurfaceMaterial * > materials;
		m_fbxScene->FillMaterialArray( materials );

		for ( int i = 0; i < materials.Size(); ++i )
		{
			fbx::FbxSurfaceMaterial * fbxMaterial = materials[i];
			auto name = getMaterialName( c3d::makeString( fbxMaterial->GetName() ) );
			m_sceneData.materials.try_emplace( name, fbxMaterial );

			if ( parameters.get< bool >( "list_properties" ) )
			{
				c3d::log::info << "Listing source properties for material [" << name << "]:" << std::endl;
				for ( int j = 0; j < fbxMaterial->GetSrcPropertyCount(); ++j )
					c3d::log::info << cuT( "Property " ) << file::displayProperty( fbxMaterial->GetSrcProperty( j ), cuT( "  " ) ) << std::endl;
			}
		}
	}

	//*********************************************************************************************
}
