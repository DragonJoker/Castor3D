#include "PrecompiledHeader.h"

#include "material/Image.h"

using namespace Castor3D;

//*********************************************************************************************

String FormatToString( PixelFormat p_fmt)
{
	switch( p_fmt)
	{
	case pxfL8 :
		return C3D_T( "8bit luminosity");
		break;
	case pxfA8L8:
		return C3D_T( "16 bits alpha and luminosity");
		break;
	case pxfA1R5G5B5:
		return C3D_T( "16 bits 1555 ARGB");
		break;
	case pxfR4G4B4A4:
		return C3D_T( "16 bits 4444 RGBA");
		break;
	case pxfR8G8B8:
		return C3D_T( "24 bits 888 RGB");
		break;
	case pxfR8G8B8A8:
		return C3D_T( "32 bits 8888 ARGB");
		break;
	case pxfDXTC1:
		return C3D_T( "DXT1 8 bits compressed format");
		break;
	case pxfDXTC3:
		return C3D_T( "DXT3 16 bits compressed format");
		break;
	case pxfDXTC5:
		return C3D_T( "DXT5 16 bits compressed format");
		break;
	default:
		return C3D_T( "Unknown format");
	}
}
/*
template <>
inline void ConvertPixel<pxfL8, pxfL8>(const unsigned char* Src, unsigned char* Dest)
{
	*Dest = *Src;
}

template <>
inline void ConvertPixel<pxfL8, pxfA8L8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = *Src;
	Dest[1] = 0xFF;
}

template <>
inline void ConvertPixel<pxfL8, pxfA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
{
	*reinterpret_cast<unsigned short*>(Dest) = 0x8000 |
		((*Src >> 3) << 10) |
		((*Src >> 3) <<  5) |
		((*Src >> 3) <<  0);
}

template <>
inline void ConvertPixel<pxfL8, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = (*Src & 0xF0) | (*Src >> 4);
	Dest[1] = 0xF0 | (*Src >> 4);
}

template <>
inline void ConvertPixel<pxfL8, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = *Src;
	Dest[1] = *Src;
	Dest[2] = *Src;
}

template <>
inline void ConvertPixel<pxfL8, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = *Src;
	Dest[1] = *Src;
	Dest[2] = *Src;
	Dest[3] = 0xFF;
}


////////////////////////////////////////////////////////////////
// Spécialisations pour les conversions pxfA8L8 -> ???
////////////////////////////////////////////////////////////////
template <>
inline void ConvertPixel<pxfA8L8, pxfL8>(const unsigned char* Src, unsigned char* Dest)
{
	*Dest = Src[0];
}

template <>
inline void ConvertPixel<pxfA8L8, pxfA8L8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[1];
}

template <>
inline void ConvertPixel<pxfA8L8, pxfA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
{
	*reinterpret_cast<unsigned short*>(Dest) = ((Src[1] >> 7) << 15) |
		((Src[0] >> 3) << 10) |
		((Src[0] >> 3) <<  5) |
		((Src[0] >> 3) <<  0);
}

template <>
inline void ConvertPixel<pxfA8L8, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = (Src[0] & 0xF0) | (Src[0] >> 4);
	Dest[1] = (Src[1] & 0xF0) | (Src[0] >> 4);
}

template <>
inline void ConvertPixel<pxfA8L8, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[0];
	Dest[2] = Src[0];
}

template <>
inline void ConvertPixel<pxfA8L8, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[0];
	Dest[2] = Src[0];
	Dest[3] = Src[1];
}


////////////////////////////////////////////////////////////////
// Spécialisations pour les conversions pxfA1R5G5B5 -> ???
////////////////////////////////////////////////////////////////
template <>
inline void ConvertPixel<pxfA1R5G5B5, pxfL8>(const unsigned char* Src, unsigned char* Dest)
{
	unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
	*Dest = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
		((Pix & 0x03E0) >> 2) * 0.59 +
		((Pix & 0x001F) << 3) * 0.11);
}

template <>
inline void ConvertPixel<pxfA1R5G5B5, pxfA8L8>(const unsigned char* Src, unsigned char* Dest)
{
	unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
	Dest[0] = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
		((Pix & 0x03E0) >> 2) * 0.59 +
		((Pix & 0x001F) << 3) * 0.11);
	Dest[1] = Src[1] & 0x8000 ? 0xFF : 0x00;
}

template <>
inline void ConvertPixel<pxfA1R5G5B5, pxfA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[1];
}

template <>
inline void ConvertPixel<pxfA1R5G5B5, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
{
	unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
	Dest[1] = (Pix & 0x8000 ? 0xF0 : 0x00) | ((Pix & 0x7C00) >> 11);
	Dest[0] = ((Pix & 0x03C0) >> 2) | ((Pix & 0x001F) >> 1);
}

template <>
inline void ConvertPixel<pxfA1R5G5B5, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
{
	unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
	Dest[2] = (Pix & 0x7C00) >> 7;
	Dest[1] = (Pix & 0x03E0) >> 2;
	Dest[0] = (Pix & 0x001F) << 3;
}

template <>
inline void ConvertPixel<pxfA1R5G5B5, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
{
	unsigned short Pix = *reinterpret_cast<const unsigned short*>(Src);
	Dest[3] = (Pix & 0x8000) >> 8;
	Dest[2] = (Pix & 0x7C00) >> 7;
	Dest[1] = (Pix & 0x03E0) >> 2;
	Dest[0] = (Pix & 0x001F) << 3;
}


////////////////////////////////////////////////////////////////
// Spécialisations pour les conversions pxfR4G4B4A4 -> ???
////////////////////////////////////////////////////////////////
template <>
inline void ConvertPixel<pxfR4G4B4A4, pxfL8>(const unsigned char* Src, unsigned char* Dest)
{
	*Dest = static_cast<unsigned char>(((Src[1] & 0x0F) << 4) * 0.30 +
		((Src[0] & 0xF0) >> 0) * 0.59 +
		((Src[0] & 0x0F) << 4) * 0.11);
}

template <>
inline void ConvertPixel<pxfR4G4B4A4, pxfA8L8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = static_cast<unsigned char>(((Src[1] & 0x0F) << 4) * 0.30 +
		((Src[0] & 0xF0) >> 0) * 0.59 +
		((Src[0] & 0x0F) << 4) * 0.11);
	Dest[1] = Src[1] & 0xF0;
}

template <>
inline void ConvertPixel<pxfR4G4B4A4, pxfA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
{
	*reinterpret_cast<unsigned short*>(Dest) = ((Src[1] & 0x80) <<  8) |
		((Src[1] & 0x0F) << 11) |
		((Src[0] & 0xF0) <<  2) |
		((Src[0] & 0x0F) <<  1);
}

template <>
inline void ConvertPixel<pxfR4G4B4A4, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[1];
}

template <>
inline void ConvertPixel<pxfR4G4B4A4, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = (Src[0] & 0x0F) << 4;
	Dest[1] = (Src[0] & 0xF0);
	Dest[2] = (Src[1] & 0x0F) << 4;
}

template <>
inline void ConvertPixel<pxfR4G4B4A4, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = (Src[0] & 0x0F) << 4;
	Dest[1] = (Src[0] & 0xF0);
	Dest[2] = (Src[1] & 0x0F) << 4;
	Dest[3] = (Src[1] & 0xF0);
}


////////////////////////////////////////////////////////////////
// Spécialisations pour les conversions pxfR8G8B8 -> ???
////////////////////////////////////////////////////////////////
template <>
inline void ConvertPixel<pxfR8G8B8, pxfL8>(const unsigned char* Src, unsigned char* Dest)
{
	*Dest = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
}

template <>
inline void ConvertPixel<pxfR8G8B8, pxfA8L8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
	Dest[1] = 0xFF;
}

template <>
inline void ConvertPixel<pxfR8G8B8, pxfA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
{
	*reinterpret_cast<unsigned short*>(Dest) = 0x8000 |
		((Src[2] >> 3) << 10) |
		((Src[1] >> 3) <<  5) |
		((Src[0] >> 3) <<  0);
}

template <>
inline void ConvertPixel<pxfR8G8B8, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = (Src[1] & 0xF0) | (Src[0] >> 4);
	Dest[1] = 0xF0 | (Src[2] >> 4);
}

template <>
inline void ConvertPixel<pxfR8G8B8, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[1];
	Dest[2] = Src[2];
}

template <>
inline void ConvertPixel<pxfR8G8B8, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[1];
	Dest[2] = Src[2];
	Dest[3] = 0xFF;
}


////////////////////////////////////////////////////////////////
// Spécialisations pour les conversions pxfR8G8B8A8 -> ???
////////////////////////////////////////////////////////////////
template <>
inline void ConvertPixel<pxfR8G8B8A8, pxfL8>(const unsigned char* Src, unsigned char* Dest)
{
	*Dest = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
}

template <>
inline void ConvertPixel<pxfR8G8B8A8, pxfA8L8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = static_cast<unsigned char>(Src[2] * 0.30 + Src[1] * 0.59 + Src[0] * 0.11);
	Dest[1] = Src[3];
}

template <>
inline void ConvertPixel<pxfR8G8B8A8, pxfA1R5G5B5>(const unsigned char* Src, unsigned char* Dest)
{
	*reinterpret_cast<unsigned short*>(Dest) = ((Src[3] >> 7) << 15) |
		((Src[2] >> 3) << 10) |
		((Src[1] >> 3) <<  5) |
		((Src[0] >> 3) <<  0);
}

template <>
inline void ConvertPixel<pxfR8G8B8A8, pxfR4G4B4A4>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = (Src[1] & 0xF0) | (Src[0] >> 4);
	Dest[1] = (Src[3] & 0xF0) | (Src[2] >> 4);
}

template <>
inline void ConvertPixel<pxfR8G8B8A8, pxfR8G8B8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[1];
	Dest[2] = Src[2];
}

template <>
inline void ConvertPixel<pxfR8G8B8A8, pxfR8G8B8A8>(const unsigned char* Src, unsigned char* Dest)
{
	Dest[0] = Src[0];
	Dest[1] = Src[1];
	Dest[2] = Src[2];
	Dest[3] = Src[3];
}
*/
//*********************************************************************************************

