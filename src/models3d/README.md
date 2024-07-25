we use only KHR_texture_transform extension, for
"advanced" (it's just more convenient) texture mapping,
which is supported in Blender, where all models
are created.
tinygltf doesn't support it, but we do (small changes to tiny_gltf.h -
see external/README.md in section "tinygltf")

we assume all models are valid (check online gltf validator).
If model is invalid you simply get UB or smt else;