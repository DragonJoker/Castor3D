option( CASTOR_BUILDGRP_IMPORTER "Build Castor3D Importer plugins" TRUE )
option( CASTOR_BUILD_IMPORTER_ASSIMP "Build Assimp Importer plugin (needs ASSIMP library)" TRUE )
option( CASTOR_BUILD_IMPORTER_ASE "Build ASE Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_FBX "Build FBX Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_LWO "Build LWO Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_MD2 "Build MD2 Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_MD3 "Build MD3 Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_OBJ "Build OBJ Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_PLY "Build PLY Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_3DS "Build 3DS Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_OGR "Build Ogre Importer plugin" FALSE )
option( CASTOR_FORCE_ALL_IMPORTERS "Build every Importer plugin regardless of the presence of ASSIMP" FALSE )

function( ImportersInit )
	set( Assimp "no (Not wanted)" PARENT_SCOPE )
	set( Ase "no (Not wanted)" PARENT_SCOPE )
	set( Fbx "no (Not wanted)" PARENT_SCOPE )
	set( Md2 "no (Not wanted)" PARENT_SCOPE )
	set( Md3 "no (Not wanted)" PARENT_SCOPE )
	set( 3ds "no (Not wanted)" PARENT_SCOPE )
	set( Lwo "no (Not wanted)" PARENT_SCOPE )
	set( Obj "no (Not wanted)" PARENT_SCOPE )
	set( Ply "no (Not wanted)" PARENT_SCOPE )
	set( Ogr "no (Not wanted)" PARENT_SCOPE )
endfunction( ImportersInit )

function( ImportersBuild )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		set( BUILTIN_NEEDED ON )

		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( Build ${Assimp} )
			add_subdirectory( Plugins/Importers/AssimpImporter )
			set( Assimp ${Build} PARENT_SCOPE )
			set( BUILTIN_NEEDED OFF )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_FBX} )
			set( Build ${Fbx} )
			add_subdirectory( Plugins/Importers/FbxImporter )
			set( Fbx ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_OGR} )
			set( Build ${Ogr} )
			add_subdirectory( Plugins/Importers/OgreImporter )
			set( Ogr ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_OBJ} )
			set( Build ${Obj} )
			add_subdirectory( Plugins/Importers/ObjImporter )
			set( Obj ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_PLY} )
			set( Build ${Ply} )
			add_subdirectory( Plugins/Importers/PlyImporter )
			set( Ply ${Build} PARENT_SCOPE )
		endif()

		if( BUILTIN_NEEDED OR CASTOR_FORCE_ALL_IMPORTERS )
			msg_debug( "using builtin importers" )
			if( ${CASTOR_BUILD_IMPORTER_ASE} )
				set( Build ${Ase} )
				add_subdirectory( Plugins/Importers/AseImporter )
				set( Ase ${Build} PARENT_SCOPE )
			endif()

			if( ${CASTOR_BUILD_IMPORTER_LWO} )
				set( Build ${Lwo} )
				add_subdirectory( Plugins/Importers/LwoImporter )
				set( Lwo ${Build} PARENT_SCOPE )
			endif()

			if( ${CASTOR_BUILD_IMPORTER_MD2} )
				set( Build ${Md2} )
				add_subdirectory( Plugins/Importers/Md2Importer )
				set( Md2 ${Build} PARENT_SCOPE )
			endif()

			if( ${CASTOR_BUILD_IMPORTER_MD3} )
				set( Build ${Md3} )
				add_subdirectory( Plugins/Importers/Md3Importer )
				set( Md3 ${Build} PARENT_SCOPE )
			endif()

			if( ${CASTOR_BUILD_IMPORTER_3DS} )
				set( Build ${3ds} )
				add_subdirectory( Plugins/Importers/SMaxImporter )
				set( 3ds ${Build} PARENT_SCOPE )
			endif()
		else()
			set( Ase "no (Not needed)" PARENT_SCOPE )
			set( Md2 "no (Not needed)" PARENT_SCOPE )
			set( Md3 "no (Not needed)" PARENT_SCOPE )
			set( 3ds "no (Not needed)" PARENT_SCOPE )
			set( Lwo "no (Not needed)" PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)

		set( CastorBinsDependencies
			${CastorBinsDependencies}
			PARENT_SCOPE
		)
	endif ()
endfunction( ImportersBuild )

