/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SmaaConfig_H___
#define ___C3D_SmaaConfig_H___

#include <Miscellaneous/Parameter.hpp>

namespace smaa
{
	enum class Mode
	{
		e1X,
		eT2X,
		eS2X,
		e4X
	};

	enum class Preset
	{
		eLow,
		eMedium,
		eHigh,
		eUltra,
		eCustom
	};

	enum class EdgeDetectionType
	{
		eDepth,
		eColour,
		eLuma,
	};

	struct SmaaConfig
	{
		SmaaConfig( castor3d::Parameters const & parameters )
		{
			castor::String value;

			if ( !parameters.get( cuT( "preset" ), value ) )
			{
				value = "high";
			}

			if ( value == cuT( "low" ) )
			{
				data.threshold = 0.15f;
				data.maxSearchSteps = 4;
				data.maxSearchStepsDiag = 0;
				data.cornerRounding = 100;
			}
			else if ( value == cuT( "medium" ) )
			{
				data.threshold = 0.1f;
				data.maxSearchSteps = 8;
				data.maxSearchStepsDiag = 0;
				data.cornerRounding = 100;
			}
			else if ( value == cuT( "high" ) )
			{
				data.threshold = 0.1f;
				data.maxSearchSteps = 16;
				data.maxSearchStepsDiag = 8;
				data.cornerRounding = 25;
			}
			else if ( value == cuT( "ultra" ) )
			{
				data.threshold = 0.05f;
				data.maxSearchSteps = 32;
				data.maxSearchStepsDiag = 16;
				data.cornerRounding = 25;
			}
			else if ( value == cuT( "custom" ) )
			{
				parameters.get( cuT( "threshold" ), data.threshold );
				parameters.get( cuT( "maxSearchSteps" ), data.maxSearchSteps );
				parameters.get( cuT( "maxSearchStepsDiag" ), data.maxSearchStepsDiag );
				parameters.get( cuT( "cornerRounding" ), data.cornerRounding );
			}

			if ( parameters.get( cuT( "mode" ), value ) )
			{
				if ( value == cuT( "T2X" ) )
				{
					data.mode = Mode::eT2X;
				}
				else if ( value == cuT( "S2X" ) )
				{
					data.mode = Mode::eS2X;
				}
				else if ( value == cuT( "4X" ) )
				{
					data.mode = Mode::e4X;
				}
			}

			if ( parameters.get( cuT( "edgeDetection" ), value ) )
			{
				if ( value == cuT( "colour" ) )
				{
					data.edgeDetection = EdgeDetectionType::eColour;
				}
				else if ( value == cuT( "depth" ) )
				{
					data.edgeDetection = EdgeDetectionType::eDepth;
				}
			}

			parameters.get( cuT( "disableDiagonalDetection" ), data.disableDiagonalDetection );
			parameters.get( cuT( "disableCornerDetection" ), data.disableCornerDetection );
			parameters.get( cuT( "localContrastAdaptationFactor" ), data.localContrastAdaptationFactor );
			parameters.get( cuT( "predicationScale" ), data.predicationScale );
			parameters.get( cuT( "predicationStrength" ), data.predicationStrength );

			switch ( data.mode )
			{
			case Mode::e1X:
				subsampleIndices[0] = castor::Point4i{ 0, 0, 0, 0 };
				maxSubsampleIndices = 1u;
				break;

			case Mode::eT2X:
			case Mode::eS2X:
				/***
				* Sample positions (bottom-to-top y axis):
				*   _______
				*  | S1    |  S0:  0.25    -0.25
				*  |       |  S1: -0.25     0.25
				*  |____S0_|
				*/
				subsampleIndices[0] = castor::Point4i{ 1, 1, 1, 0 }; // S0
				subsampleIndices[1] = castor::Point4i{ 2, 2, 2, 0 }; // S1
				maxSubsampleIndices = 2u;
				// (it's 1 for the horizontal slot of S0 because horizontal
				//  blending is reversed: positive numbers point to the right)
				break;

			case Mode::e4X:
				/***
				* Sample positions (bottom-to-top y axis):
				*   ________
				*  |  S1    |  S0:  0.3750   -0.1250
				*  |      S0|  S1: -0.1250    0.3750
				*  |S3      |  S2:  0.1250   -0.3750
				*  |____S2__|  S3: -0.3750    0.1250
				*/
				subsampleIndices[0] = castor::Point4i{ 5, 3, 1, 3 }; // S0
				subsampleIndices[1] = castor::Point4i{ 4, 6, 2, 3 }; // S1
				subsampleIndices[2] = castor::Point4i{ 3, 5, 1, 4 }; // S2
				subsampleIndices[3] = castor::Point4i{ 6, 4, 2, 4 }; // S3
				maxSubsampleIndices = 4u;
				break;
			}

			switch ( data.mode )
			{
			case Mode::e1X:
			case Mode::eS2X:
				jitters.emplace_back( 0.0, 0.0 );
				break;

			case Mode::eT2X:
				jitters.emplace_back( 0.25, -0.25 );
				jitters.emplace_back( -0.25, 0.25 );
				break;

			case Mode::e4X:
				jitters.emplace_back( 0.125, -0.125 );
				jitters.emplace_back( -0.125, 0.125 );
				break;
			}
		}

		struct Data
		{
			Mode mode{ Mode::e1X };
			EdgeDetectionType edgeDetection{ EdgeDetectionType::eLuma };
			bool reprojection{ false };
			bool disableDiagonalDetection{ false };
			bool disableCornerDetection{ false };
			float reprojectionWeightScale{ 30.0f };
			float threshold{ 0.1f };
			int maxSearchSteps{ 16 };
			int maxSearchStepsDiag{ 8 };
			int cornerRounding{ 25 };
			float predicationThreshold{ 0.01f };
			float predicationScale{ 2.0f };
			float predicationStrength{ 0.4f };
			float localContrastAdaptationFactor{ 2.0f };
		};
		Data data;
		uint32_t subsampleIndex{ 0u };
		std::array< castor::Point4i, 4u > subsampleIndices;
		uint32_t maxSubsampleIndices{ 1u };
		std::vector< castor::Point2r > jitters;
	};
}

#endif
