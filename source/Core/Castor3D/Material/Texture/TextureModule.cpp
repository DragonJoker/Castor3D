#include "Castor3D/Material/Texture/TextureModule.hpp"

#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

CU_ImplementCUSmartPtr( castor3d, SamplerCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::Sampler, castor::String >::Name = cuT( "Sampler" );

	castor::String getName( TextureSpace value )
	{
		switch ( value )
		{
		case castor3d::TextureSpace::eNormalised:
			return cuT( "Normalised" );
		case castor3d::TextureSpace::eYInverted:
			return cuT( "YInverted" );
		case castor3d::TextureSpace::eColour:
			return cuT( "Colour" );
		case castor3d::TextureSpace::eDepth:
			return cuT( "Depth" );
		case castor3d::TextureSpace::eTangentSpace:
			return cuT( "TangentSpace" );
		case castor3d::TextureSpace::eObjectSpace:
			return cuT( "ObjectSpace" );
		case castor3d::TextureSpace::eWorldSpace:
			return cuT( "WorldSpace" );
		case castor3d::TextureSpace::eViewSpace:
			return cuT( "ViewSpace" );
		case castor3d::TextureSpace::eClipSpace:
			return cuT( "ClipSpace" );
		case castor3d::TextureSpace::eStencil:
			return cuT( "Stencil" );
		default:
			CU_Failure( "Unsupported TextureSpace" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( TextureSpaces values )
	{
		castor::String result;
		castor::String sep;

		for ( auto value : values )
		{
			result += sep + getName( value );
			sep = "|";
		}

		return result;
	}

	castor::String getName( CubeMapFace value )
	{
		switch ( value )
		{
		case castor3d::CubeMapFace::ePositiveX:
			return cuT( "PositiveX" );
		case castor3d::CubeMapFace::eNegativeX:
			return cuT( "NegativeX" );
		case castor3d::CubeMapFace::ePositiveY:
			return cuT( "PositiveY" );
		case castor3d::CubeMapFace::eNegativeY:
			return cuT( "NegativeY" );
		case castor3d::CubeMapFace::ePositiveZ:
			return cuT( "PositiveZ" );
		case castor3d::CubeMapFace::eNegativeZ:
			return cuT( "NegativeZ" );
		default:
			CU_Failure( "Unsupported CubeMapFace" );
			return castor::cuEmptyString;
		}
	}

	std::ostream & operator<<( std::ostream & stream, TextureLayout const & rhs )
	{
		stream << ashes::getName( rhs.getType() )
			<< ", " << ashes::getName( rhs.getPixelFormat() )
			<< ", " << rhs.getDimensions().width
			<< "x" << rhs.getDimensions().height;

		if ( rhs.getDimensions().depth > 1 )
		{
			stream << ", " << rhs.getDimensions().depth << " slices";
		}
		else
		{
			stream << ", " << rhs.getLayersCount() << " layers";
		}

		stream << ", " << rhs.getMipmapCount() << " miplevels";
		return stream;
	}
}
