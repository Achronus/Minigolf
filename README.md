# Minigolf

This repository focuses on a simple minigolf game with one level, exploring different physics simulation aspects using PhysX 3.4.2. The level takes place on top of a kitchen counter and allows switching between different types of golf balls to complete the level.

[demo placeholder]

## File Structure

The file structure used for the project is outlined below.

```pseudocode
.
+-- Extras
|   +-- GLFontData.h
|   +-- GLFontRenderer.cpp
|   +-- GLFontRenderer.h
|   +-- HUD.h
|   +-- Renderer.cpp
|   +-- Renderer.h
|   +-- UserData.h
+-- Graphics
|   +-- include
|       +-- ...
|   +-- lib
|       +-- ...
+-- Headers
|   +-- Ball.h
|   +-- BasicActors.h
|   +-- Camera.h
|   +-- Exception.h
|   +-- MyPhysicsEngine.h
|   +-- PhysicsEngine.h
|   +-- VisualDebugger.h
+-- Source
|   +-- Ball.cpp
|   +-- Camera.cpp
|   +-- Main.cpp
|   +-- PhysicsEngine.cpp
|   +-- VisualDebugger.cpp
+-- .gitignore
+-- LICENSE
+-- Macros.props
+-- minigolf.sln
+-- minigolf.vcxproj
+-- README.md
```

__Key__:

- `\Extras` folder - contains functionality that handles the projects rendering.
- `\Graphics` folder - contains files that handles GLUT.
- `\Headers` folder - contains the core .h files used to build the project.
- `\Source` folder - contains the core .cpp files used to build the project. These call their respective .h file from the `\Headers` folder.
- `Macros.props` - a Visual Studio properties file used to obtain the required NVIDIA PhysX external dependices.

## Dependencies

This project requires a Microsoft Visual Studio 2019 environment, with the C++ programming language packages enabled. Additionally, it uses PhysX version 3.4.2 which requires additional step that is configured using the instructions below.

_Please note: this follows a Windows 10 install process, some of these steps may be different for other operating systems._

1. Download the PhysX SDK v3.2.4 from NIVIDIA's website - [https://developer.nvidia.com/rdp/physx-downloads](https://developer.nvidia.com/rdp/physx-downloads).

2. Navigate to the following file path and unpack the zip file (create the path if required).

    ```bash
    C:\Program Files (x86)\NVIDIA Corporation\PhysX SDK\PhysX-3.4.2
    ```
  
3. Test that the packages have been unpacked succesfully by opening the run window (keyboard shortcut: windows key + r) and copy the below path.

    ```bash
    C:\Program  Files  (x86)\NVIDIA  Corporation\PhysX SDK\PhysX-3.4.2\PxShared\bin\vc15win64
    ```

    If working correctly, proceed to step 4. Otherwise, double check the file paths and folder structure.

4. Update the Windows system path to allow the program to find the PhysX DLLs. These paths __must__ be set before starting Visual Studio.

    Open the Windows Control Panel and select:

    ```bash
    System and Security -> System -> Advanced System Settings -> Environment Variables-> User variables for [user] -> Path -> New
    ```

    Create the following two required DLL paths:

    ```bash
    C:\Program Files (x86)\NVIDIA Corporation\PhysX SDK\PhysX-3.4.2\PhysX_3.4\Bin\vc15win64

    C:\Program Files (x86)\NVIDIA Corporation\PhysX SDK\PhysX-3.4.2\PxShared\bin\vc15win64
    ```

    Once added at the bottom of the list, click OK and close all the control panel windows. This completes the SDK setup.

5. Clone the repository, navigate to the `minigolf/` folder and open the `minigolf.sln` file to load it into Visual Studio 2019. Run the local windows debugger to build and test the project.
