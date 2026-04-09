#include "Castor3D/Render/Volumetric/VolumetricRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Texture.hpp"
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
		, m_components{ getEngine( m_device ).getVolumeComponentsRegister() }
		, m_frustumFroxels{ m_components.hasAnyEnabled()
			? c3d::makeUnique< FrustumFroxels >( device, parent.getResources(), *parent.getFrustumClusters(), parent.getFroxelsConfig() )
			: nullptr }
		, m_transmittance{ m_components.hasAnyEnabled()
			? c3d::makeUnique< Texture >( m_device
				, parent.getResources()
				, cuT( "Transmittance" )
				, TextureCreateInfo{ ImageCreateFlags::eNone
					, colour.getExtent(), 1u, 1u
					, m_device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( volrnd::transmittanceUsageFlags ) )
					, volrnd::transmittanceUsageFlags }
				, TextureSamplerInfo{ BorderColour::eFloatOpaqueBlack } )
			: nullptr }
		, m_scattering{ m_components.hasAnyEnabled()
			? c3d::makeUnique< Texture >( m_device
				, parent.getResources()
				, cuT( "Scattering" )
				, TextureCreateInfo{ ImageCreateFlags::eNone
					, colour.getExtent(), 1u, 1u
					, m_device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( volrnd::inscatterUsageFlags ) )
					, volrnd::inscatterUsageFlags }
				, TextureSamplerInfo{ BorderColour::eFloatOpaqueBlack } )
			: nullptr }
	{
		if ( m_components.hasAnyEnabled() )
		{
			m_components.registerCamera( parent.getCamera(), &depthObj );
			m_components.registerScenePasses( parent.getResources(), m_graph, parent.getScene() );
			m_components.registerCameraPasses( parent.getResources(), m_graph, parent.getCamera() );
			m_frustumFroxels->createFramePasses( m_graph );
			m_frustumFroxels->createDebugDisplayPrograms( parent.getCameraUbo(), parent.getRenderUbo() );
			createVolumesTraversalPass( m_graph, m_device, parent.getCamera(), *this, parent.getCameraUbo(), *m_transmittance, *m_scattering );
		}
	}

	void VolumetricRendering::update( CpuUpdater & updater )
	{
		if ( !m_components.hasAnyEnabled() )
			return;

		m_frustumFroxels->update( updater );

		if ( updater.debugDrawer )
			m_frustumFroxels->updateDebug( *updater.debugDrawer );
	}

	void VolumetricRendering::update( GpuUpdater & updater )
	{
		if ( !m_components.hasAnyEnabled() )
			return;
	}

	void VolumetricRendering::upload( UploadData & uploader )
	{
		if ( !m_components.hasAnyEnabled() )
			return;
	}

	void VolumetricRendering::accept( RenderTechniqueVisitor & visitor )
	{
		if ( !m_components.hasAnyEnabled() )
			return;
	}
}