Image * ImageLoader :: LoadFromFile( const String & p_file)
{
	fipImage * l_image = new fipImage();
	if ( ! l_image->load( ToString( p_file).c_str()))
	{
		delete l_image;
		return NULL;
	}
	l_image->convertTo32Bits();
	unsigned int l_width = l_image->getWidth();
	unsigned int l_height = l_image->getHeight();

	Buffer <unsigned char> * l_buffer = new Buffer <unsigned char>();
	l_buffer->InitialiseBuffer( l_width * l_height * 4, 0);
	unsigned char * l_pixels = l_image->accessPixels();

	for (unsigned int l_pix = 0 ; l_pix < l_width * l_height ; l_pix++)
	{
		l_buffer->Add( l_pixels[l_pix * 4 + 2]);
		l_buffer->Add( l_pixels[l_pix * 4 + 1]);
		l_buffer->Add( l_pixels[l_pix * 4 + 0]);
		l_buffer->Add( l_pixels[l_pix * 4 + 3]);
	}
	delete l_image;

	Image * l_newImage = new Image( p_file, l_buffer);
	l_newImage->SetWidth( l_width);
	l_newImage->SetHeight( l_height);

	return l_newImage;
}

//*********************************************************************************************

Image :: Image( const String & p_path, Buffer <unsigned char> * p_buffer)
	:	Resource(),
		m_path( p_path),
		m_buffer( p_buffer)
{
}

Image :: ~Image()
{
	delete m_buffer;
}

void Image :: Release()
{
	if (*m_references == 1)
	{
		ImageManager::GetSingletonPtr()->RemoveElement( this);
	}
	Resource::Release();
}

//*********************************************************************************************

Image * ImageManager :: CreateImage( const String & p_name, const String & p_path)
{
	Image * l_image = GetElementByName( p_name);
	if (l_image != NULL)
	{
		l_image->Ref();
	}
	else
	{
		ImageLoader l_loader;
		l_image = l_loader.LoadFromFile( p_path);
		if (l_image != NULL)
		{
			l_image->SetName( p_name);
			AddElement( l_image);
		}
	}
	return l_image;
}

//*********************************************************************************************