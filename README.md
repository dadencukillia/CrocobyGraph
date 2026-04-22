# CrocobyGraph
Under development. Sorry, no time for `README.md`

## 📁 Code file structure
```tree
.
├── 📁 bin
├── 📁 cmake
├── 📄 CMakeLists.txt
├── 📁 external
├── 📁 include
│   └── 📁 crocobygraph
├── 📄 Makefile
└── 📁 src
    ├── 📄 config.hpp
    ├── 📁 interface
    ├── 📁 internal
    └── 📁 resources
```

### Description
- **bin** - a directory with a library usage demo
- **cmake** - a directory with external libraries connection instructions for CMake
- **CMakeLists.txt** - instructions on how to build the library
- **external** - a directory with libraries source code as Git submodules
- **include** - a directory with umbrella headers (accessible to users)
- **Makefile** - a file with shortcut commands for easy development and CI/CD
- **src** - a directory with the library source code
- **src/config.hpp** - a file with library constants
- **src/interface** - a directory with code
- **src/internal** - a directory with code
- **src/resources** - a directory with resources (fonts, images, sounds etc) and C++ files with `#embed` preprocessor

### src/interface vs src/internal
The division of source code into two directories was made to keep encapsulation easily.
The main purpose is to not crowd the main namespace by including **CrocobyGraph headers** with external libraries variables, functions, types etc.
The users must include the libraries by themselves if they want. To put it simply:
- **interface** - a directory with Header files (and their CPP) to be accessible for use
- **internal** - a directory with code that contains logic and interaction with the external libraries

**Rules:**
- **src/interface/*.hpp** can't include any file from external libraries or from internal directory
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

**What I can do if the interface class wants to use the external type as a field?**
- There's a lot ways to avoid including. My favourite is to use the the **Forward Declarations** and **Opaque Pointers**.
