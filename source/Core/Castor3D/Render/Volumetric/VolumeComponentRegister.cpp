
#include "Castor3D/Render/Volumetric/VolumeComponentRegister.hpp"
#include "Castor3D/Render/Volumetric/VolumeComponentPlugin.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Camera.hpp"

CU_ImplementSmartPtr( c3d, VolumeComponentRegister )

namespace c3d
{
	namespace volcompreg
	{
		static constexpr uint32_t InvalidId = uint32_t( ~uint32_t{} );
	}

	VolumeComponentRegister::VolumeComponentRegister( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	VolumeComponentRegister::~VolumeComponentRegister()noexcept
	{
		while ( !m_registered.empty() )
		{
			if ( auto const & componentDesc = m_registered.back();
				componentDesc.plugin )
			{
				unregisterComponent( componentDesc.id );
			}

			m_registered.pop_back();
		}
	}

	void VolumeComponentRegister::registerComponent( String const & componentType
		, VolumeComponentPluginUPtr componentPlugin )
	{
		if ( auto id = getNameId( componentType );
			id != volcompreg::InvalidId )
		{
			log::error << "Volume component type [" << componentType << "] is already registered." << std::endl;
			CU_Failure( "Volume component type is already registered" );
			return;
		}

		auto & componentDesc = getNextId();
		registerComponent( componentDesc
			, componentType
			, c3d::move( componentPlugin ) );
	}

	void VolumeComponentRegister::unregisterComponent( String const & componentType )
	{
		auto id = getNameId( componentType );

		if ( id == volcompreg::InvalidId )
		{
			log::error << "Volume component type [" << componentType << "] was not found." << std::endl;
			CU_Failure( "Volume component type was not found." );
			return;
		}

		unregisterComponent( id );
	}

	uint32_t VolumeComponentRegister::getNameId( String const & componentType )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&componentType]( Components::value_type const & lookup )
			{
				return lookup.name == componentType;
			} );
		return ( it == m_registered.end() )
			? volcompreg::InvalidId
			: it->id;
	}

	VolumeComponentPlugin & VolumeComponentRegister::getPlugin( String const & componentType )const
	{
		return getPlugin( getNameId( componentType ) );
	}

	VolumeComponentPlugin & VolumeComponentRegister::getPlugin( uint32_t componentId )const
	{
		if ( componentId > m_registered.size()
			|| componentId == 0u
			|| !m_registered[componentId - 1u].plugin )
		{
			CU_Failure( "Volume component ID was not found." );
			CU_Exception( "Volume component ID was not found." );
		}

		return *m_registered[componentId - 1u].plugin;
	}

	VolumeComponentRegister::Component & VolumeComponentRegister::getNextId()
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin == nullptr;
			} );

		if ( it == m_registered.end() )
		{
			auto id = uint32_t( m_registered.size() + 1u );
			m_registered.emplace_back( id );
			it = std::next( m_registered.begin()
				, ptrdiff_t( id - 1u ) );
		}

		return *it;
	}

	void VolumeComponentRegister::registerComponent( Component & componentDesc
		, String const & componentType
		, VolumeComponentPluginUPtr componentPlugin )
	{
		componentPlugin->setId( componentDesc.id );
		componentDesc.name = componentType;
		componentDesc.plugin = c3d::move( componentPlugin );

		log::debug << "Registered volume component ID " << componentDesc.id << " for [" << componentType << "]" << std::endl;
	}

	void VolumeComponentRegister::unregisterComponent( uint32_t id )
	{
		if ( id > 0 && id <= m_registered.size() )
		{
			auto & componentDesc = m_registered[id - 1u];
			log::debug << "Unregistered volume component " << id << " (" << componentDesc.name << ")" << std::endl;
			componentDesc.name.clear();
			componentDesc.plugin.reset();
			componentDesc.id = volcompreg::InvalidId;
		}
	}

	void VolumeComponentRegister::registerCamera( Camera const & camera
		, c3d::Texture const * depthObj )const
	{
		for ( auto & component : m_registered )
			if ( component.plugin->isEnabled() )
				component.plugin->registerCamera( camera, depthObj );
	}

	bool VolumeComponentRegister::hasAnyEnabled()const noexcept
	{
		return std::any_of( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin && lookup.plugin->isEnabled();
			} );
	}

	Vector< shader::VolumeComponentShaderPtr > VolumeComponentRegister::createShaders( sdw::ShaderWriter & writer
		, shader::VolumeShaders const & volumeShaders
		, c3d::Extent2D const & targetExtent
		, bool hasDepth
		, uint32_t enabledPlugins
		, uint32_t & bindingId )const
	{
		Vector< shader::VolumeComponentShaderPtr > result;
		for ( auto & component : m_registered )
			if ( component.plugin->isEnabled() && checkFlag( enabledPlugins, 0x00000001u << component.plugin->getId() ) )
				result.emplace_back( component.plugin->createComponentsShader( writer, volumeShaders, targetExtent, hasDepth, bindingId ) );
		return result;
	}

	void VolumeComponentRegister::registerPasses( crg::ResourcesCache & resources
		, crg::FramePassGroup & graph
		, c3d::Camera const & camera )const
	{
		for ( auto & component : m_registered )
		{
			if ( component.plugin->isEnabled() )
			{
				component.plugin->registerScenePasses( resources, graph, *camera.getScene() );
				component.plugin->registerCameraPasses( resources, graph, camera );
			}
		}
	}

	void VolumeComponentRegister::registerBindings( crg::FramePass & pass
		, uint32_t enabledPlugins
		, Camera const & camera
		, uint32_t & bindingId )const
	{
		for ( auto & component : m_registered )
			if ( component.plugin->isEnabled() && checkFlag( enabledPlugins, 0x00000001u << component.plugin->getId() ) )
				component.plugin->registerBindings( pass, camera, bindingId );
	}
}
