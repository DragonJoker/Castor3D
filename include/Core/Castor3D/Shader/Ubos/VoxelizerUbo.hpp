/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizerUbo_H___
#define ___C3D_VoxelizerUbo_H___

#include "Castor3D/Shader/Ubos/Ubo.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizeModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
{
	namespace shader
	{
		struct VoxelData
			: public sdw::StructInstanceHelperT< "C3D_VoxelData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::FloatField< "worldToGrid" >
				, sdw::FloatField< "gridToWorld" >
				, sdw::FloatField< "clipToGrid" >
				, sdw::FloatField< "gridToClip" >
				, sdw::FloatField< "radianceMaxDistance" >
				, sdw::FloatField< "radianceMips" >
				, sdw::UInt32Field< "radianceNumCones" >
				, sdw::FloatField< "radianceNumConesInv" >
				, sdw::Vec3Field< "pad" >
				, sdw::FloatField< "rayStepSize" >
				, sdw::UInt32Field< "enabled" >
				, sdw::UInt32Field< "enableConservativeRasterization" >
				, sdw::UInt32Field< "enableOcclusion" >
				, sdw::UInt32Field< "enableSecondaryBounce" > >
		{
			SDW_DeclStructInstance( C3D_API, VoxelData );

			VoxelData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
				, worldToGrid{ this->getMember< "worldToGrid" >() }
				, gridToWorld{ this->getMember< "gridToWorld" >() }
				, clipToGrid{ this->getMember< "clipToGrid" >() }
				, gridToClip{ this->getMember< "gridToClip" >() }
				, radianceMaxDistance{ this->getMember< "radianceMaxDistance" >() }
				, radianceMips{ this->getMember< "radianceMips" >() }
				, radianceNumCones{ this->getMember< "radianceNumCones" >() }
				, radianceNumConesInv{ this->getMember< "radianceNumConesInv" >() }
				, rayStepSize{ this->getMember< "rayStepSize" >() }
				, enabled{ this->getMember< "enabled" >() }
				, enableConservativeRasterization{ this->getMember< "enableConservativeRasterization" >() }
				, enableOcclusion{ this->getMember< "enableOcclusion" >() }
				, enableSecondaryBounce{ this->getMember< "enableSecondaryBounce" >() }
			{
			}

			C3D_API sdw::Vec3 worldToTex( sdw::Vec3 const & wsPosition )const;
			C3D_API sdw::IVec3 worldToImg( sdw::Vec3 const & wsPosition )const;
			C3D_API sdw::Vec3 worldToClip( sdw::Vec3 const & wsPosition )const;

			// Raw values
			sdw::Float worldToGrid;
			sdw::Float gridToWorld;
			sdw::Float clipToGrid;
			sdw::Float gridToClip;

			sdw::Float radianceMaxDistance;
			sdw::Float radianceMips;
			sdw::UInt radianceNumCones;
			sdw::Float radianceNumConesInv;

			//! sdw::Vec3 pad;
			sdw::Float rayStepSize;

			sdw::UInt enabled;
			sdw::UInt enableConservativeRasterization;
			sdw::UInt enableOcclusion;
			sdw::UInt enableSecondaryBounce;
		};
	}

	struct VoxelizerUboConfiguration
	{
		/**
		*	- float worldToGrid: Factor from world position to grid position
		*	- float gridToWorld: Factor from grid position to world position
		*	- float gridToClip: Factor from grid position to clip position
		*	- float clipToGrid: Factor from clip position to grid position
		*/
		float worldToGrid;
		float gridToWorld;
		float clipToGrid;
		float gridToClip;
		/**
		*	- float radianceMaxDistance;
		*	- float radianceMips;
		*	- uint radianceNumCones;
		*	- float radianceNumConesInv;
		*/
		float radianceMaxDistance;
		float radianceMips;
		uint32_t radianceNumCones;
		float radianceNumConesInv;
		/**
		*	- vec3 gridCenter: Center of the voxel grid, in world space units
		*	- float rayStepSize;
		*/
		Point3f pad;
		float rayStepSize;
		/**
		*	- uint enabled;
		*	- uint enableConservativeRasterization;
		*	- uint enableOcclusion;
		*	- uint enableSecondaryBounce;
		*/
		uint32_t enabled;
		uint32_t enableConservativeRasterization;
		uint32_t enableOcclusion;
		uint32_t enableSecondaryBounce;
	};

	class VoxelizerUbo
		: public UboT< VoxelizerUboConfiguration >
	{
	public:
		using Configuration = VoxelizerUboConfiguration;

	public:
		C3D_API explicit VoxelizerUbo( RenderDevice const & device );

		C3D_API void cpuUpdate( VctConfig const & voxelConfig
			, float worldToGrid
			, uint32_t voxelGridSize );
	};
}

#define C3D_Voxelizer( writer, binding, set, enable )\
	sdw::UniformBuffer voxelizer{ writer\
		, "C3D_Voxelizer"\
		, "c3d_voxelizer"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, enable };\
	auto c3d_voxelData = voxelizer.declMember< c3d::shader::VoxelData >( "c3d_voxelData" );\
	voxelizer.end()

#endif
