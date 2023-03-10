#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	void OverlayData::cropMinValue( sdw::Float const & pssAbsOvPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Vec2 const & pssCropRange
		, sdw::Vec2 const & puvRange
		, sdw::Float pssRelBound
		, sdw::Float puv )
	{
		if ( !m_cropMinValue )
		{
			m_cropMinValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMinValue"
				, [&]( sdw::Float const & ssAbsOvPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Vec2 const & ssCropRange
					, sdw::Vec2 const & uvRange
					, sdw::Float ssRelBound
					, sdw::Float uv )
				{
					auto ssAbsBound = m_writer->declLocale( "ssAbsBound"
						, ssRelBound + ssAbsOvPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( ( ssAbsBound - ssCropRange.x() ) / ssAbsBoundSize
							, ( ssAbsBound - ssCropRange.y() ) / ssAbsBoundSize ) );

					IF( *m_writer, ssAbsBound > ssCropRange.y() )
					{
						ssRelBound = ssCropRange.y() - ssAbsOvPosition;
						uv += crop.y() * ( uvRange.y() - uvRange.x() );
						uv = clamp( uv, uvRange.x(), uvRange.y() );
					}
					ELSEIF( ssAbsBound < ssCropRange.x() )
					{
						ssRelBound = ssCropRange.x() - ssAbsOvPosition;
						uv -= crop.x() * ( uvRange.y() - uvRange.x() );
						uv = clamp( uv, uvRange.x(), uvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssAbsOvPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InVec2{ *m_writer, "ssCropRange" }
				, sdw::InVec2{ *m_writer, "uvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "uv" } );
		}

		m_cropMinValue( pssAbsOvPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssCropRange
			, puvRange
			, pssRelBound
			, puv );
	}

	void OverlayData::cropMaxValue( sdw::Float const & pssAbsOvPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Vec2 const & pssCropRange
		, sdw::Vec2 const & puvRange
		, sdw::Float pssRelBound
		, sdw::Float puv )
	{
		if ( !m_cropMaxValue )
		{
			m_cropMaxValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMaxValue"
				, [&]( sdw::Float const & ssAbsOvPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Vec2 const & ssCropRange
					, sdw::Vec2 const & uvRange
					, sdw::Float ssRelBound
					, sdw::Float uv )
				{
					auto ssAbsBound = m_writer->declLocale( "ssAbsBound"
						, ssRelBound + ssAbsOvPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( ( ssAbsBound - ssCropRange.x() ) / ssAbsBoundSize
							, ( ssAbsBound - ssCropRange.y() ) / ssAbsBoundSize ) );

					IF( *m_writer, ssAbsBound > ssCropRange.y() )
					{
						ssRelBound = ssCropRange.y() - ssAbsOvPosition;
						uv -= crop.y() * ( uvRange.y() - uvRange.x() );
						uv = clamp( uv, uvRange.x(), uvRange.y() );
					}
					ELSEIF( ssAbsBound < ssCropRange.x() )
					{
						ssRelBound = ssCropRange.x() - ssAbsOvPosition;
						uv += crop.x() * ( uvRange.y() - uvRange.x() );
						uv = clamp( uv, uvRange.x(), uvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssAbsOvPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InVec2{ *m_writer, "ssCropRange" }
				, sdw::InVec2{ *m_writer, "uvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "uv" } );
		}

		m_cropMaxValue( pssAbsOvPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssCropRange
			, puvRange
			, pssRelBound
			, puv );
	}

	void OverlayData::cropMinMinValue( sdw::Float const & pssAbsOvPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Float const & pssAbsCharSize
		, sdw::Vec2 const & pssCropRange
		, sdw::Vec2 const & ptexUvRange
		, sdw::Vec2 const & pfontUvRange
		, sdw::Float pssRelBound
		, sdw::Float ptexUv
		, sdw::Float pfontUv )
	{
		if ( !m_cropMinMinValue )
		{
			m_cropMinMinValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMinMinValue"
				, [&]( sdw::Float const & ssAbsOvPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Float const & ssAbsCharSize
					, sdw::Vec2 const & ssCropRange
					, sdw::Vec2 const & texUvRange
					, sdw::Vec2 const & fontUvRange
					, sdw::Float ssRelBound
					, sdw::Float texUv
					, sdw::Float fontUv )
				{
					auto ssAbsBound = m_writer->declLocale( "ssAbsBound"
						, ssRelBound + ssAbsOvPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( ( ssAbsBound - ssCropRange.x() ) / ssAbsCharSize
							, ( ssAbsBound - ssCropRange.y() ) / ssAbsCharSize ) );

					IF( *m_writer, ssAbsBound > ssCropRange.y() )
					{
						ssRelBound = ssCropRange.y() - ssAbsOvPosition;
						texUv += crop.y() * ( texUvRange.y() - texUvRange.x() );
						fontUv += crop.y() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					ELSEIF( ssAbsBound < ssCropRange.x() )
					{
						ssRelBound = ssCropRange.x() - ssAbsOvPosition;
						texUv -= crop.x() * ( texUvRange.y() - texUvRange.x() );
						fontUv -= crop.x() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssAbsOvPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InFloat{ *m_writer, "ssAbsCharSize" }
				, sdw::InVec2{ *m_writer, "ssCropRange" }
				, sdw::InVec2{ *m_writer, "texUvRange" }
				, sdw::InVec2{ *m_writer, "fontUvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "texUv" }
				, sdw::InOutFloat{ *m_writer, "fontUv" } );
		}

		m_cropMinMinValue( pssAbsOvPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssAbsCharSize
			, pssCropRange
			, ptexUvRange
			, pfontUvRange
			, pssRelBound
			, ptexUv
			, pfontUv );
	};

	void OverlayData::cropMinMaxValue( sdw::Float const & pssAbsOvPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Float const & pssAbsCharSize
		, sdw::Vec2 const & pssCropRange
		, sdw::Vec2 const & ptexUvRange
		, sdw::Vec2 const & pfontUvRange
		, sdw::Float pssRelBound
		, sdw::Float ptexUv
		, sdw::Float pfontUv )
	{
		if ( !m_cropMinMaxValue )
		{
			m_cropMinMaxValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMinMaxValue"
				, [&]( sdw::Float const & ssAbsOvPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Float const & ssAbsCharSize
					, sdw::Vec2 const & ssCropRange
					, sdw::Vec2 const & texUvRange
					, sdw::Vec2 const & fontUvRange
					, sdw::Float ssRelBound
					, sdw::Float texUv
					, sdw::Float fontUv )
				{
					auto ssAbsBound = m_writer->declLocale( "ssAbsBound"
						, ssRelBound + ssAbsOvPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( ( ssAbsBound - ssCropRange.x() ) / ssAbsCharSize
							, ( ssAbsBound - ssCropRange.y() ) / ssAbsCharSize ) );

					IF( *m_writer, ssAbsBound > ssCropRange.y() )
					{
						ssRelBound = ssCropRange.y() - ssAbsOvPosition;
						texUv += crop.y() * ( texUvRange.y() - texUvRange.x() );
						fontUv -= crop.y() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					ELSEIF( ssAbsBound < ssCropRange.x() )
					{
						ssRelBound = ssCropRange.x() - ssAbsOvPosition;
						texUv -= crop.x() * ( texUvRange.y() - texUvRange.x() );
						fontUv += crop.x() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssAbsOvPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InFloat{ *m_writer, "ssAbsCharSize" }
				, sdw::InVec2{ *m_writer, "ssCropRange" }
				, sdw::InVec2{ *m_writer, "texUvRange" }
				, sdw::InVec2{ *m_writer, "fontUvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "texUv" }
				, sdw::InOutFloat{ *m_writer, "fontUv" } );
		}

		m_cropMinMaxValue( pssAbsOvPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssAbsCharSize
			, pssCropRange
			, ptexUvRange
			, pfontUvRange
			, pssRelBound
			, ptexUv
			, pfontUv );
	}

	void OverlayData::cropMaxMinValue( sdw::Float const & pssAbsOvPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Float const & pssAbsCharSize
		, sdw::Vec2 const & pssCropRange
		, sdw::Vec2 const & ptexUvRange
		, sdw::Vec2 const & pfontUvRange
		, sdw::Float pssRelBound
		, sdw::Float ptexUv
		, sdw::Float pfontUv )
	{
		if ( !m_cropMaxMinValue )
		{
			m_cropMaxMinValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMaxMinValue"
				, [&]( sdw::Float const & ssAbsOvPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Float const & ssAbsCharSize
					, sdw::Vec2 const & ssCropRange
					, sdw::Vec2 const & texUvRange
					, sdw::Vec2 const & fontUvRange
					, sdw::Float ssRelBound
					, sdw::Float texUv
					, sdw::Float fontUv )
				{
					auto ssAbsBound = m_writer->declLocale( "ssAbsBound"
						, ssRelBound + ssAbsOvPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( ( ssAbsBound - ssCropRange.x() ) / ssAbsCharSize
							, ( ssAbsBound - ssCropRange.y() ) / ssAbsCharSize ) );

					IF( *m_writer, ssAbsBound > ssCropRange.y() )
					{
						ssRelBound = ssCropRange.y() - ssAbsOvPosition;
						texUv -= crop.y() * ( texUvRange.y() - texUvRange.x() );
						fontUv += crop.y() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					ELSEIF( ssAbsBound < ssCropRange.x() )
					{
						ssRelBound = ssCropRange.x() - ssAbsOvPosition;
						texUv += crop.x() * ( texUvRange.y() - texUvRange.x() );
						fontUv -= crop.x() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssAbsOvPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InFloat{ *m_writer, "ssAbsCharSize" }
				, sdw::InVec2{ *m_writer, "ssCropRange" }
				, sdw::InVec2{ *m_writer, "texUvRange" }
				, sdw::InVec2{ *m_writer, "fontUvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "texUv" }
				, sdw::InOutFloat{ *m_writer, "fontUv" } );
		}

		m_cropMaxMinValue( pssAbsOvPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssAbsCharSize
			, pssCropRange
			, ptexUvRange
			, pfontUvRange
			, pssRelBound
			, ptexUv
			, pfontUv );
	};

	void OverlayData::cropMaxMaxValue( sdw::Float const & pssAbsOvPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Float const & pssAbsCharSize
		, sdw::Vec2 const & pssCropRange
		, sdw::Vec2 const & ptexUvRange
		, sdw::Vec2 const & pfontUvRange
		, sdw::Float pssRelBound
		, sdw::Float ptexUv
		, sdw::Float pfontUv )
	{
		if ( !m_cropMaxMaxValue )
		{
			m_cropMaxMaxValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMaxMaxValue"
				, [&]( sdw::Float const & ssAbsOvPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Float const & ssAbsCharSize
					, sdw::Vec2 const & ssCropRange
					, sdw::Vec2 const & texUvRange
					, sdw::Vec2 const & fontUvRange
					, sdw::Float ssRelBound
					, sdw::Float texUv
					, sdw::Float fontUv )
				{
					auto ssAbsBound = m_writer->declLocale( "ssAbsBound"
						, ssRelBound + ssAbsOvPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( ( ssAbsBound - ssCropRange.x() ) / ssAbsCharSize
							, ( ssAbsBound - ssCropRange.y() ) / ssAbsCharSize ) );

					IF( *m_writer, ssAbsBound > ssCropRange.y() )
					{
						ssRelBound = ssCropRange.y() - ssAbsOvPosition;
						texUv -= crop.y() * ( texUvRange.y() - texUvRange.x() );
						fontUv -= crop.y() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					ELSEIF( ssAbsBound < ssCropRange.x() )
					{
						ssRelBound = ssCropRange.x() - ssAbsOvPosition;
						texUv += crop.x() * ( texUvRange.y() - texUvRange.x() );
						fontUv += crop.x() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssAbsOvPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InFloat{ *m_writer, "ssAbsCharSize" }
				, sdw::InVec2{ *m_writer, "ssCropRange" }
				, sdw::InVec2{ *m_writer, "texUvRange" }
				, sdw::InVec2{ *m_writer, "fontUvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "texUv" }
				, sdw::InOutFloat{ *m_writer, "fontUv" } );
		}

		m_cropMaxMaxValue( pssAbsOvPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssAbsCharSize
			, pssCropRange
			, ptexUvRange
			, pfontUvRange
			, pssRelBound
			, ptexUv
			, pfontUv );
	}
}
