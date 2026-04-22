# CrocobyGraph

An interactive graph visualization library for C++ with real-time rendering and ECS-based architecture.

## 📁 Code file structure

```tree
.
├── 📁 bin
├── 📁 cmake
├── 📄 CMakeLists.txt
├── 📁 external
├── 📁 include
│   └── 📁 crocobygraph
├── 📄 Makefile
└── 📁 src
    ├── 📄 config.hpp
    ├── 📁 interface
    ├── 📁 internal
    └── 📁 resources
```

### Description

- **bin** - a directory with library usage examples
- **cmake** - a directory with external library connection instructions for CMake
- **CMakeLists.txt** - instructions on how to build the library
- **external** - a directory with library source code as Git submodules
- **include** - a directory with umbrella headers (accessible to users)
- **Makefile** - a file with shortcut commands for easy development and CI/CD
- **src** - a directory with the library source code
- **src/config.hpp** - a file with library constants
- **src/interface** - a directory with public interface code
- **src/internal** - a directory with internal implementation code
- **src/resources** - a directory with resources (fonts, images, sounds, etc.) and C++ files with `#embed` preprocessor directives

### src/interface vs src/internal

The division of source code into two directories maintains encapsulation clearly. The main purpose is to avoid cluttering the main namespace by including **CrocobyGraph headers** with external library variables, functions, types, and so on. Users must include the external libraries themselves if they need them. To summarize:

- **interface** - a directory with header files (and their corresponding .cpp files) meant to be accessible for public use
- **internal** - a directory with code that contains logic and interaction with external libraries

**Rules:**

- **src/interface/*.hpp** cannot include any files from external libraries or the internal directory
- **src/interface/*.cpp** can freely include any file
- **src/internal/*.hpp** can freely include any file
- **src/internal/*.cpp** can freely include any file
- **src/config.hpp** can be included by any file
- **include/crocobygraph/*.hpp** can include only interface .hpp files
- **src/resources/*.hpp** can be included only by .cpp files

|                     | interface | internal | external |
|---------------------|-----------|----------|----------|
| include/*.hpp       | ✅ Yes    | ❌ No    | ❌ No    |
| src/interface/*.hpp | ✅ Yes    | ❌ No    | ❌ No    |
| src/interface/*.cpp | ✅ Yes    | ✅ Yes   | ✅ Yes   |
| src/internal/*.hpp  | ✅ Yes    | ✅ Yes   | ✅ Yes   |
| src/internal/*.cpp  | ✅ Yes    | ✅ Yes   | ✅ Yes   |  

**What can I do if an interface class needs to use an external type as a field?**

There are many ways to avoid including external headers. My favorite approach is to use **Forward Declarations** and **Opaque Pointers**.