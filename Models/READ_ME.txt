A small utility program for DirectX 11 called meshconvert that converts 3D model files in the obj format 
to a custom binary format used by DirectX called CMO: Compiled Mesh Object.

Place the obj and its associated mtl file in this project's directory: GED2025/Models.

A pre-build event is set up to automatically run meshconvert.exe on all obj files in 
this directory when the project is built.

The converted CMO files will then be placed in the GED2025/Assets directory by a post-build event.

Any textures for the models should be added to the Textures project and converted to dds format.

Note: This project will only build if you specifically tell Visual Studio to build it. 
I.e. right-click on the project in Solution Explorer and select "Build" or "Rebuild."
Do so whenever you add or modify obj model files in this directory.
