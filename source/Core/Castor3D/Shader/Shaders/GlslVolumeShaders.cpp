#include "Castor3D/Shader/Shaders/GlslVolumeShaders.hpp"

#include "Castor3D/Render/Volumetric/VolumeComponentPlugin.hpp"
#include "Castor3D/Render/Volumetric/VolumeComponentRegister.hpp"
#include "Castor3D/Shader/Shaders/GlslRay.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

namespace c3d::shader
{
	//************************************************************************************************

	Volume::Volume( sdw::UInt32 const & pid
		, sdw::UInt32 const & ptype
		, sdw::Float const & pbegin
		, sdw::Float const & pend
		, sdw::Float const & pstepSize
		, sdw::Float const & psamples )
		: Volume{ sdw::findWriterMandat( pid, ptype, pbegin, pend, pstepSize )
			, sdw::makeAggrInit( makeType( sdw::findTypesCache( pid, ptype, pbegin, pend, pstepSize ) )
				, makeExprList( sdw::makeExpr( pid ), sdw::makeExpr( ptype )
					, sdw::makeExpr( pbegin ), sdw::makeExpr( pend )
					, sdw::makeExpr( pstepSize ), sdw::makeExpr( psamples ) ) )
			, true }
	{
	}

	//************************************************************************************************

	VolumesTraversalResult::VolumesTraversalResult( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, c3d::move( expr ), enabled }
		, inscatter{ getMember< sdw::Vec3 >( "inscatter" ) }
		, transmittance{ getMember< sdw::Vec3 >( "transmittance" ) }
		, renderSize{ getMember< sdw::Vec2 >( "renderSize" ) }
		, transmittanceAboveThreshold{ getMember< sdw::Boolean >( "transmittanceAboveThreshold" ) }
	{
	}

	VolumesTraversalResult::VolumesTraversalResult( sdw::ShaderWriter & writer
		, VolumeShaders const & volumeShaders
		, sdw::Vec2 const & size )
		: VolumesTraversalResult{ writer
			, makeInit( writer, volumeShaders, size )
			, true }
	{
	}

