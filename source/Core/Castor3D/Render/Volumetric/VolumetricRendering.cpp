#include "Castor3D/Render/Volumetric/VolumetricRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Texture.hpp"
#include "Castor3D/Render/Volumetric/DownscaleDepthPass.hpp"
#include "Castor3D/Render/Volumetric/VolumeComponentRegister.hpp"
#include "Castor3D/Render/Volumetric/VolumesTraversal.hpp"

namespace c3d
{
	namespace volrnd
	{
		static ImageUsageFlags constexpr transmittanceUsageFlags = ( ImageUsageFlags::eTransferDst
			| ImageUsageFlags::eStorage
			| ImageUsageFlags::eSampled
			| ImageUsageFlags::eColorAttachment );
		static ImageUsageFlags constexpr inscatterUsageFlags = ( ImageUsageFlags::eTransferDst
			| ImageUsageFlags::eStorage
			| ImageUsageFlags::eSampled
			| ImageUsageFlags::eColorAttachment );
	}

	VolumetricRendering::~VolumetricRendering()noexcept = default;

	VolumetricRendering::VolumetricRendering( RenderTechnique & parent
		, RenderDevice const & device
		, Texture const & colour
		, Texture const & depthObj
		, ProgressBar * progress )
		: OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ getOwner()->getGraph().createPassGroup( "Volumetric" ) }
		, m_colour{ colour }
		, m_frustumFroxels{ device, parent.getResources(), *parent.getFrustumClusters(), parent.getFroxelsConfig() }
		, m_downscaledDepth{ c3d::makeUnique< Texture >( m_device
			, parent.getResources()
			, cuT( "DownscaledDepthMinMax" )
			, TextureCreateInfo{ ImageCreateFlags::eNone
				, Extent3D{ colour.getExtent().width >> 1u, colour.getExtent().height >> 1u, 1u }, 1u, 1u
				, m_device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( volrnd::transmittanceUsageFlags ) )
				, volrnd::transmittanceUsageFlags }
			, TextureSamplerInfo{ BorderColour::eFloatOpaqueBlack } ) }
		, m_transmittance{ c3d::makeUnique< Texture >( m_device
			, parent.getResources()
			, cuT( "Transmittance" )
			, TextureCreateInfo{ ImageCreateFlags::eNone
				, colour.getExtent(), 1u, 1u
				, m_device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( volrnd::transmittanceUsageFlags ) )
				, volrnd::transmittanceUsageFlags }
			, TextureSamplerInfo{ BorderColour::eFloatOpaqueBlack } ) }
		, m_scattering{ c3d::makeUnique< Texture >( m_device
			, parent.getResources()
			, cuT( "Scattering" )
			, TextureCreateInfo{ ImageCreateFlags::eNone
				, colour.getExtent(), 1u, 1u
				, m_device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( volrnd::inscatterUsageFlags ) )
				, volrnd::inscatterUsageFlags }
			, TextureSamplerInfo{ BorderColour::eFloatOpaqueBlack } ) }
	{
		getEngine( m_device ).getVolumeComponentsRegister().registerCamera( parent.getCamera(), &depthObj );
		getEngine( m_device ).getVolumeComponentsRegister().registerScenePasses( parent.getResources(), m_graph, parent.getScene() );
		getEngine( m_device ).getVolumeComponentsRegister().registerCameraPasses( parent.getResources(), m_graph, parent.getCamera() );
		//createDownscaleDepthPass( m_graph, m_device, parent.getScene(), depthObj, *m_downscaledDepth );
		m_frustumFroxels.createFramePasses( m_graph );
		m_frustumFroxels.createDebugDisplayPrograms( parent.getCameraUbo(), parent.getRenderUbo() );
		createVolumesTraversalPass( m_graph, m_device, parent.getCamera(), *this, parent.getCameraUbo(), *m_transmittance, *m_scattering );
	}

	void VolumetricRendering::update( CpuUpdater & updater )
	{
		m_frustumFroxels.update( updater );

		if ( updater.debugDrawer )
			m_frustumFroxels.updateDebug( *updater.debugDrawer );
	}

	void VolumetricRendering::update( GpuUpdater & updater )
	{
	}

	void VolumetricRendering::upload( UploadData & uploader )
	{
	}

	void VolumetricRendering::accept( RenderTechniqueVisitor & visitor )
	{
	}
}
