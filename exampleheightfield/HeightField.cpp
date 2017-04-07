#include "HeightField.h"

HeightField::HeightField() 
  :vert_diffuse(0)
  ,frag_diffuse(0)
  ,prog_diffuse(0)
  ,vao(0)
  ,vbo_els(0)
  ,vbo_diffuse(0)
  ,fbo_diffuse(0)
  ,tex_u0(0)
  ,tex_u1(0)
  ,tex_v0(0)
  ,tex_v1(0)
  ,tex_norm(0)
  ,state_diffuse(0)
  ,vao_draw(0)
  ,vert_draw(0)
  ,frag_draw(0)
  ,prog_draw(0)
  ,prog_render(0)
  ,vert_render(0)
  ,frag_render(0)
  ,vert_norm(0)
  ,frag_norm(0)
  ,prog_norm(0)
{
}

bool HeightField::setup() {

  pm.perspective(60.0f, float(W)/H, 0.01, 100.0);
  //vm.lookAt(vec3(0.0, 15.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
  vm.translate(0.0, -3.0, -5.0);

  // initial data
  float init_v[NN];
  float init_u[NN];
  int upper = N * 0.30;
  int lower = N * 0.10;
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < N; ++j) {
      int dx = j * N + i;
      init_v[dx] = 0.0f;
      init_u[dx] = 0.0f;

      if(i > lower && i < upper && j > lower && j < upper) {
        init_u[dx] = 1.0f;
        init_v[dx] = 0.0f;
      }
    }
  }

  // create diffuse shader
  vert_diffuse = rx_create_shader(GL_VERTEX_SHADER, HF_DIFFUSE_VERT);
  frag_diffuse = rx_create_shader(GL_FRAGMENT_SHADER, HF_DIFFUSE_FRAG);
  prog_diffuse = rx_create_program(vert_diffuse, frag_diffuse);
  glBindAttribLocation(prog_diffuse, 0, "a_tex"); // tex
  glBindFragDataLocation(prog_diffuse, 0, "u_out"); // new velocity value
  glBindFragDataLocation(prog_diffuse, 1, "v_out"); // new height value
  glLinkProgram(prog_diffuse);
  rx_print_shader_link_info(prog_diffuse);
  glUseProgram(prog_diffuse);
  glUniform1i(glGetUniformLocation(prog_diffuse, "u_tex_u"), 0);
  glUniform1i(glGetUniformLocation(prog_diffuse, "u_tex_v"), 1);

  // create draw shader for debugging
  vert_draw = rx_create_shader(GL_VERTEX_SHADER, HF_DEBUG_FLOAT_VS);
  frag_draw = rx_create_shader(GL_FRAGMENT_SHADER, HF_DEBUG_FLOAT_FS);
  prog_draw = rx_create_program(vert_draw, frag_draw);
  glLinkProgram(prog_draw);
  rx_print_shader_link_info(prog_draw);
  glUseProgram(prog_draw);
  glUniform1i(glGetUniformLocation(prog_draw, "u_tex"), 0);

  // create render shader for final result.
  vert_render = rx_create_shader(GL_VERTEX_SHADER, HF_RENDER_VS);
  frag_render = rx_create_shader(GL_FRAGMENT_SHADER, HF_RENDER_FS);
  prog_render = rx_create_program(vert_render, frag_render);
  glBindAttribLocation(prog_render, 0, "a_tex");
  glLinkProgram(prog_render);
  rx_print_shader_link_info(prog_render);
  glUseProgram(prog_render);
  glUniformMatrix4fv(glGetUniformLocation(prog_render, "u_pm"), 1, GL_FALSE, pm.ptr());
  glUniformMatrix4fv(glGetUniformLocation(prog_render, "u_vm"), 1, GL_FALSE, vm.ptr());
  glUniform1i(glGetUniformLocation(prog_render, "u_tex_u"), 0);
  glUniform1i(glGetUniformLocation(prog_render, "u_tex_norm"), 1);

  // create normal shader
  vert_norm = rx_create_shader(GL_VERTEX_SHADER, HF_NORMALS_VS);
  frag_norm = rx_create_shader(GL_FRAGMENT_SHADER, HF_NORMALS_FS);
  prog_norm = rx_create_program(vert_norm, frag_norm);
  glBindAttribLocation(prog_norm, 0, "a_tex");
  glLinkProgram(prog_norm);
  rx_print_shader_link_info(prog_norm);
  glUseProgram(prog_norm);
  glUniform1i(glGetUniformLocation(prog_norm, "u_tex_u"), 0);

  glGenVertexArrays(1, &vao_draw);
  
  // create textures that will hold the height field
  glGenTextures(1, &tex_u0);
  glBindTexture(GL_TEXTURE_2D, tex_u0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, N, N, 0,  GL_RED, GL_FLOAT, init_u);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &tex_u1);
  glBindTexture(GL_TEXTURE_2D, tex_u1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, N, N, 0,  GL_RED, GL_FLOAT, init_u);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &tex_v0);
  glBindTexture(GL_TEXTURE_2D, tex_v0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, N, N, 0,  GL_RED, GL_FLOAT, init_v);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &tex_v1);
  glBindTexture(GL_TEXTURE_2D, tex_v1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, N, N, 0,  GL_RED, GL_FLOAT, init_v);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &tex_norm);
  glBindTexture(GL_TEXTURE_2D, tex_norm);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, N, N, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // fbo 
  glGenFramebuffers(1, &fbo_diffuse);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_diffuse);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_u0,   0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_u1,   0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tex_v0,   0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tex_v1,   0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, tex_norm, 0);
                
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Error: framebuffer is not complete.\n");
    ::exit(EXIT_FAILURE);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // create buffer
  std::vector<FieldVertex> field_vertices(NN, FieldVertex());
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < N; ++j) {
      int dx = j * N + i;
      field_vertices[dx].set(i, j);
    }
  }

  // triangle indices
  for(int i = 1; i < N - 1; ++i) {
    for(int j = 1; j < N - 1; ++j) {
      int a = (j + 0) * N + (i + 0); // bottom left
      int b = (j + 0) * N + (i + 1); // bottom right
      int c = (j + 1) * N + (i + 1); // top right
      int d = (j + 1) * N + (i + 0); // top left
      indices.push_back(a);
      indices.push_back(b);
      indices.push_back(c);

      indices.push_back(a);
      indices.push_back(c);
      indices.push_back(d);
    }
  }

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo_diffuse);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_diffuse);
  glBufferData(GL_ARRAY_BUFFER, sizeof(FieldVertex) * field_vertices.size(), &field_vertices[0].tex[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0); // tex
  glVertexAttribIPointer(0, 2, GL_INT, sizeof(FieldVertex), (GLvoid*)0); // tex

  glGenBuffers(1, &vbo_els);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_els);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indices.size(), &indices[0], GL_STATIC_DRAW);

  printf("height_field.tex_u0: %d\n", tex_u0);
  printf("height_field.tex_u1: %d\n", tex_u1);
  printf("height_field.tex_v0: %d\n", tex_v0);
  printf("height_field.tex_v1: %d\n", tex_v1);
  printf("height_field.tex_norm: %d\n", tex_norm);
    
  return true;
}

