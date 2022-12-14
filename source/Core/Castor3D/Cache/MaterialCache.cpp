#include "Castor3D/Cache/MaterialCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

CU_ImplementCUSmartPtr( castor3d, MaterialCache )

namespace castor3d
{
	const castor::String ResourceCacheTraitsT< castor3d::Material, castor::String >::Name = cuT( "Material" );
}

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::initialise( RenderDevice const & device )
	{
		for ( auto & buffer : m_buffers )
		{
			buffer.second.second = buffer.second.first.create( device );
		}
	}
	
	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::cleanup()
	{
		for ( auto & buffer : m_buffers )
		{
			buffer.second.second.reset();
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::update( castor3d::PassBuffer & passBuffer
		, ashes::CommandBuffer const & cb )const
	{
		passBuffer.update( m_buffers, cb );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::registerBuffer( std::string const & name
		, SpecificsBuffer buffer )
	{
		auto it = m_buffers.find( name );

		if ( it != m_buffers.end() )
		{
			CU_Exception( "Buffer with given name already registered." );
		}

		m_buffers.emplace( name
			, std::make_pair( std::move( buffer ), nullptr ) );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::unregisterBuffer( std::string const & name )
	{
		auto it = m_buffers.find( name );

		if ( it != m_buffers.end() )
		{
			m_buffers.erase( it );
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::addBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		for ( auto & buffer : m_buffers )
		{
			bindings.push_back( buffer.second.second->createLayoutBinding( index, shaderStages ) );
			++index;
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::addDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		for ( auto & buffer : m_buffers )
		{
			descriptorWrites.push_back( buffer.second.second->getBinding( index ) );
			++index;
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::createPassBindings( crg::FramePass & pass
		, uint32_t & index )const
	{
		for ( auto & buffer : m_buffers )
		{
			buffer.second.second->createPassBinding( pass, buffer.first, index );
			++index;
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::PassDataBuffers::declareShaderBuffers( sdw::ShaderWriter & writer
		, std::map< std::string, shader::BufferBaseUPtr > & buffers
		, uint32_t & binding
		, uint32_t set )const
	{
		for ( auto & buffer : m_buffers )
		{
			buffers.emplace( buffer.first
				, buffer.second.first.declare( writer, binding, set ) );
			++binding;
		}
	}

	//*********************************************************************************************

	ResourceCacheT< Material, String, MaterialCacheTraits >::ResourceCacheT( Engine & engine )
		: ElementCacheT{ engine.getLogger()
			, [this]( ElementT & resource )
			{
				m_engine.postEvent( makeCpuInitialiseEvent( resource ) );
			}
			, [this]( ElementT & resource )
			{
				m_engine.postEvent( makeCpuCleanupEvent( resource ) );
			}
			, castor::ResourceMergerT< MaterialCache >{ "_" } }
		, m_engine{ engine }
	{
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::initialise( RenderDevice const & device
		, PassTypeID passType )
	{
		if ( !m_passBuffer )
		{
			auto lock( makeUniqueLock( *this ) );
			MaterialResPtr created;
			auto defaultMaterial = doTryAddNoLockT( Material::DefaultMaterialName
				, false
				, created
				, m_engine, passType );
			auto material = defaultMaterial.lock();

			if ( created.lock() == material )
			{
				material->createPass();
				material->getPass( 0 )->createComponent< castor3d::TwoSidedComponent >()->setTwoSided( true );
			}

			m_defaultMaterial = material.get();
			m_passBuffer = std::make_shared< PassBuffer >( m_engine
				, device
				, MaxMaterialsCount );
			m_sssProfileBuffer = std::make_shared< SssProfileBuffer >( m_engine
				, device
				, MaxSssProfilesCount );
			m_texConfigBuffer = std::make_shared< TextureConfigurationBuffer >( m_engine
				, device
				, ( device.hasBindless()
					? device.getMaxBindlessSampled()
					: MaxTextureConfigurationCount ) );
			m_texAnimBuffer = std::make_shared< TextureAnimationBuffer >( m_engine
				, device
				, ( device.hasBindless()
					? device.getMaxBindlessSampled()
					: MaxTextureAnimationCount ) );

			for ( auto & it : *this )
			{
				if ( !it.second->isInitialised() )
				{
					it.second->initialise();
				}
			}

			for ( auto pass : m_pendingPasses )
			{
				registerPass( *pass );
			}

			for ( auto unit : m_pendingUnits )
			{
				registerUnit( *unit );
			}

			for ( auto texture : m_pendingTextures )
			{
				registerTexture( *texture );
			}

			m_pendingPasses.clear();
			m_pendingUnits.clear();
			m_pendingTextures.clear();
			m_specificsBuffers.initialise( device );
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::cleanup()
	{
		{
			auto lock( makeUniqueLock( *this ) );
			doCleanupNoLock();

			if ( m_passBuffer )
			{
				m_passBuffer->cleanup();
			}
		}
		m_engine.postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this]()
			{
				auto lock( makeUniqueLock( *this ) );
				m_pendingPasses.clear();
				m_pendingUnits.clear();
				m_pendingTextures.clear();
				m_specificsBuffers.cleanup();
				m_passBuffer.reset();
				m_sssProfileBuffer.reset();
				m_texConfigBuffer.reset();
				m_texAnimBuffer.reset();
			} ) );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::clear()
	{
		auto lock( makeUniqueLock( *this ) );
		m_defaultMaterial = {};
		doClearNoLock();
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( makeUniqueLock( *this ) );

		for ( auto & it : *this )
		{
			for ( auto & pass : *it.second )
			{
				pass->update();
			}
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::update( GpuUpdater & updater )
	{
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::upload( ashes::CommandBuffer const & cb )const
	{
		if ( m_passBuffer )
		{
			m_specificsBuffers.update( *m_passBuffer, cb );
			m_sssProfileBuffer->update( cb );
			m_texConfigBuffer->update( cb );
			m_texAnimBuffer->update( cb );
		}
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::registerSpecificsBuffer( std::string const & name
		, SpecificsBuffer buffer )
	{
		m_specificsBuffers.registerBuffer( name, buffer );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::unregisterSpecificsBuffer( std::string const & name )
	{
		m_specificsBuffers.unregisterBuffer( name );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::addSpecificsBuffersBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		m_specificsBuffers.addBindings( bindings, shaderStages, index );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::addSpecificsBuffersDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		m_specificsBuffers.addDescriptors( descriptorWrites, index );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::createSpecificsBuffersPassBindings( crg::FramePass & pass
		, uint32_t & index )const
	{
		m_specificsBuffers.createPassBindings( pass, index );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::declareSpecificsShaderBuffers( sdw::ShaderWriter & writer
		, std::map< std::string, shader::BufferBaseUPtr > & buffers
		, uint32_t & binding
		, uint32_t set )const
	{
		m_specificsBuffers.declareShaderBuffers( writer, buffers, binding, set );
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::getNames( StringArray & names )
	{
		auto lock( makeUniqueLock( *this ) );
		names.clear();
		auto it = begin();

		while ( it != end() )
		{
			names.push_back( it->first );
			it++;
		}
	}

	uint32_t ResourceCacheT< Material, String, MaterialCacheTraits >::getMaxPassTypeCount()const
	{
		return m_passBuffer->getMaxPassTypeCount();
	}

	std::tuple< PassComponentCombineID, TextureCombineID > ResourceCacheT< Material, String, MaterialCacheTraits >::getPassTypeDetails( uint32_t passTypeIndex )const
	{
		return m_passBuffer->getPassTypeDetails( passTypeIndex );
	}

	uint32_t ResourceCacheT< Material, String, MaterialCacheTraits >::getPassTypeIndex( castor3d::PassComponentCombineID passType
		, castor3d::TextureCombineID textureFlags )const
	{
		return m_passBuffer->getPassTypeIndex( passType, textureFlags );
	}

	bool ResourceCacheT< Material, String, MaterialCacheTraits >::registerPass( Pass & pass )
	{
		if ( m_passBuffer )
		{
			m_passBuffer->addPass( pass );

			if ( auto sss = pass.getComponent< SubsurfaceScatteringComponent >() )
			{
				m_sssProfileBuffer->addPass( *sss );
			}

			return true;
		}

		m_pendingPasses.push_back( &pass );
		return false;
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::unregisterPass( Pass & pass )
	{
		if ( m_passBuffer
			&& pass.getId() )
		{
			if ( auto sss = pass.getComponent< SubsurfaceScatteringComponent >() )
			{
				m_sssProfileBuffer->removePass( *sss );
			}

			m_passBuffer->removePass( pass );
		}

		auto it = std::find( m_pendingPasses.begin()
			, m_pendingPasses.end()
			, &pass );

		if ( it != m_pendingPasses.end() )
		{
			m_pendingPasses.erase( it );
		}
	}

	bool ResourceCacheT< Material, String, MaterialCacheTraits >::registerUnit( TextureUnit & unit )
	{
		if ( m_texConfigBuffer
			&& unit.getId() == 0u )
		{
			m_texConfigBuffer->addTextureConfiguration( unit );

			return true;
		}

		m_pendingUnits.push_back( &unit );
		return false;
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::unregisterUnit( TextureUnit & unit )
	{
		if ( m_texConfigBuffer
			&& unit.getId() )
		{
			m_texConfigBuffer->removeTextureConfiguration( unit );
		}

		auto it = std::find( m_pendingUnits.begin()
			, m_pendingUnits.end()
			, &unit );

		if ( it != m_pendingUnits.end() )
		{
			m_pendingUnits.erase( it );
		}
	}

	bool ResourceCacheT< Material, String, MaterialCacheTraits >::registerTexture( AnimatedTexture const & texture )
	{
		if ( m_texAnimBuffer )
		{
			if ( texture.getTexture().getId() == 0u )
			{
				registerUnit( texture.getTexture() );
			}

			m_texAnimBuffer->addTextureAnimation( texture );
			return true;
		}

		m_pendingTextures.push_back( &texture );
		return false;
	}

	void ResourceCacheT< Material, String, MaterialCacheTraits >::unregisterTexture( AnimatedTexture const & texture )
	{
		if ( m_texAnimBuffer )
		{
			m_texAnimBuffer->removeTextureAnimation( texture );
		}

		auto it = std::find( m_pendingTextures.begin()
			, m_pendingTextures.end()
			, &texture );

		if ( it != m_pendingTextures.end() )
		{
			m_pendingTextures.erase( it );
		}
	}

	uint32_t ResourceCacheT< Material, String, MaterialCacheTraits >::getCurrentPassTypeCount()const
	{
		return m_passBuffer->getCurrentPassTypeCount();
	}

	//*********************************************************************************************
}
