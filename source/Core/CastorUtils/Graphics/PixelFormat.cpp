#include "CastorUtils/Graphics/PixelFormat.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

namespace castor
{
	namespace PF
	{
		namespace
		{
			inline int8_t doSignInt32( int v )
			{
				return ( int8_t )( ( v >> 31 ) | -( -v >> 31 ) );
			}

			inline uint32_t doDivide0To767By3( uint32_t value )
			{
				static uint8_t const divisionTable[768]
				{
					0, 0, 0, 1, 1, 1, 2, 2,
					2, 3, 3, 3, 4, 4, 4, 5,
					5, 5, 6, 6, 6, 7, 7, 7,
					8, 8, 8, 9, 9, 9, 10, 10,
					10, 11, 11, 11, 12, 12, 12, 13,
					13, 13, 14, 14, 14, 15, 15, 15,
					16, 16, 16, 17, 17, 17, 18, 18,
					18, 19, 19, 19, 20, 20, 20, 21,
					21, 21, 22, 22, 22, 23, 23, 23,
					24, 24, 24, 25, 25, 25, 26, 26,
					26, 27, 27, 27, 28, 28, 28, 29,
					29, 29, 30, 30, 30, 31, 31, 31,
					32, 32, 32, 33, 33, 33, 34, 34,
					34, 35, 35, 35, 36, 36, 36, 37,
					37, 37, 38, 38, 38, 39, 39, 39,
					40, 40, 40, 41, 41, 41, 42, 42,
					42, 43, 43, 43, 44, 44, 44, 45,
					45, 45, 46, 46, 46, 47, 47, 47,
					48, 48, 48, 49, 49, 49, 50, 50,
					50, 51, 51, 51, 52, 52, 52, 53,
					53, 53, 54, 54, 54, 55, 55, 55,
					56, 56, 56, 57, 57, 57, 58, 58,
					58, 59, 59, 59, 60, 60, 60, 61,
					61, 61, 62, 62, 62, 63, 63, 63,
					64, 64, 64, 65, 65, 65, 66, 66,
					66, 67, 67, 67, 68, 68, 68, 69,
					69, 69, 70, 70, 70, 71, 71, 71,
					72, 72, 72, 73, 73, 73, 74, 74,
					74, 75, 75, 75, 76, 76, 76, 77,
					77, 77, 78, 78, 78, 79, 79, 79,
					80, 80, 80, 81, 81, 81, 82, 82,
					82, 83, 83, 83, 84, 84, 84, 85,
					85, 85, 86, 86, 86, 87, 87, 87,
					88, 88, 88, 89, 89, 89, 90, 90,
					90, 91, 91, 91, 92, 92, 92, 93,
					93, 93, 94, 94, 94, 95, 95, 95,
					96, 96, 96, 97, 97, 97, 98, 98,
					98, 99, 99, 99, 100, 100, 100, 101,
					101, 101, 102, 102, 102, 103, 103, 103,
					104, 104, 104, 105, 105, 105, 106, 106,
					106, 107, 107, 107, 108, 108, 108, 109,
					109, 109, 110, 110, 110, 111, 111, 111,
					112, 112, 112, 113, 113, 113, 114, 114,
					114, 115, 115, 115, 116, 116, 116, 117,
					117, 117, 118, 118, 118, 119, 119, 119,
					120, 120, 120, 121, 121, 121, 122, 122,
					122, 123, 123, 123, 124, 124, 124, 125,
					125, 125, 126, 126, 126, 127, 127, 127,
					128, 128, 128, 129, 129, 129, 130, 130,
					130, 131, 131, 131, 132, 132, 132, 133,
					133, 133, 134, 134, 134, 135, 135, 135,
					136, 136, 136, 137, 137, 137, 138, 138,
					138, 139, 139, 139, 140, 140, 140, 141,
					141, 141, 142, 142, 142, 143, 143, 143,
					144, 144, 144, 145, 145, 145, 146, 146,
					146, 147, 147, 147, 148, 148, 148, 149,
					149, 149, 150, 150, 150, 151, 151, 151,
					152, 152, 152, 153, 153, 153, 154, 154,
					154, 155, 155, 155, 156, 156, 156, 157,
					157, 157, 158, 158, 158, 159, 159, 159,
					160, 160, 160, 161, 161, 161, 162, 162,
					162, 163, 163, 163, 164, 164, 164, 165,
					165, 165, 166, 166, 166, 167, 167, 167,
					168, 168, 168, 169, 169, 169, 170, 170,
					170, 171, 171, 171, 172, 172, 172, 173,
					173, 173, 174, 174, 174, 175, 175, 175,
					176, 176, 176, 177, 177, 177, 178, 178,
					178, 179, 179, 179, 180, 180, 180, 181,
					181, 181, 182, 182, 182, 183, 183, 183,
					184, 184, 184, 185, 185, 185, 186, 186,
					186, 187, 187, 187, 188, 188, 188, 189,
					189, 189, 190, 190, 190, 191, 191, 191,
					192, 192, 192, 193, 193, 193, 194, 194,
					194, 195, 195, 195, 196, 196, 196, 197,
					197, 197, 198, 198, 198, 199, 199, 199,
					200, 200, 200, 201, 201, 201, 202, 202,
					202, 203, 203, 203, 204, 204, 204, 205,
					205, 205, 206, 206, 206, 207, 207, 207,
					208, 208, 208, 209, 209, 209, 210, 210,
					210, 211, 211, 211, 212, 212, 212, 213,
					213, 213, 214, 214, 214, 215, 215, 215,
					216, 216, 216, 217, 217, 217, 218, 218,
					218, 219, 219, 219, 220, 220, 220, 221,
					221, 221, 222, 222, 222, 223, 223, 223,
					224, 224, 224, 225, 225, 225, 226, 226,
					226, 227, 227, 227, 228, 228, 228, 229,
					229, 229, 230, 230, 230, 231, 231, 231,
					232, 232, 232, 233, 233, 233, 234, 234,
					234, 235, 235, 235, 236, 236, 236, 237,
					237, 237, 238, 238, 238, 239, 239, 239,
					240, 240, 240, 241, 241, 241, 242, 242,
					242, 243, 243, 243, 244, 244, 244, 245,
					245, 245, 246, 246, 246, 247, 247, 247,
					248, 248, 248, 249, 249, 249, 250, 250,
					250, 251, 251, 251, 252, 252, 252, 253,
					253, 253, 254, 254, 254, 255, 255, 255,
				};
				return divisionTable[value];
			}

