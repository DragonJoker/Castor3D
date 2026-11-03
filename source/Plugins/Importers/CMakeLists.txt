option( CASTOR_BUILD_IMPORTER_ASSIMP "Build Assimp Importer plugin (needs ASSIMP library)" TRUE )
option( CASTOR_BUILD_IMPORTER_GLTF "Build GLTF Importer plugin (needs fastgltf library)" TRUE )
option( CASTOR_BUILD_IMPORTER_FBX "Build FBX Material Importer plugin (needs FBX SDK)" TRUE )
option( CASTOR_BUILD_IMPORTER_PLY "Build PLY Mesh Importer" TRUE )

function( ImportersInit )
	set( Assimp "no (Not wanted)" PARENT_SCOPE )
	set( GltfMat "no (Not wanted)" PARENT_SCOPE )
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
			set( Build ${GltfMat} )
			add_subdirectory( Plugins/Importers/GltfMaterialImporter )
			set( GltfMat ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			set( Build ${Gltf} )
			add_subdirectory( Plugins/Importers/GltfImporter )
			set( Gltf ${Build} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_REL ${ADDITIONAL_DEPS_REL} PARENT_SCOPE )
			set( ADDITIONAL_DEPS_DBG ${ADDITIONAL_DEPS_DBG} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_FBX} )
			set( Build ${Fbx} )
			add_subdirectory( Plugins/Importers/FbxMaterialImporter )
			set( Fbx ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_IMPORTER_PLY} )
			set( Build ${Ply} )
			add_subdirectory( Plugins/Importers/PlyMeshImporter )
			set( Ply ${Build} PARENT_SCOPE )
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
		if( ${CASTOR_BUILD_IMPORTER_FBX} )
			set( msg_tmp "${msg_tmp}\n    FBX Materials        ${Fbx}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_PLY} )
			set( msg_tmp "${msg_tmp}\n    PLY Meshes           ${Ply}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			set( msg_tmp "${msg_tmp}\n    glTF Materials       ${GltfMat}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			set( msg_tmp "${msg_tmp}\n    glTF Scenes          ${Gltf}" )
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			set( msg_tmp "${msg_tmp}\n    Assimp               ${Assimp}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( ImportersSummary )

function( ImportersSetup )
	if ( ${CASTOR_BUILDGRP_IMPORTER} )
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} OR ${CASTOR_BUILD_IMPORTER_FBX} )
			cpack_add_component( FbxMaterialImporter
				DISPLAY_NAME "FbxMaterialImporter"
				DESCRIPTION "Allows importing materials from FBX files, for Assimp scene importer in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_PLY} )
			cpack_add_component( PlyImporter
				DISPLAY_NAME "PlyImporter"
				DESCRIPTION "Allows importing meshes from Stanford PLY files, for Assimp scene importer in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			cpack_add_component( GltfMaterialImporter
				DISPLAY_NAME "GltfMaterialImporter"
				DESCRIPTION "Allows importing materials from glTF files, for both glTF and Assimp scene importers in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
			cpack_add_component( AssimpImporter
				DISPLAY_NAME "AssimpImporter"
				DESCRIPTION "Allows importing scenes using Assimp library in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
		if( ${CASTOR_BUILD_IMPORTER_GLTF} )
			cpack_add_component( GltfImporter
				DISPLAY_NAME "GltfImporter"
				DESCRIPTION "Allows importing scenes using fastgltf library in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Minimal Full
			)
		endif ()
	endif ()
endfunction( ImportersSetup )
