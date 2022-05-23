/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ImporterFile_H___
#define ___C3D_ImporterFile_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Animation/AnimationModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

namespace castor3d
{
	bool parseImportParameters( Parameters const & parameters
		, castor::Point3f & scale
		, castor::Quaternion & orientation );

	class ImporterFile
		: public castor::OwnedBy< Engine >
	{
	public:
		struct GeometryData
		{
			castor::String name;
			castor::String node;
			castor::String mesh;
		};

	protected:
		C3D_API ImporterFile( Engine & engine
			, Scene * scene
			, castor::Path const & path
			, Parameters const & parameters );

	public:
		C3D_API virtual ~ImporterFile() = default;

		C3D_API virtual std::vector< castor::String > listMaterials() = 0;
		C3D_API virtual std::vector< std::pair< castor::String, castor::String > > listMeshes() = 0;
		C3D_API virtual std::vector< castor::String > listSkeletons() = 0;
		C3D_API virtual std::vector< castor::String > listSceneNodes() = 0;
		C3D_API virtual std::vector< GeometryData > listGeometries() = 0;
		C3D_API virtual std::vector< std::pair< castor::String, LightType > > listLights() = 0;
		C3D_API virtual std::vector< castor::String > listMeshAnimations( Mesh const & mesh ) = 0;
		C3D_API virtual std::vector< castor::String > listSkeletonAnimations( Skeleton const & skeleton ) = 0;
		C3D_API virtual std::vector< castor::String > listSceneNodeAnimations( SceneNode const & node ) = 0;

		C3D_API virtual MaterialImporterUPtr createMaterialImporter() = 0;
		C3D_API virtual AnimationImporterUPtr createAnimationImporter() = 0;
		C3D_API virtual SkeletonImporterUPtr createSkeletonImporter() = 0;
		C3D_API virtual MeshImporterUPtr createMeshImporter() = 0;
		C3D_API virtual SceneNodeImporterUPtr createSceneNodeImporter() = 0;
		C3D_API virtual LightImporterUPtr createLightImporter() = 0;

		castor::String const & getExtension()const
		{
			return m_extension;
		}

		castor::String getName()const
		{
			return m_fileName.getFileName();
		}

		castor::Path const & getFileName()const
		{
			return m_fileName;
		}

		castor::Path const & getFilePath()const
		{
			return m_filePath;
		}

		Parameters const & getParameters()const
		{
			return m_parameters;
		}

		castor::String getExternalName( castor::String const & name )const
		{
			if ( name.find( m_prefix ) == 0u )
			{
				return name.substr( m_prefix.size() );
			}

			return name;
		}

		castor::String getInternalName( castor::String const & name )const
		{
			return m_prefix + name;
		}

		void setScene( Scene & scene )
		{
			m_scene = &scene;
		}

		Scene * getScene()const
		{
			return m_scene;
		}

	private:
		Scene * m_scene{};
		//!\~english The file full path.
		//!\~french Le chemin complet du fichier.
		castor::Path m_fileName;
		//!\~english The file parent path.
		//!\~french Le chemin parent du fichier.
		castor::Path m_filePath;
		castor::String m_extension;
		Parameters m_parameters;
		castor::String m_prefix;
	};

	class ImporterFileFactory
		: public castor::Factory< ImporterFile
			, castor::String
			, ImporterFileUPtr
			, std::function< ImporterFileUPtr( Engine &, Scene *, castor::Path const &, Parameters const & ) > >
	{
		using MyFactory = castor::Factory< ImporterFile
			, castor::String
			, ImporterFileUPtr
			, std::function< ImporterFileUPtr( Engine &, Scene *, castor::Path const &, Parameters const & ) > >;

	public:
		C3D_API ImporterFileFactory();

		C3D_API ImporterFileUPtr create( castor::String const & key
			, Engine & engine
			, castor::Path const & file
			, Parameters const & parameters );
		C3D_API ImporterFileUPtr create( castor::String const & key
			, Scene & scene
			, castor::Path const & file
			, Parameters const & parameters );
	};
}

#endif
