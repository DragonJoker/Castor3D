/*
See LICENSE file in root folder
*/
#ifndef ___OBJ_IMPORTER_PCH_H___
#define ___OBJ_IMPORTER_PCH_H___

#include <CastorUtils/config.hpp>

#if CU_UsePCH
#	include <Castor3D/Engine.hpp>
#	include <Castor3D/Cache/GeometryCache.hpp>
#	include <Castor3D/Cache/MaterialCache.hpp>
#	include <Castor3D/Cache/MeshCache.hpp>
#	include <Castor3D/Cache/SceneCache.hpp>
#	include <Castor3D/Cache/SceneNodeCache.hpp>
#	include <Castor3D/Material/Pass/Pass.hpp>
#	include <Castor3D/Model/Mesh/Submesh/Component/Face.hpp>
#	include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#	include <Castor3D/Model/Vertex.hpp>
#	include <Castor3D/Miscellaneous/Version.hpp>
#	include <Castor3D/Plugin/Plugin.hpp>
#	include <Castor3D/Material/Texture/TextureLayout.hpp>
#	include <Castor3D/Material/Texture/TextureUnit.hpp>

#	include <ashespp/Buffer/Buffer.hpp>
#endif

#endif
