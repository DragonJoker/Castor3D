/* See LICENSE file in root folder */
#ifndef ___3DS_IMPORTER_H___
#define ___3DS_IMPORTER_H___

#include <Mesh/Importer.hpp>

#include <Graphics/Colour.hpp>

namespace C3dSMax
{
	typedef enum eSMAX_CHUNK
	{
		eSMAX_CHUNK_NULL_CHUNK = 0x0000,
		eSMAX_CHUNK_UNKNOWN_CHUNK = 0x0001,
		eSMAX_CHUNK_M3D_VERSION = 0x0002,
		eSMAX_CHUNK_M3D_KFVERSION = 0x0005,
		eSMAX_CHUNK_COLOR_F = 0x0010,
		eSMAX_CHUNK_COLOR_24 = 0x0011,
		eSMAX_CHUNK_LIN_COLOR_24 = 0x0012,
		eSMAX_CHUNK_LIN_COLOR_F = 0x0013,
		eSMAX_CHUNK_INT_PERCENTAGE = 0x0030,
		eSMAX_CHUNK_FLOAT_PERCENTAGE = 0x0031,
		eSMAX_CHUNK_MASTER_SCALE = 0x0100,
		eSMAX_CHUNK_CHUNKTYPE = 0x0995,
		eSMAX_CHUNK_CHUNKUNIQUE = 0x0996,
		eSMAX_CHUNK_NOTCHUNK = 0x0997,
		eSMAX_CHUNK_CONTAINER = 0x0998,
		eSMAX_CHUNK_ISCHUNK = 0x0999,
		eSMAX_CHUNK_C_SXP_SELFI_MASKDATA = 0x0C3C,

		eSMAX_CHUNK_BIT_MAP = 0x1100,
		eSMAX_CHUNK_USE_BIT_MAP = 0x1101,
		eSMAX_CHUNK_SOLID_BGND = 0x1200,
		eSMAX_CHUNK_USE_SOLID_BGND = 0x1201,
		eSMAX_CHUNK_V_GRADIENT = 0x1300,
		eSMAX_CHUNK_USE_V_GRADIENT = 0x1301,
		eSMAX_CHUNK_LO_SHADOW_BIAS = 0x1400,
		eSMAX_CHUNK_HI_SHADOW_BIAS = 0x1410,
		eSMAX_CHUNK_SHADOW_MAP_SIZE = 0x1420,
		eSMAX_CHUNK_SHADOW_SAMPLES = 0x1430,
		eSMAX_CHUNK_SHADOW_RANGE = 0x1440,
		eSMAX_CHUNK_SHADOW_FILTER = 0x1450,
		eSMAX_CHUNK_RAY_BIAS = 0x1460,
		eSMAX_CHUNK_O_CONSTS = 0x1500,

		eSMAX_CHUNK_AMBIENT_LIGHT = 0x2100,
		eSMAX_CHUNK_FOG = 0x2200,
		eSMAX_CHUNK_USE_FOG = 0x2201,
		eSMAX_CHUNK_FOG_BGND = 0x2210,
		eSMAX_CHUNK_DISTANCE_CUE = 0x2300,
		eSMAX_CHUNK_USE_DISTANCE_CUE = 0x2301,
		eSMAX_CHUNK_LAYER_FOG = 0x2302,
		eSMAX_CHUNK_USE_LAYER_FOG = 0x2303,
		eSMAX_CHUNK_DCUE_BGND = 0x2310,
		eSMAX_CHUNK_SMAGIC = 0x2D2D,
		eSMAX_CHUNK_LMAGIC = 0x2D3D,

		eSMAX_CHUNK_DEFAULT_VIEW = 0x3000,
		eSMAX_CHUNK_VIEW_TOP = 0x3010,
		eSMAX_CHUNK_VIEW_BOTTOM = 0x3020,
		eSMAX_CHUNK_VIEW_LEFT = 0x3030,
		eSMAX_CHUNK_VIEW_RIGHT = 0x3040,
		eSMAX_CHUNK_VIEW_FRONT = 0x3050,
		eSMAX_CHUNK_VIEW_BACK = 0x3060,
		eSMAX_CHUNK_VIEW_USER = 0x3070,
		eSMAX_CHUNK_VIEW_CAMERA = 0x3080,
		eSMAX_CHUNK_VIEW_WINDOW = 0x3090,
		eSMAX_CHUNK_MDATA = 0x3D3D,
		eSMAX_CHUNK_MESH_VERSION = 0x3D3E,
		eSMAX_CHUNK_MLIBMAGIC = 0x3DAA,
		eSMAX_CHUNK_PRJMAGIC = 0x3DC2,
		eSMAX_CHUNK_MATMAGIC = 0x3DFF,

		eSMAX_CHUNK_NAMED_OBJECT = 0x4000,
		eSMAX_CHUNK_OBJ_HIDDEN = 0x4010,
		eSMAX_CHUNK_OBJ_VIS_LOFTER = 0x4011,
		eSMAX_CHUNK_OBJ_DOESNT_CAST = 0x4012,
		eSMAX_CHUNK_OBJ_MATTE = 0x4013,
		eSMAX_CHUNK_OBJ_FAST = 0x4014,
		eSMAX_CHUNK_OBJ_PROCEDURAL = 0x4015,
		eSMAX_CHUNK_OBJ_FROZEN = 0x4016,
		eSMAX_CHUNK_OBJ_DONT_RCVSHADOW = 0x4017,
		eSMAX_CHUNK_N_TRI_OBJECT = 0x4100,
		eSMAX_CHUNK_POINT_ARRAY = 0x4110,
		eSMAX_CHUNK_POINT_FLAG_ARRAY = 0x4111,
		eSMAX_CHUNK_FACE_ARRAY = 0x4120,
		eSMAX_CHUNK_MSH_MAT_GROUP = 0x4130,
		eSMAX_CHUNK_OLD_MAT_GROUP = 0x4131,
		eSMAX_CHUNK_TEX_VERTS = 0x4140,
		eSMAX_CHUNK_SMOOTH_GROUP = 0x4150,
		eSMAX_CHUNK_MESH_MATRIX = 0x4160,
		eSMAX_CHUNK_MESH_COLOR = 0x4165,
		eSMAX_CHUNK_MESH_TEXTURE_INFO = 0x4170,
		eSMAX_CHUNK_PROC_NAME = 0x4181,
		eSMAX_CHUNK_PROC_DATA = 0x4182,
		eSMAX_CHUNK_MSH_BOXMAP = 0x4190,
		eSMAX_CHUNK_N_D_L_OLD = 0x4400,
		eSMAX_CHUNK_N_CAM_OLD = 0x4500,
		eSMAX_CHUNK_N_DIRECT_LIGHT = 0x4600,
		eSMAX_CHUNK_DL_SPOTLIGHT = 0x4610,
		eSMAX_CHUNK_DL_OFF = 0x4620,
		eSMAX_CHUNK_DL_ATTENUATE = 0x4625,
		eSMAX_CHUNK_DL_RAYSHAD = 0x4627,
		eSMAX_CHUNK_DL_SHADOWED = 0x4630,
		eSMAX_CHUNK_DL_LOCAL_SHADOW = 0x4640,
		eSMAX_CHUNK_DL_LOCAL_SHADOW2 = 0x4641,
		eSMAX_CHUNK_DL_SEE_CONE = 0x4650,
		eSMAX_CHUNK_DL_SPOT_RECTANGULAR = 0x4651,
		eSMAX_CHUNK_DL_SPOT_OVERSHOOT = 0x4652,
		eSMAX_CHUNK_DL_SPOT_PROJECTOR = 0x4653,
		eSMAX_CHUNK_DL_EXCLUDE = 0x4654,
		eSMAX_CHUNK_DL_RANGE = 0x4655,
		eSMAX_CHUNK_DL_SPOT_ROLL = 0x4656,
		eSMAX_CHUNK_DL_SPOT_ASPECT = 0x4657,
		eSMAX_CHUNK_DL_RAY_BIAS = 0x4658,
		eSMAX_CHUNK_DL_INNER_RANGE = 0x4659,
		eSMAX_CHUNK_DL_OUTER_RANGE = 0x465A,
		eSMAX_CHUNK_DL_MULTIPLIER = 0x465B,
		eSMAX_CHUNK_N_AMBIENT_LIGHT = 0x4680,
		eSMAX_CHUNK_N_CAMERA = 0x4700,
		eSMAX_CHUNK_CAM_SEE_CONE = 0x4710,
		eSMAX_CHUNK_CAM_RANGES = 0x4720,
		eSMAX_CHUNK_M3DMAGIC = 0x4D4D,
		eSMAX_CHUNK_HIERARCHY = 0x4F00,
		eSMAX_CHUNK_PARENT_OBJECT = 0x4F10,
		eSMAX_CHUNK_PIVOT_OBJECT = 0x4F20,
		eSMAX_CHUNK_PIVOT_LIMITS = 0x4F30,
		eSMAX_CHUNK_PIVOT_ORDER = 0x4F40,
		eSMAX_CHUNK_XLATE_RANGE = 0x4F50,

		eSMAX_CHUNK_POLY_2D = 0x5000,
		eSMAX_CHUNK_SHAPE_OK = 0x5010,
		eSMAX_CHUNK_SHAPE_NOT_OK = 0x5011,
		eSMAX_CHUNK_SHAPE_HOOK = 0x5020,

		eSMAX_CHUNK_PATH_3D = 0x6000,
		eSMAX_CHUNK_PATH_MATRIX = 0x6005,
		eSMAX_CHUNK_SHAPE_2D = 0x6010,
		eSMAX_CHUNK_M_SCALE = 0x6020,
		eSMAX_CHUNK_M_TWIST = 0x6030,
		eSMAX_CHUNK_M_TEETER = 0x6040,
		eSMAX_CHUNK_M_FIT = 0x6050,
		eSMAX_CHUNK_M_BEVEL = 0x6060,
		eSMAX_CHUNK_XZ_CURVE = 0x6070,
		eSMAX_CHUNK_YZ_CURVE = 0x6080,
		eSMAX_CHUNK_INTERPCT = 0x6090,
		eSMAX_CHUNK_DEFORM_LIMIT = 0x60A0,
		eSMAX_CHUNK_USE_CONTOUR = 0x6100,
		eSMAX_CHUNK_USE_TWEEN = 0x6110,
		eSMAX_CHUNK_USE_SCALE = 0x6120,
		eSMAX_CHUNK_USE_TWIST = 0x6130,
		eSMAX_CHUNK_USE_TEETER = 0x6140,
		eSMAX_CHUNK_USE_FIT = 0x6150,
		eSMAX_CHUNK_USE_BEVEL = 0x6160,

		eSMAX_CHUNK_VIEWPORT_LAYOUT_OLD = 0x7000,
		eSMAX_CHUNK_VIEWPORT_LAYOUT = 0x7001,
		eSMAX_CHUNK_VIEWPORT_DATA_OLD = 0x7010,
		eSMAX_CHUNK_VIEWPORT_DATA = 0x7011,
		eSMAX_CHUNK_VIEWPORT_DATA_3 = 0x7012,
		eSMAX_CHUNK_VIEWPORT_SIZE = 0x7020,
		eSMAX_CHUNK_NETWORK_VIEW = 0x7030,

		eSMAX_CHUNK_XDATA_SECTION = 0X8000,
		eSMAX_CHUNK_XDATA_ENTRY = 0X8001,
		eSMAX_CHUNK_XDATA_APPNAME = 0X8002,
		eSMAX_CHUNK_XDATA_STRING = 0X8003,
		eSMAX_CHUNK_XDATA_FLOAT = 0X8004,
		eSMAX_CHUNK_XDATA_DOUBLE = 0X8005,
		eSMAX_CHUNK_XDATA_SHORT = 0X8006,
		eSMAX_CHUNK_XDATA_LONG = 0X8007,
		eSMAX_CHUNK_XDATA_VOID = 0X8008,
		eSMAX_CHUNK_XDATA_GROUP = 0X8009,
		eSMAX_CHUNK_XDATA_RFU6 = 0X800A,
		eSMAX_CHUNK_XDATA_RFU5 = 0X800B,
		eSMAX_CHUNK_XDATA_RFU4 = 0X800C,
		eSMAX_CHUNK_XDATA_RFU3 = 0X800D,
		eSMAX_CHUNK_XDATA_RFU2 = 0X800E,
		eSMAX_CHUNK_XDATA_RFU1 = 0X800F,
		eSMAX_CHUNK_PARENT_NAME = 0X80F0,

		eSMAX_CHUNK_MAT_NAME = 0xA000,
		eSMAX_CHUNK_MAT_AMBIENT = 0xA010,
		eSMAX_CHUNK_MAT_DIFFUSE = 0xA020,
		eSMAX_CHUNK_MAT_SPECULAR = 0xA030,
		eSMAX_CHUNK_MAT_SHININESS = 0xA040,
		eSMAX_CHUNK_MAT_SHIN2PCT = 0xA041,
		eSMAX_CHUNK_MAT_SHIN3PCT = 0xA042,
		eSMAX_CHUNK_MAT_TRANSPARENCY = 0xA050,
		eSMAX_CHUNK_MAT_XPFALL = 0xA052,
		eSMAX_CHUNK_MAT_REFBLUR = 0xA053,
		eSMAX_CHUNK_MAT_SELF_ILLUM = 0xA080,
		eSMAX_CHUNK_MAT_TWO_SIDE = 0xA081,
		eSMAX_CHUNK_MAT_DECAL = 0xA082,
		eSMAX_CHUNK_MAT_ADDITIVE = 0xA083,
		eSMAX_CHUNK_MAT_SELF_ILPCT = 0xA084,
		eSMAX_CHUNK_MAT_WIRE = 0xA085,
		eSMAX_CHUNK_MAT_SUPERSMP = 0xA086,
		eSMAX_CHUNK_MAT_WIRESIZE = 0xA087,
		eSMAX_CHUNK_MAT_FACEMAP = 0xA088,
		eSMAX_CHUNK_MAT_XPFALLIN = 0xA08A,
		eSMAX_CHUNK_MAT_PHONGSOFT = 0xA08C,
		eSMAX_CHUNK_MAT_WIREABS = 0xA08E,
		eSMAX_CHUNK_MAT_SHADING = 0xA100,
		eSMAX_CHUNK_MAT_TEXMAP = 0xA200,
		eSMAX_CHUNK_MAT_SPECMAP = 0xA204,
		eSMAX_CHUNK_MAT_OPACMAP = 0xA210,
		eSMAX_CHUNK_MAT_REFLMAP = 0xA220,
		eSMAX_CHUNK_MAT_BUMPMAP = 0xA230,
		eSMAX_CHUNK_MAT_USE_XPFALL = 0xA240,
		eSMAX_CHUNK_MAT_USE_REFBLUR = 0xA250,
		eSMAX_CHUNK_MAT_BUMP_PERCENT = 0xA252,
		eSMAX_CHUNK_MAT_MAPNAME = 0xA300,
		eSMAX_CHUNK_MAT_ACUBIC = 0xA310,
		eSMAX_CHUNK_MAT_SXP_TEXT_DATA = 0xA320,
		eSMAX_CHUNK_MAT_SXP_TEXT2_DATA = 0xA321,
		eSMAX_CHUNK_MAT_SXP_OPAC_DATA = 0xA322,
		eSMAX_CHUNK_MAT_SXP_BUMP_DATA = 0xA324,
		eSMAX_CHUNK_MAT_SXP_SPEC_DATA = 0xA325,
		eSMAX_CHUNK_MAT_SXP_SHIN_DATA = 0xA326,
		eSMAX_CHUNK_MAT_SXP_SELFI_DATA = 0xA328,
		eSMAX_CHUNK_MAT_SXP_TEXT_MASKDATA = 0xA32A,
		eSMAX_CHUNK_MAT_SXP_TEXT2_MASKDATA = 0xA32C,
		eSMAX_CHUNK_MAT_SXP_OPAC_MASKDATA = 0xA32E,
		eSMAX_CHUNK_MAT_SXP_BUMP_MASKDATA = 0xA330,
		eSMAX_CHUNK_MAT_SXP_SPEC_MASKDATA = 0xA332,
		eSMAX_CHUNK_MAT_SXP_SHIN_MASKDATA = 0xA334,
		eSMAX_CHUNK_MAT_SXP_SELFI_MASKDATA = 0xA336,
		eSMAX_CHUNK_MAT_SXP_REFL_MASKDATA = 0xA338,
		eSMAX_CHUNK_MAT_TEX2MAP = 0xA33A,
		eSMAX_CHUNK_MAT_SHINMAP = 0xA33C,
		eSMAX_CHUNK_MAT_SELFIMAP = 0xA33D,
		eSMAX_CHUNK_MAT_TEXMASK = 0xA33E,
		eSMAX_CHUNK_MAT_TEX2MASK = 0xA340,
		eSMAX_CHUNK_MAT_OPACMASK = 0xA342,
		eSMAX_CHUNK_MAT_BUMPMASK = 0xA344,
		eSMAX_CHUNK_MAT_SHINMASK = 0xA346,
		eSMAX_CHUNK_MAT_SPECMASK = 0xA348,
		eSMAX_CHUNK_MAT_SELFIMASK = 0xA34A,
		eSMAX_CHUNK_MAT_REFLMASK = 0xA34C,
		eSMAX_CHUNK_MAT_MAP_TILINGOLD = 0xA350,
		eSMAX_CHUNK_MAT_MAP_TILING = 0xA351,
		eSMAX_CHUNK_MAT_MAP_TEXBLUR_OLD = 0xA352,
		eSMAX_CHUNK_MAT_MAP_TEXBLUR = 0xA353,
		eSMAX_CHUNK_MAT_MAP_USCALE = 0xA354,
		eSMAX_CHUNK_MAT_MAP_VSCALE = 0xA356,
		eSMAX_CHUNK_MAT_MAP_UOFFSET = 0xA358,
		eSMAX_CHUNK_MAT_MAP_VOFFSET = 0xA35A,
		eSMAX_CHUNK_MAT_MAP_ANG = 0xA35C,
		eSMAX_CHUNK_MAT_MAP_COL1 = 0xA360,
		eSMAX_CHUNK_MAT_MAP_COL2 = 0xA362,
		eSMAX_CHUNK_MAT_MAP_RCOL = 0xA364,
		eSMAX_CHUNK_MAT_MAP_GCOL = 0xA366,
		eSMAX_CHUNK_MAT_MAP_BCOL = 0xA368,
		eSMAX_CHUNK_MAT_ENTRY = 0xAFFF,

		eSMAX_CHUNK_KFDATA = 0xB000,
		eSMAX_CHUNK_AMBIENT_NODE_TAG = 0xB001,
		eSMAX_CHUNK_OBJECT_NODE_TAG = 0xB002,
		eSMAX_CHUNK_CAMERA_NODE_TAG = 0xB003,
		eSMAX_CHUNK_TARGET_NODE_TAG = 0xB004,
		eSMAX_CHUNK_LIGHT_NODE_TAG = 0xB005,
		eSMAX_CHUNK_L_TARGET_NODE_TAG = 0xB006,
		eSMAX_CHUNK_SPOTLIGHT_NODE_TAG = 0xB007,
		eSMAX_CHUNK_KFSEG = 0xB008,
		eSMAX_CHUNK_KFCURTIME = 0xB009,
		eSMAX_CHUNK_KFHDR = 0xB00A,
		eSMAX_CHUNK_NODE_HDR = 0xB010,
		eSMAX_CHUNK_INSTANCE_NAME = 0xB011,
		eSMAX_CHUNK_PRESCALE = 0xB012,
		eSMAX_CHUNK_PIVOT = 0xB013,
		eSMAX_CHUNK_BOUNDBOX = 0xB014,
		eSMAX_CHUNK_MORPH_SMOOTH = 0xB015,
		eSMAX_CHUNK_POS_TRACK_TAG = 0xB020,
		eSMAX_CHUNK_ROT_TRACK_TAG = 0xB021,
		eSMAX_CHUNK_SCL_TRACK_TAG = 0xB022,
		eSMAX_CHUNK_FOV_TRACK_TAG = 0xB023,
		eSMAX_CHUNK_ROLL_TRACK_TAG = 0xB024,
		eSMAX_CHUNK_COL_TRACK_TAG = 0xB025,
		eSMAX_CHUNK_MORPH_TRACK_TAG = 0xB026,
		eSMAX_CHUNK_HOT_TRACK_TAG = 0xB027,
		eSMAX_CHUNK_FALL_TRACK_TAG = 0xB028,
		eSMAX_CHUNK_HIDE_TRACK_TAG = 0xB029,
		eSMAX_CHUNK_NODE_ID = 0xB030,

		eSMAX_CHUNK_C_MDRAWER = 0xC010,
		eSMAX_CHUNK_C_TDRAWER = 0xC020,
		eSMAX_CHUNK_C_SHPDRAWER = 0xC030,
		eSMAX_CHUNK_C_MODDRAWER = 0xC040,
		eSMAX_CHUNK_C_RIPDRAWER = 0xC050,
		eSMAX_CHUNK_C_TXDRAWER = 0xC060,
		eSMAX_CHUNK_C_PDRAWER = 0xC062,
		eSMAX_CHUNK_C_MTLDRAWER = 0xC064,
		eSMAX_CHUNK_C_FLIDRAWER = 0xC066,
		eSMAX_CHUNK_C_CUBDRAWER = 0xC067,
		eSMAX_CHUNK_C_MFILE = 0xC070,
		eSMAX_CHUNK_C_SHPFILE = 0xC080,
		eSMAX_CHUNK_C_MODFILE = 0xC090,
		eSMAX_CHUNK_C_RIPFILE = 0xC0A0,
		eSMAX_CHUNK_C_TXFILE = 0xC0B0,
		eSMAX_CHUNK_C_PFILE = 0xC0B2,
		eSMAX_CHUNK_C_MTLFILE = 0xC0B4,
		eSMAX_CHUNK_C_FLIFILE = 0xC0B6,
		eSMAX_CHUNK_C_PALFILE = 0xC0B8,
		eSMAX_CHUNK_C_TX_STRING = 0xC0C0,
		eSMAX_CHUNK_C_CONSTS = 0xC0D0,
		eSMAX_CHUNK_C_SNAPS = 0xC0E0,
		eSMAX_CHUNK_C_GRIDS = 0xC0F0,
		eSMAX_CHUNK_C_ASNAPS = 0xC100,
		eSMAX_CHUNK_C_GRID_RANGE = 0xC110,
		eSMAX_CHUNK_C_RENDTYPE = 0xC120,
		eSMAX_CHUNK_C_PROGMODE = 0xC130,
		eSMAX_CHUNK_C_PREVMODE = 0xC140,
		eSMAX_CHUNK_C_MODWMODE = 0xC150,
		eSMAX_CHUNK_C_MODMODEL = 0xC160,
		eSMAX_CHUNK_C_ALL_LINES = 0xC170,
		eSMAX_CHUNK_C_BACK_TYPE = 0xC180,
		eSMAX_CHUNK_C_MD_CS = 0xC190,
		eSMAX_CHUNK_C_MD_CE = 0xC1A0,
		eSMAX_CHUNK_C_MD_SML = 0xC1B0,
		eSMAX_CHUNK_C_MD_SMW = 0xC1C0,
		eSMAX_CHUNK_C_LOFT_WITH_TEXTURE = 0xC1C3,
		eSMAX_CHUNK_C_LOFT_L_REPEAT = 0xC1C4,
		eSMAX_CHUNK_C_LOFT_W_REPEAT = 0xC1C5,
		eSMAX_CHUNK_C_LOFT_UV_NORMALIZE = 0xC1C6,
		eSMAX_CHUNK_C_WELD_LOFT = 0xC1C7,
		eSMAX_CHUNK_C_MD_PDET = 0xC1D0,
		eSMAX_CHUNK_C_MD_SDET = 0xC1E0,
		eSMAX_CHUNK_C_RGB_RMODE = 0xC1F0,
		eSMAX_CHUNK_C_RGB_HIDE = 0xC200,
		eSMAX_CHUNK_C_RGB_MAPSW = 0xC202,
		eSMAX_CHUNK_C_RGB_TWOSIDE = 0xC204,
		eSMAX_CHUNK_C_RGB_SHADOW = 0xC208,
		eSMAX_CHUNK_C_RGB_AA = 0xC210,
		eSMAX_CHUNK_C_RGB_OVW = 0xC220,
		eSMAX_CHUNK_C_RGB_OVH = 0xC230,
		eSMAX_CHUNK_CMAGIC = 0xC23D,
		eSMAX_CHUNK_C_RGB_PICTYPE = 0xC240,
		eSMAX_CHUNK_C_RGB_OUTPUT = 0xC250,
		eSMAX_CHUNK_C_RGB_TODISK = 0xC253,
		eSMAX_CHUNK_C_RGB_COMPRESS = 0xC254,
		eSMAX_CHUNK_C_JPEG_COMPRESSION = 0xC255,
		eSMAX_CHUNK_C_RGB_DISPDEV = 0xC256,
		eSMAX_CHUNK_C_RGB_HARDDEV = 0xC259,
		eSMAX_CHUNK_C_RGB_PATH = 0xC25A,
		eSMAX_CHUNK_C_BITMAP_DRAWER = 0xC25B,
		eSMAX_CHUNK_C_RGB_FILE = 0xC260,
		eSMAX_CHUNK_C_RGB_OVASPECT = 0xC270,
		eSMAX_CHUNK_C_RGB_ANIMTYPE = 0xC271,
		eSMAX_CHUNK_C_RENDER_ALL = 0xC272,
		eSMAX_CHUNK_C_REND_FROM = 0xC273,
		eSMAX_CHUNK_C_REND_TO = 0xC274,
		eSMAX_CHUNK_C_REND_NTH = 0xC275,
		eSMAX_CHUNK_C_PAL_TYPE = 0xC276,
		eSMAX_CHUNK_C_RND_TURBO = 0xC277,
		eSMAX_CHUNK_C_RND_MIP = 0xC278,
		eSMAX_CHUNK_C_BGND_METHOD = 0xC279,
		eSMAX_CHUNK_C_AUTO_REFLECT = 0xC27A,
		eSMAX_CHUNK_C_VP_FROM = 0xC27B,
		eSMAX_CHUNK_C_VP_TO = 0xC27C,
		eSMAX_CHUNK_C_VP_NTH = 0xC27D,
		eSMAX_CHUNK_C_REND_TSTEP = 0xC27E,
		eSMAX_CHUNK_C_VP_TSTEP = 0xC27F,
		eSMAX_CHUNK_C_SRDIAM = 0xC280,
		eSMAX_CHUNK_C_SRDEG = 0xC290,
		eSMAX_CHUNK_C_SRSEG = 0xC2A0,
		eSMAX_CHUNK_C_SRDIR = 0xC2B0,
		eSMAX_CHUNK_C_HETOP = 0xC2C0,
		eSMAX_CHUNK_C_HEBOT = 0xC2D0,
		eSMAX_CHUNK_C_HEHT = 0xC2E0,
		eSMAX_CHUNK_C_HETURNS = 0xC2F0,
		eSMAX_CHUNK_C_HEDEG = 0xC300,
		eSMAX_CHUNK_C_HESEG = 0xC310,
		eSMAX_CHUNK_C_HEDIR = 0xC320,
		eSMAX_CHUNK_C_QUIKSTUFF = 0xC330,
		eSMAX_CHUNK_C_SEE_LIGHTS = 0xC340,
		eSMAX_CHUNK_C_SEE_CAMERAS = 0xC350,
		eSMAX_CHUNK_C_SEE_3D = 0xC360,
		eSMAX_CHUNK_C_MESHSEL = 0xC370,
		eSMAX_CHUNK_C_MESHUNSEL = 0xC380,
		eSMAX_CHUNK_C_POLYSEL = 0xC390,
		eSMAX_CHUNK_C_POLYUNSEL = 0xC3A0,
		eSMAX_CHUNK_C_SHPLOCAL = 0xC3A2,
		eSMAX_CHUNK_C_MSHLOCAL = 0xC3A4,
		eSMAX_CHUNK_C_NUM_FORMAT = 0xC3B0,
		eSMAX_CHUNK_C_ARCH_DENOM = 0xC3C0,
		eSMAX_CHUNK_C_IN_DEVICE = 0xC3D0,
		eSMAX_CHUNK_C_MSCALE = 0xC3E0,
		eSMAX_CHUNK_C_COMM_PORT = 0xC3F0,
		eSMAX_CHUNK_C_TAB_BASES = 0xC400,
		eSMAX_CHUNK_C_TAB_DIVS = 0xC410,
		eSMAX_CHUNK_C_MASTER_SCALES = 0xC420,
		eSMAX_CHUNK_C_SHOW_1STVERT = 0xC430,
		eSMAX_CHUNK_C_SHAPER_OK = 0xC440,
		eSMAX_CHUNK_C_LOFTER_OK = 0xC450,
		eSMAX_CHUNK_C_EDITOR_OK = 0xC460,
		eSMAX_CHUNK_C_KEYFRAMER_OK = 0xC470,
		eSMAX_CHUNK_C_PICKSIZE = 0xC480,
		eSMAX_CHUNK_C_MAPTYPE = 0xC490,
		eSMAX_CHUNK_C_MAP_DISPLAY = 0xC4A0,
		eSMAX_CHUNK_C_TILE_XY = 0xC4B0,
		eSMAX_CHUNK_C_MAP_XYZ = 0xC4C0,
		eSMAX_CHUNK_C_MAP_SCALE = 0xC4D0,
		eSMAX_CHUNK_C_MAP_MATRIX_OLD = 0xC4E0,
		eSMAX_CHUNK_C_MAP_MATRIX = 0xC4E1,
		eSMAX_CHUNK_C_MAP_WID_HT = 0xC4F0,
		eSMAX_CHUNK_C_OBNAME = 0xC500,
		eSMAX_CHUNK_C_CAMNAME = 0xC510,
		eSMAX_CHUNK_C_LTNAME = 0xC520,
		eSMAX_CHUNK_C_CUR_MNAME = 0xC525,
		eSMAX_CHUNK_C_CURMTL_FROM_MESH = 0xC526,
		eSMAX_CHUNK_C_GET_SHAPE_MAKE_FACES = 0xC527,
		eSMAX_CHUNK_C_DETAIL = 0xC530,
		eSMAX_CHUNK_C_VERTMARK = 0xC540,
		eSMAX_CHUNK_C_MSHAX = 0xC550,
		eSMAX_CHUNK_C_MSHCP = 0xC560,
		eSMAX_CHUNK_C_USERAX = 0xC570,
		eSMAX_CHUNK_C_SHOOK = 0xC580,
		eSMAX_CHUNK_C_RAX = 0xC590,
		eSMAX_CHUNK_C_STAPE = 0xC5A0,
		eSMAX_CHUNK_C_LTAPE = 0xC5B0,
		eSMAX_CHUNK_C_ETAPE = 0xC5C0,
		eSMAX_CHUNK_C_KTAPE = 0xC5C8,
		eSMAX_CHUNK_C_SPHSEGS = 0xC5D0,
		eSMAX_CHUNK_C_GEOSMOOTH = 0xC5E0,
		eSMAX_CHUNK_C_HEMISEGS = 0xC5F0,
		eSMAX_CHUNK_C_PRISMSEGS = 0xC600,
		eSMAX_CHUNK_C_PRISMSIDES = 0xC610,
		eSMAX_CHUNK_C_TUBESEGS = 0xC620,
		eSMAX_CHUNK_C_TUBESIDES = 0xC630,
		eSMAX_CHUNK_C_TORSEGS = 0xC640,
		eSMAX_CHUNK_C_TORSIDES = 0xC650,
		eSMAX_CHUNK_C_CONESIDES = 0xC660,
		eSMAX_CHUNK_C_CONESEGS = 0xC661,
		eSMAX_CHUNK_C_NGPARMS = 0xC670,
		eSMAX_CHUNK_C_PTHLEVEL = 0xC680,
		eSMAX_CHUNK_C_MSCSYM = 0xC690,
		eSMAX_CHUNK_C_MFTSYM = 0xC6A0,
		eSMAX_CHUNK_C_MTTSYM = 0xC6B0,
		eSMAX_CHUNK_C_SMOOTHING = 0xC6C0,
		eSMAX_CHUNK_C_MODICOUNT = 0xC6D0,
		eSMAX_CHUNK_C_FONTSEL = 0xC6E0,
		eSMAX_CHUNK_C_TESS_TYPE = 0xC6F0,
		eSMAX_CHUNK_C_TESS_TENSION = 0xC6F1,
		eSMAX_CHUNK_C_SEG_START = 0xC700,
		eSMAX_CHUNK_C_SEG_END = 0xC705,
		eSMAX_CHUNK_C_CURTIME = 0xC710,
		eSMAX_CHUNK_C_ANIMLENGTH = 0xC715,
		eSMAX_CHUNK_C_PV_FROM = 0xC720,
		eSMAX_CHUNK_C_PV_TO = 0xC725,
		eSMAX_CHUNK_C_PV_DOFNUM = 0xC730,
		eSMAX_CHUNK_C_PV_RNG = 0xC735,
		eSMAX_CHUNK_C_PV_NTH = 0xC740,
		eSMAX_CHUNK_C_PV_TYPE = 0xC745,
		eSMAX_CHUNK_C_PV_METHOD = 0xC750,
		eSMAX_CHUNK_C_PV_FPS = 0xC755,
		eSMAX_CHUNK_C_VTR_FRAMES = 0xC765,
		eSMAX_CHUNK_C_VTR_HDTL = 0xC770,
		eSMAX_CHUNK_C_VTR_HD = 0xC771,
		eSMAX_CHUNK_C_VTR_TL = 0xC772,
		eSMAX_CHUNK_C_VTR_IN = 0xC775,
		eSMAX_CHUNK_C_VTR_PK = 0xC780,
		eSMAX_CHUNK_C_VTR_SH = 0xC785,
		eSMAX_CHUNK_C_WORK_MTLS = 0xC790,
		eSMAX_CHUNK_C_WORK_MTLS_2 = 0xC792,
		eSMAX_CHUNK_C_WORK_MTLS_3 = 0xC793,
		eSMAX_CHUNK_C_WORK_MTLS_4 = 0xC794,
		eSMAX_CHUNK_C_BGTYPE = 0xC7A1,
		eSMAX_CHUNK_C_MEDTILE = 0xC7B0,
		eSMAX_CHUNK_C_LO_CONTRAST = 0xC7D0,
		eSMAX_CHUNK_C_HI_CONTRAST = 0xC7D1,
		eSMAX_CHUNK_C_FROZ_DISPLAY = 0xC7E0,
		eSMAX_CHUNK_C_BOOLWELD = 0xC7F0,
		eSMAX_CHUNK_C_BOOLTYPE = 0xC7F1,
		eSMAX_CHUNK_C_ANG_THRESH = 0xC900,
		eSMAX_CHUNK_C_SS_THRESH = 0xC901,
		eSMAX_CHUNK_C_TEXTURE_BLUR_DEFAULT = 0xC903,
		eSMAX_CHUNK_C_MAPDRAWER = 0xCA00,
		eSMAX_CHUNK_C_MAPDRAWER1 = 0xCA01,
		eSMAX_CHUNK_C_MAPDRAWER2 = 0xCA02,
		eSMAX_CHUNK_C_MAPDRAWER3 = 0xCA03,
		eSMAX_CHUNK_C_MAPDRAWER4 = 0xCA04,
		eSMAX_CHUNK_C_MAPDRAWER5 = 0xCA05,
		eSMAX_CHUNK_C_MAPDRAWER6 = 0xCA06,
		eSMAX_CHUNK_C_MAPDRAWER7 = 0xCA07,
		eSMAX_CHUNK_C_MAPDRAWER8 = 0xCA08,
		eSMAX_CHUNK_C_MAPDRAWER9 = 0xCA09,
		eSMAX_CHUNK_C_MAPDRAWER_ENTRY = 0xCA10,
		eSMAX_CHUNK_C_BACKUP_FILE = 0xCA20,
		eSMAX_CHUNK_C_DITHER_256 = 0xCA21,
		eSMAX_CHUNK_C_SAVE_LAST = 0xCA22,
		eSMAX_CHUNK_C_USE_ALPHA = 0xCA23,
		eSMAX_CHUNK_C_TGA_DEPTH = 0xCA24,
		eSMAX_CHUNK_C_REND_FIELDS = 0xCA25,
		eSMAX_CHUNK_C_REFLIP = 0xCA26,
		eSMAX_CHUNK_C_SEL_ITEMTOG = 0xCA27,
		eSMAX_CHUNK_C_SEL_RESET = 0xCA28,
		eSMAX_CHUNK_C_STICKY_KEYINF = 0xCA29,
		eSMAX_CHUNK_C_WELD_THRESHOLD = 0xCA2A,
		eSMAX_CHUNK_C_ZCLIP_POINT = 0xCA2B,
		eSMAX_CHUNK_C_ALPHA_SPLIT = 0xCA2C,
		eSMAX_CHUNK_C_KF_SHOW_BACKFACE = 0xCA30,
		eSMAX_CHUNK_C_OPTIMIZE_LOFT = 0xCA40,
		eSMAX_CHUNK_C_TENS_DEFAULT = 0xCA42,
		eSMAX_CHUNK_C_CONT_DEFAULT = 0xCA44,
		eSMAX_CHUNK_C_BIAS_DEFAULT = 0xCA46,
		eSMAX_CHUNK_C_DXFNAME_SRC = 0xCA50,
		eSMAX_CHUNK_C_AUTO_WELD = 0xCA60,
		eSMAX_CHUNK_C_AUTO_UNIFY = 0xCA70,
		eSMAX_CHUNK_C_AUTO_SMOOTH = 0xCA80,
		eSMAX_CHUNK_C_DXF_SMOOTH_ANG = 0xCA90,
		eSMAX_CHUNK_C_SMOOTH_ANG = 0xCAA0,
		eSMAX_CHUNK_C_WORK_MTLS_5 = 0xCB00,
		eSMAX_CHUNK_C_WORK_MTLS_6 = 0xCB01,
		eSMAX_CHUNK_C_WORK_MTLS_7 = 0xCB02,
		eSMAX_CHUNK_C_WORK_MTLS_8 = 0xCB03,
		eSMAX_CHUNK_C_WORKMTL = 0xCB04,
		eSMAX_CHUNK_C_SXP_TEXT_DATA = 0xCB10,
		eSMAX_CHUNK_C_SXP_OPAC_DATA = 0xCB11,
		eSMAX_CHUNK_C_SXP_BUMP_DATA = 0xCB12,
		eSMAX_CHUNK_C_SXP_SHIN_DATA = 0xCB13,
		eSMAX_CHUNK_C_SXP_TEXT2_DATA = 0xCB20,
		eSMAX_CHUNK_C_SXP_SPEC_DATA = 0xCB24,
		eSMAX_CHUNK_C_SXP_SELFI_DATA = 0xCB28,
		eSMAX_CHUNK_C_SXP_TEXT_MASKDATA = 0xCB30,
		eSMAX_CHUNK_C_SXP_TEXT2_MASKDATA = 0xCB32,
		eSMAX_CHUNK_C_SXP_OPAC_MASKDATA = 0xCB34,
		eSMAX_CHUNK_C_SXP_BUMP_MASKDATA = 0xCB36,
		eSMAX_CHUNK_C_SXP_SPEC_MASKDATA = 0xCB38,
		eSMAX_CHUNK_C_SXP_SHIN_MASKDATA = 0xCB3A,
		eSMAX_CHUNK_C_SXP_REFL_MASKDATA = 0xCB3E,
		eSMAX_CHUNK_C_NET_USE_VPOST = 0xCC00,
		eSMAX_CHUNK_C_NET_USE_GAMMA = 0xCC10,
		eSMAX_CHUNK_C_NET_FIELD_ORDER = 0xCC20,
		eSMAX_CHUNK_C_BLUR_FRAMES = 0xCD00,
		eSMAX_CHUNK_C_BLUR_SAMPLES = 0xCD10,
		eSMAX_CHUNK_C_BLUR_DUR = 0xCD20,
		eSMAX_CHUNK_C_HOT_METHOD = 0xCD30,
		eSMAX_CHUNK_C_HOT_CHECK = 0xCD40,
		eSMAX_CHUNK_C_PIXEL_SIZE = 0xCD50,
		eSMAX_CHUNK_C_DISP_GAMMA = 0xCD60,
		eSMAX_CHUNK_C_FBUF_GAMMA = 0xCD70,
		eSMAX_CHUNK_C_FILE_OUT_GAMMA = 0xCD80,
		eSMAX_CHUNK_C_FILE_IN_GAMMA = 0xCD82,
		eSMAX_CHUNK_C_GAMMA_CORRECT = 0xCD84,
		eSMAX_CHUNK_C_APPLY_DISP_GAMMA = 0xCD90,
		eSMAX_CHUNK_C_APPLY_FBUF_GAMMA = 0xCDA0,
		eSMAX_CHUNK_C_APPLY_FILE_GAMMA = 0xCDB0,
		eSMAX_CHUNK_C_FORCE_WIRE = 0xCDC0,
		eSMAX_CHUNK_C_RAY_SHADOWS = 0xCDD0,
		eSMAX_CHUNK_C_MASTER_AMBIENT = 0xCDE0,
		eSMAX_CHUNK_C_SUPER_SAMPLE = 0xCDF0,
		eSMAX_CHUNK_C_OBJECT_MBLUR = 0xCE00,
		eSMAX_CHUNK_C_MBLUR_DITHER = 0xCE10,
		eSMAX_CHUNK_C_DITHER_24 = 0xCE20,
		eSMAX_CHUNK_C_SUPER_BLACK = 0xCE30,
		eSMAX_CHUNK_C_SAFE_FRAME = 0xCE40,
		eSMAX_CHUNK_C_VIEW_PRES_RATIO = 0xCE50,
		eSMAX_CHUNK_C_BGND_PRES_RATIO = 0xCE60,
		eSMAX_CHUNK_C_NTH_SERIAL_NUM = 0xCE70,

		eSMAX_CHUNK_VPDATA = 0xD000,
		eSMAX_CHUNK_P_QUEUE_ENTRY = 0xD100,
		eSMAX_CHUNK_P_QUEUE_IMAGE = 0xD110,
		eSMAX_CHUNK_P_QUEUE_USEIGAMMA = 0xD114,
		eSMAX_CHUNK_P_QUEUE_PROC = 0xD120,
		eSMAX_CHUNK_P_QUEUE_SOLID = 0xD130,
		eSMAX_CHUNK_P_QUEUE_GRADIENT = 0xD140,
		eSMAX_CHUNK_P_QUEUE_KF = 0xD150,
		eSMAX_CHUNK_P_QUEUE_MOTBLUR = 0xD152,
		eSMAX_CHUNK_P_QUEUE_MB_REPEAT = 0xD153,
		eSMAX_CHUNK_P_QUEUE_NONE = 0xD160,
		eSMAX_CHUNK_P_QUEUE_RESIZE = 0xD180,
		eSMAX_CHUNK_P_QUEUE_OFFSET = 0xD185,
		eSMAX_CHUNK_P_QUEUE_ALIGN = 0xD190,
		eSMAX_CHUNK_P_CUSTOM_SIZE = 0xD1A0,
		eSMAX_CHUNK_P_ALPH_NONE = 0xD210,
		eSMAX_CHUNK_P_ALPH_PSEUDO = 0xD220,
		eSMAX_CHUNK_P_ALPH_OP_PSEUDO = 0xD221,
		eSMAX_CHUNK_P_ALPH_BLUR = 0xD222,
		eSMAX_CHUNK_P_ALPH_PCOL = 0xD225,
		eSMAX_CHUNK_P_ALPH_C0 = 0xD230,
		eSMAX_CHUNK_P_ALPH_OP_KEY = 0xD231,
		eSMAX_CHUNK_P_ALPH_KCOL = 0xD235,
		eSMAX_CHUNK_P_ALPH_OP_NOCONV = 0xD238,
		eSMAX_CHUNK_P_ALPH_IMAGE = 0xD240,
		eSMAX_CHUNK_P_ALPH_ALPHA = 0xD250,
		eSMAX_CHUNK_P_ALPH_QUES = 0xD260,
		eSMAX_CHUNK_P_ALPH_QUEIMG = 0xD265,
		eSMAX_CHUNK_P_ALPH_CUTOFF = 0xD270,
		eSMAX_CHUNK_P_ALPHANEG = 0xD280,
		eSMAX_CHUNK_P_TRAN_NONE = 0xD300,
		eSMAX_CHUNK_P_TRAN_IMAGE = 0xD310,
		eSMAX_CHUNK_P_TRAN_FRAMES = 0xD312,
		eSMAX_CHUNK_P_TRAN_FADEIN = 0xD320,
		eSMAX_CHUNK_P_TRAN_FADEOUT = 0xD330,
		eSMAX_CHUNK_P_TRANNEG = 0xD340,
		eSMAX_CHUNK_P_RANGES = 0xD400,
		eSMAX_CHUNK_P_PROC_DATA = 0xD500,

		eSMAX_CHUNK_POS_TRACK_TAG_KEY = 0xF020,
		eSMAX_CHUNK_ROT_TRACK_TAG_KEY = 0xF021,
		eSMAX_CHUNK_SCL_TRACK_TAG_KEY = 0xF022,
		eSMAX_CHUNK_FOV_TRACK_TAG_KEY = 0xF023,
		eSMAX_CHUNK_ROLL_TRACK_TAG_KEY = 0xF024,
		eSMAX_CHUNK_COL_TRACK_TAG_KEY = 0xF025,
		eSMAX_CHUNK_MORPH_TRACK_TAG_KEY = 0xF026,
		eSMAX_CHUNK_HOT_TRACK_TAG_KEY = 0xF027,
		eSMAX_CHUNK_FALL_TRACK_TAG_KEY = 0xF028,
		eSMAX_CHUNK_POINT_ARRAY_ENTRY = 0xF110,
		eSMAX_CHUNK_POINT_FLAG_ARRAY_ENTRY = 0xF111,
		eSMAX_CHUNK_FACE_ARRAY_ENTRY = 0xF120,
		eSMAX_CHUNK_MSH_MAT_GROUP_ENTRY = 0xF130,
		eSMAX_CHUNK_TEX_VERTS_ENTRY = 0xF140,
		eSMAX_CHUNK_SMOOTH_GROUP_ENTRY = 0xF150,
		eSMAX_CHUNK_DUMMY = 0xFFFF,

	}	eSMAX_CHUNK;
	//! 3DS file importer
	/*!
	Imports data from 3DS (3D Studio Max) files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class SMaxImporter : public castor3d::Importer
	{
	private:
		struct SMaxChunk
		{
			eSMAX_CHUNK m_eChunkId;
			unsigned long m_ulLength;
			unsigned long m_ulBytesRead;
		};

	public:
		/**
		 * Constructor
		 */
		SMaxImporter( castor3d::Engine & p_engine );

