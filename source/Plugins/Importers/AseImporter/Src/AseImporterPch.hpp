/* See LICENSE file in root folder */
#ifndef ___ASE_IMPORTER_PCH_H___
#define ___ASE_IMPORTER_PCH_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	include <Castor3DPrerequisites.hpp>

#	include <Engine.hpp>
#	include <Cache/MaterialCache.hpp>
#	include <Cache/MeshCache.hpp>
#	include <Cache/SceneCache.hpp>
#	include <Cache/SceneNodeCache.hpp>

#	include <Cache/CacheView.hpp>
#	include <Material/Pass.hpp>
#	include <Mesh/Submesh.hpp>
#	include <Mesh/Face.hpp>
#	include <Mesh/Vertex.hpp>
#	include <Mesh/Buffer/Buffer.hpp>
#	include <Miscellaneous/Version.hpp>
#	include <Plugin/Plugin.hpp>
#	include <Plugin/ImporterPlugin.hpp>
#	include <Render/RenderSystem.hpp>
#	include <Render/Viewport.hpp>
#	include <Scene/Camera.hpp>
#	include <Scene/Geometry.hpp>
#	include <Texture/TextureLayout.hpp>
#	include <Texture/TextureUnit.hpp>
#endif

#endif
