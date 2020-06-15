#include "CastorUtils/Graphics/DdsImageLoader.hpp"

#include "CastorUtils/Data/BinaryFile.hpp"
#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/Rectangle.hpp"
#include "CastorUtils/Log/Logger.hpp"

namespace castor
{
	//************************************************************************************************

	namespace
	{
		inline constexpr uint32_t DDS_FourCC( uint32_t a, uint32_t b, uint32_t c, uint32_t d )
		{
			return a << 0 |
				b << 8 |
				c << 16 |
				d << 24;
		}

		constexpr uint32_t DDS_Magic = DDS_FourCC( 'D', 'D', 'S', ' ' );

		enum DXGI_FORMAT
		{
			DXGI_FORMAT_UNKNOWN = 0,
			DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
			DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
			DXGI_FORMAT_R32G32B32A32_UINT = 3,
			DXGI_FORMAT_R32G32B32A32_SINT = 4,
			DXGI_FORMAT_R32G32B32_TYPELESS = 5,
			DXGI_FORMAT_R32G32B32_FLOAT = 6,
			DXGI_FORMAT_R32G32B32_UINT = 7,
			DXGI_FORMAT_R32G32B32_SINT = 8,
			DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
			DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
			DXGI_FORMAT_R16G16B16A16_UNORM = 11,
			DXGI_FORMAT_R16G16B16A16_UINT = 12,
			DXGI_FORMAT_R16G16B16A16_SNORM = 13,
			DXGI_FORMAT_R16G16B16A16_SINT = 14,
			DXGI_FORMAT_R32G32_TYPELESS = 15,
			DXGI_FORMAT_R32G32_FLOAT = 16,
			DXGI_FORMAT_R32G32_UINT = 17,
			DXGI_FORMAT_R32G32_SINT = 18,
			DXGI_FORMAT_R32G8X24_TYPELESS = 19,
			DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
			DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
			DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
			DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
			DXGI_FORMAT_R10G10B10A2_UNORM = 24,
			DXGI_FORMAT_R10G10B10A2_UINT = 25,
			DXGI_FORMAT_R11G11B10_FLOAT = 26,
			DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
			DXGI_FORMAT_R8G8B8A8_UNORM = 28,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
			DXGI_FORMAT_R8G8B8A8_UINT = 30,
			DXGI_FORMAT_R8G8B8A8_SNORM = 31,
			DXGI_FORMAT_R8G8B8A8_SINT = 32,
			DXGI_FORMAT_R16G16_TYPELESS = 33,
			DXGI_FORMAT_R16G16_FLOAT = 34,
			DXGI_FORMAT_R16G16_UNORM = 35,
			DXGI_FORMAT_R16G16_UINT = 36,
			DXGI_FORMAT_R16G16_SNORM = 37,
			DXGI_FORMAT_R16G16_SINT = 38,
			DXGI_FORMAT_R32_TYPELESS = 39,
			DXGI_FORMAT_D32_FLOAT = 40,
			DXGI_FORMAT_R32_FLOAT = 41,
			DXGI_FORMAT_R32_UINT = 42,
			DXGI_FORMAT_R32_SINT = 43,
			DXGI_FORMAT_R24G8_TYPELESS = 44,
			DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
			DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
			DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
			DXGI_FORMAT_R8G8_TYPELESS = 48,
			DXGI_FORMAT_R8G8_UNORM = 49,
			DXGI_FORMAT_R8G8_UINT = 50,
			DXGI_FORMAT_R8G8_SNORM = 51,
			DXGI_FORMAT_R8G8_SINT = 52,
			DXGI_FORMAT_R16_TYPELESS = 53,
			DXGI_FORMAT_R16_FLOAT = 54,
			DXGI_FORMAT_D16_UNORM = 55,
			DXGI_FORMAT_R16_UNORM = 56,
			DXGI_FORMAT_R16_UINT = 57,
			DXGI_FORMAT_R16_SNORM = 58,
			DXGI_FORMAT_R16_SINT = 59,
			DXGI_FORMAT_R8_TYPELESS = 60,
			DXGI_FORMAT_R8_UNORM = 61,
			DXGI_FORMAT_R8_UINT = 62,
			DXGI_FORMAT_R8_SNORM = 63,
			DXGI_FORMAT_R8_SINT = 64,
			DXGI_FORMAT_A8_UNORM = 65,
			DXGI_FORMAT_R1_UNORM = 66,
			DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
			DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
			DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
			DXGI_FORMAT_BC1_TYPELESS = 70,
			DXGI_FORMAT_BC1_UNORM = 71,
			DXGI_FORMAT_BC1_UNORM_SRGB = 72,
			DXGI_FORMAT_BC2_TYPELESS = 73,
			DXGI_FORMAT_BC2_UNORM = 74,
			DXGI_FORMAT_BC2_UNORM_SRGB = 75,
			DXGI_FORMAT_BC3_TYPELESS = 76,
			DXGI_FORMAT_BC3_UNORM = 77,
			DXGI_FORMAT_BC3_UNORM_SRGB = 78,
			DXGI_FORMAT_BC4_TYPELESS = 79,
			DXGI_FORMAT_BC4_UNORM = 80,
			DXGI_FORMAT_BC4_SNORM = 81,
			DXGI_FORMAT_BC5_TYPELESS = 82,
			DXGI_FORMAT_BC5_UNORM = 83,
			DXGI_FORMAT_BC5_SNORM = 84,
			DXGI_FORMAT_B5G6R5_UNORM = 85,
			DXGI_FORMAT_B5G5R5A1_UNORM = 86,
			DXGI_FORMAT_B8G8R8A8_UNORM = 87,
			DXGI_FORMAT_B8G8R8X8_UNORM = 88,
			DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
			DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
			DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
			DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
			DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
			DXGI_FORMAT_BC6H_TYPELESS = 94,
			DXGI_FORMAT_BC6H_UF16 = 95,
			DXGI_FORMAT_BC6H_SF16 = 96,
			DXGI_FORMAT_BC7_TYPELESS = 97,
			DXGI_FORMAT_BC7_UNORM = 98,
			DXGI_FORMAT_BC7_UNORM_SRGB = 99
		};

