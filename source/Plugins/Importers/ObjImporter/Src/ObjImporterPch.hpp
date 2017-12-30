/*
See LICENSE file in root folder
*/
#ifndef ___OBJ_IMPORTER_PCH_H___
#define ___OBJ_IMPORTER_PCH_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	include <Engine.hpp>
#	include <Cache/GeometryCache.hpp>
#	include <Cache/MaterialCache.hpp>
#	include <Cache/MeshCache.hpp>
#	include <Cache/SceneCache.hpp>
#	include <Cache/SceneNodeCache.hpp>
#	include <Material/Pass.hpp>
#	include <Mesh/SubmeshComponent/Face.hpp>
#	include <Mesh/Submesh.hpp>
#	include <Mesh/Vertex.hpp>
#	include <Mesh/Buffer/Buffer.hpp>
#	include <Miscellaneous/Version.hpp>
#	include <Plugin/Plugin.hpp>
#	include <Texture/TextureLayout.hpp>
#	include <Texture/TextureUnit.hpp>
#endif

#endif