			inline uint32_t doDivide0To1791By7( uint32_t value )
			{
				static uint8_t const divisionTable[1792] = {
					0, 0, 0, 0, 0, 0, 0, 1,
					1, 1, 1, 1, 1, 1, 2, 2,
					2, 2, 2, 2, 2, 3, 3, 3,
					3, 3, 3, 3, 4, 4, 4, 4,
					4, 4, 4, 5, 5, 5, 5, 5,
					5, 5, 6, 6, 6, 6, 6, 6,
					6, 7, 7, 7, 7, 7, 7, 7,
					8, 8, 8, 8, 8, 8, 8, 9,
					9, 9, 9, 9, 9, 9, 10, 10,
					10, 10, 10, 10, 10, 11, 11, 11,
					11, 11, 11, 11, 12, 12, 12, 12,
					12, 12, 12, 13, 13, 13, 13, 13,
					13, 13, 14, 14, 14, 14, 14, 14,
					14, 15, 15, 15, 15, 15, 15, 15,
					16, 16, 16, 16, 16, 16, 16, 17,
					17, 17, 17, 17, 17, 17, 18, 18,
					18, 18, 18, 18, 18, 19, 19, 19,
					19, 19, 19, 19, 20, 20, 20, 20,
					20, 20, 20, 21, 21, 21, 21, 21,
					21, 21, 22, 22, 22, 22, 22, 22,
					22, 23, 23, 23, 23, 23, 23, 23,
					24, 24, 24, 24, 24, 24, 24, 25,
					25, 25, 25, 25, 25, 25, 26, 26,
					26, 26, 26, 26, 26, 27, 27, 27,
					27, 27, 27, 27, 28, 28, 28, 28,
					28, 28, 28, 29, 29, 29, 29, 29,
					29, 29, 30, 30, 30, 30, 30, 30,
					30, 31, 31, 31, 31, 31, 31, 31,
					32, 32, 32, 32, 32, 32, 32, 33,
					33, 33, 33, 33, 33, 33, 34, 34,
					34, 34, 34, 34, 34, 35, 35, 35,
					35, 35, 35, 35, 36, 36, 36, 36,
					36, 36, 36, 37, 37, 37, 37, 37,
					37, 37, 38, 38, 38, 38, 38, 38,
					38, 39, 39, 39, 39, 39, 39, 39,
					40, 40, 40, 40, 40, 40, 40, 41,
					41, 41, 41, 41, 41, 41, 42, 42,
					42, 42, 42, 42, 42, 43, 43, 43,
					43, 43, 43, 43, 44, 44, 44, 44,
					44, 44, 44, 45, 45, 45, 45, 45,
					45, 45, 46, 46, 46, 46, 46, 46,
					46, 47, 47, 47, 47, 47, 47, 47,
					48, 48, 48, 48, 48, 48, 48, 49,
					49, 49, 49, 49, 49, 49, 50, 50,
					50, 50, 50, 50, 50, 51, 51, 51,
					51, 51, 51, 51, 52, 52, 52, 52,
					52, 52, 52, 53, 53, 53, 53, 53,
					53, 53, 54, 54, 54, 54, 54, 54,
					54, 55, 55, 55, 55, 55, 55, 55,
					56, 56, 56, 56, 56, 56, 56, 57,
					57, 57, 57, 57, 57, 57, 58, 58,
					58, 58, 58, 58, 58, 59, 59, 59,
					59, 59, 59, 59, 60, 60, 60, 60,
					60, 60, 60, 61, 61, 61, 61, 61,
					61, 61, 62, 62, 62, 62, 62, 62,
					62, 63, 63, 63, 63, 63, 63, 63,
					64, 64, 64, 64, 64, 64, 64, 65,
					65, 65, 65, 65, 65, 65, 66, 66,
					66, 66, 66, 66, 66, 67, 67, 67,
					67, 67, 67, 67, 68, 68, 68, 68,
					68, 68, 68, 69, 69, 69, 69, 69,
					69, 69, 70, 70, 70, 70, 70, 70,
					70, 71, 71, 71, 71, 71, 71, 71,
					72, 72, 72, 72, 72, 72, 72, 73,
					73, 73, 73, 73, 73, 73, 74, 74,
					74, 74, 74, 74, 74, 75, 75, 75,
					75, 75, 75, 75, 76, 76, 76, 76,
					76, 76, 76, 77, 77, 77, 77, 77,
					77, 77, 78, 78, 78, 78, 78, 78,
					78, 79, 79, 79, 79, 79, 79, 79,
					80, 80, 80, 80, 80, 80, 80, 81,
					81, 81, 81, 81, 81, 81, 82, 82,
					82, 82, 82, 82, 82, 83, 83, 83,
					83, 83, 83, 83, 84, 84, 84, 84,
					84, 84, 84, 85, 85, 85, 85, 85,
					85, 85, 86, 86, 86, 86, 86, 86,
					86, 87, 87, 87, 87, 87, 87, 87,
					88, 88, 88, 88, 88, 88, 88, 89,
					89, 89, 89, 89, 89, 89, 90, 90,
					90, 90, 90, 90, 90, 91, 91, 91,
					91, 91, 91, 91, 92, 92, 92, 92,
					92, 92, 92, 93, 93, 93, 93, 93,
					93, 93, 94, 94, 94, 94, 94, 94,
					94, 95, 95, 95, 95, 95, 95, 95,
					96, 96, 96, 96, 96, 96, 96, 97,
					97, 97, 97, 97, 97, 97, 98, 98,
					98, 98, 98, 98, 98, 99, 99, 99,
					99, 99, 99, 99, 100, 100, 100, 100,
					100, 100, 100, 101, 101, 101, 101, 101,
					101, 101, 102, 102, 102, 102, 102, 102,
					102, 103, 103, 103, 103, 103, 103, 103,
					104, 104, 104, 104, 104, 104, 104, 105,
					105, 105, 105, 105, 105, 105, 106, 106,
					106, 106, 106, 106, 106, 107, 107, 107,
					107, 107, 107, 107, 108, 108, 108, 108,
					108, 108, 108, 109, 109, 109, 109, 109,
					109, 109, 110, 110, 110, 110, 110, 110,
					110, 111, 111, 111, 111, 111, 111, 111,
					112, 112, 112, 112, 112, 112, 112, 113,
					113, 113, 113, 113, 113, 113, 114, 114,
					114, 114, 114, 114, 114, 115, 115, 115,
					115, 115, 115, 115, 116, 116, 116, 116,
					116, 116, 116, 117, 117, 117, 117, 117,
					117, 117, 118, 118, 118, 118, 118, 118,
					118, 119, 119, 119, 119, 119, 119, 119,
					120, 120, 120, 120, 120, 120, 120, 121,
					121, 121, 121, 121, 121, 121, 122, 122,
					122, 122, 122, 122, 122, 123, 123, 123,
					123, 123, 123, 123, 124, 124, 124, 124,
					124, 124, 124, 125, 125, 125, 125, 125,
					125, 125, 126, 126, 126, 126, 126, 126,
					126, 127, 127, 127, 127, 127, 127, 127,
					128, 128, 128, 128, 128, 128, 128, 129,
					129, 129, 129, 129, 129, 129, 130, 130,
					130, 130, 130, 130, 130, 131, 131, 131,
					131, 131, 131, 131, 132, 132, 132, 132,
					132, 132, 132, 133, 133, 133, 133, 133,
					133, 133, 134, 134, 134, 134, 134, 134,
					134, 135, 135, 135, 135, 135, 135, 135,
					136, 136, 136, 136, 136, 136, 136, 137,
					137, 137, 137, 137, 137, 137, 138, 138,
					138, 138, 138, 138, 138, 139, 139, 139,
					139, 139, 139, 139, 140, 140, 140, 140,
					140, 140, 140, 141, 141, 141, 141, 141,
					141, 141, 142, 142, 142, 142, 142, 142,
					142, 143, 143, 143, 143, 143, 143, 143,
					144, 144, 144, 144, 144, 144, 144, 145,
					145, 145, 145, 145, 145, 145, 146, 146,
					146, 146, 146, 146, 146, 147, 147, 147,
					147, 147, 147, 147, 148, 148, 148, 148,
					148, 148, 148, 149, 149, 149, 149, 149,
					149, 149, 150, 150, 150, 150, 150, 150,
					150, 151, 151, 151, 151, 151, 151, 151,
					152, 152, 152, 152, 152, 152, 152, 153,
					153, 153, 153, 153, 153, 153, 154, 154,
					154, 154, 154, 154, 154, 155, 155, 155,
					155, 155, 155, 155, 156, 156, 156, 156,
					156, 156, 156, 157, 157, 157, 157, 157,
					157, 157, 158, 158, 158, 158, 158, 158,
					158, 159, 159, 159, 159, 159, 159, 159,
					160, 160, 160, 160, 160, 160, 160, 161,
					161, 161, 161, 161, 161, 161, 162, 162,
					162, 162, 162, 162, 162, 163, 163, 163,
					163, 163, 163, 163, 164, 164, 164, 164,
					164, 164, 164, 165, 165, 165, 165, 165,
					165, 165, 166, 166, 166, 166, 166, 166,
					166, 167, 167, 167, 167, 167, 167, 167,
					168, 168, 168, 168, 168, 168, 168, 169,
					169, 169, 169, 169, 169, 169, 170, 170,
					170, 170, 170, 170, 170, 171, 171, 171,
					171, 171, 171, 171, 172, 172, 172, 172,
					172, 172, 172, 173, 173, 173, 173, 173,
					173, 173, 174, 174, 174, 174, 174, 174,
					174, 175, 175, 175, 175, 175, 175, 175,
					176, 176, 176, 176, 176, 176, 176, 177,
					177, 177, 177, 177, 177, 177, 178, 178,
					178, 178, 178, 178, 178, 179, 179, 179,
					179, 179, 179, 179, 180, 180, 180, 180,
					180, 180, 180, 181, 181, 181, 181, 181,
					181, 181, 182, 182, 182, 182, 182, 182,
					182, 183, 183, 183, 183, 183, 183, 183,
					184, 184, 184, 184, 184, 184, 184, 185,
					185, 185, 185, 185, 185, 185, 186, 186,
					186, 186, 186, 186, 186, 187, 187, 187,
					187, 187, 187, 187, 188, 188, 188, 188,
					188, 188, 188, 189, 189, 189, 189, 189,
					189, 189, 190, 190, 190, 190, 190, 190,
					190, 191, 191, 191, 191, 191, 191, 191,
					192, 192, 192, 192, 192, 192, 192, 193,
					193, 193, 193, 193, 193, 193, 194, 194,
					194, 194, 194, 194, 194, 195, 195, 195,
					195, 195, 195, 195, 196, 196, 196, 196,
					196, 196, 196, 197, 197, 197, 197, 197,
					197, 197, 198, 198, 198, 198, 198, 198,
					198, 199, 199, 199, 199, 199, 199, 199,
					200, 200, 200, 200, 200, 200, 200, 201,
					201, 201, 201, 201, 201, 201, 202, 202,
					202, 202, 202, 202, 202, 203, 203, 203,
					203, 203, 203, 203, 204, 204, 204, 204,
					204, 204, 204, 205, 205, 205, 205, 205,
					205, 205, 206, 206, 206, 206, 206, 206,
					206, 207, 207, 207, 207, 207, 207, 207,
					208, 208, 208, 208, 208, 208, 208, 209,
					209, 209, 209, 209, 209, 209, 210, 210,
					210, 210, 210, 210, 210, 211, 211, 211,
					211, 211, 211, 211, 212, 212, 212, 212,
					212, 212, 212, 213, 213, 213, 213, 213,
					213, 213, 214, 214, 214, 214, 214, 214,
					214, 215, 215, 215, 215, 215, 215, 215,
					216, 216, 216, 216, 216, 216, 216, 217,
					217, 217, 217, 217, 217, 217, 218, 218,
					218, 218, 218, 218, 218, 219, 219, 219,
					219, 219, 219, 219, 220, 220, 220, 220,
					220, 220, 220, 221, 221, 221, 221, 221,
					221, 221, 222, 222, 222, 222, 222, 222,
					222, 223, 223, 223, 223, 223, 223, 223,
					224, 224, 224, 224, 224, 224, 224, 225,
					225, 225, 225, 225, 225, 225, 226, 226,
					226, 226, 226, 226, 226, 227, 227, 227,
					227, 227, 227, 227, 228, 228, 228, 228,
					228, 228, 228, 229, 229, 229, 229, 229,
					229, 229, 230, 230, 230, 230, 230, 230,
					230, 231, 231, 231, 231, 231, 231, 231,
					232, 232, 232, 232, 232, 232, 232, 233,
					233, 233, 233, 233, 233, 233, 234, 234,
					234, 234, 234, 234, 234, 235, 235, 235,
					235, 235, 235, 235, 236, 236, 236, 236,
					236, 236, 236, 237, 237, 237, 237, 237,
					237, 237, 238, 238, 238, 238, 238, 238,
					238, 239, 239, 239, 239, 239, 239, 239,
					240, 240, 240, 240, 240, 240, 240, 241,
					241, 241, 241, 241, 241, 241, 242, 242,
					242, 242, 242, 242, 242, 243, 243, 243,
					243, 243, 243, 243, 244, 244, 244, 244,
					244, 244, 244, 245, 245, 245, 245, 245,
					245, 245, 246, 246, 246, 246, 246, 246,
					246, 247, 247, 247, 247, 247, 247, 247,
					248, 248, 248, 248, 248, 248, 248, 249,
					249, 249, 249, 249, 249, 249, 250, 250,
					250, 250, 250, 250, 250, 251, 251, 251,
					251, 251, 251, 251, 252, 252, 252, 252,
					252, 252, 252, 253, 253, 253, 253, 253,
					253, 253, 254, 254, 254, 254, 254, 254,
					254, 255, 255, 255, 255, 255, 255, 255,
				};
				return divisionTable[value];
			}

