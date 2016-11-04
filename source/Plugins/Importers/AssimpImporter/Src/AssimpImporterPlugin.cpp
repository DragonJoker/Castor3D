#include "AssimpImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <Engine.hpp>
#include <PluginCache.hpp>

#include <Plugin/ImporterPlugin.hpp>

#include <assimp/version.h>

#ifndef _WIN32
#	define C3D_Assimp_API
#else
#	ifdef AssimpImporter_EXPORTS
#		define C3D_Assimp_API __declspec(dllexport)
#	else
#		define C3D_Assimp_API __declspec(dllimport)
#	endif
#endif

C3D_Assimp_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version{ CASTOR_VERSION_MAJOR, CASTOR_VERSION_MINOR, CASTOR_VERSION_BUILD };
}

C3D_Assimp_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::eImporter;
}

C3D_Assimp_API Castor::String GetName()
{
	return cuT( "ASSIMP Importer" );
}

C3D_Assimp_API Castor3D::ImporterPlugin::ExtensionArray GetExtensions( Castor3D::Engine * p_engine )
{
	static Castor3D::ImporterPlugin::ExtensionArray l_extensions;

	if ( l_extensions.empty() )
	{
		l_extensions.emplace_back( cuT( "AC" ), cuT( "AC3D" ) );
		l_extensions.emplace_back( cuT( "ACC" ), cuT( "AC3D" ) );
		l_extensions.emplace_back( cuT( "AC3D" ), cuT( "AC3D" ) );
		l_extensions.emplace_back( cuT( "BLEND" ), cuT( "Blender" ) );
		l_extensions.emplace_back( cuT( "BVH" ), cuT( "Biovision BVH" ) );
		l_extensions.emplace_back( cuT( "COB" ), cuT( "TrueSpace" ) );
		l_extensions.emplace_back( cuT( "CSM" ), cuT( "CharacterStudio Motion" ) );
		l_extensions.emplace_back( cuT( "DAE" ), cuT( "Collada" ) );
		l_extensions.emplace_back( cuT( "DXF" ), cuT( "Autodesk DXF" ) );
		l_extensions.emplace_back( cuT( "ENFF" ), cuT( "Neutral File Format" ) );
		l_extensions.emplace_back( cuT( "HMP" ), cuT( "3D GameStudio Heightmap" ) );
		l_extensions.emplace_back( cuT( "IFC" ), cuT( "IFC-STEP, Industry Foundation Classes" ) );
		l_extensions.emplace_back( cuT( "IFCZIP" ), cuT( "IFC-STEP, Industry Foundation Classes" ) );
		l_extensions.emplace_back( cuT( "IRR" ), cuT( "Irrlicht Scene" ) );
		l_extensions.emplace_back( cuT( "IRRMESH" ), cuT( "Irrlicht Mesh" ) );
		l_extensions.emplace_back( cuT( "LWS" ), cuT( "LightWave Scene" ) );
		l_extensions.emplace_back( cuT( "LXO" ), cuT( "Modo Model" ) );
		l_extensions.emplace_back( cuT( "MD5MESH" ), cuT( "Doom 3 / MD5 Mesh" ) );
		l_extensions.emplace_back( cuT( "MDC" ), cuT( "Return To Castle Wolfenstein Mesh" ) );
		l_extensions.emplace_back( cuT( "MDL" ), cuT( "Quake Mesh / 3D GameStudio Mesh" ) );
		l_extensions.emplace_back( cuT( "MOT" ), cuT( "LightWave Scene" ) );
		l_extensions.emplace_back( cuT( "MS3D" ), cuT( "Milkshape 3D" ) );
		l_extensions.emplace_back( cuT( "NFF" ), cuT( "Neutral File Format" ) );
		l_extensions.emplace_back( cuT( "OFF" ), cuT( "Object File Format" ) );
		l_extensions.emplace_back( cuT( "PK3" ), cuT( "Quake III BSP" ) );
		l_extensions.emplace_back( cuT( "Q3O" ), cuT( "Quick3D" ) );
		l_extensions.emplace_back( cuT( "Q3S" ), cuT( "Quick3D" ) );
		l_extensions.emplace_back( cuT( "RAW" ), cuT( "Raw Triangles" ) );
		l_extensions.emplace_back( cuT( "SCN" ), cuT( "TrueSpace" ) );
		l_extensions.emplace_back( cuT( "SMD" ), cuT( "Valve Model" ) );
		l_extensions.emplace_back( cuT( "STL" ), cuT( "Stereolithography" ) );
		l_extensions.emplace_back( cuT( "TER" ), cuT( "Terragen Heightmap" ) );
		l_extensions.emplace_back( cuT( "VTA" ), cuT( "Valve Model" ) );
		l_extensions.emplace_back( cuT( "X" ), cuT( "Direct3D XFile" ) );
		l_extensions.emplace_back( cuT( "XGL" ), cuT( "XGL" ) );
		l_extensions.emplace_back( cuT( "XML" ), cuT( "Irrlicht Scene" ) );
		l_extensions.emplace_back( cuT( "ZGL" ), cuT( "XGL" ) );

		if ( aiGetVersionMajor() >= 3 )
		{
			if ( aiGetVersionMajor() >= 2 )
			{
				l_extensions.emplace_back( cuT( "3D" ), cuT( "Unreal" ) );
				l_extensions.emplace_back( cuT( "ASSBIN" ), cuT( "Assimp binary dump" ) );
				l_extensions.emplace_back( cuT( "B3D" ), cuT( "BlitzBasic 3D" ) );
				l_extensions.emplace_back( cuT( "NDO" ), cuT( "Nendo Mesh" ) );
				l_extensions.emplace_back( cuT( "OGEX" ), cuT( "Open Game Engine Exchange" ) );
				l_extensions.emplace_back( cuT( "UC" ), cuT( "Unreal" ) );
			}
		}

		std::set< Castor::String > l_alreadyLoaded;

		for ( auto l_it : p_engine->GetPluginCache().GetPlugins( Castor3D::PluginType::eImporter ) )
		{
			auto const l_importer = std::static_pointer_cast< Castor3D::ImporterPlugin >( l_it.second );

			if ( l_importer->GetName() != GetName() )
			{
				for ( auto l_extension : l_importer->GetExtensions() )
				{
					l_alreadyLoaded.insert( l_extension.first );
				}
			}
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "3DS" ) ) )
		{
			l_extensions.emplace_back( cuT( "3DS" ), cuT( "3D Studio Max 3DS" ) );
			l_extensions.emplace_back( cuT( "PRJ" ), cuT( "3D Studio Max 3DS" ) );
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "ASE" ) ) )
		{
			l_extensions.emplace_back( cuT( "ASE" ), cuT( "3D Studio Max ASE" ) );
			l_extensions.emplace_back( cuT( "ASK" ), cuT( "3D Studio Max ASE" ) );
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "OBJ" ) ) )
		{
			l_extensions.emplace_back( cuT( "OBJ" ), cuT( "Wavefront Object" ) );
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "PLY" ) ) )
		{
			// Assimp's implementation crashes on big meshes.
			l_extensions.emplace_back( cuT( "PLY" ), cuT( "Stanford Polygon Library" ) );
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "MD2" ) ) )
		{
			l_extensions.emplace_back( cuT( "MD2" ), cuT( "Quake II Mesh" ) );
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "MD3" ) ) )
		{
			l_extensions.emplace_back( cuT( "MD3" ), cuT( "Quake III Mesh" ) );
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "LWO" ) ) )
		{
			l_extensions.emplace_back( cuT( "LWO" ), cuT( "LightWave/Modo Object" ) );
			l_extensions.emplace_back( cuT( "LXO" ), cuT( "LightWave/Modo Object" ) );
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "FBX" ) ) )
		{
			l_extensions.emplace_back( cuT( "FBX" ), cuT( "Autodesk FBX" ) );
		}

		if ( l_alreadyLoaded.end() == l_alreadyLoaded.find( cuT( "MESH" ) ) )
		{
			l_extensions.emplace_back( cuT( "MESH" ), cuT( "Ogre 3D Mesh" ) );
			l_extensions.emplace_back( cuT( "MESH.XML" ), cuT( "LOgre 3D Mesh" ) );
		}
	}

	return l_extensions;
}

C3D_Assimp_API void OnLoad( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Register( Castor::string::lower_case( l_extension.first ), &C3dAssimp::AssimpImporter::Create );
	}
}

C3D_Assimp_API void OnUnload( Castor3D::Engine * p_engine )
{
	auto l_extensions = GetExtensions( p_engine );

	for ( auto const & l_extension : l_extensions )
	{
		p_engine->GetImporterFactory().Unregister( Castor::string::lower_case( l_extension.first ) );
	}
}

