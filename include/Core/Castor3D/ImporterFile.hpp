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

namespace c3d
{
	C3D_API bool parseImportParameters( Parameters const & parameters
		, Point3f & scale
		, Quaternion & orientation );

	class ImporterFile
		: public OwnedBy< Engine >
	{
	public:
		struct NodeData
		{
			NodeData( String pparent
				, String pname
				, bool pisCamera )
				: parent{ c3d::move( pparent ) }
				, name{ c3d::move( pname ) }
				, isCamera{ pisCamera }
			{
			}

			String parent{};
			String name{};
			bool isCamera{};
		};

		struct GeometryData
		{
			GeometryData( String pname
				, String pnode
				, String pmesh )
				: name{ c3d::move( pname ) }
				, node{ c3d::move( pnode ) }
				, mesh{ c3d::move( pmesh ) }
			{
			}

			String name;
			String node;
			String mesh;
		};

		struct MeshData
		{
			MeshData( String pname
				, String pskeleton )
				: name{ c3d::move( pname ) }
				, skeleton{ c3d::move( pskeleton ) }
			{
			}

			String name;
			String skeleton;
		};

		struct LightData
		{
			LightData( String pname
				, LightType ptype )
				: name{ c3d::move( pname ) }
				, type{ c3d::move( ptype ) }
			{
			}

			String name;
			LightType type;
		};

		struct LightGroupData
		{
			LightGroupData( String pname
				, LightType ptype )
				: name{ c3d::move( pname ) }
				, type{ c3d::move( ptype ) }
			{
			}

			String name;
			LightType type;
		};

		struct CameraData
		{
			CameraData( String pname
				, ViewportType ptype )
				: name{ c3d::move( pname ) }
				, type{ ptype }
			{
			}

			String name;
			ViewportType type;
		};

	protected:
		C3D_API ImporterFile( Engine & engine
			, Scene * scene
			, Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );

	public:
		C3D_API virtual ~ImporterFile() = default;

		C3D_API virtual Vector< String > listMaterials() = 0;
		C3D_API virtual Vector< MeshData > listMeshes() = 0;
		C3D_API virtual Vector< String > listSkeletons() = 0;
		C3D_API virtual Vector< NodeData > listSceneNodes() = 0;
		C3D_API virtual Vector< GeometryData > listGeometries() = 0;
		C3D_API virtual Vector< LightData > listLights() = 0;
		C3D_API virtual Vector< LightGroupData > listLightGroups() = 0;
		C3D_API virtual Vector< CameraData > listCameras() = 0;
		C3D_API virtual Vector< String > listMeshAnimations( Mesh const & mesh ) = 0;
		C3D_API virtual Vector< String > listSkeletonAnimations( Skeleton const & skeleton ) = 0;
		C3D_API virtual Vector< String > listSceneNodeAnimations( SceneNode const & node ) = 0;
		C3D_API virtual Vector< uint32_t > listTextureAnimations( Material const & material
			, uint32_t pass ) = 0;
		C3D_API virtual uint32_t countAllMeshAnimations()const = 0;
		C3D_API virtual uint32_t countAllSkeletonAnimations()const = 0;
		C3D_API virtual uint32_t countAllSceneNodeAnimations()const = 0;
		C3D_API virtual uint32_t countAllTextureAnimations()const = 0;

		C3D_API virtual MaterialImporterUPtr createMaterialImporter() = 0;
		C3D_API virtual AnimationImporterUPtr createAnimationImporter() = 0;
		C3D_API virtual SkeletonImporterUPtr createSkeletonImporter() = 0;
		C3D_API virtual MeshImporterUPtr createMeshImporter() = 0;
		C3D_API virtual SceneNodeImporterUPtr createSceneNodeImporter() = 0;
		C3D_API virtual LightImporterUPtr createLightImporter() = 0;
		C3D_API virtual CameraImporterUPtr createCameraImporter() = 0;

		C3D_API static c3d::String reworkName( c3d::String const & name
			, c3d::StringView baseName
			, size_t index );

		String const & getExtension()const noexcept
		{
			return m_extension;
		}

		String getName()const noexcept
		{
			return String{ m_fileName.getFileName() };
		}

		Path const & getFileName()const noexcept
		{
			return m_fileName;
		}

		Path const & getFilePath()const noexcept
		{
			return m_filePath;
		}

		Parameters const & getParameters()const noexcept
		{
			return m_parameters;
		}

		String getExternalName( String const & name )const
		{
			if ( name.find( m_prefix ) == 0u )
			{
				return name.substr( m_prefix.size() );
			}

			return name;
		}

		String getInternalName( String const & name )const
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

		String const & getPrefix()const noexcept
		{
			return m_prefix;
		}

	private:
		Scene * m_scene{};
		//!\~english The file full path.
		//!\~french Le chemin complet du fichier.
		Path m_fileName;
		//!\~english The file parent path.
		//!\~french Le chemin parent du fichier.
		Path m_filePath;
		String m_extension;
		Parameters m_parameters;
		String m_prefix;
		ProgressBar * m_progress;
	};

