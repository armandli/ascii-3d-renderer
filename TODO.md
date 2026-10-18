# TODO — ASCII 3D Renderer (C++ port)

Step-by-step plan for implementing the pipeline from
[kciter/ascii-3d-renderer.js](https://github.com/kciter/ascii-3d-renderer.js) and the
[blog post](https://kciter.so/posts/ascii-3d-renderer/en/). Each phase ends in a state you can
**build and test**. Every stage sits behind a small interface, so you can swap one implementation
for another (for example, a wireframe rasterizer for the filled one).

---

## 0. Pipeline overview

```
 Mesh (model space, Vector3)
   │  MeshSource ............ OBJ loader / primitive generator
   ▼
 [Model matrix]  Object::model_matrix()          model → world
   ▼
 [View matrix]   Camera::view_matrix()           world → view (camera at origin, looking +Z)
   ▼
 [Face culling]  FaceCuller (optional)           drop back faces
   ▼
 [Lighting]      LightingModel                   view-space normal + light → intensity [0,1]
   ▼
 [Projection]    Projection::matrix()            view → clip (Vector4, w = z_view)
   ▼
 [Clipping]      Clipper (optional)              near-plane clip, so w > 0 always holds
   ▼
 [Persp. divide] clip → NDC  (x/w, y/w, z/w)
   ▼
 [Viewport]      Viewport::to_screen()           NDC [-1,1] → character cells (y flipped)
   ▼
 [Rasterizer]    Rasterizer                      triangle → fragments (x, y, depth, barycentrics)
   ▼
 [Depth test]    DepthBuffer                     keep the nearest fragment
   ▼
 [Glyph mapping] GlyphMapper                     intensity → character from a ramp
   ▼
 [Framebuffer]   Framebuffer                     grid of glyphs
   ▼
 [Present]       Presenter                       std::string / ANSI terminal / file
```

The reference does all of this in one `ASCII3DRenderer.process()` method. Here, each arrow is a
separate, unit-testable module.

---

## 1. Conventions to decide first (write them in `docs/CONVENTIONS.md`)

- [x] **Vector convention.** **Column vectors**: `v' = M * v`, with translation in column 3
      (`m[0..2][3]`), and composes as `A * B * v` (B is applied first). Storage is row-major
      (`m[row][col]`). The reference uses row vectors (`v * M`), so every reference matrix is the
      **transpose** of ours. Document this in `matrix.h` and `vector.h`.
- [x] **Handedness / view space.** **Left-handed** (Direct3D style): x right, y up, and the camera
      looks down **+Z**, so visible points have `z_view > 0` and clip `w = z_view`. The reference
      is right-handed with `w = -z_view`, so numbers do not match it one to one; compare rendered
      frames, not matrices.
- [x] **Angles** are in radians everywhere except `Projection::fov_degrees`.
- [x] **Screen space.** `x` ∈ [0, width) goes right and `y` ∈ [0, height) goes down. Sample at
      pixel centers `(x + 0.5, y + 0.5)` (the reference samples at integer coordinates).
- [x] **Depth.** Depth is `z_view` (= clip `w`). Smaller is closer. Clear value is `+∞` (the
      reference clears to `255`). The depth test passes on strict `<`, so equal depth is rejected
      (the reference uses `≤`).
- [x] **Winding.** Front faces are **clockwise as seen from the camera**. The face normal is
      `cross(v2 - v1, v3 - v1)` and points toward the viewer for front faces (outward on a closed
      mesh). The viewport's y flip reverses the sign of the screen-space signed area, so pin down
      which sign front faces get with a test. The reference OBJ assets are probably wound
      counter-clockwise (OpenGL style); check against the cube OBJ and flip on load if needed.
- [x] **Terminal cell aspect.** Characters are about 2× taller than wide, so
      `aspect = (width / cell_aspect) / height` with `cell_aspect = 2.0` by default. Make it
      configurable.
- [x] **Float tolerance** for tests: `kEps = 1e-5f`. Add `EXPECT_VEC3_NEAR` /
      `EXPECT_MAT44_NEAR` helpers in `test/test_helpers.h`.

### Known quirks in the reference (do NOT copy blindly)

| Reference behaviour | Problem | Plan |
|---|---|---|
| `Camera.transform` builds rows `[right, up, forward, eye]` and multiplies directly | This is the camera-to-world matrix, not its inverse. Translation is `+eye` instead of `-eye`, and with the default vectors the image is rotated 90°. | Implement a proper `look_at` (step 4) and test it. |
| `newUp = forward × up` and then `right = newUp × forward` | The names are swapped, which causes the 90° roll above. | Left-handed: `right = normalize(cross(up, forward))`, `up' = cross(forward, right)` |
| `Object.transform` uses `Rx·Ry·Rz·S·T` | Scale is applied after rotation, which shears when the scale is non-uniform. | Use `T·Rz·Ry·Rx·S` (column-vector order: S applied first) |
| Row vectors, right-handed, camera looks down `-Z`, `w = -z_view` | Differs from our conventions (section 1). | Transpose reference matrices, negate reference `z` positions, and flip asset winding if needed. |
| The matrix is rebuilt for **every vertex** | Wastes work. | Build the MVP once per object per frame. |
| Depth is one value per triangle: `(w1 + w2 + w3) / 3` | Intersecting or large triangles sort incorrectly. | Start with flat depth to match the reference, then interpolate per pixel (step 13). |
| No near-plane clipping | Vertices behind the camera (`w ≤ 0`) produce garbage. | Add a clipper (step 13). |
| No back-face culling (back faces are drawn with brightness 0, which is `'.'`) | Wasted fill, and dots show through gaps. | Add an optional `FaceCuller` (step 13). |
| Framebuffer is `(h + 1) × (w + 1)` | Off-by-one padding. | Use exactly `w × h`. |
| OBJ parser reads only the first 3 indices of each `f` line | Quads and n-gons lose triangles. | Fan-triangulate (step 2). |
| `·┼╬░▒▓█` ramp in `char` | These are multi-byte UTF-8 characters. | Store `char32_t` in the framebuffer and UTF-8-encode in the presenter. |

---

## 2. Module interfaces (target design)

Value types (math, geometry) are plain structs with free functions. Pipeline **stages** are
abstract interfaces with one default implementation each, so you can swap them at runtime. If
you later want zero-cost dispatch, add a C++20 concept that mirrors each interface.

```cpp
// ---- geometry -------------------------------------------------------------
struct Triangle      { std::array<Vector3, 3> v; };            // (existing Polygon)
struct Mesh          { std::vector<Triangle> triangles; };
struct ClipVertex    { Vector4 pos; };                         // after projection, before divide
struct ScreenVertex  { float x, y, z, inv_w; };                // after viewport
struct ScreenTriangle{ std::array<ScreenVertex, 3> v; };
struct Fragment      { int x, y; float depth; Vector3 bary; };

// ---- sources ----------------------------------------------------------------
class MeshSource {                       // OBJ file, OBJ string, procedural cube/torus...
public:
  virtual ~MeshSource() = default;
  virtual std::expected<Mesh, std::string> load() = 0;
};

// ---- transforms (pure, no state beyond parameters) ---------------------------
Matrix44 Object::model_matrix() const;
Matrix44 Camera::view_matrix() const;
Matrix44 Projection::matrix() const;
struct Viewport { int width, height; Vector3 to_screen(Vector3 ndc) const; };

// ---- optional stages ---------------------------------------------------------
class FaceCuller { public: virtual ~FaceCuller() = default;
  virtual bool keep(const std::array<Vector3, 3>& view_space) const = 0; };
class Clipper    { public: virtual ~Clipper() = default;
  virtual void clip(const std::array<ClipVertex, 3>&, std::vector<std::array<ClipVertex, 3>>& out) const = 0; };

// ---- shading -------------------------------------------------------------------
struct ShadingInput { Vector3 normal_view; Vector3 position_view; };
class LightingModel { public: virtual ~LightingModel() = default;
  virtual float intensity(const ShadingInput&) const = 0; };      // returns [0, 1]
class GlyphMapper   { public: virtual ~GlyphMapper() = default;
  virtual char32_t glyph(float intensity) const = 0; };

// ---- raster ----------------------------------------------------------------------
using FragmentSink = std::function<void(const Fragment&)>;        // or std::function_ref if available
class Rasterizer { public: virtual ~Rasterizer() = default;
  virtual void rasterize(const ScreenTriangle&, int width, int height, const FragmentSink&) const = 0; };

// ---- targets -----------------------------------------------------------------------
class Framebuffer {          // glyph grid + depth buffer, no knowledge of the pipeline
public:
  Framebuffer(int width, int height);
  void clear(char32_t bg = U' ');
  bool depth_test_and_set(int x, int y, float depth, char32_t glyph);
  char32_t at(int x, int y) const;
  std::string to_utf8() const;     // rows joined with '\n'; the basis for golden tests
};
class Presenter { public: virtual ~Presenter() = default;
  virtual void present(const Framebuffer&) = 0; };

// ---- orchestration -------------------------------------------------------------------
struct RenderPipeline {      // each stage is replaceable; nullptr means "stage disabled"
  std::unique_ptr<FaceCuller>    culler;
  std::unique_ptr<Clipper>       clipper;
  std::unique_ptr<LightingModel> lighting;
  std::unique_ptr<Rasterizer>    rasterizer;
  std::unique_ptr<GlyphMapper>   glyphs;
};
class Renderer {
public:
  explicit Renderer(RenderPipeline);
  void render(const Scene&, const Camera&, const Projection&, Framebuffer&) const;
};
```

Mapping to the current stubs: `Polygon` becomes `Triangle` (or keep the name), and the
`Framebuffer::chars` field changes from `std::vector<char>` to `std::vector<char32_t>`. `Shader`
splits into `LightingModel` and `GlyphMapper`, so update `test/smoke_test.cpp` to match.

---

## 3. Implementation steps

Each step lists its files, the work to do, and the **tests that must pass before you move on**.
Use one test file per module (`test/<module>_test.cpp`) and add each to `test/CMakeLists.txt`.

### Step 0 — Test scaffolding
- [x] `test/test_helpers.h`: `kEps`, `EXPECT_VEC2/3/4_NEAR`, `EXPECT_MAT44_NEAR`.
- [x] `test/golden/` directory and a `expect_golden(name, actual_string)` helper. On mismatch,
      it writes `<name>.actual.txt` for diffing. Set `ASCII3D_UPDATE_GOLDEN=1` to regenerate.
- [x] Optional: add a `ASCII3D_SANITIZE` CMake option (ASan/UBSan) for the Debug + test build.
- **Test:** `make test` stays green with an empty helper test.

### Step 1 — Math: vectors (`src/math/vector.h`)
- [ ] `operator+ - *(scalar) /(scalar)`, unary `-`, `==` for Vector2/3/4.
- [ ] `dot`, `cross` (Vector3), `length`, `length_squared`, `normalize`. Decide how to handle a
      zero-length vector: return a zero vector, never NaN.
- [ ] `Vector4 to_vec4(Vector3, float w = 1)`, `Vector3 xyz(Vector4)`.
- [ ] Mark everything `constexpr` / `[[nodiscard]]` where possible.
- **Tests:** algebraic identities (`cross(x̂, ŷ) == ẑ`, `dot(a, cross(a, b)) == 0`, normalized
  length is 1, normalizing a zero vector gives no NaN), plus `static_assert` checks for constexpr.

### Step 2 — Math: matrices (`src/math/matrix.h`)
- [ ] `identity()`, `operator*(Matrix44, Matrix44)`, `operator*(Matrix44, Vector4)` (column
      vector: `M * v`), `transpose`.
- [ ] Factories: `translate(Vector3)` (translation in column 3), `scale(Vector3)`,
      `rotate_x/y/z(radians)`. These are the **transposes** of the layouts in reference
      `matrix44.ts`.
- [ ] Optional: `inverse()` (affine is enough). It is useful for testing `view_matrix`.
- **Tests:** identity is neutral; composition order (`translate(t) * rotate_z(a)` rotates first,
  and differs from `rotate_z(a) * translate(t)`); `rotate_z(π/2) * x̂ == ŷ` (pin down the sign);
  `translate` leaves `w = 0` direction vectors unchanged; `M * inverse(M) ≈ I`.

### Step 3 — Geometry and mesh loading (`src/scene/polygon.h`, `mesh.*`, new `src/io/obj_loader.*`)
- [ ] `Mesh` with `triangles`, plus an optional `bounds()` that returns an AABB (handy for
      auto-framing).
- [ ] `ObjLoader : MeshSource`. Parse from `std::istream` (and from a string or file path through
      wrappers). Handle `v`, `f` with `i`, `i/t`, `i/t/n`, and `i//n` forms, negative (relative)
      indices, and fan-triangulation of n-gons. Ignore `vn/vt/o/g/s/#/usemtl` for now. Return
      `std::expected` errors for bad indices or malformed lines (include the line number).
- [ ] Procedural sources: `make_cube()`, `make_torus(R, r, segs, rings)`. This lets you test the
      pipeline without asset files. Emit triangles clockwise as seen from outside.
- [ ] `ObjLoader` option to reverse winding (swap `v2`/`v3`), for assets wound counter-clockwise.
- [ ] Copy the reference OBJ meshes (cube, donut, teapot, ship, rocket, cow) into `assets/*.obj`.
      They are MIT-licensed, so keep the attribution.
- **Tests:** an inline 1-triangle OBJ parses correctly; a quad gives 2 triangles; `1/2/3` and
  `1//3` index forms work; a bad index returns an error; the cube OBJ has 12 triangles; the cube
  bounds are [-1, 1]³; the torus triangle count is `2·segs·rings`; every triangle of the loaded
  cube OBJ and of `make_cube()` has `cross(v2 - v1, v3 - v1)` pointing outward (positive dot
  product with its centroid).

### Step 4 — Model transform (`src/scene/object.*`)
- [ ] `Object { Mesh mesh; Vector3 position, rotation, scale; Matrix44 model_matrix() const; }`
      composed as `T · Rz · Ry · Rx · S` (column-vector order: scale, then Rx, Ry, Rz, then
      translate).
- [ ] Animation stays **out of** `Object`. Use `std::function<void(Object&, double dt)> update`
      or a separate `Animator` (see step 11).
- **Tests:** a default object gives the identity matrix; position only translates the origin;
  `scale = 2` doubles vertex distances; rotation is applied before translation (the origin stays
  at `position`).

### Step 5 — Camera / view transform (`src/scene/camera.*`)
- [ ] Rename the members to be explicit: `eye`, `target` (the reference uses `look` as the
      target), and `up`.
- [ ] `view_matrix()` = proper left-handed look-at in column-vector form. Compute
      `f = normalize(target - eye)`, `r = normalize(cross(up, f))`, and `u = cross(f, r)`. The
      rows are `r`, `u`, and `f`, and the translation column is
      `(-dot(r, eye), -dot(u, eye), -dot(f, eye))`.
- **Tests:** `target` maps to `(0, 0, +d)`; `eye` maps to the origin; a point to the camera's
  right has `x > 0` and a point above it has `y > 0` (this test catches the reference's 90° bug);
  `view * inverse(view) ≈ I`.

### Step 6 — Projection and perspective divide (`src/scene/projection.*`)
- [ ] `matrix()`: left-handed, column-vector perspective with NDC z in [-1, 1]. This differs
      from reference `projection.ts` (right-handed, row vectors). With `g = 1 / tan(fov / 2)`,
      near `n` and far `f`, the rows are `[g / aspect, 0, 0, 0]`, `[0, g, 0, 0]`,
      `[0, 0, (f + n) / (f - n), -2fn / (f - n)]`, `[0, 0, 1, 0]`.
- [ ] `aspect_for(width, height, cell_aspect = 2.0f)` helper.
- [ ] A free function `perspective_divide(Vector4 clip) -> Vector3 ndc`, kept separate from the
      matrix so a clipper can run between the two.
- **Tests:** a point on the near plane gives `z_ndc = -1` and one on the far plane gives `+1`;
  `w_clip == z_view`; a point on the frustum's top edge gives `y_ndc = 1`; changing `aspect`
  scales only x.

### Step 7 — Viewport (`src/render/viewport.h`, new)
- [ ] `to_screen(ndc)`: `x = (ndc.x + 1) · W / 2` and `y = (1 - ndc.y) · H / 2`. Keep `z` and pass
      `inv_w` through for later perspective-correct interpolation.
- **Tests:** NDC `(-1, 1)` maps to `(0, 0)`, `(1, -1)` maps to `(W, H)`, and `(0, 0)` maps to the
  center.

### Step 8 — Framebuffer and presenter (`src/render/framebuffer.*`, new `src/render/presenter.*`)
- [ ] `Framebuffer(width, height)`, `clear` (depth reset to `+∞`), `depth_test_and_set` (passes
      on strict `<`), `at`, and `to_utf8`. Out of range writes are ignored, not undefined
      behaviour.
- [ ] `StringPresenter` captures the frame into a string, for tests.
- [ ] `TerminalPresenter`: on start, switch to the alternate screen (`\x1b[?1049h`) and hide the
      cursor (`\x1b[?25l`). Each frame, move the cursor home (`\x1b[H`) and write all rows in one
      `fwrite`. On exit or SIGINT, restore the terminal.
- [ ] `terminal_size()` via `ioctl(TIOCGWINSZ)`, falling back to 80×24.
- **Tests:** a closer depth wins; a farther depth is rejected; equal depth is rejected (strict
  `<`; the reference uses `≤`); any finite depth passes after `clear`; out-of-range writes are
  no-ops; `to_utf8` encodes `'█'` correctly
  and has exactly H lines of W glyphs.
- **Visual checkpoint:** `ascii3d_demo --test-pattern` fills a gradient with the ramp and
  presents it.

### Step 9 — Rasterizer (`src/render/rasterizer.*`)
- [ ] `edge(a, b, p) = (p.x - a.x)(b.y - a.y) - (p.y - a.y)(b.x - a.x)`. This is the reference
      `sign` function.
- [ ] `BarycentricRasterizer : Rasterizer`. Clamp the bounding box to the viewport, sample each
      pixel center `(x + 0.5, y + 0.5)`, accept either winding (as the reference does) or only one, and emit a
      `Fragment` with normalized barycentrics and interpolated depth.
- [ ] Skip degenerate triangles (area ≈ 0).
- [ ] `WireframeRasterizer : Rasterizer` (DDA or Bresenham, like the reference `drawLine`) to
      prove the interface is replaceable.
- [ ] Later: a top-left fill rule so that shared edges are not drawn twice.
- **Tests:** a right triangle covering half of a 4×4 grid emits the expected exact cell set
  (golden); both windings give the same coverage; a triangle fully outside the viewport emits
  nothing; a partially outside triangle is clipped to the bounds; the barycentrics at each emitted
  fragment sum to 1; a degenerate triangle emits nothing.

### Step 10 — Shading (`src/render/shader.*` → `lighting.*` + `glyph_mapper.*`)
- [ ] `face_normal(a, b, c) = normalize(cross(b - a, c - a))`, which points toward the viewer
      for clockwise front faces (section 1).
- [ ] `LambertLighting : LightingModel` with a configurable `light_dir` (the direction toward the
      light). The default is the headlight `(0, 0, -1)` in view space, pointing back at the
      camera; the reference uses `(0, 0, 1)` because it looks down `-Z`. Add an `ambient` term (default 0
      to match the reference). Compute `intensity = clamp(ambient + max(0, n·l), 0, 1)`.
- [ ] `RampGlyphMapper : GlyphMapper` taking `std::u32string_view`. Look up
      `ramp[round(i · (len - 1))]`. Provide presets `kRampAscii = U".;ox%@"` and
      `kRampBlocks = U"·┼╬░▒▓█"`, plus a longer ramp (for example ``.'`^",:;Il!i><~+_-?][}{1)(|\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$``).
- **Tests:** a face toward the light gives intensity 1; a perpendicular face gives 0; a back face
  gives 0 (clamped); intensity 0 maps to `ramp.front()` and 1 maps to `ramp.back()`; out-of-range
  input is clamped.

### Step 11 — Pipeline assembly (`src/render/renderer.*`, new `src/scene/scene.h`)
- [ ] `Scene { std::vector<Object> objects; }`.
- [ ] `Renderer::render`: for each object, build `MV = view * model` and `P` once
      (so `MVP = P * view * model`). For each
      triangle: transform to view space, cull (optional), compute intensity, project, clip
      (optional), divide, apply the viewport, rasterize, depth-test, map to a glyph, and write.
- [ ] Depth policy: start with the reference's flat average `w` to get matching output, then add
      a `DepthMode { FlatAverage, Interpolated }` switch.
- [ ] Keep `Renderer` free of terminal I/O: it writes only to a `Framebuffer`.
- **Tests (golden):** a single front-facing triangle centered in 20×10 cells; the unit cube at
  `z = 3` rotated `(0.4, 0.6, 0)` in 40×20 cells with the ASCII ramp; an empty scene gives a blank
  frame; an object behind the camera gives a blank frame (and no crash). Also test pipeline
  invariants with mock stages: a counting `Rasterizer` is called once per triangle, and a
  `FaceCuller` that rejects everything gives a blank frame.
- **Visual checkpoint:** `ascii3d_demo --still cube` prints one frame.

### Step 12 — Animation loop and demo app (`src/app/`, `src/main.cpp`)
- [ ] `FrameClock` (`std::chrono::steady_clock`) with a target FPS that sleeps for the rest of the
      frame budget and reports `dt`.
- [ ] `Animator`: `std::function<void(Object&, double t, double dt)>`. Add a `spin(rx, ry, rz)`
      preset matching the reference (`angle += 0.007` per frame, with `rot = (-2a, -2a, -a)`).
      Make it time-based rather than frame-based.
- [ ] `App` owns the Scene, Camera, Projection, Renderer, Framebuffer, and Presenter. It handles
      terminal resize (SIGWINCH): it reallocates the framebuffer and updates the aspect.
- [ ] CLI: `--model <name|path.obj>`, `--ramp ascii|blocks|long`, `--fps N`, `--size WxH`,
      `--frames N` (render N frames then exit, which is useful for CI), and `--wireframe`.
- [ ] Use the reference's default placements with `z` negated for the +Z camera: cube
      `(0.5, 0.5, 3)`, donut `(0, 0.5, 1)`, teapot `(0, 1, 4)`, rocket `(0, 0.5, 2)`, ship
      `(-0.5, 1, 7)`, cow `(0, 1, 5)`. Because the camera now views each mesh from its `-Z` side,
      asymmetric models (teapot, cow) may need a π rotation about y to face the same way as in the
      reference. Adjust these once the camera is fixed, or auto-frame using `Mesh::bounds()`.
- **Tests:** `FrameClock` is tested with an injected fake clock; `ascii3d_demo --frames 3 --size
  40x20` exits with code 0 (add it as a ctest).

### Step 12b — FTXUI display front-end (`src/ui/`, separate `ascii3d_ui` library)
FTXUI is the interactive display layer. It is **only** a `Presenter` and input source: the core
`ascii3d` library must never include or link FTXUI, so the raw-ANSI `TerminalPresenter` and the
string presenter used in tests keep working.
- [ ] CMake: get FTXUI with `FetchContent` (pin a release tag) inside an `ASCII3D_BUILD_UI` option.
      Link `ftxui::screen/dom/component` only into `ascii3d_ui` and the demo.
- [ ] `FramebufferElement`: a custom `ftxui::Node` whose `ComputeRequirement` accepts any size.
      `SetBox` reports the cell box it is given, so the app can resize the `Framebuffer` and
      `Projection::aspect` to fit. `Render(Screen&)` copies glyphs into `screen.PixelAt(x, y)`,
      which is far faster than building one `text()` per row.
- [ ] `FtxuiApp`: `App::Fullscreen()` with a layout of render viewport (`flex`) + side panel
      (model `Menu`, ramp `Radiobox`, FPS / triangle count / frame-ms readout) + status bar.
- [ ] Frame pacing: a background thread ticks at the target FPS and calls
      `screen.PostEvent(Event::Custom)`, which is FTXUI's only thread-safe call. Animate and
      render on the UI thread inside the `Renderer` lambda, so the core needs no locks.
- [ ] Input: arrow keys / WASD orbit the camera, `+`/`-` zoom, space pauses, `w` toggles
      wireframe, `c` toggles culling, `q` quits. These map to plain `AppAction`s handled outside
      FTXUI, so they can be unit-tested.
- [ ] Later: per-cell color (`Pixel::foreground_color`) from intensity or normals, which needs
      a color channel in `Framebuffer`.
- **Tests:** render `FramebufferElement` headless with
  `Screen::Create(Dimension::Fixed(W), Dimension::Fixed(H))` and `screen.ToString()`, and
  compare against the same golden text files as Step 11 (strip the ANSI codes); key event →
  `AppAction` mapping table test.
- **Visual checkpoint:** `ascii3d_demo --ui` shows a spinning teapot with a working model menu.

### Step 13 — Correctness upgrades (each one is a drop-in module)
- [ ] `BackFaceCuller : FaceCuller` (`dot(normal, -position_view) <= 0` → drop it).
- [ ] `NearPlaneClipper : Clipper` (Sutherland–Hodgman against `w > near`, giving 0–2 triangles).
- [ ] Per-pixel depth interpolation (`DepthMode::Interpolated`). Depth is `z_view`, which is not
      linear in screen space: interpolate `inv_w = 1 / z_view` with the barycentrics and invert it.
- [ ] Smooth (Gouraud) shading: parse `vn` or compute averaged vertex normals, then interpolate
      intensity per fragment through the barycentrics. Plan for this in `Fragment` and the
      `FragmentSink`.
- [ ] Top-left fill rule in `BarycentricRasterizer`.
- **Tests:** a culled cube draws at most 6 triangles (count them through a mock rasterizer); a
  triangle straddling the near plane is clipped into valid triangles with `w > 0`; two
  intersecting triangles show the correct crossing line with interpolated depth (golden).

### Step 14 — Performance (optional)
- [ ] Benchmark target (`ascii3d_bench`): teapot or cow at 160×50, measuring ms per frame.
- [ ] Avoid allocations in the hot loop: reuse the clipper output vector, and use
      `std::function_ref` or a template sink instead of `std::function`.
- [ ] Incremental edge functions (step by constant deltas instead of re-evaluating each one).
- [ ] Optional: a `Rasterizer` implementation that uses SIMD or several threads, partitioning the
      screen into tiles.

---

## 4. Suggested file layout after all steps

```
src/
  math/        vector.h  matrix.h
  scene/       polygon.h mesh.{h,cpp} object.{h,cpp} camera.{h,cpp} projection.{h,cpp} scene.h
  io/          obj_loader.{h,cpp} primitives.{h,cpp}
  render/      viewport.h framebuffer.{h,cpp} rasterizer.{h,cpp} lighting.{h,cpp}
               glyph_mapper.{h,cpp} culling.{h,cpp} clipping.{h,cpp} renderer.{h,cpp}
               presenter.{h,cpp}
  app/         frame_clock.{h,cpp} animator.h app.{h,cpp} terminal.{h,cpp}
  ui/          framebuffer_element.{h,cpp} ftxui_app.{h,cpp} actions.{h,cpp}   (links FTXUI)
  main.cpp
assets/        cube.obj donut.obj teapot.obj ship.obj rocket.obj cow.obj
test/          test_helpers.h  golden/*.txt  <module>_test.cpp ...
docs/          CONVENTIONS.md
```

## 5. Definition of done per step
1. Its public header compiles on its own (add an `#include`-only test translation unit).
2. Its unit tests pass under `make test` (and under sanitizers if enabled).
3. It has no dependency on a later stage. The dependency order is `math → scene/io → render →
   app → ui`, and `render` never includes `app`, `ui`, terminal or FTXUI headers.
4. Any alternative implementation can be swapped in through `RenderPipeline` without editing
   `Renderer`.
