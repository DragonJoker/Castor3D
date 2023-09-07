#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/ImporterFile.hpp>
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

namespace
{
	std::vector< std::pair< castor::String, castor3d::ImporterPlugin::Extension > > getExtensions( castor3d::Engine * engine )
	{
		std::vector< std::pair< castor::String, castor3d::ImporterPlugin::Extension > > extensions;
		using Extension = castor3d::ImporterPlugin::Extension;

		if ( extensions.empty() )
		{
			extensions.emplace_back( cuT( "3ds" ), Extension{ cuT( "3DS" ), cuT( "3D Studio Max 3DS" ) } );
			extensions.emplace_back( cuT( "ac" ), Extension{ cuT( "AC" ), cuT( "AC3D" ) } );
			extensions.emplace_back( cuT( "acc" ), Extension{ cuT( "ACC" ), cuT( "AC3D" ) } );
			extensions.emplace_back( cuT( "ac3d" ), Extension{ cuT( "AC3D" ), cuT( "AC3D" ) } );
			extensions.emplace_back( cuT( "ase" ), Extension{ cuT( "ASE" ), cuT( "3D Studio Max ASE" ) } );
			extensions.emplace_back( cuT( "ask" ), Extension{ cuT( "ASK" ), cuT( "3D Studio Max ASE" ) } );
			extensions.emplace_back( cuT( "blend" ), Extension{ cuT( "BLEND" ), cuT( "Blender" ) } );
			extensions.emplace_back( cuT( "bvh" ), Extension{ cuT( "BVH" ), cuT( "Biovision BVH" ) } );
			extensions.emplace_back( cuT( "cob" ), Extension{ cuT( "COB" ), cuT( "TrueSpace" ) } );
			extensions.emplace_back( cuT( "csm" ), Extension{ cuT( "CSM" ), cuT( "CharacterStudio Motion" ) } );
			extensions.emplace_back( cuT( "dae" ), Extension{ cuT( "DAE" ), cuT( "Collada" ) } );
			extensions.emplace_back( cuT( "dxf" ), Extension{ cuT( "DXF" ), cuT( "Autodesk DXF" ) } );
			extensions.emplace_back( cuT( "enff" ), Extension{ cuT( "ENFF" ), cuT( "Neutral File Format" ) } );
			extensions.emplace_back( cuT( "fbx" ), Extension{ cuT( "FBX" ), cuT( "Autodesk FBX" ) } );
			extensions.emplace_back( cuT( "hmp" ), Extension{ cuT( "HMP" ), cuT( "3D GameStudio Heightmap" ) } );
			extensions.emplace_back( cuT( "ifc" ), Extension{ cuT( "IFC" ), cuT( "IFC-STEP, Industry Foundation Classes" ) } );
			extensions.emplace_back( cuT( "ifczip" ), Extension{ cuT( "IFCZIP" ), cuT( "IFC-STEP, Industry Foundation Classes" ) } );
			extensions.emplace_back( cuT( "irr" ), Extension{ cuT( "IRR" ), cuT( "Irrlicht Scene" ) } );
			extensions.emplace_back( cuT( "irrmesh" ), Extension{ cuT( "IRRMESH" ), cuT( "Irrlicht Mesh" ) } );
			extensions.emplace_back( cuT( "lws" ), Extension{ cuT( "LWS" ), cuT( "LightWave Scene" ) } );
			extensions.emplace_back( cuT( "lwo" ), Extension{ cuT( "LWO" ), cuT( "LightWave/Modo Object" ) } );
			extensions.emplace_back( cuT( "lxo" ), Extension{ cuT( "LXO" ), cuT( "LightWave/Modo Object" ) } );
			extensions.emplace_back( cuT( "md2" ), Extension{ cuT( "MD2" ), cuT( "Quake II Mesh" ) } );
			extensions.emplace_back( cuT( "md3" ), Extension{ cuT( "MD3" ), cuT( "Quake III Mesh" ) } );
			extensions.emplace_back( cuT( "md5mesh" ), Extension{ cuT( "MD5MESH" ), cuT( "doom 3 / MD5 Mesh" ) } );
			extensions.emplace_back( cuT( "md5anim" ), Extension{ cuT( "MD5ANIM" ), cuT( "doom 3 / MD5 Animation" ) } );
			extensions.emplace_back( cuT( "mdc" ), Extension{ cuT( "MDC" ), cuT( "Return To Castle Wolfenstein Mesh" ) } );
			extensions.emplace_back( cuT( "mdl" ), Extension{ cuT( "MDL" ), cuT( "Quake Mesh / 3D GameStudio Mesh" ) } );
			extensions.emplace_back( cuT( "mesh" ), Extension{ cuT( "MESH" ), cuT( "Ogre 3D Mesh" ) } );
			extensions.emplace_back( cuT( "mesh" ), Extension{ cuT( "MESH.XML" ), cuT( "LOgre 3D Mesh" ) } );
			extensions.emplace_back( cuT( "mot" ), Extension{ cuT( "MOT" ), cuT( "LightWave Scene" ) } );
			extensions.emplace_back( cuT( "ms3d" ), Extension{ cuT( "MS3D" ), cuT( "Milkshape 3D" ) } );
			extensions.emplace_back( cuT( "nff" ), Extension{ cuT( "NFF" ), cuT( "Neutral File Format" ) } );
			extensions.emplace_back( cuT( "off" ), Extension{ cuT( "OFF" ), cuT( "Object File Format" ) } );
			extensions.emplace_back( cuT( "pk3" ), Extension{ cuT( "PK3" ), cuT( "Quake III BSP" ) } );
			extensions.emplace_back( cuT( "prj" ), Extension{ cuT( "PRJ" ), cuT( "3D Studio Max 3DS" ) } );
			extensions.emplace_back( cuT( "q3o" ), Extension{ cuT( "Q3O" ), cuT( "Quick3D" ) } );
			extensions.emplace_back( cuT( "q3s" ), Extension{ cuT( "Q3S" ), cuT( "Quick3D" ) } );
			extensions.emplace_back( cuT( "raw" ), Extension{ cuT( "RAW" ), cuT( "Raw Triangles" ) } );
			extensions.emplace_back( cuT( "scn" ), Extension{ cuT( "SCN" ), cuT( "TrueSpace" ) } );
			extensions.emplace_back( cuT( "smd" ), Extension{ cuT( "SMD" ), cuT( "Valve Model" ) } );
			extensions.emplace_back( cuT( "stl" ), Extension{ cuT( "STL" ), cuT( "Stereolithography" ) } );
			extensions.emplace_back( cuT( "ter" ), Extension{ cuT( "TER" ), cuT( "Terragen Heightmap" ) } );
			extensions.emplace_back( cuT( "vta" ), Extension{ cuT( "VTA" ), cuT( "Valve Model" ) } );
			extensions.emplace_back( cuT( "x" ), Extension{ cuT( "X" ), cuT( "Direct3D XFile" ) } );
			extensions.emplace_back( cuT( "xgl" ), Extension{ cuT( "XGL" ), cuT( "XGL" ) } );
			extensions.emplace_back( cuT( "xml" ), Extension{ cuT( "XML" ), cuT( "Irrlicht Scene" ) } );
			extensions.emplace_back( cuT( "zgl" ), Extension{ cuT( "ZGL" ), cuT( "XGL" ) } );
			extensions.emplace_back( cuT( "obj" ), Extension{ cuT( "OBJ" ), cuT( "Wavefront Object" ) } );
			extensions.emplace_back( cuT( "ply" ), Extension{ cuT( "PLY" ), cuT( "Stanford Polygon Library" ) } );

			if ( aiGetVersionMajor() > 3
				|| ( aiGetVersionMajor() == 3 && aiGetVersionMajor() >= 2 ) )
			{
				extensions.emplace_back( cuT( "3d" ), Extension{ cuT( "3D" ), cuT( "Unreal" ) } );
				extensions.emplace_back( cuT( "assbin" ), Extension{ cuT( "ASSBIN" ), cuT( "Assimp binary dump" ) } );
				extensions.emplace_back( cuT( "b3d" ), Extension{ cuT( "B3D" ), cuT( "BlitzBasic 3D" ) } );
				extensions.emplace_back( cuT( "ndo" ), Extension{ cuT( "NDO" ), cuT( "Nendo Mesh" ) } );
				extensions.emplace_back( cuT( "ogex" ), Extension{ cuT( "OGEX" ), cuT( "open Game Engine Exchange" ) } );
				extensions.emplace_back( cuT( "uc" ), Extension{ cuT( "UC" ), cuT( "Unreal" ) } );
				extensions.emplace_back( cuT( "assimp" ), Extension{ cuT( "GLTF" ), cuT( "Text glTF" ) } );
				extensions.emplace_back( cuT( "assimp" ), Extension{ cuT( "GLB" ), cuT( "Binary glTF" ) } );
			}
		}

		return extensions;
	}
}

extern "C"
{
	C3D_Assimp_API void getRequiredVersion( castor3d::Version * version );
	C3D_Assimp_API void getType( castor3d::PluginType * type );
	C3D_Assimp_API void getName( char const ** name );
	C3D_Assimp_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_Assimp_API void OnUnload( castor3d::Engine * engine );

	C3D_Assimp_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Assimp_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eImporter;
	}

	C3D_Assimp_API void getName( char const ** name )
	{
		*name = c3d_assimp::AssimpImporterFile::Name.c_str();
	}

	C3D_Assimp_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		auto importer = static_cast< castor3d::ImporterPlugin * >( plugin );
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			importer->addExtension( extension.second );
			engine->getImporterFileFactory().registerType( castor::string::lowerCase( extension.second.first )
				, extension.first
				, &c3d_assimp::AssimpImporterFile::create );
		}
	}

	C3D_Assimp_API void OnUnload( castor3d::Engine * engine )
	{
		auto extensions = getExtensions( engine );

		for ( auto const & extension : extensions )
		{
			engine->getImporterFileFactory().unregisterType( castor::string::lowerCase( extension.second.first )
				, extension.first );
		}
	}
}
