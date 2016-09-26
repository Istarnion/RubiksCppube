#include "imgui-SFML.h"
#include "imgui.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <glm/glm.hpp>

// Because gluPerspective is deprecated, we implement it ourself. No warnings!
void perspective(GLdouble fovy, GLdouble aspectRatio, GLdouble near, GLdouble far) {
  GLdouble xmin, xmax, ymin, ymax;
  ymax = near * tan(fovy * M_PI / 360.0);
  ymin = -ymax;
  xmin = ymin * aspectRatio;
  xmax = ymax * aspectRatio;
  
  glFrustum(xmin, xmax, ymin, ymax, near, far);
}

enum Rotation {
  YAW,
  PITCH,
  ROLL
};

enum Side {
  RED,
  GREEN,
  BLUE,
  ORANGE,
  YELLOW,
  WHITE
};

class Cubie {
public:
  const float SIDE_COLORS[6][3] = {
      {0.77f, 0.12f, 0.23f}, // RED
      {0.00f, 0.62f, 0.38f}, // GREEN
      {0.00f, 0.32f, 0.73f}, // BLUE
      {1.00f, 0.35f, 0.00f}, // ORANGE
      {1.00f, 0.84f, 0.00f}, // YELLOW
      {1.00f, 1.00f, 1.00f}  // WHITE
  };

  float
      right[3],
      front[3],
      back[3],
      left[3],
      down[3],
      up[3];

  const size_t BYTES;

  Cubie() : BYTES(sizeof(float)*3) {
    // Could have used std::copy here, but I like C
    memcpy(right, SIDE_COLORS[RED], BYTES);
    memcpy(front, SIDE_COLORS[GREEN], BYTES);
    memcpy(back, SIDE_COLORS[BLUE], BYTES);
    memcpy(left, SIDE_COLORS[ORANGE], BYTES);
    memcpy(down, SIDE_COLORS[YELLOW], BYTES);
    memcpy(up, SIDE_COLORS[WHITE], BYTES);
  }

  // Copy constructor
  Cubie(const Cubie &original) : BYTES(sizeof(float)*3) {
    memcpy(right, original.right, BYTES);
    memcpy(front, original.front, BYTES);
    memcpy(back, original.back, BYTES);
    memcpy(left, original.left, BYTES);
    memcpy(down, original.down, BYTES);
    memcpy(up, original.up, BYTES);
  }

  // Copy assignment
  const Cubie &operator=(const Cubie &original)
  {
    memcpy(right, original.right, BYTES);
    memcpy(front, original.front, BYTES);
    memcpy(back, original.back, BYTES);
    memcpy(left, original.left, BYTES);
    memcpy(down, original.down, BYTES);
    memcpy(up, original.up, BYTES);
    
    return *this;
  }

  void draw() {
    glBegin(GL_QUADS);
    {
      glColor3fv(right);
      glVertex3f(0.5f,  0.5f,  0.5f);
      glVertex3f(0.5f, -0.5f,  0.5f);
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f, -0.5f);

      glColor3fv(front);
      glVertex3f( 0.5f,  0.5f, 0.5f);
      glVertex3f( 0.5f, -0.5f, 0.5f);
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f(-0.5f,  0.5f, 0.5f);

      glColor3fv(back);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      glColor3fv(left);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      glColor3fv(down);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);

      glColor3fv(up);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
    }
    glEnd();
  }
  
  void swapAndRotate(Cubie other, Rotation rot, bool clockwise) {
    switch(rot) {
    case YAW:
      if(clockwise) {
        memcpy(right, other.back, BYTES);
        memcpy(front, other.right, BYTES);
        memcpy(left, other.front, BYTES);
        memcpy(back, other.left, BYTES);
        memcpy(up, other.up, BYTES);
        memcpy(down, other.down, BYTES);
      }
      else {
        memcpy(right, other.front, BYTES);
        memcpy(front, other.left, BYTES);
        memcpy(left, other.back, BYTES);
        memcpy(back, other.right, BYTES);
        memcpy(up, other.up, BYTES);
        memcpy(down, other.down, BYTES);
      }
      break;
    case PITCH:
      if(clockwise) {
        memcpy(right, other.right, BYTES);
        memcpy(front, other.up, BYTES);
        memcpy(left, other.left, BYTES);
        memcpy(back, other.down, BYTES);
        memcpy(up, other.back, BYTES);
        memcpy(down, other.front, BYTES);
      }
      else {
        memcpy(right, other.right, BYTES);
        memcpy(front, other.up, BYTES);
        memcpy(left, other.left, BYTES);
        memcpy(back, other.down, BYTES);
        memcpy(up, other.back, BYTES);
        memcpy(down, other.front, BYTES);
      }
      break;
    case ROLL:
      if(clockwise) {
        memcpy(right, other.up, BYTES);
        memcpy(front, other.front, BYTES);
        memcpy(left, other.down, BYTES);
        memcpy(back, other.back, BYTES);
        memcpy(up, other.left, BYTES);
        memcpy(down, other.right, BYTES);
      }
      else {
        memcpy(right, other.down, BYTES);
        memcpy(front, other.front, BYTES);
        memcpy(left, other.up, BYTES);
        memcpy(back, other.back, BYTES);
        memcpy(up, other.right, BYTES);
        memcpy(down, other.left, BYTES);
      }
      break;
    default:
      break;
    }
  }
};

