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

int main()
{
    //----------------< Breadth-first search start >----------------
    bool bfs_finished = 0;

    unsigned short bfs_path_length = 0;
    unsigned short bfs_total_checks = 0;

    std::chrono::microseconds bfs_duration(0);

    std::map<gbl::Position<>, gbl::Position<>> bfs_previous_cell;

    std::queue<gbl::Position<>> bfs_path_queue;

    gbl::Map<> bfs_map = {};

    gbl::Map<float> bfs_distances = {};
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

    // User interaction variables
    bool mouse_pressed = 0;
    bool pause_search = 1;
    std::chrono::microseconds lag(0);
    std::chrono::steady_clock::time_point previous_time;
    sf::Event event;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(gbl::SCREEN::RESIZE * gbl::SCREEN::WIDTH, gbl::SCREEN::RESIZE * gbl::SCREEN::HEIGHT), "Pathfinding", sf::Style::Close);
    window.setView(sf::View(sf::FloatRect(0, 0, gbl::SCREEN::WIDTH, gbl::SCREEN::HEIGHT)));

    sf::Sprite map_sprite;

    // Load textures
    sf::Texture font_texture, map_texture;
    font_texture.loadFromFile("/Users/junaidalam/Desktop/Study/Semester 3/DSA/DSA Project/Pathfinding-Main/Source/Resources/Images/Font.png");
    map_texture.loadFromFile("/Users/junaidalam/Desktop/Study/Semester 3/DSA/DSA Project/Pathfinding-Main/Source/Resources/Images/Map.png");

    // Initialize the map and positions
    gbl::Map<> map = {};
    gbl::Position<> finish_position(gbl::MAP::COLUMNS - 1, gbl::MAP::ROWS - 1);
    gbl::Position<> start_position(0, 0);
    for (std::array<gbl::MAP::Cell, gbl::MAP::ROWS> &column : map)
    {
        column.fill(gbl::MAP::Cell::Empty);
    }

    // Set the initial maps for algorithms
    bfs_map = map;
    dijkstra_map = map;
    astar_map = map;

    map_sprite.setTexture(map_texture);

    previous_time = std::chrono::steady_clock::now();

    while (window.isOpen())
    {
        std::chrono::microseconds delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);
        lag += delta_time;
        previous_time += delta_time;

        while (gbl::SCREEN::FRAME_DURATION <= lag)
        {
            bool map_updated = 0;
            lag -= gbl::SCREEN::FRAME_DURATION;

            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                else if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Enter)
                    {
                        pause_search = 1 - pause_search; // Pause/Resume
                    }
                    else if (event.key.code == sf::Keyboard::R)
                    {
                        map_updated = 1; // Restart
                    }
                    else if (event.key.code == sf::Keyboard::Space)
                    {
                        for (unsigned short a = 0; a < gbl::MAP::COLUMNS; a++)
                        {
                            for (unsigned short b = 0; b < gbl::MAP::ROWS; b++)
                            {
                                map[a][b] = gbl::MAP::Empty; // Clear the map
                            }
                        }
                        map_updated = 1;
                    }
                }
            }

            // Handle mouse input for moving start and finish positions
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            {
                if (!mouse_pressed)
                {
                    mouse_pressed = 1;
                    gbl::Position<short> mouse_cell = get_mouse_cell(window);
                    if (mouse_cell == start_position)
                    {
                        start_position = mouse_cell; // Move start position
                    }
                    else if (mouse_cell == finish_position)
                    {
                        finish_position = mouse_cell; // Move finish position
                    }
                }
            }
            else
            {
                mouse_pressed = 0;
            }

            // Reset everything if the map is updated
            if (map_updated)
            {
                bfs_map = map;
                bfs_reset(bfs_finished, bfs_path_length, bfs_total_checks, bfs_duration, bfs_previous_cell, bfs_path_queue, bfs_distances, start_position, bfs_map);
                dijkstra_map = map;
                dijkstra_reset(dijkstra_finished, dijkstra_path_length, dijkstra_total_checks, dijkstra_duration, dijkstra_previous_cell, dijkstra_path_queue, dijkstra_distances, start_position, dijkstra_map);
                astar_map = map;
                astar_reset(astar_finished, astar_path_length, astar_total_checks, astar_duration, astar_previous_cell, astar_path_vector, astar_f_scores, astar_g_scores, astar_h_scores, finish_position, start_position, astar_map);
            }

            // Execute search algorithms if not paused
            if (!pause_search)
            {
                if (!bfs_finished)
                {
                    bfs_finished = bfs_search(bfs_path_length, bfs_total_checks, bfs_duration, bfs_previous_cell, bfs_path_queue, bfs_distances, finish_position, start_position, bfs_map);
                }
                if (!dijkstra_finished)
                {
                    dijkstra_finished = dijkstra_search(dijkstra_path_length, dijkstra_total_checks, dijkstra_duration, dijkstra_previous_cell, dijkstra_path_queue, dijkstra_distances, finish_position, start_position, dijkstra_map);
                }
                if (!astar_finished)
                {
                    astar_finished = astar_search(astar_path_length, astar_total_checks, astar_duration, astar_previous_cell, astar_path_vector, astar_f_scores, astar_g_scores, astar_h_scores, finish_position, start_position, astar_map);
                }
            }

            // Draw everything
            if (gbl::SCREEN::FRAME_DURATION > lag)
            {
                window.clear();
                draw_map(0, 0, finish_position, start_position, window, map_sprite, map, 1, bfs_distances); // Draw the single large map
                draw_stats(0.625f * gbl::SCREEN::WIDTH, 0.625f * gbl::SCREEN::HEIGHT, bfs_path_length, bfs_total_checks, bfs_duration, "BFS", window, font_texture);
                draw_stats(0.875f * gbl::SCREEN::WIDTH, 0.625f * gbl::SCREEN::HEIGHT, dijkstra_path_length, dijkstra_total_checks, dijkstra_duration, "Dijkstra", window, font_texture);
                draw_stats(0.75f * gbl::SCREEN::WIDTH, 0.875f * gbl::SCREEN::HEIGHT, astar_path_length, astar_total_checks, astar_duration, "A star", window, font_texture);
                window.display();
            }
        }
    }
}