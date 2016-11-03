/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GL_RENDER_SYSTEM_PREREQUISITES_H___
#define ___GL_RENDER_SYSTEM_PREREQUISITES_H___

#include <cstddef>

#include <CastorUtils.hpp>
#include <Engine.hpp>

#ifdef _WIN32
#	ifdef GlRenderSystem_EXPORTS
#		define C3D_Gl_API __declspec( dllexport )
#	else
#		define C3D_Gl_API __declspec( dllimport )
#	endif
#else
#	define C3D_Gl_API
#endif

#define BUFFER_OFFSET( n ) ( ( uint8_t * )nullptr + ( n ) )

using Castor::real;

namespace GlRender
{
	using Castor::real;

	enum class GlBufferBinding
		: uint32_t
	{
		None = 0x0000,
		FrontLeft = 0x0400,
		FrontRight = 0x0401,
		BackLeft = 0x0402,
		BackRight = 0x0403,
		Front = 0x0404,
		Back = 0x0405,
		Left = 0x0406,
		Right = 0x0407,
		FrontAndBack = 0x0408,
		Color0 = 0x8CE0,
		Color1 = 0x8CE1,
		Color2 = 0x8CE2,
		Color3 = 0x8CE3,
		Color4 = 0x8CE4,
		Color5 = 0x8CE5,
		Color6 = 0x8CE6,
		Color7 = 0x8CE7,
		Color8 = 0x8CE8,
		Color9 = 0x8CE9,
		Color10 = 0x8CEA,
		Color11 = 0x8CEB,
		Color12 = 0x8CEC,
		Color13 = 0x8CED,
		Color14 = 0x8CEE,
		Color15 = 0x8CEF,
		Depth = 0x8D00,
		Stencil = 0x8D20,
	};

	enum class GlPatchParameter
		: uint32_t
	{
		Vertices = 0x8E72,
		DefaultInnerLevel = 0x8E73,
		DefaultOuterLevel = 0x8E74,
	};

	enum class GlTopology
		: uint32_t
	{
		Points = 0x0000,
		Lines = 0x0001,
		LineLoop = 0x0002,
		LineStrip = 0x0003,
		Triangles = 0x0004,
		TriangleStrip = 0x0005,
		TriangleFan = 0x0006,
		Quads = 0x0007,
		QuadStrip = 0x0008,
		Polygon = 0x0009,
		Patches = 0x000E,
	};

	enum class GlInternal						//	Type	Comps	Norm	R	G	B	A
		: uint32_t
	{
		One = 0x0001,
		Two = 0x0002,
		Three = 0x0003,
		Four = 0x0004,
		R3G3B2 = 0x2A10,
		Alpha4 = 0x803B,
		Alpha8 = 0x803C,
		Alpha12 = 0x803D,
		Alpha16 = 0x803E,
		CompressedAlpha = 0x84E9,
		CompressedLuminance = 0x84EA,
		CompressedLuminanceAlpha = 0x84EB,
		CompressedIntensity = 0x84EC,
		CompressedRGB = 0x84ED,
		CompressedRGBA = 0x84EE,
		L4 = 0x803F,
		L8 = 0x8040,
		L12 = 0x8041,
		L16 = 0x8042,
		L4A4 = 0x8043,
		L6A2 = 0x8044,
		L8A8 = 0x8045,
		L12A4 = 0x8046,
		L12A12 = 0x8047,
		L16A16 = 0x8048,
		Intensity = 0x8049,
		I4 = 0x804A,
		I8 = 0x804B,
		I12 = 0x804C,
		I16 = 0x804D,
		RGB4 = 0x804F,
		RGB5 = 0x8050,
		RGB8 = 0x8051,
		RGB10 = 0x8052,
		RGB12 = 0x8053,
		RGB16 = 0x8054,
		RGBA2 = 0x8055,
		RGBA4 = 0x8056,
		RGB5A1 = 0x8057,
		RGBA8 = 0x8058,							//	uint		4	YES		R	G	B	A
		RGB10A2 = 0x8059,
		RGBA12 = 0x805A,
		RGBA16 = 0x805B,						//	short		4	YES		R	G	B	A
		RGB565 = 0x8D62,
		D16 = 0x81A5,
		D24 = 0x81A6,
		D32 = 0x81A7,
		R8 = 0x8229,							//	ubyte		1	YES		R	0	0	1
		R16 = 0x822A,							//	ushort		1	YES		R	0	0	1
		RG8 = 0x822B,							//	ubyte		2	YES		R	G	0	1
		RG16 = 0x822C,							//	ushort		2	YES		R	G	0	1
		R16F = 0x822D,							//	half		1	NO		R	0	0	1
		R32F = 0x822E,							//	float		1	NO		R	0	0	1
		RG16F = 0x822F,							//	half		2	NO		R	G	0	1
		RG32F = 0x8230,							//	float		2	NO		R	G	0	1
		R8I = 0x8231,							//	byte		1	NO		R	0	0	1
		R8UI = 0x8232,							//	ubyte		1	NO		R	0	0	1
		R16I = 0x8233,							//	short		1	NO		R	0	0	1
		R16UI = 0x8234,							//	ushort		1	NO		R	0	0	1
		R32I = 0x8235,							//	int			1	NO		R	0	0	1
		R32UI = 0x8236,							//	uint		1	NO		R	0	0	1
		RG8I = 0x8237,							//	byte		2	NO		R	G	0	1
		RG8UI = 0x8238,							//	ubyte		2	NO		R	G	0	1
		RG16I = 0x8239,							//	short		2	NO		R	G	0	1
		RG16UI = 0x823A,						//	ushort		2	NO		R	G	0	1
		RG32I = 0x823B,							//	int			2	NO		R	G	0	1
		RG32UI = 0x823C,						//	uint		2	NO		R	G	0	1
		DXT1 = 0x83F1,
		DXT3 = 0x83F2,
		DXT5 = 0x83F3,
		RGBA32F = 0x8814,						//	float		4	NO		R	G	B	A
		RGB32F = 0x8815,
		RGBA16F = 0x881A,						//	half		4	NO		R	G	B	A
		RGB16F = 0x881B,
		D24S8 = 0x88F0,
		SRGB = 0x8C40,
		SRGB8 = 0x8C41,
		SRGBA = 0x8C42,
		SRGB8A8 = 0x8C43,
		LA = 0x8C44,
		SL8A8 = 0x8C45,
		SLuminance = 0x8C46,
		SL8 = 0x8C47,
		D32F = 0x8CAC,
		S1 = 0x8D46,
		S4 = 0x8D47,
		S8 = 0x8D48,
		S16 = 0x8D49,
		RGBA32UI = 0x8D70,						//	uint		4	NO		R	G	B	A
		RGBA16UI = 0x8D76,						//	ushort		4	NO		R	G	B	A
		RGB16UI = 0x8D77,						//	ushort		3	NO		R	G	B	1
		RGBA8UI = 0x8D7C,						//	ubyte		4	NO		R	G	B	A
		RGBA32I = 0x8D82,						//	int			4	NO		R	G	B	A
		RGBA16I = 0x8D88,						//	short		4	NO		R	G	B	A
		RGBA8I = 0x8D8E,						//	byte		4	NO		R	G	B	A
	};

