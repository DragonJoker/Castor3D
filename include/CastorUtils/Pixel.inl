namespace Castor
{	namespace Resources
{
	//*************************************************************************************************

	template <> struct pixel_definitions<ePIXEL_FORMAT_L8>
	{
		enum { size = 1 };
		static inline String to_string()
		{
			return cuT( "8 bits luminosity");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_A8L8>
	{
		enum { size = 2 };
		static inline String to_string()
		{
			return cuT( "16 bits luminosity and alpha");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_A1R5G5B5>
	{
		enum { size = 2 };
		static inline String to_string()
		{
			return cuT( "16 bits 5551 ARGB");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_A4R4G4B4>
	{
		enum { size = 2 };
		static inline String to_string()
		{
			return cuT( "16 bits 4444 ARGB");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_R8G8B8>
	{
		enum { size = 3 };
		static inline String to_string()
		{
			return cuT( "24 bits 888 RGB");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_A8R8G8B8>
	{
		enum { size = 4 };
		static inline String to_string()
		{
			return cuT( "32 bits 8888 ARGB");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_DXTC1>
	{
		enum { size = 1 };
		static inline String to_string()
		{
			return cuT( "DXT1 8 bits compressed format");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_DXTC3>
	{
		enum { size = 2 };
		static inline String to_string()
		{
			return cuT( "DXT3 16 bits compressed format");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_DXTC5>
	{
		enum { size = 2 };
		static inline String to_string()
		{
			return cuT( "DXT5 16 bits compressed format");
		}
	};

	template <> struct pixel_definitions<ePIXEL_FORMAT_YUY2>
	{
		enum { size = 2 };
		static inline String to_string()
		{
			return cuT( "YUY2 16 bits compressed format");
		}
	};

	//*************************************************************************************************
	////////////////////////////////////////////////////////////////
	// Spécialisations for ePIXEL_FORMAT_L8 convertions
	////////////////////////////////////////////////////////////////
	template <> struct pixel_converter<ePIXEL_FORMAT_L8, ePIXEL_FORMAT_L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*p_pDest = *p_pSrc;
		}
	};
	template <> struct pixel_converter<ePIXEL_FORMAT_L8, ePIXEL_FORMAT_A8L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = *p_pSrc;
			p_pDest[1] = 0xFF;
		}
	};
	template <> struct pixel_converter<ePIXEL_FORMAT_L8, ePIXEL_FORMAT_A1R5G5B5>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*reinterpret_cast<unsigned short*>(p_pDest) = 0x8000 |
				((*p_pSrc >> 3) << 10) |
				((*p_pSrc >> 3) <<  5) |
				((*p_pSrc >> 3) <<  0);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_L8, ePIXEL_FORMAT_A4R4G4B4>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = (*p_pSrc & 0xF0) | (*p_pSrc >> 4);
			p_pDest[1] = 0xF0 | (*p_pSrc >> 4);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_L8, ePIXEL_FORMAT_R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = *p_pSrc;
			p_pDest[1] = *p_pSrc;
			p_pDest[2] = *p_pSrc;
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_L8, ePIXEL_FORMAT_A8R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = *p_pSrc;
			p_pDest[1] = *p_pSrc;
			p_pDest[2] = *p_pSrc;
			p_pDest[3] = 0xFF;
		}
	};


	////////////////////////////////////////////////////////////////
	// Spécialisations for ePIXEL_FORMAT_A8L8 convertions
	////////////////////////////////////////////////////////////////

	template <> struct pixel_converter<ePIXEL_FORMAT_A8L8, ePIXEL_FORMAT_L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*p_pDest = p_pSrc[0];
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8L8, ePIXEL_FORMAT_A8L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[1];
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8L8, ePIXEL_FORMAT_A1R5G5B5>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*reinterpret_cast<unsigned short*>(p_pDest) = ((p_pSrc[1] >> 7) << 15) |
				((p_pSrc[0] >> 3) << 10) |
				((p_pSrc[0] >> 3) <<  5) |
				((p_pSrc[0] >> 3) <<  0);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8L8, ePIXEL_FORMAT_A4R4G4B4>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = (p_pSrc[0] & 0xF0) | (p_pSrc[0] >> 4);
			p_pDest[1] = (p_pSrc[1] & 0xF0) | (p_pSrc[0] >> 4);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8L8, ePIXEL_FORMAT_R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[0];
			p_pDest[2] = p_pSrc[0];
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8L8, ePIXEL_FORMAT_A8R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[0];
			p_pDest[2] = p_pSrc[0];
			p_pDest[3] = p_pSrc[1];
		}
	};


	////////////////////////////////////////////////////////////////
	// Spécialisations for ePIXEL_FORMAT_A1R5G5B5 convertions
	////////////////////////////////////////////////////////////////

	template <> struct pixel_converter<ePIXEL_FORMAT_A1R5G5B5, ePIXEL_FORMAT_L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			unsigned short Pix = *reinterpret_cast<const unsigned short*>(p_pSrc);
			*p_pDest = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
				((Pix & 0x03E0) >> 2) * 0.59 +
				((Pix & 0x001F) << 3) * 0.11);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A1R5G5B5, ePIXEL_FORMAT_A8L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			unsigned short Pix = *reinterpret_cast<const unsigned short*>(p_pSrc);
			p_pDest[0] = static_cast<unsigned char>(((Pix & 0x7C00) >> 7) * 0.30 +
				((Pix & 0x03E0) >> 2) * 0.59 +
				((Pix & 0x001F) << 3) * 0.11);
			p_pDest[1] = p_pSrc[1] & 0x8000 ? 0xFF : 0x00;
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A1R5G5B5, ePIXEL_FORMAT_A1R5G5B5>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[1];
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A1R5G5B5, ePIXEL_FORMAT_A4R4G4B4>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			unsigned short Pix = *reinterpret_cast<const unsigned short*>(p_pSrc);
			p_pDest[1] = (Pix & 0x8000 ? 0xF0 : 0x00) | ((Pix & 0x7C00) >> 11);
			p_pDest[0] = ((Pix & 0x03C0) >> 2) | ((Pix & 0x001F) >> 1);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A1R5G5B5, ePIXEL_FORMAT_R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			unsigned short Pix = *reinterpret_cast<const unsigned short*>(p_pSrc);
			p_pDest[2] = (Pix & 0x7C00) >> 7;
			p_pDest[1] = (Pix & 0x03E0) >> 2;
			p_pDest[0] = (Pix & 0x001F) << 3;
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A1R5G5B5, ePIXEL_FORMAT_A8R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			unsigned short Pix = *reinterpret_cast<const unsigned short*>(p_pSrc);
			p_pDest[3] = (Pix & 0x8000) >> 8;
			p_pDest[2] = (Pix & 0x7C00) >> 7;
			p_pDest[1] = (Pix & 0x03E0) >> 2;
			p_pDest[0] = (Pix & 0x001F) << 3;
		}
	};


	////////////////////////////////////////////////////////////////
	// Spécialisations for ePIXEL_FORMAT_A4R4G4B4 convertions
	////////////////////////////////////////////////////////////////

	template <> struct pixel_converter<ePIXEL_FORMAT_A4R4G4B4, ePIXEL_FORMAT_L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*p_pDest = static_cast<unsigned char>(((p_pSrc[1] & 0x0F) << 4) * 0.30 +
				((p_pSrc[0] & 0xF0) >> 0) * 0.59 +
				((p_pSrc[0] & 0x0F) << 4) * 0.11);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A4R4G4B4, ePIXEL_FORMAT_A8L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = static_cast<unsigned char>(((p_pSrc[1] & 0x0F) << 4) * 0.30 +
				((p_pSrc[0] & 0xF0) >> 0) * 0.59 +
				((p_pSrc[0] & 0x0F) << 4) * 0.11);
			p_pDest[1] = p_pSrc[1] & 0xF0;
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A4R4G4B4, ePIXEL_FORMAT_A1R5G5B5>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*reinterpret_cast<unsigned short*>(p_pDest) = ((p_pSrc[1] & 0x80) <<  8) |
				((p_pSrc[1] & 0x0F) << 11) |
				((p_pSrc[0] & 0xF0) <<  2) |
				((p_pSrc[0] & 0x0F) <<  1);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A4R4G4B4, ePIXEL_FORMAT_A4R4G4B4>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[1];
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A4R4G4B4, ePIXEL_FORMAT_R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = (p_pSrc[0] & 0x0F) << 4;
			p_pDest[1] = (p_pSrc[0] & 0xF0);
			p_pDest[2] = (p_pSrc[1] & 0x0F) << 4;
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A4R4G4B4, ePIXEL_FORMAT_A8R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = (p_pSrc[0] & 0x0F) << 4;
			p_pDest[1] = (p_pSrc[0] & 0xF0);
			p_pDest[2] = (p_pSrc[1] & 0x0F) << 4;
			p_pDest[3] = (p_pSrc[1] & 0xF0);
		}
	};


	////////////////////////////////////////////////////////////////
	// Spécialisations for ePIXEL_FORMAT_R8G8B8 convertions
	////////////////////////////////////////////////////////////////

	template <> struct pixel_converter<ePIXEL_FORMAT_R8G8B8, ePIXEL_FORMAT_L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*p_pDest = static_cast<unsigned char>(p_pSrc[2] * 0.30 + p_pSrc[1] * 0.59 + p_pSrc[0] * 0.11);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_R8G8B8, ePIXEL_FORMAT_A8L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = static_cast<unsigned char>(p_pSrc[2] * 0.30 + p_pSrc[1] * 0.59 + p_pSrc[0] * 0.11);
			p_pDest[1] = 0xFF;
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_R8G8B8, ePIXEL_FORMAT_A1R5G5B5>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*reinterpret_cast<unsigned short*>(p_pDest) = 0x8000 |
				((p_pSrc[2] >> 3) << 10) |
				((p_pSrc[1] >> 3) <<  5) |
				((p_pSrc[0] >> 3) <<  0);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_R8G8B8, ePIXEL_FORMAT_A4R4G4B4>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = (p_pSrc[1] & 0xF0) | (p_pSrc[0] >> 4);
			p_pDest[1] = 0xF0 | (p_pSrc[2] >> 4);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_R8G8B8, ePIXEL_FORMAT_R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[1];
			p_pDest[2] = p_pSrc[2];
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_R8G8B8, ePIXEL_FORMAT_A8R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[1];
			p_pDest[2] = p_pSrc[2];
			p_pDest[3] = 0xFF;
		}
	};


	////////////////////////////////////////////////////////////////
	// Spécialisations for ePIXEL_FORMAT_A8R8G8B8 convertions
	////////////////////////////////////////////////////////////////

	template <> struct pixel_converter<ePIXEL_FORMAT_A8R8G8B8, ePIXEL_FORMAT_L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*p_pDest = static_cast<unsigned char>(p_pSrc[2] * 0.30 + p_pSrc[1] * 0.59 + p_pSrc[0] * 0.11);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8R8G8B8, ePIXEL_FORMAT_A8L8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = static_cast<unsigned char>(p_pSrc[2] * 0.30 + p_pSrc[1] * 0.59 + p_pSrc[0] * 0.11);
			p_pDest[1] = p_pSrc[3];
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8R8G8B8, ePIXEL_FORMAT_A1R5G5B5>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			*reinterpret_cast<unsigned short*>(p_pDest) = ((p_pSrc[3] >> 7) << 15) |
				((p_pSrc[2] >> 3) << 10) |
				((p_pSrc[1] >> 3) <<  5) |
				((p_pSrc[0] >> 3) <<  0);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8R8G8B8, ePIXEL_FORMAT_A4R4G4B4>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = (p_pSrc[1] & 0xF0) | (p_pSrc[0] >> 4);
			p_pDest[1] = (p_pSrc[3] & 0xF0) | (p_pSrc[2] >> 4);
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8R8G8B8, ePIXEL_FORMAT_R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[1];
			p_pDest[2] = p_pSrc[2];
		}
	};

	template <> struct pixel_converter<ePIXEL_FORMAT_A8R8G8B8, ePIXEL_FORMAT_A8R8G8B8>
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			p_pDest[0] = p_pSrc[0];
			p_pDest[1] = p_pSrc[1];
			p_pDest[2] = p_pSrc[2];
			p_pDest[3] = p_pSrc[3];
		}
	};


	////////////////////////////////////////////////////////////////
	// To tell no conversion available
	////////////////////////////////////////////////////////////////

	template <ePIXEL_FORMAT SrcFmt, ePIXEL_FORMAT DstFmt> struct pixel_converter
	{
		static inline void convert( unsigned char const * p_pSrc, unsigned char * p_pDest)
		{
			UNSUPPORTED_ERROR( String( cuT( "Conversion software de format de pixel (")) + pixel_definitions<SrcFmt>::to_string() + String( cuT( " -> ")) + pixel_definitions<DstFmt>::to_string() + String( cuT( ")")));
		}
	};

	//*************************************************************************************************

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU> struct PxOperators
	{
		static void Add( Pixel<FT> & p_pxA, Pixel<FU> const & p_pxB)
		{
			Pixel<FT> & l_pxB( p_pxB);
			PxOperators<FT, FT>::Add( p_pxA, l_pxB);
		}

		static void Substract( Pixel<FT> & p_pxA, Pixel<FU> const & p_pxB)
		{
			Pixel<FT> & l_pxB( p_pxB);
			PxOperators<FT>::Substract( p_pxA, l_pxB);
		}

		static void Multiply( Pixel<FT> & p_pxA, Pixel<FU> const & p_pxB)
		{
			Pixel<FT> & l_pxB( p_pxB);
			PxOperators<FT>::Multiply( p_pxA, l_pxB);
		}

		static void Divide( Pixel<FT> & p_pxA, Pixel<FU> const & p_pxB)
		{
			Pixel<FT> & l_pxB( p_pxB);
			PxOperators<FT>::Divide( p_pxA, l_pxB);
		}

		static void Assign( Pixel<FT> & p_pxA, Pixel<FU> const & p_pxB)
		{
			Pixel<FT> & l_pxB( p_pxB);
			PxOperators<FT>::Assign( p_pxA, l_pxB);
		}

		static bool Equals( Pixel<FT> const & p_pxA, Pixel<FU> const & p_pxB)
		{
			Pixel<FT> & l_pxB( p_pxB);
			return PxOperators<FT>::Equals( p_pxA, l_pxB);
		}
	};

	template <ePIXEL_FORMAT FT> struct PxOperators<FT, FT>
	{
		static void Add( Pixel<FT> & p_pxA, Pixel<FT> const & p_pxB)
		{
			for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
			{
				p_pxA[i] += p_pxB[i];
			}
		}

		static void Substract( Pixel<FT> & p_pxA, Pixel<FT> const & p_pxB)
		{
			for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
			{
				p_pxA[i] -= p_pxB[i];
			}
		}
		static void Multiply( Pixel<FT> & p_pxA, Pixel<FT> const & p_pxB)
		{
			for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
			{
				p_pxA[i] *= p_pxB[i];
			}
		}

		static void Divide( Pixel<FT> & p_pxA, Pixel<FT> const & p_pxB)
		{
			for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
			{
				p_pxA[i] = (p_pxB[i] == 0 ? p_pxA[i] : p_pxA[i] / p_pxB[i]);
			}
		}

		static void Assign( Pixel<FT> & p_pxA, Pixel<FT> const & p_pxB)
		{
			for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
			{
				p_pxA[i] = p_pxB[i];
			}
		}

		static bool Equals( Pixel<FT> & p_pxA, Pixel<FT> const & p_pxB)
		{
			bool l_bReturn = true;

			for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
			{
				l_bReturn = p_pxA[i] == p_pxB[i];
			}

			return l_bReturn;
		}
	};

	//*************************************************************************************************

	template <ePIXEL_FORMAT FT>
	Pixel<FT> :: Pixel( bool p_bInit)
		:	m_bOwnComponents	( p_bInit)
		,	m_pComponents		( p_bInit ? new unsigned char[pixel_definitions<FT>::size] : NULL)
	{
		if (p_bInit)
		{
			memset( m_pComponents, 0, pixel_definitions<FT>::size);
		}
	}

	template <ePIXEL_FORMAT FT>
	Pixel<FT> :: Pixel( unsigned char * p_components)
		:	m_bOwnComponents	( false)
	{
		m_pComponents = p_components;
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FT> :: Pixel( array<unsigned char, pixel_definitions<FU>::size> const & p_components)
		:	m_bOwnComponents	( true)
		,	m_pComponents		( new unsigned char[pixel_definitions<FT>::size])
	{
		pixel_converter<FU, FT>::convert( & p_components[0], m_pComponents);
	}
	
	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FT> :: Pixel( Pixel<FU> const & p_pxl)
	{
		pixel_converter<FU, FT>::convert( p_pxl.const_ptr(), m_pComponents);
	}

	template <ePIXEL_FORMAT FT>
	Pixel<FT> :: ~Pixel()
	{
		clear();
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FT> & Pixel<FT> :: operator =( Pixel<FU> const & p_pxl)
	{
		pixel_converter<FU, FT>::convert( p_pxl.const_ptr(), ptr());
		return * this;
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FT> & Pixel<FT> :: operator +=( Pixel<FU> const & p_px)
	{
		PxOperators<FT, FU>::Add( * this, p_px);
		return * this;
	}

	template <ePIXEL_FORMAT FT>
	template <typename U>
	Pixel<FT> & Pixel<FT> :: operator +=( U const & p_t)
	{
		for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
		{
			m_components[i] = T( U( m_components[i]) + p_t);
		}

		return * this;
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FT> & Pixel<FT> :: operator -=( Pixel<FU> const & p_px)
	{
		PxOperators<FT, FU>::Substract( * this, p_px);
		return * this;
	}

	template <ePIXEL_FORMAT FT>
	template <typename U>
	Pixel<FT> & Pixel<FT> :: operator -=( U const & p_t)
	{
		for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
		{
			m_components[i] = T( U( m_components[i]) - p_t);
		}

		return * this;
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FT> & Pixel<FT> :: operator /=( Pixel<FU> const & p_px)
	{
		PxOperators<FT, FU>::Divide( * this, p_px);
		return * this;
	}

	template <ePIXEL_FORMAT FT>
	template <typename U>
	Pixel<FT> & Pixel<FT> :: operator /=( U const & p_t)
	{
		for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
		{
			m_components[i] = (p_t == 0 ? m_components[i] : T( U( m_components[i]) + p_t));
		}

		return * this;
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FT> & Pixel<FT> :: operator *=( Pixel<FU> const & p_px)
	{
		PxOperators<FT, FU>::Multiply( * this, p_px);
		return * this;
	}

	template <ePIXEL_FORMAT FT>
	template <typename U>
	Pixel<FT> & Pixel<FT> :: operator *=( U const & p_t)
	{
		for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
		{
			m_components[i] = T( U( m_components[i]) * p_t);
		}

		return * this;
	}

	template <ePIXEL_FORMAT FT>
	Pixel<FT> & Pixel<FT> :: operator >>( size_t p_t)
	{
		for (size_t i = 0 ; i < pixel_definitions<FT>::size ; i++)
		{
			m_components[i] = m_components[i] >> p_t;
		}

		return * this;
	}

	template <ePIXEL_FORMAT FT>
	void Pixel<FT> :: clear()
	{
		if (m_bOwnComponents)
		{
			delete [] m_pComponents;
			m_pComponents = NULL;
		}
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	void Pixel<FT> :: set( array<unsigned char, pixel_definitions<FU>::size> const & p_components)
	{
		pixel_converter<FU, FT>::convert( & p_components[0], ptr());
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	void Pixel<FT> :: set( unsigned char * p_components)
	{
		pixel_converter<FU, FT>::convert( & p_components[0], ptr());
	}
	
	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	void Pixel<FT> :: set( Pixel<FU> const & p_px)
	{
		pixel_converter<FU, FT>::convert( p_px.const_ptr(), ptr());
	}

	template <ePIXEL_FORMAT FT>
	template <typename U>
	U Pixel<FT> :: sum()const
	{
		U l_base = 0;
		return std::accumulate( m_components.begin(), m_components.end(), l_base);
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FU> Pixel<FT> :: mul( Pixel<FU> const & p_px)const
	{
		Pixel<U> l_pxReturn( * this);
		l_pxReturn *= p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT>
	template <ePIXEL_FORMAT FU>
	Pixel<FU> Pixel<FT> :: mul( unsigned char const & p_val)const
	{
		Pixel<FU> l_pxReturn( * this);
		l_pxReturn *= p_val;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT>
	void Pixel<FT> :: link( unsigned char * p_pComponents)
	{
		clear();
		m_bOwnComponents = false;
		m_pComponents = p_pComponents;
	}

	template <ePIXEL_FORMAT FT>
	void Pixel<FT> :: unlink()
	{
		if ( ! m_bOwnComponents)
		{
			m_pComponents = new unsigned char[pixel_definitions<FT>::size];
			memset( m_pComponents, 0, pixel_definitions<FT>::size);
		}

		m_bOwnComponents = true;
	}

//*************************************************************************************************

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	bool operator ==( Pixel<FT> const & p_pixel, Pixel<FU> const & p_pxl)
	{
		return PxOperators<FT, FU>::Equals( p_pixel, p_pxl);
	}

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	Pixel<FT> operator +( Pixel<FT> const & p_pixel, Pixel<FU> const & p_px)
	{
		Pixel<FT> l_pxReturn( p_pixel);
		l_pxReturn += p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, typename U>
	Pixel<FT> operator +( Pixel<FT> const & p_pixel, U const & p_t)
	{
		Pixel<FT> l_pxReturn( p_pixel);
		l_pxReturn += p_t;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	Pixel<FT> operator -( Pixel<FT> const & p_pixel, Pixel<FU> const & p_px)
	{
		Pixel<FT> l_pxReturn( p_pixel);
		l_pxReturn -= p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, typename U>
	Pixel<FT> operator -( Pixel<FT> const & p_pixel, U const & p_t)
	{
		Pixel<FT> l_pxReturn( p_pixel);
		l_pxReturn -= p_t;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	Pixel<FT> operator /( Pixel<FT> const & p_pixel, Pixel<FU> const & p_px)
	{
		Pixel<FT> l_pxReturn( p_pixel);
		l_pxReturn /= p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, typename U>
	Pixel<FT> operator /( Pixel<FT> const & p_pixel, U const & p_t)
	{
		Pixel<FT> l_pxReturn( p_pixel);
		l_pxReturn /= p_t;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, ePIXEL_FORMAT FU>
	Pixel<FT> operator *( Pixel<FT> const & p_pixel, Pixel<FU> const & p_px)
	{
		Pixel<FT> l_pxReturn( p_pixel);
		l_pxReturn *= p_px;
		return l_pxReturn;
	}

	template <ePIXEL_FORMAT FT, typename U>
	Pixel<FT> operator *( Pixel<FT> const & p_pixel, U const & p_t)
	{
		Pixel<FT> l_pxReturn( p_pixel);
		l_pxReturn *= p_t;
		return l_pxReturn;
	}

	//*************************************************************************************************

	template <ePIXEL_FORMAT FT>
	PxBuffer<FT> :: PxBuffer( Math::Size const & p_size)
		:	PxBufferBase( p_size, FT)
	{
		_init();
	}

	template <ePIXEL_FORMAT FT>
	PxBuffer<FT> :: PxBuffer( Math::Size const & p_size, unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)
		:	PxBufferBase( p_size, FT, p_pBuffer, p_eBufferFormat)
	{
		_init();
	}

	template <ePIXEL_FORMAT FT>
	PxBuffer<FT> :: PxBuffer( PxBuffer<FT> const & p_pixelBuffer)
		:	PxBufferBase( p_pixelBuffer.dimensions(), FT)
	{
		_init();
	}

	template <ePIXEL_FORMAT FT>
	PxBuffer<FT> :: ~PxBuffer()
	{
		_clear();
	}

	template <ePIXEL_FORMAT FT>
	PxBuffer<FT> & PxBuffer<FT> :: operator =( PxBuffer<FT> const & p_pixelBuffer)
	{
		PxBufferBase::operator=( p_pixelBuffer);
		_init();
		return * this;
	}

	template <ePIXEL_FORMAT FT>
	void PxBuffer<FT> :: clear()
	{
		_clear();
		PxBufferBase::clear();
	}

	template <ePIXEL_FORMAT FT>
	void PxBuffer<FT> :: init( unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)
	{
		PxBufferBase::init( p_pBuffer, p_eBufferFormat);
		_init();
	}

	template <ePIXEL_FORMAT FT>
	void PxBuffer<FT> :: swap( PxBuffer<FT> & p_pixelBuffer)
	{
		PxBufferBase::swap( p_pixelBuffer);
		std::swap( m_arrayPixels,	p_pixelBuffer.m_arrayPixels);
		std::swap( m_arrayColumns,	p_pixelBuffer.m_arrayColumns);
	}

	template <ePIXEL_FORMAT FT>
	void PxBuffer<FT> :: assign( std::vector<unsigned char> const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)
	{
		if (p_pBuffer.size() > 0)
		{
			size_t l_uiSize = GetBytesPerPixel( p_eBufferFormat);
			unsigned char const * l_pBuffer = & p_pBuffer[0];
			size_t l_uiCount = 0;

			for (size_t i = 0 ; i < count() && l_uiCount < p_pBuffer.size() ; i++)
			{
				ConvertPixel( p_eBufferFormat, l_pBuffer, format(), m_arrayPixels[i].ptr());
				l_pBuffer += l_uiSize;
				l_uiCount += l_uiSize;
			}
		}
	}

	template <ePIXEL_FORMAT FT>
	PxBufferBase * PxBuffer<FT> :: clone()const
	{
		return new PxBuffer<FT>( * this);
	}

	template <ePIXEL_FORMAT FT>
	unsigned char * PxBuffer<FT> :: get_at( size_t x, size_t y)
	{
		CASTOR_ASSERT( x < width() && y < height());
		return operator[]( x)[y].ptr();
	}

	template <ePIXEL_FORMAT FT>
	unsigned char const * PxBuffer<FT> :: get_at( size_t x, size_t y)const
	{
		CASTOR_ASSERT( x < width() && y < height());
		return operator[]( x)[y].const_ptr();
	}

	template <ePIXEL_FORMAT FT>
	void PxBuffer<FT> :: flip()
	{
		for (unsigned int i = 0 ; i < width() ; ++i)
		{
			m_arrayColumns[i].flip();
		}
	}

	template <ePIXEL_FORMAT FT>
	void PxBuffer<FT> :: mirror()
	{
		for (unsigned int i = 0 ; i < width() / 2 ; ++i)
		{
			std::swap( m_arrayColumns[i], m_arrayColumns[width() - i]);
		}
	}

	template <ePIXEL_FORMAT FT>
	void PxBuffer<FT> :: _clear()
	{
		m_arrayColumns.clear();
		m_arrayPixels.clear();
	}

	template <ePIXEL_FORMAT FT>
	void PxBuffer<FT> :: _init()
	{
		m_arrayPixels.resize( count(), Pixel<FT>( false));
		m_arrayColumns.clear();
		m_arrayColumns.reserve( width());
		unsigned char * l_pBuffer = m_pBuffer;
		pixel *			l_pPixels = & m_arrayPixels[0];

		for (size_t i = 0 ; i < width() ; i++)
		{
			m_arrayColumns.push_back( column( height(), l_pPixels));

			for (size_t j = 0 ; j < height() ; j++)
			{
				m_arrayColumns[i][j].link( l_pBuffer);
				l_pBuffer += pixel_definitions<FT>::size;
			}

			l_pPixels += height();
		}
	}

	//*************************************************************************************************
}
}