option( CASTOR_BUILDGRP_IMPORTER "Build Castor3D Importer plugins" TRUE )
option( CASTOR_BUILD_IMPORTER_ASSIMP "Build Assimp Importer plugin (needs ASSIMP library)" TRUE )
option( CASTOR_BUILD_IMPORTER_OBJ "Build OBJ Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_PLY "Build PLY Importer plugin" TRUE )

function( ImportersInit )
	set( Assimp "no (Not wanted)" PARENT_SCOPE )
	set( Obj "no (Not wanted)" PARENT_SCOPE )
	set( Ply "no (Not wanted)" PARENT_SCOPE )
endfunction( ImportersInit )

function( ImportersBuild )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		set( ImporterIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/include/Plugins/Importers
			${CASTOR_BINARY_DIR}/include/Plugins/Importers
			${CASTOR_SOURCE_DIR}/source/Plugins/Importers
			${CASTOR_BINARY_DIR}/source/Plugins/Importers
		)

		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( Build ${Assimp} )
			add_subdirectory( Plugins/Importers/AssimpImporter )
			set( Assimp ${Build} PARENT_SCOPE )
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

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)

		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
	endif ()
endfunction( ImportersBuild )

function( ImportersSummary msg )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		set( msg_tmp "\n\nImporters :" )
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( msg_tmp "${msg_tmp}\n    Assimp               ${Assimp}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_OBJ} )
			set( msg_tmp "${msg_tmp}\n    OBJ                  ${Obj}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_PLY} )
			set( msg_tmp "${msg_tmp}\n    PLY                  ${Ply}" )
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

		if( ${CASTOR_BUILD_IMPORTER_OBJ} )
			cpack_add_component( ObjImporter
				DISPLAY_NAME "ObjImporter"
				DESCRIPTION "Allows the OBJ objects import in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Full
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
	endif ()
endfunction( ImportersSetup )
