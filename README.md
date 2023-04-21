# GLSL-Include

**OpenGL** doesn't have a standard way to manage include/import statements in **glsl shader files**. But if you are reading this, you probably have some utility functions, that you'd like to use in multiple shader files and as a programmer, code repetition - hopefully - bothers you. This small library is here to address this inconvenience. It provides simple C-like **#include** capability for GLSL shader files (or any text files really).

## Why does this exist?

Well, a simple include implementation with file loading and text replacement is something, that probably anyone could do... but no one really wants to deal with it, error handling and paths can get a bit messy, if you want to be safe. To me at least this was a "nice to have" feature, that I didn't touch for a long time.

But one day I thought I'd do it, and while I was searching through the internet I didn't find a solution, that I felt convinient enough, so I though I'd implement this and might as well share it. So if you don't want to mess with this on your own, you might save an hour or two, that's all.

## Features

 This library is capable of:
- **Loading shader files**
- Finding **custom include keywords** and replace them by
- **Inserting the contents of** the corresponding **file**
- **Include** more **files recursively**
- Handling relative paths in different environments
- Addressing the issues of accidental **self includes**, **duplicate includes** and **circular dependencies**
- Warning you about **missing files**

## Example usage

Simply include the **single header** file in your project, create a ShaderLoader instance, choose your keyword and load any number of files by calling the provided function:

```cpp
#include "glsl_include.hpp"
```
```cpp
glsl_include::ShaderLoader sl = glsl_include::ShaderLoader("//#include");
std::string shader_source_code1 = sl.load_shader("/path/to/your_shader.glsl");
std::string shader_source_code2 = sl.load_shader("/path/to/your_next_shader.glsl");
```

Rules of your include statement:
- Your custom keyword must be at the start of the line
- Your custom keyword must be followed by a path enclosed in double quotes
- Given path will be relative to the original files path

Side notes:
- I settled on using `//#include` as my custom keyword, because it resembles C, but writing it as a comment made some syntax highlighters stop complaining about it.
- For any hardcore Windows users out there: You can use single backslash in the shader include statements, it should not cause trouble.

### Example source code and output

*/path/to/your_shader.glsl*
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Including myself by accident
//#include "your_shader.glsl"
// Including myself by accident

//#include "common.glsl"

// Duplicate include avoided
//#include "your_dir/functions.glsl"
// Duplicate include avoided

// Missing include
//#include "missing_file.glsl"
// Missing include

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

*/path/to/common_structs.glsl*
```glsl
//###################
//### common.glsl ###
//###################

//#include "your_dir\functions.glsl"

// Circular dependency avoided
//#include "your_shader.glsl"
// Circular dependency avoided

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};  

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
//######################
```

*/path/to/your_dir/functions.glsl*
```glsl
//######################
//### functions.glsl ###
//######################

vec3 my_funcion1(in vec3) {
    //...
}

vec3 my_funcion2(in vec3) {
    //...
}

vec3 my_funcion3(in vec3) {
    //...
}
//######################
```

*String returned from load_shader()*
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Including myself by accident
// Including myself by accident

//###################
//### common.glsl ###
//###################

//######################
//### functions.glsl ###
//######################

vec3 my_funcion1(in vec3) {
    //...
}

vec3 my_funcion2(in vec3) {
    //...
}

vec3 my_funcion3(in vec3) {
    //...
}
//######################


// Circular dependency avoided
// Circular dependency avoided

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};  

struct DirLight {
        vec3 direction;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
};
//######################


// Duplicate include avoided
// Duplicate include avoided

// Missing include

// Missing include

void main()
{
        gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

*Terminal output*
```
WARNING [ ShaderLoader::load_shader ]: '/path/to/your_shader.glsl' tried to include itself
ERROR [ ShaderLoader::load_shader ]: Failed to start fstream, check if '/path/to/missing_file.glsl' exists
```

## Build & Requirements
No special build process required, just include *glsl_include.hpp* in your project.

No dependencies, the only requirement is **C++11 standard**.

Tested on:
```
Ubuntu 18.04
GCC 7.5.0, GCC 8.4.0

Windows 11
Microsoft Visual C++ 2022
```
If anyone feels like verifying builds on other OS-es or by using different compilers, I can include those here as well.


## Development
For now, I consider this library mostly finished, so my only plans are:
- Maintenance, bug fixes
- Test more OS-s, compilers

## Contribution

If you find any errors or have an idea to improve this small library, please open an **Issue**. Help is appreciated.