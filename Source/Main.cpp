#include <array>
#include <chrono>
#include <queue>
#include <SFML/Graphics.hpp>

#include "Headers/DrawText.hpp"
#include "Headers/Global.hpp"
#include "Headers/DrawMap.hpp"
#include "Headers/GetMouseCell.hpp"
#include "Headers/Astar.hpp"
#include "Headers/BFS.hpp"
#include "Headers/Dijkstra.hpp"
#include "Headers/TextBoxWindow.h"
#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <string>

int main()
{
	bool save = false;
	bool load = false;

	//----------------< Breadth-first search start >----------------
	bool bfs_finished = 0;

	unsigned short bfs_path_length = 0;
	unsigned short bfs_total_checks = 0;

	std::chrono::microseconds bfs_duration(0);

	std::map<gbl::Position<>, gbl::Position<>> bfs_previous_cell;

	std::queue<gbl::Position<>> bfs_path_queue;

	gbl::Map<> bfs_map = {};

	gbl::Map<float> bfs_distances = {};

	int algo = 0;
	int option = 0;
	//----------------< Breadth-first search end >----------------

	//----------------< Dijkstra's algorithm start >----------------
	bool dijkstra_finished = 0;

	unsigned short dijkstra_path_length = 0;
	unsigned short dijkstra_total_checks = 0;

	std::chrono::microseconds dijkstra_duration(0);

	std::map<gbl::Position<>, gbl::Position<>> dijkstra_previous_cell;

	gbl::Map<float> dijkstra_distances = {};

	// I should've used typedef here.
	std::priority_queue<gbl::Position<>, std::vector<gbl::Position<>>, DijkstraComparison> dijkstra_path_queue((DijkstraComparison(&dijkstra_distances)));

	gbl::Map<> dijkstra_map = {};
	//----------------< Dijkstra's algorithm end >----------------

	//----------------< A star algorithm start >----------------
	bool astar_finished = 0;

	unsigned short astar_path_length = 0;
	unsigned short astar_total_checks = 0;

	std::chrono::microseconds astar_duration(0);

	std::map<gbl::Position<>, gbl::Position<>> astar_previous_cell;

	std::vector<gbl::Position<>> astar_path_vector;

	gbl::Map<> astar_map = {};

	gbl::Map<float> astar_f_scores = {};
	gbl::Map<float> astar_g_scores = {};
	gbl::Map<float> astar_h_scores = {};
	//----------------< A star algorithm end >----------------

	bool mouse_pressed = 0;
	bool pause_search = 1;

	std::chrono::microseconds lag(0);

	std::chrono::steady_clock::time_point previous_time;

	sf::Event event;

	sf::RenderWindow window(sf::VideoMode(gbl::SCREEN::RESIZE * gbl::SCREEN::WIDTH, gbl::SCREEN::RESIZE * gbl::SCREEN::HEIGHT), "Pathfinding", sf::Style::Close);
	window.setView(sf::View(sf::FloatRect(0, 0, gbl::SCREEN::WIDTH, gbl::SCREEN::HEIGHT)));

	sf::Sprite map_sprite;

	sf::Texture font_texture;
	font_texture.loadFromFile("Resources/Images/Font.png");

	sf::Texture map_texture;
	map_texture.loadFromFile("Resources/Images/Map.png");

	gbl::Map<> map = {};

	gbl::Position<> finish_position(gbl::MAP::COLUMNS - 1, gbl::MAP::ROWS - 1);
	gbl::Position<> start_position(0, 0);

	// This is used to draw lines of cells.
	gbl::Position<short> mouse_cell_start;

	for (std::array<gbl::CellData, gbl::MAP::ROWS> &column : map)
	{
		column.fill(gbl::MAP::Cell::Empty);
	}

	bfs_map = map;
	// dijkstra_map = map;
	// astar_map = map;

	map_sprite.setTexture(map_texture);

	previous_time = std::chrono::steady_clock::now();

	while (1 == window.isOpen())
	{
		std::chrono::microseconds delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);

		lag += delta_time;
		previous_time += delta_time;

		while (gbl::SCREEN::FRAME_DURATION <= lag)
		{
			bool map_updated = 0;

			lag -= gbl::SCREEN::FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					window.close();

					break;
				}
				case sf::Event::KeyPressed:
				{
					switch (event.key.code)
					{
					case sf::Keyboard::Enter: // Pause/Resume the search
					{
						if (option != 3 && option != 4)
						{
							pause_search = 1 - pause_search;
						}
						else
						{
							if (option == 3)
							{
								save = true;
							}
							else if (option == 4)
							{
								load = true;
							}
						}

						break;
					}
					case sf::Keyboard::R: // Restart the search
					{
						map_updated = 1;

						break;
					}
					case sf::Keyboard::Space: // Clear the map
					{
						for (unsigned short a = 0; a < gbl::MAP::COLUMNS; a++)
						{
							for (unsigned short b = 0; b < gbl::MAP::ROWS; b++)
							{
								map[a][b] = gbl::MAP::Empty;
							}
						}

						map_updated = 1;
						break;
					}
					case sf::Keyboard::Down:
					{
						option++;
						if (option > 4)
						{
							option = 0;
						}

						if (option >= 0 && option <= 2)
						{ // Check if option is between 0 and 2 (inclusive)
							algo = option;
						}

						map_updated = 1;
						break; // Prevent fall-through
					}

					case sf::Keyboard::Up:
					{
						option--;
						if (option < 0)
						{
							option = 4;
						}

						if (option >= 0 && option <= 2)
						{
							algo = option;
						}

						map_updated = 1;
						break; // Prevent fall-through
					}

					default:
						break;
					}
				}
				default:
					break;
				}
			}

			if (1 == sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || 1 == sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
			{
				if (0 == mouse_pressed)
				{
					mouse_pressed = 1;

					mouse_cell_start = get_mouse_cell(window);
				}
			}
			else
			{
				mouse_pressed = 0;
			}

			if (1 == mouse_pressed)
			{
				char step_x;
				char step_y;

				// Why it's line_size and not line_length?
				unsigned short line_size;

				gbl::Position<short> mouse_cell = get_mouse_cell(window);

				// Here we're choosing the maximum between the vertical and horizontal distances.
				line_size = 1 + std::max(abs(mouse_cell.first - mouse_cell_start.first), abs(mouse_cell.second - mouse_cell_start.second));

				step_x = gbl::sign(mouse_cell.first - mouse_cell_start.first);
				step_y = gbl::sign(mouse_cell.second - mouse_cell_start.second);

				for (unsigned short a = 0; a < line_size; a++)
				{
					gbl::Position<short> cell;

					// We take 1 step in one direction and use the slope to calculate the step in the other direction.
					cell.first = mouse_cell_start.first + step_x * floor(a * (1 + abs(mouse_cell.first - mouse_cell_start.first)) / static_cast<float>(line_size));
					cell.second = mouse_cell_start.second + step_y * floor(a * (1 + abs(mouse_cell.second - mouse_cell_start.second)) / static_cast<float>(line_size));

					if (0 <= cell.first && 0 <= cell.second && cell.first < gbl::MAP::COLUMNS && cell.second < gbl::MAP::ROWS)
					{
						if (finish_position != static_cast<gbl::Position<>>(cell) && start_position != static_cast<gbl::Position<>>(cell))
						{
							if (1 == sf::Mouse::isButtonPressed(sf::Mouse::Left))
							{
								if (gbl::MAP::Cell::Wall != map[cell.first][cell.second].value)
								{
									map_updated = 1;

									if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::E))
									{
										finish_position = cell;
									}
									else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::S))
									{
										start_position = cell;
									}
									else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::N))
									{

										std::string userInput = map[cell.first][cell.second].name;
										TextBoxWindow textBox;
										std::string prompt = "Enter Cell Name:";
										textBox.run(userInput, prompt);
										map[cell.first][cell.second].name = userInput;
									}
									else if (map[cell.first][cell.second].name == "")
									{
										map[cell.first][cell.second] = gbl::MAP::Cell::Wall;
									}
								}
							}
							else if (1 == sf::Mouse::isButtonPressed(sf::Mouse::Right))
							{
								if (gbl::MAP::Cell::Wall == map[cell.first][cell.second].value)
								{
									map_updated = 1;

									map[cell.first][cell.second] = gbl::MAP::Cell::Empty;
								}
							}
						}
					}
				}

				mouse_cell_start = mouse_cell;
			}

			// Reset everything!
			if (1 == map_updated)
			{
				if (algo == 0)
				{
					bfs_map = map;

					bfs_reset(bfs_finished, bfs_path_length, bfs_total_checks, bfs_duration, bfs_previous_cell, bfs_path_queue, bfs_distances, start_position, bfs_map);
				}
				else if (algo == 1)
				{
					dijkstra_map = map;

					dijkstra_reset(dijkstra_finished, dijkstra_path_length, dijkstra_total_checks, dijkstra_duration, dijkstra_previous_cell, dijkstra_path_queue, dijkstra_distances, start_position, dijkstra_map);
				}
				else if (algo == 2)
				{
					astar_map = map;

					astar_reset(astar_finished, astar_path_length, astar_total_checks, astar_duration, astar_previous_cell, astar_path_vector, astar_f_scores, astar_g_scores, astar_h_scores, finish_position, start_position, astar_map);
				}
			}

			// Search!
			if (0 == pause_search)
			{
				if (algo == 0)
				{
					if (0 == bfs_finished)
					{
						bfs_finished = bfs_search(bfs_path_length, bfs_total_checks, bfs_duration, bfs_previous_cell, bfs_path_queue, bfs_distances, finish_position, start_position, bfs_map);
					}
				}
				else if (algo == 1)
				{
					if (0 == dijkstra_finished)
					{
						dijkstra_finished = dijkstra_search(dijkstra_path_length, dijkstra_total_checks, dijkstra_duration, dijkstra_previous_cell, dijkstra_path_queue, dijkstra_distances, finish_position, start_position, dijkstra_map);
					}
				}
				else if (algo == 2)
				{
					if (0 == astar_finished)
					{
						astar_finished = astar_search(astar_path_length, astar_total_checks, astar_duration, astar_previous_cell, astar_path_vector, astar_f_scores, astar_g_scores, astar_h_scores, finish_position, start_position, astar_map);
					}
				}
			}

			// Draw everything!
			if (gbl::SCREEN::FRAME_DURATION > lag)
			{
				window.clear();

				auto cell = get_mouse_cell(window);
				if (cell.first < 0 || cell.first >= gbl::MAP::COLUMNS || cell.second < 0 || cell.second >= gbl::MAP::ROWS)
				{
					draw_text(0, 0, 650, 662, "Mouse is outside\nthe map bounds.", window, font_texture);
				}
				else
				{
					draw_text(0, 0, 650, 662, "Start: " + std::to_string(start_position.first) + "," + std::to_string(start_position.second) + "\nEnd: " + std::to_string(finish_position.first) + "," + std::to_string(finish_position.second) + "\nCurrent: " + std::to_string(cell.first) + "," + std::to_string(cell.second) + "\nName: " + map[cell.first][cell.second].name,
							  window, font_texture);
				}

				if (algo == 0)
				{
					draw_map(0, 0, finish_position, start_position, window, map_sprite, bfs_map, 1, bfs_distances);

					if (option == 0)
					{
						draw_text(0, 0, 10, 662, "\n->\n\n", window, font_texture);
					}

					draw_stats(gbl::SCREEN::WIDTH - 125, 724, bfs_path_length, bfs_total_checks, bfs_duration, "BFS", window, font_texture);
				}
				else if (algo == 1)
				{
					draw_map(0, 0, finish_position, start_position, window, map_sprite, dijkstra_map, 1, dijkstra_distances);

					if (option == 1)
					{
						draw_text(0, 0, 10, 662, "\n\n->\n", window, font_texture);
					}

					draw_stats(gbl::SCREEN::WIDTH - 125, 724, dijkstra_path_length, dijkstra_total_checks, dijkstra_duration, "Dijkstra", window, font_texture);
				}
				else if (algo == 2)
				{
					draw_map(0, 0, finish_position, start_position, window, map_sprite, astar_map, 1, astar_g_scores);

					if (option == 2)
					{
						draw_text(0, 0, 10, 662, "\n\n\n->", window, font_texture);
					}

					draw_stats(gbl::SCREEN::WIDTH - 125, 724, astar_path_length, astar_total_checks, astar_duration, "A star", window, font_texture);
				}

				if (option == 3)
				{
					draw_text(0, 0, 310, 662, "\n->\n", window, font_texture);

					if (save)
					{
						nlohmann::json json_map; // JSON object to store the map

						// Convert the map to JSON
						for (size_t row = 0; row < gbl::MAP::ROWS; ++row)
						{
							for (size_t col = 0; col < gbl::MAP::COLUMNS; ++col)
							{
								const auto &cell = map[col][row];
								nlohmann::json cell_data;

								// Store structured position fields
								cell_data["row"] = row;
								cell_data["col"] = col;

								// Optionally add a human-readable string representation of the position
								cell_data["position"] = "(" + std::to_string(row) + ", " + std::to_string(col) + ")";

								cell_data["value"] = static_cast<int>(cell.value);
								cell_data["name"] = cell.name;

								// Add the cell data to the map
								json_map["map"].push_back(cell_data);
							}
						}

						std::string filename;
						TextBoxWindow textBox;
						std::string prompt = "Enter name to save the map:";
						textBox.run(filename, prompt);

						// Ensure the filename is valid
						if (filename.empty())
						{
							std::cerr << "Error: Invalid filename" << std::endl;
							return 0;
						}

						// Save JSON to file
						std::ofstream file(filename);
						if (file.is_open())
						{
							file << json_map.dump(4); // Save with 4 spaces indentation for readability
							file.close();
							std::cout << "Map saved successfully to " << filename << std::endl;
						}
						else
						{
							std::cerr << "Error: Could not save the file to " << filename << std::endl;
						}

						save = false;
					}
				}
				if (option == 4)
				{
					draw_text(0, 0, 310, 662, "\n\n->", window, font_texture);

					if (load)
					{
						nlohmann::json json_map; // JSON object to load the map

						// Prompt user for a filename
						std::string filename;
						TextBoxWindow textBox;
						std::string prompt = "Enter name to load the map:";
						textBox.run(filename, prompt);

						// Open the file
						std::ifstream file(filename);
						if (file.is_open())
						{
							try
							{
								// Parse the JSON file
								file >> json_map;

								// Clear the current map
								map = {};

								// Check if the "map" key exists
								if (json_map.contains("map"))
								{
									for (const auto &cell_data : json_map["map"])
									{
										size_t row = cell_data["row"].get<size_t>();
										size_t col = cell_data["col"].get<size_t>();

										if (row < gbl::MAP::ROWS && col < gbl::MAP::COLUMNS)
										{
											map[col][row].value = static_cast<gbl::MAP::Cell>(cell_data["value"].get<int>());
											map[col][row].name = cell_data["name"].get<std::string>();
										}
									}
									std::cout << "Map loaded successfully from " << filename << std::endl;
								}
								else
								{
									std::cerr << "Error: 'map' key not found in the JSON file." << std::endl;
								}
							}
							catch (const std::exception &e)
							{
								std::cerr << "Error: Failed to parse the JSON file. " << e.what() << std::endl;
							}

							file.close();
						}
						else
						{
							std::cerr << "Error: Could not open the file " << filename << std::endl;
						}

						load = false;
					}
				}

				draw_text(0, 0, 50, 664, "Choose Algorithm\n1. BFS\n2. Dijkstra\n3. A*", window, font_texture);
				draw_text(0, 0, 350, 664, "Choose Option\n1. Save Map\n2. Load Map", window, font_texture);

				window.display();
			}
		}
	}
}