function( ImportersSummary msg )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		set( msg_tmp "\n\nImporters :" )
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( msg_tmp "${msg_tmp}\n    Assimp              ${Assimp}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_FBX} )
			set( msg_tmp "${msg_tmp}\n    Built-in FBX        ${Fbx}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_OBJ} )
			set( msg_tmp "${msg_tmp}\n    Built-in OBJ        ${Obj}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_OGR} )
			set( msg_tmp "${msg_tmp}\n    Built-in Ogre       ${Ogr}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_PLY} )
			set( msg_tmp "${msg_tmp}\n    Built-in PLY        ${Ply}" )
		endif ()

		STRING( FIND "${Assimp}" "yes" POS )
		if( ${POS} STREQUAL -1 OR CASTOR_FORCE_ALL_IMPORTERS )
			if( ${CASTOR_BUILD_IMPORTER_ASE} )
				set( msg_tmp "${msg_tmp}\n    Built-in ASE        ${Ase}" )
			endif ()
			if( ${CASTOR_BUILD_IMPORTER_MD2} )
				set( msg_tmp "${msg_tmp}\n    Built-in MD2        ${Md2}" )
			endif ()
			if( ${CASTOR_BUILD_IMPORTER_MD3} )
				set( msg_tmp "${msg_tmp}\n    Built-in MD3        ${Md3}" )
			endif ()
			if( ${CASTOR_BUILD_IMPORTER_3DS} )
				set( msg_tmp "${msg_tmp}\n    Built-in 3DS        ${3ds}" )
			endif ()
			if( ${CASTOR_BUILD_IMPORTER_LWO} )
				set( msg_tmp "${msg_tmp}\n    Built-in LWO        ${Lwo}" )
			endif ()
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( ImportersSummary )

function( ImportersSetup )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		cpack_add_component_group( Importers
			DISPLAY_NAME "Importers"
			DESCRIPTION "Importer plugins usable in Castor3D."
			PARENT_GROUP Plugins
		)
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			cpack_add_component( AssimpImporter
				DISPLAY_NAME "AssimpImporter"
				DESCRIPTION "Allows the mesh import using Assimp library in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()

		if( ${CASTOR_BUILD_IMPORTER_FBX} )
			cpack_add_component( FbxImporter
				DISPLAY_NAME "FbxImporter"
				DESCRIPTION "Allows the FBX objects import in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()

		if( ${CASTOR_BUILD_IMPORTER_OBJ} )
			cpack_add_component( ObjImporter
				DISPLAY_NAME "ObjImporter"
				DESCRIPTION "Allows the OBJ objects import in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()

		if( ${CASTOR_BUILD_IMPORTER_OGR} )
			cpack_add_component( OgreImporter
				DISPLAY_NAME "OgreImporter"
				DESCRIPTION "Allows the Ogre3D objects import in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()

		if( ${CASTOR_BUILD_IMPORTER_PLY} )
			cpack_add_component( PlyImporter
				DISPLAY_NAME "PlyImporter"
				DESCRIPTION "Allows the Stanford PLY objects import in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()

		STRING( FIND "${Assimp}" "yes" POS )
		if( ${POS} STREQUAL -1 OR CASTOR_FORCE_ALL_IMPORTERS )
			if( ${CASTOR_BUILD_IMPORTER_ASE} )
				cpack_add_component( AseImporter
					DISPLAY_NAME "AseImporter"
					DESCRIPTION "Allows the ASE objects import in Castor3D."
					GROUP Importers
					DEPENDS Castor3D
					INSTALL_TYPES Full
				)
			endif ()

			if( ${CASTOR_BUILD_IMPORTER_LWO} )
				cpack_add_component( LwoImporter
					DISPLAY_NAME "AseImporter"
					DESCRIPTION "Allows the LightWave Objects import in Castor3D."
					GROUP Importers
					DEPENDS Castor3D
					INSTALL_TYPES Full
				)
			endif ()

			if( ${CASTOR_BUILD_IMPORTER_MD2} )
				cpack_add_component( Md2Importer
					DISPLAY_NAME "Md2Importer"
					DESCRIPTION "Allows the MD2 objects import in Castor3D."
					GROUP Importers
					DEPENDS Castor3D
					INSTALL_TYPES Full
				)
			endif ()

			if( ${CASTOR_BUILD_IMPORTER_MD3} )
				cpack_add_component( Md3Importer
					DISPLAY_NAME "Md3Importer"
					DESCRIPTION "Allows the MD3 objects import in Castor3D."
					GROUP Importers
					DEPENDS Castor3D
					INSTALL_TYPES Full
				)
			endif ()

			if( ${CASTOR_BUILD_IMPORTER_3DS} )
				cpack_add_component( SMaxImporter
					DISPLAY_NAME "SMaxImporter"
					DESCRIPTION "Allows the 3D Studio Max objects import in Castor3D."
					GROUP Importers
					DEPENDS Castor3D
					INSTALL_TYPES Full
				)
			endif ()
		endif()
	endif ()
endfunction( ImportersSetup )
