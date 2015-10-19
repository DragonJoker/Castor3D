#include "ImageManager.hpp"

#include "Image.hpp"
#include "Logger.hpp"

#if defined( CreateFont )
#	undef CreateFont
#endif

namespace Castor
{
	ImageManager::BinaryLoader::BinaryLoader()
		: Loader< ImageManager, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	bool ImageManager::BinaryLoader::operator()( ImageManager & p_manager, BinaryFile & p_file )
	{
		return true;
	}

	//*********************************************************************************************

	ImageManager::ImageManager()
	{
	}

	ImageManager::~ImageManager()
	{
	}

	ImageSPtr ImageManager::create( String const & p_name, Path const & p_path )
	{
		auto l_lock = make_unique_lock( *this );
		ImageSPtr l_return;

		if ( Collection< Image, String >::has( p_name ) )
		{
			l_return = Collection< Image, String >::find( p_name );

			if ( !l_return->GetBuffer() )
			{
				Image::BinaryLoader()( *l_return, p_path );
			}
			else
			{
				Logger::LogWarning( cuT( "Trying to create an already existing image: " ) + p_name );
			}
		}
		else
		{
			String l_name = p_path.GetFileName() + cuT( "." ) + p_path.GetExtension();

			if ( File::FileExists( p_path ) )
			{
				l_return = std::make_shared< Image >( p_name, p_path );
				Collection< Image, String >::insert( p_name, l_return );
			}
			else
			{
				CASTOR_EXCEPTION( "Can't create the image [" + string::string_cast< char >( p_name ) + "], invalid path: " + string::string_cast< char >( p_path ) );
			}
		}

		return l_return;
	}

	ImageSPtr ImageManager::create( String const & p_name, Size const & p_size, ePIXEL_FORMAT p_format )
	{
		auto l_lock = make_unique_lock( *this );
		ImageSPtr l_return;

		if ( Collection< Image, String >::has( p_name ) )
		{
			l_return = Collection< Image, String >::find( p_name );

			if ( !l_return->GetBuffer() )
			{
				*l_return = std::move( Image( p_name, p_size, p_format ) );
			}
			else
			{
				Logger::LogWarning( cuT( "Trying to create an already existing image: " ) + p_name );
			}
		}
		else
		{
			l_return = std::make_shared< Image >( p_name, p_size, p_format );
			Collection< Image, String >::insert( p_name, l_return );
		}

		return l_return;
	}

	ImageSPtr ImageManager::get( String const & p_name )
	{
		auto l_lock = make_unique_lock( *this );
		ImageSPtr l_return = Collection< Image, String >::find( p_name );

		if ( !l_return )
		{
			Logger::LogWarning( cuT( "Trying to retrieve a non existing font : " ) + p_name );
		}

		return l_return;
	}
}
