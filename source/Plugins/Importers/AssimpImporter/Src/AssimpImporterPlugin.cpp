#include "AssimpImporter.hpp"

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#endif

#include <PluginManager.hpp>

#include <Plugin/ImporterPlugin.hpp>

#include <assimp/version.h>

C3D_Assimp_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version{ CASTOR_VERSION_MAJOR, CASTOR_VERSION_MINOR, CASTOR_VERSION_BUILD };
}

C3D_Assimp_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_IMPORTER;
}

C3D_Assimp_API Castor::String GetName()
{
	return cuT( "ASSIMP Importer" );
}

C3D_Assimp_API Castor3D::ImporterPlugin::ExtensionArray GetExtensions( Castor3D::Engine * p_engine )
{
	Castor3D::ImporterPlugin::ExtensionArray l_extensions
	{
		{ cuT( "AC" ), cuT( "AC3D" ) },
		{ cuT( "ACC" ), cuT( "AC3D" ) },
		{ cuT( "AC3D" ), cuT( "AC3D" ) },
		{ cuT( "BLEND" ), cuT( "Blender" ) },
		{ cuT( "BVH" ), cuT( "Biovision BVH" ) },
		{ cuT( "COB" ), cuT( "TrueSpace" ) },
		{ cuT( "CSM" ), cuT( "CharacterStudio Motion" ) },
		{ cuT( "DAE" ), cuT( "Collada" ) },
		{ cuT( "DXF" ), cuT( "Autodesk DXF" ) },
		{ cuT( "ENFF" ), cuT( "Neutral File Format" ) },
		{ cuT( "HMP" ), cuT( "3D GameStudio Heightmap" ) },
		{ cuT( "IFC" ), cuT( "IFC-STEP, Industry Foundation Classes" ) },
		{ cuT( "IFCZIP" ), cuT( "IFC-STEP, Industry Foundation Classes" ) },
		{ cuT( "IRR" ), cuT( "Irrlicht Scene" ) },
		{ cuT( "IRRMESH" ), cuT( "Irrlicht Mesh" ) },
		{ cuT( "LWS" ), cuT( "LightWave Scene" ) },
		{ cuT( "LXO" ), cuT( "Modo Model" ) },
		{ cuT( "MD5MESH" ), cuT( "Doom 3 / MD5 Mesh" ) },
		{ cuT( "MDC" ), cuT( "Return To Castle Wolfenstein Mesh" ) },
		{ cuT( "MDL" ), cuT( "Quake Mesh / 3D GameStudio Mesh" ) },
		{ cuT( "MESH" ), cuT( "Ogre 3D Mesh" ) },
		{ cuT( "MESH.XML" ), cuT( "Ogre 3D Mesh" ) },
		{ cuT( "MOT" ), cuT( "LightWave Scene" ) },
		{ cuT( "MS3D" ), cuT( "Milkshape 3D" ) },
		{ cuT( "NFF" ), cuT( "Neutral File Format" ) },
		{ cuT( "OFF" ), cuT( "Object File Format" ) },
		{ cuT( "PK3" ), cuT( "Quake III BSP" ) },
		{ cuT( "Q3O" ), cuT( "Quick3D" ) },
		{ cuT( "Q3S" ), cuT( "Quick3D" ) },
		{ cuT( "RAW" ), cuT( "Raw Triangles" ) },
		{ cuT( "SCN" ), cuT( "TrueSpace" ) },
		{ cuT( "SMD" ), cuT( "Valve Model" ) },
		{ cuT( "STL" ), cuT( "Stereolithography" ) },
		{ cuT( "TER" ), cuT( "Terragen Heightmap" ) },
		{ cuT( "VTA" ), cuT( "Valve Model" ) },
		{ cuT( "X" ), cuT( "Direct3D XFile" ) },
		{ cuT( "XGL" ), cuT( "XGL" ) },
		{ cuT( "XML" ), cuT( "Irrlicht Scene" ) },
		{ cuT( "ZGL" ), cuT( "XGL" ) },
	};

	if ( aiGetVersionMajor() >= 3 )
	{
		if (aiGetVersionMajor () >= 2)
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

	for ( auto l_it : p_engine->GetPluginManager().GetPlugins( Castor3D::ePLUGIN_TYPE_IMPORTER ) )
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

	return l_extensions;
}

C3D_Assimp_API void Create( Castor3D::Engine * p_engine, Castor3D::ImporterPlugin * p_plugin )
{
	Castor3D::ImporterSPtr l_pImporter = std::make_shared< C3dAssimp::AssimpImporter >( *p_engine );
	p_plugin->AttachImporter( l_pImporter );
}

C3D_Assimp_API void Destroy( Castor3D::ImporterPlugin * p_plugin )
{
	p_plugin->DetachImporter();
}

C3D_Assimp_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Assimp_API void OnUnload( Castor3D::Engine * p_engine )
{
}

