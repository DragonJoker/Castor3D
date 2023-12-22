/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ImporterFile_H___
#define ___C3D_ImporterFile_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Animation/AnimationModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

#include <unordered_map>

namespace castor3d
{
	bool parseImportParameters( Parameters const & parameters
		, castor::Point3f & scale
		, castor::Quaternion & orientation );

	class ImporterFile
		: public castor::OwnedBy< Engine >
	{
	public:
		struct NodeData
		{
			NodeData( castor::String pparent
				, castor::String pname
				, bool pisCamera )
				: parent{ std::move( pparent ) }
				, name{ std::move( pname ) }
				, isCamera{ pisCamera }
			{
			}

			castor::String parent{};
			castor::String name{};
			bool isCamera{};
		};

		struct GeometryData
		{
			GeometryData( castor::String pname
				, castor::String pnode
				, castor::String pmesh )
				: name{ std::move( pname ) }
				, node{ std::move( pnode ) }
				, mesh{ std::move( pmesh ) }
			{
			}

			castor::String name;
			castor::String node;
			castor::String mesh;
		};

		struct MeshData
		{
			MeshData( castor::String pname
				, castor::String pskeleton )
				: name{ std::move( pname ) }
				, skeleton{ std::move( pskeleton ) }
			{
			}

			castor::String name;
			castor::String skeleton;
		};

		struct LightData
		{
			LightData( castor::String pname
				, LightType ptype )
				: name{ std::move( pname ) }
				, type{ std::move( ptype ) }
			{
			}

			castor::String name;
			LightType type;
		};

		struct CameraData
		{
			CameraData( castor::String pname
				, ViewportType ptype )
				: name{ std::move( pname ) }
				, type{ ptype }
			{
			}

			castor::String name;
			ViewportType type;
		};

	protected:
		C3D_API ImporterFile( Engine & engine
			, Scene * scene
			, castor::Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );

	public:
		C3D_API virtual ~ImporterFile() = default;

		C3D_API virtual std::vector< castor::String > listMaterials() = 0;
		C3D_API virtual std::vector< MeshData > listMeshes() = 0;
		C3D_API virtual std::vector< castor::String > listSkeletons() = 0;
		C3D_API virtual std::vector< NodeData > listSceneNodes() = 0;
		C3D_API virtual std::vector< GeometryData > listGeometries() = 0;
		C3D_API virtual std::vector< LightData > listLights() = 0;
		C3D_API virtual std::vector< CameraData > listCameras() = 0;
		C3D_API virtual std::vector< castor::String > listMeshAnimations( Mesh const & mesh ) = 0;
		C3D_API virtual std::vector< castor::String > listSkeletonAnimations( Skeleton const & skeleton ) = 0;
		C3D_API virtual std::vector< castor::String > listSceneNodeAnimations( SceneNode const & node ) = 0;
		C3D_API virtual std::vector< castor::String > listAllMeshAnimations() = 0;
		C3D_API virtual std::vector< castor::String > listAllSkeletonAnimations() = 0;
		C3D_API virtual std::vector< castor::String > listAllSceneNodeAnimations() = 0;

		C3D_API virtual MaterialImporterUPtr createMaterialImporter() = 0;
		C3D_API virtual AnimationImporterUPtr createAnimationImporter() = 0;
		C3D_API virtual SkeletonImporterUPtr createSkeletonImporter() = 0;
		C3D_API virtual MeshImporterUPtr createMeshImporter() = 0;
		C3D_API virtual SceneNodeImporterUPtr createSceneNodeImporter() = 0;
		C3D_API virtual LightImporterUPtr createLightImporter() = 0;
		C3D_API virtual CameraImporterUPtr createCameraImporter() = 0;

		castor::String const & getExtension()const noexcept
		{
			return m_extension;
		}

		castor::String getName()const noexcept
		{
			return m_fileName.getFileName();
		}

		castor::Path const & getFileName()const noexcept
		{
			return m_fileName;
		}

		castor::Path const & getFilePath()const noexcept
		{
			return m_filePath;
		}

		Parameters const & getParameters()const noexcept
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

		void setScene( Scene & scene )noexcept
		{
			m_scene = &scene;
		}

		Scene * getScene()const noexcept
		{
			return m_scene;
		}

		ProgressBar * getProgressBar()const noexcept
		{
			return m_progress;
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
		ProgressBar * m_progress;
	};

	class ImporterFileFactory
	{
		using Creator = std::function< ImporterFileUPtr( Engine &, Scene *, castor::Path const &, Parameters const &, ProgressBar * ) >;

	public:
		C3D_API ImporterFileFactory();
		/**
		 *\~english
		 *\brief		Creates an importer from a file type.
		 *\param[in]	type		The file type
		 *\param[in]	name		The preferred importer name
		 *\param[in]	parameters	The import parameters.
		 *\return		The created importer.
		 *\~french
		 *\brief		Crée un importeur à partir d'un type de fichier.
		 *\param[in]	type		Le type de fichier.
		 *\param[in]	name		Le nom de l'importeur préféré.
		 *\param[in]	parameters	Les paramètres d'import.
		 *\return		L'importeur créé
		 */
		C3D_API ImporterFileUPtr create( castor::String const & type
			, castor::String const & name
			, Engine & engine
			, castor::Path const & file
			, Parameters const & parameters
			, ProgressBar * progress = nullptr );
		C3D_API ImporterFileUPtr create( castor::String const & type
			, castor::String const & name
			, Scene & scene
			, castor::Path const & file
			, Parameters const & parameters
			, ProgressBar * progress = nullptr );
		/**
		 *\~english
		 *\brief		Creates an importer from a file type.
		 *\param[in]	key			The file type.
		 *\param[in]	parameters	The import parameters.
		 *\return		The created importer.
		 *\~french
		 *\brief		Crée un importeur à partir d'un type de fichier.
		 *\param[in]	type		Le type de fichier.
		 *\param[in]	parameters	Les paramètres d'import.
		 *\return		L'importeur créé
		 */
		C3D_API ImporterFileUPtr create( castor::String const & type
			, Engine & engine
			, castor::Path const & file
			, Parameters const & parameters
			, ProgressBar * progress = nullptr );
		C3D_API ImporterFileUPtr create( castor::String const & type
			, Scene & scene
			, castor::Path const & file
			, Parameters const & parameters
			, ProgressBar * progress = nullptr );
		/**
		 *\~english
		 *\brief		Registers an file type.
		 *\param[in]	type	The file type.
		 *\param[in]	name	The importer name.
		 *\param[in]	create	The object creation function.
		 *\~french
		 *\brief		Enregistre un type de fichier.
		 *\param[in]	type	Le type d'objet.
		 *\param[in]	name	Le nom de l'importeur.
		 *\param[in]	create	La fonction de création d'objet.
		 */
		void registerType( castor::String const & type
			, castor::String const & name
			, Creator create )
		{
			auto it = m_registered.emplace( type
				, std::unordered_map< castor::String, Creator >{} ).first;
			it->second.emplace( name, create );
		}
		/**
		 *\~english
		 *\brief		Unregisters an file type.
		 *\param[in]	type	The file type.
		 *\param[in]	name	The importer name.
		 *\~french
		 *\brief		Désenregistre un type de fichier.
		 *\param[in]	type	Le type d'objet.
		 *\param[in]	name	Le nom de l'importeur.
		 */
		void unregisterType( castor::String const & type
			, castor::String const & name )
		{
			auto it = m_registered.find( type );

			if ( it != m_registered.end() )
			{
				auto tit = it->second.find( name );

				if ( tit != it->second.end() )
				{
					it->second.erase( tit );
				}

				if ( it->second.empty() )
				{
					m_registered.erase( it );
				}
			}
		}
		/**
		 *\~english
		 *\brief		Checks if the given file type is registered.
		 *\param[in]	key	The file type.
		 *\return		\p true if registered.
		 *\~french
		 *\brief		Vérifie si un type de fichier est enregistré.
		 *\param[in]	key	Le type de fichier.
		 *\return		\p true si enregistré.
		 */
		bool isTypeRegistered( castor::String const & type )const
		{
			return m_registered.end() != m_registered.find( type );
		}

	private:
		ImporterFileUPtr doCreate( castor::String const & type
			, castor::String const & name
			, Engine & engine
			, Scene * scene
			, castor::Path const & file
			, Parameters const & parameters
			, ProgressBar * progress )const;

	private:
		std::unordered_map< castor::String, std::unordered_map< castor::String, Creator > > m_registered;
	};
}

#endif