			inline int doDivideMinus895To895By7( int value )
			{
				return ( int8_t )doDivide0To1791By7( abs( value ) ) * doSignInt32( value );
			}

			inline uint32_t doDivide0To1279By5( uint32_t value )
			{
				static uint8_t const divisionTable[1280] = {
					0, 0, 0, 0, 0, 1, 1, 1,
					1, 1, 2, 2, 2, 2, 2, 3,
					3, 3, 3, 3, 4, 4, 4, 4,
					4, 5, 5, 5, 5, 5, 6, 6,
					6, 6, 6, 7, 7, 7, 7, 7,
					8, 8, 8, 8, 8, 9, 9, 9,
					9, 9, 10, 10, 10, 10, 10, 11,
					11, 11, 11, 11, 12, 12, 12, 12,
					12, 13, 13, 13, 13, 13, 14, 14,
					14, 14, 14, 15, 15, 15, 15, 15,
					16, 16, 16, 16, 16, 17, 17, 17,
					17, 17, 18, 18, 18, 18, 18, 19,
					19, 19, 19, 19, 20, 20, 20, 20,
					20, 21, 21, 21, 21, 21, 22, 22,
					22, 22, 22, 23, 23, 23, 23, 23,
					24, 24, 24, 24, 24, 25, 25, 25,
					25, 25, 26, 26, 26, 26, 26, 27,
					27, 27, 27, 27, 28, 28, 28, 28,
					28, 29, 29, 29, 29, 29, 30, 30,
					30, 30, 30, 31, 31, 31, 31, 31,
					32, 32, 32, 32, 32, 33, 33, 33,
					33, 33, 34, 34, 34, 34, 34, 35,
					35, 35, 35, 35, 36, 36, 36, 36,
					36, 37, 37, 37, 37, 37, 38, 38,
					38, 38, 38, 39, 39, 39, 39, 39,
					40, 40, 40, 40, 40, 41, 41, 41,
					41, 41, 42, 42, 42, 42, 42, 43,
					43, 43, 43, 43, 44, 44, 44, 44,
					44, 45, 45, 45, 45, 45, 46, 46,
					46, 46, 46, 47, 47, 47, 47, 47,
					48, 48, 48, 48, 48, 49, 49, 49,
					49, 49, 50, 50, 50, 50, 50, 51,
					51, 51, 51, 51, 52, 52, 52, 52,
					52, 53, 53, 53, 53, 53, 54, 54,
					54, 54, 54, 55, 55, 55, 55, 55,
					56, 56, 56, 56, 56, 57, 57, 57,
					57, 57, 58, 58, 58, 58, 58, 59,
					59, 59, 59, 59, 60, 60, 60, 60,
					60, 61, 61, 61, 61, 61, 62, 62,
					62, 62, 62, 63, 63, 63, 63, 63,
					64, 64, 64, 64, 64, 65, 65, 65,
					65, 65, 66, 66, 66, 66, 66, 67,
					67, 67, 67, 67, 68, 68, 68, 68,
					68, 69, 69, 69, 69, 69, 70, 70,
					70, 70, 70, 71, 71, 71, 71, 71,
					72, 72, 72, 72, 72, 73, 73, 73,
					73, 73, 74, 74, 74, 74, 74, 75,
					75, 75, 75, 75, 76, 76, 76, 76,
					76, 77, 77, 77, 77, 77, 78, 78,
					78, 78, 78, 79, 79, 79, 79, 79,
					80, 80, 80, 80, 80, 81, 81, 81,
					81, 81, 82, 82, 82, 82, 82, 83,
					83, 83, 83, 83, 84, 84, 84, 84,
					84, 85, 85, 85, 85, 85, 86, 86,
					86, 86, 86, 87, 87, 87, 87, 87,
					88, 88, 88, 88, 88, 89, 89, 89,
					89, 89, 90, 90, 90, 90, 90, 91,
					91, 91, 91, 91, 92, 92, 92, 92,
					92, 93, 93, 93, 93, 93, 94, 94,
					94, 94, 94, 95, 95, 95, 95, 95,
					96, 96, 96, 96, 96, 97, 97, 97,
					97, 97, 98, 98, 98, 98, 98, 99,
					99, 99, 99, 99, 100, 100, 100, 100,
					100, 101, 101, 101, 101, 101, 102, 102,
					102, 102, 102, 103, 103, 103, 103, 103,
					104, 104, 104, 104, 104, 105, 105, 105,
					105, 105, 106, 106, 106, 106, 106, 107,
					107, 107, 107, 107, 108, 108, 108, 108,
					108, 109, 109, 109, 109, 109, 110, 110,
					110, 110, 110, 111, 111, 111, 111, 111,
					112, 112, 112, 112, 112, 113, 113, 113,
					113, 113, 114, 114, 114, 114, 114, 115,
					115, 115, 115, 115, 116, 116, 116, 116,
					116, 117, 117, 117, 117, 117, 118, 118,
					118, 118, 118, 119, 119, 119, 119, 119,
					120, 120, 120, 120, 120, 121, 121, 121,
					121, 121, 122, 122, 122, 122, 122, 123,
					123, 123, 123, 123, 124, 124, 124, 124,
					124, 125, 125, 125, 125, 125, 126, 126,
					126, 126, 126, 127, 127, 127, 127, 127,
					128, 128, 128, 128, 128, 129, 129, 129,
					129, 129, 130, 130, 130, 130, 130, 131,
					131, 131, 131, 131, 132, 132, 132, 132,
					132, 133, 133, 133, 133, 133, 134, 134,
					134, 134, 134, 135, 135, 135, 135, 135,
					136, 136, 136, 136, 136, 137, 137, 137,
					137, 137, 138, 138, 138, 138, 138, 139,
					139, 139, 139, 139, 140, 140, 140, 140,
					140, 141, 141, 141, 141, 141, 142, 142,
					142, 142, 142, 143, 143, 143, 143, 143,
					144, 144, 144, 144, 144, 145, 145, 145,
					145, 145, 146, 146, 146, 146, 146, 147,
					147, 147, 147, 147, 148, 148, 148, 148,
					148, 149, 149, 149, 149, 149, 150, 150,
					150, 150, 150, 151, 151, 151, 151, 151,
					152, 152, 152, 152, 152, 153, 153, 153,
					153, 153, 154, 154, 154, 154, 154, 155,
					155, 155, 155, 155, 156, 156, 156, 156,
					156, 157, 157, 157, 157, 157, 158, 158,
					158, 158, 158, 159, 159, 159, 159, 159,
					160, 160, 160, 160, 160, 161, 161, 161,
					161, 161, 162, 162, 162, 162, 162, 163,
					163, 163, 163, 163, 164, 164, 164, 164,
					164, 165, 165, 165, 165, 165, 166, 166,
					166, 166, 166, 167, 167, 167, 167, 167,
					168, 168, 168, 168, 168, 169, 169, 169,
					169, 169, 170, 170, 170, 170, 170, 171,
					171, 171, 171, 171, 172, 172, 172, 172,
					172, 173, 173, 173, 173, 173, 174, 174,
					174, 174, 174, 175, 175, 175, 175, 175,
					176, 176, 176, 176, 176, 177, 177, 177,
					177, 177, 178, 178, 178, 178, 178, 179,
					179, 179, 179, 179, 180, 180, 180, 180,
					180, 181, 181, 181, 181, 181, 182, 182,
					182, 182, 182, 183, 183, 183, 183, 183,
					184, 184, 184, 184, 184, 185, 185, 185,
					185, 185, 186, 186, 186, 186, 186, 187,
					187, 187, 187, 187, 188, 188, 188, 188,
					188, 189, 189, 189, 189, 189, 190, 190,
					190, 190, 190, 191, 191, 191, 191, 191,
					192, 192, 192, 192, 192, 193, 193, 193,
					193, 193, 194, 194, 194, 194, 194, 195,
					195, 195, 195, 195, 196, 196, 196, 196,
					196, 197, 197, 197, 197, 197, 198, 198,
					198, 198, 198, 199, 199, 199, 199, 199,
					200, 200, 200, 200, 200, 201, 201, 201,
					201, 201, 202, 202, 202, 202, 202, 203,
					203, 203, 203, 203, 204, 204, 204, 204,
					204, 205, 205, 205, 205, 205, 206, 206,
					206, 206, 206, 207, 207, 207, 207, 207,
					208, 208, 208, 208, 208, 209, 209, 209,
					209, 209, 210, 210, 210, 210, 210, 211,
					211, 211, 211, 211, 212, 212, 212, 212,
					212, 213, 213, 213, 213, 213, 214, 214,
					214, 214, 214, 215, 215, 215, 215, 215,
					216, 216, 216, 216, 216, 217, 217, 217,
					217, 217, 218, 218, 218, 218, 218, 219,
					219, 219, 219, 219, 220, 220, 220, 220,
					220, 221, 221, 221, 221, 221, 222, 222,
					222, 222, 222, 223, 223, 223, 223, 223,
					224, 224, 224, 224, 224, 225, 225, 225,
					225, 225, 226, 226, 226, 226, 226, 227,
					227, 227, 227, 227, 228, 228, 228, 228,
					228, 229, 229, 229, 229, 229, 230, 230,
					230, 230, 230, 231, 231, 231, 231, 231,
					232, 232, 232, 232, 232, 233, 233, 233,
					233, 233, 234, 234, 234, 234, 234, 235,
					235, 235, 235, 235, 236, 236, 236, 236,
					236, 237, 237, 237, 237, 237, 238, 238,
					238, 238, 238, 239, 239, 239, 239, 239,
					240, 240, 240, 240, 240, 241, 241, 241,
					241, 241, 242, 242, 242, 242, 242, 243,
					243, 243, 243, 243, 244, 244, 244, 244,
					244, 245, 245, 245, 245, 245, 246, 246,
					246, 246, 246, 247, 247, 247, 247, 247,
					248, 248, 248, 248, 248, 249, 249, 249,
					249, 249, 250, 250, 250, 250, 250, 251,
					251, 251, 251, 251, 252, 252, 252, 252,
					252, 253, 253, 253, 253, 253, 254, 254,
					254, 254, 254, 255, 255, 255, 255, 255,
				};
				return divisionTable[value];
			}

