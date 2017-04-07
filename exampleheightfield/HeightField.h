#ifndef HEIGHTFIELD_H
#define HEIGHTFIELD_H

#define W 1280
#define H 720
#define N 128
#define NN (N * N)
#define DRAW_WIDTH 15.0
#define DRAW_HEIGHT 15.0
#define QUOTE_E(x) # x
#define QUOTE(x) QUOTE_E(x)

#define ROXLU_USE_OPENGL
#define ROXLU_USE_MATH
#define ROXLU_USE_PNG
#include <tinylib.h>
#include <vector>

// Diffuses the height field
static const char* HF_DIFFUSE_VERT = ""
  "#version 150\n"
  "in ivec2 a_tex;                "
  "uniform sampler2D u_tex_u;     "
  "uniform sampler2D u_tex_v;     "
  "const float dt = 0.16;        "
  "out float v_new_u_value;       "
  "out float v_new_v_value;       "
  "void main() {                  "
  "  gl_Position = vec4(-1.0 + float(a_tex.x) * (1.0 / " QUOTE(N) ") * 2.0, -1.0 + float(a_tex.y) * (1.0 / " QUOTE(N) ") * 2.0, 0.0, 1.0);"
  "  float u_center = texelFetch(u_tex_u, ivec2(a_tex.s + 0, a_tex.t + 0), 0).r;"
  "  float u_left   = texelFetch(u_tex_u, ivec2(a_tex.s - 1, a_tex.t + 0), 0).r;"
  "  float u_right  = texelFetch(u_tex_u, ivec2(a_tex.s + 1, a_tex.t + 0), 0).r;"
  "  float u_top    = texelFetch(u_tex_u, ivec2(a_tex.s + 0, a_tex.t - 1), 0).r;"
  "  float u_bottom = texelFetch(u_tex_u, ivec2(a_tex.s + 0, a_tex.t + 1), 0).r;"
  "  float f = 8.4 * ((u_right + u_left + u_bottom + u_top) - (4.0 * u_center));"
  "  { "
  "    float max = 0.3;"
  "    if(f > max) { f = max; } else if( f < -max) { f = -max; } "
  "  } "
  "  float v = texelFetch(u_tex_v, a_tex, 0).r;"
  "  v_new_v_value = (v + f * dt); "
  "  v_new_u_value = u_center + v_new_v_value * dt;"
  "  v_new_v_value = v_new_v_value * 0.97;"
  "}"
  "";

static const char* HF_DIFFUSE_FRAG = ""
  "#version 150\n"
  "out vec4 v_out;     "
  "out vec4 u_out;     "
  "in float v_new_u_value; "
  "in float v_new_v_value; "
  "void main() {"
  "  v_out = vec4(v_new_v_value);"
  "  u_out = vec4(v_new_u_value);"
  "}"
  "";

// Renders the height field
// -----------------------------------------------------------

static const char* HF_RENDER_VS = ""
  "#version 150\n"
  "uniform sampler2D u_tex_u;"  
  "uniform sampler2D u_tex_norm;"
  "uniform mat4 u_pm;"
  "uniform mat4 u_vm;"

  "in ivec2 a_tex;"
  "out vec3 v_norm;"

  "const float size_y = " QUOTE(DRAW_WIDTH) ";"
  "const float size_x = " QUOTE(DRAW_HEIGHT) ";"
  "const float step_y = size_y / " QUOTE(N) ";"
  "const float step_x = size_x / " QUOTE(N) ";"
  "const float hx = size_x * 0.5;"
  "const float hy = size_y * 0.5;"
  
  "void main() {"
  "  float height = texelFetch(u_tex_u, a_tex, 0).r;"
  "  vec4 v = vec4(-hx + a_tex.s * step_x, height, -hy + a_tex.t * step_y, 1.0);"
  "  gl_Position = u_pm * u_vm * v;"
  "  v_norm = texelFetch(u_tex_norm, a_tex, 0).rgb;"
  "}"
  "";

static const char* HF_RENDER_FS = ""
  "#version 150\n"

  "in vec3 v_norm;"
  "out vec4 fragcolor;"
  
  "void main() {"
  "  fragcolor = vec4(1.0, 0.0, 0.0, 1.0);"
  "  fragcolor.rgb = 0.5 + 0.5 * v_norm;"
  "}"
  "";

