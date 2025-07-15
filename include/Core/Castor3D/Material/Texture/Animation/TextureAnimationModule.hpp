/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialTextureAnimationModule_H___
#define ___C3D_MaterialTextureAnimationModule_H___

#include "Castor3D/Material/Texture/TextureModule.hpp"

#include <CastorUtils/Math/MathModule.hpp>

namespace c3d
{
	/**@name Material */
	//@{
	/**@name Texture */
	//@{
	/**@name Animation */
	//@{

	struct TextureAnimationData
	{
		Point3f translate{};
		float rotateU{};
		Point3f scale{};
		float rotateV{};
		Point4f tileSet{};
	};

	class TextureAnimation;
	class TextureAnimationKeyFrame;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, TextureAnimation, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureAnimationKeyFrame, C3D_API );
	/** @endcond */

	using TextureRotateSpeed = SpeedT< Angle, Seconds >;
	using TextureTranslateSpeed = SpeedT< Point2f, Seconds >;
	using TextureScaleSpeed = SpeedT< Point2f, Seconds >;

	//@}
	//@}
	//@}
}

#endif
