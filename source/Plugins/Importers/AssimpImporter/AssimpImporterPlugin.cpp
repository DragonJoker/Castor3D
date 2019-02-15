#include "AssimpImporter/AssimpImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>

#include <assimp/version.h>

#ifndef CU_PlatformWindows
#	define C3D_Assimp_API
#else
#	ifdef AssimpImporter_EXPORTS
#		define C3D_Assimp_API __declspec(dllexport)
#	else
#		define C3D_Assimp_API __declspec(dllimport)
#	endif
#endif

castor3d::ImporterPlugin::ExtensionArray getExtensions( castor3d::Engine * engine )
{
	static castor3d::ImporterPlugin::ExtensionArray extensions;

	if ( extensions.empty() )
	{
		extensions.emplace_back( cuT( "AC" ), cuT( "AC3D" ) );
		extensions.emplace_back( cuT( "ACC" ), cuT( "AC3D" ) );
		extensions.emplace_back( cuT( "AC3D" ), cuT( "AC3D" ) );
		extensions.emplace_back( cuT( "BLEND" ), cuT( "Blender" ) );
		extensions.emplace_back( cuT( "BVH" ), cuT( "Biovision BVH" ) );
		extensions.emplace_back( cuT( "COB" ), cuT( "TrueSpace" ) );
		extensions.emplace_back( cuT( "CSM" ), cuT( "CharacterStudio Motion" ) );
		extensions.emplace_back( cuT( "DAE" ), cuT( "Collada" ) );
		extensions.emplace_back( cuT( "DXF" ), cuT( "Autodesk DXF" ) );
		extensions.emplace_back( cuT( "ENFF" ), cuT( "Neutral File Format" ) );
		extensions.emplace_back( cuT( "HMP" ), cuT( "3D GameStudio Heightmap" ) );
		extensions.emplace_back( cuT( "IFC" ), cuT( "IFC-STEP, Industry Foundation Classes" ) );
		extensions.emplace_back( cuT( "IFCZIP" ), cuT( "IFC-STEP, Industry Foundation Classes" ) );
		extensions.emplace_back( cuT( "IRR" ), cuT( "Irrlicht Scene" ) );
		extensions.emplace_back( cuT( "IRRMESH" ), cuT( "Irrlicht Mesh" ) );
		extensions.emplace_back( cuT( "LWS" ), cuT( "LightWave Scene" ) );
		extensions.emplace_back( cuT( "LXO" ), cuT( "Modo Model" ) );
		extensions.emplace_back( cuT( "MD5MESH" ), cuT( "doom 3 / MD5 Mesh" ) );
		extensions.emplace_back( cuT( "MDC" ), cuT( "Return To Castle Wolfenstein Mesh" ) );
		extensions.emplace_back( cuT( "MDL" ), cuT( "Quake Mesh / 3D GameStudio Mesh" ) );
		extensions.emplace_back( cuT( "MOT" ), cuT( "LightWave Scene" ) );
		extensions.emplace_back( cuT( "MS3D" ), cuT( "Milkshape 3D" ) );
		extensions.emplace_back( cuT( "NFF" ), cuT( "Neutral File Format" ) );
		extensions.emplace_back( cuT( "OFF" ), cuT( "Object File Format" ) );
		extensions.emplace_back( cuT( "PK3" ), cuT( "Quake III BSP" ) );
		extensions.emplace_back( cuT( "Q3O" ), cuT( "Quick3D" ) );
		extensions.emplace_back( cuT( "Q3S" ), cuT( "Quick3D" ) );
		extensions.emplace_back( cuT( "RAW" ), cuT( "Raw Triangles" ) );
		extensions.emplace_back( cuT( "SCN" ), cuT( "TrueSpace" ) );
		extensions.emplace_back( cuT( "SMD" ), cuT( "Valve Model" ) );
		extensions.emplace_back( cuT( "STL" ), cuT( "Stereolithography" ) );
		extensions.emplace_back( cuT( "TER" ), cuT( "Terragen Heightmap" ) );
		extensions.emplace_back( cuT( "VTA" ), cuT( "Valve Model" ) );
		extensions.emplace_back( cuT( "X" ), cuT( "Direct3D XFile" ) );
		extensions.emplace_back( cuT( "XGL" ), cuT( "XGL" ) );
		extensions.emplace_back( cuT( "XML" ), cuT( "Irrlicht Scene" ) );
		extensions.emplace_back( cuT( "ZGL" ), cuT( "XGL" ) );

		if ( aiGetVersionMajor() >= 3 )
		{
			if ( aiGetVersionMajor() >= 2 )
			{
				extensions.emplace_back( cuT( "3D" ), cuT( "Unreal" ) );
				extensions.emplace_back( cuT( "ASSBIN" ), cuT( "Assimp binary dump" ) );
				extensions.emplace_back( cuT( "B3D" ), cuT( "BlitzBasic 3D" ) );
				extensions.emplace_back( cuT( "NDO" ), cuT( "Nendo Mesh" ) );
				extensions.emplace_back( cuT( "OGEX" ), cuT( "open Game Engine Exchange" ) );
				extensions.emplace_back( cuT( "UC" ), cuT( "Unreal" ) );
			}
		}

		std::set< castor::String > alreadyLoaded;

		for ( auto it : engine->getPluginCache().getPlugins( castor3d::PluginType::eImporter ) )
		{
			auto const importer = std::static_pointer_cast< castor3d::ImporterPlugin >( it.second );

			if ( importer->getName() != C3dAssimp::AssimpImporter::Name )
			{
				for ( auto extension : importer->getExtensions() )
				{
					alreadyLoaded.insert( extension.first );
				}
			}
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "3DS" ) ) )
		{
			extensions.emplace_back( cuT( "3DS" ), cuT( "3D Studio Max 3DS" ) );
			extensions.emplace_back( cuT( "PRJ" ), cuT( "3D Studio Max 3DS" ) );
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "ASE" ) ) )
		{
			extensions.emplace_back( cuT( "ASE" ), cuT( "3D Studio Max ASE" ) );
			extensions.emplace_back( cuT( "ASK" ), cuT( "3D Studio Max ASE" ) );
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "OBJ" ) ) )
		{
			extensions.emplace_back( cuT( "OBJ" ), cuT( "Wavefront Object" ) );
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "PLY" ) ) )
		{
			// Assimp's implementation crashes on big meshes.
			extensions.emplace_back( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) );
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "MD2" ) ) )
		{
			extensions.emplace_back( cuT( "MD2" ), cuT( "Quake II Mesh" ) );
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "MD3" ) ) )
		{
			extensions.emplace_back( cuT( "MD3" ), cuT( "Quake III Mesh" ) );
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "LWO" ) ) )
		{
			extensions.emplace_back( cuT( "LWO" ), cuT( "LightWave/Modo Object" ) );
			extensions.emplace_back( cuT( "LXO" ), cuT( "LightWave/Modo Object" ) );
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "FBX" ) ) )
		{
			extensions.emplace_back( cuT( "FBX" ), cuT( "Autodesk FBX" ) );
		}

		if ( alreadyLoaded.end() == alreadyLoaded.find( cuT( "MESH" ) ) )
		{
			extensions.emplace_back( cuT( "MESH" ), cuT( "Ogre 3D Mesh" ) );
			extensions.emplace_back( cuT( "MESH.XML" ), cuT( "LOgre 3D Mesh" ) );
		}
	}

	return extensions;
}

extern "C"
{
	C3D_Assimp_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Assimp_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eImporter;
	}

	C3D_Assimp_API void getName( char const ** p_name )
	{
		*p_name = C3dAssimp::AssimpImporter::Name.c_str();
	}

	C3D_Assimp_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::ImporterPlugin * >( p_plugin );
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			plugin->addExtension( extension );
			engine->getImporterFactory().registerType( castor::string::lowerCase( extension.first ), &C3dAssimp::AssimpImporter::create );
		}
	}

	C3D_Assimp_API void OnUnload( castor3d::Engine * engine )
	{
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			engine->getImporterFactory().unregisterType( castor::string::lowerCase( extension.first ) );
		}
	}
}