// Normals
// -----------------------------------------------------------
static const char* HF_NORMALS_VS = ""
  "#version 150\n"
  "uniform sampler2D u_tex_u;"
  "in ivec2 a_tex;"
  "out vec3 v_norm;"
  "const float size_y = " QUOTE(DRAW_WIDTH) ";"
  "const float size_x = " QUOTE(DRAW_HEIGHT) ";"
  "const float step_y = size_y / " QUOTE(N) ";"
  "const float step_x = size_x / " QUOTE(N) ";"
  "const float hx = size_x * 0.5;"
  "const float hy = size_y * 0.5;"
  "const float step_size = 2.0 * (1.0 / " QUOTE(N) ");"

  "void main() {"
  "  gl_Position = vec4(-1.0 + float(a_tex.s) * step_size, -1.0 + a_tex.t * step_size, 0.0, 1.0);"
  "  v_norm = vec3(0.0, 1.0, 0.0);"
  "  float current_height = texelFetch(u_tex_u, ivec2(a_tex.s + 0, a_tex.t + 0), 0).r;"
  "  float right_height   = texelFetch(u_tex_u, ivec2(a_tex.s + 1, a_tex.t + 0), 0).r;"
  "  float top_height     = texelFetch(u_tex_u, ivec2(a_tex.s + 0, a_tex.t - 1), 0).r;"
  "  vec3 current_pos     = vec3(-hx + (a_tex.s + 0) * step_x, current_height, -hy + a_tex.t * step_y + 0);"
  "  vec3 right_pos       = vec3(-hx + (a_tex.s + 1) * step_x, right_height,   -hy + a_tex.t * step_y + 0);"
  "  vec3 top_pos         = vec3(-hx + (a_tex.s + 0) * step_x, top_height,     -hy + a_tex.t * step_y - 1);"
  "  vec3 to_right        = right_pos - current_pos; "
  "  vec3 to_top          = top_pos - current_pos;"
  "  v_norm = normalize(cross(to_right, to_top));"
  "}"
  "";

static const char* HF_NORMALS_FS = ""
  "#version 150\n"
  "in vec3 v_norm;"
  "out vec4 norm_out;"

  "void main() {"
  "  norm_out = vec4(v_norm, 1.0);"
  "}"
  "";

// Debug drawing
// -----------------------------------------------------------

static const char* HF_DEBUG_FLOAT_VS = ""
  "#version 150\n"
  "uniform mat4 u_pm;"
  "uniform mat4 u_mm;"

  "const vec2 verts[4] = vec2[]("
  "  vec2(-1.0,  1.0),   "
  "  vec2(-1.0, -1.0),   "
  "  vec2( 1.0,  1.0),   "
  "  vec2( 1.0, -1.0)    "
  ");"
  
  "const vec2 tex[4] = vec2[]("
  "  vec2(0.0, 1.0), " 
  "  vec2(0.0, 0.0), "
  "  vec2(1.0, 1.0), "
  "  vec2(1.0, 0.0)"
  ");"

  "out vec2 v_tex;"

  "void main() {"
  "  vec4 vert = vec4(verts[gl_VertexID], 0.0, 1.0);"
  "  gl_Position = u_pm * u_mm * vert;"
  "  v_tex = tex[gl_VertexID];"
  "}"
  "";

static const char* HF_DEBUG_FLOAT_FS = ""
  "#version 150\n"
  "uniform sampler2D u_tex;"
  "in vec2 v_tex;"
  "out vec4 fragcolor;"
  "void main() {"
  "  fragcolor.rgb = texture(u_tex, v_tex).rgb;"
  "  fragcolor.a = 0.0;"
  "}"
  "";

struct FieldVertex {
  FieldVertex(){ tex[0] = 0; tex[1] = 0; }
  FieldVertex(int i, int j) { tex[0] = i; tex[1] = j; }
  void set(int i, int j) { tex[0] = i; tex[1] = j; }
  GLint tex[2];
};

class HeightField {

 public:
  HeightField();
  bool setup();
  void diffuse();             /* diffuse step */
  void calculateNormals();    /* after calling diffuse() you need to diffuse the normals */
  void drawTexture(GLuint tex, float x, float y, float w, float h);
  void render();

 public:

  /* Shared */
  GLuint vao;
  GLuint vbo_els;
  std::vector<GLint> indices;

  /* Diffuse the height field */
  GLuint vert_diffuse;
  GLuint frag_diffuse;
  GLuint prog_diffuse;
  GLuint vbo_diffuse;    /* contains the FieldVertex data that is used to sample from the correct location in the shader */
  GLuint fbo_diffuse;
  GLuint tex_u0;
  GLuint tex_u1;
  GLuint tex_v0;
  GLuint tex_v1;
  GLuint tex_norm;      /* contains the normals of the height field */
  int state_diffuse;    /* toggles between 0 and 1 to ping/pong the read/write buffers */

  /* Debug drawing */
  GLuint vao_draw;      /* vao used to draw attribute less vertices for a texture */
  GLuint vert_draw;
  GLuint frag_draw;
  GLuint prog_draw;

  /* Normal shader */
  GLuint vert_norm;
  GLuint frag_norm;
  GLuint prog_norm;

  /* Draw the height field */
  GLuint prog_render;
  GLuint vert_render;
  GLuint frag_render;

  mat4 pm;
  mat4 vm;
};

#endif
