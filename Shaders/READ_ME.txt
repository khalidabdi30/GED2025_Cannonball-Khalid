This is a project for shaders to be placed.

It is only here as a quality of life hack to allow Visual Studio to show shader files in the Solution Explorer 
and thus allow easy editing.

To prevent Visual Studio from trying to compile these shader files as part of this project, select the file's
properties and set "Item Type" to "Does Not Participate in Build".

The shader files themselves are actually loaded and compiled at runtime from within the main Game project library.
