/* See LICENSE file in root folder */
#ifndef ___LWO_IMPORTER_PCH_H___
#define ___LWO_IMPORTER_PCH_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	include <Graphics/Colour.hpp>

#	include <Engine.hpp>
#	include <Cache/MeshCache.hpp>
#	include <Cache/GeometryCache.hpp>
#	include <Cache/MaterialCache.hpp>
#	include <Cache/SceneCache.hpp>
#	include <Cache/SceneNodeCache.hpp>
#	include <Event/Frame/InitialiseEvent.hpp>
#	include <Material/Pass.hpp>
#	include <Mesh/SubmeshComponent/Face.hpp>
#	include <Mesh/Importer.hpp>
#	include <Mesh/Submesh.hpp>
#	include <Mesh/Vertex.hpp>
#	include <Mesh/Buffer/Buffer.hpp>
#	include <Miscellaneous/Version.hpp>
#	include <Plugin/ImporterPlugin.hpp>
#	include <Plugin/Plugin.hpp>
#	include <Texture/TextureLayout.hpp>
#	include <Texture/TextureUnit.hpp>
#endif

#endif
