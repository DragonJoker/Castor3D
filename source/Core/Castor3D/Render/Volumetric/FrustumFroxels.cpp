#include "Castor3D/Render/Volumetric/FrustumFroxels.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Volumetric/AssignLightsToFroxels.hpp"
#include "Castor3D/Render/Volumetric/FinaliseFroxelsLighting.hpp"
#include "Castor3D/Render/Volumetric/FroxelsConfig.hpp"
#include "Castor3D/Render/Volumetric/IntegrateFroxels.hpp"
#include "Castor3D/Render/Debug/DebugModule.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <RenderGraph/FramePassGroup.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace frsfxl
	{
		static uint32_t constexpr MaxLightsPerSlice = 64u;
		static uint32_t constexpr MaxFroxelGridWidth = 128u;
		static uint32_t constexpr MaxFroxelGridHeight = 128u;
		static uint32_t constexpr MaxFroxelCount = MaxFroxelGridWidth * MaxFroxelGridHeight * MaxFroxelGridDepth;
		static Extent3D constexpr MaxFroxelExtent = { MaxFroxelGridWidth, MaxFroxelGridHeight, MaxFroxelGridDepth };

		static void computeDimension( FroxelsConfig const & config
			, u32 renderWidth, u32 renderHeight
			, Point3ui & dimensionsWithMargin, Point3ui & dimensionsWithoutMargin )
		{
			dimensionsWithoutMargin = { 16u, 16u, MaxFroxelGridDepth };

			f32 aspectRatio = f32( std::max( 1u, renderWidth ) ) / f32( std::max( 1u, renderHeight ) );

			// check total blur width never larger than the image size, /4 is arbitrary here
			u32 nBlurFilterHalfSize = std::min( config.blurFilterSize.value().value(), MaxFroxelGridWidth / 4u );

			// total froxels per slice
			auto T = f32( MaxFroxelGridWidth * MaxFroxelGridWidth );
			// margin because of blur
			auto m = f32( nBlurFilterHalfSize );
			//
			auto a = 1.0f / aspectRatio;
			auto b = 2.0f * m * ( 1.0f + 1.0f / aspectRatio );
			auto c = 4.0f * m * m - T;
			auto delta = b * b - 4.0f * a * c;
			CU_Require( delta >= 0.0f );
			if ( delta >= 0.0f )
			{
				f32 x1 = ( -b + sqrt( delta ) ) / ( 2.0f * a );

				dimensionsWithoutMargin->x = ( u32 )std::ceil( x1 );
				dimensionsWithoutMargin->y = ( u32 )std::ceil( x1 / aspectRatio );

				// clamp to max image size
				dimensionsWithoutMargin->x = std::min( dimensionsWithoutMargin->x, MaxFroxelGridWidth - 2u * nBlurFilterHalfSize );
				dimensionsWithoutMargin->y = std::min( dimensionsWithoutMargin->y, MaxFroxelGridHeight - 2u * nBlurFilterHalfSize );
			}

			//
			dimensionsWithMargin = dimensionsWithoutMargin;
			dimensionsWithMargin->x += 2u * nBlurFilterHalfSize;
			dimensionsWithMargin->y += 2u * nBlurFilterHalfSize;

			// final check
			CU_Require( dimensionsWithMargin->x <= MaxFroxelGridWidth );
			CU_Require( dimensionsWithMargin->y <= MaxFroxelGridHeight );
		}

		static Angle updateAngle( Angle const & theta, f64 srcOpposite, f64 dstOpposite )
		{
			if ( srcOpposite == dstOpposite )
				return theta;
			f64 adjacent = theta.radians() == 0.0
				? srcOpposite / 2.0
				: srcOpposite / ( 2.0 * ( theta / 2.0 ).tan() );
			return atanf( ( dstOpposite / 2.0 ) / adjacent ) * 2.0;
		}

		static void updateViewportBoundaries( Viewport const & srcViewport, Viewport & dstViewport
			, const Size & sourceSize, const Point2f & targetSize )
		{
			if ( targetSize->x != 0 && targetSize->y != 0 )
			{
				switch ( srcViewport.getType() )
				{
				case ViewportType::ePerspective:
					{
						auto fovY = updateAngle( srcViewport.getFovY(), sourceSize->x, targetSize->x );
						dstViewport.setPerspective( fovY, srcViewport.getRatio(), srcViewport.getNear(), srcViewport.getFar() );
						break;
					}
				case ViewportType::eInfinitePerspective:
					{
						auto fovY = updateAngle( srcViewport.getFovY(), sourceSize->x, targetSize->x );
						dstViewport.setInfinitePerspective( fovY, srcViewport.getRatio(), srcViewport.getNear() );
						break;
					}
				case ViewportType::eFrustum:
					{
						dstViewport.setFrustum( f32( targetSize->x * ( srcViewport.getLeft() / f64( sourceSize->x ) ) )
							, f32( targetSize->x * ( srcViewport.getRight() / f64( sourceSize->x ) ) )
							, f32( targetSize->y * ( srcViewport.getBottom() / f64( sourceSize->y ) ) )
							, f32( targetSize->y * ( srcViewport.getTop() / f64( sourceSize->y ) ) )
							, srcViewport.getNear()
							, srcViewport.getFar() );
						break;
					}
				case ViewportType::eOrtho:
					{
						dstViewport.setOrtho( f32( targetSize->x * ( srcViewport.getLeft() / f64( sourceSize->x ) ) )
							, f32( targetSize->x * ( srcViewport.getRight() / f64( sourceSize->x ) ) )
							, f32( targetSize->y * ( srcViewport.getBottom() / f64( sourceSize->y ) ) )
							, f32( targetSize->y * ( srcViewport.getTop() / f64( sourceSize->y ) ) )
							, srcViewport.getNear()
							, srcViewport.getFar() );
						break;
					}
				default:
					CU_Failure( "updateViewportBoundaries: Unsupported ViewportType" );
					break;
				}
			}
			dstViewport.update();
		}
	}

	//*********************************************************************************************

	FrustumFroxels::FrustumFroxels( RenderDevice const & device
		, crg::ResourcesCache & resources
		, FrustumClusters const & clusters
		, FroxelsConfig const & config )
		: m_device{ device }
		, m_clusters{ clusters }
		, m_config{ config }
		, m_viewport{ getEngine( device ) }
		, m_froxelSize{ m_froxelsDirty, Point2f{} }
		, m_froxelsUbo{ m_device }
		, m_counterBuffer{ m_device, resources
			, cuT( "C3D_FroxelsCounter" )
			, BufferCreateFlags::eNone
			, 2u
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_froxelsIndirectBuffer{ m_device, resources
			, cuT( "C3D_FroxelsIndirect" )
			, BufferCreateFlags::eNone
			, 2u
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eIndirectBuffer }
		, m_froxelsPointIndirectBuffer{ resources.getHandler().createViewId( crg::BufferViewData{ m_froxelsIndirectBuffer.bufferId.data->name + "Point"
			, m_froxelsIndirectBuffer.bufferId
			, { 0u, sizeof( VkDispatchIndirectCommand ) } } ) }
		, m_froxelsSpotIndirectBuffer{ resources.getHandler().createViewId( crg::BufferViewData{ m_froxelsIndirectBuffer.bufferId.data->name + "Spot"
			, m_froxelsIndirectBuffer.bufferId
			, { sizeof( VkDispatchIndirectCommand ), sizeof( VkDispatchIndirectCommand ) } } ) }
		, m_froxelsPointLightsBuffer{ device, resources
			, cuT( "C3D_FroxelsPointLights" )
			, BufferCreateFlags::eNone
			, frsfxl::MaxFroxelCount * frsfxl::MaxLightsPerSlice
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_froxelsSpotLightsBuffer{ device, resources
			, cuT( "C3D_FroxelsSpotLights" )
			, BufferCreateFlags::eNone
			, frsfxl::MaxFroxelCount * frsfxl::MaxLightsPerSlice
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_rawFroxelsLightingU32R{ device, resources
			, cuT( "C3D_RawFroxelsLightingU32R")
			, { ImageCreateFlags::eNone
				, frsfxl::MaxFroxelExtent, 1u, 1u
				, PixelFormat::eR32_UINT
				, ImageUsageFlags::eTransferDst | ImageUsageFlags::eStorage }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_rawFroxelsLightingU32G{ device, resources
			, cuT( "C3D_RawFroxelsLightingU32G")
			, { ImageCreateFlags::eNone
				, frsfxl::MaxFroxelExtent, 1u, 1u
				, PixelFormat::eR32_UINT
				, ImageUsageFlags::eTransferDst | ImageUsageFlags::eStorage }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_rawFroxelsLightingU32B{ device, resources
			, cuT( "C3D_RawFroxelsLightingU32B")
			, { ImageCreateFlags::eNone
				, frsfxl::MaxFroxelExtent, 1u, 1u
				, PixelFormat::eR32_UINT
				, ImageUsageFlags::eTransferDst | ImageUsageFlags::eStorage }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_rawFroxelsLighting{ device, resources
			, cuT( "C3D_RawFroxelsLighting")
			, { ImageCreateFlags::eNone
				, frsfxl::MaxFroxelExtent, 1u, 1u
				, PixelFormat::eB10G11R11_UFLOAT
				, ImageUsageFlags::eStorage }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_finalFroxelsLighting{ device, resources
			, cuT( "C3D_FroxelsLighting")
			, { ImageCreateFlags::eNone
				, frsfxl::MaxFroxelExtent, 1u, 1u
				, PixelFormat::eB10G11R11_UFLOAT
				, ImageUsageFlags::eStorage }
			, { BorderColour::eFloatOpaqueBlack } }
	{
		m_depthSegments.resize( MaxFroxelGridDepth + ( ( ( MaxFroxelGridDepth % 4u ) == 0u ) ? 4u : 0u ) );
		m_kernelWeights.resize( 64u );
		m_counterBuffer.create();
		m_froxelsIndirectBuffer.create();
		m_froxelsPointLightsBuffer.create();
		m_froxelsSpotLightsBuffer.create();
		doUpdate( {} );
	}

	FrustumFroxels::~FrustumFroxels()noexcept
	{
		m_counterBuffer.destroy();
		m_froxelsIndirectBuffer.destroy();
		m_froxelsPointLightsBuffer.destroy();
		m_froxelsSpotLightsBuffer.destroy();
	}

	void FrustumFroxels::update( CpuUpdater & updater )
	{
		auto const & camera = m_clusters.getCamera();
		auto scene = camera.getScene();
		auto const & lightCache = scene->getLightCache();
		m_froxelsDirty = lightCache.hasClusteredLights()
			&& m_first > 0;
		doUpdate( updater.renderSize );
		updater.viewport = &m_viewport;
		updater.finalRenderSize = m_finalRenderSize;
		doUpdateBlurKernel();
		m_froxelsUbo.cpuUpdate( m_finalDimensions
			, m_froxelSize.value()
			, m_finalRenderSize
			, camera.getNear()
			, camera.getFar()
			, m_projection
			, m_projectionInverse
			, { m_config.sampleCountX, m_config.sampleCountY, m_config.sampleCountMinZ, m_config.sampleCountMaxZ }
			, m_config.blurFilterSize.value().value()
			, m_depthSegments
			, m_kernelWeights);
		auto it = updater.dirtyScenes.find( scene );
		m_lightsDirty = lightCache.hasClusteredLights()
			&& ( m_froxelsDirty
				|| lightCache.isDirty()
				|| ( it != updater.dirtyScenes.end() && !it->second.isEmpty() )
				|| m_config.dirty );
		m_first = camera.getEngine()->areUpdateOptimisationsEnabled()
			? std::max( 0, m_first - 1 )
			: 5;
	}

	void FrustumFroxels::updateDebug( DebugDrawer & drawer )const
	{
	}

	void FrustumFroxels::createFramePasses( crg::FramePassGroup & parentGraph )
	{
		auto & graph = parentGraph.createPassGroup( "Volumetric" );
		createAssignLightsToFroxelsPass( graph, m_device, *this, m_clusters
			, m_froxelsPointLightsBuffer, m_froxelsSpotLightsBuffer, m_counterBuffer, m_froxelsIndirectBuffer );
		createIntegrateFroxelsPass( graph, m_device, *this
			, m_rawFroxelsLightingU32R, m_rawFroxelsLightingU32G, m_rawFroxelsLightingU32B );
		createFinaliseFroxelsLightingPass( graph, m_device, *this
			, m_rawFroxelsLightingU32R, m_rawFroxelsLightingU32G, m_rawFroxelsLightingU32B
			, m_rawFroxelsLighting, m_finalFroxelsLighting );
	}

	void FrustumFroxels::createDebugDisplayPrograms( CameraUbo const & cameraUbo
		, RenderUbo const & renderUbo )
	{
		if ( m_displayFroxelsAABBProgram.empty() )
			createDisplayFroxelsAABBProgram( m_device, *this, cameraUbo, m_clusters.getCameraUbo(), renderUbo
				, m_displayFroxelsAABBProgram, m_displayFroxelsAABBBindings, m_displayFroxelsAABBWrites );
	}

	Scene const & FrustumFroxels::getScene()const noexcept
	{
		return m_clusters.getScene();
	}

	CameraUbo const & FrustumFroxels::getCameraUbo()const noexcept
	{
		return m_clusters.getCameraUbo();
	}

	ClustersUbo const & FrustumFroxels::getClustersUbo()const noexcept
	{
		return m_clusters.getClustersUbo();
	}

	Camera const & FrustumFroxels::getCamera()const noexcept
	{
		return m_clusters.getCamera();
	}

	void FrustumFroxels::doUpdate( Size const & renderSize )
	{
		m_rawRenderSize = renderSize;
		frsfxl::computeDimension( m_config
			, m_rawRenderSize->x, m_rawRenderSize->y
			, m_finalDimensions, m_rawDimensions );

		m_froxelSize = { float( double( m_rawRenderSize->x ) / double( m_rawDimensions->x ) )
			, float( double( m_rawRenderSize->y ) / double( m_rawDimensions->y ) ) };
		m_finalRenderSize->x = float( double( m_finalDimensions->x ) * m_froxelSize.value()->x );
		m_finalRenderSize->y = float( double( m_finalDimensions->y ) * m_froxelSize.value()->y );
		m_renderOffset->x = float( ( m_finalRenderSize->x - double( m_rawRenderSize->x ) ) / 2.0 );
		m_renderOffset->y = float( ( m_finalRenderSize->y - double( m_rawRenderSize->y ) ) / 2.0 );

		if ( m_config.blurFilterSize.value() > 0 )
		{
			frsfxl::updateViewportBoundaries( m_clusters.getCamera().getViewport(), m_viewport, m_rawRenderSize, m_finalRenderSize );
			m_projection = m_viewport.getProjection();
		}
		else
		{
			m_projection = m_viewport.getSafeBandedProjection( m_rawRenderSize );
		}

		m_projectionInverse = m_projection.getInverse();

		f64 powerBase{ 4.0f };
		f64 powerRatio{ 1.0f };
		if ( powerBase != m_powerBase || powerRatio != m_powerRatio || m_finalDimensions->z != m_gridDepth )
		{
			m_powerBase = powerBase;
			m_powerRatio = powerRatio;
			m_gridDepth = m_finalDimensions->z;
			f64 dimRatio = f64( m_gridDepth ) / 4.0;
			u32 count = std::min( MaxFroxelGridDepth, m_gridDepth );

			// Compute total segments size
			f64 segmentsTotal = 0;
			for ( u32 i = 0u; i < count; ++i )
				segmentsTotal += pow( m_powerBase, m_powerRatio * f64( i ) / dimRatio );

			// Compute each segment ratio to total
			f64 segmentsCurrent = 0;
			m_depthSegments[0] = 0.0f;
			for ( u32 i = 0u; i < count; ++i )
			{
				segmentsCurrent += pow( m_powerBase, m_powerRatio * f64( i ) / dimRatio );
				m_depthSegments[i + 1u] = f32( segmentsCurrent / segmentsTotal );
			}

			// Fill with ones
			for ( u32 i = count + 1u; i < m_depthSegments.size(); ++i )
				m_depthSegments[i] = 1.0f;
		}
	}

	void FrustumFroxels::doUpdateBlurKernel()
	{
		if ( m_blurFilterHalfSize == m_config.blurFilterSize.value().value() )
			return;

		m_blurFilterHalfSize = m_config.blurFilterSize.value().value();
		u32 halfHeight = m_blurFilterHalfSize;
		u32 height = 2 * halfHeight;

		// Gaussian filter through a Pascal triangle
		f32 doubleFactor = 0.0f;
		f32 singleFactor = 1.0f;
		u32 index = halfHeight;
		m_kernelWeights[index] = 1.0f;
		f32 prev = 1.0f;

		for ( u32 i = 1; i < halfHeight + 1; i++ )
		{
			--index;
			f32 curr = prev * f32( ( height - i + 1 ) ) / f32( i );
			m_kernelWeights[index] = curr;
			prev = curr;
			if ( i == halfHeight )
				singleFactor += curr;
			else
				doubleFactor += curr;
		}

		f32 sum = doubleFactor * 2.0f + singleFactor;
		// Normalize kernel coefficients
		for ( u32 i = 0u; i <= height; ++i )
			m_kernelWeights[i] /= sum;
	}

	//*********************************************************************************************
}
