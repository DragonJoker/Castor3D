option( CASTOR_BUILD_IMPORTER_ASSIMP "Build Assimp Importer plugin (needs ASSIMP library)" TRUE )
option( CASTOR_BUILD_IMPORTER_GLTF "Build GLTF Importer plugin (needs fastgltf library)" TRUE )
option( CASTOR_BUILD_IMPORTER_FBX "Build FBX Importer plugin (needs FBX SDK)" TRUE )
option( CASTOR_BUILD_IMPORTER_OBJ "Build OBJ Importer plugin" TRUE )
option( CASTOR_BUILD_IMPORTER_3DS "Build 3DS Importer plugin" TRUE )

function( ImportersInit )
	set( Assimp "no (Not wanted)" PARENT_SCOPE )
	set( Gltf "no (Not wanted)" PARENT_SCOPE )
	set( Fbx "no (Not wanted)" PARENT_SCOPE )
	set( Obj "no (Not wanted)" PARENT_SCOPE )
	set( 3ds "no (Not wanted)" PARENT_SCOPE )
endfunction( ImportersInit )

function( ImportersBuild )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		set( ImporterIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Plugins/Importers
			${CASTOR_BINARY_DIR}/source/Plugins/Importers
			${Castor3D_MATERIAL_DIRS}
		)
		set( ImporterDependencies
			${Castor3D_MATERIAL_DEPS}
		)

		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			set( Build ${Gltf} )
			add_subdirectory( Plugins/Importers/GltfImporter )
			set( Gltf ${Build} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_REL ${ADDITIONAL_DEPS_REL} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_DBG ${ADDITIONAL_DEPS_DBG} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( Build ${Assimp} )
			add_subdirectory( Plugins/Importers/AssimpImporter )
			set( Assimp ${Build} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_REL ${ADDITIONAL_DEPS_REL} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_DBG ${ADDITIONAL_DEPS_DBG} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_FBX} )
			set( Build ${Fbx} )
			add_subdirectory( Plugins/Importers/FbxImporter )
			set( Fbx ${Build} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_REL ${ADDITIONAL_DEPS_REL} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_DBG ${ADDITIONAL_DEPS_DBG} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_OBJ} )
			set( Build ${Obj} )
			add_subdirectory( Plugins/Importers/ObjImporter )
			set( Obj ${Build} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_REL ${ADDITIONAL_DEPS_REL} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_DBG ${ADDITIONAL_DEPS_DBG} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_3DS} )
			set( Build ${3ds} )
			add_subdirectory( Plugins/Importers/TdsImporter )
			set( 3ds ${Build} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_REL ${ADDITIONAL_DEPS_REL} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_DBG ${ADDITIONAL_DEPS_DBG} PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)
		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_LIST
			${CASTOR_INSTEXP_LIST}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_PLUGINS
			${CASTOR_INSTEXP_PLUGINS}
			PARENT_SCOPE
		)
	endif ()
endfunction( ImportersBuild )

function( ImportersSummary msg )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		set( msg_tmp "\n\n  Importer plugins:" )
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( msg_tmp "${msg_tmp}\n    Assimp               ${Assimp}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			set( msg_tmp "${msg_tmp}\n    GLTF                 ${Gltf}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_FBX} )
			set( msg_tmp "${msg_tmp}\n    FBX                  ${Fbx}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_OBJ} )
			set( msg_tmp "${msg_tmp}\n    OBJ                  ${Obj}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_3DS} )
			set( msg_tmp "${msg_tmp}\n    3DS                  ${3ds}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( ImportersSummary )

function( ImportersSetup )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			cpack_add_component( AssimpImporter
				DISPLAY_NAME "AssimpImporter"
				DESCRIPTION "Allows the mesh import using Assimp library in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			cpack_add_component( GltfImporter
				DISPLAY_NAME "GltfImporter"
				DESCRIPTION "Allows the mesh import using fastgltf library in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_FBX} )
			cpack_add_component( FbxImporter
				DISPLAY_NAME "FbxImporter"
				DESCRIPTION "Allows the materials import using FBX SDK in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_OBJ} )
			cpack_add_component( ObjImporter
				DISPLAY_NAME "ObjImporter"
				DESCRIPTION "Allows the OBJ materials import in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_3DS} )
			cpack_add_component( TdsImporter
				DISPLAY_NAME "TdsImporter"
				DESCRIPTION "Allows the 3DS materials import in Castor3D."
				GROUP Importers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()
	endif ()
endfunction( ImportersSetup )