			inline int doDivideMinus639To639By5( int value )
			{
				return ( int8_t )doDivide0To1279By5( abs( value ) ) * doSignInt32( value );
			}

			uint32_t doPack32( uint32_t r, uint32_t g, uint32_t b, uint32_t a )
			{
				return uint32_t( uint8_t( r ) << 0 )
					| uint32_t( uint8_t( g ) << 8 )
					| uint32_t( uint8_t( g ) << 16 )
					| uint32_t( uint8_t( a ) << 24 );
			}

			bool doDecompressBC1Block( uint8_t const * bitstring
				, uint8_t * pixelBuffer )
			{
				uint32_t colors = *( uint32_t * )&bitstring[0];

				// Decode the two 5-6-5 RGB colors.
				int color_r[4], color_g[4], color_b[4];
				color_b[0] = ( colors & 0x0000001F ) << 3;
				color_g[0] = ( colors & 0x000007E0 ) >> ( 5 - 2 );
				color_r[0] = ( colors & 0x0000F800 ) >> ( 11 - 3 );
				color_b[1] = ( colors & 0x001F0000 ) >> ( 16 - 3 );
				color_g[1] = ( colors & 0x07E00000 ) >> ( 21 - 2 );
				color_r[1] = ( colors & 0xF8000000 ) >> ( 27 - 3 );

				if ( ( colors & 0xFFFF ) > ( ( colors & 0xFFFF0000 ) >> 16 ) )
				{
					color_r[2] = doDivide0To767By3( 2 * color_r[0] + color_r[1] );
					color_g[2] = doDivide0To767By3( 2 * color_g[0] + color_g[1] );
					color_b[2] = doDivide0To767By3( 2 * color_b[0] + color_b[1] );
					color_r[3] = doDivide0To767By3( color_r[0] + 2 * color_r[1] );
					color_g[3] = doDivide0To767By3( color_g[0] + 2 * color_g[1] );
					color_b[3] = doDivide0To767By3( color_b[0] + 2 * color_b[1] );
				}
				else
				{
					color_r[2] = ( color_r[0] + color_r[1] ) / 2;
					color_g[2] = ( color_g[0] + color_g[1] ) / 2;
					color_b[2] = ( color_b[0] + color_b[1] ) / 2;
					color_r[3] = color_g[3] = color_b[3] = 0;
				}

				uint32_t pixels = *( uint32_t * )&bitstring[4];

				for ( int i = 0; i < 16; i++ )
				{
					int pixel = ( pixels >> ( i * 2 ) ) & 0x3;
					*( uint32_t * )( pixelBuffer + i * 4u ) = doPack32( color_r[pixel]
						, color_g[pixel]
						, color_b[pixel]
						, 0xFF );
				}

				return true;
			}

