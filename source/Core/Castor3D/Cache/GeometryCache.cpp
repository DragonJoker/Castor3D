#include "Castor3D/Cache/GeometryCache.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

using namespace castor;

namespace castor3d
{
	template<> const String ObjectCacheTraits< Geometry, String >::Name = cuT( "Geometry" );

	namespace
	{
		struct GeometryInitialiser
		{
			GeometryInitialiser( uint32_t & faceCount
				, uint32_t & vertexCount
				, FrameListener & listener )
				: m_faceCount{ faceCount }
				, m_vertexCount{ vertexCount }
				, m_listener{ listener }
			{
			}

			inline void operator()( GeometrySPtr element )
			{
				m_listener.postEvent( makeFunctorEvent( EventType::ePreRender
					, [element, this]()
					{
						element->prepare( m_faceCount, m_vertexCount );
					} ) );
			}

			uint32_t & m_faceCount;
			uint32_t & m_vertexCount;
			FrameListener & m_listener;
		};

		size_t hash( Geometry const & geometry
			, Submesh const & submesh
			, Pass const & pass )
		{
			size_t result = std::hash< Geometry const * >{}( &geometry );
			castor::hashCombine( result, submesh );
			castor::hashCombine( result, pass );
			return result;
		}
	}

	GeometryCache::ObjectCache( Engine & engine
		, Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode
		, Producer && produce
		, Initialiser && initialise
		, Cleaner && clean
		, Merger && merge
		, Attacher && attach
		, Detacher && detach )
		: MyObjectCache( engine
			, scene
			, rootNode
			, rootCameraNode
			, rootCameraNode
			, std::move( produce )
			, std::bind( GeometryInitialiser{ m_faceCount, m_vertexCount, scene.getListener() }, std::placeholders::_1 )
			, std::move( clean )
			, std::move( merge )
			, std::move( attach )
			, std::move( detach ) )
		, m_modelUboPool{ *engine.getRenderSystem(), cuT( "GeometryUboPools_Model" ) }
		, m_modelMatrixUboPool{ *engine.getRenderSystem(), cuT( "GeometryUboPools_ModelMatrix" ) }
		, m_pickingUboPool{ *engine.getRenderSystem(), cuT( "GeometryUboPools_Picking" ) }
		, m_texturesUboPool{ *engine.getRenderSystem(), cuT( "GeometryUboPoolsTextures_" ) }
	{
		getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_updateTimer = std::make_shared< RenderPassTimer >( *getEngine()
					, cuT( "Update" )
					, cuT( "Model UBOs" )
					, 3u );
				m_updatePickingTimer = std::make_shared< RenderPassTimer >( *getEngine()
					, cuT( "Update" )
					, cuT( "Picking UBOs" )
					, 1u );
				m_updateTexturesTimer = std::make_shared< RenderPassTimer >( *getEngine()
					, cuT( "Update" )
					, cuT( "Textures UBOs" )
					, 1u );
			} ) );
	}

	GeometryCache::~ObjectCache()
	{
	}

	void GeometryCache::fillInfo( RenderInfo & info )const
	{
		auto lock( castor::makeUniqueLock( m_elements ) );

		for ( auto element : m_elements )
		{
			if ( element.second->getMesh() )
			{
				auto mesh = element.second->getMesh();
				info.m_totalObjectsCount += mesh->getSubmeshCount();
				info.m_totalVertexCount += mesh->getVertexCount();
				info.m_totalFaceCount += mesh->getFaceCount();
			}
		}
	}

	void GeometryCache::update()
	{
		for ( auto & pair : m_entries )
		{
			auto & entry = pair.second;

			if ( entry.geometry.getParent()
				&& bool( entry.modelUbo )
				&& bool( entry.modelMatrixUbo ) )
			{
				auto & modelData = entry.modelUbo.getData();
				modelData.shadowReceiver = entry.geometry.isShadowReceiver();
				modelData.materialIndex = entry.pass.getId();

				if ( entry.pass.hasEnvironmentMapping() )
				{
					auto & envMap = getScene()->getEnvironmentMap( *entry.geometry.getParent() );
					modelData.environmentIndex = envMap.getIndex();
				}

				auto & modelMatrixData = entry.modelMatrixUbo.getData();
				modelMatrixData.prvModel = modelMatrixData.curModel;
				modelMatrixData.curModel = entry.geometry.getParent()->getDerivedTransformationMatrix();
				auto & texturesData = entry.texturesUbo.getData();
				uint32_t index = 0u;

				for ( auto & unit : entry.pass )
				{
					texturesData.indices[index / 4u][index % 4] = unit->getId();
					++index;
				}
			}
		}
	}

	void GeometryCache::uploadUbos( ashes::CommandBuffer const & commandBuffer )const
	{
		auto count = m_modelUboPool.getBufferCount()
			+ m_modelMatrixUboPool.getBufferCount()
			+ m_texturesUboPool.getBufferCount();

		if ( count )
		{
			//m_updateTimer->updateCount( count );
			//auto timerBlock = m_updateTimer->start();
			//uint32_t index = 0u;
			//m_modelUboPool.upload( commandBuffer , *m_updateTimer, index );
			//index += m_modelUboPool.getBufferCount();
			//m_modelMatrixUboPool.upload( commandBuffer, *m_updateTimer, index );
			//index += m_modelMatrixUboPool.getBufferCount();
			//m_texturesUboPool.upload( commandBuffer, *m_updateTimer, index );
			m_modelUboPool.upload( commandBuffer );
			m_modelMatrixUboPool.upload( commandBuffer );
			m_texturesUboPool.upload( commandBuffer );
		}
	}

	void GeometryCache::uploadPickingUbos( ashes::CommandBuffer const & commandBuffer )const
	{
		auto count = m_pickingUboPool.getBufferCount();

		if ( count )
		{
			m_updatePickingTimer->updateCount( count );
			auto timerBlock = m_updatePickingTimer->start();
			m_pickingUboPool.upload( commandBuffer, *m_updatePickingTimer, 0u );
		}
	}

	void GeometryCache::cleanupUbos()
	{
		m_modelUboPool.cleanup();
		m_modelMatrixUboPool.cleanup();
		m_pickingUboPool.cleanup();
		m_texturesUboPool.cleanup();
	}

	GeometryCache::PoolsEntry GeometryCache::getUbos( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass )const
	{
		return m_entries.at( hash( geometry, submesh, pass ) );
	}

	void GeometryCache::clear()
	{
		MyObjectCache::clear();

		for ( auto & entry : m_entries )
		{
			m_modelUboPool.putBuffer( entry.second.modelUbo );
			m_modelMatrixUboPool.putBuffer( entry.second.modelMatrixUbo );
			m_pickingUboPool.putBuffer( entry.second.pickingUbo );
			m_texturesUboPool.putBuffer( entry.second.texturesUbo );
		}

		m_entries.clear();

		getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_updateTimer.reset();
			} ) );
	}

	void GeometryCache::add( ElementPtr element )
	{
		MyObjectCache::add( element->getName(), element );
		getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [this, element]()
			{
				doRegister( *element );
			} ) );
	}

	GeometrySPtr GeometryCache::add( Key const & name
		, SceneNode & parent
		, MeshSPtr mesh )
	{
		CU_Require( mesh );
		auto result = MyObjectCache::add( name, parent, mesh );
		getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [this, result]()
			{
				doRegister( *result );
			} ) );
		return result;
	}

	void GeometryCache::remove( Key const & name )
	{
		auto lock( castor::makeUniqueLock( m_elements ) );

		if ( m_elements.has( name ) )
		{
			auto element = m_elements.find( name );
			m_detach( element );
			m_elements.erase( name );
			onChanged();
			doUnregister( *element );
		}
	}

	GeometryCache::PoolsEntry GeometryCache::doCreateEntry( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		return
		{
			geometry,
			submesh,
			pass,
			m_modelUboPool.getBuffer( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
			m_modelMatrixUboPool.getBuffer( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
			m_pickingUboPool.getBuffer( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
			m_texturesUboPool.getBuffer( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
		};
	}

	void GeometryCache::doRemoveEntry( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass )
	{
		auto entry = getUbos( geometry, submesh, pass );
		m_entries.erase( hash( geometry, submesh, pass ) );
		m_modelUboPool.putBuffer( entry.modelUbo );
		m_modelMatrixUboPool.putBuffer( entry.modelMatrixUbo );
		m_pickingUboPool.putBuffer( entry.pickingUbo );
		m_texturesUboPool.putBuffer( entry.texturesUbo );
	}

	void GeometryCache::doRegister( Geometry & geometry )
	{
		m_connections.emplace( &geometry, geometry.onMaterialChanged.connect( [this]( Geometry const & geometry
			, Submesh const & submesh
			, MaterialSPtr oldMaterial
			, MaterialSPtr newMaterial )
		{
			if ( oldMaterial )
			{
				for ( auto & pass : *oldMaterial )
				{
					doRemoveEntry( geometry, submesh, *pass );
				}
			}

			if ( newMaterial )
			{
				for ( auto & pass : *newMaterial )
				{
					m_entries.emplace( hash( geometry, submesh, *pass )
						, doCreateEntry( geometry, submesh, *pass ) );
				}
			}
		} ) );

		if ( geometry.getMesh() )
		{
			for ( auto & submesh : *geometry.getMesh() )
			{
				auto material = geometry.getMaterial( *submesh );

				if ( material )
				{
					for ( auto & pass : *material )
					{
						m_entries.emplace( hash( geometry, *submesh, *pass )
							, doCreateEntry( geometry, *submesh, *pass ) );
					}
				}
			}
		}
	}

	void GeometryCache::doUnregister( Geometry & geometry )
	{
		m_connections.erase( &geometry );

		if ( geometry.getMesh() )
		{
			for ( auto & submesh : *geometry.getMesh() )
			{
				for ( auto & pass : *geometry.getMaterial( *submesh ) )
				{
					doRemoveEntry( geometry, *submesh, *pass );
				}
			}
		}
	}
}