		static castor3d::ImporterUPtr create( castor3d::Engine & p_engine );

	private:
		/**
		 *\copydoc		castor3d::Importer::doImportScene
		 */
		bool doImportScene( castor3d::Scene & p_scene )override;
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & p_mesh )override;

		int doGetString( castor::String & p_strString );
		void doReadChunk( SMaxChunk * p_chunk );
		void doProcessNextChunk( castor3d::Scene & p_scene, SMaxChunk * p_chunk, castor3d::Mesh & p_mesh );
		void doProcessNextObjectChunk( castor3d::Scene & p_scene, SMaxChunk * p_chunk, castor3d::Mesh & p_mesh );
		void doProcessNextMaterialChunk( castor3d::Scene & p_scene, SMaxChunk * p_chunk );
		void doProcessMaterialMapChunk( SMaxChunk * p_chunk, castor::String & p_strName );
		void doReadColorChunk( SMaxChunk * p_chunk, castor::Colour & p_clrColour );
		void doReadVertices( SMaxChunk * p_chunk, castor3d::Submesh & p_submesh );
		void doReadVertexIndices( castor3d::Scene & p_scene, SMaxChunk * p_chunk, castor3d::Submesh & p_submesh );
		void doReadUVCoordinates( SMaxChunk * p_chunk, castor3d::Submesh & p_submesh );
		void doReadObjectMaterial( castor3d::Scene & p_scene, SMaxChunk * p_chunk, castor3d::Submesh & p_submesh );
		void doDiscardChunk( SMaxChunk * p_chunk );
		bool doIsValidChunk( SMaxChunk * p_chunk, SMaxChunk * p_pParent );

	private:
		castor::BinaryFile * m_pFile;
		std::vector< float > m_arrayTexVerts;
		int m_iNumOfMaterials;
		std::size_t m_uiNbVertex;
		bool m_bIndicesFound;
	};
}

#endif