		enum D3D10_RESOURCE_DIMENSION
		{
			D3D10_RESOURCE_DIMENSION_UNKNOWN = 0,
			D3D10_RESOURCE_DIMENSION_BUFFER = 1,
			D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
			D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
			D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4
		};

		enum D3DFMT
		{
			D3DFMT_UNKNOWN = 0,

			D3DFMT_R8G8B8 = 20,
			D3DFMT_A8R8G8B8 = 21,
			D3DFMT_X8R8G8B8 = 22,
			D3DFMT_R5G6B5 = 23,
			D3DFMT_X1R5G5B5 = 24,
			D3DFMT_A1R5G5B5 = 25,
			D3DFMT_A4R4G4B4 = 26,
			D3DFMT_R3G3B2 = 27,
			D3DFMT_A8 = 28,
			D3DFMT_A8R3G3B2 = 29,
			D3DFMT_X4R4G4B4 = 30,
			D3DFMT_A2B10G10R10 = 31,
			D3DFMT_A8B8G8R8 = 32,
			D3DFMT_X8B8G8R8 = 33,
			D3DFMT_G16R16 = 34,
			D3DFMT_A2R10G10B10 = 35,
			D3DFMT_A16B16G16R16 = 36,

			D3DFMT_A8P8 = 40,
			D3DFMT_P8 = 41,

			D3DFMT_L8 = 50,
			D3DFMT_A8L8 = 51,
			D3DFMT_A4L4 = 52,

			D3DFMT_V8U8 = 60,
			D3DFMT_L6V5U5 = 61,
			D3DFMT_X8L8V8U8 = 62,
			D3DFMT_Q8W8V8U8 = 63,
			D3DFMT_V16U16 = 64,
			D3DFMT_A2W10V10U10 = 67,