class RubiksCube {
public:
  float spacing = 1.1f;
  Cubie cubes[27];
  
  bool left, right, up, down;
  float xangle = 0, yangle = 0;
  float rotationSpeed = 60; // Degrees per second

  void draw(float delta) {
    glLoadIdentity();
    glTranslatef(0, 0, -10);
    
    if(left) yangle -= rotationSpeed * delta;
    if(right) yangle += rotationSpeed * delta;
    if(up) xangle -= rotationSpeed * delta;
    if(down) xangle += rotationSpeed * delta;
    
    glRotatef(xangle, 1, 0, 0);
    glRotatef(yangle, 0, 1, 0);

    for(float x = -1; x <= 1; ++x) {
      for(float y = -1; y <= 1; ++y) {
        for(float z = -1; z <= 1; ++z) {
          int index = toIndex((int)x + 1, (int)y + 1, (int)z + 1);

          glPushMatrix();

          glTranslatef(x * spacing, y * spacing, z * spacing);
          cubes[index].draw();

          glPopMatrix();
        }
      }
    }
  }

  void rotate(Side side, bool clockwise) {
    Rotation rot;
    bool localClockwise;
    
    switch(side) {
      case RED: case ORANGE:
      {
        rot = PITCH;
        localClockwise = clockwise && side == RED;
        
        int xindex = side == RED? 2 : 0;
        
        Cubie temp;
        for(int i=0; i<2; ++i) {
          temp = cubes[toIndex(xindex, 0, i)];
          
          cubes[toIndex(xindex, 0, i)].swapAndRotate(
            cubes[toIndex(xindex, i, 2)], rot, localClockwise);
          
          cubes[toIndex(xindex, i, 2)].swapAndRotate(
            cubes[toIndex(xindex, 2, 2-i)], rot, localClockwise);
          
          cubes[toIndex(xindex, 2, 2-i)].swapAndRotate(
            cubes[toIndex(xindex, 2-i, 0)], rot, localClockwise);
          
          cubes[toIndex(xindex, 2-i, 0)].swapAndRotate(
            temp, rot, localClockwise);
        }
      } break;
      case GREEN: case BLUE:
      {
        rot = ROLL;
        localClockwise = clockwise && side == GREEN;
        
        int zindex = side == GREEN? 2 : 0;
        
        Cubie temp;
        for(int i=0; i<2; ++i)
        {
          temp = cubes[toIndex(0, i, zindex)];
          
          cubes[toIndex(0, i, zindex)].swapAndRotate(
            cubes[toIndex(i, 2, zindex)], rot, localClockwise);
          
          cubes[toIndex(i, 2, zindex)].swapAndRotate(
            cubes[toIndex(2, 2-i, zindex)], rot, localClockwise);
          
          cubes[toIndex(2, 2-i, zindex)].swapAndRotate(
            cubes[toIndex(2-i, 0, zindex)], rot, localClockwise);
          
          cubes[toIndex(2-i, 0, zindex)].swapAndRotate(
            temp, rot, localClockwise);
        }
      } break;
      case WHITE: case YELLOW:
      {
        rot = YAW;
        localClockwise = clockwise && side == WHITE;
        
        int yindex = side == WHITE? 2 : 0;
        
        Cubie temp;
        for(int i=0; i<2; ++i) {
          temp = cubes[toIndex(0, yindex, i)];
          
          cubes[toIndex(0, yindex, i)].swapAndRotate(
            cubes[toIndex(i, yindex, 2)], rot, localClockwise);
          
          cubes[toIndex(i, yindex, 2)].swapAndRotate(
            cubes[toIndex(2, yindex, 2-i)], rot, localClockwise);
          
          cubes[toIndex(2, yindex, 2-i)].swapAndRotate(
            cubes[toIndex(2-i, yindex, 0)], rot, localClockwise);
          
          cubes[toIndex(2-i, yindex, 0)].swapAndRotate(
            temp, rot, localClockwise);
        }
      } break;
      default: break;
    }
  }