			bool doDecompressBC3Block( uint8_t const * bitstring
				, uint8_t * pixelBuffer )
			{
				int alpha0 = bitstring[0];
				int alpha1 = bitstring[1];
				uint32_t colors = *( uint32_t * )&bitstring[8];

				int color_r[4], color_g[4], color_b[4];
				// color_x[] has a value between 0 and 248 with the lower three bits zero.
				color_b[0] = ( colors & 0x0000001F ) << 3;
				color_g[0] = ( colors & 0x000007E0 ) >> ( 5 - 2 );
				color_r[0] = ( colors & 0x0000F800 ) >> ( 11 - 3 );
				color_b[1] = ( colors & 0x001F0000 ) >> ( 16 - 3 );
				color_g[1] = ( colors & 0x07E00000 ) >> ( 21 - 2 );
				color_r[1] = ( colors & 0xF8000000 ) >> ( 27 - 3 );
				color_r[2] = doDivide0To767By3( 2 * color_r[0] + color_r[1] );
				color_g[2] = doDivide0To767By3( 2 * color_g[0] + color_g[1] );
				color_b[2] = doDivide0To767By3( 2 * color_b[0] + color_b[1] );
				color_r[3] = doDivide0To767By3( color_r[0] + 2 * color_r[1] );
				color_g[3] = doDivide0To767By3( color_g[0] + 2 * color_g[1] );
				color_b[3] = doDivide0To767By3( color_b[0] + 2 * color_b[1] );
				uint32_t pixels = *( uint32_t * )&bitstring[12];
				uint64_t alpha_bits = ( uint32_t )bitstring[2] |
					( ( uint32_t )bitstring[3] << 8 ) |
					( ( uint64_t )*( uint32_t * )&bitstring[4] << 16 );

				for ( int i = 0; i < 16; i++ )
				{
					int pixel = ( pixels >> ( i * 2 ) ) & 0x3;
					int code = ( alpha_bits >> ( i * 3 ) ) & 0x7;
					int alpha;
					if ( alpha0 > alpha1 )
					{
						switch ( code )
						{
						case 0: alpha = alpha0; break;
						case 1: alpha = alpha1; break;
						case 2: alpha = doDivide0To1791By7( 6 * alpha0 + 1 * alpha1 ); break;
						case 3: alpha = doDivide0To1791By7( 5 * alpha0 + 2 * alpha1 ); break;
						case 4: alpha = doDivide0To1791By7( 4 * alpha0 + 3 * alpha1 ); break;
						case 5: alpha = doDivide0To1791By7( 3 * alpha0 + 4 * alpha1 ); break;
						case 6: alpha = doDivide0To1791By7( 2 * alpha0 + 5 * alpha1 ); break;
						case 7: alpha = doDivide0To1791By7( 1 * alpha0 + 6 * alpha1 ); break;
						}
					}
					else
					{
						switch ( code )
						{
						case 0: alpha = alpha0; break;
						case 1: alpha = alpha1; break;
						case 2: alpha = doDivide0To1279By5( 4 * alpha0 + 1 * alpha1 ); break;
						case 3: alpha = doDivide0To1279By5( 3 * alpha0 + 2 * alpha1 ); break;
						case 4: alpha = doDivide0To1279By5( 2 * alpha0 + 3 * alpha1 ); break;
						case 5: alpha = doDivide0To1279By5( 1 * alpha0 + 4 * alpha1 ); break;
						case 6: alpha = 0; break;
						case 7: alpha = 0xFF; break;
						}
					}

					*( uint32_t * )( pixelBuffer + i * 4 ) = doPack32( color_r[pixel]
						, color_g[pixel]
						, color_b[pixel]
						, alpha );
				}

				return true;
			}

