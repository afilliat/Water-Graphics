TARGET = a2_solution_glfw
OBJECTS = main.o src/ShaderProgram.o src/ShaderUtils.o src/OpenGLUtils.o src/BezierSurfaceUtils.o

CXX    = C:/Rtools/mingw_64/bin/g++.exe
CFLAGS = -Wall -g

INCPATH += -IC:/sw/opengl/include -I./include
LIBPATH += -LC:/sw/opengl/lib -L./lib

DEL = rm

LIBS += -lopengl32 -lglu32 -lglew32.dll -lglfw3 -lgdi32

all: $(TARGET)

clean:
	$(DEL) -f $(OBJECTS) $(TARGET)

depend:
	$(DEL) -f Makefile.bak
	mv Makefile Makefile.bak
	sed '/^# DEPENDENCIES/,$$d' Makefile.bak > Makefile
	echo '# DEPENDENCIES' >> Makefile
	$(CXX) -MM *.cpp >> Makefile

.c.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

.cc.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

.cpp.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(INCPATH) -o $@ $^ $(LIBPATH) $(LIBS)

new: clean $(TARGET)

# DEPENDENCIES
