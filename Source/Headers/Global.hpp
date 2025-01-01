#pragma once
#include <cfloat>

namespace gbl
{
	namespace PATHFINDING
	{
		// If you don't want to see the process of the search, set this constant to USHRT_MAX.
		constexpr unsigned short CHECKS_PER_FRAME = 24;
	}

	namespace MAP
	{
		constexpr unsigned char CELL_SIZE = 8;
		constexpr unsigned char ORIGIN_X = 0;
		constexpr unsigned char ORIGIN_Y = 0;

		constexpr unsigned short COLUMNS = 144;
		constexpr unsigned short ROWS = 81;

		enum Cell
		{
			Empty,
			Invalid,
			Path,
			Visited,
			Wall
		};
	}

	namespace SCREEN
	{
		constexpr unsigned char RESIZE = 1;

		constexpr unsigned short HEIGHT = 814;
		constexpr unsigned short WIDTH = 1152;

		// This will make the program run at 60 FPS.
		// Because 1 second == 1,000,000 microseconds
		// 1,000,000 microseconds / 60 frames = 16667 microseconds per frame
		constexpr std::chrono::microseconds FRAME_DURATION(16667);
	}

	// Did they add the sign function in the new versions of C++? I need to check that. Because it makes ABSOLUTELY no sense that it's still not added.
	template <typename value_type>
	char sign(const value_type i_value)
	{
		return (0 < i_value) - (0 > i_value);
	}

	struct CellData
	{
		MAP::Cell value;  // Existing cell value
		std::string name; // New name variable

		CellData(MAP::Cell v = MAP::Cell::Empty, const std::string &n = "")
			: value(v), name(n) {}
	};

	template <typename value_type = CellData>
	using Map = std::array<std::array<value_type, MAP::ROWS>, MAP::COLUMNS>;

	template <typename value_type = unsigned short>
	using Position = std::pair<value_type, value_type>;
}