			inline bool doDecodeBC5Block( const uint8_t * bitstring
				, int shift
				, int offset
				, uint8_t * pixelBuffer )
			{
				// LSBFirst byte order only.
				uint64_t bits = ( *( uint64_t * )&bitstring[0] ) >> 16;
				int lum0 = ( int8_t )bitstring[0];
				int lum1 = ( int8_t )bitstring[1];

				if ( lum0 == -127 && lum1 == -128 )
				{
					// Not allowed.
					return false;
				}

				if ( lum0 == -128 )
				{
					lum0 = -127;
				}

				if ( lum1 == -128 )
				{
					lum1 = -127;
				}

				// Note: values are mapped to a red value of -127 to 127.
				uint16_t *pixel16_buffer = ( uint16_t * )pixelBuffer;

				for ( int i = 0; i < 16; i++ )
				{
					int control_code = bits & 0x7;
					int32_t result;

					if ( lum0 > lum1 )
					{
						switch ( control_code )
						{
						case 0: result = lum0; break;
						case 1: result = lum1; break;
						case 2: result = doDivideMinus895To895By7( 6 * lum0 + lum1 ); break;
						case 3: result = doDivideMinus895To895By7( 5 * lum0 + 2 * lum1 ); break;
						case 4: result = doDivideMinus895To895By7( 4 * lum0 + 3 * lum1 ); break;
						case 5: result = doDivideMinus895To895By7( 3 * lum0 + 4 * lum1 ); break;
						case 6: result = doDivideMinus895To895By7( 2 * lum0 + 5 * lum1 ); break;
						case 7: result = doDivideMinus895To895By7( lum0 + 6 * lum1 ); break;
						}
					}
					else
					{
						switch ( control_code )
						{
						case 0: result = lum0; break;
						case 1: result = lum1; break;
						case 2: result = doDivideMinus639To639By5( 4 * lum0 + lum1 ); break;
						case 3: result = doDivideMinus639To639By5( 3 * lum0 + 2 * lum1 ); break;
						case 4: result = doDivideMinus639To639By5( 2 * lum0 + 3 * lum1 ); break;
						case 5: result = doDivideMinus639To639By5( lum0 + 4 * lum1 ); break;
						case 6: result = -127; break;
						case 7: result = 127; break;
						}
					}

					// Map from [-127, 127] to [-32768, 32767].
					pixel16_buffer[( i << shift ) + offset] = ( uint16_t )( int16_t )
						( ( result + 127 ) * 65535 / 254 - 32768 );
					bits >>= 3;
				}

				return true;
			}

			bool doDecompressBC5Block( uint8_t const * bitstring
				, uint8_t * pixelBuffer )
			{
				bool result = doDecodeBC5Block( bitstring, 1, 0, pixelBuffer );

				if ( result )
				{
					result = doDecodeBC5Block( &bitstring[8], 1, 1, pixelBuffer );
				}

				return result;
			}
		}

