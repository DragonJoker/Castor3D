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

	enum class GlProvider
	{
		eUnknown,
		eNvidia,
		eATI,
		eIntel,
	};

	enum class GlBufferBinding
		: uint32_t
	{
		eNone = 0x0000,
		eFrontLeft = 0x0400,
		eFrontRight = 0x0401,
		eBackLeft = 0x0402,
		eBackRight = 0x0403,
		eFront = 0x0404,
		eBack = 0x0405,
		eLeft = 0x0406,
		eRight = 0x0407,
		eFrontAndBack = 0x0408,
		eColor0 = 0x8CE0,
		eColor1 = 0x8CE1,
		eColor2 = 0x8CE2,
		eColor3 = 0x8CE3,
		eColor4 = 0x8CE4,
		eColor5 = 0x8CE5,
		eColor6 = 0x8CE6,
		eColor7 = 0x8CE7,
		eColor8 = 0x8CE8,
		eColor9 = 0x8CE9,
		eColor10 = 0x8CEA,
		eColor11 = 0x8CEB,
		eColor12 = 0x8CEC,
		eColor13 = 0x8CED,
		eColor14 = 0x8CEE,
		eColor15 = 0x8CEF,
		eDepth = 0x8D00,
		eStencil = 0x8D20,
	};

	enum class GlPatchParameter
		: uint32_t
	{
		eVertices = 0x8E72,
		eDefaultInnerLevel = 0x8E73,
		eDefaultOuterLevel = 0x8E74,
	};

	enum class GlTopology
		: uint32_t
	{
		ePoints = 0x0000,
		eLines = 0x0001,
		eLineLoop = 0x0002,
		eLineStrip = 0x0003,
		eTriangles = 0x0004,
		eTriangleStrip = 0x0005,
		eTriangleFan = 0x0006,
		eQuads = 0x0007,
		eQuadStrip = 0x0008,
		ePolygon = 0x0009,
		ePatches = 0x000E,
	};

	enum class GlInternal						//	Type	Comps	Norm	R	G	B	A
		: uint32_t
	{
		eOne = 0x0001,
		eTwo = 0x0002,
		eThree = 0x0003,
		eFour = 0x0004,
		eR3G3B2 = 0x2A10,
		eAlpha4 = 0x803B,
		eAlpha8 = 0x803C,
		eAlpha12 = 0x803D,
		eAlpha16 = 0x803E,
		eCompressedAlpha = 0x84E9,
		eCompressedLuminance = 0x84EA,
		eCompressedLuminanceAlpha = 0x84EB,
		eCompressedIntensity = 0x84EC,
		eCompressedRGB = 0x84ED,
		eCompressedRGBA = 0x84EE,
		eL4 = 0x803F,
		eL8 = 0x8040,
		eL12 = 0x8041,
		eL16 = 0x8042,
		eL4A4 = 0x8043,
		eL6A2 = 0x8044,
		eL8A8 = 0x8045,
		eL12A4 = 0x8046,
		eL12A12 = 0x8047,
		eL16A16 = 0x8048,
		eIntensity = 0x8049,
		eI4 = 0x804A,
		eI8 = 0x804B,
		eI12 = 0x804C,
		eI16 = 0x804D,
		eRGB4 = 0x804F,
		eRGB5 = 0x8050,
		eRGB8 = 0x8051,
		eRGB10 = 0x8052,
		eRGB12 = 0x8053,
		eRGB16 = 0x8054,
		eRGBA2 = 0x8055,
		eRGBA4 = 0x8056,
		eRGB5A1 = 0x8057,
		eRGBA8 = 0x8058,						//	uint		4	YES		R	G	B	A
		eRGB10A2 = 0x8059,
		eRGBA12 = 0x805A,
		eRGBA16 = 0x805B,						//	short		4	YES		R	G	B	A
		eRGB565 = 0x8D62,
		eD16 = 0x81A5,
		eD24 = 0x81A6,
		eD32 = 0x81A7,
		eR8 = 0x8229,							//	ubyte		1	YES		R	0	0	1
		eR16 = 0x822A,							//	ushort		1	YES		R	0	0	1
		eRG8 = 0x822B,							//	ubyte		2	YES		R	G	0	1
		eRG16 = 0x822C,							//	ushort		2	YES		R	G	0	1
		eR16F = 0x822D,							//	half		1	NO		R	0	0	1
		eR32F = 0x822E,							//	float		1	NO		R	0	0	1
		eRG16F = 0x822F,						//	half		2	NO		R	G	0	1
		eRG32F = 0x8230,						//	float		2	NO		R	G	0	1
		eR8I = 0x8231,							//	byte		1	NO		R	0	0	1
		eR8UI = 0x8232,							//	ubyte		1	NO		R	0	0	1
		eR16I = 0x8233,							//	short		1	NO		R	0	0	1
		eR16UI = 0x8234,						//	ushort		1	NO		R	0	0	1
		eR32I = 0x8235,							//	int			1	NO		R	0	0	1
		eR32UI = 0x8236,						//	uint		1	NO		R	0	0	1
		eRG8I = 0x8237,							//	byte		2	NO		R	G	0	1
		eRG8UI = 0x8238,						//	ubyte		2	NO		R	G	0	1
		eRG16I = 0x8239,						//	short		2	NO		R	G	0	1
		eRG16UI = 0x823A,						//	ushort		2	NO		R	G	0	1
		eRG32I = 0x823B,						//	int			2	NO		R	G	0	1
		eRG32UI = 0x823C,						//	uint		2	NO		R	G	0	1
		eDXT1 = 0x83F1,
		eDXT3 = 0x83F2,
		eDXT5 = 0x83F3,
		eRGBA32F = 0x8814,						//	float		4	NO		R	G	B	A
		eRGB32F = 0x8815,
		eRGBA16F = 0x881A,						//	half		4	NO		R	G	B	A
		eRGB16F = 0x881B,
		eD24S8 = 0x88F0,
		eSRGB = 0x8C40,
		eSRGB8 = 0x8C41,
		eSRGBA = 0x8C42,
		eSRGB8A8 = 0x8C43,
		eLA = 0x8C44,
		eSL8A8 = 0x8C45,
		eSLuminance = 0x8C46,
		eSL8 = 0x8C47,
		eD32F = 0x8CAC,
		eS1 = 0x8D46,
		eS4 = 0x8D47,
		eS8 = 0x8D48,
		eS16 = 0x8D49,
		eRGBA32UI = 0x8D70,						//	uint		4	NO		R	G	B	A
		eRGBA16UI = 0x8D76,						//	ushort		4	NO		R	G	B	A
		eRGB16UI = 0x8D77,						//	ushort		3	NO		R	G	B	1
		eRGBA8UI = 0x8D7C,						//	ubyte		4	NO		R	G	B	A
		eRGBA32I = 0x8D82,						//	int			4	NO		R	G	B	A
		eRGBA16I = 0x8D88,						//	short		4	NO		R	G	B	A
		eRGBA8I = 0x8D8E,						//	byte		4	NO		R	G	B	A
	};

	enum class GlTexDim
		: uint32_t
	{
		eBuffer = 0x8C2A,
		e1D = 0x0DE0,
		e1DArray = 0x8C18,
		e2D = 0x0DE1,
		e2DArray = 0x8C1A,
		e2DMS = 0x9100,
		e2DMSArray = 0x9102,
		e3D = 0x806F,
		eCube = 0x8513,
		ePositiveX = 0x8515,
		eNegativeX = 0x8516,
		ePositiveY = 0x8517,
		eNegativeY = 0x8518,
		ePositiveZ = 0x8519,
		eNegativeZ = 0x851A,
		eCubeArray = 0x9009,
	};

	enum class GlComparator
		: uint32_t
	{
		eNever = 0x0200,
		eLess = 0x0201,
		eEqual = 0x0202,
		eLEqual = 0x0203,
		eGreater = 0x0204,
		eNEqual = 0x0205,
		eGEqual = 0x0206,
		eAlways = 0x0207,
	};

	enum class GlWrapMode
		: uint32_t
	{
		eClamp = 0x2900,
		eRepeat = 0x2901,
		eClampToBorder = 0x812D,
		eClampToEdge = 0x812F,
		eMirroredRepeat = 0x8370,
	};

	enum class GlInterpolationMode
		: uint32_t
	{
		eNearest = 0x2600,
		eLinear = 0x2601,
		eNearestMipmapNearest = 0x2700,
		eLinearMipmapNearest = 0x2701,
		eNearestMipmapLinear = 0x2702,
		eLinearMipmapLinear = 0x2703,
	};

	enum class GlBlendFactor
		: uint32_t
	{
		eZero = 0x0000,
		eOne = 0x0001,
		eSrcColour = 0x0300,
		eInvSrcColour = 0x0301,
		eSrcAlpha = 0x0302,
		eInvSrcAlpha = 0x0303,
		eDstAlpha = 0x0304,
		eInvDstAlpha = 0x0305,
		eDstColour = 0x0306,
		eInvDstColour = 0x0307,
		eSrcAlphaSaturate = 0x0308,
		eConstant = 0x8001,
		eInvConstant = 0x8002,
		eSrc1Alpha = 0x8589,
		eSrc1Colour = 0x88F9,
		eInvSrc1Colour = 0x88FA,
		eInvSrc1Alpha = 0x88FB,
	};

	enum class GlTextureIndex
		: uint32_t
	{
		eIndex0 = 0x84C0,
		eIndex1 = 0x84C1,
		eIndex2 = 0x84C2,
		eIndex3 = 0x84C3,
		eIndex4 = 0x84C4,
		eIndex5 = 0x84C5,
		eIndex6 = 0x84C6,
		eIndex7 = 0x84C7,
		eIndex8 = 0x84C8,
		eIndex9 = 0x84C9,
		eIndex10 = 0x84CA,
		eIndex11 = 0x84CB,
		eIndex12 = 0x84CC,
		eIndex13 = 0x84CD,
		eIndex14 = 0x84CE,
		eIndex15 = 0x84CF,
		eIndex16 = 0x84D0,
		eIndex17 = 0x84D1,
		eIndex18 = 0x84D2,
		eIndex19 = 0x84D3,
		eIndex20 = 0x84D4,
		eIndex21 = 0x84D5,
		eIndex22 = 0x84D6,
		eIndex23 = 0x84D7,
		eIndex24 = 0x84D8,
		eIndex25 = 0x84D9,
		eIndex26 = 0x84DA,
		eIndex27 = 0x84DB,
		eIndex28 = 0x84DC,
		eIndex29 = 0x84DD,
		eIndex30 = 0x84DE,
		eIndex31 = 0x84DF,
	};

	enum class GlBlendSource
		: uint32_t
	{
		eTexture = 0x1702,
		eTexture0 = 0x84C0,
		eTexture1 = 0x84C1,
		eTexture2 = 0x84C2,
		eTexture3 = 0x84C3,
		eConstant = 0x8576,
		eColour = 0x8577,
		ePrevious = 0x8578,
	};

	enum class GlBlendFunc
		: uint32_t
	{
		eAdd = 0x0104,
		eReplace = 0x1E01,
		eModulate = 0x2100,
		eSubtract = 0x84E7,
		eAddSigned = 0x8574,
		eInterpolate = 0x8575,
		eDot3RGB = 0x86AE,
		eDot3RGBA = 0x86AF,
	};

	enum class GlBlendOp
		: uint32_t
	{
		eAdd = 0x8006,
		eMin = 0x8007,
		eMax = 0x8008,
		eSubtract = 0x800A,
		eRevSubtract = 0x800B,
	};

	enum class GlBufferMode
		: uint32_t
	{
		eStreamDraw = 0x88E0,
		eStreamRead = 0x88E1,
		eStreamCopy = 0x88E2,
		eStaticDraw = 0x88E4,
		eStaticRead = 0x88E5,
		eStaticCopy = 0x88E6,
		eDynamicDraw = 0x88E8,
		eDynamicRead = 0x88E9,
		eDynamicCopy = 0x88EA,
	};

	enum class GlComponent
		: uint32_t
	{
		eColour = 0x1900,
		eStencil = 0x1901,
		eDepth = 0x1902,
	};

	enum class GlBufferTarget
		: uint32_t
	{
		eArray = 0x8892,
		eElementArray = 0x8893,
		ePixelPack = 0x88EB,
		ePixelUnpack = 0x88EC,
		eTexture = 0x8C2A,
		eUniform = 0x8A11,
		eTransformFeedback = 0x8E22,
		eTransformFeedbackBuffer = 0x8C8E,
		eRead = 0x8F36,
		eWrite = 0x8F37,
		eAtomicCounter = 0x92C0,
		eShaderStorage = 0x90D2,
	};

	enum class GlShaderType
		: uint32_t
	{
		eFragment = 0x8B30,
		eVertex = 0x8B31,
		eGeometry = 0x8DD9,
		eTessEvaluation = 0x8E87,
		eTessControl = 0x8E88,
		eCompute = 0x91B9,
	};

	enum class GlAccessType
		: uint32_t
	{
		eRead = 0x88B8,
		eWrite = 0x88B9,
		eReadWrite = 0x88BA,
	};

	enum class GlUniformBlockValue
		: uint32_t
	{
		eBinding = 0x8A3F,
		eDataSize = 0x8A40,
		eNameLength = 0x8A41,
		eActiveUniforms = 0x8A42,
		eActiveUniformIndices = 0x8A43,
		eReferencedByVertexShader = 0x8A44,
		eReferencedByGeometryShader = 0x8A45,
		eReferencedByFragmentShader = 0x8A46,
		eReferencedByTessControlShader = 0x84F0,
		eReferencedByTessEvaluationShader = 0x84F1,
		eReferencedByComputeShader = 0x90EC,
	};

	enum class GlUniformValue
		: uint32_t
	{
		eType = 0x8A37,
		eSize = 0x8A38,
		eNameLength = 0x8A39,
		eBlockIndex = 0x8A3A,
		eOffset = 0x8A3B,
		eArrayStride = 0x8A3C,
		eMatrixStride = 0x8A3D,
		eIsRowMajor = 0x8A3E,
		eGpuAddress = 0x8F34,
		eAtomicCounterBufferIndex = 0x92DA
	};

	enum class GlContextAttribute
		: uint32_t
	{
		eDebugBit = 0x0001,
		eForwardCompatibleBit = 0x0002,
		eMajorVersion = 0x2091,
		eMinorVersion = 0x2092,
		eLayerPlane = 0x2093,
		eFlags = 0x2094,
	};

	enum class GlProfileAttribute
		: uint32_t
	{
		eCoreBit = 0x0001,
		eCompatibilityBit = 0x0002,
		eMask = 0x9126,
	};

	enum class GlShaderStatus
		: uint32_t
	{
		eDelete = 0x8B80,
		eCompile = 0x8B81,
		eLink = 0x8B82,
		eValidate = 0x8B83,
		eInfoLogLength = 0x8B84,
		eAttachedShaders = 0x8B85,
		eActiveUniforms = 0x8B86,
		eSourceLength = 0x8B88,
		eActiveAttributes = 0x8B89,
		eActiveAttributeMaxLength = 0x8B8A,
	};

	enum class GlType
		: uint32_t
	{
		eDefault = 0x0000,
		eByte = 0x1400,
		eUnsignedByte = 0x1401,
		eShort = 0x1402,
		eUnsignedShort = 0x1403,
		eInt = 0x1404,
		eUnsignedInt = 0x1405,
		eFloat = 0x1406,
		eDouble = 0x140A,
		eHalfFloat = 0x140B,
		eBitmap = 0X1A00,
		eUnsignedByte332 = 0x8032,
		eUnsignedShort4444 = 0x8033,
		eUnsignedShort5551 = 0x8034,
		eUnsignedInt8888 = 0x8035,
		eUnsignedInt1010102 = 0x8036,
		eUnsignedByte233Rev = 0x8362,
		eUnsignedShort565 = 0x8363,
		eUnsignedShort565Rev = 0x8364,
		eUnsignedShort4444Rev = 0x8365,
		eUnsignedShort1555Rev = 0x8366,
		eUnsignedInt8888Rev = 0x8367,
		eUnsignedInt2101010Rev = 0x8368,
		eUnsignedInt248 = 0x84FA,
#if CASTOR_USE_DOUBLE
		eReal = eDouble
#else
		eReal = eFloat
#endif
	};

	enum class GlFormat
		: uint32_t
	{
		eStencil = 0x1901,
		eDepth = 0x1902,
		eRed = 0x1903,
		eGreen = 0x1904,
		eBlue = 0x1905,
		eAlpha = 0x1906,
		eRGB = 0x1907,
		eRGBA = 0x1908,
		eLuminance = 0x1909,
		eLuminanceAlpha = 0x190A,
		eBGR = 0x80E0,
		eBGRA = 0x80E1,
		eRG = 0x8227,
		eDepthStencil = 0x84F9,
	};

	enum class GlTweak
		: uint32_t
	{
		eLineSmooth = 0x0B20,
		eCullFace = 0x0B44,
		eDepthTest = 0x0B71,
		eStencilTest = 0x0B90,
		eAlphaTest = 0x0BC0,
		eDither = 0x0BD0,
		eBlend = 0x0BE2,
		eScissorTest = 0x0C11,
		eOffsetFill = 0x8037,
		eMultisample = 0x809D,
		eAlphaToCoverage = 0x809E,
		eDebugOutputSynchronous = 0x8242,
		eDepthClamp = 0x864F,
		eRasterizerDiscard = 0x8C89,
	};

	enum class GlFramebufferStatus
		: uint32_t
	{
		eInvalid = 0xFFFFFFFF,
		eComplete = 0x8CD5,
		eIncompleteAttachment = 0x8CD6,
		eIncompleteMissingAttachment = 0x8CD7,
		eIncompleteDrawBuffer = 0x8CDB,
		eIncompleteReadBuffer = 0x8CDC,
		eUnsupported = 0x8CDD,
		eIncompleteMultisample = 0x8D56,
		eIncompleteLayerTargets = 0x8DA8,
	};

	enum class GlAttachmentPoint
		: uint32_t
	{
		eNone = 0x0000,
		eColour0 = 0x8CE0,
		eDepth = 0x8D00,
		eStencil = 0x8D20,
	};

	enum class GlFrameBufferMode
		: uint32_t
	{
		eRead = 0x8CA8,
		eDraw = 0x8CA9,
		eDefault = 0x8D40,
	};

	enum class GlRenderBufferMode
		: uint32_t
	{
		eDefault = 0x8D41
	};

	enum class GlRenderBufferParameter
		: uint32_t
	{
		eWidth = 0x8D42,
		eHeight = 0x8D43,
		eInternalFormat = 0x8D44,
		eRedSize = 0x8D50,
		eGreenSize = 0x8D51,
		eBlueSize = 0x8D52,
		eAlphaSize = 0x8D53,
		eDepthSize = 0x8D54,
		eStencilSize = 0x8D55,
	};

	enum class GlFrontFaceDirection
		: uint32_t
	{
		eClockWise = 0x0900,
		eCounterClockWise = 0x0901,
	};

	enum class GlBufferBit
		: uint32_t
	{
		eColour = 0x00004000,
		eDepth = 0x00000100,
		eStencil = 0x00000400,
	};

	constexpr uint32_t GlInvalidIndex = 0xFFFFFFFF;

	enum class GlBufferMappingBit
		: uint32_t
	{
		eRead = 0x0001,
		eWrite = 0x0002,
		eInvalidateRange = 0x0004,
		eInvalidateBuffer = 0x0008,
		eFlushExplicit = 0x0010,
		eUnsynchronised = 0x0020,
	};

	enum class GlFace
		: uint32_t
	{
		eFront = 0x0404,
		eBack = 0x0405,
		eBoth = 0x0408,
	};

	enum class GlStencilOp
		: uint32_t
	{
		eZero = 0x0000,
		eInvert = 0x150A,
		eKeep = 0x1E00,
		eReplace = 0x1E01,
		eIncrement = 0x1E02,
		eDecrement = 0x1E03,
		eIncrementWrap = 0x8507,
		eDecrementWrap = 0x8508,
	};

	enum class GlFillMode
		: uint32_t
	{
		ePoint = 0x1B00,
		eLine = 0x1B01,
		eFill = 0x1B02,
	};

	enum class GlCompareMode
		: uint32_t
	{
		eNone = 0,
		eRefToTexture = 0x884E
	};

	enum class GlSamplerParameter
		: uint32_t
	{
		eBorderColour = 0x1004,
		eMagFilter = 0x2800,
		eMinFilter = 0x2801,
		eUWrap = 0x2802,
		eVWrap = 0x2803,
		eWWrap = 0x8072,
		eMinLOD = 0x813A,
		eMaxLOD = 0x813B,
		eMaxAnisotropy = 0x84FE,
		eLODBias = 0x8501,
		eCompareMode = 0x884C,
		eCompareFunc = 0x884D,
	};

	enum class GlDebugType
		: uint32_t
	{
		eError = 0x824C,
		eDeprecatedBehavior = 0x824D,
		eUndefinedBehavior = 0x824E,
		ePortability = 0x824F,
		ePerformance = 0x8250,
		eOther = 0x8251,
	};

	enum class GlDebugSource
		: uint32_t
	{
		eAPI = 0x8246,
		eWindowSystem = 0x8247,
		eShaderCompiler = 0x8248,
		eThirdParty = 0x8249,
		eApplication = 0x824A,
		eOther = 0x824B,
	};

	enum class GlDebugCategory
		: uint32_t
	{
		eAPIError = 0x9149,
		eWindowSystem = 0x914A,
		eDeprecation = 0x914B,
		eUndefinedBehavior = 0x914C,
		ePerformance = 0x914D,
		eShaderCompiler = 0x914E,
		eApplication = 0x914F,
		eOther = 0x9150,
	};

	enum class GlDebugSeverity
		: uint32_t
	{
		eHigh = 0x9146,
		eMedium = 0x9147,
		eLow = 0x9148,
	};

	enum class GlMin
		: uint32_t
	{
		eProgramTexelOffset = 0x8904,
		eMapBufferAlignment = 0x90BC,
	};

	enum class GlMax
		: uint32_t
	{
		eComputeShaderStorageBlocks = 0x90DB,
		eCombinedShaderStorageBlocks = 0x90DC,
		eComputeUniformBlocks = 0x91BB,
		eComputeTextureImageUnits = 0x91BC,
		eComputeUniformComponents = 0x8263,
		eComputeAtomicCounters = 0x8265,
		eComputeAtomicCounterBuffers = 0x8264,
		eCombinedComputeUniformComponents = 0x8266,
		eComputeWorkGroupInvocations = 0x90EB,
		eComputeWorkGroupCount = 0x91BE,
		eComputeWorkGroupSize = 0x91BF,
		eDebugGroupStackDepth = 0x826C,
		eTexture3DSize = 0x8073,
		eTextureArrayLayers = 0x88FF,
		eClipDistances = 0x0D32,
		eColorTextureSamples = 0x910E,
		eCombinedAtomicCounters = 0x92D7,
		eCombinedFragmentUniformComponents = 0x8A33,
		eCombinedGeometryUniformComponents = 0x8A32,
		eCombinedTextureImageUnits = 0x8B4D,
		eCombinedUniformBlocks = 0x8A2E,
		eCombinedVertexUniformComponents = 0x8A31,
		eTextureCubeSize = 0x851C,
		eDepthTextureSamples = 0x910F,
		eDrawBuffers = 0x8824,
		eDualSourceDrawBuffers = 0x88FC,
		eElementsIndices = 0x80E9,
		eElementsVertices = 0x80E8,
		eFragmentAtomicCounters = 0x92D6,
		eFragmentShaderStorageBlocks = 0x90DA,
		eFragmentInputComponents = 0x9125,
		eFragmentUniformComponents = 0x8B49,
		eFragmentUniformVectors = 0x8DFD,
		eFragmentUniformBlocks = 0x8A2D,
		eFramebufferWidth = 0x9315,
		eFramebufferHeight = 0x9316,
		eFramebufferLayers = 0x9317,
		eFramebufferSamples = 0x9318,
		eGeometryAtomicCounters = 0x92D5,
		eGeometryShaderStorageBlocks = 0x90D7,
		eGeometryInputComponents = 0x9123,
		eGeometryOutputComponents = 0x9124,
		eGeometryTextureImageUnits = 0x8C29,
		eGeometryUniformBlocks = 0x8A2C,
		eGeometryUniformComponents = 0x8DDF,
		eIntegerSamples = 0x9110,
		eLabelLength = 0x82E8,
		eProgramTexelOffset = 0x8905,
		eRectangleTextureSize = 0x84F8,
		eRenderbufferSize = 0x84E8,
		eSampleMaskWords = 0x8E59,
		eSamples = 0x8D57,
		eServerWaitTimeout = 0x9111,
		eShaderStorageBufferBindings = 0x90DD,
		eTessControlAtomicCounters = 0x92D3,
		eTessEvaluationAtomicCounters = 0x92D4,
		eTessControlShaderStorageBlocks = 0x90D8,
		eTessEvaluationShaderStorageBlocks = 0x90D9,
		eTextureBufferSize = 0x8C2B,
		eTextureImageUnits = 0x8872,
		eTextureLODBias = 0x84FD,
		eTextureSize = 0x0D33,
		eUniformBufferBindings = 0x8A2F,
		eUniformBlockSize = 0x8A30,
		eUniformLocations = 0x826E,
		eVaryingComponents = 0x8B4B,
		eVaryingVectors = 0x8DFC,
		eVaryingFloats = 0x8B4B,
		eVertexAtomicCounters = 0x92D2,
		eVertexAttribs = 0x8869,
		eVertexShaderStorageBlocks = 0x90D6,
		eVertexTextureImageUnits = 0x8B4C,
		eVertexUniformComponents = 0x8B4A,
		eVertexUniformVectors = 0x8DFB,
		eVertexOutputComponents = 0x9122,
		eVertexUniformBlocks = 0x8A2B,
		eViewportDims = 0x0D3A,
		eViewports = 0x825B,
	};

	enum class GlGpuInfo
	{
		eTotalAvailableMemNVX = 0x9048,
		eVBOFreeMemoryATI = 0x87FB,
		eTextureFreeMemoryATI = 0x87FC,
		eRenderbufferFreeMemoryATI = 0x87FD,
	};

	enum class GlHint
	{
		eLineSmooth = 0x0C52,
		ePolygonSmooth = 0x0C53,
		eVolumeClipping = 0x80F0,
		eCompression = 0x84EF,
		eShaderDerivative = 0x8B8B,
	};

	enum class GlHintValue
	{
		eDontCare = 0x1100,
		eFastest = 0x1101,
		eNicest = 0x1102,
	};

	enum class GlStorageMode
		: uint32_t
	{
		eUnpackSwapBytes = 0x0CF0,
		eUnpackLSBFirst = 0x0CF1,
		eUnpackRowLength = 0x0CF2,
		eUnpackSkipRows = 0x0CF3,
		eUnpackSkipPixels = 0x0CF4,
		eUnpackAlignment = 0x0CF5,
		ePackSwapBytes = 0x0D00,
		ePackLSBFirst = 0x0D01,
		ePackRowLength = 0x0D02,
		ePackSkipRows = 0x0D03,
		ePackSkipPixels = 0x0D04,
		ePackAlignment = 0x0D05,
		ePackImageHeight = 0x806C,
		eUnpackImageHeight = 0x806E,
	};

	enum class GlBufferParameter
		: uint32_t
	{
		eSize = 0x8764,
		eUsage = 0x8765,
		eAccess = 0x88BB,
		eMapped = 0x88BD,
		eGPUAddress = 0x8F1D,
	};

	enum class GlAddress
		: uint32_t
	{
		eVertexAttribArray = 0x8F20,
		eTextureCoordArray = 0x8F25,
		eVertexArray = 0x8F21,
		eNormalArray = 0x8F22,
		eColourArray = 0x8F23,
		eIndexArray = 0x8F24,
		eEdgeFlagArray = 0x8F26,
		eSecondaryColourArray = 0x8F27,
		eFogCoordArray = 0x8F28,
		eElementArray = 0x8F29,
	};

	enum class GlQueryType
		: uint32_t
	{
		eTimeElapsed = 0x88BF,
		eSamplesPassed = 0x8914,
		eAnySamplesPassed = 0x8C2F,
		ePrimitivesGenerated = 0x8C87,
		eTransformFeedbackPrimitivesWritten = 0x8C88,
		eAnySamplesPassedConservative = 0x8D6A,
		eTimestamp = 0x8E28,
	};

	enum class GlQueryInfo
		: uint32_t
	{
		eResult = 0x8866,
		eResultAvailable = 0x8867,
		eResultNoWait = 0x9194,
	};

	enum class GlslInterface
		: uint32_t
	{
		eAtomicCounterBuffer = 0x92C0,
		eUniform = 0x92E1,
		eUniformBlock = 0x92E2,
		eProgramInput = 0x92E3,
		eProgramOutput = 0x92E4,
		eBufferVariable = 0x92E5,
		eShaderStorageBlock = 0x92E6,
		eVertexSubroutine = 0x92E8,
		eTessControlSubroutine = 0x92E9,
		eTessEvaluationSubroutine = 0x92EA,
		eGeometrySubroutine = 0x92EB,
		eFragmentSubroutine = 0x92EC,
		eComputeSubroutine = 0x92ED,
		eVertexSubroutineUniform = 0x92EE,
		eTessControlSubroutineUniform = 0x92EF,
		eTessEvaluationSubroutineUniform = 0x92F0,
		eGeometrySubroutineUniform = 0x92F1,
		eFragmentSubroutineUniform = 0x92F2,
		eComputeSubroutineUniform = 0x92F3,
		eTransformFeedbackVarying = 0x92F4,
	};

	enum class GlslDataName
		: uint32_t
	{
		eActiveResources = 0x92F5,
		eMaxNameLength = 0x92F6,
		eMaxNumActiveVariables = 0x92F7,
		eMaxNumCompatibleSubroutines = 0x92F8,
	};

	enum class GlslProperty
		: uint32_t
	{
		eNumCompatibleSubroutines = 0x8E4A,
		eCompatibleSubroutines = 0x8E4B,
		eIsPerPatch = 0x92E7,
		eNameLength = 0x92F9,
		eType = 0x92FA,
		eArraySize = 0x92FB,
		eOffset = 0x92FC,
		eBlockIndex = 0x92FD,
		eArrayStride = 0x92FE,
		eMatrixStride = 0x92FF,
		eIsRowMajor = 0x9300,
		eAtomicCounterBufferIndex = 0x9301,
		eBufferBinding = 0x9302,
		eBufferDataSize = 0x9303,
		eNumActiveVariables = 0x9304,
		eActiveVariables = 0x9305,
		eReferencedByVertexShader = 0x9306,
		eReferencedByTessControlShader = 0x9307,
		eReferencedByTessEvaluationShader = 0x9308,
		eReferencedByGeometryShader = 0x9309,
		eReferencedByFragmentShader = 0x930A,
		eReferencedByComputeShader = 0x930B,
		eTopLevelArraySize = 0x930C,
		eTopLevelArrayStride = 0x930D,
		eLocation = 0x930E,
		eLocationIndex = 0x930F,
		eLocationComponent = 0x934A,
		eTransformFeedbackBufferIndex = 0x934B,
		eTransformFeedbackBufferStride = 0x934C,
	};

	enum class GlslAttributeType
		: uint32_t
	{
		eInt = 0x1404,
		eUnsignedInt = 0x1405,
		eFloat = 0x1406,
		eDouble = 0x140A,
		eHalfFloat = 0x140B,
		eFloatVec2 = 0x8B50,
		eFloatVec3 = 0x8B51,
		eFloatVec4 = 0x8B52,
		eIntVec2 = 0x8B53,
		eIntVec3 = 0x8B54,
		eIntVec4 = 0x8B55,
		eBool = 0x8B56,
		eBoolVec2 = 0x8B57,
		eBoolVec3 = 0x8B58,
		eBoolVec4 = 0x8B59,
		eFloatMat2 = 0x8B5A,
		eFloatMat3 = 0x8B5B,
		eFloatMat4 = 0x8B5C,
		eSampler1D = 0x8B5D,
		eSampler2D = 0x8B5E,
		eSampler3D = 0x8B5F,
		eSamplerCube = 0x8B60,
		eSampler1DShadow = 0x8B61,
		eSampler2DShadow = 0x8B62,
		eSampler2DRect = 0x8B63,
		eSampler2DRectShadow = 0x8B64,
		eMat2x3 = 0x8B65,
		eMat2x4 = 0x8B66,
		eMat3x2 = 0x8B67,
		eMat3x4 = 0x8B68,
		eMat4x2 = 0x8B69,
		eMat4x3 = 0x8B6A,
		eSampler1DArray = 0x8DC0,
		eSampler2DArray = 0x8DC1,
		eSamplerBuffer = 0x8DC2,
		eSampler1DArrayShadow = 0x8DC3,
		eSampler2DArrayShadow = 0x8DC4,
		eSamplerCubeShadow = 0x8DC5,
		eUnsignedIntVec2 = 0x8DC6,
		eUnsignedIntVec3 = 0x8DC7,
		eUnsignedIntVec4 = 0x8DC8,
		eIntSampler1D = 0x8DC9,
		eIntSampler2D = 0x8DCA,
		eIntSampler3D = 0x8DCB,
		eIntSamplerCube = 0x8DCC,
		eIntSampler2DRect = 0x8DCD,
		eIntSampler1DArray = 0x8DCE,
		eIntSampler2DArray = 0x8DCF,
		eIntSamplerBuffer = 0x8DD0,
		eUnsignedIntSampler1D = 0x8DD1,
		eUnsignedIntSampler2D = 0x8DD2,
		eUnsignedIntSampler3D = 0x8DD3,
		eUnsignedIntSamplerCube = 0x8DD4,
		eUnsignedIntSampler2DRect = 0x8DD5,
		eUnsignedIntSampler1DArray = 0x8DD6,
		eUnsignedIntSampler2DArray = 0x8DD7,
		eUnsignedIntSamplerBuffer = 0x8DD8,
		eDoubleMat2 = 0x8F46,
		eDoubleMat3 = 0x8F47,
		eDoubleMat4 = 0x8F48,
		eDoubleMat2x3 = 0x8F49,
		eDoubleMat2x4 = 0x8F4A,
		eDoubleMat3x2 = 0x8F4B,
		eDoubleMat3x4 = 0x8F4C,
		eDoubleMat4x2 = 0x8F4D,
		eDoubleMat4x3 = 0x8F4E,
		eDoubleVec2 = 0x8FFC,
		eDoubleVec3 = 0x8FFD,
		eDoubleVec4 = 0x8FFE,
		eSampler2DMultisample = 0x9108,
		eIntSampler2DMultisample = 0x9109,
		eUnsignedIntSampler2DMultisample = 0x910A,
		eSampler2DMultisampleArray = 0x910B,
		eIntSampler2DMultisampleArray = 0x910C,
		eUnsignedIntSampler2DMultisampleArray = 0x910D,
	};

	enum class GlslObject
		: uint32_t
	{
		eActiveAttributes,
		eActiveUniforms,
	};

	enum class GlTextureStorageType
		: uint32_t
	{
		e1D = 0x0DE0,
		e2D = 0x0DE1,
		e3D = 0x806F,
		eCubeMap = 0x8513,
		eCubeMapFacePosX = 0x8515,
		eCubeMapFaceNegX = 0x8516,
		eCubeMapFacePosY = 0x8517,
		eCubeMapFaceNegY = 0x8518,
		eCubeMapFacePosZ = 0x8519,
		eCubeMapFaceNegZ = 0x851A,
		eCubeMapProxy = 0x851B,
		e2DArray = 0x8C1A,
		eBuffer = 0x8C2A,
		eCubeMapArray = 0x9009,
		e2DMS = 0x9100,
	};

	enum class GlAttributeLayout
		: uint32_t
	{
		eInterleaved = 0x8C8C,
		eSeparate = 0x8C8D,
	};

	enum class GlBarrierBit
		: uint32_t
	{
		eVertexArrayAttrib = 0x00000001,
		eElementArray = 0x00000002,
		eUniform = 0x00000004,
		eTextureFetch = 0x00000008,
		eShaderImageAccess = 0x00000020,
		eCommand = 0x00000040,
		ePixelBuffer = 0x00000080,
		eTextureUpdate = 0x00000100,
		eBufferUpdate = 0x00000200,
		eFramebuffer = 0x00000400,
		eTransformFeedback = 0x00000800,
		eAtomicCounter = 0x00001000,
		eClientMappedBuffer = 0x00004000,
		eQueryBuffer = 0x00008000,
		eShaderStorage = 0x00002000,
		eAll = 0xFFFFFFFF,
	};

	IMPLEMENT_FLAGS( GlBarrierBit )

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