			D3DFMT_UYVY = DDS_FourCC( 'U', 'Y', 'V', 'Y' ),
			D3DFMT_R8G8_B8G8 = DDS_FourCC( 'R', 'G', 'B', 'G' ),
			D3DFMT_YUY2 = DDS_FourCC( 'Y', 'U', 'Y', '2' ),
			D3DFMT_G8R8_G8B8 = DDS_FourCC( 'G', 'R', 'G', 'B' ),
			D3DFMT_DXT1 = DDS_FourCC( 'D', 'X', 'T', '1' ),
			D3DFMT_DXT2 = DDS_FourCC( 'D', 'X', 'T', '2' ),
			D3DFMT_DXT3 = DDS_FourCC( 'D', 'X', 'T', '3' ),
			D3DFMT_DXT4 = DDS_FourCC( 'D', 'X', 'T', '4' ),
			D3DFMT_DXT5 = DDS_FourCC( 'D', 'X', 'T', '5' ),
			D3DFMT_ATI2 = DDS_FourCC( 'A', 'T', 'I', '2' ),

			D3DFMT_D16_LOCKABLE = 70,
			D3DFMT_D32 = 71,
			D3DFMT_D15S1 = 73,
			D3DFMT_D24S8 = 75,
			D3DFMT_D24X8 = 77,
			D3DFMT_D24X4S4 = 79,
			D3DFMT_D16 = 80,

			D3DFMT_D32F_LOCKABLE = 82,
			D3DFMT_D24FS8 = 83,

			D3DFMT_L16 = 81,

			D3DFMT_VERTEXDATA = 100,
			D3DFMT_INDEX16 = 101,
			D3DFMT_INDEX32 = 102,

			D3DFMT_Q16W16V16U16 = 110,

			D3DFMT_MULTI2_ARGB8 = DDS_FourCC( 'M', 'E', 'T', '1' ),

			D3DFMT_R16F = 111,
			D3DFMT_G16R16F = 112,
			D3DFMT_A16B16G16R16F = 113,

			D3DFMT_R32F = 114,
			D3DFMT_G32R32F = 115,
			D3DFMT_A32B32G32R32F = 116,

			D3DFMT_CxV8U8 = 117,

			D3DFMT_DX10 = DDS_FourCC( 'D', 'X', '1', '0' )
		};

		enum DDPF
		{
			DDPF_ALPHAPIXELS = 0x00001,
			DDPF_ALPHA = 0x00002,
			DDPF_FOURCC = 0x00004,
			DDPF_RGB = 0x00040,
			DDPF_YUV = 0x00200,
			DDPF_LUMINANCE = 0x20000
		};

		enum DDSD
		{
			DDSD_CAPS = 0x00000001,
			DDSD_HEIGHT = 0x00000002,
			DDSD_WIDTH = 0x00000004,
			DDSD_PITCH = 0x00000008,
			DDSD_PIXELFORMAT = 0x00001000,
			DDSD_MIPMAPCOUNT = 0x00020000,
			DDSD_LINEARSIZE = 0x00080000,
			DDSD_DEPTH = 0x00800000
		};

		enum DDSCAPS
		{
			DDSCAPS_COMPLEX = 0x00000008,
			DDSCAPS_MIPMAP = 0x00400000,
			DDSCAPS_TEXTURE = 0x00001000
		};

		enum DDSCAPS2
		{
			DDSCAPS2_CUBEMAP = 0x00000200,
			DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
			DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
			DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
			DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
			DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
			DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
			DDSCAPS2_VOLUME = 0x00200000,

			DDSCAPS2_CUBEMAP_ALLFACES = DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX |
			DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY |
			DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ
		};

		enum DDS_COLOR
		{
			DDS_COLOR_DEFAULT = 0,
			DDS_COLOR_DISTANCE,
			DDS_COLOR_LUMINANCE,
			DDS_COLOR_INSET_BBOX,
			DDS_COLOR_MAX
		};