	enum class GlTexDim
		: uint32_t
	{
		Buffer = 0x8C2A,
		OneDimension = 0x0DE0,
		OneDimensionArray = 0x8C18,
		TwoDimensions = 0x0DE1,
		TwoDimensionsArray = 0x8C1A,
		TwoDimensionsMS = 0x9100,
		TwoDimensionsMSArray = 0x9102,
		ThreeDimensions = 0x806F,
		Cube = 0x8513,
		PositiveX = 0x8515,
		NegativeX = 0x8516,
		PositiveY = 0x8517,
		NegativeY = 0x8518,
		PositiveZ = 0x8519,
		NegativeZ = 0x851A,
		CubeArray = 0x9009,
	};

	enum class GlComparator
		: uint32_t
	{
		Never = 0x0200,
		Less = 0x0201,
		Equal = 0x0202,
		LEqual = 0x0203,
		Greater = 0x0204,
		NEqual = 0x0205,
		GEqual = 0x0206,
		Always = 0x0207,
	};

	enum class GlWrapMode
		: uint32_t
	{
		Clamp = 0x2900,
		Repeat = 0x2901,
		ClampToBorder = 0x812D,
		ClampToEdge = 0x812F,
		MirroredRepeat = 0x8370,
	};

	enum class GlInterpolationMode
		: uint32_t
	{
		Nearest = 0x2600,
		Linear = 0x2601,
		NearestMipmapNearest = 0x2700,
		LinearMipmapNearest = 0x2701,
		NearestMipmapLinear = 0x2702,
		LinearMipmapLinear = 0x2703,
	};

	enum class GlBlendFactor
		: uint32_t
	{
		Zero = 0x0000,
		One = 0x0001,
		SrcColour = 0x0300,
		InvSrcColour = 0x0301,
		SrcAlpha = 0x0302,
		InvSrcAlpha = 0x0303,
		DstAlpha = 0x0304,
		InvDstAlpha = 0x0305,
		DstColour = 0x0306,
		InvDstColour = 0x0307,
		SrcAlphaSaturate = 0x0308,
		Constant = 0x8001,
		InvConstant = 0x8002,
		Src1Alpha = 0x8589,
		Src1Colour = 0x88F9,
		InvSrc1Colour = 0x88FA,
		InvSrc1Alpha = 0x88FB,
	};

	enum class GlTextureIndex
		: uint32_t
	{
		Index0 = 0x84C0,
		Index1 = 0x84C1,
		Index2 = 0x84C2,
		Index3 = 0x84C3,
		Index4 = 0x84C4,
		Index5 = 0x84C5,
		Index6 = 0x84C6,
		Index7 = 0x84C7,
		Index8 = 0x84C8,
		Index9 = 0x84C9,
		Index10 = 0x84CA,
		Index11 = 0x84CB,
		Index12 = 0x84CC,
		Index13 = 0x84CD,
		Index14 = 0x84CE,
		Index15 = 0x84CF,
		Index16 = 0x84D0,
		Index17 = 0x84D1,
		Index18 = 0x84D2,
		Index19 = 0x84D3,
		Index20 = 0x84D4,
		Index21 = 0x84D5,
		Index22 = 0x84D6,
		Index23 = 0x84D7,
		Index24 = 0x84D8,
		Index25 = 0x84D9,
		Index26 = 0x84DA,
		Index27 = 0x84DB,
		Index28 = 0x84DC,
		Index29 = 0x84DD,
		Index30 = 0x84DE,
		Index31 = 0x84DF,
	};

	enum class GlBlendSource
		: uint32_t
	{
		Texture = 0x1702,
		Texture0 = 0x84C0,
		Texture1 = 0x84C1,
		Texture2 = 0x84C2,
		Texture3 = 0x84C3,
		Constant = 0x8576,
		Colour = 0x8577,
		Previous = 0x8578,
	};

	enum class GlBlendFunc
		: uint32_t
	{
		Add = 0x0104,
		Replace = 0x1E01,
		Modulate = 0x2100,
		Subtract = 0x84E7,
		AddSigned = 0x8574,
		Interpolate = 0x8575,
		Dot3RGB = 0x86AE,
		Dot3RGBA = 0x86AF,
	};

	enum class GlBlendOp
		: uint32_t
	{
		Add = 0x8006,
		Min = 0x8007,
		Max = 0x8008,
		Subtract = 0x800A,
		RevSubtract = 0x800B,
	};

	enum class GlBufferMode
		: uint32_t
	{
		StreamDraw = 0x88E0,
		StreamRead = 0x88E1,
		StreamCopy = 0x88E2,
		StaticDraw = 0x88E4,
		StaticRead = 0x88E5,
		StaticCopy = 0x88E6,
		DynamicDraw = 0x88E8,
		DynamicRead = 0x88E9,
		DynamicCopy = 0x88EA,
	};

	enum class GlComponent
		: uint32_t
	{
		Colour = 0x1900,
		Stencil = 0x1901,
		Depth = 0x1902,
	};

	enum class GlBufferTarget
		: uint32_t
	{
		Array = 0x8892,
		ElementArray = 0x8893,
		PixelPack = 0x88EB,
		PixelUnpack = 0x88EC,
		Texture = 0x8C2A,
		Uniform = 0x8A11,
		TransformFeedback = 0x8E22,
		TransformFeedbackBuffer = 0x8C8E,
		Read = 0x8F36,
		Write = 0x8F37,
	};

