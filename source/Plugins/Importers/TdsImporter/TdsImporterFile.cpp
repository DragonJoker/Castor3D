#include "TdsImporter/TdsImporterFile.hpp"

#include "TdsImporter/TdsMaterialImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace c3d_3ds
{
	//*********************************************************************************************

	namespace file
	{
		static bool isValidChunk( TdsChunk const & chunk, TdsChunk const & parent )
		{
			bool result = false;

			switch ( chunk.chunkId )
			{
			default:
				result = false;
				break;

			case TdsChunkID::NULL_CHUNK:
			case TdsChunkID::UNKNOWN_CHUNK:
			case TdsChunkID::M3D_VERSION:
			case TdsChunkID::M3D_KFVERSION:
			case TdsChunkID::COLOR_F:
			case TdsChunkID::COLOR_24:
			case TdsChunkID::LIN_COLOR_24:
			case TdsChunkID::LIN_COLOR_F:
			case TdsChunkID::INT_PERCENTAGE:
			case TdsChunkID::FLOAT_PERCENTAGE:
			case TdsChunkID::MASTER_SCALE:
			case TdsChunkID::CHUNKTYPE:
			case TdsChunkID::CHUNKUNIQUE:
			case TdsChunkID::NOTCHUNK:
			case TdsChunkID::CONTAINER:
			case TdsChunkID::ISCHUNK:
			case TdsChunkID::C_SXP_SELFI_MASKDATA:
			case TdsChunkID::BIT_MAP:
			case TdsChunkID::USE_BIT_MAP:
			case TdsChunkID::SOLID_BGND:
			case TdsChunkID::USE_SOLID_BGND:
			case TdsChunkID::V_GRADIENT:
			case TdsChunkID::USE_V_GRADIENT:
			case TdsChunkID::LO_SHADOW_BIAS:
			case TdsChunkID::HI_SHADOW_BIAS:
			case TdsChunkID::SHADOW_MAP_SIZE:
			case TdsChunkID::SHADOW_SAMPLES:
			case TdsChunkID::SHADOW_RANGE:
			case TdsChunkID::SHADOW_FILTER:
			case TdsChunkID::RAY_BIAS:
			case TdsChunkID::O_CONSTS:
			case TdsChunkID::AMBIENT_LIGHT:
			case TdsChunkID::FOG:
			case TdsChunkID::USE_FOG:
			case TdsChunkID::FOG_BGND:
			case TdsChunkID::DISTANCE_CUE:
			case TdsChunkID::USE_DISTANCE_CUE:
			case TdsChunkID::LAYER_FOG:
			case TdsChunkID::USE_LAYER_FOG:
			case TdsChunkID::DCUE_BGND:
			case TdsChunkID::SMAGIC:
			case TdsChunkID::LMAGIC:
			case TdsChunkID::DEFAULT_VIEW:
			case TdsChunkID::VIEW_TOP:
			case TdsChunkID::VIEW_BOTTOM:
			case TdsChunkID::VIEW_LEFT:
			case TdsChunkID::VIEW_RIGHT:
			case TdsChunkID::VIEW_FRONT:
			case TdsChunkID::VIEW_BACK:
			case TdsChunkID::VIEW_USER:
			case TdsChunkID::VIEW_CAMERA:
			case TdsChunkID::VIEW_WINDOW:
			case TdsChunkID::MDATA:
			case TdsChunkID::MESH_VERSION:
			case TdsChunkID::MLIBMAGIC:
			case TdsChunkID::PRJMAGIC:
			case TdsChunkID::MATMAGIC:
			case TdsChunkID::NAMED_OBJECT:
			case TdsChunkID::OBJ_HIDDEN:
			case TdsChunkID::OBJ_VIS_LOFTER:
			case TdsChunkID::OBJ_DOESNT_CAST:
			case TdsChunkID::OBJ_MATTE:
			case TdsChunkID::OBJ_FAST:
			case TdsChunkID::OBJ_PROCEDURAL:
			case TdsChunkID::OBJ_FROZEN:
			case TdsChunkID::OBJ_DONT_RCVSHADOW:
			case TdsChunkID::N_TRI_OBJECT:
			case TdsChunkID::POINT_ARRAY:
			case TdsChunkID::POINT_FLAG_ARRAY:
			case TdsChunkID::FACE_ARRAY:
			case TdsChunkID::MSH_MAT_GROUP:
			case TdsChunkID::OLD_MAT_GROUP:
			case TdsChunkID::TEX_VERTS:
			case TdsChunkID::SMOOTH_GROUP:
			case TdsChunkID::MESH_MATRIX:
			case TdsChunkID::MESH_COLOR:
			case TdsChunkID::MESH_TEXTURE_INFO:
			case TdsChunkID::PROC_NAME:
			case TdsChunkID::PROC_DATA:
			case TdsChunkID::MSH_BOXMAP:
			case TdsChunkID::N_D_L_OLD:
			case TdsChunkID::N_CAM_OLD:
			case TdsChunkID::N_DIRECT_LIGHT:
			case TdsChunkID::DL_SPOTLIGHT:
			case TdsChunkID::DL_OFF:
			case TdsChunkID::DL_ATTENUATE:
			case TdsChunkID::DL_RAYSHAD:
			case TdsChunkID::DL_SHADOWED:
			case TdsChunkID::DL_LOCAL_SHADOW:
			case TdsChunkID::DL_LOCAL_SHADOW2:
			case TdsChunkID::DL_SEE_CONE:
			case TdsChunkID::DL_SPOT_RECTANGULAR:
			case TdsChunkID::DL_SPOT_OVERSHOOT:
			case TdsChunkID::DL_SPOT_PROJECTOR:
			case TdsChunkID::DL_EXCLUDE:
			case TdsChunkID::DL_RANGE:
			case TdsChunkID::DL_SPOT_ROLL:
			case TdsChunkID::DL_SPOT_ASPECT:
			case TdsChunkID::DL_RAY_BIAS:
			case TdsChunkID::DL_INNER_RANGE:
			case TdsChunkID::DL_OUTER_RANGE:
			case TdsChunkID::DL_MULTIPLIER:
			case TdsChunkID::N_AMBIENT_LIGHT:
			case TdsChunkID::N_CAMERA:
			case TdsChunkID::CAM_SEE_CONE:
			case TdsChunkID::CAM_RANGES:
			case TdsChunkID::M3DMAGIC:
			case TdsChunkID::HIERARCHY:
			case TdsChunkID::PARENT_OBJECT:
			case TdsChunkID::PIVOT_OBJECT:
			case TdsChunkID::PIVOT_LIMITS:
			case TdsChunkID::PIVOT_ORDER:
			case TdsChunkID::XLATE_RANGE:
			case TdsChunkID::POLY_2D:
			case TdsChunkID::SHAPE_OK:
			case TdsChunkID::SHAPE_NOT_OK:
			case TdsChunkID::SHAPE_HOOK:
			case TdsChunkID::PATH_3D:
			case TdsChunkID::PATH_MATRIX:
			case TdsChunkID::SHAPE_2D:
			case TdsChunkID::M_SCALE:
			case TdsChunkID::M_TWIST:
			case TdsChunkID::M_TEETER:
			case TdsChunkID::M_FIT:
			case TdsChunkID::M_BEVEL:
			case TdsChunkID::XZ_CURVE:
			case TdsChunkID::YZ_CURVE:
			case TdsChunkID::INTERPCT:
			case TdsChunkID::DEFORM_LIMIT:
			case TdsChunkID::USE_CONTOUR:
			case TdsChunkID::USE_TWEEN:
			case TdsChunkID::USE_SCALE:
			case TdsChunkID::USE_TWIST:
			case TdsChunkID::USE_TEETER:
			case TdsChunkID::USE_FIT:
			case TdsChunkID::USE_BEVEL:
			case TdsChunkID::VIEWPORT_LAYOUT_OLD:
			case TdsChunkID::VIEWPORT_LAYOUT:
			case TdsChunkID::VIEWPORT_DATA_OLD:
			case TdsChunkID::VIEWPORT_DATA:
			case TdsChunkID::VIEWPORT_DATA_3:
			case TdsChunkID::VIEWPORT_SIZE:
			case TdsChunkID::NETWORK_VIEW:
			case TdsChunkID::XDATA_SECTION:
			case TdsChunkID::XDATA_ENTRY:
			case TdsChunkID::XDATA_APPNAME:
			case TdsChunkID::XDATA_STRING:
			case TdsChunkID::XDATA_FLOAT:
			case TdsChunkID::XDATA_DOUBLE:
			case TdsChunkID::XDATA_SHORT:
			case TdsChunkID::XDATA_LONG:
			case TdsChunkID::XDATA_VOID:
			case TdsChunkID::XDATA_GROUP:
			case TdsChunkID::XDATA_RFU6:
			case TdsChunkID::XDATA_RFU5:
			case TdsChunkID::XDATA_RFU4:
			case TdsChunkID::XDATA_RFU3:
			case TdsChunkID::XDATA_RFU2:
			case TdsChunkID::XDATA_RFU1:
			case TdsChunkID::PARENT_NAME:
			case TdsChunkID::MAT_NAME:
			case TdsChunkID::MAT_AMBIENT:
			case TdsChunkID::MAT_DIFFUSE:
			case TdsChunkID::MAT_SPECULAR:
			case TdsChunkID::MAT_SHININESS:
			case TdsChunkID::MAT_SHIN2PCT:
			case TdsChunkID::MAT_SHIN3PCT:
			case TdsChunkID::MAT_TRANSPARENCY:
			case TdsChunkID::MAT_XPFALL:
			case TdsChunkID::MAT_REFBLUR:
			case TdsChunkID::MAT_SELF_ILLUM:
			case TdsChunkID::MAT_TWO_SIDE:
			case TdsChunkID::MAT_DECAL:
			case TdsChunkID::MAT_ADDITIVE:
			case TdsChunkID::MAT_SELF_ILPCT:
			case TdsChunkID::MAT_WIRE:
			case TdsChunkID::MAT_SUPERSMP:
			case TdsChunkID::MAT_WIRESIZE:
			case TdsChunkID::MAT_FACEMAP:
			case TdsChunkID::MAT_XPFALLIN:
			case TdsChunkID::MAT_PHONGSOFT:
			case TdsChunkID::MAT_WIREABS:
			case TdsChunkID::MAT_SHADING:
			case TdsChunkID::MAT_TEXMAP:
			case TdsChunkID::MAT_SPECMAP:
			case TdsChunkID::MAT_OPACMAP:
			case TdsChunkID::MAT_REFLMAP:
			case TdsChunkID::MAT_BUMPMAP:
			case TdsChunkID::MAT_USE_XPFALL:
			case TdsChunkID::MAT_USE_REFBLUR:
			case TdsChunkID::MAT_BUMP_PERCENT:
			case TdsChunkID::MAT_MAPNAME:
			case TdsChunkID::MAT_ACUBIC:
			case TdsChunkID::MAT_SXP_TEXT_DATA:
			case TdsChunkID::MAT_SXP_TEXT2_DATA:
			case TdsChunkID::MAT_SXP_OPAC_DATA:
			case TdsChunkID::MAT_SXP_BUMP_DATA:
			case TdsChunkID::MAT_SXP_SPEC_DATA:
			case TdsChunkID::MAT_SXP_SHIN_DATA:
			case TdsChunkID::MAT_SXP_SELFI_DATA:
			case TdsChunkID::MAT_SXP_TEXT_MASKDATA:
			case TdsChunkID::MAT_SXP_TEXT2_MASKDATA:
			case TdsChunkID::MAT_SXP_OPAC_MASKDATA:
			case TdsChunkID::MAT_SXP_BUMP_MASKDATA:
			case TdsChunkID::MAT_SXP_SPEC_MASKDATA:
			case TdsChunkID::MAT_SXP_SHIN_MASKDATA:
			case TdsChunkID::MAT_SXP_SELFI_MASKDATA:
			case TdsChunkID::MAT_SXP_REFL_MASKDATA:
			case TdsChunkID::MAT_TEX2MAP:
			case TdsChunkID::MAT_SHINMAP:
			case TdsChunkID::MAT_SELFIMAP:
			case TdsChunkID::MAT_TEXMASK:
			case TdsChunkID::MAT_TEX2MASK:
			case TdsChunkID::MAT_OPACMASK:
			case TdsChunkID::MAT_BUMPMASK:
			case TdsChunkID::MAT_SHINMASK:
			case TdsChunkID::MAT_SPECMASK:
			case TdsChunkID::MAT_SELFIMASK:
			case TdsChunkID::MAT_REFLMASK:
			case TdsChunkID::MAT_MAP_TILINGOLD:
			case TdsChunkID::MAT_MAP_TILING:
			case TdsChunkID::MAT_MAP_TEXBLUR_OLD:
			case TdsChunkID::MAT_MAP_TEXBLUR:
			case TdsChunkID::MAT_MAP_USCALE:
			case TdsChunkID::MAT_MAP_VSCALE:
			case TdsChunkID::MAT_MAP_UOFFSET:
			case TdsChunkID::MAT_MAP_VOFFSET:
			case TdsChunkID::MAT_MAP_ANG:
			case TdsChunkID::MAT_MAP_COL1:
			case TdsChunkID::MAT_MAP_COL2:
			case TdsChunkID::MAT_MAP_RCOL:
			case TdsChunkID::MAT_MAP_GCOL:
			case TdsChunkID::MAT_MAP_BCOL:
			case TdsChunkID::MAT_ENTRY:
			case TdsChunkID::KFDATA:
			case TdsChunkID::AMBIENT_NODE_TAG:
			case TdsChunkID::OBJECT_NODE_TAG:
			case TdsChunkID::CAMERA_NODE_TAG:
			case TdsChunkID::TARGET_NODE_TAG:
			case TdsChunkID::LIGHT_NODE_TAG:
			case TdsChunkID::L_TARGET_NODE_TAG:
			case TdsChunkID::SPOTLIGHT_NODE_TAG:
			case TdsChunkID::KFSEG:
			case TdsChunkID::KFCURTIME:
			case TdsChunkID::KFHDR:
			case TdsChunkID::NODE_HDR:
			case TdsChunkID::INSTANCE_NAME:
			case TdsChunkID::PRESCALE:
			case TdsChunkID::PIVOT:
			case TdsChunkID::BOUNDBOX:
			case TdsChunkID::MORPH_SMOOTH:
			case TdsChunkID::POS_TRACK_TAG:
			case TdsChunkID::ROT_TRACK_TAG:
			case TdsChunkID::SCL_TRACK_TAG:
			case TdsChunkID::FOV_TRACK_TAG:
			case TdsChunkID::ROLL_TRACK_TAG:
			case TdsChunkID::COL_TRACK_TAG:
			case TdsChunkID::MORPH_TRACK_TAG:
			case TdsChunkID::HOT_TRACK_TAG:
			case TdsChunkID::FALL_TRACK_TAG:
			case TdsChunkID::HIDE_TRACK_TAG:
			case TdsChunkID::NODE_ID:
			case TdsChunkID::C_MDRAWER:
			case TdsChunkID::C_TDRAWER:
			case TdsChunkID::C_SHPDRAWER:
			case TdsChunkID::C_MODDRAWER:
			case TdsChunkID::C_RIPDRAWER:
			case TdsChunkID::C_TXDRAWER:
			case TdsChunkID::C_PDRAWER:
			case TdsChunkID::C_MTLDRAWER:
			case TdsChunkID::C_FLIDRAWER:
			case TdsChunkID::C_CUBDRAWER:
			case TdsChunkID::C_MFILE:
			case TdsChunkID::C_SHPFILE:
			case TdsChunkID::C_MODFILE:
			case TdsChunkID::C_RIPFILE:
			case TdsChunkID::C_TXFILE:
			case TdsChunkID::C_PFILE:
			case TdsChunkID::C_MTLFILE:
			case TdsChunkID::C_FLIFILE:
			case TdsChunkID::C_PALFILE:
			case TdsChunkID::C_TX_STRING:
			case TdsChunkID::C_CONSTS:
			case TdsChunkID::C_SNAPS:
			case TdsChunkID::C_GRIDS:
			case TdsChunkID::C_ASNAPS:
			case TdsChunkID::C_GRID_RANGE:
			case TdsChunkID::C_RENDTYPE:
			case TdsChunkID::C_PROGMODE:
			case TdsChunkID::C_PREVMODE:
			case TdsChunkID::C_MODWMODE:
			case TdsChunkID::C_MODMODEL:
			case TdsChunkID::C_ALL_LINES:
			case TdsChunkID::C_BACK_TYPE:
			case TdsChunkID::C_MD_CS:
			case TdsChunkID::C_MD_CE:
			case TdsChunkID::C_MD_SML:
			case TdsChunkID::C_MD_SMW:
			case TdsChunkID::C_LOFT_WITH_TEXTURE:
			case TdsChunkID::C_LOFT_L_REPEAT:
			case TdsChunkID::C_LOFT_W_REPEAT:
			case TdsChunkID::C_LOFT_UV_NORMALIZE:
			case TdsChunkID::C_WELD_LOFT:
			case TdsChunkID::C_MD_PDET:
			case TdsChunkID::C_MD_SDET:
			case TdsChunkID::C_RGB_RMODE:
			case TdsChunkID::C_RGB_HIDE:
			case TdsChunkID::C_RGB_MAPSW:
			case TdsChunkID::C_RGB_TWOSIDE:
			case TdsChunkID::C_RGB_SHADOW:
			case TdsChunkID::C_RGB_AA:
			case TdsChunkID::C_RGB_OVW:
			case TdsChunkID::C_RGB_OVH:
			case TdsChunkID::CMAGIC:
			case TdsChunkID::C_RGB_PICTYPE:
			case TdsChunkID::C_RGB_OUTPUT:
			case TdsChunkID::C_RGB_TODISK:
			case TdsChunkID::C_RGB_COMPRESS:
			case TdsChunkID::C_JPEG_COMPRESSION:
			case TdsChunkID::C_RGB_DISPDEV:
			case TdsChunkID::C_RGB_HARDDEV:
			case TdsChunkID::C_RGB_PATH:
			case TdsChunkID::C_BITMAP_DRAWER:
			case TdsChunkID::C_RGB_FILE:
			case TdsChunkID::C_RGB_OVASPECT:
			case TdsChunkID::C_RGB_ANIMTYPE:
			case TdsChunkID::C_RENDER_ALL:
			case TdsChunkID::C_REND_FROM:
			case TdsChunkID::C_REND_TO:
			case TdsChunkID::C_REND_NTH:
			case TdsChunkID::C_PAL_TYPE:
			case TdsChunkID::C_RND_TURBO:
			case TdsChunkID::C_RND_MIP:
			case TdsChunkID::C_BGND_METHOD:
			case TdsChunkID::C_AUTO_REFLECT:
			case TdsChunkID::C_VP_FROM:
			case TdsChunkID::C_VP_TO:
			case TdsChunkID::C_VP_NTH:
			case TdsChunkID::C_REND_TSTEP:
			case TdsChunkID::C_VP_TSTEP:
			case TdsChunkID::C_SRDIAM:
			case TdsChunkID::C_SRDEG:
			case TdsChunkID::C_SRSEG:
			case TdsChunkID::C_SRDIR:
			case TdsChunkID::C_HETOP:
			case TdsChunkID::C_HEBOT:
			case TdsChunkID::C_HEHT:
			case TdsChunkID::C_HETURNS:
			case TdsChunkID::C_HEDEG:
			case TdsChunkID::C_HESEG:
			case TdsChunkID::C_HEDIR:
			case TdsChunkID::C_QUIKSTUFF:
			case TdsChunkID::C_SEE_LIGHTS:
			case TdsChunkID::C_SEE_CAMERAS:
			case TdsChunkID::C_SEE_3D:
			case TdsChunkID::C_MESHSEL:
			case TdsChunkID::C_MESHUNSEL:
			case TdsChunkID::C_POLYSEL:
			case TdsChunkID::C_POLYUNSEL:
			case TdsChunkID::C_SHPLOCAL:
			case TdsChunkID::C_MSHLOCAL:
			case TdsChunkID::C_NUM_FORMAT:
			case TdsChunkID::C_ARCH_DENOM:
			case TdsChunkID::C_IN_DEVICE:
			case TdsChunkID::C_MSCALE:
			case TdsChunkID::C_COMM_PORT:
			case TdsChunkID::C_TAB_BASES:
			case TdsChunkID::C_TAB_DIVS:
			case TdsChunkID::C_MASTER_SCALES:
			case TdsChunkID::C_SHOW_1STVERT:
			case TdsChunkID::C_SHAPER_OK:
			case TdsChunkID::C_LOFTER_OK:
			case TdsChunkID::C_EDITOR_OK:
			case TdsChunkID::C_KEYFRAMER_OK:
			case TdsChunkID::C_PICKSIZE:
			case TdsChunkID::C_MAPTYPE:
			case TdsChunkID::C_MAP_DISPLAY:
			case TdsChunkID::C_TILE_XY:
			case TdsChunkID::C_MAP_XYZ:
			case TdsChunkID::C_MAP_SCALE:
			case TdsChunkID::C_MAP_MATRIX_OLD:
			case TdsChunkID::C_MAP_MATRIX:
			case TdsChunkID::C_MAP_WID_HT:
			case TdsChunkID::C_OBNAME:
			case TdsChunkID::C_CAMNAME:
			case TdsChunkID::C_LTNAME:
			case TdsChunkID::C_CUR_MNAME:
			case TdsChunkID::C_CURMTL_FROM_MESH:
			case TdsChunkID::C_GET_SHAPE_MAKE_FACES:
			case TdsChunkID::C_DETAIL:
			case TdsChunkID::C_VERTMARK:
			case TdsChunkID::C_MSHAX:
			case TdsChunkID::C_MSHCP:
			case TdsChunkID::C_USERAX:
			case TdsChunkID::C_SHOOK:
			case TdsChunkID::C_RAX:
			case TdsChunkID::C_STAPE:
			case TdsChunkID::C_LTAPE:
			case TdsChunkID::C_ETAPE:
			case TdsChunkID::C_KTAPE:
			case TdsChunkID::C_SPHSEGS:
			case TdsChunkID::C_GEOSMOOTH:
			case TdsChunkID::C_HEMISEGS:
			case TdsChunkID::C_PRISMSEGS:
			case TdsChunkID::C_PRISMSIDES:
			case TdsChunkID::C_TUBESEGS:
			case TdsChunkID::C_TUBESIDES:
			case TdsChunkID::C_TORSEGS:
			case TdsChunkID::C_TORSIDES:
			case TdsChunkID::C_CONESIDES:
			case TdsChunkID::C_CONESEGS:
			case TdsChunkID::C_NGPARMS:
			case TdsChunkID::C_PTHLEVEL:
			case TdsChunkID::C_MSCSYM:
			case TdsChunkID::C_MFTSYM:
			case TdsChunkID::C_MTTSYM:
			case TdsChunkID::C_SMOOTHING:
			case TdsChunkID::C_MODICOUNT:
			case TdsChunkID::C_FONTSEL:
			case TdsChunkID::C_TESS_TYPE:
			case TdsChunkID::C_TESS_TENSION:
			case TdsChunkID::C_SEG_START:
			case TdsChunkID::C_SEG_END:
			case TdsChunkID::C_CURTIME:
			case TdsChunkID::C_ANIMLENGTH:
			case TdsChunkID::C_PV_FROM:
			case TdsChunkID::C_PV_TO:
			case TdsChunkID::C_PV_DOFNUM:
			case TdsChunkID::C_PV_RNG:
			case TdsChunkID::C_PV_NTH:
			case TdsChunkID::C_PV_TYPE:
			case TdsChunkID::C_PV_METHOD:
			case TdsChunkID::C_PV_FPS:
			case TdsChunkID::C_VTR_FRAMES:
			case TdsChunkID::C_VTR_HDTL:
			case TdsChunkID::C_VTR_HD:
			case TdsChunkID::C_VTR_TL:
			case TdsChunkID::C_VTR_IN:
			case TdsChunkID::C_VTR_PK:
			case TdsChunkID::C_VTR_SH:
			case TdsChunkID::C_WORK_MTLS:
			case TdsChunkID::C_WORK_MTLS_2:
			case TdsChunkID::C_WORK_MTLS_3:
			case TdsChunkID::C_WORK_MTLS_4:
			case TdsChunkID::C_BGTYPE:
			case TdsChunkID::C_MEDTILE:
			case TdsChunkID::C_LO_CONTRAST:
			case TdsChunkID::C_HI_CONTRAST:
			case TdsChunkID::C_FROZ_DISPLAY:
			case TdsChunkID::C_BOOLWELD:
			case TdsChunkID::C_BOOLTYPE:
			case TdsChunkID::C_ANG_THRESH:
			case TdsChunkID::C_SS_THRESH:
			case TdsChunkID::C_TEXTURE_BLUR_DEFAULT:
			case TdsChunkID::C_MAPDRAWER:
			case TdsChunkID::C_MAPDRAWER1:
			case TdsChunkID::C_MAPDRAWER2:
			case TdsChunkID::C_MAPDRAWER3:
			case TdsChunkID::C_MAPDRAWER4:
			case TdsChunkID::C_MAPDRAWER5:
			case TdsChunkID::C_MAPDRAWER6:
			case TdsChunkID::C_MAPDRAWER7:
			case TdsChunkID::C_MAPDRAWER8:
			case TdsChunkID::C_MAPDRAWER9:
			case TdsChunkID::C_MAPDRAWER_ENTRY:
			case TdsChunkID::C_BACKUP_FILE:
			case TdsChunkID::C_DITHER_256:
			case TdsChunkID::C_SAVE_LAST:
			case TdsChunkID::C_USE_ALPHA:
			case TdsChunkID::C_TGA_DEPTH:
			case TdsChunkID::C_REND_FIELDS:
			case TdsChunkID::C_REFLIP:
			case TdsChunkID::C_SEL_ITEMTOG:
			case TdsChunkID::C_SEL_RESET:
			case TdsChunkID::C_STICKY_KEYINF:
			case TdsChunkID::C_WELD_THRESHOLD:
			case TdsChunkID::C_ZCLIP_POINT:
			case TdsChunkID::C_ALPHA_SPLIT:
			case TdsChunkID::C_KF_SHOW_BACKFACE:
			case TdsChunkID::C_OPTIMIZE_LOFT:
			case TdsChunkID::C_TENS_DEFAULT:
			case TdsChunkID::C_CONT_DEFAULT:
			case TdsChunkID::C_BIAS_DEFAULT:
			case TdsChunkID::C_DXFNAME_SRC:
			case TdsChunkID::C_AUTO_WELD:
			case TdsChunkID::C_AUTO_UNIFY:
			case TdsChunkID::C_AUTO_SMOOTH:
			case TdsChunkID::C_DXF_SMOOTH_ANG:
			case TdsChunkID::C_SMOOTH_ANG:
			case TdsChunkID::C_WORK_MTLS_5:
			case TdsChunkID::C_WORK_MTLS_6:
			case TdsChunkID::C_WORK_MTLS_7:
			case TdsChunkID::C_WORK_MTLS_8:
			case TdsChunkID::C_WORKMTL:
			case TdsChunkID::C_SXP_TEXT_DATA:
			case TdsChunkID::C_SXP_OPAC_DATA:
			case TdsChunkID::C_SXP_BUMP_DATA:
			case TdsChunkID::C_SXP_SHIN_DATA:
			case TdsChunkID::C_SXP_TEXT2_DATA:
			case TdsChunkID::C_SXP_SPEC_DATA:
			case TdsChunkID::C_SXP_SELFI_DATA:
			case TdsChunkID::C_SXP_TEXT_MASKDATA:
			case TdsChunkID::C_SXP_TEXT2_MASKDATA:
			case TdsChunkID::C_SXP_OPAC_MASKDATA:
			case TdsChunkID::C_SXP_BUMP_MASKDATA:
			case TdsChunkID::C_SXP_SPEC_MASKDATA:
			case TdsChunkID::C_SXP_SHIN_MASKDATA:
			case TdsChunkID::C_SXP_REFL_MASKDATA:
			case TdsChunkID::C_NET_USE_VPOST:
			case TdsChunkID::C_NET_USE_GAMMA:
			case TdsChunkID::C_NET_FIELD_ORDER:
			case TdsChunkID::C_BLUR_FRAMES:
			case TdsChunkID::C_BLUR_SAMPLES:
			case TdsChunkID::C_BLUR_DUR:
			case TdsChunkID::C_HOT_METHOD:
			case TdsChunkID::C_HOT_CHECK:
			case TdsChunkID::C_PIXEL_SIZE:
			case TdsChunkID::C_DISP_GAMMA:
			case TdsChunkID::C_FBUF_GAMMA:
			case TdsChunkID::C_FILE_OUT_GAMMA:
			case TdsChunkID::C_FILE_IN_GAMMA:
			case TdsChunkID::C_GAMMA_CORRECT:
			case TdsChunkID::C_APPLY_DISP_GAMMA:
			case TdsChunkID::C_APPLY_FBUF_GAMMA:
			case TdsChunkID::C_APPLY_FILE_GAMMA:
			case TdsChunkID::C_FORCE_WIRE:
			case TdsChunkID::C_RAY_SHADOWS:
			case TdsChunkID::C_MASTER_AMBIENT:
			case TdsChunkID::C_SUPER_SAMPLE:
			case TdsChunkID::C_OBJECT_MBLUR:
			case TdsChunkID::C_MBLUR_DITHER:
			case TdsChunkID::C_DITHER_24:
			case TdsChunkID::C_SUPER_BLACK:
			case TdsChunkID::C_SAFE_FRAME:
			case TdsChunkID::C_VIEW_PRES_RATIO:
			case TdsChunkID::C_BGND_PRES_RATIO:
			case TdsChunkID::C_NTH_SERIAL_NUM:
			case TdsChunkID::VPDATA:
			case TdsChunkID::P_QUEUE_ENTRY:
			case TdsChunkID::P_QUEUE_IMAGE:
			case TdsChunkID::P_QUEUE_USEIGAMMA:
			case TdsChunkID::P_QUEUE_PROC:
			case TdsChunkID::P_QUEUE_SOLID:
			case TdsChunkID::P_QUEUE_GRADIENT:
			case TdsChunkID::P_QUEUE_KF:
			case TdsChunkID::P_QUEUE_MOTBLUR:
			case TdsChunkID::P_QUEUE_MB_REPEAT:
			case TdsChunkID::P_QUEUE_NONE:
			case TdsChunkID::P_QUEUE_RESIZE:
			case TdsChunkID::P_QUEUE_OFFSET:
			case TdsChunkID::P_QUEUE_ALIGN:
			case TdsChunkID::P_CUSTOM_SIZE:
			case TdsChunkID::P_ALPH_NONE:
			case TdsChunkID::P_ALPH_PSEUDO:
			case TdsChunkID::P_ALPH_OP_PSEUDO:
			case TdsChunkID::P_ALPH_BLUR:
			case TdsChunkID::P_ALPH_PCOL:
			case TdsChunkID::P_ALPH_C0:
			case TdsChunkID::P_ALPH_OP_KEY:
			case TdsChunkID::P_ALPH_KCOL:
			case TdsChunkID::P_ALPH_OP_NOCONV:
			case TdsChunkID::P_ALPH_IMAGE:
			case TdsChunkID::P_ALPH_ALPHA:
			case TdsChunkID::P_ALPH_QUES:
			case TdsChunkID::P_ALPH_QUEIMG:
			case TdsChunkID::P_ALPH_CUTOFF:
			case TdsChunkID::P_ALPHANEG:
			case TdsChunkID::P_TRAN_NONE:
			case TdsChunkID::P_TRAN_IMAGE:
			case TdsChunkID::P_TRAN_FRAMES:
			case TdsChunkID::P_TRAN_FADEIN:
			case TdsChunkID::P_TRAN_FADEOUT:
			case TdsChunkID::P_TRANNEG:
			case TdsChunkID::P_RANGES:
			case TdsChunkID::P_PROC_DATA:
			case TdsChunkID::POS_TRACK_TAG_KEY:
			case TdsChunkID::ROT_TRACK_TAG_KEY:
			case TdsChunkID::SCL_TRACK_TAG_KEY:
			case TdsChunkID::FOV_TRACK_TAG_KEY:
			case TdsChunkID::ROLL_TRACK_TAG_KEY:
			case TdsChunkID::COL_TRACK_TAG_KEY:
			case TdsChunkID::MORPH_TRACK_TAG_KEY:
			case TdsChunkID::HOT_TRACK_TAG_KEY:
			case TdsChunkID::FALL_TRACK_TAG_KEY:
			case TdsChunkID::POINT_ARRAY_ENTRY:
			case TdsChunkID::POINT_FLAG_ARRAY_ENTRY:
			case TdsChunkID::FACE_ARRAY_ENTRY:
			case TdsChunkID::MSH_MAT_GROUP_ENTRY:
			case TdsChunkID::TEX_VERTS_ENTRY:
			case TdsChunkID::SMOOTH_GROUP_ENTRY:
			case TdsChunkID::DUMMY:
				result = true;
				break;
			}

			if ( result )
				result = chunk.length + parent.bytesRead <= parent.length;
			return result;
		}

		static void discardChunk( c3d::BinaryFile & file
			, TdsChunk & chunk )
		{
			if ( chunk.length > chunk.bytesRead )
			{
				file.seek( std::min( chunk.length - chunk.bytesRead
					, uint32_t( file.getLength() - file.tell() ) ), c3d::File::OffsetMode::eCurrent );
				chunk.bytesRead = chunk.length;
			}
		}

		static void readChunk( c3d::BinaryFile & file
			, TdsChunk & chunk )
		{
			try
			{
				chunk.bytesRead = uint32_t( file.read( chunk.chunkId ) );
				chunk.bytesRead += uint32_t( file.read( chunk.length ) );
			}
			catch ( ... )
			{
				c3d::log::warn << cuT( "Exception caught when reading chunk\n" );
			}
		}

		static uint32_t getString( c3d::BinaryFile & file
			, TdsChunk const & chunk
			, c3d::String & value )
		{
			std::array< char, 255u > data;

			try
			{
				auto buffer = data.data();
				int index = 0;
				file.read( *buffer );

				while ( file.isOk()
					&& chunk.bytesRead + index <= chunk.length
					&& *( buffer + index ) != 0 )
				{
					++index;
					file.read( *( buffer + index ) );
				}

				value = c3d::makeString( buffer );
			}
			catch ( ... )
			{
				c3d::log::warn << cuT( "Exception caught when loading string.\n" );
			}

			return uint32_t( value.size() );
		}

		static void getF32( c3d::BinaryFile & file
			, TdsChunk & chunk
			, std::optional< c3d::f32 > & value )
		{
			c3d::s16 result;
			chunk.bytesRead += uint32_t( file.read( result ) );
			value = result;
		}

		static void getS16( c3d::BinaryFile & file
			, TdsChunk & chunk
			, std::optional< c3d::s16 > & value )
		{
			c3d::s16 result;
			chunk.bytesRead += uint32_t( file.read( result ) );
			value = result;
		}

		static bool getPercentage( c3d::BinaryFile & file
			, TdsChunk & chunk
			, std::optional< c3d::f32 > & value )
		{
			bool result{};
			TdsChunk tempChunk;

			try
			{
				readChunk( file, tempChunk );

				if ( tempChunk.chunkId == TdsChunkID::FLOAT_PERCENTAGE )
				{
					float f;
					tempChunk.bytesRead += uint32_t( file.read( f ) );
					value = f * 100.0f / float( 0xFFFF );
					result = true;
				}
				else if ( tempChunk.chunkId == TdsChunkID::INT_PERCENTAGE )
				{
					c3d::s16 s;
					tempChunk.bytesRead += uint32_t( file.read( s ) );
					value = float( s ) / float( 0xFFFF );
					result = true;
				}

				chunk.bytesRead += tempChunk.bytesRead;
			}
			catch ( ... )
			{
				c3d::log::warn << cuT( "Exception caught when loading colour.\n" );
				chunk.bytesRead = chunk.length;
			}
			return result;
		}

		static bool getRgbColour( c3d::BinaryFile & file
			, TdsChunk & chunk
			, std::optional< c3d::RgbColour > & value )
		{
			bool result{};
			TdsChunk tempChunk;

			try
			{
				readChunk( file, tempChunk );

				if ( tempChunk.chunkId == TdsChunkID::LIN_COLOR_F )
				{
					c3d::HdrRgbColour c;
					c3d::Array< float, 3u > colour;
					tempChunk.bytesRead += uint32_t( file.readArray( colour.data(), 3 ) );
					c.red() = colour[0];
					c.green() = colour[1];
					c.blue() = colour[2];
					value = c3d::RgbColour{ c, 2.2f };
					result = true;
				}
				else if ( tempChunk.chunkId == TdsChunkID::COLOR_F )
				{
					c3d::RgbColour c;
					c3d::Array< float, 3u > colour;
					tempChunk.bytesRead += uint32_t( file.readArray( colour.data(), 3 ) );
					c.red() = colour[0];
					c.green() = colour[1];
					c.blue() = colour[2];
					value = c;
					result = true;
				}
				else if ( tempChunk.chunkId == TdsChunkID::LIN_COLOR_24 )
				{
					c3d::HdrRgbColour c;
					c3d::Array< uint8_t, 3u > colour;
					tempChunk.bytesRead += uint32_t( file.readArray( colour.data(), 3 ) );
					c.red() = float( colour[0] ) / 255.0f;
					c.green() = float( colour[1] ) / 255.0f;
					c.blue() = float( colour[2] ) / 255.0f;
					value = c3d::RgbColour{ c, 2.2f };
					result = true;
				}
				else if ( tempChunk.chunkId == TdsChunkID::COLOR_24 )
				{
					c3d::RgbColour c;
					c3d::Array< uint8_t, 3u > colour;
					tempChunk.bytesRead += uint32_t( file.readArray( colour.data(), 3 ) );
					c.red() = float( colour[0] ) / 255.0f;
					c.green() = float( colour[1] ) / 255.0f;
					c.blue() = float( colour[2] ) / 255.0f;
					value = c;
					result = true;
				}
				else if ( tempChunk.chunkId == TdsChunkID::FLOAT_PERCENTAGE )
				{
					float f;
					tempChunk.bytesRead += uint32_t( file.read( f ) );
					c3d::RgbColour c;
					c.red() = f;
					c.green() = f;
					c.blue() = f;
					value = c;
					result = true;
				}
				else if ( tempChunk.chunkId == TdsChunkID::INT_PERCENTAGE )
				{
					c3d::s16 s;
					tempChunk.bytesRead += uint32_t( file.read( s ) );
					c3d::RgbColour c;
					c.red() = float( s ) / float( 0xFFFF );
					c.green() = float( s ) / float( 0xFFFF );
					c.blue() = float( s ) / float( 0xFFFF );
					value = c;
					result = true;
				}

				chunk.bytesRead += tempChunk.bytesRead;
			}
			catch ( ... )
			{
				c3d::log::warn << cuT( "Exception caught when loading colour.\n" );
				chunk.bytesRead = chunk.length;
			}
			return result;
		}

		static void processTextureChunk( c3d::BinaryFile & file
			, TdsChunk & chunk
			, TdsTextureData & texture )
		{
			TdsChunk currentChunk;
			bool carryOn = true;
			c3d::String mapName;
			c3d::u16 s;

			while ( file.isOk() && chunk.bytesRead < chunk.length && carryOn )
			{
				readChunk( file, currentChunk );

				if ( !isValidChunk( currentChunk, chunk ) )
				{
					carryOn = false;
				}
				else
				{
					switch ( currentChunk.chunkId )
					{
					case TdsChunkID::MAT_MAPNAME:
						getString( file, currentChunk, mapName );
						texture.path = c3d::Path{ mapName };
						currentChunk.bytesRead = currentChunk.length;
						break;
					case TdsChunkID::MAT_MAP_USCALE:
						getF32( file, currentChunk, texture.scaleU );
						break;
					case TdsChunkID::MAT_MAP_VSCALE:
						getF32( file, currentChunk, texture.scaleV );
						break;
					case TdsChunkID::MAT_MAP_UOFFSET:
						getF32( file, currentChunk, texture.offsetU );
						break;
					case TdsChunkID::MAT_MAP_VOFFSET:
						getF32( file, currentChunk, texture.offsetV );
						break;
					case TdsChunkID::MAT_MAP_ANG:
						getF32( file, currentChunk, texture.rotation );
						break;
					case TdsChunkID::MAT_MAP_TILING:
						currentChunk.bytesRead += uint32_t( file.read( s ) );
						if ( c3d::checkFlag( s, c3d::u16( 0x02u ) ) )
							texture.wrapMode = c3d::WrapMode::eMirrorClampToEdge;
						else
							texture.wrapMode = c3d::WrapMode::eRepeat;
						break;
					default:
						break;
					}

					// discard the chunk anyway.
					discardChunk( file, currentChunk );
				}

				chunk.bytesRead += currentChunk.bytesRead;
			}

			discardChunk( file, chunk );
		}

		static void processMaterialChunk( TdsImporterFile const & importerFile
			, c3d::BinaryFile & file
			, TdsChunk & chunk
			, TdsSceneData & sceneData )
		{
			TdsChunk currentChunk;
			bool carryOn{ true };
			TdsMaterialData material;

			while ( file.isOk() && chunk.bytesRead < chunk.length && carryOn )
			{
				readChunk( file, currentChunk );

				if ( !isValidChunk( currentChunk, chunk ) )
				{
					carryOn = false;
				}
				else
				{
					switch ( currentChunk.chunkId )
					{
					case TdsChunkID::MAT_NAME:
						if ( !material.name.empty() )
							sceneData.materials.try_emplace( material.name, material );
						getString( file, currentChunk, material.name );
						currentChunk.bytesRead = currentChunk.length;
						material.name = importerFile.getMaterialName( material.name );
						break;
					case TdsChunkID::MAT_SHADING:
						// Not handled
						break;
					case TdsChunkID::MAT_AMBIENT:
						getRgbColour( file, currentChunk, material.ambient );
						break;
					case TdsChunkID::MAT_DIFFUSE:
						getRgbColour( file, currentChunk, material.diffuse );
						break;
					case TdsChunkID::MAT_SPECULAR:
						getRgbColour( file, currentChunk, material.specular );
						break;
					case TdsChunkID::MAT_SELF_ILLUM:
						getRgbColour( file, currentChunk, material.emissive );
						break;
					case TdsChunkID::MAT_SELF_ILPCT:
						if ( getPercentage( file, currentChunk, material.emissiveFactor ) )
							*material.emissiveFactor *= float( 0xFFFF ) / 100.0f;
						break;
					case TdsChunkID::MAT_TRANSPARENCY:
						if ( getPercentage( file, currentChunk, material.transparency ) )
							*material.transparency *= float( 0xFFFF ) / 100.0f;
						break;
					case TdsChunkID::MAT_SHININESS:
						if ( getPercentage( file, currentChunk, material.specularExponent ) )
							*material.specularExponent *= float( 0xFFFF );
						break;
					case TdsChunkID::MAT_SHIN2PCT:
						if ( getPercentage( file, currentChunk, material.shininessStrength ) )
							*material.shininessStrength *= float( 0xFFFF ) / 100.0f;
						break;
					case TdsChunkID::MAT_TWO_SIDE:
						material.twoSided = true;
						break;
					case TdsChunkID::MAT_TEXMAP:
						processTextureChunk( file, currentChunk, material.diffuseMap );
						break;
					case TdsChunkID::MAT_SPECMAP:
						processTextureChunk( file, currentChunk, material.specularMap );
						break;
					case TdsChunkID::MAT_SHINMAP:
						processTextureChunk( file, currentChunk, material.shininessMap );
						break;
					case TdsChunkID::MAT_SELFIMAP:
						processTextureChunk( file, currentChunk, material.emissiveFactorMap );
						break;
					case TdsChunkID::MAT_OPACMAP:
						processTextureChunk( file, currentChunk, material.opacityMap );
						break;
					case TdsChunkID::MAT_BUMPMAP:
						processTextureChunk( file, currentChunk, material.heightMap );
						break;
					default:
						break;
					}

					// discard the chunk anyway.
					discardChunk( file, currentChunk );
				}

				chunk.bytesRead += currentChunk.bytesRead;
			}

			discardChunk( file, chunk );

			if ( !material.name.empty() )
				sceneData.materials.try_emplace( material.name, material );
		}

		static void processChunk( TdsImporterFile const & importerFile
			, c3d::BinaryFile & file
			, TdsChunk & chunk
			, TdsSceneData & sceneData )
		{
			TdsChunk currentChunk;
			bool carryOn = true;

			while ( file.isOk() && chunk.bytesRead < chunk.length && carryOn )
			{
				readChunk( file, currentChunk );

				if ( !isValidChunk( currentChunk, chunk ) )
				{
					carryOn = false;
				}
				else
				{
					if ( currentChunk.chunkId == TdsChunkID::MAT_ENTRY )
						processMaterialChunk( importerFile, file, currentChunk, sceneData );
					else if ( currentChunk.chunkId == TdsChunkID::MDATA )
						processChunk( importerFile, file, currentChunk, sceneData );
					else if ( currentChunk.chunkId == TdsChunkID::AMBIENT_LIGHT )
						getRgbColour( file, currentChunk, sceneData.ambient );
				}

				discardChunk( file, currentChunk );
				chunk.bytesRead += currentChunk.bytesRead;
			}

			discardChunk( file, chunk );
		}
	}

	//*********************************************************************************************

	c3d::MbString const TdsImporterFile::Name = "3DS Importer";

	TdsImporterFile::TdsImporterFile( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
		: c3d_assimp::AssimpImporterFile{ engine, scene, path, parameters, progress }
	{
		c3d::BinaryFile file{ path, c3d::File::OpenMode::eRead };

		if ( file.isOk() )
			doPrelistMaterials( file );
	}

	c3d::ImporterFileUPtr TdsImporterFile::create( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
	{
		return c3d::makeUniqueDerived< c3d::ImporterFile, TdsImporterFile >( engine, scene, path, parameters, progress );
	}

	c3d::StringArray TdsImporterFile::listMaterials()
	{
		c3d::StringArray result;
		if ( isValid() )
			for ( auto const & [name, _] : m_sceneData.materials )
				result.emplace_back( name );
		return result;
	}

	c3d::Vector< uint32_t > TdsImporterFile::listTextureAnimations( c3d::Material const & material
		, uint32_t pass )
	{
		c3d::Vector< uint32_t > result;
		return result;
	}

	c3d::MaterialImporterUPtr TdsImporterFile::createMaterialImporter()
	{
		return c3d::makeUniqueDerived< c3d::MaterialImporter, TdsMaterialImporter >( *getOwner() );
	}

	void TdsImporterFile::doPrelistMaterials( c3d::BinaryFile & file )
	{
		TdsChunk chunk;
		file::readChunk( file , chunk );

		if ( chunk.chunkId == TdsChunkID::M3DMAGIC )
			file::processChunk( *this, file, chunk, m_sceneData );
	}

	//*********************************************************************************************
}
