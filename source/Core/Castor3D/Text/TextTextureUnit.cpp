#include "Castor3D/Text/TextTextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Text/TextRenderTarget.hpp"
#include "Castor3D/Text/TextTextureConfiguration.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< TextureUnit >::TextWriter( String const & tabs
		, PassTypeID type
		, bool isPbr
		, Path const & folder
		, String subFolder )
		: TextWriterT< TextureUnit >{ tabs, "TextureUnit" }
		, m_type{ type }
		, m_isPbr{ isPbr }
		, m_folder{ folder }
		, m_subFolder{ subFolder }
	{
	}

	bool TextWriter< TextureUnit >::operator()( TextureUnit const & unit
		, StringStream & file )
	{
		bool result = true;
		auto hasTexture = unit.isTextured() && unit.getTexture();

		if ( hasTexture )
		{
			log::info << tabs() << cuT( "Writing TextureUnit" ) << std::endl;
			auto texture = unit.getTexture();
			auto image = texture->getPath();
			hasTexture = !image.empty() || !texture->isStatic();
			bool createImageFile = false;
			auto config = unit.getConfiguration();

			if ( !hasTexture
				&& texture->isStatic() )
			{
				hasTexture = true;
				createImageFile = true;
				image = Path{ Path{ texture->getName() }.getFileName() + cuT( ".dds" ) };

				if ( Path{ texture->getName() }.getExtension() != "dds" )
				{
					config.needsYInversion = 1u - config.needsYInversion;
				}
			}

			if ( hasTexture )
			{
				if ( auto block{ beginBlock( file, cuT( "texture_unit" ) ) } )
				{
					if ( unit.getSampler().lock() && unit.getSampler().lock()->getName() != cuT( "Default" ) )
					{
						result = writeName( file, cuT( "sampler" ), unit.getSampler().lock()->getName() );
					}

					auto dimensions = unit.getTexture()->getDimensions();

					if ( result
						&& unit.getTexture()->getMipmapCount() > 1
						&& unit.getTexture()->getMipmapCount() != ashes::getMaxMipCount( dimensions ) )
					{
						result = write( file, cuT( "levels_count" ), unit.getTexture()->getMipmapCount() );
					}

					if ( result )
					{
						if ( !texture->isStatic() )
						{
							if ( unit.getRenderTarget() )
							{
								result = writeSub( file, *unit.getRenderTarget() );
							}
							else
							{
								// Procedurally generated textures, certainly will go here
							}
						}
						else
						{
							if ( createImageFile )
							{
								log::info << tabs() << cuT( "\tCreating texture image" ) << std::endl;
								castor::Path path{ cuT( "Textures" ) };

								if ( !m_subFolder.empty() )
								{
									path /= m_subFolder;
								}

								if ( !File::directoryExists( m_folder / path ) )
								{
									File::directoryCreate( m_folder / path );
								}

								path /= image;
								auto & writer = texture->getOwner()->getEngine()->getImageWriter();
								result = writer.write( m_folder / path, texture->getImage().getPxBuffer() );
								checkError( result, "Image creation" );

								if ( result )
								{
									result = writePath( file, cuT( "image" ), path );
								}
							}
							else
							{
								log::info << tabs() << cuT( "\tCopying texture image" ) << std::endl;

								if ( m_subFolder.empty() )
								{
									result = writeFile( file, cuT( "image" ), Path{ image }, m_folder, cuT( "Textures" ) );
								}
								else
								{
									result = writeFile( file, cuT( "image" ), Path{ image }, m_folder, String{ cuT( "Textures" ) } + Path::GenericSeparator + m_subFolder );
								}
							}
						}
					}

					if ( result )
					{
						result = writeSub( file, config, m_type, m_isPbr );
						checkError( result, "configuration" );
					}

					if ( result )
					{
						auto & transform = unit.getTransform();
						auto rotate = transform.rotate.degrees();
						auto translate = castor::Point3f{ transform.translate };
						auto scale = castor::Point3f{ transform.scale };

						if ( config.tileSet->z > 1 || config.tileSet->w > 1 )
						{
							result = writeNamedSub( file, cuT( "tile" ), castor::Point2ui{ config.tileSet } );
						}

						if ( translate != castor::Point3f{}
							|| rotate != 0.0f
							|| scale != castor::Point3f{ 1.0f, 1.0f, 1.0f } )
						{
							if ( auto animBlock{ beginBlock( file, "transform" ) } )
							{
								result = writeNamedSubOpt( file, cuT( "translate" ), translate, castor::Point3f{} )
									&& writeNamedSubOpt( file, cuT( "rotate" ), rotate, 0.0f )
									&& writeNamedSubOpt( file, cuT( "scale" ), scale, castor::Point3f{ 1.0f, 1.0f, 1.0f } );
							}
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
							|| translate != castor::Point2f{}
							|| rotate != 0.0f
							|| scale != castor::Point2f{} )
						{
							if ( auto animBlock{ beginBlock( file, "animation" ) } )
							{
								if ( anim.isTileAnimated() )
								{
									result = write( file, cuT( "tiles" ), anim.isTileAnimated() );
								}

								if ( translate != castor::Point2f{}
									|| rotate != 0.0f
									|| scale != castor::Point2f{} )
								{
									result = result
										&& writeNamedSubOpt( file, cuT( "translate" ), translate, castor::Point2f{} )
										&& writeNamedSubOpt( file, cuT( "rotate" ), rotate, 0.0f )
										&& writeNamedSubOpt( file, cuT( "scale" ), scale, castor::Point2f{} );
								}
							}
						}
					}
				}
			}
		}

		return result;
	}
}