	enum class GlShaderType
		: uint32_t
	{
		Fragment = 0x8B30,
		Vertex = 0x8B31,
		Geometry = 0x8DD9,
		TessEvaluation = 0x8E87,
		TessControl = 0x8E88,
		Compute = 0x91B9,
	};

	enum class GlAccessType
		: uint32_t
	{
		Read = 0x88B8,
		Write = 0x88B9,
		ReadWrite = 0x88BA,
	};

	enum class GlUniformBlockValue
		: uint32_t
	{
		Size = 0x8A38,
		Offset = 0x8A3B,
		BlockDataSize = 0x8A40,
		GpuAddress = 0x8F34,
	};

	enum class GlContextAttribute
		: uint32_t
	{
		DebugBit = 0x0001,
		ForwardCompatibleBit = 0x0002,
		MajorVersion = 0x2091,
		MinorVersion = 0x2092,
		LayerPlane = 0x2093,
		Flags = 0x2094,
	};

	enum class GlProfileAttribute
		: uint32_t
	{
		CoreBit = 0x0001,
		CompatibilityBit = 0x0002,
		Mask = 0x9126,
	};

	enum class GlShaderStatus
		: uint32_t
	{
		Delete = 0x8B80,
		Compile = 0x8B81,
		Link = 0x8B82,
		Validate = 0x8B83,
		InfoLogLength = 0x8B84,
		AttachedShaders = 0x8B85,
		ActiveUniforms = 0x8B86,
		SourceLength = 0x8B88,
		ActiveAttributes = 0x8B89,
		ActiveAttributeMaxLength = 0x8B8A,
	};

	enum class GlType
		: uint32_t
	{
		Default = 0x0000,
		Byte = 0x1400,
		UnsignedByte = 0x1401,
		Short = 0x1402,
		UnsignedShort = 0x1403,
		Int = 0x1404,
		UnsignedInt = 0x1405,
		Float = 0x1406,
		Double = 0x140A,
		HalfFloat = 0x140B,
		Bitmap = 0X1A00,
		UnsignedByte332 = 0x8032,
		UnsignedShort4444 = 0x8033,
		UnsignedShort5551 = 0x8034,
		UnsignedInt8888 = 0x8035,
		UnsignedInt1010102 = 0x8036,
		UnsignedByte233Rev = 0x8362,
		UnsignedShort565 = 0x8363,
		UnsignedShort565Rev = 0x8364,
		UnsignedShort4444Rev = 0x8365,
		UnsignedShort1555Rev = 0x8366,
		UnsignedInt8888Rev = 0x8367,
		UnsignedInt2101010Rev = 0x8368,
		UnsignedInt248 = 0x84FA,
#if CASTOR_USE_DOUBLE
		Real = Double
#else
		Real = Float
#endif
	};

	enum class GlFormat
		: uint32_t
	{
		Stencil = 0x1901,
		Depth = 0x1902,
		Red = 0x1903,
		Green = 0x1904,
		Blue = 0x1905,
		Alpha = 0x1906,
		RGB = 0x1907,
		RGBA = 0x1908,
		Luminance = 0x1909,
		LuminanceAlpha = 0x190A,
		BGR = 0x80E0,
		BGRA = 0x80E1,
		RG = 0x8227,
		DepthStencil = 0x84F9,
	};

	enum class GlTweak
		: uint32_t
	{
		LineSmooth = 0x0B20,
		CullFace = 0x0B44,
		DepthTest = 0x0B71,
		StencilTest = 0x0B90,
		AlphaTest = 0x0BC0,
		Dither = 0x0BD0,
		Blend = 0x0BE2,
		ScissorTest = 0x0C11,
		OffsetFill = 0x8037,
		Multisample = 0x809D,
		AlphaToCoverage = 0x809E,
		DebugOutputSynchronous = 0x8242,
		DepthClamp = 0x864F,
		RasterizerDiscard = 0x8C89,
	};

	enum class GlFramebufferStatus
		: uint32_t
	{
		Invalid = 0xFFFFFFFF,
		Complete = 0x8CD5,
		IncompleteAttachment = 0x8CD6,
		IncompleteMissingAttachment = 0x8CD7,
		IncompleteDrawBuffer = 0x8CDB,
		IncompleteReadBuffer = 0x8CDC,
		Unsupported = 0x8CDD,
		IncompleteMultisample = 0x8D56,
		IncompleteLayerTargets = 0x8DA8,
	};

	enum class GlAttachmentPoint
		: uint32_t
	{
		None = 0x0000,
		Colour0 = 0x8CE0,
		Depth = 0x8D00,
		Stencil = 0x8D20,
	};

	enum class GlFrameBufferMode
		: uint32_t
	{
		Read = 0x8CA8,
		Draw = 0x8CA9,
		Default = 0x8D40,
	};

	enum class GlRenderBufferMode
		: uint32_t
	{
		Default = 0x8D41
	};

	enum class GlRenderBufferParameter
		: uint32_t
	{
		Width = 0x8D42,
		Height = 0x8D43,
		InternalFormat = 0x8D44,
		RedSize = 0x8D50,
		GreenSize = 0x8D51,
		BlueSize = 0x8D52,
		AlphaSize = 0x8D53,
		DepthSize = 0x8D54,
		StencilSize = 0x8D55,
	};

	enum class GlFrontFaceDirection
		: uint32_t
	{
		ClockWise = 0x0900,
		CounterClockWise = 0x0901,
	};

	enum class GlBufferBit
		: uint32_t
	{
		Colour = 0x00004000,
		Depth = 0x00000100,
		Stencil = 0x00000400,
	};

	constexpr uint32_t GlInvalidIndex = 0xFFFFFFFF;

	enum class GlBufferMappingBit
		: uint32_t
	{
		Read = 0x0001,
		Write = 0x0002,
		InvalidateRange = 0x0004,
		InvalidateBuffer = 0x0008,
		FlushExplicit = 0x0010,
		Unsynchronised = 0x0020,
	};

	enum class GlCulledFace
		: uint32_t
	{
		Front = 0x0404,
		Back = 0x0405,
		Both = 0x0408,
	};

