#include "GuiCommon/System/DebugMeshManager.hpp"

#include <Castor3D/Engine.hpp>
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include <Castor3D/Buffer/GpuBufferPool.hpp>
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include <Castor3D/Buffer/ObjectBufferPool.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Other/ColourComponent.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#include <cstddef>

namespace GuiCommon
{
	namespace dbgmsh
	{
		enum BindingPoints
		{
			eCamera,
			eMeshConfig,
		};

		struct DebugMeshData
			: public sdw::StructInstanceHelperT< "GC_DebugMeshData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Mat4x4Field< "world" >
				, sdw::Vec4Field< "colour" > >
		{
			DebugMeshData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
			{
			}

			auto world()const { return getMember< "world" >(); }
			auto colour()const { return getMember< "colour" >(); }

			sdw::Vec4 modelToWorld( sdw::Vec4 const & msPosition )const
			{
				return world() * msPosition;
			}

		};

		static sdw::ShaderPtr createDisplayCubeProgram( castor3d::RenderDevice const & device )
		{
			namespace c3d = castor3d::shader;

			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_Camera( writer
				, eCamera
				, 0u );
			sdw::StorageBuffer cubeBox{ writer
				, "GC_DebugMesh"
				, "gc_debugMesh"
				, uint32_t( eMeshConfig )
				, uint32_t( 0u )
				, sdw::type::MemoryLayout::eStd430 };
			auto gc_debugMeshData = cubeBox.declMemberArray< DebugMeshData >( "c" );
			cubeBox.end();

			auto positions = writer.declGlobalArray( "positions"
				, 24u
				, std::vector< sdw::Vec4 >{ vec4( -0.5_f, -0.5_f, -0.5_f, 1.0_f ), vec4( +0.5_f, -0.5_f, -0.5_f, 1.0_f )
					, vec4( +0.5_f, -0.5_f, -0.5_f, 1.0_f ), vec4( +0.5_f, +0.5_f, -0.5_f, 1.0_f )
					, vec4( +0.5_f, +0.5_f, -0.5_f, 1.0_f ), vec4( -0.5_f, +0.5_f, -0.5_f, 1.0_f )
					, vec4( -0.5_f, +0.5_f, -0.5_f, 1.0_f ), vec4( -0.5_f, -0.5_f, -0.5_f, 1.0_f )
					, vec4( -0.5_f, -0.5_f, +0.5_f, 1.0_f ), vec4( +0.5_f, -0.5_f, +0.5_f, 1.0_f )
					, vec4( +0.5_f, -0.5_f, +0.5_f, 1.0_f ), vec4( +0.5_f, +0.5_f, +0.5_f, 1.0_f )
					, vec4( +0.5_f, +0.5_f, +0.5_f, 1.0_f ), vec4( -0.5_f, +0.5_f, +0.5_f, 1.0_f )
					, vec4( -0.5_f, +0.5_f, +0.5_f, 1.0_f ), vec4( -0.5_f, -0.5_f, +0.5_f, 1.0_f )
					, vec4( -0.5_f, -0.5_f, -0.5_f, 1.0_f ), vec4( -0.5_f, -0.5_f, +0.5_f, 1.0_f )
					, vec4( +0.5_f, -0.5_f, -0.5_f, 1.0_f ), vec4( +0.5_f, -0.5_f, +0.5_f, 1.0_f )
					, vec4( +0.5_f, +0.5_f, -0.5_f, 1.0_f ), vec4( +0.5_f, +0.5_f, +0.5_f, 1.0_f )
					, vec4( -0.5_f, +0.5_f, -0.5_f, 1.0_f ), vec4( -0.5_f, +0.5_f, +0.5_f, 1.0_f ) } );

			writer.implementEntryPointT< sdw::VoidT, c3d::Colour4FT >( [&writer, &positions, &c3d_cameraData, &gc_debugMeshData]( sdw::VertexIn const & in
				, sdw::VertexOutT< c3d::Colour4FT > out )
				{
					auto index = writer.declLocale( "index"
						, in.vertexIndex );
					auto position = writer.declLocale< sdw::Vec4 >( "position"
						, positions[index] );
					position = gc_debugMeshData[in.instanceIndex].modelToWorld( position );
					out.vtx.position = c3d_cameraData.worldToCurProj( position );
					out.colour() = gc_debugMeshData[in.instanceIndex].colour();
				} );

			writer.implementEntryPointT< c3d::Colour4FT, c3d::Colour4FT >( []( sdw::FragmentInT< c3d::Colour4FT > const & in
				, sdw::FragmentOutT< c3d::Colour4FT > const & out )
				{
					out.colour() = in.colour();
				} );

			return writer.getBuilder().releaseShader();
		}