		enum DDS_COMPRESS
		{
			DDS_COMPRESS_NONE = 0,
			DDS_COMPRESS_BC1,        /* DXT1  */
			DDS_COMPRESS_BC2,        /* DXT3  */
			DDS_COMPRESS_BC3,        /* DXT5  */
			DDS_COMPRESS_BC3N,       /* DXT5n */
			DDS_COMPRESS_BC4,        /* ATI1  */
			DDS_COMPRESS_BC5,        /* ATI2  */
			DDS_COMPRESS_AEXP,       /* DXT5  */
			DDS_COMPRESS_YCOCG,      /* DXT5  */
			DDS_COMPRESS_YCOCGS,     /* DXT5  */
			DDS_COMPRESS_MAX
		};

		enum DDS_FORMAT
		{
			DDS_FORMAT_DEFAULT = 0,
			DDS_FORMAT_RGB8,
			DDS_FORMAT_RGBA8,
			DDS_FORMAT_BGR8,
			DDS_FORMAT_ABGR8,
			DDS_FORMAT_R5G6B5,
			DDS_FORMAT_RGBA4,
			DDS_FORMAT_RGB5A1,
			DDS_FORMAT_RGB10A2,
			DDS_FORMAT_R3G3B2,
			DDS_FORMAT_A8,
			DDS_FORMAT_L8,
			DDS_FORMAT_L8A8,
			DDS_FORMAT_AEXP,
			DDS_FORMAT_YCOCG,
			DDS_FORMAT_MAX
		};

		enum DDS_MIPMAP
		{
			DDS_MIPMAP_DEFAULT = 0,
			DDS_MIPMAP_NEAREST,
			DDS_MIPMAP_BOX,
			DDS_MIPMAP_BILINEAR,
			DDS_MIPMAP_BICUBIC,
			DDS_MIPMAP_LANCZOS,
			DDS_MIPMAP_MAX
		};

		enum DDS_SAVE
		{
			DDS_SAVE_SELECTED_LAYER = 0,
			DDS_SAVE_CUBEMAP,
			DDS_SAVE_VOLUMEMAP,
			DDS_SAVE_MAX
		};

		struct DDSPixelFormat // DDPIXELFORMAT
		{
			uint32_t size;
			uint32_t flags;
			uint32_t fourCC;
			uint32_t bpp;
			uint32_t redMask;
			uint32_t greenMask;
			uint32_t blueMask;
			uint32_t alphaMask;
		};

		struct DDSHeader
		{
			uint32_t size;
			uint32_t flags;
			uint32_t height;
			uint32_t width;
			uint32_t pitch;
			uint32_t depth;
			uint32_t levelCount;
			uint32_t reserved1[11];
			DDSPixelFormat format;
			uint32_t ddsCaps[4];
			uint32_t reserved2;
		};

		struct DDSHeaderDX10Ext
		{
			DXGI_FORMAT dxgiFormat;
			D3D10_RESOURCE_DIMENSION resourceDimension;
			uint32_t miscFlag;
			uint32_t arraySize;
			uint32_t reserved;
		};

		bool preMultiplyWithAlpha( PxBufferBaseSPtr pixelBuffer )
		{
			struct Pixel
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
			int width = pixelBuffer->getWidth();
			int height = pixelBuffer->getHeight();
			auto buffer = reinterpret_cast< Pixel * >( pixelBuffer->getPtr() );

			for ( int y = 0; y < height; ++y )
			{
				for ( int x = 0; x < width; ++x )
				{
					const uint8_t alpha = buffer->a;

					// slightly faster: care for two special cases
					if ( alpha == 0x00 )
					{
						// special case for alpha == 0x00
						// color * 0x00 / 0xFF = 0x00
						buffer->r = 0x00;
						buffer->g = 0x00;
						buffer->b = 0x00;
					}
					else if ( alpha == 0xFF )
					{
						// nothing to do for alpha == 0xFF
						// color * 0xFF / 0xFF = color
						continue;
					}
					else
					{
						buffer->r = uint8_t( ( alpha * uint16_t( buffer->r ) + 127 ) / 255 );
						buffer->g = uint8_t( ( alpha * uint16_t( buffer->g ) + 127 ) / 255 );
						buffer->b = uint8_t( ( alpha * uint16_t( buffer->b ) + 127 ) / 255 );
					}

					++buffer;
				}
			}

			return true;
		}