	typedef enum eGL_STENCIL_OP
		: uint32_t
	{
		eGL_STENCIL_OP_ZERO = 0x0000,
		eGL_STENCIL_OP_INVERT = 0x150A,
		eGL_STENCIL_OP_KEEP = 0x1E00,
		eGL_STENCIL_OP_REPLACE = 0x1E01,
		eGL_STENCIL_OP_INCR = 0x1E02,
		eGL_STENCIL_OP_DECR = 0x1E03,
		eGL_STENCIL_OP_INCR_WRAP = 0x8507,
		eGL_STENCIL_OP_DECR_WRAP = 0x8508,
	}	eGL_STENCIL_OP;

	typedef enum eGL_FILL_MODE
		: uint32_t
	{
		eGL_FILL_MODE_POINT = 0x1B00,
		eGL_FILL_MODE_LINE = 0x1B01,
		eGL_FILL_MODE_FILL = 0x1B02,
	}	eGL_FILL_MODE;

	typedef enum eGL_COMPARE_MODE
		: uint32_t
	{
		eGL_COMPARE_MODE_NONE = 0,
		eGL_COMPARE_MODE_REF_TO_TEXTURE = 0x884E
	}	eGL_COMPARE_MODE;

	typedef enum eGL_SAMPLER_PARAMETER
		: uint32_t
	{
		eGL_SAMPLER_PARAMETER_BORDERCOLOUR = 0x1004,
		eGL_SAMPLER_PARAMETER_MAGFILTER = 0x2800,
		eGL_SAMPLER_PARAMETER_MINFILTER = 0x2801,
		eGL_SAMPLER_PARAMETER_UWRAP = 0x2802,
		eGL_SAMPLER_PARAMETER_VWRAP = 0x2803,
		eGL_SAMPLER_PARAMETER_WWRAP = 0x8072,
		eGL_SAMPLER_PARAMETER_MINLOD = 0x813A,
		eGL_SAMPLER_PARAMETER_MAXLOD = 0x813B,
		eGL_SAMPLER_PARAMETER_MAXANISOTROPY = 0x84FE,
		eGL_SAMPLER_PARAMETER_LODBIAS = 0x8501,
		eGL_SAMPLER_PARAMETER_COMPAREMODE = 0x884C,
		eGL_SAMPLER_PARAMETER_COMPAREFUNC = 0x884D,
	}	eGL_SAMPLER_PARAMETER;

	typedef enum eGL_DEBUG_TYPE
		: uint32_t
	{
		eGL_DEBUG_TYPE_ERROR = 0x824C,
		eGL_DEBUG_TYPE_DEPRECATED_BEHAVIOR = 0x824D,
		eGL_DEBUG_TYPE_UNDEFINED_BEHAVIOR = 0x824E,
		eGL_DEBUG_TYPE_PORTABILITY = 0x824F,
		eGL_DEBUG_TYPE_PERFORMANCE = 0x8250,
		eGL_DEBUG_TYPE_OTHER = 0x8251,
	}	eGL_DEBUG_TYPE;

	typedef enum eGL_DEBUG_SOURCE
		: uint32_t
	{
		eGL_DEBUG_SOURCE_API = 0x8246,
		eGL_DEBUG_SOURCE_WINDOW_SYSTEM = 0x8247,
		eGL_DEBUG_SOURCE_SHADER_COMPILER = 0x8248,
		eGL_DEBUG_SOURCE_THIRD_PARTY = 0x8249,
		eGL_DEBUG_SOURCE_APPLICATION = 0x824A,
		eGL_DEBUG_SOURCE_OTHER = 0x824B,
	}	eGL_DEBUG_SOURCE;

	typedef enum eGL_DEBUG_CATEGORY
		: uint32_t
	{
		eGL_DEBUG_CATEGORY_API_ERROR = 0x9149,
		eGL_DEBUG_CATEGORY_WINDOW_SYSTEM = 0x914A,
		eGL_DEBUG_CATEGORY_DEPRECATION = 0x914B,
		eGL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR = 0x914C,
		eGL_DEBUG_CATEGORY_PERFORMANCE = 0x914D,
		eGL_DEBUG_CATEGORY_SHADER_COMPILER = 0x914E,
		eGL_DEBUG_CATEGORY_APPLICATION = 0x914F,
		eGL_DEBUG_CATEGORY_OTHER = 0x9150,
	}	eGL_DEBUG_CATEGORY;

	typedef enum eGL_DEBUG_SEVERITY
		: uint32_t
	{
		eGL_DEBUG_SEVERITY_HIGH = 0x9146,
		eGL_DEBUG_SEVERITY_MEDIUM = 0x9147,
		eGL_DEBUG_SEVERITY_LOW = 0x9148,
	}	eGL_DEBUG_SEVERITY;

	enum class GlMin
		: uint32_t
	{
		ProgramTexelOffset = 0x8904,
		MapBufferAlignment = 0x90BC,
	};

