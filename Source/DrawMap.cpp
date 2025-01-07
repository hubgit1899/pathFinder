#include <array>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/DrawMap.hpp"

void draw_map(const unsigned short i_x, const unsigned short i_y, const gbl::Position<> &i_finish_position, const gbl::Position<> &i_start_position, sf::RenderWindow &i_window, sf::Sprite &i_map_sprite, const gbl::Map<> &i_map, const bool i_change_colors, const gbl::Map<float> &i_distances)
{
	float max_distance = FLT_MIN;

	if (1 == i_change_colors)
	{
		for (const std::array<float, gbl::MAP::ROWS> &column : i_distances)
		{
			for (const float distance : column)
			{
				if (FLT_MAX != distance)
				{
					max_distance = std::max(distance, max_distance);
				}
			}
		}
	}

	for (unsigned short a = 0; a < gbl::MAP::COLUMNS; a++)
	{
		for (unsigned short b = 0; b < gbl::MAP::ROWS; b++)
		{
			i_map_sprite.setPosition(gbl::MAP::ORIGIN_X + i_x + a * gbl::MAP::CELL_SIZE, gbl::MAP::ORIGIN_Y + i_y + b * gbl::MAP::CELL_SIZE);

			if (a == i_finish_position.first && b == i_finish_position.second)
			{
				sf::IntRect texture_rect(2 * gbl::MAP::CELL_SIZE, 0, gbl::MAP::CELL_SIZE, gbl::MAP::CELL_SIZE);
				i_map_sprite.setTextureRect(texture_rect);
			}
			else if (a == i_start_position.first && b == i_start_position.second)
			{
				sf::IntRect texture_rect(3 * gbl::MAP::CELL_SIZE, 0, gbl::MAP::CELL_SIZE, gbl::MAP::CELL_SIZE);
				i_map_sprite.setTextureRect(texture_rect);
			}
			else
			{

				if (!i_map[a][b].name.empty()) // Check if cell name is not empty
				{
					// Set the texture rect for cells with names (bright yellow indicator)
					sf::IntRect texture_rect(4 * gbl::MAP::CELL_SIZE, 0, gbl::MAP::CELL_SIZE, gbl::MAP::CELL_SIZE);
					i_map_sprite.setTextureRect(texture_rect);
				}
				else
				{
					// Reset the texture rect to the default for other cells
					i_map_sprite.setTextureRect(sf::IntRect(0, 0, gbl::MAP::CELL_SIZE, gbl::MAP::CELL_SIZE));

					gbl::MAP::Cell cell_type = i_map[a][b].value;
					switch (cell_type)
					{
					case gbl::MAP::Cell::Empty:
						i_map_sprite.setColor(sf::Color(255, 255, 255)); // White for empty cells
						break;

					case gbl::MAP::Cell::Path:
						i_map_sprite.setColor(sf::Color(255, 255, 50)); // Path color
						break;

					case gbl::MAP::Cell::Visited:
						if (0 != i_change_colors)
						{
							unsigned short color_value = round(255 * i_distances[a][b] / max_distance);
							unsigned short red = std::max(120, 230 - color_value / 3);
							unsigned short green = std::max(120, 230 - color_value / 3);
							unsigned short blue = std::max(180, 255 - color_value / 4);
							i_map_sprite.setColor(sf::Color(red, green, blue));
						}
						else
						{
							i_map_sprite.setColor(sf::Color(230, 230, 255)); // Default fallback color
						}
						break;

					case gbl::MAP::Cell::Wall:
						i_map_sprite.setColor(sf::Color(86, 86, 135)); // Wall color
						break;
					}
				}
			}

			i_window.draw(i_map_sprite);
		}
	}
}