	ast::type::BaseStructPtr VolumesTraversalResult::makeType( ast::type::TypesCache & cache
		, VolumeShaders const & volumeShaders )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430, "C3D_VolumesTraversalResult" );

		if ( result->empty() )
		{
			result->declMember( "inscatter", sdw::type::Kind::eVec3F );
			result->declMember( "transmittanceAboveThreshold", sdw::type::Kind::eBoolean );
			result->declMember( "transmittance", sdw::type::Kind::eVec3F );
			result->declMember( "renderSize", sdw::type::Kind::eVec2F );
			volumeShaders.fillType( *result );
		}

		return result;
	}

	sdw::expr::ExprPtr VolumesTraversalResult::makeInit( sdw::ShaderWriter const & writer
		, VolumeShaders const & volumeShaders
		, sdw::Vec2 const & size )
	{
		auto type = makeType( writer.getTypesCache(), volumeShaders );
		sdw::expr::ExprList result;
		result.emplace_back( makeExpr( vec3( 0.0_f ) ) ); // inscatter
		result.emplace_back( makeExpr( 1_b ) ); // transmittanceAboveThreshold
		result.emplace_back( makeExpr( vec3( 1.0_f ) ) ); // transmittance
		result.emplace_back( makeExpr( size ) ); // renderSize
		volumeShaders.fillInit( *type, result );
		return sdw::makeAggrInit( type, c3d::move( result ) );
	}

	//************************************************************************************************

	Volumes::Volumes( sdw::ShaderWriter & writer
		, VolumeShaders const & volumeShaders )
		: m_writer{ writer }
		, m_volumeShaders{ volumeShaders }
		, m_volumes{ m_writer.declLocaleArray< Volume >( "volumesToTraverse", MaxVolumeCount ) }
		, m_volumeCount{ m_writer.declLocale< sdw::UInt >( "volumesCount", 0_u ) }
	{
	}

	sdw::Float Volumes::addVolume( uint32_t type, sdw::Float const & begin, sdw::Float const & end, sdw::Float const & samples )
	{
		m_volumes[m_volumeCount] = Volume{ m_volumeCount, sdw::UInt{ type }
			, begin, end
			, ( end - begin ) / m_writer.cast< sdw::Float >( samples )
			, samples };
		return m_volumes[m_volumeCount++].end;
	}

	sdw::RetVoid Volumes::step( Volume const & pvolume
		, Ray const & pray
		, sdw::Float const & psample
		, sdw::Float const & pt
		, sdw::Float const & pdt
		, VolumesTraversalResult const & ptraversal )
	{
		if ( !m_stepVolume )
		{
			m_stepVolume = m_writer.implementFunction< sdw::Void >( "c3d_stepInVolume"
				, [this]( Volume const & volume
					, Ray const & ray
					, sdw::Float const & sample
					, sdw::Float t
					, sdw::Float dt
					, VolumesTraversalResult const & traversal )
				{
					sdwSWITCH( m_writer, volume.type )
					{
						uint32_t i = 0u;
						for ( auto func : m_stepFuncs )
						{
							sdwCASE( m_writer, i )
							{
								( *func )( volume, ray, sample, t, dt, traversal );
								m_writer.caseBreakStmt();
							}
							sdwESAC
							++i;
						}
						sdwDEFAULT( m_writer )
						{
							dt = volume.stepSize;
							t += dt;
							m_writer.caseBreakStmt();
						}
						sdwESAC
					}
					sdwHCTIWS
				}
				, InVolume{ m_writer, "volume" }
				, InOutRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "sample" }
				, sdw::InOutFloat{ m_writer, "t" }
				, sdw::InOutFloat{ m_writer, "dt" }
				, InOutVolumesTraversalResult{ m_writer, "traversal", m_volumeShaders } );
		}

		return m_stepVolume( pvolume, pray, psample, pt, pdt, ptraversal );
	}

	sdw::RetVoid Volumes::traverse( Volume const & pvolume
		, Ray const & pray
		, sdw::Float const & psample
		, sdw::Float const & pt
		, sdw::Float const & pdt
		, VolumesTraversalResult & presult )
	{
		if ( !m_traverseVolume )
		{
			m_traverseVolume = m_writer.implementFunction< sdw::Void >( "c3d_traverseVolume"
				, [this]( Volume const & volume
					, Ray const & ray
					, sdw::Float const & sample
					, sdw::Float const & t
					, sdw::Float const & dt
					, VolumesTraversalResult const & result )
				{
					auto pos = m_writer.declLocale( "pos", ray.step( t ) );
					sdwSWITCH( m_writer, volume.type )
					{
						uint32_t i = 0u;
						for ( auto func : m_traversalFuncs )
						{
							sdwCASE( m_writer, i )
							{
								( *func )( volume, ray, sample, t, dt, pos, result );
								m_writer.caseBreakStmt();
							}
							sdwESAC
							++i;
						}
					}
					sdwHCTIWS
				}
				, InVolume{ m_writer, "volume" }
				, InRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "sample" }
				, sdw::InFloat{ m_writer, "t" }
				, sdw::InFloat{ m_writer, "dt" }
				, InOutVolumesTraversalResult{ m_writer, "result", m_volumeShaders } );
		}

		return m_traverseVolume( pvolume, pray, psample, pt, pdt, presult );
	}

	uint32_t Volumes::registerVolumeType( VolumeTraversalFunc const & traversal, VolumeStepFunc const & step )
	{
		auto result = uint32_t( m_traversalFuncs.size() );
		m_traversalFuncs.push_back( &traversal );
		m_stepFuncs.push_back( &step );
		return result;
	}

	//************************************************************************************************

	VolumeShaders::~VolumeShaders()noexcept = default;

	VolumeShaders::VolumeShaders( sdw::ShaderWriter & writer
		, VolumeComponentRegister const & volumeRegister
		, c3d::Extent2D const & targetExtent
		, bool hasDepth
		, uint32_t enabledPlugins
		, uint32_t & binding )
		: m_volumeRegister{ volumeRegister }
		, m_volumesShaders{ volumeRegister.createShaders( writer, *this, targetExtent, hasDepth, enabledPlugins, binding ) }
	{
	}

	void VolumeShaders::fillType( sdw::type::BaseStruct & type )const
	{
		for ( auto & shader : m_volumesShaders )
			shader->fillType( type );
	}

	void VolumeShaders::fillInit( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		for ( auto & shader : m_volumesShaders )
			shader->fillInit( type, inits );
	}

	void VolumeShaders::registerVolumeTypes( Volumes & volumes )const
	{
		for ( auto & shader : m_volumesShaders )
			shader->registerVolumes( volumes );
	}

	void VolumeShaders::initialise( sdw::Vec2 const & pixelCoord, Ray & ray
		, Volumes & volumes, VolumesTraversalResult & result )const
	{
		for ( auto & shader : m_volumesShaders )
			shader->initialise( pixelCoord, ray, volumes, result );
	}

	void VolumeShaders::finalise( Ray const & ray, VolumesTraversalResult & result )const
	{
		for ( auto & shader : m_volumesShaders )
			shader->finalise( ray, result );
	}

	Engine & VolumeShaders::getEngine()const noexcept
	{
		return *m_volumeRegister.getEngine();
	}

	//************************************************************************************************
}