	enum class GlMax
		: uint32_t
	{
		ComputeShaderStorageBlocks = 0x90DB,
		CombinedShaderStorageBlocks = 0x90DC,
		ComputeUniformBlocks = 0x91BB,
		ComputeTextureImageUnits = 0x91BC,
		ComputeUniformComponents = 0x8263,
		ComputeAtomicCounters = 0x8265,
		ComputeAtomicCounterBuffers = 0x8264,
		CombinedComputeUniformComponents = 0x8266,
		ComputeWorkGroupInvocations = 0x90EB,
		ComputeWorkGroupCount = 0x91BE,
		ComputeWorkGroupSize = 0x91BF,
		DebugGroupStackDepth = 0x826C,
		Texture3DSize = 0x8073,
		TextureArrayLayers = 0x88FF,
		ClipDistances = 0x0D32,
		ColorTextureSamples = 0x910E,
		CombinedAtomicCounters = 0x92D7,
		CombinedFragmentUniformComponents = 0x8A33,
		CombinedGeometryUniformComponents = 0x8A32,
		CombinedTextureImageUnits = 0x8B4D,
		CombinedUniformBlocks = 0x8A2E,
		CombinedVertexUniformComponents = 0x8A31,
		TextureCubeSize = 0x851C,
		DepthTextureSamples = 0x910F,
		DrawBuffers = 0x8824,
		DualSourceDrawBuffers = 0x88FC,
		ElementsIndices = 0x80E9,
		ElementsVertices = 0x80E8,
		FragmentAtomicCounters = 0x92D6,
		FragmentShaderStorageBlocks = 0x90DA,
		FragmentInputComponents = 0x9125,
		FragmentUniformComponents = 0x8B49,
		FragmentUniformVectors = 0x8DFD,
		FragmentUniformBlocks = 0x8A2D,
		FramebufferWidth = 0x9315,
		FramebufferHeight = 0x9316,
		FramebufferLayers = 0x9317,
		FramebufferSamples = 0x9318,
		GeometryAtomicCounters = 0x92D5,
		GeometryShaderStorageBlocks = 0x90D7,
		GeometryInputComponents = 0x9123,
		GeometryOutputComponents = 0x9124,
		GeometryTextureImageUnits = 0x8C29,
		GeometryUniformBlocks = 0x8A2C,
		GeometryUniformComponents = 0x8DDF,
		IntegerSamples = 0x9110,
		LabelLength = 0x82E8,
		ProgramTexelOffset = 0x8905,
		RectangleTextureSize = 0x84F8,
		RenderbufferSize = 0x84E8,
		SampleMaskWords = 0x8E59,
		Samples = 0x8D57,
		ServerWaitTimeout = 0x9111,
		ShaderStorageBufferBindings = 0x90DD,
		TessControlAtomicCounters = 0x92D3,
		TessEvaluationAtomicCounters = 0x92D4,
		TessControlShaderStorageBlocks = 0x90D8,
		TessEvaluationShaderStorageBlocks = 0x90D9,
		TextureBufferSize = 0x8C2B,
		TextureImageUnits = 0x8872,
		TextureLODBias = 0x84FD,
		TextureSize = 0x0D33,
		UniformBufferBindings = 0x8A2F,
		UniformBlockSize = 0x8A30,
		UniformLocations = 0x826E,
		VaryingComponents = 0x8B4B,
		VaryingVectors = 0x8DFC,
		VaryingFloats = 0x8B4B,
		VertexAtomicCounters = 0x92D2,
		VertexAttribs = 0x8869,
		VertexShaderStorageBlocks = 0x90D6,
		VertexTextureImageUnits = 0x8B4C,
		VertexUniformComponents = 0x8B4A,
		VertexUniformVectors = 0x8DFB,
		VertexOutputComponents = 0x9122,
		VertexUniformBlocks = 0x8A2B,
		ViewportDims = 0x0D3A,
		Viewports = 0x825B,
	};

	enum class GlGpuInfo
	{
		TotalAvailableMemNVX = 0x9048,
		VBOFreeMemoryATI = 0x87FB,
		TextureFreeMemoryATI = 0x87FC,
		RenderbufferFreeMemoryATI = 0x87FD,
	};

	typedef enum eGL_HINT
	{
		eGL_HINT_LINE_SMOOTH = 0x0C52,
		eGL_HINT_POLYGON_SMOOTH = 0x0C53,
		eGL_HINT_VOLUME_CLIPPING = 0x80F0,
		eGL_HINT_TEXTURE_COMPRESSION = 0x84EF,
		eGL_HINT_SHADER_DERIVATIVE = 0x8B8B,
	}	eGL_HINT;

	typedef enum eGL_HINT_VALUE
	{
		eGL_HINT_VALUE_DONTCARE = 0x1100,
		eGL_HINT_VALUE_FASTEST = 0x1101,
		eGL_HINT_VALUE_NICEST = 0x1102,
	}	eGL_HINT_VALUE;

	typedef enum eGL_STORAGE_MODE
		: uint32_t
	{
		eGL_STORAGE_MODE_UNPACK_SWAP_BYTES = 0x0CF0,
		eGL_STORAGE_MODE_UNPACK_LSB_FIRST = 0x0CF1,
		eGL_STORAGE_MODE_UNPACK_ROW_LENGTH = 0x0CF2,
		eGL_STORAGE_MODE_UNPACK_SKIP_ROWS = 0x0CF3,
		eGL_STORAGE_MODE_UNPACK_SKIP_PIXELS = 0x0CF4,
		eGL_STORAGE_MODE_UNPACK_ALIGNMENT = 0x0CF5,
		eGL_STORAGE_MODE_PACK_SWAP_BYTES = 0x0D00,
		eGL_STORAGE_MODE_PACK_LSB_FIRST = 0x0D01,
		eGL_STORAGE_MODE_PACK_ROW_LENGTH = 0x0D02,
		eGL_STORAGE_MODE_PACK_SKIP_ROWS = 0x0D03,
		eGL_STORAGE_MODE_PACK_SKIP_PIXELS = 0x0D04,
		eGL_STORAGE_MODE_PACK_ALIGNMENT = 0x0D05,
		eGL_STORAGE_MODE_PACK_IMAGE_HEIGHT = 0x806C,
		eGL_STORAGE_MODE_UNPACK_IMAGE_HEIGHT = 0x806E,
	}	eGL_STORAGE_MODE;

	typedef enum eGL_BUFFER_PARAMETER
		: uint32_t
	{
		eGL_BUFFER_PARAMETER_SIZE = 0x8764,
		eGL_BUFFER_PARAMETER_USAGE = 0x8765,
		eGL_BUFFER_PARAMETER_ACCESS = 0x88BB,
		eGL_BUFFER_PARAMETER_MAPPED = 0x88BD,
		eGL_BUFFER_PARAMETER_GPU_ADDRESS = 0x8F1D,
	}	eGL_BUFFER_PARAMETER;

	typedef enum eGL_RESIDENT_BUFFER_ACCESS
		: uint32_t
	{
		eGL_RESIDENT_BUFFER_ACCESS_READ_ONLY = 0x88B8
	}	eGL_RESIDENT_BUFFER_ACCESS;

