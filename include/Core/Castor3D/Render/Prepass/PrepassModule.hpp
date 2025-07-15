/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PrepassModule_H___
#define ___C3D_PrepassModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace c3d
{
	/**@name Render */
	//@{
	/**@name Prepass */
	//@{

	/**
	*\~english
	*\brief
	*	Enumerator of textures used by the visibility passes.
	*\~french
	*\brief
	*	Enumération des textures utilisées par les passes de visibilité.
	*/
	enum class PpTexture
		: uint8_t
	{
		eDepthObj = 0, // R => Normalised depth, G => Linear depth, B => Node ID, A => Lighting Model ID
		eVisibility = 1, // R => node ID + pipeline ID, G => primitive ID
		CU_ScopedEnumBounds( eDepthObj, eVisibility ),
	};
	C3D_API String getTextureName( PpTexture texture );
	C3D_API String getTexName( PpTexture texture );
	C3D_API PixelFormat getFormat( RenderDevice const & device, PpTexture texture );
	C3D_API ClearValue getClearValue( PpTexture texture );
	C3D_API ImageUsageFlags getUsageFlags( PpTexture texture );
	C3D_API BorderColour getBorderColor( PpTexture texture );
	inline uint32_t getMipLevels( RenderDevice const & /*device*/
		, PpTexture /*texture*/
		, Size const & /*size*/ )
	{
		return 1u;
	}
	inline ComparisonFunc getCompareOp( PpTexture /*texture*/ )
	{
		return ComparisonFunc::eNever;
	}
	/**
	*\~english
	*\brief
	*	Depth prepass.
	*\~french
	*\brief
	*	Classe de prépasse de profondeur.
	*/
	class DepthPass;
	/**
	*\~english
	*\brief
	*	The render technique part dedicated to prepass.
	*\~french
	*\brief
	*	La partie de render technique dédiée à la prépasse.
	*/
	class PrepassRendering;
	/**
	*\~english
	*\brief
	*	The result of the prepass.
	*\~french
	*\brief
	*	Résultat de la pré-passe.
	*/
	class PrepassResult;
	/**
	*\~english
	*\brief
	*	The render nodes pass writing visibility buffer.
	*\~french
	*\brief
	*	La passe de noeuds de rendu générant le buffer de visibilité.
	*/
	class VisibilityPass;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, DepthPass, C3D_API );
	CU_DeclareSmartPtr( c3d, PrepassRendering, C3D_API );
	CU_DeclareSmartPtr( c3d, PrepassResult, C3D_API );
	CU_DeclareSmartPtr( c3d, VisibilityPass, C3D_API );
	/** @endcond */

	//@}
	//@}
}

#endif