		std::istream & operator>>( std::istream & stream, DDSPixelFormat & pixelFormat )
		{
			stream >> pixelFormat.size;
			stream >> pixelFormat.flags;
			stream >> pixelFormat.fourCC;
			stream >> pixelFormat.bpp;
			stream >> pixelFormat.redMask;
			stream >> pixelFormat.greenMask;
			stream >> pixelFormat.blueMask;
			stream >> pixelFormat.alphaMask;
			return stream;
		}

		std::istream & operator>>( std::istream & stream, DDSHeader & header )
		{
			stream >> header.size;
			stream >> header.flags;
			stream >> header.height;
			stream >> header.width;
			stream >> header.pitch;
			stream >> header.depth;
			stream >> header.levelCount;

			for ( unsigned int i = 0; i < getCountOf( header.reserved1 ); ++i )
			{
				stream >> header.reserved1[i];
			}

			stream >> header.format;

			for ( unsigned int i = 0; i < getCountOf( header.ddsCaps ); ++i )
			{
				stream >> header.ddsCaps[i];
			}

			stream >> header.reserved2;

			return stream;
		}

		std::istream & operator>>( std::istream & stream, DDSHeaderDX10Ext & header )
		{
			uint32_t enumValue;

			stream >> enumValue;
			header.dxgiFormat = static_cast< DXGI_FORMAT >( enumValue );

			stream >> enumValue;
			header.resourceDimension = static_cast< D3D10_RESOURCE_DIMENSION >( enumValue );

			stream >> header.miscFlag;
			stream >> header.arraySize;
			stream >> header.reserved;

			return stream;
		}

		bool identifyImageType( DDSHeader const & header
			, DDSHeaderDX10Ext const & headerExt
			, castor::ImageLayout::Type & type )
		{
			if ( headerExt.arraySize > 1 )
			{
				if ( header.ddsCaps[1] & DDSCAPS2_CUBEMAP )
				{
					CU_LoaderError( "Can't load DDS image: Cubemap arrays are not yet supported, sorry" );
				}
				else if ( header.flags & DDSD_HEIGHT )
				{
					type = castor::ImageLayout::e2DArray;
				}
				else
				{
					type = castor::ImageLayout::e1DArray;
				}
			}
			else
			{
				if ( header.ddsCaps[1] & DDSCAPS2_CUBEMAP )
				{
					if ( ( header.ddsCaps[1] & DDSCAPS2_CUBEMAP_ALLFACES ) != DDSCAPS2_CUBEMAP_ALLFACES )
					{
						CU_LoaderError( "Can't load DDS image: Partial cubemap are not yet supported, sorry" );
					}

					type = castor::ImageLayout::eCube;
				}
				else if ( headerExt.resourceDimension == D3D10_RESOURCE_DIMENSION_BUFFER )
				{
					CU_LoaderError( "Can't load DDS image: Texture buffers are not yet supported, sorry" );
				}
				else if ( headerExt.resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE1D )
				{
					type = castor::ImageLayout::e1D;
				}
				else if ( header.ddsCaps[1] & DDSCAPS2_VOLUME || header.flags & DDSD_DEPTH || headerExt.resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE3D )
				{
					type = castor::ImageLayout::e3D;
				}
				else
				{
					type = castor::ImageLayout::e2D;
				}
			}

			return true;
		}

		struct PixelFormatInfo
		{
			uint32_t bpp;
			uint64_t redMask;
			uint64_t greenMask;
			uint64_t blueMask;
			uint64_t alphaMask;
		};

