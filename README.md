# CrocobyGraph
An interactive graph visualization library for C++ with real-time rendering and ECS-based architecture.

<img width="1861" height="1004" alt="image" src="https://github.com/user-attachments/assets/c144023d-a52f-4bc0-b844-8f040e4bf222" />

## ✨ Features
- **Interactive Visualization** - Real-time rendering of complex graphs and neural networks
- **ECS Architecture** - Entity Component System design for flexible and extensible graph management
- **Physics-based Layout** - Automatic node positioning with physics simulation
- **Editor Framework** - Built-in editor interface for graph manipulation and inspection
- **Modern C++** - Written in C++23 with best practices and clean API design

## 🚀 Quick Start

### Prerequisites
- C++23 compatible compiler
- CLang 20 or higher
- CMake 3.30.0 or higher
- Dependencies (managed via CMake):
  - raylib - Graphics rendering
  - imgui - UI framework
  - rlImGui - raylib + ImGui integration
  - EnTT - Entity Component System library

### Installation
1. Create `CMakeLists.txt`
1. Create `external` directory
1. Run command:

    ```bash
    cd external
    git submodule add --depth=1 https://github.com/dadencukillia/CrocobyGraph.git
    cd CrocobyGraph
    git submodule update --init --recursive --progress --depth=1
    cd ../..
    ```
1. Add into `CMakeLists.txt` this line:

    ```CMake
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/CrocobyGraph)
    ```
1. And link it to your executable:

    ```CMake
    target_link_libraries(YOUR_EXECUTABLE PRIVATE CrocobyGraph)
    ```

## 🍎 Examples
```C++
#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"
#include <vector>

namespace cg = CrocobyGraph;

int main() {
  // Create a simple graph
  std::vector<cg::LayoutGraphNode> nodes = {
    {
      .label = "Start",
      .color = cg::ColorPresets::RED,
      .points_to = { "finish" },
      .id = "start",
    },
    {
      .label = "Finish",
      .color = { 0x00FF00FF },
      .points_to = { "start" },
      .id = "finish",
    },
  };

  // Create ECS and visualize
  cg::GraphECS ecs {};
  ecs.get_scene().append(cg::decompose(std::move(nodes)));
  ecs.add_system(cg::get_window_system<cg::PhysicsFrame, cg::EditorFrame>());
  ecs.run_loop();

  return 0;
}
```

```C++
std::string nodes[3] = { "Parent", "Child 1", "Child 2" };
const bool connections[3][3] = {
  { true, true, true },
  { false, true, false },
  { false, false, true }
};

auto graph = cg::layout_from_adjacency_matrix<3>(
  std::move(nodes),
  connections
);
```

```C++
auto neural_net = cg::neural_network({
  "Ticket class",
  "Gender",
  "Age",
  "Parch",
  "Fare"
}, { 32, 16 }, {
  "Survivance chance"
});

cg::GraphECS ecs {};
ecs.get_scene().append(std::move(neural_net));
ecs.add_system(cg::get_window_system<cg::PhysicsFrame, cg::EditorFrame>());
ecs.run_loop();
```

## 🧱 Under development
More features and more detailed documentation are coming soon.

## 📚 Used in the project
- [`skypjack/entt`](https://github.com/skypjack/entt) - Gaming meets modern C++ - a fast and reliable entity component system (ECS) and much more
- [`ocornut/imgui`](https://github.com/ocornut/imgui) -  Bloat-free Graphical User interface for C++ with minimal dependencies
- [`raysan5/raylib`](https://github.com/raysan5/raylib) - A simple and easy-to-use library to enjoy videogames programming 
- [`raylib-extras/rlImGui`](https://github.com/raylib-extras/rlImGui) - A Raylib integration with DearImGui
- [`googlefonts/opensans`](https://github.com/googlefonts/opensans) - Open Sans font

## 🧑‍⚖ License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Code and documentation Copyright (c) 2026 Illia Diadenchuk.

The project is distributing as a library, so if you've compiled a binary you can choose any way how to include notices in the binary.
