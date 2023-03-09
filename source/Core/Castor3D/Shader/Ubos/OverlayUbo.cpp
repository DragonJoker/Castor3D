#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

namespace castor3d::shader
{
	void OverlayData::cropMinValue( sdw::Float const & prelativePosition
		, sdw::Float const & pparentSize
		, sdw::Float const & pboundSize
		, sdw::Vec2 const & puvRange
		, sdw::Float pbound
		, sdw::Float puv )
	{
		if ( !m_cropMinValue )
		{
			m_cropMinValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMinValue"
				, [&]( sdw::Float const & relativePosition
					, sdw::Float const & parentSize
					, sdw::Float const & boundSize
					, sdw::Vec2 const & uvRange
					, sdw::Float bound
					, sdw::Float uv )
				{
					auto relBound = m_writer->declLocale( "relBound"
						, bound + relativePosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( relBound / boundSize
							, ( relBound - parentSize ) / boundSize ) );

					IF( *m_writer, relBound > parentSize )
					{
						bound = parentSize - relativePosition;
						uv += crop.y() * ( uvRange.y() - uvRange.x() );
						uv = clamp( uv, uvRange.x(), uvRange.y() );
					}
					ELSEIF( relBound < 0.0_f )
					{
						bound = 0.0_f - relativePosition;
						uv -= crop.x() * ( uvRange.y() - uvRange.x() );
						uv = clamp( uv, uvRange.x(), uvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "relativePosition" }
				, sdw::InFloat{ *m_writer, "parentSize" }
				, sdw::InFloat{ *m_writer, "boundSize" }
				, sdw::InVec2{ *m_writer, "uvRange" }
				, sdw::InOutFloat{ *m_writer, "bound" }
				, sdw::InOutFloat{ *m_writer, "uv" } );
		}

		m_cropMinValue( prelativePosition
			, pparentSize
			, pboundSize
			, puvRange
			, pbound
			, puv );
	}

	void OverlayData::cropMaxValue( sdw::Float const & prelativePosition
		, sdw::Float const & pparentSize
		, sdw::Float const & pboundSize
		, sdw::Vec2 const & puvRange
		, sdw::Float pbound
		, sdw::Float puv )
	{
		if ( !m_cropMaxValue )
		{
			m_cropMaxValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMaxValue"
				, [&]( sdw::Float const & relativePosition
					, sdw::Float const & parentSize
					, sdw::Float const & boundSize
					, sdw::Vec2 const & uvRange
					, sdw::Float bound
					, sdw::Float uv )
				{
					auto relBound = m_writer->declLocale( "relBound"
						, bound + relativePosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( relBound / boundSize
							, ( relBound - parentSize ) / boundSize ) );

					IF( *m_writer, relBound > parentSize )
					{
						bound = parentSize - relativePosition;
						uv -= crop.y() * ( uvRange.y() - uvRange.x() );
						uv = clamp( uv, uvRange.x(), uvRange.y() );
					}
					ELSEIF( relBound < 0.0_f )
					{
						bound = 0.0_f - relativePosition;
						uv += crop.x() * ( uvRange.y() - uvRange.x() );
						uv = clamp( uv, uvRange.x(), uvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "relativePosition" }
				, sdw::InFloat{ *m_writer, "parentSize" }
				, sdw::InFloat{ *m_writer, "boundSize" }
				, sdw::InVec2{ *m_writer, "uvRange" }
				, sdw::InOutFloat{ *m_writer, "bound" }
				, sdw::InOutFloat{ *m_writer, "uv" } );
		}

		m_cropMaxValue( prelativePosition
			, pparentSize
			, pboundSize
			, puvRange
			, pbound
			, puv );
	}

	void OverlayData::cropMinMinValue( sdw::Float const & pssRelPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Float const & pssAbsCharSize
		, sdw::Vec2 const & ptexUvRange
		, sdw::Vec2 const & pfontUvRange
		, sdw::Float pssRelBound
		, sdw::Float ptexUv
		, sdw::Float pfontUv )
	{
		if ( !m_cropMinMinValue )
		{
			m_cropMinMinValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMinMinValue"
				, [&]( sdw::Float const & ssRelPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Float const & ssAbsCharSize
					, sdw::Vec2 const & texUvRange
					, sdw::Vec2 const & fontUvRange
					, sdw::Float ssRelBound
					, sdw::Float texUv
					, sdw::Float fontUv )
				{
					auto relBound = m_writer->declLocale( "relBound"
						, ssRelBound + ssRelPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( relBound / ssAbsCharSize
							, ( relBound - ssAbsParentSize ) / ssAbsCharSize ) );

					IF( *m_writer, relBound > ssAbsParentSize )
					{
						ssRelBound = ssAbsParentSize - ssRelPosition;
						texUv += crop.y() * ( texUvRange.y() - texUvRange.x() );
						fontUv += crop.y() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					ELSEIF( relBound < 0.0_f )
					{
						ssRelBound = 0.0_f - ssRelPosition;
						texUv -= crop.x() * ( texUvRange.y() - texUvRange.x() );
						fontUv -= crop.x() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssRelPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InFloat{ *m_writer, "ssAbsCharSize" }
				, sdw::InVec2{ *m_writer, "texUvRange" }
				, sdw::InVec2{ *m_writer, "fontUvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "texUv" }
				, sdw::InOutFloat{ *m_writer, "fontUv" } );
		}

		m_cropMinMinValue( pssRelPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssAbsCharSize
			, ptexUvRange
			, pfontUvRange
			, pssRelBound
			, ptexUv
			, pfontUv );
	};

	void OverlayData::cropMinMaxValue( sdw::Float const & pssRelPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Float const & pssAbsCharSize
		, sdw::Vec2 const & ptexUvRange
		, sdw::Vec2 const & pfontUvRange
		, sdw::Float pssRelBound
		, sdw::Float ptexUv
		, sdw::Float pfontUv )
	{
		if ( !m_cropMinMaxValue )
		{
			m_cropMinMaxValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMinMaxValue"
				, [&]( sdw::Float const & ssRelPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Float const & ssAbsCharSize
					, sdw::Vec2 const & texUvRange
					, sdw::Vec2 const & fontUvRange
					, sdw::Float ssRelBound
					, sdw::Float texUv
					, sdw::Float fontUv )
				{
					auto relBound = m_writer->declLocale( "relBound"
						, ssRelBound + ssRelPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( relBound / ssAbsCharSize
							, ( relBound - ssAbsParentSize ) / ssAbsCharSize ) );

					IF( *m_writer, relBound > ssAbsParentSize )
					{
						ssRelBound = ssAbsParentSize - ssRelPosition;
						texUv += crop.y() * ( texUvRange.y() - texUvRange.x() );
						fontUv -= crop.y() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					ELSEIF( relBound < 0.0_f )
					{
						ssRelBound = 0.0_f - ssRelPosition;
						texUv -= crop.x() * ( texUvRange.y() - texUvRange.x() );
						fontUv += crop.x() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssRelPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InFloat{ *m_writer, "ssAbsCharSize" }
				, sdw::InVec2{ *m_writer, "texUvRange" }
				, sdw::InVec2{ *m_writer, "fontUvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "texUv" }
				, sdw::InOutFloat{ *m_writer, "fontUv" } );
		}

		m_cropMinMaxValue( pssRelPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssAbsCharSize
			, ptexUvRange
			, pfontUvRange
			, pssRelBound
			, ptexUv
			, pfontUv );
	}

	void OverlayData::cropMaxMinValue( sdw::Float const & pssRelPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Float const & pssAbsCharSize
		, sdw::Vec2 const & ptexUvRange
		, sdw::Vec2 const & pfontUvRange
		, sdw::Float pssRelBound
		, sdw::Float ptexUv
		, sdw::Float pfontUv )
	{
		if ( !m_cropMaxMinValue )
		{
			m_cropMaxMinValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMaxMinValue"
				, [&]( sdw::Float const & ssRelPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Float const & ssAbsCharSize
					, sdw::Vec2 const & texUvRange
					, sdw::Vec2 const & fontUvRange
					, sdw::Float ssRelBound
					, sdw::Float texUv
					, sdw::Float fontUv )
				{
					auto relBound = m_writer->declLocale( "relBound"
						, ssRelBound + ssRelPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( relBound / ssAbsCharSize
							, ( relBound - ssAbsParentSize ) / ssAbsCharSize ) );

					IF( *m_writer, relBound > ssAbsParentSize )
					{
						ssRelBound = ssAbsParentSize - ssRelPosition;
						texUv -= crop.y() * ( texUvRange.y() - texUvRange.x() );
						fontUv += crop.y() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					ELSEIF( relBound < 0.0_f )
					{
						ssRelBound = 0.0_f - ssRelPosition;
						texUv += crop.x() * ( texUvRange.y() - texUvRange.x() );
						fontUv -= crop.x() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssRelPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InFloat{ *m_writer, "ssAbsCharSize" }
				, sdw::InVec2{ *m_writer, "texUvRange" }
				, sdw::InVec2{ *m_writer, "fontUvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "texUv" }
				, sdw::InOutFloat{ *m_writer, "fontUv" } );
		}

		m_cropMaxMinValue( pssRelPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssAbsCharSize
			, ptexUvRange
			, pfontUvRange
			, pssRelBound
			, ptexUv
			, pfontUv );
	};

	void OverlayData::cropMaxMaxValue( sdw::Float const & pssRelPosition
		, sdw::Float const & pssAbsParentSize
		, sdw::Float const & pssAbsBoundSize
		, sdw::Float const & pssAbsCharSize
		, sdw::Vec2 const & ptexUvRange
		, sdw::Vec2 const & pfontUvRange
		, sdw::Float pssRelBound
		, sdw::Float ptexUv
		, sdw::Float pfontUv )
	{
		if ( !m_cropMaxMaxValue )
		{
			m_cropMaxMaxValue = m_writer->implementFunction< sdw::Void >( "c3d_cropMaxMaxValue"
				, [&]( sdw::Float const & ssRelPosition
					, sdw::Float const & ssAbsParentSize
					, sdw::Float const & ssAbsBoundSize
					, sdw::Float const & ssAbsCharSize
					, sdw::Vec2 const & texUvRange
					, sdw::Vec2 const & fontUvRange
					, sdw::Float ssRelBound
					, sdw::Float texUv
					, sdw::Float fontUv )
				{
					auto relBound = m_writer->declLocale( "relBound"
						, ssRelBound + ssRelPosition );
					auto crop = m_writer->declLocale( "crop"
						, vec2( relBound / ssAbsCharSize
							, ( relBound - ssAbsParentSize ) / ssAbsCharSize ) );

					IF( *m_writer, relBound > ssAbsParentSize )
					{
						ssRelBound = ssAbsParentSize - ssRelPosition;
						texUv -= crop.y() * ( texUvRange.y() - texUvRange.x() );
						fontUv -= crop.y() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					ELSEIF( relBound < 0.0_f )
					{
						ssRelBound = 0.0_f - ssRelPosition;
						texUv += crop.x() * ( texUvRange.y() - texUvRange.x() );
						fontUv += crop.x() * ( fontUvRange.y() - fontUvRange.x() );
						texUv = clamp( texUv, texUvRange.x(), texUvRange.y() );
						fontUv = clamp( fontUv, fontUvRange.x(), fontUvRange.y() );
					}
					FI;
				}
				, sdw::InFloat{ *m_writer, "ssRelPosition" }
				, sdw::InFloat{ *m_writer, "ssAbsParentSize" }
				, sdw::InFloat{ *m_writer, "ssAbsBoundSize" }
				, sdw::InFloat{ *m_writer, "ssAbsCharSize" }
				, sdw::InVec2{ *m_writer, "texUvRange" }
				, sdw::InVec2{ *m_writer, "fontUvRange" }
				, sdw::InOutFloat{ *m_writer, "ssRelBound" }
				, sdw::InOutFloat{ *m_writer, "texUv" }
				, sdw::InOutFloat{ *m_writer, "fontUv" } );
		}

		m_cropMaxMaxValue( pssRelPosition
			, pssAbsParentSize
			, pssAbsBoundSize
			, pssAbsCharSize
			, ptexUvRange
			, pfontUvRange
			, pssRelBound
			, ptexUv
			, pfontUv );
	}
}