# The-Frim

A Story focused 3D-Voxel (minecraft clone) game completely written from *scratch**.

# Building

The game can be build with **Cmake** the main CMakeLists.txt is in the `src/` directory.

The project is **cross-compatible** with Linux and Windows; I personally used clang-16 on std::C++20

```cmake
cmake -B build
cmake --build build
```

Then copy the `Assets` folder into the `build` folder.

# Libraries and Attributions

[The OpenGL Spec](https://www.opengl.org/)

[GLFW](https://www.glfw.org/)

[MSGPack](https://msgpack.org/index.html)

[MSGPack-c](https://github.com/msgpack/msgpack-c)

[Glad](https://github.com/Dav1dde/glad)

[Assimp](https://github.com/assimp/assimp)

[BS::Thread_Pool](https://github.com/bshoshany/thread-pool)
