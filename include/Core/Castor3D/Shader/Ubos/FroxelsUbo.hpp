/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FroxelsUbo_H___
#define ___C3D_FroxelsUbo_H___

#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"

#include "Castor3D/Shader/Ubos/Ubo.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
{
	namespace shader
	{
		struct FroxelsData
			: public sdw::StructInstanceHelperT< "C3D_FroxelsData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Vec4ArrayField< "depthSegments", 1u + ( MaxFroxelGridDepth / 4u ) >
				, sdw::Vec4ArrayField< "blurKernelWeights", 16u >
				, sdw::Mat4x4Field< "projection" >
				, sdw::Mat4x4Field< "projectionInverse" >
				, sdw::U32Vec4Field< "sampleCount" >
				, sdw::U32Vec3Field< "dimensions" >
				, sdw::UIntField< "blurFilterSize" >
				, sdw::Vec2Field< "froxelSize" >
				, sdw::Vec2Field< "viewNearFar" >
				, sdw::Vec2Field< "renderSize" >
				, sdw::Vec2Field< "invRenderSize" > >
		{
			SDW_DeclStructInstance( C3D_INL_API, FroxelsData );

			FroxelsData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			{
			}

			auto depthSegment( sdw::UInt const & slice )const { return getMember< "depthSegments" >()[slice >> 2u][slice & 0x03u]; }
			auto blurKernelWeight( sdw::UInt const & index )const { return getMember< "blurKernelWeights" >()[index >> 2u][index & 0x03u]; }
			auto projection()const { return getMember< "projection" >(); }
			auto projectionInverse()const { return getMember< "projectionInverse" >(); }
			auto sampleCount()const { return getMember< "sampleCount" >(); }
			auto renderSize()const { return getMember< "renderSize" >(); }
			auto invRenderSize()const { return getMember< "invRenderSize" >(); }
			auto dimensions()const { return getMember< "dimensions" >(); }
			auto blurFilterSize()const { return getMember< "blurFilterSize" >(); }
			auto froxelSize()const { return getMember< "froxelSize" >(); }
			auto viewNearFar()const { return getMember< "viewNearFar" >(); }

			C3D_API sdw::RetU32Vec3 computeIndex3D( sdw::UInt32 const & index );
			C3D_API sdw::RetUInt32 getSamplesCountZ( sdw::U32Vec3 const & froxelIndex3D, sdw::Vec2 const & depthBounds );
			C3D_API sdw::RetFloat getScreenSpaceDepth( sdw::Float const & vsDepth )const;
			C3D_API sdw::RetVec3 getViewSpacePosition( shader::Utils & utils, sdw::Mat4x4 const & invProjection
				, sdw::Float const & ssX, sdw::Float const & ssY, sdw::Float const & ssZ );
			C3D_API sdw::RetUInt32 computeIndex1D( sdw::U32Vec3 const & froxelIndex3D )const;
			C3D_API sdw::RetFloat getSliceDepth( sdw::UInt const & slice )const;
			C3D_API sdw::Vec2 getDepthBounds( sdw::UInt const & slice )const;
			C3D_API sdw::UInt quantize( sdw::Float const & x )const;
			C3D_API sdw::Float dequantize( sdw::UInt const & x )const;

			static u32 constexpr FroxelGroupSize{ 2u };
			static u32 constexpr FroxelsPerGroup{ FroxelGroupSize * FroxelGroupSize * FroxelGroupSize };
			static Array< Point3ui, FroxelsPerGroup > constexpr FroxelOffsets = { Point3ui{ 0u, 0u, 0u }
				, Point3ui{ 0u, 0u, 1u }
				, Point3ui{ 0u, 1u, 0u }
				, Point3ui{ 0u, 1u, 1u }
				, Point3ui{ 1u, 0u, 0u }
				, Point3ui{ 1u, 0u, 1u }
				, Point3ui{ 1u, 1u, 0u }
				, Point3ui{ 1u, 1u, 1u } };

		private:
			sdw::Function< sdw::Vec3
				, sdw::InMat4, sdw::InFloat, sdw::InFloat, sdw::InFloat > m_getViewSpacePosition;
			sdw::Function< sdw::U32Vec3
				, sdw::InUInt32 > m_computeIndex3DIdx;
			sdw::Function< sdw::UInt32
				, sdw::InU32Vec3, sdw::InVec2 > m_getSamplesCountZ;
		};
	}

	class FroxelsUbo
		: public UboT< FroxelsUboConfiguration >
	{
	public:
		using Configuration = FroxelsUboConfiguration;

	public:
		C3D_API explicit FroxelsUbo( RenderDevice const & device );

		C3D_API void cpuUpdate( Point3ui const & gridDim
			, Point2f const & froxelSize
			, Point2f const & renderSize
			, f32 viewNear, f32 viewFar
			, Matrix4x4f const & projection
			, Matrix4x4f const & projectionInverse
			, Point4ui const & sampleCount
			, u32 blurFilterSize
			, ArrayView< f32 const > depthSegments
			, ArrayView< f32 const > kernelWeights );
	};
}

#define C3D_FroxelsEx( writer, binding, set, enabled ) \
	sdw::UniformBuffer froxels{ writer \
		, "C3D_Froxels" \
		, "c3d_froxels" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, uint32_t( set ) \
		, ast::type::MemoryLayout::eStd140 \
		, enabled }; \
	auto c3d_froxelsData = froxels.declMember< c3d::shader::FroxelsData >( "c", enabled ); \
	froxels.end()

#define C3D_Froxels( writer, binding, set ) \
	C3D_FroxelsEx( writer, binding, set, true )

#endif