		static sdw::ShaderPtr createDisplayLinesProgram( castor3d::RenderDevice const & device )
		{
			namespace c3d = castor3d::shader;

			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_Camera( writer
				, eCamera
				, 0u );
			sdw::StorageBuffer cubeBox{ writer
				, "GC_DebugMesh"
				, "gc_debugMesh"
				, uint32_t( eMeshConfig )
				, uint32_t( 0u )
				, sdw::type::MemoryLayout::eStd430 };
			auto gc_debugMeshData = cubeBox.declMemberArray< DebugMeshData >( "c" );
			cubeBox.end();

			writer.implementEntryPointT< c3d::Position4FT, c3d::Colour4FT >( [&writer, &c3d_cameraData, &gc_debugMeshData]( sdw::VertexInT< c3d::Position4FT > const & in
				, sdw::VertexOutT< c3d::Colour4FT > out )
				{
					auto position = writer.declLocale< sdw::Vec4 >( "position"
						, in.position() );
					position = gc_debugMeshData[in.instanceIndex].modelToWorld( position );
					out.vtx.position = c3d_cameraData.worldToCurProj( position );
					out.colour() = gc_debugMeshData[in.instanceIndex].colour();
				} );

			writer.implementEntryPointT< c3d::Colour4FT, c3d::Colour4FT >( []( sdw::FragmentInT< c3d::Colour4FT > const & in
				, sdw::FragmentOutT< c3d::Colour4FT > const & out )
				{
					out.colour() = in.colour();
				} );

			return writer.getBuilder().releaseShader();
		}