	class ImporterFileFactory
	{
		using Creator = Function< ImporterFileUPtr( Engine &, Scene *, Path const &, Parameters const &, ProgressBar * ) >;

	public:
		C3D_API ImporterFileFactory();
		/**
		 *\~english
		 *\brief		Creates an importer from a file type.
		 *\param[in]	type		The file type.
		 *\param[in]	name		The preferred importer name.
		 *\param[in]	engine		The engine.
		 *\param[in]	file		The file containing the data to import.
		 *\param[in]	parameters	The import parameters.
		 *\param[in]	progress	An optional progress bar.
		 *\return		The created importer.
		 *\~french
		 *\brief		Crée un importeur à partir d'un type de fichier.
		 *\param[in]	type		Le type de fichier.
		 *\param[in]	name		Le nom de l'importeur préféré.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	file		Le fichier contenant les données à importer.
		 *\param[in]	parameters	Les paramètres d'import.
		 *\param[in]	progress	Une barre de progression optionnelle.
		 *\return		L'importeur créé.
		 */
		C3D_API ImporterFileUPtr create( String const & type
			, String const & name
			, Engine & engine
			, Path const & file
			, Parameters const & parameters
			, ProgressBar * progress = nullptr )const;
		/**
		 *\~english
		 *\brief		Creates an importer from a file type.
		 *\param[in]	type		The file type.
		 *\param[in]	name		The preferred importer name.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	file		The file containing the data to import.
		 *\param[in]	parameters	The import parameters.
		 *\param[in]	progress	An optional progress bar.
		 *\return		The created importer.
		 *\~french
		 *\brief		Crée un importeur à partir d'un type de fichier.
		 *\param[in]	type		Le type de fichier.
		 *\param[in]	name		Le nom de l'importeur préféré.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	file		Le fichier contenant les données à importer.
		 *\param[in]	parameters	Les paramètres d'import.
		 *\param[in]	progress	Une barre de progression optionnelle.
		 *\return		L'importeur créé.
		 */
		C3D_API ImporterFileUPtr create( String const & type
			, String const & name
			, Scene & scene
			, Path const & file
			, Parameters const & parameters
			, ProgressBar * progress = nullptr )const;
		/**
		 *\~english
		 *\brief		Creates an importer from a file type.
		 *\param[in]	type		The file type.
		 *\param[in]	engine		The engine.
		 *\param[in]	file		The file containing the data to import.
		 *\param[in]	parameters	The import parameters.
		 *\param[in]	progress	An optional progress bar.
		 *\return		The created importer.
		 *\~french
		 *\brief		Crée un importeur à partir d'un type de fichier.
		 *\param[in]	type		Le type de fichier.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	file		Le fichier contenant les données à importer.
		 *\param[in]	parameters	Les paramètres d'import.
		 *\param[in]	progress	Une barre de progression optionnelle.
		 *\return		L'importeur créé.
		 */
		C3D_API ImporterFileUPtr create( String const & type
			, Engine & engine
			, Path const & file
			, Parameters const & parameters
			, ProgressBar * progress = nullptr )const;
		/**
		 *\~english
		 *\brief		Creates an importer from a file type.
		 *\param[in]	type		The file type.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	file		The file containing the data to import.
		 *\param[in]	parameters	The import parameters.
		 *\param[in]	progress	An optional progress bar.
		 *\return		The created importer.
		 *\~french
		 *\brief		Crée un importeur à partir d'un type de fichier.
		 *\param[in]	type		Le type de fichier.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	file		Le fichier contenant les données à importer.
		 *\param[in]	parameters	Les paramètres d'import.
		 *\param[in]	progress	Une barre de progression optionnelle.
		 *\return		L'importeur créé.
		 */
		C3D_API ImporterFileUPtr create( String const & type
			, Scene & scene
			, Path const & file
			, Parameters const & parameters
			, ProgressBar * progress = nullptr )const;
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
		void registerType( String const & type
			, String const & name
			, Creator create )
		{
			auto it = m_registered.try_emplace( type ).first;
			it->second.try_emplace( name, create );
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
		void unregisterType( String const & type
			, String const & name )noexcept
		{
			if ( auto it = m_registered.find( type );
				it != m_registered.end() )
			{
				if ( auto tit = it->second.find( name );
					tit != it->second.end() )
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
		 *\param[in]	type	The file type.
		 *\return		\p true if registered.
		 *\~french
		 *\brief		Vérifie si un type de fichier est enregistré.
		 *\param[in]	type	Le type de fichier.
		 *\return		\p true si enregistré.
		 */
		bool isTypeRegistered( String const & type )const
		{
			return m_registered.end() != m_registered.find( type );
		}

	private:
		ImporterFileUPtr doCreate( String const & type
			, String const & name
			, Engine & engine
			, Scene * scene
			, Path const & file
			, Parameters const & parameters
			, ProgressBar * progress )const;

	private:
		HashStringMap< HashStringMap< Creator > > m_registered;
	};
}

#endif
