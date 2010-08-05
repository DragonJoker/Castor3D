#ifndef ___C3D_Image___
#define ___C3D_Image___

#include "Module_Material.h"

#include <CastorUtils/Manager.h>
#include <CastorUtils/AutoSingleton.h>
#include <CastorUtils/Resource.h>
#include <CastorUtils/ResourceLoader.h>

namespace Castor3D
{
	String FormatToString( PixelFormat p_fmt);

	template <PixelFormat SrcFmt, PixelFormat DestFmt>
	void ConvertPixel( const unsigned char * Src, unsigned char * Dest)
	{
		throw UnsupportedError( String( C3D_T( "Conversion software de format de pixel (")) +
								FormatToString(SrcFmt) + " -> " + FormatToString(DestFmt) + ")");
	}
	/*!
	Image loader from file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ImageLoader : ResourceLoader <Image>
	{
	public:
		Image * LoadFromFile( const String & p_file);
		void SaveToFile( const String & p_file)
		{
			LoaderError( "Le loader enregistré pour ce format ne prend pas en charge l'exportation");
		}
	};
	/*!
	Image resource
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Image : public Resource
	{
	protected:
		friend class ImageLoader;
		String m_path;
		Buffer<unsigned char> * m_buffer;
		unsigned int m_width;
		unsigned int m_height;

		/**
		* Constructor, needs the name and the image path
		*/
		Image( const String & p_path, Buffer <unsigned char> * p_buffer);

	public:
		/**
		* Destructor
		*/
		virtual ~Image();

		void Release();
		/**
		* Returns the resource type (image, sound, ...)
		*/
		virtual ResourceType GetResourceType()const { return rtImage; }

	public:
		inline unsigned int		GetWidth	()const { return m_width; }
		inline unsigned int		GetHeight	()const { return m_height; }
		inline String			GetPath		()const { return m_path; }
		inline unsigned char *	GetBuffer	()const { return m_buffer->GetBuffer(); }

		inline void SetWidth	( unsigned int p_w) { m_width = p_w; }
		inline void SetHeight	( unsigned int p_h) { m_height = p_h; }
	};
	/*!
	Manages all the images loaded in the engine.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ImageManager : public General::Templates::Manager<Image>, public General::Theory::AutoSingleton<ImageManager>
	{
	public:
		/**
		* Constructor
		*/
		ImageManager(){}
		/**
		* Destructor
		*/
		~ImageManager(){}
		/**
		* Creates an image with the given path and the given anem
		*@param p_name : [in] The resource name
		*@param p_path : [in] The image path
		*@return The created image
		*/
		Image * CreateImage( const String & p_name, const String & p_path);
	};
}

#endif