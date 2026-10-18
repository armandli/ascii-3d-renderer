#ifndef RENDERER_H
#define RENDERER_H

namespace ascii3d {

// Drives the pipeline: vertex processing (model -> view -> projection),
// rasterization, then fragment shading into the framebuffer.
// TODO: render(scene objects, camera, projection, framebuffer).
struct Renderer {};

}  // namespace ascii3d

#endif  // RENDERER_H