	typedef enum eGL_ADDRESS
		: uint32_t
	{
		eGL_ADDRESS_VERTEX_ATTRIB_ARRAY = 0x8F20,
		eGL_ADDRESS_TEXTURE_COORD_ARRAY = 0x8F25,
		eGL_ADDRESS_VERTEX_ARRAY = 0x8F21,
		eGL_ADDRESS_NORMAL_ARRAY = 0x8F22,
		eGL_ADDRESS_COLOR_ARRAY = 0x8F23,
		eGL_ADDRESS_INDEX_ARRAY = 0x8F24,
		eGL_ADDRESS_EDGE_FLAG_ARRAY = 0x8F26,
		eGL_ADDRESS_SECONDARY_COLOR_ARRAY = 0x8F27,
		eGL_ADDRESS_FOG_COORD_ARRAY = 0x8F28,
		eGL_ADDRESS_ELEMENT_ARRAY = 0x8F29,
	}	eGL_ADDRESS;

	typedef enum eGL_QUERY
		: uint32_t
	{
		eGL_QUERY_TIME_ELAPSED = 0x88BF,
		eGL_QUERY_SAMPLES_PASSED = 0x8914,
		eGL_QUERY_ANY_SAMPLES_PASSED = 0x8C2F,
		eGL_QUERY_PRIMITIVES_GENERATED = 0x8C87,
		eGL_QUERY_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = 0x8C88,
		eGL_QUERY_ANY_SAMPLES_PASSED_CONSERVATIVE = 0x8D6A,
		eGL_QUERY_TIMESTAMP = 0x8E28,
	}	eGL_QUERY;

	typedef enum eGL_QUERY_INFO
		: uint32_t
	{
		eGL_QUERY_INFO_RESULT = 0x8866,
		eGL_QUERY_INFO_RESULT_AVAILABLE = 0x8867,
		eGL_QUERY_INFO_RESULT_NO_WAIT = 0x9194,
	}	eGL_QUERY_INFO;

	typedef enum eGLSL_INTERFACE
		: uint32_t
	{
		eGLSL_INTERFACE_UNIFORM = 0x92E1,
		eGLSL_INTERFACE_UNIFORM_BLOCK = 0x92E2,
		eGLSL_INTERFACE_PROGRAM_INPUT = 0x92E3,
		eGLSL_INTERFACE_PROGRAM_OUTPUT = 0x92E4,
		eGLSL_INTERFACE_BUFFER_VARIABLE = 0x92E5,
		eGLSL_INTERFACE_SHADER_STORAGE_BLOCK = 0x92E6,
		eGLSL_INTERFACE_VERTEX_SUBROUTINE = 0x92E8,
		eGLSL_INTERFACE_TESS_CONTROL_SUBROUTINE = 0x92E9,
		eGLSL_INTERFACE_TESS_EVALUATION_SUBROUTINE = 0x92EA,
		eGLSL_INTERFACE_GEOMETRY_SUBROUTINE = 0x92EB,
		eGLSL_INTERFACE_FRAGMENT_SUBROUTINE = 0x92EC,
		eGLSL_INTERFACE_COMPUTE_SUBROUTINE = 0x92ED,
		eGLSL_INTERFACE_VERTEX_SUBROUTINE_UNIFORM = 0x92EE,
		eGLSL_INTERFACE_TESS_CONTROL_SUBROUTINE_UNIFORM = 0x92EF,
		eGLSL_INTERFACE_TESS_EVALUATION_SUBROUTINE_UNIFORM = 0x92F0,
		eGLSL_INTERFACE_GEOMETRY_SUBROUTINE_UNIFORM = 0x92F1,
		eGLSL_INTERFACE_FRAGMENT_SUBROUTINE_UNIFORM = 0x92F2,
		eGLSL_INTERFACE_COMPUTE_SUBROUTINE_UNIFORM = 0x92F3,
		eGLSL_INTERFACE_TRANSFORM_FEEDBACK_VARYING = 0x92F4,
	}	eGLSL_INTERFACE;

	typedef enum eGLSL_DATA_NAME
		: uint32_t
	{
		eGLSL_DATA_NAME_ACTIVE_RESOURCES = 0x92F5,
		eGLSL_DATA_NAME_MAX_NAME_LENGTH = 0x92F6,
		eGLSL_DATA_NAME_MAX_NUM_ACTIVE_VARIABLES = 0x92F7,
		eGLSL_DATA_NAME_MAX_NUM_COMPATIBLE_SUBROUTINES = 0x92F8,
	}	eGLSL_DATA_NAME;

	typedef enum eGLSL_PROPERTY
		: uint32_t
	{
		eGLSL_PROPERTY_NUM_COMPATIBLE_SUBROUTINES = 0x8E4A,
		eGLSL_PROPERTY_COMPATIBLE_SUBROUTINES = 0x8E4B,
		eGLSL_PROPERTY_IS_PER_PATCH = 0x92E7,
		eGLSL_PROPERTY_NAME_LENGTH = 0x92F9,
		eGLSL_PROPERTY_TYPE = 0x92FA,
		eGLSL_PROPERTY_ARRAY_SIZE = 0x92FB,
		eGLSL_PROPERTY_OFFSET = 0x92FC,
		eGLSL_PROPERTY_BLOCK_INDEX = 0x92FD,
		eGLSL_PROPERTY_ARRAY_STRIDE = 0x92FE,
		eGLSL_PROPERTY_MATRIX_STRIDE = 0x92FF,
		eGLSL_PROPERTY_IS_ROW_MAJOR = 0x9300,
		eGLSL_PROPERTY_ATOMIC_COUNTER_BUFFER_INDEX = 0x9301,
		eGLSL_PROPERTY_BUFFER_BINDING = 0x9302,
		eGLSL_PROPERTY_BUFFER_DATA_SIZE = 0x9303,
		eGLSL_PROPERTY_NUM_ACTIVE_VARIABLES = 0x9304,
		eGLSL_PROPERTY_ACTIVE_VARIABLES = 0x9305,
		eGLSL_PROPERTY_REFERENCED_BY_VERTEX_SHADER = 0x9306,
		eGLSL_PROPERTY_REFERENCED_BY_TESS_CONTROL_SHADER = 0x9307,
		eGLSL_PROPERTY_REFERENCED_BY_TESS_EVALUATION_SHADER = 0x9308,
		eGLSL_PROPERTY_REFERENCED_BY_GEOMETRY_SHADER = 0x9309,
		eGLSL_PROPERTY_REFERENCED_BY_FRAGMENT_SHADER = 0x930A,
		eGLSL_PROPERTY_REFERENCED_BY_COMPUTE_SHADER = 0x930B,
		eGLSL_PROPERTY_TOP_LEVEL_ARRAY_SIZE = 0x930C,
		eGLSL_PROPERTY_TOP_LEVEL_ARRAY_STRIDE = 0x930D,
		eGLSL_PROPERTY_LOCATION = 0x930E,
		eGLSL_PROPERTY_LOCATION_INDEX = 0x930F,
		eGLSL_PROPERTY_LOCATION_COMPONENT = 0x934A,
		eGLSL_PROPERTY_TRANSFORM_FEEDBACK_BUFFER_INDEX = 0x934B,
		eGLSL_PROPERTY_TRANSFORM_FEEDBACK_BUFFER_STRIDE = 0x934C,
	}	eGLSL_PROPERTY;

