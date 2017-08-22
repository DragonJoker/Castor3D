#include "Render/GlRenderSystem.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlBackBuffers.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "Mesh/GlGeometryBuffers.hpp"
#include "Miscellaneous/GlComputePipeline.hpp"
#include "Miscellaneous/GlQuery.hpp"
#include "Miscellaneous/GlTransformFeedback.hpp"
#include "Render/GlContext.hpp"
#include "Render/GlRenderPipeline.hpp"
#include "Render/GlViewport.hpp"
#include "Shader/GlUniformBufferBinding.hpp"
#include "Shader/GlShaderObject.hpp"
#include "Shader/GlShaderProgram.hpp"
#include "Texture/GlDirectTextureStorage.hpp"
#include "Texture/GlPboTextureStorage.hpp"
#include "Texture/GlSampler.hpp"
#include "Texture/GlImmutableTextureStorage.hpp"
#include "Texture/GlTboTextureStorage.hpp"
#include "Texture/GlGpuOnlyTextureStorage.hpp"
#include "Texture/GlTexture.hpp"

#include <Log/Logger.hpp>

#include <GlslWriter.hpp>

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>
#include <Technique/RenderTechnique.hpp>

#include <GL/gl.h>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	namespace
	{
#if defined( CASTOR_PLATFORM_WINDOWS )

		uint32_t getVideoMemorySizeBytes()
		{
			uint32_t result{ 0u };
			HDC hdc = ::CreateDC( "DISPLAY", 0, 0, 0 );

			if ( hdc )
			{
				std::array< uint32_t, 5 > input = { 0, 0, 0x27, 0, 0 };
				std::array< uint32_t, 5 > output = { 0, 0, 0, 0, 0 };
				int const size = int( input.size() * sizeof( uint32_t ) );

				if ( ::ExtEscape( hdc, 0x7032, size, LPCSTR( input.data() ), size, LPSTR( output.data() ) ) > 0 )
				{
					result = output[3] * 1048576;
				}

				::DeleteDC( hdc );
			}

			return result;
		}

#else

		uint32_t getVideoMemorySizeBytes()
		{
			return 0u;
		}

#endif
	}

	String GlRenderSystem::Name = cuT( "OpenGL Renderer" );
	String GlRenderSystem::Type = cuT( "opengl" );

	GlRenderSystem::GlRenderSystem( Engine & engine )
		: RenderSystem( engine, Name )
		, m_openGlMajor( 0 )
		, m_openGlMinor( 0 )
		, m_useVertexBufferObjects( false )
		, m_extensionsInit( false )
		, m_openGl( *this )
	{
	}

	GlRenderSystem::~GlRenderSystem()
	{
	}

	RenderSystemUPtr GlRenderSystem::create( Engine & engine )
	{
		return std::make_unique< GlRenderSystem >( engine );
	}

	bool GlRenderSystem::InitOpenGlExtensions()
	{
		if ( !m_extensionsInit )
		{
			if ( !getOpenGl().initialise() )
			{
				m_extensionsInit = false;
			}
			else
			{
				m_extensionsInit = true;
				m_gpuInformations.setVendor( getOpenGl().getVendor() );
				m_gpuInformations.setRenderer( getOpenGl().getRenderer() );
				m_gpuInformations.setVersion( getOpenGl().getStrVersion() );
				m_gpuInformations.setShaderLanguageVersion( getOpenGl().getGlslVersion() );
				m_gpuInformations.updateFeature( GpuFeature::eConstantsBuffers, getOpenGl().hasUbo() );
				m_gpuInformations.updateFeature( GpuFeature::eTextureBuffers, getOpenGl().hasTbo() );
				m_gpuInformations.updateFeature( GpuFeature::eInstancing, getOpenGl().hasInstancing() );
				m_gpuInformations.updateFeature( GpuFeature::eAccumulationBuffer, true );
				m_gpuInformations.updateFeature( GpuFeature::eNonPowerOfTwoTextures, getOpenGl().hasNonPowerOfTwoTextures() );
				m_gpuInformations.updateFeature( GpuFeature::eAtomicCounterBuffers, getOpenGl().hasExtension( ARB_shader_atomic_counters, false ) );
				m_gpuInformations.updateFeature( GpuFeature::eImmutableTextureStorage, getOpenGl().hasExtension( ARB_texture_storage, false ) );
				m_gpuInformations.updateFeature( GpuFeature::eShaderStorageBuffers, getOpenGl().hasSsbo() );
				m_gpuInformations.updateFeature( GpuFeature::eTransformFeedback, getOpenGl().hasExtension( ARB_transform_feedback2 ) );

				m_openGlMajor = getOpenGl().getVersion() / 10;
				m_openGlMinor = getOpenGl().getVersion() % 10;

				Logger::logInfo( StringStream() << cuT( "Using version " ) << m_openGlMajor << cuT( "." ) << m_openGlMinor << cuT( " core functions" ) );
				m_gpuInformations.useShaderType( ShaderType::eCompute, getOpenGl().hasCSh() );
				m_gpuInformations.useShaderType( ShaderType::eHull, getOpenGl().hasTSh() );
				m_gpuInformations.useShaderType( ShaderType::eDomain, getOpenGl().hasTSh() );
				m_gpuInformations.useShaderType( ShaderType::eGeometry, getOpenGl().hasGSh() );
				m_gpuInformations.useShaderType( ShaderType::ePixel, getOpenGl().hasPSh() );
				m_gpuInformations.useShaderType( ShaderType::eVertex, getOpenGl().hasVSh() );

				std::array< int, 3 > value{};
				getOpenGl().GetIntegerv( GlMin::eMapBufferAlignment, value.data() );
				m_gpuInformations.setMinValue( GpuMin::eMapBufferAlignment, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMin::eProgramTexelOffset, value.data() );
				m_gpuInformations.setMinValue( GpuMin::eProgramTexelOffset, value[0] );
				value = { 0, 0, 0 };

				if ( m_gpuInformations.hasShaderType( ShaderType::eCompute ) )
				{
					getOpenGl().GetIntegerv( GlMax::eComputeUniformBlocks, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eComputeUniformBlocks, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eComputeTextureImageUnits, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eComputeTextureImageUnits, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eComputeUniformComponents, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eComputeUniformComponents, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eComputeAtomicCounters, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eComputeAtomicCounters, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eComputeAtomicCounterBuffers, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eComputeAtomicCounterBuffers, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eCombinedComputeUniformComponents, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eComputeCombinedUniformComponents, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eComputeWorkGroupInvocations, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eComputeWorkGroupInvocations, value[0] );
					value = { 0, 0, 0 };
					//getOpenGl().GetIntegerv( GlMax::eComputeWorkGroupCount, value.data() );
					//m_gpuInformations.setMaxValue( GpuMax::eComputeWorkGroupCount, value[0] );l_value = { 0, 0, 0 };
					//getOpenGl().GetIntegerv( GlMax::eComputeWorkGroupSize, value.data() );
					//m_gpuInformations.setMaxValue( GpuMax::eComputeWorkGroupSize, value[0] );l_value = { 0, 0, 0 };
				}

				if ( getOpenGl().hasExtension( ARB_shader_storage_buffer_object ) )
				{
					if ( m_gpuInformations.hasShaderType( ShaderType::eCompute ) )
					{
						getOpenGl().GetIntegerv( GlMax::eComputeShaderStorageBlocks, value.data() );
						m_gpuInformations.setMaxValue( GpuMax::eComputeShaderStorageBlocks, value[0] );
						value = { 0, 0, 0 };
						getOpenGl().GetIntegerv( GlMax::eCombinedShaderStorageBlocks, value.data() );
						m_gpuInformations.setMaxValue( GpuMax::eCombinedShaderStorageBlocks, value[0] );
						value = { 0, 0, 0 };
					}

					getOpenGl().GetIntegerv( GlMax::eFragmentShaderStorageBlocks, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eFragmentShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eGeometryShaderStorageBlocks, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eGeometryShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eTessControlShaderStorageBlocks, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eTessControlShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eTessEvaluationShaderStorageBlocks, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eTessEvaluationShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eVertexShaderStorageBlocks, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eVertexShaderStorageBlocks, value[0] );
					value = { 0, 0, 0 };
					getOpenGl().GetIntegerv( GlMax::eShaderStorageBufferBindings, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eShaderStorageBufferBindings, value[0] );
					value = { 0, 0, 0 };
				}

				if ( getOpenGl().hasExtension( ARB_explicit_uniform_location ) )
				{
					getOpenGl().GetIntegerv( GlMax::eUniformLocations, value.data() );
					m_gpuInformations.setMaxValue( GpuMax::eUniformLocations, value[0] );
					value = { 0, 0, 0 };
				}

				if ( getOpenGl().hasExtension( ATI_meminfo ) )
				{
					getOpenGl().GetIntegerv( GlGpuInfo::eVBOFreeMemoryATI, &value[0] );
					getOpenGl().GetIntegerv( GlGpuInfo::eTextureFreeMemoryATI, &value[1] );
					getOpenGl().GetIntegerv( GlGpuInfo::eRenderbufferFreeMemoryATI, &value[2] );
					m_gpuInformations.setTotalMemorySize( value[0] + value[1] + value[2] );
					value = { 0, 0, 0 };
				}
				else if ( getOpenGl().hasExtension( NVX_gpu_memory_info ) )
				{
					getOpenGl().GetIntegerv( GlGpuInfo::eTotalAvailableMemNVX, value.data() );
					m_gpuInformations.setTotalMemorySize( value[0] );
					value = { 0, 0, 0 };
				}
				else
				{
					m_gpuInformations.setTotalMemorySize( getVideoMemorySizeBytes() );
				}

				getOpenGl().GetIntegerv( GlMax::eDebugGroupStackDepth, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eDebugGroupStackDepth, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTexture3DSize, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTexture3DSize, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTextureArrayLayers, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eArrayTextureLayers, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eClipDistances, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eClipDistances, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eColorTextureSamples, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eColourTextureSamples, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eCombinedAtomicCounters, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eCombinedAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eCombinedFragmentUniformComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFragmentCombinedUniformComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eCombinedGeometryUniformComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eGeometryCombinedUniformComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eCombinedTextureImageUnits, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eCombinedTextureImageUnits, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eCombinedUniformBlocks, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eCombinedUniformBlocks, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eCombinedVertexUniformComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVertexCombinedUniformComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTextureCubeSize, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTextureCubeMapSize, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eDepthTextureSamples, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eDepthTextureSamples, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eDrawBuffers, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eDrawBuffers, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eDualSourceDrawBuffers, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eDualSourceDrawBuffers, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eElementsIndices, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eElementIndices, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eElementsVertices, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eElementVertices, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFragmentAtomicCounters, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFragmentAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFragmentInputComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFragmentInputComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFragmentUniformComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFragmentUniformComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFragmentUniformVectors, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFragmentUniformVectors, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFragmentUniformBlocks, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFragmentUniformBlocks, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFramebufferWidth, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFramebufferWidth, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFramebufferHeight, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFramebufferHeight, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFramebufferLayers, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFramebufferLayers, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eFramebufferSamples, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eFramebufferSamples, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eGeometryAtomicCounters, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eGeometryAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eGeometryInputComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eGeometryInputComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eGeometryOutputComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eGeometryOutputComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eGeometryTextureImageUnits, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eGeometryTextureImageUnits, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eGeometryUniformBlocks, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eGeometryUniformBlocks, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eGeometryUniformComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eGeometryUniformComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eIntegerSamples, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eIntegerSamples, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eLabelLength, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eLabelLength, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eProgramTexelOffset, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eProgramTexelOffset, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eRectangleTextureSize, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTextureRectangleSize, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eRenderbufferSize, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eRenderbufferSize, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eSampleMaskWords, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eSampleMaskWords, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eServerWaitTimeout, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eServerWaitTimeout, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTessControlAtomicCounters, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTessControlAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTessEvaluationAtomicCounters, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTessEvaluationAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTextureBufferSize, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTextureBufferSize, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTextureImageUnits, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTextureImageUnits, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTextureLODBias, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTextureLodBias, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eTextureSize, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eTextureSize, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eUniformBufferBindings, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eUniformBufferBindings, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eUniformBlockSize, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eUniformBlockSize, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVaryingComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVaryingComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVaryingVectors, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVaryingVectors, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVaryingFloats, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVaryingFloats, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVertexAtomicCounters, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVertexAtomicCounters, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVertexAttribs, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVertexAttribs, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVertexTextureImageUnits, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVertexTextureImageUnits, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVertexUniformComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVertexUniformComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVertexUniformVectors, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVertexUniformVectors, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVertexOutputComponents, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVertexOutputComponents, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eVertexUniformBlocks, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eVertexUniformBlocks, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eViewportDims, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eViewportWidth, value[0] );
				m_gpuInformations.setMaxValue( GpuMax::eViewportHeight, value[1] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eViewports, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eViewports, value[0] );
				value = { 0, 0, 0 };
				getOpenGl().GetIntegerv( GlMax::eSamples, value.data() );
				m_gpuInformations.setMaxValue( GpuMax::eSamples, value[0] );
				value = { 0, 0, 0 };
			}
		}

		return m_extensionsInit;
	}

	ContextSPtr GlRenderSystem::createContext()
	{
		return std::make_shared< GlContext >( *this, getOpenGl() );
	}

	GeometryBuffersSPtr GlRenderSystem::createGeometryBuffers( Topology p_topology
		, ShaderProgram const & p_program )
	{
		return std::make_shared< GlGeometryBuffers >( getOpenGl(), p_topology, p_program );
	}

	RenderPipelineUPtr GlRenderSystem::createRenderPipeline( DepthStencilState && p_dsState
		, RasteriserState && p_rsState
		, BlendState && p_bdState
		, MultisampleState && p_msState
		, ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		return std::make_unique< GlRenderPipeline >( getOpenGl(), *this, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags );
	}

	ComputePipelineUPtr GlRenderSystem::createComputePipeline( ShaderProgram & p_program )
	{
		return std::make_unique< GlComputePipeline >( getOpenGl(), *this, p_program );
	}

	SamplerSPtr GlRenderSystem::createSampler( castor::String const & p_name )
	{
		return std::make_shared< GlSampler >( getOpenGl(), this, p_name );
	}

	ShaderProgramSPtr GlRenderSystem::createShaderProgram()
	{
		return std::make_shared< GlShaderProgram >( getOpenGl(), *this );
	}

	UniformBufferBindingUPtr GlRenderSystem::createUniformBufferBinding( UniformBuffer & p_ubo
		, ShaderProgram const & p_program )
	{
		return std::make_unique< GlUniformBufferBinding >( getOpenGl()
			, p_ubo
			, static_cast< GlShaderProgram const & >( p_program ) );
	}

	TransformFeedbackUPtr GlRenderSystem::createTransformFeedback( BufferDeclaration const & p_computed
		, Topology p_topology
		, ShaderProgram & p_program )
	{
		return std::make_unique< GlTransformFeedback >( getOpenGl(), *this, p_computed, p_topology, p_program );
	}

	TextureLayoutSPtr GlRenderSystem::createTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess )
	{
		return std::make_shared< GlTexture >( getOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess );
	}

	TextureLayoutSPtr GlRenderSystem::createTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess
		, PixelFormat p_format
		, Size const & p_size )
	{
		return std::make_shared< GlTexture >( getOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureLayoutSPtr GlRenderSystem::createTexture( TextureType p_type
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess
		, PixelFormat p_format
		, Point3ui const & p_size )
	{
		return std::make_shared< GlTexture >( getOpenGl(), *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureStorageUPtr GlRenderSystem::createTextureStorage( TextureStorageType p_type
		, TextureLayout & p_image
		, AccessTypes const & p_cpuAccess
		, AccessTypes const & p_gpuAccess )
	{
		TextureStorageUPtr result;

		if ( p_type == TextureStorageType::eBuffer )
		{
			result = std::make_unique< GlTextureStorage< GlTboTextureStorageTraits > >( getOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
		}

		if ( !result )
		{
			if ( !checkFlag( p_gpuAccess, AccessType::eWrite ) )
			{
				if ( getOpenGl().hasExtension( ARB_texture_storage, false )
					 && p_cpuAccess == AccessType::eNone )
				{
					result = std::make_unique< GlTextureStorage< GlImmutableTextureStorageTraits > >( getOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
				else
				{
					result = std::make_unique< GlTextureStorage< GlDirectTextureStorageTraits > >( getOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
				}
			}
			else if ( p_cpuAccess != AccessType::eNone )
			{
				result = std::make_unique< GlTextureStorage< GlPboTextureStorageTraits > >( getOpenGl(), *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
			}
			else
			{
				result = std::make_unique< GlTextureStorage< GlGpuOnlyTextureStorageTraits > >( getOpenGl(), *this, p_type, p_image, AccessType::eNone, p_gpuAccess );
			}
		}

		return result;
	}

	FrameBufferSPtr GlRenderSystem::createFrameBuffer()
	{
		return std::make_shared< GlFrameBuffer >( getOpenGl(), *getEngine() );
	}

	BackBuffersSPtr GlRenderSystem::createBackBuffers()
	{
		return std::make_shared< GlBackBuffers >( getOpenGl(), *getEngine() );
	}

	GpuQueryUPtr GlRenderSystem::createQuery( QueryType p_type )
	{
		return std::make_unique< GlQuery >( *this, p_type );
	}

	IViewportImplUPtr GlRenderSystem::createViewport( Viewport & p_viewport )
	{
		return std::make_unique< GlViewport >( *this, p_viewport );
	}

	void GlRenderSystem::doInitialise()
	{
		if ( !m_initialised )
		{
			InitOpenGlExtensions();

			m_useVertexBufferObjects = getOpenGl().hasVbo();
			m_initialised = true;
		}
	}

	void GlRenderSystem::doCleanup()
	{
		m_extensionsInit = false;
		m_initialised = false;
		getOpenGl().cleanup();
	}

	GpuBufferUPtr GlRenderSystem::doCreateBuffer( BufferType p_type )
	{
		GpuBufferUPtr result;

		switch ( p_type )
		{
		case BufferType::eArray:
			result = std::make_unique< GlBuffer >( *this
				, getOpenGl()
				, GlBufferTarget::eArray );
			break;

		case BufferType::eElementArray:
			result = std::make_unique< GlBuffer >( *this
				, getOpenGl()
				, GlBufferTarget::eElementArray );
			break;

		case BufferType::eUniform:
			result = std::make_unique< GlBuffer >( *this
				, getOpenGl()
				, GlBufferTarget::eUniform );
			break;

		case BufferType::eAtomicCounter:
			if ( getOpenGl().hasSsbo() )
			{
				result = std::make_unique< GlBuffer >( *this
					, getOpenGl()
					, GlBufferTarget::eAtomicCounter );
			}
			break;

		case BufferType::eShaderStorage:
			if ( getOpenGl().hasSsbo() )
			{
				result = std::make_unique< GlBuffer >( *this
					, getOpenGl()
					, GlBufferTarget::eShaderStorage );
			}
			break;
		}

		return result;
	}

#if C3D_TRACE_OBJECTS

	bool GlRenderSystem::track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )
	{
		std::string name;
		return m_tracker.track( p_object, p_name, p_file, p_line, name );
	}

	bool GlRenderSystem::untrack( void * p_object )
	{
		GpuObjectTracker::ObjectDeclaration decl;
		return m_tracker.untrack( p_object, decl );
	}

#endif

}
