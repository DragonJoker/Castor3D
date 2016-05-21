/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___LWO_IMPORTER_PREREQUISITES_H___
#define ___LWO_IMPORTER_PREREQUISITES_H___

#include <Mesh/Importer.hpp>

#ifndef _WIN32
#	define C3D_Lwo_API
#else
#	ifdef LwoImporter_EXPORTS
#		define C3D_Lwo_API __declspec(dllexport)
#	else
#		define C3D_Lwo_API __declspec(dllimport)
#	endif
#endif

namespace Lwo
{
	typedef uint8_t UI1;
	typedef uint16_t UI2;
	typedef uint32_t UI4;
	typedef char I1;
	typedef short I2;
	typedef int I4;
	typedef float F4;
	typedef std::string S0;
#define MAKE_ID( ch0, ch1, ch2, ch3 )	((UI4( ch0 ) << 24) | (UI4( ch1 ) << 16) | (UI4( ch2 ) << 8) | (UI4( ch3 ) << 0))
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2012
	\~english
	\brief		Enumeration of supported ID Tags
	\~french
	\brief		Enumération des ID Tags supportés
	*/
	typedef enum eID_TAG
	{
		eID_TAG_FORM						= MAKE_ID( 'F', 'O', 'R', 'M' )
											  ,	eID_TAG_LAYR						= MAKE_ID( 'L', 'A', 'Y', 'R' )
													  ,	eID_TAG_PNTS						= MAKE_ID( 'P', 'N', 'T', 'S' )
															  ,	eID_TAG_VMAP						= MAKE_ID( 'V', 'M', 'A', 'P' )
																	  ,	eID_TAG_POLS						= MAKE_ID( 'P', 'O', 'L', 'S' )
																			  ,	eID_TAG_TAGS						= MAKE_ID( 'T', 'A', 'G', 'S' )
																					  ,	eID_TAG_PTAG						= MAKE_ID( 'P', 'T', 'A', 'G' )
																							  ,	eID_TAG_VMAD						= MAKE_ID( 'V', 'M', 'A', 'D' )
																									  ,	eID_TAG_VMPA						= MAKE_ID( 'V', 'M', 'P', 'A' )
																											  ,	eID_TAG_ENVL						= MAKE_ID( 'E', 'N', 'V', 'L' )
																													  //		Enveloppe Subchunk IDs
																													  ,		eID_TAG_ENVL_TYPE				= MAKE_ID( 'T', 'Y', 'P', 'E' )
																															  ,		eID_TAG_ENVL_PRE				= MAKE_ID( 'P', 'R', 'E', ' ' )
																																	  ,		eID_TAG_ENVL_POST				= MAKE_ID( 'P', 'O', 'S', 'T' )
																																			  ,		eID_TAG_ENVL_KEY				= MAKE_ID( 'K', 'E', 'Y', ' ' )
																																					  ,		eID_TAG_ENVL_SPAN				= MAKE_ID( 'S', 'P', 'A', 'N' )
																																							  ,		eID_TAG_ENVL_CHAN				= MAKE_ID( 'C', 'H', 'A', 'N' )
																																									  ,		eID_TAG_ENVL_NAME				= MAKE_ID( 'N', 'A', 'M', 'E' )
																																											  ,	eID_TAG_CLIP						= MAKE_ID( 'C', 'L', 'I', 'P' )
																																													  //		Clip Subchunk IDs
																																													  ,		eID_TAG_CLIP_STIL				= MAKE_ID( 'S', 'T', 'I', 'L' )
																																															  ,		eID_TAG_CLIP_ANIM				= MAKE_ID( 'A', 'N', 'I', 'M' )
																																																	  ,		eID_TAG_CLIP_XREF				= MAKE_ID( 'X', 'R', 'E', 'F' )
																																																			  ,		eID_TAG_CLIP_STCC				= MAKE_ID( 'S', 'T', 'C', 'C' )
																																																					  ,		eID_TAG_CLIP_TIME				= MAKE_ID( 'T', 'I', 'M', 'E' )
																																																							  ,		eID_TAG_CLIP_CLRS				= MAKE_ID( 'C', 'L', 'R', 'S' )
																																																									  ,		eID_TAG_CLIP_CLRA				= MAKE_ID( 'C', 'L', 'R', 'A' )
																																																											  ,		eID_TAG_CLIP_FILT				= MAKE_ID( 'F', 'I', 'L', 'T' )
																																																													  ,		eID_TAG_CLIP_DITH				= MAKE_ID( 'D', 'I', 'T', 'H' )
																																																															  ,		eID_TAG_CLIP_CONT				= MAKE_ID( 'C', 'O', 'N', 'T' )
																																																																	  ,		eID_TAG_CLIP_BRIT				= MAKE_ID( 'B', 'R', 'I', 'T' )
																																																																			  ,		eID_TAG_CLIP_SATR				= MAKE_ID( 'S', 'A', 'T', 'R' )
																																																																					  ,		eID_TAG_CLIP_HUE				= MAKE_ID( 'H', 'U', 'E', ' ' )
																																																																							  ,		eID_TAG_CLIP_GAMM				= MAKE_ID( 'G', 'A', 'M', 'M' )
																																																																									  ,		eID_TAG_CLIP_NEGA				= MAKE_ID( 'N', 'E', 'G', 'A' )
																																																																											  ,		eID_TAG_CLIP_IFLT				= MAKE_ID( 'I', 'F', 'L', 'T' )
																																																																													  ,		eID_TAG_CLIP_PFLT				= MAKE_ID( 'P', 'F', 'L', 'T' )
																																																																															  ,	eID_TAG_SURF						= MAKE_ID( 'S', 'U', 'R', 'F' )
																																																																																	  //		Basic surface parameters Subchunk IDs
																																																																																	  ,		eID_TAG_SURF_COLR				= MAKE_ID( 'C', 'O', 'L', 'R' )
																																																																																			  ,		eID_TAG_SURF_DIFF				= MAKE_ID( 'D', 'I', 'F', 'F' )
																																																																																					  ,		eID_TAG_SURF_LUMI				= MAKE_ID( 'L', 'U', 'M', 'I' )
																																																																																							  ,		eID_TAG_SURF_SPEC				= MAKE_ID( 'S', 'P', 'E', 'C' )
																																																																																									  ,		eID_TAG_SURF_REFL				= MAKE_ID( 'R', 'E', 'F', 'L' )
																																																																																											  ,		eID_TAG_SURF_TRAN				= MAKE_ID( 'T', 'R', 'A', 'N' )
																																																																																													  ,		eID_TAG_SURF_TRNL				= MAKE_ID( 'T', 'R', 'N', 'L' )
																																																																																															  ,		eID_TAG_SURF_GLOS				= MAKE_ID( 'G', 'L', 'O', 'S' )
																																																																																																	  ,		eID_TAG_SURF_SHRP				= MAKE_ID( 'S', 'H', 'R', 'P' )
																																																																																																			  ,		eID_TAG_SURF_BUMP				= MAKE_ID( 'B', 'U', 'M', 'P' )
																																																																																																					  ,		eID_TAG_SURF_SIDE				= MAKE_ID( 'S', 'I', 'D', 'E' )
																																																																																																							  ,		eID_TAG_SURF_SMAN				= MAKE_ID( 'S', 'M', 'A', 'N' )
																																																																																																									  ,		eID_TAG_SURF_RFOP				= MAKE_ID( 'R', 'F', 'O', 'P' )
																																																																																																											  ,		eID_TAG_SURF_RIMG				= MAKE_ID( 'R', 'I', 'M', 'G' )
																																																																																																													  ,		eID_TAG_SURF_RSAN				= MAKE_ID( 'R', 'S', 'A', 'N' )
																																																																																																															  ,		eID_TAG_SURF_RBLR				= MAKE_ID( 'R', 'B', 'L', 'R' )
																																																																																																																	  ,		eID_TAG_SURF_RIND				= MAKE_ID( 'R', 'I', 'N', 'D' )
																																																																																																																			  ,		eID_TAG_SURF_TROP				= MAKE_ID( 'T', 'R', 'O', 'P' )
																																																																																																																					  ,		eID_TAG_SURF_TIMG				= MAKE_ID( 'T', 'I', 'M', 'G' )
																																																																																																																							  ,		eID_TAG_SURF_TBLR				= MAKE_ID( 'T', 'B', 'L', 'R' )
																																																																																																																									  ,		eID_TAG_SURF_CLRH				= MAKE_ID( 'C', 'L', 'R', 'H' )
																																																																																																																											  ,		eID_TAG_SURF_CLRF				= MAKE_ID( 'C', 'L', 'R', 'F' )
																																																																																																																													  ,		eID_TAG_SURF_ADTR				= MAKE_ID( 'A', 'D', 'T', 'R' )
																																																																																																																															  ,		eID_TAG_SURF_GLOW				= MAKE_ID( 'G', 'L', 'O', 'W' )
																																																																																																																																	  ,		eID_TAG_SURF_LINE				= MAKE_ID( 'L', 'I', 'N', 'E' )
																																																																																																																																			  ,		eID_TAG_SURF_ALPH				= MAKE_ID( 'A', 'L', 'P', 'H' )
																																																																																																																																					  ,		eID_TAG_SURF_VCOL				= MAKE_ID( 'V', 'C', 'O', 'L' )
																																																																																																																																							  //		Surface block Subchunk IDs
																																																																																																																																							  ,		eID_TAG_SURF_BLOK				= MAKE_ID( 'B', 'L', 'O', 'K' )
																																																																																																																																									  //			Block Header IDs
																																																																																																																																									  ,			eID_TAG_BLOK_IMAP			= MAKE_ID( 'I', 'M', 'A', 'P' )
																																																																																																																																											  //				Image mapping
																																																																																																																																											  ,				eID_TAG_BLOK_PROJ		= MAKE_ID( 'P', 'R', 'O', 'J' )
																																																																																																																																													  ,				eID_TAG_BLOK_IMAG		= MAKE_ID( 'I', 'M', 'A', 'G' )
																																																																																																																																															  ,				eID_TAG_BLOK_WRAP		= MAKE_ID( 'W', 'R', 'A', 'P' )
																																																																																																																																																	  ,				eID_TAG_BLOK_WRPW		= MAKE_ID( 'W', 'R', 'P', 'W' )
																																																																																																																																																			  ,				eID_TAG_BLOK_WRPH		= MAKE_ID( 'W', 'R', 'P', 'H' )
																																																																																																																																																					  ,				eID_TAG_BLOK_VMAP		= MAKE_ID( 'V', 'M', 'A', 'P' )
																																																																																																																																																							  ,				eID_TAG_BLOK_AAST		= MAKE_ID( 'A', 'A', 'S', 'T' )
																																																																																																																																																									  ,				eID_TAG_BLOK_PIXB		= MAKE_ID( 'P', 'I', 'X', 'B' )
																																																																																																																																																											  ,				eID_TAG_BLOK_STCK		= MAKE_ID( 'S', 'T', 'C', 'K' )
																																																																																																																																																													  ,				eID_TAG_BLOK_TAMP		= MAKE_ID( 'T', 'A', 'M', 'P' )
																																																																																																																																																															  ,			eID_TAG_BLOK_PROC			= MAKE_ID( 'P', 'R', 'O', 'C' )
																																																																																																																																																																	  //				Procedural textures
																																																																																																																																																																	  ,				eID_TAG_BLOK_VALU		= MAKE_ID( 'V', 'A', 'L', 'U' )
																																																																																																																																																																			  ,				eID_TAG_BLOK_FUNC		= MAKE_ID( 'F', 'U', 'N', 'C' )
																																																																																																																																																																					  ,			eID_TAG_BLOK_GRAD			= MAKE_ID( 'G', 'R', 'A', 'D' )
																																																																																																																																																																							  //				Gradient textures
																																																																																																																																																																							  ,				eID_TAG_BLOK_PNAM		= MAKE_ID( 'P', 'N', 'A', 'M' )
																																																																																																																																																																									  ,				eID_TAG_BLOK_INAM		= MAKE_ID( 'I', 'N', 'A', 'M' )
																																																																																																																																																																											  ,				eID_TAG_BLOK_GRST		= MAKE_ID( 'G', 'R', 'S', 'T' )
																																																																																																																																																																													  ,				eID_TAG_BLOK_GREN		= MAKE_ID( 'G', 'R', 'E', 'N' )
																																																																																																																																																																															  ,				eID_TAG_BLOK_GRPT		= MAKE_ID( 'G', 'R', 'P', 'T' )
																																																																																																																																																																																	  ,				eID_TAG_BLOK_FKEY		= MAKE_ID( 'F', 'K', 'E', 'Y' )
																																																																																																																																																																																			  ,				eID_TAG_BLOK_IKEY		= MAKE_ID( 'I', 'K', 'E', 'Y' )
																																																																																																																																																																																					  ,			eID_TAG_BLOK_SHDR			= MAKE_ID( 'S', 'H', 'D', 'R' )
																																																																																																																																																																																							  //				Shaders
																																																																																																																																																																																							  ,				eID_TAG_SHDR_FUNC		= MAKE_ID( 'F', 'U', 'N', 'C' )
																																																																																																																																																																																									  //			Block header subchunks IDs
																																																																																																																																																																																									  ,			eID_TAG_BLOK_CHAN			= MAKE_ID( 'C', 'H', 'A', 'N' )
																																																																																																																																																																																											  ,			eID_TAG_BLOK_ENAB			= MAKE_ID( 'E', 'N', 'A', 'B' )
																																																																																																																																																																																													  ,			eID_TAG_BLOK_OPAC			= MAKE_ID( 'O', 'P', 'A', 'C' )
																																																																																																																																																																																															  ,			eID_TAG_BLOK_AXIS			= MAKE_ID( 'A', 'X', 'I', 'S' )
																																																																																																																																																																																																	  ,			eID_TAG_BLOK_TMAP			= MAKE_ID( 'T', 'M', 'A', 'P' )
																																																																																																																																																																																																			  //				Texture mapping
																																																																																																																																																																																																			  ,				eID_TAG_BLOK_CNTR		= MAKE_ID( 'C', 'N', 'T', 'R' )
																																																																																																																																																																																																					  ,				eID_TAG_BLOK_SIZE		= MAKE_ID( 'S', 'I', 'Z', 'E' )
																																																																																																																																																																																																							  ,				eID_TAG_BLOK_ROTA		= MAKE_ID( 'R', 'O', 'T', 'A' )
																																																																																																																																																																																																									  ,				eID_TAG_BLOK_OREF		= MAKE_ID( 'O', 'R', 'E', 'F' )
																																																																																																																																																																																																											  ,				eID_TAG_BLOK_FALL		= MAKE_ID( 'F', 'A', 'L', 'L' )
																																																																																																																																																																																																													  ,				eID_TAG_BLOK_CSYS		= MAKE_ID( 'C', 'S', 'Y', 'S' )
																																																																																																																																																																																																															  ,	eID_TAG_BBOX						= MAKE_ID( 'B', 'B', 'O', 'X' )
																																																																																																																																																																																																																	  ,	eID_TAG_DESC						= MAKE_ID( 'D', 'E', 'S', 'C' )
																																																																																																																																																																																																																			  ,	eID_TAG_TEXT						= MAKE_ID( 'T', 'E', 'X', 'T' )
																																																																																																																																																																																																																					  ,	eID_TAG_ICON						= MAKE_ID( 'I', 'C', 'O', 'N' )
	}	eID_TAG;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2012
	\~english
	\brief		Enumeration of texture channels
	\~french
	\brief		Enumération des canaux de texture
	*/
	typedef enum eTEX_CHANNEL
	{
		eTEX_CHANNEL_COLR	= MAKE_ID( 'C', 'O', 'L', 'R' )
							  ,	eTEX_CHANNEL_DIFF	= MAKE_ID( 'D', 'I', 'F', 'F' )
									  ,	eTEX_CHANNEL_LUMI	= MAKE_ID( 'L', 'U', 'M', 'I' )
											  ,	eTEX_CHANNEL_SPEC	= MAKE_ID( 'S', 'P', 'E', 'C' )
													  ,	eTEX_CHANNEL_GLOS	= MAKE_ID( 'G', 'L', 'O', 'S' )
															  ,	eTEX_CHANNEL_REFL	= MAKE_ID( 'R', 'E', 'F', 'L' )
																	  ,	eTEX_CHANNEL_TRAN	= MAKE_ID( 'T', 'R', 'A', 'N' )
																			  ,	eTEX_CHANNEL_RIND	= MAKE_ID( 'R', 'I', 'N', 'D' )
																					  ,	eTEX_CHANNEL_TRNL	= MAKE_ID( 'T', 'R', 'N', 'L' )
																							  ,	eTEX_CHANNEL_BUMP	= MAKE_ID( 'B', 'U', 'M', 'P' )
	}	eTEX_CHANNEL;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2012
	\~english
	\brief		Enumeration of Vertex mapping types
	\~french
	\brief		Enumération des types de vertex mapping
	*/
	typedef enum eVMAP_TYPE
	{
		eVMAP_TYPE_PICK	= MAKE_ID( 'P', 'I', 'C', 'K' )
						  ,	eVMAP_TYPE_WGHT	= MAKE_ID( 'W', 'G', 'H', 'T' )
												,	eVMAP_TYPE_MNVW	= MAKE_ID( 'M', 'N', 'V', 'W' )
														,	eVMAP_TYPE_TXUV	= MAKE_ID( 'T', 'X', 'U', 'V' )
																,	eVMAP_TYPE_RGB	= MAKE_ID( 'R', 'G', 'B', ' ' )
																		,	eVMAP_TYPE_RGBA	= MAKE_ID( 'R', 'G', 'B', 'A' )
																				,	eVMAP_TYPE_MORF	= MAKE_ID( 'M', 'O', 'R', 'F' )
																						,	eVMAP_TYPE_SPOT	= MAKE_ID( 'S', 'P', 'O', 'T' )
	}	eVMAP_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2012
	\~english
	\brief		Enumeration of polygon types
	\~french
	\brief		Enumération des types de polygones
	*/
	typedef enum ePOLS_TYPE
	{
		ePOLS_TYPE_FACE	= MAKE_ID( 'F', 'A', 'C', 'E' )
						  ,	ePOLS_TYPE_CURV	= MAKE_ID( 'C', 'U', 'R', 'V' )
												,	ePOLS_TYPE_PTCH	= MAKE_ID( 'P', 'T', 'C', 'H' )
														,	ePOLS_TYPE_MBAL	= MAKE_ID( 'M', 'B', 'A', 'l' )
																,	ePOLS_TYPE_BONE	= MAKE_ID( 'B', 'O', 'N', 'E' )
	}	ePOLS_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2012
	\~english
	\brief		Enumeration of polygon tag types
	\~french
	\brief		Enumération des types de polygon tag
	*/
	typedef enum ePTAG_TYPE
	{
		ePTAG_TYPE_SURF	= MAKE_ID( 'S', 'U', 'R', 'F' )
						  ,	ePTAG_TYPE_PART	= MAKE_ID( 'P', 'A', 'R', 'T' )
												,	ePTAG_TYPE_SMGP	= MAKE_ID( 'S', 'M', 'G', 'P' )
	}	ePTAG_TYPE;

	struct stLWO_CHUNK;
	struct stLWO_SUBCHUNK;
	class LwoImporter;

	typedef std::map<		UI4,			Castor::ImageSPtr		>	ImageVxMap;
	typedef std::vector< 	UI4										>	Face;
	typedef std::pair< 		std::string,	Castor3D::SubmeshSPtr	>	SubmeshPtrStrPair;
}

#endif
