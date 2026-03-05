#include "Castor3D/Render/Volumetric/VolumetricRendering.hpp"

#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Texture.hpp"

namespace c3d
{
	namespace volrnd
	{
		static ImageUsageFlags constexpr transmittanceUsageFlags = ( ImageUsageFlags::eTransferDst
			| ImageUsageFlags::eStorage
			| ImageUsageFlags::eSampled );
		static ImageUsageFlags constexpr inscatterUsageFlags = ( ImageUsageFlags::eTransferDst
			| ImageUsageFlags::eStorage
			| ImageUsageFlags::eSampled );
	}

	VolumetricRendering::~VolumetricRendering()noexcept = default;

	VolumetricRendering::VolumetricRendering( RenderTechnique & parent
		, RenderDevice const & device
		, Texture const & colour
		, ProgressBar * progress )
		: OwnedBy< RenderTechnique >{ parent }
		, m_device{ device }
		, m_graph{ getOwner()->getGraph().createPassGroup( "Volumetric" ) }
		, m_colour{ colour }
		, m_frustumFroxels{ device, parent.getResources(), *parent.getFrustumClusters(), parent.getFroxelsConfig() }
		, m_transmittance{ c3d::makeUnique< Texture >( m_device
			, parent.getResources()
			, cuT( "Transmittance" )
			, TextureCreateInfo{ ImageCreateFlags::eNone
				, colour.getExtent(), 1u, 1u
				, m_device.selectSuitableDepthStencilFormat( getFeatureFlags( volrnd::transmittanceUsageFlags ) )
				, volrnd::transmittanceUsageFlags }
			, TextureSamplerInfo{ BorderColour::eFloatOpaqueBlack } ) }
		, m_inscatter{ c3d::makeUnique< Texture >( m_device
			, parent.getResources()
			, cuT( "Transmittance" )
			, TextureCreateInfo{ ImageCreateFlags::eNone
				, colour.getExtent(), 1u, 1u
				, m_device.selectSuitableDepthStencilFormat( getFeatureFlags( volrnd::inscatterUsageFlags ) )
				, volrnd::inscatterUsageFlags }
			, TextureSamplerInfo{ BorderColour::eFloatOpaqueBlack } ) }
	{
		m_frustumFroxels.createFramePasses( m_graph );
		m_frustumFroxels.createDebugDisplayPrograms( parent.getCameraUbo(), parent.getRenderUbo() );
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
