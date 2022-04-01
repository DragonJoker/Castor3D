/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslGlobalIllumination_H___
#define ___C3D_GlslGlobalIllumination_H___

#include "SdwModule.hpp"

#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	class GlobalIllumination
	{
	public:
		C3D_API explicit GlobalIllumination( sdw::ShaderWriter & writer
			, Utils & utils );
		C3D_API explicit GlobalIllumination( sdw::ShaderWriter & writer
			, Utils & utils
			, uint32_t & bindingIndex
			, uint32_t setIndex
			, SceneFlags sceneFlags
			, IndirectLightingData const & indirectLighting );

		C3D_API void computeCombinedDifSpec( SceneFlags sceneFlags
			, bool hasDiffuseGI
			, CookTorranceBRDF & cookTorrance
			, LightSurface lightSurface
			, sdw::Float roughness
			, sdw::CombinedImage2DRgba32 brdfMap
			, IndirectLighting & indirectLighting
			, DebugOutput & debugOutput );
		C3D_API void computeOcclusion( SceneFlags sceneFlags
			, LightSurface lightSurface
			, IndirectLighting & indirectLighting
			, DebugOutput & debugOutput );
		C3D_API void computeDiffuse( SceneFlags sceneFlags
			, LightSurface lightSurface
			, IndirectLighting & indirectLighting
			, DebugOutput & debugOutput );
		C3D_API void computeAmbient( SceneFlags sceneFlags
			, IndirectLighting & indirectLighting
			, DebugOutput & debugOutput );
		C3D_API void computeSpecular( SceneFlags sceneFlags
			, LightSurface lightSurface
			, sdw::Float roughness
			, sdw::CombinedImage2DRgba32 brdfMap
			, IndirectLighting & indirectLighting
			, DebugOutput & debugOutput );
		C3D_API sdw::Vec4 traceConeRadiance( sdw::CombinedImage3DRgba32 const & voxels
			, LightSurface lightSurface
			, VoxelData const & voxelData );
		C3D_API sdw::Vec4 traceConeRadiance( sdw::CombinedImage3DRgba32 const & voxels
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, VoxelData const & voxelData );

	private:
		void declareVct( uint32_t & uboBindingIndex
			, uint32_t & texBindingIndex
			, uint32_t uboSetIndex = 0u
			, uint32_t texSetIndex = 0u );
		void declareRsm( uint32_t & texBindingIndex
			, uint32_t texSetIndex = 0u );
		void declareLpv( uint32_t & uboBindingIndex
			, uint32_t & texBindingIndex
			, uint32_t uboSetIndex = 0u
			, uint32_t texSetIndex = 0u );
		void declareLayeredLpv( uint32_t & uboBindingIndex
			, uint32_t & texBindingIndex
			, uint32_t uboSetIndex = 0u
			, uint32_t texSetIndex = 0u );
		sdw::Vec4 computeVCTRadiance( LightSurface const & lightSurface
			, VoxelData const & voxelData
			, sdw::Float const & indirectOcclusion );
		sdw::Vec3 computeVCTSpecular( LightSurface const & lightSurface
			, sdw::Float const & roughness
			, sdw::Float const & indirectOcclusion
			, sdw::Float const & indirectBlend
			, VoxelData const & voxelData );
		sdw::Vec4 computeRSMRadiance( sdw::Vec2 const & texcoord );
		sdw::Vec4 computeLPVRadiance( LightSurface surface
			, LpvGridData lpvGridData );
		sdw::Vec4 computeLLPVRadiance( LightSurface surface
			, LayeredLpvGridData llpvGridData );
		sdw::Vec4 traceCone( sdw::CombinedImage3DRgba32 const & voxels
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsConeDirection
			, sdw::Float const & coneAperture
			, VoxelData const & voxelData );
		sdw::Vec4 traceConeReflection( sdw::CombinedImage3DRgba32 const & voxels
			, LightSurface lightSurface
			, sdw::Float const & roughness
			, VoxelData const & voxelData );
		sdw::Float traceConeOcclusion( sdw::CombinedImage3DRgba32 const & voxels
			, LightSurface lightSurface
			, VoxelData const & voxelData );

	private:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		sdw::Function< sdw::Vec4
			, sdw::InVec3 > m_evalSH;
		sdw::Function< sdw::Vec4
			, sdw::InVec2 > m_computeRSMRadiance;
		sdw::Function< sdw::Vec4
			, sdw::InVec3
			, sdw::InVec3
			, InLpvGridData > m_computeLPVRadiance;
		sdw::Function< sdw::Vec4
			, sdw::InVec3
			, sdw::InVec3
			, InLayeredLpvGridData > m_computeLLPVRadiance;
		sdw::Function< sdw::Vec4
			, sdw::InCombinedImage3DRgba32
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, InVoxelData > m_traceCone;
		sdw::Function< sdw::Vec4
			, sdw::InCombinedImage3DRgba32
			, sdw::InVec3
			, sdw::InVec3
			, InVoxelData > m_traceConeRadiance;
		sdw::Function< sdw::Vec4
			, sdw::InCombinedImage3DRgba32
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, InVoxelData > m_traceConeReflection;
		sdw::Function< sdw::Float
			, sdw::InCombinedImage3DRgba32
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, InVoxelData > m_traceConeOcclusion;
	};
}

#endif