		PixelFormat identifyFormat( PixelFormatInfo const & info )
		{
			std::array< PixelFormatInfo, size_t( PixelFormat::eCount ) > pixelFormatInfos
			{
				/* eR8_UNORM            */PixelFormatInfo{   8u, 0x00000000000000FFull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eR16_SFLOAT          */PixelFormatInfo{  16u, 0x000000000000FFFFull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eR32_SFLOAT          */PixelFormatInfo{  32u, 0xFFFFFFFF00000000ull, 0x00000000FFFFFFFFull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eR8A8_UNORM          */PixelFormatInfo{  16u, 0x000000000000FF00ull, 0x00000000000000FFull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eR16A16_SFLOAT       */PixelFormatInfo{  32u, 0x00000000FFFF0000ull, 0x000000000000FFFFull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eR32A32_SFLOAT       */PixelFormatInfo{  64u, 0xFFFFFFFF00000000ull, 0x00000000FFFFFFFFull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eR5G5B5A1_UNORM      */PixelFormatInfo{  16u, 0x000000000000F800ull, 0x00000000000007C0ull, 0x000000000000003Eull, 0x0000000000000001ull },
				/* eR5G6B5_UNORM        */PixelFormatInfo{  16u, 0x000000000000F800ull, 0x00000000000007E0ull, 0x000000000000001Full, 0x0000000000000000ull },
				/* eR8G8B8_UNORM        */PixelFormatInfo{  24u, 0x00000000FF000000ull, 0x0000000000FF0000ull, 0x000000000000FF00ull, 0x0000000000000000ull },
				/* eB8G8R8_UNORM        */PixelFormatInfo{  24u, 0x00000000000000FFull, 0x000000000000FF00ull, 0x0000000000FF0000ull, 0x0000000000000000ull },
				/* eR8G8B8_SRGB         */PixelFormatInfo{  24u, 0x00000000FF000000ull, 0x0000000000FF0000ull, 0x000000000000FF00ull, 0x0000000000000000ull },
				/* eB8G8R8_SRGB         */PixelFormatInfo{  24u, 0x00000000000000FFull, 0x000000000000FF00ull, 0x0000000000FF0000ull, 0x0000000000000000ull },
				/* eR8G8B8A8_UNORM      */PixelFormatInfo{  32u, 0x00000000FF000000ull, 0x0000000000FF0000ull, 0x000000000000FF00ull, 0x00000000000000FFull },
				/* eA8B8G8R8_UNORM      */PixelFormatInfo{  32u, 0x00000000000000FFull, 0x000000000000FF00ull, 0x0000000000FF0000ull, 0x00000000FF000000ull },
				/* eR8G8B8A8_SRGB       */PixelFormatInfo{  32u, 0x00000000FF000000ull, 0x0000000000FF0000ull, 0x000000000000FF00ull, 0x00000000000000FFull },
				/* eA8B8G8R8_SRGB       */PixelFormatInfo{  32u, 0x00000000000000FFull, 0x000000000000FF00ull, 0x0000000000FF0000ull, 0x00000000FF000000ull },
				/* eR16G16B16_SFLOAT    */PixelFormatInfo{  64u, 0x0000FFFF00000000ull, 0x00000000FFFF0000ull, 0x000000000000FFFFull, 0x0000000000000000ull },
				/* eR16G16B16A16_SFLOAT */PixelFormatInfo{  64u, 0xFFFF000000000000ull, 0x0000FFFF00000000ull, 0x00000000FFFF0000ull, 0x000000000000FFFFull },
				/* eR32G32B32_SFLOAT    */PixelFormatInfo{ 128u, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eR32G32B32A32_SFLOAT */PixelFormatInfo{ 128u, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eBC1_RGB_UNORM_BLOCK */PixelFormatInfo{   8u, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eBC3_UNORM_BLOCK     */PixelFormatInfo{  16u, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eBC5_UNORM_BLOCK     */PixelFormatInfo{  16u, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eD16_UNORM           */PixelFormatInfo{  16u, 0x000000000000FFFFull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eD24_UNORM_S8_UINT   */PixelFormatInfo{  32u, 0x00000000FFFFFF00ull, 0x00000000000000FFull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eD32_UNORM           */PixelFormatInfo{  32u, 0x00000000FFFFFFFFull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eD32_SFLOAT          */PixelFormatInfo{  32u, 0x00000000FFFFFFFFull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eD32_SFLOAT_S8_UINT  */PixelFormatInfo{  40u, 0x000000FFFFFFFF00ull, 0x00000000000000FFull, 0x0000000000000000ull, 0x0000000000000000ull },
				/* eS8_UINT             */PixelFormatInfo{   8u, 0x00000000000000FFull, 0x0000000000000000ull, 0x0000000000000000ull, 0x0000000000000000ull },
			};

			for ( uint32_t i = 0u; i <= uint32_t( PixelFormat::eMax ); ++i )
			{
				PixelFormatInfo & info2 = pixelFormatInfos[i];

				if ( info.bpp == info2.bpp
					&& info.redMask == info2.redMask
					&& info.greenMask == info2.greenMask
					&& info.blueMask == info2.blueMask
					&& info.alphaMask == info2.alphaMask )
					return static_cast< PixelFormat >( i );
			}

			return PixelFormat( ~0u );
		}

		bool isValid( PixelFormat format )
		{
			return format != PixelFormat( ~0u );
		}

		bool identifyPixelFormat( DDSHeader const & header
			, DDSHeaderDX10Ext const & headerExt
			, PixelFormat & format )
		{
			if ( header.format.flags & ( DDPF_RGB | DDPF_ALPHA | DDPF_ALPHAPIXELS | DDPF_LUMINANCE ) )
			{
				PixelFormatInfo info
				{
					header.format.bpp,
				};

				if ( header.format.flags & DDPF_RGB )
				{
					// Reverse bits for our masks
					info.redMask = header.format.redMask;
					info.greenMask = header.format.greenMask;
					info.blueMask = header.format.blueMask;
				}
				else if ( header.format.flags & DDPF_LUMINANCE )
				{
					info.redMask = header.format.redMask;
				}

				if ( header.format.flags & ( DDPF_ALPHA | DDPF_ALPHAPIXELS ) )
				{
					info.alphaMask = header.format.alphaMask;
				}

				format = identifyFormat( info );

				if ( !isValid( format ) )
				{
					return false;
				}
			}
			else if ( header.format.flags & DDPF_FOURCC )
			{
				switch ( header.format.fourCC )
				{
				case D3DFMT_DXT1:
					format = PixelFormat::eBC1_RGB_UNORM_BLOCK;
					break;

				case D3DFMT_DXT3:
					format = PixelFormat::eBC3_UNORM_BLOCK;
					break;

				case D3DFMT_DXT5:
				case D3DFMT_ATI2:
					format = PixelFormat::eBC5_UNORM_BLOCK;
					break;

				case D3DFMT_DX10:
					{
						switch ( headerExt.dxgiFormat )
						{
						case DXGI_FORMAT_R32G32B32A32_FLOAT:
							format = PixelFormat::eR32G32B32A32_SFLOAT;
							break;
						//case DXGI_FORMAT_R32G32B32A32_UINT:
						//	format = PixelFormat::eR32G32B32A32_UINT;
						//	break;
						//case DXGI_FORMAT_R32G32B32A32_SINT:
						//	format = PixelFormat::eR32G32B32A32_SINT;
						//	break;
						case DXGI_FORMAT_R32G32B32_FLOAT:
							format = PixelFormat::eR32G32B32_SFLOAT;
							break;
						//case DXGI_FORMAT_R32G32B32_UINT:
						//	format = PixelFormat::eR32G32B32_UINT;
						//	return false;
						//case DXGI_FORMAT_R32G32B32_SINT:
						//	format = PixelFormat::eR32G32B32_SINT;
						//	break;
						//case DXGI_FORMAT_R16G16B16A16_UNORM:
						//	format = PixelFormat::eR16G16BA16_UNORM;
						//	break;
						//case DXGI_FORMAT_R16G16B16A16_SNORM:
						//	format = PixelFormat::eR16G16BA16_SNORM;
						//	break;
						//case DXGI_FORMAT_R16G16B16A16_SINT:
						//	format = PixelFormat::eR16G16BA16_SINT;
						//	break;
						//case DXGI_FORMAT_R16G16B16A16_UINT:
						//	format = PixelFormat::eR16G16BA16_UINT;
						//	break;
						default:
							break;
						}
						break;
					}

				default:
					{
						char buf[5];
						buf[0] = ( header.format.fourCC >> 0 ) & 255;
						buf[1] = ( header.format.fourCC >> 8 ) & 255;
						buf[2] = ( header.format.fourCC >> 16 ) & 255;
						buf[3] = ( header.format.fourCC >> 24 ) & 255;
						buf[4] = '\0';

						CU_LoaderError( "Can't load DDS image: Unhandled format \"" + String( buf ) + "\"" );
					}
				}
			}
			else
			{
				CU_LoaderError( "Can't load DDS image: Invalid file" );
			}

			return true;
		}

		StringArray const & listExtensions()
		{
			static StringArray const list
			{
				cuT( "dds" ),
			};
			return list;
		}

		template< typename T >
		void read( uint8_t const *& data
			, T & value )
		{
			value = *reinterpret_cast< T const * >( data );
			data += sizeof( T );
		}

		uint32_t readUInt32( uint8_t const *& data )
		{
			uint32_t result = 0u;
			read( data, result );
			return result;
		}
	}

	//************************************************************************************************

	void DdsImageLoader::registerLoader( ImageLoader & reg )
	{
		//reg.registerLoader( listExtensions()
			//, std::make_unique< DdsImageLoader >() );
	}

	void DdsImageLoader::unregisterLoader( ImageLoader & reg )
	{
		//reg.unregisterLoader( listExtensions() );
	}

	ImageLayout DdsImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseSPtr & buffer )const
	{
		auto end = data + size;
		uint32_t magic;
		read( data, magic );
		CU_Assert( magic == DDS_Magic, "Can't load DDS image: Invalid file" ); // The Check function should make sure this doesn't happen

		DDSHeader header;
		read( data, header );

		DDSHeaderDX10Ext headerDX10;
		if ( header.format.flags & DDPF_FOURCC && header.format.fourCC == D3DFMT_DX10 )
		{
			read( data, headerDX10 );
		}
		else
		{
			headerDX10.arraySize = 1;
			headerDX10.dxgiFormat = DXGI_FORMAT_UNKNOWN;
			headerDX10.miscFlag = 0;
			headerDX10.resourceDimension = D3D10_RESOURCE_DIMENSION_UNKNOWN;
		}

		if ( ( header.flags & DDSD_WIDTH ) == 0 )
		{
			Logger::logWarning( "Can't load DDS image: Ill-formed file, doesn't have a width flag" );
		}

		castor::ImageLayout result;
		result.extent->x = std::max( header.width, 1U );

		if ( header.flags & DDSD_HEIGHT )
		{
			result.extent->y = std::max( header.height, 1U );
		}

		if ( header.flags & DDSD_DEPTH )
		{
			result.extent->z = std::max( header.depth, 1U );
		}

		result.levels = header.levelCount;

		// First, identify the type
		if ( !identifyImageType( header, headerDX10, result.type ) )
		{
			return {};
		}

		// Then the format
		if ( !identifyPixelFormat( header, headerDX10, result.format ) )
		{
			return {};
		}

		// Read all mipmap levels
		auto width = result.extent->x;
		auto height = result.extent->y;
		auto depth = result.extent->z;
		buffer = PxBufferBase::create( result.dimensions(), result.depthLayers(), result.levels, result.format );
		buffer->flip();

		for ( uint32_t layer = 0u; layer < result.depthLayers(); layer++ )
		{
			width = result.extent->x;
			height = result.extent->y;
			depth = result.extent->z;

			for ( uint32_t level = result.baseLevel; level < result.baseLevel + result.levels; level++ )
			{
				if ( width >= 4u && width >= 4u )
				{
					auto & dst = result.sliceMipBuffer( *buffer, layer, level );
					std::size_t byteCount = buffer->getSize();

					if ( data + byteCount > end )
					{
						CU_LoaderError( "Can't load DDS image: Failed to read level #" + string::toString( level ) );
					}

					std::memcpy( dst.data(), data, byteCount );
					data += byteCount;
				}

				if ( width > 1 )
				{
					width >>= 1;
				}

				if ( height > 1 )
				{
					height >>= 1;
				}

				if ( depth > 1 )
				{
					depth >>= 1;
				}
			}
		}

		return result;
	}
}
