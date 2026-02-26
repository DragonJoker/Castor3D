/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FroxelsConfig_H___
#define ___C3D_FroxelsConfig_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace c3d
{
	enum class FroxelDebugDisplay
	{
		eNone = 0,
		eFroxelsAABB = 1,
		CU_ScopedEnumBounds( eNone, eFroxelsAABB )
	};

	struct FroxelsConfig
	{
		C3D_API FroxelsConfig();

		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( AttributeParsers & result );

		bool dirty{ true };
		//!\~english	Debug display mode.
		//!\~french		Mode d'affichage de debug.
		GroupChangeTracked< FroxelDebugDisplay > debugDisplay;
		//!\~english	The samples counts.
		//!\~french		Les nombres d'échantillons.
		GroupChangeTracked< u32 > sampleCountX;
		GroupChangeTracked< u32 > sampleCountY;
		GroupChangeTracked< u32 > sampleCountMinZ;
		GroupChangeTracked< u32 > sampleCountMaxZ;
		//!\~english	The size of the blur filter.
		//!\~french		La taille du filtre de flou.
		GroupChangeTracked< RangedValue< u32 > > blurFilterSize;

	private:
		friend bool operator==( FroxelsConfig const & lhs, FroxelsConfig const & rhs )noexcept
		{
			return lhs.blurFilterSize == rhs.blurFilterSize
				&& lhs.sampleCountX == rhs.sampleCountX
				&& lhs.sampleCountY == rhs.sampleCountY
				&& lhs.sampleCountMinZ == rhs.sampleCountMinZ
				&& lhs.sampleCountMaxZ == rhs.sampleCountMaxZ;
		}
	};
}

#endif