		static void createBindings( castor3d::CameraUbo const & cameraUbo
			, castor3d::GpuBufferOffsetT< DebugMeshConfig > const & cubeBoxUbo
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, ashes::WriteDescriptorSetArray & writes )
		{
			bindings.push_back( VkDescriptorSetLayoutBinding{ eCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1u, VK_SHADER_STAGE_VERTEX_BIT, nullptr } );
			bindings.push_back( VkDescriptorSetLayoutBinding{ eMeshConfig, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1u, VK_SHADER_STAGE_VERTEX_BIT, nullptr } );

			writes.emplace_back( eCamera, 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ cameraUbo.getUbo().getBuffer().getBuffer(), cameraUbo.getUbo().getByteOffset(), cameraUbo.getUbo().getByteRange() } } );
			writes.emplace_back( eMeshConfig, 0u, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ cubeBoxUbo.getBuffer().getBuffer(), cubeBoxUbo.getOffset(), cubeBoxUbo.getSize() } } );
		}

		static castor::Point4fArray generateLinesFromTriangles( castor::Point3fArray const & vertices )
		{
			castor::Point4fArray result;
			result.reserve( vertices.size() * 2u );

			for ( uint32_t i = 0; i < vertices.size(); i += 3u )
			{
				auto p0 = vertices[i + 0u];
				auto p1 = vertices[i + 1u];
				auto p2 = vertices[i + 2u];

				result.emplace_back( p0[0], p0[1], p0[2], 1.0f );
				result.emplace_back( p1[0], p1[1], p1[2], 1.0f );
				result.emplace_back( p1[0], p1[1], p1[2], 1.0f );
				result.emplace_back( p2[0], p2[1], p2[2], 1.0f );
				result.emplace_back( p2[0], p2[1], p2[2], 1.0f );
				result.emplace_back( p0[0], p0[1], p0[2], 1.0f );
			}

			return result;
		}
	}

	DebugMeshManager::DebugMeshManager( castor3d::RenderTarget const & renderTarget )
		: m_device{ *renderTarget.getEngine()->getRenderDevice() }
		, m_renderTarget{ renderTarget }
		, m_aabbMeshColour{ 0.0f, 1.0f, 0.0f, 1.0f }
		, m_obbMeshColour{ 1.0f, 0.0f, 0.0f, 1.0f }
		, m_obbSelectedSubmeshColour{ 1.0f, 1.0f, 0.0f, 1.0f }
		, m_obbSubmeshColour{ 0.0f, 0.0f, 1.0f, 1.0f }
		, m_obbBoneColour{ 0.0f, 0.0f, 0.5f, 1.0f }
		, m_meshConfigBuffer{ m_device.bufferPool->getBuffer< DebugMeshConfig >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 1000u, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ) }
	{
		castor3d::ProgramModule cubeModule{ cuT( "BoundingBox" )
			, dbgmsh::createDisplayCubeProgram( m_device ) };
		m_cubeProgram = makeProgramStates( m_device, cubeModule );
		castor3d::ProgramModule meshModule{ cuT( "Mesh" )
			, dbgmsh::createDisplayLinesProgram( m_device ) };
		m_meshProgram = makeProgramStates( m_device, meshModule );
		dbgmsh::createBindings( m_renderTarget.getCameraUbo()
			, m_meshConfigBuffer
			, m_bindings
			, m_writes );
	}

	DebugMeshManager::~DebugMeshManager()
	{
		if ( m_sceneConnection )
		{
			m_sceneConnection.disconnect();
			m_submesh = nullptr;
			m_object = nullptr;
			m_light = nullptr;
			m_device.bufferPool->putBuffer( m_meshConfigBuffer );
			m_device.bufferPool->putBuffer( m_pointLightVertexBuffer );

			for ( auto [angle, buffer] : m_spotLightVertexBuffers )
			{
				m_device.bufferPool->putBuffer( buffer );
			}
		}
	}

	void DebugMeshManager::select( castor3d::Light const & light )
	{
		if ( light.getLightType() == castor3d::LightType::eDirectional )
		{
			return;
		}

		castor3d::Engine const * engine = m_renderTarget.getEngine();

		if ( light.getLightType() == castor3d::LightType::ePoint )
		{
			if ( !m_pointLightVertexBuffer )
			{
				auto vertices = dbgmsh::generateLinesFromTriangles( castor3d::PointLight::generateVertices() );
				m_pointLightVertexBuffer = m_device.bufferPool->getBuffer< castor::Point4f >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, vertices.size()
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
				auto queue = m_device.graphicsData();
				castor3d::InstantDirectUploadData uploader{ *queue->queue
					, m_device
					, cuT( "PointLight" )
					, *queue->commandPool };
				uploader->pushUpload( vertices.data()->constPtr()
					, vertices.size() * sizeof( castor::Point4f )
					, m_pointLightVertexBuffer.getBuffer().getBuffer()
					, m_pointLightVertexBuffer.getOffset()
					, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
		}
		else if ( light.getLightType() == castor3d::LightType::eSpot )
		{
			castor3d::SpotLight const & spot = *light.getSpotLight();
			auto [it, res] = m_spotLightVertexBuffers.try_emplace( uint32_t( std::ceil( spot.getOuterCutOff().degrees() ) ) );

			if ( res )
			{
				auto vertices = dbgmsh::generateLinesFromTriangles( castor3d::SpotLight::generateVertices( it->first ) );
				it->second = m_device.bufferPool->getBuffer< castor::Point4f >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, vertices.size()
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
				auto queue = m_device.graphicsData();
				castor3d::InstantDirectUploadData uploader{ *queue->queue
					, m_device
					, cuT( "SpotLight_" ) + castor::string::toString( it->first )
					, *queue->commandPool };
				uploader->pushUpload( vertices.data()->constPtr()
					, vertices.size() * sizeof( castor::Point4f )
					, it->second.getBuffer().getBuffer()
					, it->second.getOffset()
					, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
		}

		engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this, &light]()
			{
				m_light = &light;
				auto scene = m_renderTarget.getScene();

				m_sceneConnection = scene->onUpdate.connect( [this]( castor3d::Scene const & )
					{
						onDisplayLight();
					} );
			} ) );
	}

	void DebugMeshManager::select( castor3d::Geometry const & object
		, castor3d::Submesh const & submesh )
	{
		castor3d::Engine const * engine = m_renderTarget.getEngine();
		engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this, &object, &submesh]()
			{
				m_object = &object;
				m_submesh = &submesh;
				auto scene = m_renderTarget.getScene();

				m_sceneConnection = scene->onUpdate.connect( [this]( castor3d::Scene const & )
					{
						onDisplayObject();
					} );
			} ) );
	}

	void DebugMeshManager::unselect()
	{
		if ( m_sceneConnection )
		{
			castor3d::Engine const * engine = m_renderTarget.getEngine();
			engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					m_sceneConnection.disconnect();
					m_submesh = nullptr;
					m_object = nullptr;
					m_light = nullptr;
				} ) );
		}
	}

	void DebugMeshManager::onDisplayObject()
	{
		if ( m_object )
		{
			auto debugMeshData = m_meshConfigBuffer.getData();
			auto const & baseTransform = m_object->getGlobalTransform();
			uint32_t index{};

			auto & obb = m_object->getBoundingBox();
			debugMeshData[index].colour = m_obbMeshColour;
			debugMeshData[index].world = baseTransform;
			castor::matrix::transform( debugMeshData[index].world
				, obb.getCenter()
				, obb.getDimensions()
				, castor::Quaternion::identity() );
			++index;

			auto aabb = obb.getAxisAligned( baseTransform );
			debugMeshData[index].colour = m_aabbMeshColour;
			castor::matrix::setTransform( debugMeshData[index].world
				, aabb.getCenter()
				, aabb.getDimensions()
				, castor::Quaternion::identity() );
			++index;

			for ( auto & submesh : *m_object->getMesh() )
			{
				debugMeshData[index].colour = submesh.get() == m_submesh
					? m_obbSelectedSubmeshColour
					: m_obbSubmeshColour;
				debugMeshData[index].world = baseTransform;
				auto sbb = m_object->getBoundingBox( *submesh );
				castor::matrix::transform( debugMeshData[index].world
					, sbb.getCenter()
					, sbb.getDimensions()
					, castor::Quaternion::identity() );
				++index;
			}

			m_meshConfigBuffer.markDirty( VK_ACCESS_SHADER_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );

			castor3d::addDebugDrawable( m_renderTarget
				, castor3d::DebugVertexBuffers{ {}, {}, 24u }
				, castor3d::DebugIndexBuffer{ {}, 0u }
				, ashes::VkVertexInputAttributeDescriptionArray{}
				, ashes::VkVertexInputBindingDescriptionArray{}
				, m_bindings
				, m_writes
				, index
				, m_cubeProgram
				, true );
		}
	}

	void DebugMeshManager::onDisplayLight()
	{
		if ( m_light )
		{
			castor3d::GpuBufferOffsetT< castor::Point4f > const * buffer{};
			castor::Point3f scale;

			if ( m_light->getLightType() == castor3d::LightType::ePoint )
			{
				castor3d::PointLight const & point = *m_light->getPointLight();
				buffer = &m_pointLightVertexBuffer;
				scale->x = point.getRange();
				scale->y = point.getRange();
				scale->z = point.getRange();
			}
			else if ( m_light->getLightType() == castor3d::LightType::eSpot )
			{
				castor3d::SpotLight const & spot = *m_light->getSpotLight();
				auto [it, res] = m_spotLightVertexBuffers.try_emplace( uint32_t( std::ceil( spot.getOuterCutOff().degrees() ) ) );

				if ( res )
				{
					auto vertices = dbgmsh::generateLinesFromTriangles( castor3d::SpotLight::generateVertices( it->first ) );
					it->second = m_device.bufferPool->getBuffer< castor::Point4f >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, vertices.size()
						, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
					auto queue = m_device.graphicsData();
					castor3d::InstantDirectUploadData uploader{ *queue->queue
						, m_device
						, cuT( "SpotLight_" ) + castor::string::toString( it->first )
						, *queue->commandPool };
					uploader->pushUpload( vertices.data()->constPtr()
						, vertices.size() * sizeof( castor::Point4f )
						, it->second.getBuffer().getBuffer()
						, it->second.getOffset()
						, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
						, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
				}

				buffer = &it->second;
				scale->x = spot.getRange();
				scale->y = spot.getRange();
				scale->z = spot.getRange();
			}

			if ( buffer )
			{
				auto & debugMeshData = m_meshConfigBuffer.getData()[0u];
				debugMeshData.world = m_light->getParent()->getDerivedTransformationMatrix();
				debugMeshData.colour = m_aabbMeshColour;
				castor::matrix::transform( debugMeshData.world
					, castor::Point3f{}
					, scale
					, castor::Quaternion::identity() );

				m_meshConfigBuffer.markDirty( VK_ACCESS_SHADER_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );

				castor3d::addDebugDrawable( m_renderTarget
					, castor3d::DebugVertexBuffers{ { VkBuffer( buffer->getBuffer().getBuffer() ) }
						, { buffer->getOffset() }
						, buffer->getCount() }
					, castor3d::DebugIndexBuffer{ {}, 0u }
					, ashes::VkVertexInputAttributeDescriptionArray{ VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32A32_SFLOAT, 0u } }
					, ashes::VkVertexInputBindingDescriptionArray{ VkVertexInputBindingDescription{ 0u, 16u, VK_VERTEX_INPUT_RATE_VERTEX } }
					, m_bindings
					, m_writes
					, 1u
					, m_meshProgram
					, true );
			}
		}
	}
}
