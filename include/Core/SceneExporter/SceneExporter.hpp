/* See LICENSE file in root folder */
#ifndef ___CastorMeshConverter_HPP___
#define ___CastorMeshConverter_HPP___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Scene//SceneModule.hpp>

#if !defined( CU_PlatformWindows )
#	define CSE_API
#else
#	if defined( SceneExporter_EXPORTS )
#		define CSE_API __declspec( dllexport )
#	else
#		define CSE_API __declspec( dllimport )
#	endif
#endif

namespace castor3d::exporter
{
	struct ExportOptions
	{
		float scale{ 1.0f };
		bool dataSubfolders{ true };
		bool splitPerMaterial{ false };
		bool recenter{ false };
	};
	/**
	\~english
	\brief		Scene exporter base class.
	\~french
	\brief		Classe de base pour les exporteurs de scène.
	*/
	class SceneExporter
	{
	public:
		SceneExporter( ExportOptions options )
			: m_options{ std::move( options ) }
		{
		}
		/**
		 *\~english
		 *\brief		Export function.
		 *\param[in]	scene		The scene to export.
		 *\param[in]	fileName	The exported file name.
		 *\~french
		 *\brief		Fonction d'export
		 *\param[in]	scene		La scène à exporter.
		 *\param[in]	fileName	Le nom du fichier exportà.
		 */
		CSE_API virtual bool exportScene( castor3d::Scene const & scene
			, castor::Path const & fileName ) = 0;
		CSE_API virtual bool exportMesh( castor3d::Scene const & scene
			, castor3d::Mesh const & mesh
			, castor::Path const & outputFolder
			, castor::String const & outputName ) = 0;

	protected:
		ExportOptions const m_options;
	};
}

#endif