		PixelFormat getPFWithoutAlpha( PixelFormat format )
		{
			PixelFormat result = PixelFormat::eCount;

			switch ( format )
			{
			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16A16_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::NoAlphaPF;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::NoAlphaPF;
				break;

			default:
				result = format;
				break;
			}

			return result;
		}

		bool hasComponent( PixelFormat format
			, PixelComponent component )
		{
			bool result = false;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = component == PixelComponent::eRed;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = component == PixelComponent::eRed;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = component == PixelComponent::eRed;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR5G6B5_UNORM:
			case PixelFormat::eR8G8B8_UNORM:
			case PixelFormat::eB8G8R8_UNORM:
			case PixelFormat::eR8G8B8_SRGB:
			case PixelFormat::eB8G8R8_SRGB:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
			case PixelFormat::eA8B8G8R8_UNORM:
			case PixelFormat::eR8G8B8A8_SRGB:
			case PixelFormat::eA8B8G8R8_SRGB:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen;
				break;

			case PixelFormat::eD16_UNORM:
				result = component == PixelComponent::eDepth;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = component == PixelComponent::eStencil
					|| component == PixelComponent::eDepth;
				break;

			case PixelFormat::eD32_UNORM:
				result = component == PixelComponent::eDepth;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = component == PixelComponent::eDepth;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = component == PixelComponent::eStencil
					|| component == PixelComponent::eDepth;
				break;

			case PixelFormat::eS8_UINT:
				result = component == PixelComponent::eStencil;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		bool hasAlpha( PixelFormat format )
		{
			bool result = false;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Alpha;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Alpha;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Alpha;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Alpha;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Alpha;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Alpha;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Alpha;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Alpha;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Alpha;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Alpha;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Alpha;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Alpha;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		bool isFloatingPoint( PixelFormat format )
		{
			return format == PixelFormat::eR32G32B32A32_SFLOAT
				|| format == PixelFormat::eR32G32B32_SFLOAT
				|| format == PixelFormat::eR32A32_SFLOAT
				|| format == PixelFormat::eR32_SFLOAT
				|| format == PixelFormat::eR16G16B16A16_SFLOAT
				|| format == PixelFormat::eR16G16B16_SFLOAT
				|| format == PixelFormat::eR16A16_SFLOAT
				|| format == PixelFormat::eR16_SFLOAT
				|| format == PixelFormat::eD32_SFLOAT
				|| format == PixelFormat::eD32_SFLOAT_S8_UINT;
		}

		bool isCompressed( PixelFormat format )
		{
			bool result = false;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Compressed;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Compressed;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Compressed;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Compressed;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Compressed;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Compressed;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Compressed;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Compressed;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Compressed;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Compressed;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Compressed;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Compressed;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::Compressed;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::Compressed;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::Compressed;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Compressed;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Compressed;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Compressed;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Compressed;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Compressed;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		PxBufferBaseSPtr decompressBuffer( PxBufferBaseSPtr src )
		{
			PxBufferBaseSPtr result = src;

			if ( isCompressed( src->getFormat() ) )
			{
				using PFNDecompressBlock = bool( *)( uint8_t const * data, uint8_t * pixelBuffer );
				PFNDecompressBlock decompressBlock = nullptr;

				switch ( src->getFormat() )
				{
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
					decompressBlock = doDecompressBC1Block;
					break;
				case PixelFormat::eBC3_UNORM_BLOCK:
					decompressBlock = doDecompressBC3Block;
					break;
				case PixelFormat::eBC5_UNORM_BLOCK:
					decompressBlock = doDecompressBC5Block;
					break;
				default:
					CU_Failure( "Unsupported compression format" );
					return result;
				}

				result = PxBufferBase::create( src->getDimensions()
					, PixelFormat::eR8G8B8A8_UNORM );
				uint8_t * pixelBuffer = result->getPtr();
				uint8_t blockBuffer[16 * 4u];
				uint8_t const * data = src->getConstPtr();
				uint32_t pixelSize = PF::getBytesPerPixel( result->getFormat() );
				uint32_t height = src->getHeight();
				uint32_t width = src->getWidth();
				uint32_t heightInBlocks = height / 4u;
				uint32_t widthInBlocks = width / 4u;

				for ( uint32_t y = 0u; y < heightInBlocks; ++y )
				{
					uint32_t newRows;

					if ( y * 4 + 3 >= height )
					{
						newRows = height - y * 4u;
					}
					else
					{
						newRows = 4u;
					}

					for ( uint32_t x = 0u; x < widthInBlocks; ++x )
					{
						bool r = decompressBlock( data, blockBuffer );

						if ( !r )
						{
							return src;
						}

						uint32_t blockSize = 8u;
						uint8_t * pixelp = pixelBuffer +
							y * 4u * width * pixelSize +
							+x * 4u * pixelSize;
						uint32_t newColumns;

						if ( x * 4 + 3 >= width )
						{
							newColumns = width - x * 4;
						}
						else
						{
							newColumns = 4u;
						}

						for ( uint32_t row = 0u; row < newRows; ++row )
						{
							memcpy( pixelp + row * width * pixelSize
								, blockBuffer + row * 4u * pixelSize
								, newColumns * pixelSize );
						}

						data += blockSize;
					}
				}

				if ( src->getFormat() == PixelFormat::eBC1_RGB_UNORM_BLOCK )
				{
					// Remove alpha channel for BC1 compressed buffer.
					extractAlpha( result );
				}
			}

			return result;
		}

		PxBufferBaseSPtr extractComponent( PxBufferBaseSPtr src
			, PixelComponent component )
		{
			src = decompressBuffer( src );
			PxBufferBaseSPtr result;

			if ( hasComponent( src->getFormat(), component ) )
			{
				auto index = uint32_t( component );

				if ( isFloatingPoint( src->getFormat() ) )
				{
					result = PxBufferBase::create( src->getDimensions(), PixelFormat::eR32_SFLOAT );
					auto srcBuffer = reinterpret_cast< float const * >( src->getConstPtr() );
					auto dstBuffer = reinterpret_cast< float * >( result->getPtr() );
					auto srcPixelSize = PF::getBytesPerPixel( src->getFormat() ) / sizeof( float );
					auto dstPixelSize = PF::getBytesPerPixel( result->getFormat() ) / sizeof( float );

					for ( size_t i = 0; i < result->getCount(); ++i )
					{
						*dstBuffer = srcBuffer[index];
						dstBuffer += dstPixelSize;
						srcBuffer += srcPixelSize;
					}
				}
				else
				{
					result = PxBufferBase::create( src->getDimensions(), PixelFormat::eR8_UNORM );
					auto srcBuffer = src->getConstPtr();
					auto dstBuffer = result->getPtr();
					auto srcPixelSize = PF::getBytesPerPixel( src->getFormat() );
					auto dstPixelSize = PF::getBytesPerPixel( result->getFormat() );

					for ( size_t i = 0; i < result->getCount(); ++i )
					{
						*dstBuffer = srcBuffer[index];
						dstBuffer += dstPixelSize;
						srcBuffer += srcPixelSize;
					}
				}
			}

			return result;
		}

		PxBufferBaseSPtr extractAlpha( PxBufferBaseSPtr & srcBuffer )
		{
			PxBufferBaseSPtr result;

			if ( hasAlpha( srcBuffer->getFormat() ) )
			{
				result = PxBufferBase::create( srcBuffer->getDimensions()
					, PixelFormat::eR8A8_UNORM
					, srcBuffer->getConstPtr()
					, srcBuffer->getFormat() );
				uint8_t * pBuffer = result->getPtr();

				for ( uint32_t i = 0; i < result->getSize(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}

				result = PxBufferBase::create( srcBuffer->getDimensions()
					, PixelFormat::eR8_UNORM
					, result->getConstPtr()
					, result->getFormat() );
				srcBuffer = PxBufferBase::create( srcBuffer->getDimensions()
					, getPFWithoutAlpha( srcBuffer->getFormat() )
					, srcBuffer->getConstPtr()
					, srcBuffer->getFormat() );
			}

			return result;
		}

		void reduceToAlpha( PxBufferBaseSPtr & srcBuffer )
		{
			if ( hasAlpha( srcBuffer->getFormat() ) )
			{
				if ( srcBuffer->getFormat() != PixelFormat::eR8A8_UNORM )
				{
					srcBuffer = PxBufferBase::create( srcBuffer->getDimensions()
						, PixelFormat::eR8A8_UNORM
						, srcBuffer->getConstPtr()
						, srcBuffer->getFormat() );
				}

				uint8_t * pBuffer = srcBuffer->getPtr();

				for ( uint32_t i = 0; i < srcBuffer->getSize(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}
			}

			srcBuffer = PxBufferBase::create( srcBuffer->getDimensions()
				, PixelFormat::eR8_UNORM
				, srcBuffer->getConstPtr()
				, srcBuffer->getFormat() );
		}

		uint8_t getBytesPerPixel( PixelFormat format )
		{
			uint8_t result = 0;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Size;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Size;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Size;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Size;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Size;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Size;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Size;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Size;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Size;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Size;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Size;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Size;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Size;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Size;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Size;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions<	PixelFormat::eR16G16B16_SFLOAT >::Size;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions<	PixelFormat::eR16G16B16A16_SFLOAT >::Size;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Size;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Size;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC1_RGB_UNORM_BLOCK >::Size;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC3_UNORM_BLOCK >::Size;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC5_UNORM_BLOCK >::Size;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Size;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Size;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Size;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Size;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Size;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Size;
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}

			return result;
		}

		uint8_t getComponentsCount( PixelFormat format )
		{
			uint8_t result = 0;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Count;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Count;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Count;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Count;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Count;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Count;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Count;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Count;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Count;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Count;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Count;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Count;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Count;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Count;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Count;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions<	PixelFormat::eR16G16B16_SFLOAT >::Count;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions<	PixelFormat::eR16G16B16A16_SFLOAT >::Count;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Count;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Count;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC1_RGB_UNORM_BLOCK >::Count;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC3_UNORM_BLOCK >::Count;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC5_UNORM_BLOCK >::Count;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Count;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Count;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Count;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Count;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Count;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Count;
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}

			return result;
		}

		void convertPixel( PixelFormat srcFormat
			, uint8_t const *& srcBuffer
			, PixelFormat dstFormat
			, uint8_t *& dstBuffer )
		{
			switch ( srcFormat )
			{
			case PixelFormat::eR8_UNORM:
				PixelDefinitions< PixelFormat::eR8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8A8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR5G6B5_UNORM:
				PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8G8B8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eB8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD16_UNORM:
				PixelDefinitions< PixelFormat::eD16_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD32_UNORM:
				PixelDefinitions< PixelFormat::eD32_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD32_SFLOAT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eS8_UINT:
				PixelDefinitions< PixelFormat::eS8_UINT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}
		}

		void convertBuffer( PixelFormat srcFormat
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			switch ( srcFormat )
			{
			case PixelFormat::eR8_UNORM:
				PixelDefinitions< PixelFormat::eR8_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16_SFLOAT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32_SFLOAT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8A8_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR5G6B5_UNORM:
				PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD16_UNORM:
				PixelDefinitions< PixelFormat::eD16_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_UNORM:
				PixelDefinitions< PixelFormat::eD32_UNORM >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_SFLOAT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eS8_UINT:
				PixelDefinitions< PixelFormat::eS8_UINT >::convert( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}
		}

		PixelFormat getFormatByName( String const & formatName )
		{
			PixelFormat result = PixelFormat::eCount;

			for ( int i = 0; i < int( result ); ++i )
			{
				switch ( PixelFormat( i ) )
				{
				case PixelFormat::eR8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR16_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8A8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8A8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR16A16_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR32A32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR5G5B5A1_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR5G6B5_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eB8G8R8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8_SRGB:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eB8G8R8_SRGB:
					result = ( formatName == PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8A8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8B8G8R8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8A8_SRGB:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8B8G8R8_SRGB:
					result = ( formatName == PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR16G16B16_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR16G16B16A16_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR32G32B32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR32G32B32A32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
					result = ( formatName == PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eBC3_UNORM_BLOCK:
					result = ( formatName == PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eBC5_UNORM_BLOCK:
					result = ( formatName == PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD16_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eD16_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD24_UNORM_S8_UINT:
					result = ( formatName == PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eD32_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eD32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32_SFLOAT_S8_UINT:
					result = ( formatName == PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eS8_UINT:
					result = ( formatName == PixelDefinitions< PixelFormat::eS8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				default:
					CU_Failure( "Unsupported pixel format" );
					break;
				}
			}

			return result;
		}

		String getFormatName( PixelFormat format )
		{
			String result;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::toStr();
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr();
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr();
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::toStr();
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::toStr();
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::toStr();
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::toStr();
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::toStr();
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::toStr();
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::toStr();
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::toStr();
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::toStr();
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::toStr();
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::toStr();
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::toStr();
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::toStr();
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::toStr();
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::toStr();
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::toStr();
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::toStr();
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::toStr();
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::toStr();
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::toStr();
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::toStr();
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::toStr();
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::toStr();
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::toStr();
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::toStr();
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}

			return result;
		}
	}
}
