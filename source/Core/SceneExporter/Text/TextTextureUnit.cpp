#include "TextTextureUnit.hpp"

#include "TextRenderTarget.hpp"
#include "TextTextureConfiguration.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/Animation/TextureAnimation.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/File.hpp>

namespace c3d
{
	TextWriter< TextureUnit >::TextWriter( String const & tabs
		, Path const & folder
		, String subFolder )
		: TextWriterT< TextureUnit >{ tabs, cuT( "TextureUnit" ) }
		, m_folder{ folder }
		, m_subFolder{ c3d::move( subFolder ) }
	{
	}

	bool TextWriter< TextureUnit >::operator()( TextureUnit const & unit
		, StringStream & file )
	{
		bool result = true;

		if ( unit.isTextured() )
		{
			log::info << tabs() << cuT( "Writing TextureUnit" ) << std::endl;
			auto image = unit.getTexturePath();
			auto config = unit.getConfiguration();
			auto defaultSampler = &unit.getEngine()->getDefaultSampler()->getSampler();
			if ( auto block{ beginBlock( file, cuT( "texture_unit" ) ) } )
			{
				if ( unit.getSampler().getSampler() && defaultSampler != &unit.getSampler().getSampler() )
					result = writeName( file, cuT( "sampler" ), makeString( unit.getSampler().getName() ) );

				if ( result )
					result = result && writeName( file, cuT( "texture" ), unit.getTextureName() );

				auto dimensions = unit.getTextureDimensions();

				if ( auto format = unit.getTexturePixelFormat();
					result
					&& unit.getTextureMipmapCount() > 1
					&& unit.getTextureMipmapCount() < getMipLevels( dimensions, format ) )
					result = write( file, cuT( "levels_count" ), unit.getTextureMipmapCount() );

				if ( result )
					result = writeOpt( file, cuT( "texcoord_set" ), unit.getTexcoordSet(), 0u );

				if ( result )
				{
					auto const & transform = config.transform;
					auto rotate = transform.rotate.degrees();
					auto translate = Point3f{ transform.translate };
					auto scale = Point3f{ transform.scale };

					if ( config.tileSet->z > 1 || config.tileSet->w > 1 )
						result = writeNamedSub( file, cuT( "tile" ), Point2ui{ config.tileSet } );

					if ( translate != Point3f{}
						|| rotate != 0.0f
						|| scale != Point3f{ 1.0f, 1.0f, 1.0f } )
					{
						if ( auto animBlock{ beginBlock( file, cuT( "transform" ) ) } )
							result = writeNamedSubOpt( file, cuT( "translate" ), translate, Point3f{} )
								&& writeNamedSubOpt( file, cuT( "rotate" ), rotate, 0.0f )
								&& writeNamedSubOpt( file, cuT( "scale" ), scale, Point3f{ 1.0f, 1.0f, 1.0f } );
					}
				}

				if ( result
					&& unit.hasAnimation() )
				{
					auto & anim = unit.getAnimation();
					auto rotate = anim.getRotateSpeed().getValue().degrees();
					auto translate = anim.getTranslateSpeed().getValue();
					auto scale = anim.getScaleSpeed().getValue();

					if ( anim.isTileAnimated()
						|| translate != Point2f{}
						|| rotate != 0.0f
						|| scale != Point2f{} )
					{
						if ( auto animBlock{ beginBlock( file, cuT( "animation" ) ) } )
						{
							if ( anim.isTileAnimated() )
								result = write( file, cuT( "tiles" ), anim.isTileAnimated() );

							if ( translate != Point2f{}
								|| rotate != 0.0f
								|| scale != Point2f{} )
								result = result
									&& writeNamedSubOpt( file, cuT( "translate" ), translate, Point2f{} )
									&& writeNamedSubOpt( file, cuT( "rotate" ), rotate, 0.0f )
									&& writeNamedSubOpt( file, cuT( "scale" ), scale, Point2f{} );
						}
					}
				}
			}
		}

		return result;
	}
}
