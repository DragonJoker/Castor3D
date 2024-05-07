#include "GuiCommon/System/CubeBoxManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBufferPool.hpp>
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
	namespace cubbox
	{
		static void createDisplayClustersAABBProgram( castor3d::RenderDevice const & device
			, castor3d::CameraUbo const & cameraUbo
			, castor3d::GpuBufferOffsetT< CubeBoxConfig > const & cubeBoxUbo
			, ashes::PipelineShaderStageCreateInfoArray & program
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, ashes::WriteDescriptorSetArray & writes )
		{
			enum BindingPoints
			{
				eCamera,
				eCubeBox,
			};

			struct CubeBoxData
				: public sdw::StructInstanceHelperT< "GC_CubeBoxData"
					, sdw::type::MemoryLayout::eStd140
					, sdw::Mat4x4Field< "world" >
					, sdw::Vec4Field< "colour" > >
			{
				CubeBoxData( sdw::ShaderWriter & writer
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

			namespace c3d = castor3d::shader;

			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_Camera( writer
				, eCamera
				, 0u );
			sdw::StorageBuffer cubeBox{ writer
				, "GC_CubeBox"
				, "gc_cubeBox"
				, uint32_t( eCubeBox )
				, uint32_t( 0u )
				, sdw::type::MemoryLayout::eStd430 };
			auto gc_cubeBoxData = cubeBox.declMemberArray< CubeBoxData >( "c" );
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

			writer.implementEntryPointT< sdw::VoidT, c3d::Colour4FT >( [&writer, &positions, &c3d_cameraData, &gc_cubeBoxData]( sdw::VertexIn const & in
				, sdw::VertexOutT< c3d::Colour4FT > out )
				{
					auto index = writer.declLocale( "index"
						, in.vertexIndex );
					auto position = writer.declLocale< sdw::Vec4 >( "position"
						, positions[index] );
					position = gc_cubeBoxData[in.instanceIndex].modelToWorld(position);
					out.vtx.position = c3d_cameraData.worldToCurProj( position );
					out.colour() = gc_cubeBoxData[in.instanceIndex].colour();
				} );

			writer.implementEntryPointT< c3d::Colour4FT, c3d::Colour4FT >( []( sdw::FragmentInT< c3d::Colour4FT > const & in
				, sdw::FragmentOutT< c3d::Colour4FT > const & out )
				{
					out.colour() = in.colour();
				} );

			castor3d::ProgramModule programModule{ "ClustersAABB", writer.getBuilder().releaseShader() };
			program = makeProgramStates( device, programModule );

			bindings.push_back( VkDescriptorSetLayoutBinding{ eCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1u, VK_SHADER_STAGE_VERTEX_BIT, nullptr } );
			bindings.push_back( VkDescriptorSetLayoutBinding{ eCubeBox, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1u, VK_SHADER_STAGE_VERTEX_BIT, nullptr } );

			writes.emplace_back( eCamera, 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ cameraUbo.getUbo().getBuffer().getBuffer(), cameraUbo.getUbo().getByteOffset(), cameraUbo.getUbo().getByteRange() } } );
			writes.emplace_back( eCubeBox, 0u, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ cubeBoxUbo.getBuffer().getBuffer(), cubeBoxUbo.getOffset(), cubeBoxUbo.getSize() } } );
		}
	}

	CubeBoxManager::CubeBoxManager( castor3d::RenderTarget const & renderTarget )
		: m_renderTarget{ renderTarget }
		, m_aabbMeshColour{ 0.0f, 1.0f, 0.0f, 1.0f }
		, m_obbMeshColour{ 1.0f, 0.0f, 0.0f, 1.0f }
		, m_obbSelectedSubmeshColour{ 1.0f, 1.0f, 0.0f, 1.0f }
		, m_obbSubmeshColour{ 0.0f, 0.0f, 1.0f, 1.0f }
		, m_obbBoneColour{ 0.0f, 0.0f, 0.5f, 1.0f }
		, m_cubeBoxBuffer{ m_renderTarget.getEngine()->getRenderDevice()->bufferPool->getBuffer< CubeBoxConfig >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 1000u, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ) }
	{
		cubbox::createDisplayClustersAABBProgram( *m_renderTarget.getEngine()->getRenderDevice()
			, m_renderTarget.getCameraUbo()
			, m_cubeBoxBuffer
			, m_program
			, m_bindings
			, m_writes );
	}

	CubeBoxManager::~CubeBoxManager()
	{
		if ( m_object )
		{
			m_sceneConnection.disconnect();
			m_object = nullptr;
		}
	}

	void CubeBoxManager::displayObject( castor3d::Geometry const & object
		, castor3d::Submesh const & submesh )
	{
		castor3d::Engine * engine = m_renderTarget.getEngine();
		engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this, &object, &submesh]()
			{
				m_object = &object;
				m_submesh = &submesh;
				auto scene = m_renderTarget.getScene();

				m_sceneConnection = scene->onUpdate.connect( [this]( castor3d::Scene const & scene )
					{
						onSceneUpdate( scene );
					} );
			} ) );
	}

	void CubeBoxManager::hideObject( castor3d::Geometry const & object )
	{
		CU_Require( object.getName() == m_object->getName() );
		castor3d::Engine * engine = m_renderTarget.getEngine();
		engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this]()
			{
				m_sceneConnection.disconnect();
				m_object = nullptr;
				m_submesh = nullptr;
			} ) );
	}

	void CubeBoxManager::onSceneUpdate( castor3d::Scene const & scene )
	{
		if ( m_object )
		{
			auto cubeBoxData = m_cubeBoxBuffer.getData();
			auto const & baseTransform = m_object->getGlobalTransform();
			uint32_t index{};

			auto & obb = m_object->getBoundingBox();
			cubeBoxData[index].colour = m_obbMeshColour;
			cubeBoxData[index].world = baseTransform;
			castor::matrix::transform( cubeBoxData[index].world
				, obb.getCenter()
				, obb.getDimensions()
				, castor::Quaternion::identity() );
			++index;

			auto aabb = obb.getAxisAligned( baseTransform );
			cubeBoxData[index].colour = m_aabbMeshColour;
			castor::matrix::setTransform( cubeBoxData[index].world
				, aabb.getCenter()
				, aabb.getDimensions()
				, castor::Quaternion::identity() );
			++index;

			for ( auto & submesh : *m_object->getMesh() )
			{
				cubeBoxData[index].colour = submesh.get() == m_submesh
					? m_obbSelectedSubmeshColour
					: m_obbSubmeshColour;
				cubeBoxData[index].world = baseTransform;
				auto sbb = m_object->getBoundingBox( *submesh );
				castor::matrix::transform( cubeBoxData[index].world
					, sbb.getCenter()
					, sbb.getDimensions()
					, castor::Quaternion::identity() );
				++index;
			}

			m_cubeBoxBuffer.markDirty( VK_ACCESS_SHADER_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );

			castor3d::addDebugDrawable( m_renderTarget
				, castor3d::DebugVertexBuffers{ {}, {}, 24u }
				, castor3d::DebugIndexBuffer{ {}, 0u }
				, ashes::VkVertexInputAttributeDescriptionArray{}
				, ashes::VkVertexInputBindingDescriptionArray{}
				, m_bindings
				, m_writes
				, index
				, m_program
				, true );
		}
	}
}
