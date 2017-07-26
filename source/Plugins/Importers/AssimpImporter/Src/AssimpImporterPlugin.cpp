#include "AssimpImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Engine.hpp>
#include <Cache/PluginCache.hpp>

#include <Plugin/ImporterPlugin.hpp>

#include <assimp/version.h>

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_Assimp_API
#else
#	ifdef AssimpImporter_EXPORTS
#		define C3D_Assimp_API __declspec(dllexport)
#	else
#		define C3D_Assimp_API __declspec(dllimport)
#	endif
#endif

Castor3D::ImporterPlugin::ExtensionArray GetExtensions( Castor3D::Engine * engine )
{
	static Castor3D::ImporterPlugin::ExtensionArray extensions;

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
		extensions.emplace_back( cuT( "MD5MESH" ), cuT( "Doom 3 / MD5 Mesh" ) );
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
				extensions.emplace_back( cuT( "OGEX" ), cuT( "Open Game Engine Exchange" ) );
				extensions.emplace_back( cuT( "UC" ), cuT( "Unreal" ) );
			}
		}

		std::set< Castor::String > alreadyLoaded;

		for ( auto it : engine->GetPluginCache().GetPlugins( Castor3D::PluginType::eImporter ) )
		{
			auto const importer = std::static_pointer_cast< Castor3D::ImporterPlugin >( it.second );

			if ( importer->GetName() != C3dAssimp::AssimpImporter::Name )
			{
				for ( auto extension : importer->GetExtensions() )
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
	C3D_Assimp_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Assimp_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eImporter;
	}

	C3D_Assimp_API void GetName( char const ** p_name )
	{
		*p_name = C3dAssimp::AssimpImporter::Name.c_str();
	}

	C3D_Assimp_API void OnLoad( Castor3D::Engine * engine, Castor3D::Plugin * p_plugin )
	{
		auto plugin = static_cast< Castor3D::ImporterPlugin * >( p_plugin );
		auto extensions = GetExtensions( engine );

		for ( auto const & extension : extensions )
		{
			plugin->AddExtension( extension );
			engine->GetImporterFactory().Register( Castor::string::lower_case( extension.first ), &C3dAssimp::AssimpImporter::Create );
		}
	}

	C3D_Assimp_API void OnUnload( Castor3D::Engine * engine )
	{
		auto extensions = GetExtensions( engine );

		for ( auto const & extension : extensions )
		{
			engine->GetImporterFactory().Unregister( Castor::string::lower_case( extension.first ) );
		}
	}
}