  /*
  Handle two letter commands:
  g - green side,
  b - blue side, and so on.
  
  A one letter command rotates clockwise,
  while adding an 'i' to the command means rotate counter-clockwise.
  e.g. 'yi' means rotate yellow face counter-clockwise.
  */
  void executeCommand(std::string command) {
    if(command.length() == 0) return;
    bool clockwise = (command.length() == 2 && command[1] == 'i');
    
    char side = command[0];
    switch(side) {
      case 'g':
        rotate(GREEN, clockwise);
        break;
      case 'r':
        rotate(RED, clockwise);
        break;
      case 'o':
        rotate(ORANGE, clockwise);
        break;
      case 'b':
        rotate(BLUE, clockwise);
        break;
      case 'y':
        rotate(YELLOW, clockwise);
        break;
      case 'w':
        rotate(WHITE, clockwise);
        break;
      default: break;
    }
  }

  int toIndex(int x, int y, int z) {
    return x * 9 + y * 3 + z;
  }
};

class RubiksApp {
public:
  sf::RenderWindow window;
  RubiksCube rubikscube;

RubiksApp() :
  window(
         sf::VideoMode(800, 800),
         "RubiksCube",
         sf::Style::Default,
         sf::ContextSettings(32, 0, 0, 2, 1)) {
    
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    glClearColor(0.18f, 0.18f, 0.18f, 1.0f);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    perspective(60.0, 512.0 / 512.0, 0.5, 100);

    glMatrixMode(GL_MODELVIEW);

    ImGui::SFML::Init(window);
  }

  void start() {

    char inputBuffer[3] = {0};

    sf::Clock clock;
    sf::Event event;
    bool running = true;
    while (running) {
      while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed) {
          running = false;
        }
        else if (event.type == sf::Event::Resized) {
          glViewport(0, 0, event.size.width, event.size.height);
          
          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          perspective(60.0f, event.size.width / (float)event.size.height, 0.1, 100);
          
          glMatrixMode(GL_MODELVIEW);
        }
        else if(event.type == sf::Event::KeyPressed) {
          switch(event.key.code) {
            case sf::Keyboard::Up:
              rubikscube.up = true;
              break;
            case sf::Keyboard::Right:
              rubikscube.right = true;
              break;
            case sf::Keyboard::Down:
              rubikscube.down = true;
              break;
            case sf::Keyboard::Left:
              rubikscube.left = true;
            default: break;
          }
        }
        else if(event.type == sf::Event::KeyReleased) {
          switch(event.key.code) {
            case sf::Keyboard::Up:
              rubikscube.up = false;
              break;
            case sf::Keyboard::Right:
              rubikscube.right = false;
              break;
            case sf::Keyboard::Down:
              rubikscube.down = false;
              break;
            case sf::Keyboard::Left:
              rubikscube.left = false;
            default: break;
          }
        }
      }

      sf::Time delta = clock.restart();

      ImGui::SFML::Update(delta);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // We have to enable depth testing here, because the SFML ImGui disables it
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      rubikscube.draw(delta.asSeconds());

      ImGui::SetNextWindowPos(ImVec2(0, window.getSize().y - 35));

      bool open;
      if (ImGui::Begin("ImGui", &open, ImGuiWindowFlags_NoTitleBar)) {

        ImGui::SetKeyboardFocusHere();
        ImGui::SetWindowSize(ImVec2(window.getSize().x, 35));
        if (ImGui::InputText("Enter command", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
          rubikscube.executeCommand(inputBuffer);
          memset(inputBuffer, 0, sizeof(inputBuffer));
        }
      }
      ImGui::End();

      ImGui::Render();
      window.display();
    }
  }
};

int main() {
  RubiksApp app;
  app.start();

  return 0;
}