	typedef enum eGLSL_ATTRIBUTE_TYPE
		: uint32_t
	{
		eGLSL_ATTRIBUTE_TYPE_INT = 0x1404,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT = 0x1405,
		eGLSL_ATTRIBUTE_TYPE_FLOAT = 0x1406,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE = 0x140A,
		eGLSL_ATTRIBUTE_TYPE_HALF_FLOAT = 0x140B,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC2 = 0x8B50,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC3 = 0x8B51,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_VEC4 = 0x8B52,
		eGLSL_ATTRIBUTE_TYPE_INT_VEC2 = 0x8B53,
		eGLSL_ATTRIBUTE_TYPE_INT_VEC3 = 0x8B54,
		eGLSL_ATTRIBUTE_TYPE_INT_VEC4 = 0x8B55,
		eGLSL_ATTRIBUTE_TYPE_BOOL = 0x8B56,
		eGLSL_ATTRIBUTE_TYPE_BOOL_VEC2 = 0x8B57,
		eGLSL_ATTRIBUTE_TYPE_BOOL_VEC3 = 0x8B58,
		eGLSL_ATTRIBUTE_TYPE_BOOL_VEC4 = 0x8B59,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT2 = 0x8B5A,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT3 = 0x8B5B,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT4 = 0x8B5C,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_1D = 0x8B5D,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_2D = 0x8B5E,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_3D = 0x8B5F,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_CUBE = 0x8B60,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_1D_SHADOW = 0x8B61,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_2D_SHADOW = 0x8B62,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_2D_RECT = 0x8B63,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_2D_RECT_SHADOW = 0x8B64,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT2x3 = 0x8B65,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT2x4 = 0x8B66,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT3x2 = 0x8B67,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT3x4 = 0x8B68,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT4x2 = 0x8B69,
		eGLSL_ATTRIBUTE_TYPE_FLOAT_MAT4x3 = 0x8B6A,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_1D_ARRAY = 0x8DC0,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_2D_ARRAY = 0x8DC1,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_BUFFER = 0x8DC2,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_1D_ARRAY_SHADOW = 0x8DC3,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_2D_ARRAY_SHADOW = 0x8DC4,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_CUBE_SHADOW = 0x8DC5,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_VEC2 = 0x8DC6,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_VEC3 = 0x8DC7,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_VEC4 = 0x8DC8,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_1D = 0x8DC9,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_2D = 0x8DCA,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_3D = 0x8DCB,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_CUBE = 0x8DCC,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_2D_RECT = 0x8DCD,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_1D_ARRAY = 0x8DCE,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_2D_ARRAY = 0x8DCF,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_BUFFER = 0x8DD0,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_1D = 0x8DD1,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_2D = 0x8DD2,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_3D = 0x8DD3,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_CUBE = 0x8DD4,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_2D_RECT = 0x8DD5,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_1D_ARRAY = 0x8DD6,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_2D_ARRAY = 0x8DD7,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_BUFFER = 0x8DD8,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT2 = 0x8F46,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT3 = 0x8F47,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT4 = 0x8F48,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT2x3 = 0x8F49,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT2x4 = 0x8F4A,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT3x2 = 0x8F4B,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT3x4 = 0x8F4C,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT4x2 = 0x8F4D,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_MAT4x3 = 0x8F4E,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_VEC2 = 0x8FFC,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_VEC3 = 0x8FFD,
		eGLSL_ATTRIBUTE_TYPE_DOUBLE_VEC4 = 0x8FFE,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_2D_MULTISAMPLE = 0x9108,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_2D_MULTISAMPLE = 0x9109,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE = 0x910A,
		eGLSL_ATTRIBUTE_TYPE_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910B,
		eGLSL_ATTRIBUTE_TYPE_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910C,
		eGLSL_ATTRIBUTE_TYPE_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910D,
	}	eGLSL_ATTRIBUTE_TYPE;

	typedef enum eGLSL_OBJECT
		: uint32_t
	{
		eGLSL_OBJECT_ACTIVE_ATTRIBUTES,
		eGLSL_OBJECT_ACTIVE_UNIFORMS,
	}	eGLSL_OBJECT;

	typedef enum eGL_TEXTURE_STORAGE
		: uint32_t
	{
		eGL_TEXTURE_STORAGE_1D = 0x0DE0,
		eGL_TEXTURE_STORAGE_2D = 0x0DE1,
		eGL_TEXTURE_STORAGE_3D = 0x806F,
		eGL_TEXTURE_STORAGE_CUBE_MAP = 0x8513,
		eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX = 0x8515,
		eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGX = 0x8516,
		eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSY = 0x8517,
		eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGY = 0x8518,
		eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSZ = 0x8519,
		eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGZ = 0x851A,
		eGL_TEXTURE_STORAGE_CUBE_MAP_PROXY = 0x851B,
		eGL_TEXTURE_STORAGE_2DARRAY = 0x8C1A,
		eGL_TEXTURE_STORAGE_BUFFER = 0x8C2A,
		eGL_TEXTURE_STORAGE_CUBE_MAP_ARRAY = 0x9009,
		eGL_TEXTURE_STORAGE_2DMS = 0x9100,
	}	eGL_TEXTURE_STORAGE;

	typedef enum eGL_ATTRIBS_LAYOUT
		: uint32_t
	{
		eGL_ATTRIBS_LAYOUT_INTERLEAVED = 0x8C8C,
		eGL_ATTRIBS_LAYOUT_SEPARATE = 0x8C8D,
	}	eGL_ATTRIBS_LAYOUT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to create an object on GPU
	*/
	template< typename CreateFunction >
	struct CreatorHelper;

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to destroy an object on GPU
	*/
	template< typename DestroyFunction >
	struct DestroyerHelper;