void HeightField::drawTexture(GLuint tex, float x, float y, float w, float h) {

  mat4 pm;
  mat4 mm;
  float hw = w * 0.5;
  float hh = h * 0.5;
  pm.ortho(0, W, H, 0, 0.0f, 100.0f);
  mm.translate(x + hw, y + hh, 0.0f);
  mm.scale(hw, hh, 1.0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  
  glBindVertexArray(vao_draw);
  glUseProgram(prog_draw);
  glUniformMatrix4fv(glGetUniformLocation(prog_draw, "u_pm"), 1, GL_FALSE, pm.ptr());
  glUniformMatrix4fv(glGetUniformLocation(prog_draw, "u_mm"), 1, GL_FALSE, mm.ptr());
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Renders the calculated heights/normls/etc.. to screens
void HeightField::render() {
  glUseProgram(prog_render);

  glEnable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_u0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, tex_norm);

  glBindVertexArray(vao);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
}

// Diffuses the heights
void HeightField::diffuse() {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_diffuse);
  glViewport(0, 0, N, N);
  glUseProgram(prog_diffuse);
  glBindVertexArray(vao);

  state_diffuse = 1 - state_diffuse;

  if(state_diffuse == 0) {
    // read from u0, write to u1
    // read from v0, write to v1
    GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT3 } ;
    glDrawBuffers(2, draw_bufs);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_u0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_v0);
  }
  else {
    // read from u1, write to u0
    // read from v1, write to v0
    GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT2 } ;
    glDrawBuffers(2, draw_bufs);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_u1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_v1);
  }

  glDrawArrays(GL_POINTS, 0, NN);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, W, H);
}

// This uses the current height values to produce a texture that will contain the normals
void HeightField::calculateNormals() {

  GLenum drawbufs[] = { GL_COLOR_ATTACHMENT4 } ;

  glViewport(0, 0, N, N);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_diffuse);
  glDrawBuffers(1, drawbufs);

  glUseProgram(prog_norm);
  glBindVertexArray(vao);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_u0);

  glDrawArrays(GL_POINTS, 0, NN);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, W, H);
}