	class GlRenderSystem;
	class GlAttributeBase;
	template< typename T, uint32_t Columns, uint32_t Rows > class GlMatAttribute;
	template< typename T, uint32_t Count > class GlVecAttribute;
	template< typename T > class GlBufferBase;
	template< typename T > class GlBuffer;
	class GlVertexBufferObject;
	class Gl3VertexBufferObject;
	class GlGeometryBuffers;
	class GlIndexArray;
	class GlVertexArray;
	class GlIndexBufferObject;
	class GlMatrixBufferObject;
	class GlTextureBufferObject;
	typedef GlMatAttribute< real, 4, 4 > GlAttributeMat4;
	typedef GlMatAttribute< real, 3, 3 > GlAttributeMat3;
	typedef GlMatAttribute< real, 2, 2 > GlAttributeMat2;
	typedef GlVecAttribute< real, 4 > GlAttributeVec4r;
	typedef GlVecAttribute< real, 3 > GlAttributeVec3r;
	typedef GlVecAttribute< real, 2 > GlAttributeVec2r;
	typedef GlVecAttribute< real, 1 > GlAttributeVec1r;
	typedef GlVecAttribute< int, 4 > GlAttributeVec4i;
	typedef GlVecAttribute< int, 3 > GlAttributeVec3i;
	typedef GlVecAttribute< int, 2 > GlAttributeVec2i;
	typedef GlVecAttribute< int, 1 > GlAttributeVec1i;
	typedef GlVecAttribute< uint32_t, 4 > GlAttributeVec4ui;
	typedef GlVecAttribute< uint32_t, 3 > GlAttributeVec3ui;
	typedef GlVecAttribute< uint32_t, 2 > GlAttributeVec2ui;
	typedef GlVecAttribute< uint32_t, 1 > GlAttributeVec1ui;
	DECLARE_SMART_PTR( GlAttributeBase );
	DECLARE_SMART_PTR( GlIndexArray );
	DECLARE_SMART_PTR( GlVertexArray );
	DECLARE_SMART_PTR( GlIndexBufferObject );
	DECLARE_SMART_PTR( GlVertexBufferObject );
	DECLARE_SMART_PTR( GlTextureBufferObject );
	DECLARE_VECTOR(	GlAttributeBaseSPtr, GlAttributePtr );
	DECLARE_VECTOR(	GlTextureBufferObjectSPtr, GlTextureBufferObjectPtr );

	class GlShaderObject;
	class GlShaderProgram;
	class GlProgramInputLayout;
	template< typename Type > struct OneVariableBinder;
	template< typename Type, uint32_t Count > struct PointVariableBinder;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder;
	class GlFrameVariableBase;
	struct GlVariableApplyerBase;
	class UboVariableInfos;
	class GlFrameVariableBuffer;
	template< typename T > class GlOneFrameVariable;
	template< typename T, uint32_t Count > class GlPointFrameVariable;
	template< typename T, uint32_t Rows, uint32_t Columns > class GlMatrixFrameVariable;
	DECLARE_SMART_PTR( UboVariableInfos );
	DECLARE_SMART_PTR( GlVariableApplyerBase );
	DECLARE_SMART_PTR( GlFrameVariableBase );
	DECLARE_SMART_PTR( GlShaderObject );
	DECLARE_SMART_PTR( GlShaderProgram );
	DECLARE_VECTOR( GlShaderProgramSPtr, GlShaderProgramPtr );
	DECLARE_VECTOR( GlShaderObjectSPtr, GlShaderObjectPtr );
	DECLARE_MAP( Castor::String, GlFrameVariableBaseSPtr, GlFrameVariablePtrStr );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, UboVariableInfosSPtr, UboVariableInfos );
	DECLARE_MAP( Castor3D::FrameVariableSPtr, GlVariableApplyerBaseSPtr, VariableApplyer );

	class GlRenderBuffer;
	class GlColourRenderBuffer;
	class GlDepthRenderBuffer;
	class GlStencilRenderBuffer;
	class GlRenderBufferAttachment;
	class GlTextureAttachment;
	class GlFrameBuffer;
	DECLARE_SMART_PTR( GlFrameBuffer );
	DECLARE_SMART_PTR( GlRenderBuffer );
	DECLARE_SMART_PTR( GlColourRenderBuffer );
	DECLARE_SMART_PTR( GlDepthRenderBuffer );
	DECLARE_SMART_PTR( GlTextureAttachment );
	DECLARE_SMART_PTR( GlRenderBufferAttachment );
	DECLARE_VECTOR(	GlDepthRenderBufferSPtr, GlDepthRenderBufferPtr );
	DECLARE_VECTOR(	GlTextureAttachmentSPtr, GlTextureAttachmentPtr );
	DECLARE_VECTOR(	GlRenderBufferAttachmentSPtr, GlRenderBufferAttachmentPtr );

	class GlBillboardList;
	class GlBlendState;
	class GlDepthStencilState;
	class GlRasteriserState;
	class GlPipelineImpl;
	class GlContext;
	class GlContextImpl;
	class GlRenderSystem;
	DECLARE_SMART_PTR( GlContext );

	class GlTexture;
	template< typename Traits >
	class GlTextureStorage;
	class GlGpuIoBuffer;
	class GlDownloadPixelBuffer;
	class GlUploadPixelBuffer;
	DECLARE_SMART_PTR( GlGpuIoBuffer );
	DECLARE_SMART_PTR( GlDownloadPixelBuffer );
	DECLARE_SMART_PTR( GlUploadPixelBuffer );

	class GlOverlayRenderer;
	class GlRenderTarget;
	class GlRenderWindow;
	class OpenGl;
}

#	if !defined( NDEBUG )
#		define glCheckError( gl, txt )			( gl ).GlCheckError( txt )
#		define glTrack( gl, type, object )		( gl ).Track( object, type, __FILE__, __LINE__ )
#		define glUntrack( gl, object )			( gl ).UnTrack( object )
#	else
#		define glCheckError( gl, txt )			true
#		define glTrack( gl, type, object )
#		define glUntrack( gl, object )
#	endif

//#include "Common/OpenGl.hpp"
#endif
