#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <time.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


#define FN(i, n) for (int i = 0; i < (int)(n); ++i)
#define FEN(i,n) for (int i = 1;i <= (int)(n); ++i)
#define FA(i, a) for (__typeof((a).begin()) i = (a).begin(); i != (a).end(); i++)
#define pb push_back
#define mp make_pair
#define sz(a) (int)(a).size()
#define pii pair<int,int>
#define vi vector<int>
#define ll long long
#define db long double

using namespace std;

struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;
    GLuint TextureBuffer;
    GLuint TextureID ;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID; // For use with normal shader
	GLuint TexMatrixID; // For use with texture shader
} Matrices;

struct COLOR
{
    float r;
    float g;
    float b;
};

// Some common colours
COLOR grey = {168.0/255.0,168.0/255.0,168.0/255.0};
COLOR gold = {218.0/255.0,165.0/255.0,32.0/255.0};
COLOR coingold = {255.0/255.0,223.0/255.0,0.0/255.0};
COLOR red = {255.0/255.0,51.0/255.0,51.0/255.0};
COLOR lightgreen = {57/255.0,230/255.0,0/255.0};
COLOR darkgreen = {51/255.0,102/255.0,0/255.0};
COLOR black = {30/255.0,30/255.0,21/255.0};
COLOR blue = {0,0,1};
COLOR green = {0,1,0} ;
COLOR darkbrown = {46/255.0,46/255.0,31/255.0};
COLOR lightbrown = {95/255.0,63/255.0,32/255.0};
COLOR brown1 = {117/255.0,78/255.0,40/255.0};
COLOR brown2 = {134/255.0,89/255.0,40/255.0};
COLOR brown3 = {46/255.0,46/255.0,31/255.0};
COLOR cratebrown = {153/255.0,102/255.0,0/255.0};
COLOR cratebrown1 = {121/255.0,85/255.0,0/255.0};
COLOR cratebrown2 = {102/255.0,68/255.0,0/255.0};
COLOR skyblue2 = {113/255.0,185/255.0,209/255.0};
COLOR skyblue1 = {123/255.0,201/255.0,227/255.0};
COLOR skyblue = {132/255.0,217/255.0,245/255.0};
COLOR cloudwhite = {229/255.0,255/255.0,255/255.0};
COLOR cloudwhite1 = {204/255.0,255/255.0,255/255.0};
COLOR lightpink = {255/255.0,122/255.0,173/255.0};
COLOR darkpink = {255/255.0,51/255.0,119/255.0};
COLOR white = {255/255.0,255/255.0,255/255.0};
COLOR score = {117/255.0,78/255.0,40/255.0};

// Camera
float CameraRotateAngle  = 0.5/M_PI ;
float CameraSphereRadius = 10 ;

// Tiles
float TileWidth = 2 ;
float TileLength = 2 ;
float TileHeight = 0.2 ;

struct GameObject
{
    glm::vec3 location,AxisOfRotation,scale, direction,up, gravity , speed ;
    float length,height,width,theta,radius ;
    bool fixed,isRotating ;
    int ID,color;
    struct VAO * object ;
    GameObject(void)
    {
        fixed = true , isRotating = false ;
        length = height = width = theta = radius = 0 ;
        ID = color = 0 ;
        location = AxisOfRotation = up = direction = gravity = speed = glm::vec3(0,0,0) ;
        scale = glm::vec3(1,1,1) ;
        object = NULL ;
    }
} ;

// Global Variables
vector<GameObject> Blocks ;
GameObject Camera ;
vector<GameObject> Floor ;

// Function Declarations
void InitCamera(void) ;
void UpdateCamera(void) ;
void MoveCameraVetz(float) ;
void MoveCameraRadius(float) ;

int do_rot, floor_rel;;
GLuint programID, waterProgramID, fontProgramID, textureProgramID;
double last_update_time, current_time;
glm::vec3 rect_pos, floor_pos;
float rectangle_rotation = 0;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	cout << "Compiling shader : " <<  vertex_file_path << endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage( max(InfoLogLength, int(1)) );
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	cout << VertexShaderErrorMessage.data() << endl;

	// Compile Fragment Shader
	cout << "Compiling shader : " << fragment_file_path << endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage( max(InfoLogLength, int(1)) );
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	cout << FragmentShaderErrorMessage.data() << endl;

	// Link the program
	cout << "Linking program" << endl;
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	cout << ProgramErrorMessage.data() << endl;

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// void initGLEW(void){
//     glewExperimental = GL_TRUE;
//     if(glewInit()!=GLEW_OK){
// 	fprintf(stderr,"Glew failed to initialize : %s\n", glewGetErrorString(glewInit()));
//     }
//     if(!GLEW_VERSION_3_3)
// 	fprintf(stderr, "3.3 version not available\n");
// }



/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}


struct VAO* create3DTexturedObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* texture_buffer_data, GLuint textureID, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;
	vao->TextureID = textureID;

	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->TextureBuffer));  // VBO - textures

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
						  0,                  // attribute 0. Vertices
						  3,                  // size (x,y,z)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );

	glBindBuffer (GL_ARRAY_BUFFER, vao->TextureBuffer); // Bind the VBO textures
	glBufferData (GL_ARRAY_BUFFER, 2*numVertices*sizeof(GLfloat), texture_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
						  2,                  // attribute 2. Textures
						  2,                  // size (s,t)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );

	return vao;
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

void draw3DTexturedObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Bind Textures using texture units
	glBindTexture(GL_TEXTURE_2D, vao->TextureID);

	// Enable Vertex Attribute 2 - Texture
	glEnableVertexAttribArray(2);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->TextureBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle

	// Unbind Textures to be safe
	glBindTexture(GL_TEXTURE_2D, 0);
}

/* Create an OpenGL Texture from an image */
GLuint createTexture (const char* filename)
{
	GLuint TextureID;
	// Generate Texture Buffer
	glGenTextures(1, &TextureID);
	// All upcoming GL_TEXTURE_2D operations now have effect on our texture buffer
	glBindTexture(GL_TEXTURE_2D, TextureID);
	// Set our texture parameters
	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering (interpolation)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load image and create OpenGL texture
	int twidth, theight;
	unsigned char* image = SOIL_load_image(filename, &twidth, &theight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D); // Generate MipMaps to use
	SOIL_free_image_data(image); // Free the data read from file after creating opengl texture
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess it up

	return TextureID;
}

/**************************
 * Customizable functions *
 **************************/

float rectangle_rot_dir = 1;
bool rectangle_rot_status = true;

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Function is called first on GLFW_PRESS.

    if (action == GLFW_RELEASE) {
        switch (key) {
	case GLFW_KEY_C:
	    rectangle_rot_status = !rectangle_rot_status;
	    break;
	case GLFW_KEY_P:
	    break;
	case GLFW_KEY_X:
	    // do something ..
	    break;
	default:
	    break;
        }
    }
    else if (action == GLFW_PRESS) {
        switch (key) {
	case GLFW_KEY_ESCAPE:
	    quit(window);
	    break;
	default:
	    break;
        }
    }
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
    switch (key) {
    case 'Q':
    case 'q':
	quit(window);
	break;
    case 'e':
	rectangle_rotation += 1;
	break;
    case ' ':
	do_rot ^= 1;
	break;
    case 'n' :
    MoveCameraVetz(1) ;
    break ;
    case 'm' :
    MoveCameraVetz(-1) ;
    break ;
    case 'j' : MoveCameraRadius(-1) ; break ;
    case 'k' : MoveCameraRadius(1) ; break ;
    default:
	break;
    }
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
    case GLFW_MOUSE_BUTTON_RIGHT:
	if (action == GLFW_RELEASE) {
	    rectangle_rot_dir *= -1;
	}
	break;
    default:
	break;
    }
}

void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{
    // if (yoffset==-1) {
    //     camera_fov*=1.1;
    // }
    // else if(yoffset==1){
    //     camera_fov/=1.1; //make it bigger than current size
    // }
    // if(camera_fov>=2){
    // 	camera_fov=2;
    // }
    // if(camera_fov<=0.5){
    // 	camera_fov=0.5;
    // }
    // reshapeWindow(window,700,1400);
}
/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    GLfloat fov = M_PI/2;

    // sets the viewport of openGL renderer
    glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

    // Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    Matrices.projection = glm::perspective(fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    //Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

VAO *rectangle, *cam, *floor_vao;

// Creates the rectangle object used in this sample code
VAO* createCube (GLuint textureID)
{
    // GL3 accepts only Triangles. Quads are not supported
    static const GLfloat vertex_buffer_data[] = {
        -0.5 ,  0.5 ,  0.5 ,  //  1
        -0.5 ,  -0.5 ,  0.5 ,  //  2
        0.5 ,  -0.5 ,  0.5 ,  //  3
        -0.5 ,  0.5 ,  0.5 ,  //  1
        0.5 ,  -0.5 ,  0.5 ,  //  3
        0.5 ,  0.5 ,  0.5 ,  //  4
        0.5 ,  0.5 ,  0.5 ,  //  4
        0.5 ,  -0.5 ,  0.5 ,  //  3
        0.5 ,  -0.5 ,  -0.5 ,  //  5
        0.5 ,  0.5 ,  0.5 ,  //  4
        0.5 ,  -0.5 ,  -0.5 ,  //  5
        0.5 ,  0.5 ,  -0.5 ,  //  6
        0.5 ,  0.5 ,  -0.5 ,  //  6
        0.5 ,  -0.5 ,  -0.5 ,  //  5
        -0.5 ,  -0.5 ,  -0.5 ,  //  7
        0.5 ,  0.5 ,  -0.5 ,  //  6
        -0.5 ,  -0.5 ,  -0.5 ,  //  7
        -0.5 ,  0.5 ,  -0.5 ,  //  8
        -0.5 ,  0.5 ,  -0.5 ,  //  8
        -0.5 ,  -0.5 ,  -0.5 ,  //  7
        -0.5 ,  -0.5 ,  0.5 ,  //  2
        -0.5 ,  0.5 ,  -0.5 ,  //  8
        -0.5 ,  -0.5 ,  0.5 ,  //  2
        -0.5 ,  0.5 ,  0.5 ,  //  1
        -0.5 ,  0.5 ,  -0.5 ,  //  8
        -0.5 ,  0.5 ,  0.5 ,  //  1
        0.5 ,  0.5 ,  0.5 ,  //  4
        -0.5 ,  0.5 ,  -0.5 ,  //  8
        0.5 ,  0.5 ,  0.5 ,  //  4
        0.5 ,  0.5 ,  -0.5 ,  //  6
        -0.5 ,  -0.5 ,  0.5 ,  //  2
        -0.5 ,  -0.5 ,  -0.5 ,  //  7
        0.5 ,  -0.5 ,  -0.5 ,  //  5
        -0.5 ,  -0.5 ,  0.5 ,  //  2
        0.5 ,  -0.5 ,  -0.5 ,  //  5
        0.5 ,  -0.5 ,  0.5 ,  //  3
    };
    // scaling side to requirement
    static const GLfloat color_buffer_data [] = {
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	// 0, 0, 0,
	// 0, 0, 0,
	// 1, 1, 1,
    };
    GLfloat texture_buffer_data [] = {
		0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		1,0, // TexCoord 3 - top right

        0,1,  // TexCoord 1 - bot left
		1,0, // TexCoord 3 - top right
		0,0, // TexCoord 4 - top left

        0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		1,0, // TexCoord 3 - top right

        0,1,  // TexCoord 1 - bot left
		1,0, // TexCoord 3 - top right
		0,0, // TexCoord 4 - top left

        0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		1,0, // TexCoord 3 - top right

        0,1,  // TexCoord 1 - bot left
		1,0, // TexCoord 3 - top right
		0,0, // TexCoord 4 - top left

        0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		1,0, // TexCoord 3 - top right

        0,1,  // TexCoord 1 - bot left
		1,0, // TexCoord 3 - top right
		0,0, // TexCoord 4 - top left

        0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		1,0, // TexCoord 3 - top right

        0,1,  // TexCoord 1 - bot left
		1,0, // TexCoord 3 - top right
		0,0, // TexCoord 4 - top left

        0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		1,0, // TexCoord 3 - top right

        0,1,  // TexCoord 1 - bot left
		1,0, // TexCoord 3 - top right
		0,0, // TexCoord 4 - top left

	};

    // create3DObject creates and returns a handle to a VAO that can be used later
    return create3DTexturedObject(GL_TRIANGLES, 12*3, vertex_buffer_data, texture_buffer_data, textureID, GL_FILL);
}
float camera_rotation_angle = 90;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw (GLFWwindow* window, float x, float y, float w, float h, int doM, int doV, int doP)
{
    int fbwidth, fbheight;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);
    glViewport((int)(x*fbwidth), (int)(y*fbheight), (int)(w*fbwidth), (int)(h*fbheight));


    // use the loaded shader program
    // Don't change unless you know what you are doing
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);

    glm::mat4 VP;
    glm::mat4 MVP;	// MVP = Projection * View * Model

	VP = Matrices.projection * Matrices.view;


    for(auto it:Floor)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it.object);
    }

    // Render with texture shaders now
	glUseProgram(textureProgramID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

    for(auto it:Blocks)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }
}
/************************
    CAMERA
*************************/
void InitCamera(void)
{
    Camera.location = glm::vec3( CameraSphereRadius, 0, 0);
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    Camera.direction = glm::vec3(0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    Camera.up = glm::vec3(0, 0, 1);
    UpdateCamera() ;
}
void UpdateCamera(void) {  Matrices.view = glm::lookAt(Camera.location,Camera.direction,Camera.up); }
void MoveCameraHoz(float direction)
{
    Camera.location = glm::rotate(Camera.location,CameraRotateAngle*direction,glm::vec3(0,0,1)) ;
    UpdateCamera() ;
}
void MoveCameraVetz(float direction)
{
    glm::vec3 normal = normalize(cross(Camera.up,Camera.direction - Camera.location)) ;
    Camera.location = glm::rotate(Camera.location,CameraRotateAngle*direction,normal) ;
    // Camera.up = normalize(cross(Camera.direction - Camera.location,normal)) ;
    UpdateCamera() ;
}
void MoveCameraRadius(float direction)
{
    float Radius = glm::length(Camera.location) * (float)(1 + direction*0.05)  ;
    if(Radius < 5) Radius = 5 ;
    Camera.location = normalize(Camera.location) * Radius  ;
    UpdateCamera() ;
}
/************************
    BLOCKS
*************************/
void CreateBlocks(int textureID)
{
    GameObject temp ;
    temp.height = 4 ;temp.width = TileWidth ;temp.length = TileLength ;
    temp.scale = glm::vec3(temp.width,temp.length,temp.height) ;
    temp.object = createCube(textureID) ;
    temp.AxisOfRotation = glm::vec3(0,0,1) ;
    temp.location = glm::vec3(-3,-9,-1) ;
    Blocks.pb(temp) ;
}
/**********************
    FLOOR
***********************/
void createFloor(void)
{
    float delta = 0.05 ;
    GameObject temp ;
    temp.height = TileHeight ;temp.width = TileWidth ;temp.length = TileLength ;
    temp.scale = glm::vec3(temp.width - delta,temp.length - delta,temp.height) ;
    // temp.object = createCube() ;
    temp.AxisOfRotation = glm::vec3(0,0,1) ;
    temp.location = glm::vec3(-3,-9,-3) ;
    FN(x,4)
    {
        FN(y,10)
        {
            Floor.pb(temp) ;
            temp.location.y += temp.length ;
        }
        temp.location.y = -9 ;
        temp.location.x += temp.width ;
    }
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
	GLFWwindow* window; // window desciptor/handle

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );

	/* --- register callbacks with GLFW --- */

	/* Register function to handle window resizes */
	/* With Retina display on Mac OS X GLFW's FramebufferSize
	 is different from WindowSize */
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);

	/* Register function to handle window close */
	glfwSetWindowCloseCallback(window, quit);

	/* Register function to handle keyboard input */
	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

	/* Register function to handle mouse click */
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
	glfwSetScrollCallback(window, mousescroll); // mouse scroll

	return window;
}


/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
    glActiveTexture(GL_TEXTURE0);
	textureProgramID = LoadShaders( "TextureRender.vert", "TextureRender.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.TexMatrixID = glGetUniformLocation(textureProgramID, "MVP");
    GLint textureID5 = createTexture("Images/middle.png");
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    // rectangle = createCube() ;
    InitCamera() ;
    CreateBlocks(textureID5) ;
    // createFloor();

    // Create and compile our GLSL program from the shaders
	programID = LoadShaders( "shader.vert", "shader.frag" );
	waterProgramID = LoadShaders ( "watershader.vert", "watershader.frag");
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	//glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    // cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    // cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    // cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
    int width = 600;
    int height = 600;
    rect_pos = glm::vec3(0, 0, 0);
    floor_pos = glm::vec3(0, 0, 0);
    do_rot = 0;
    floor_rel = 1;

    GLFWwindow* window = initGLFW(width, height);
    // initGLEW();
    initGL (window, width, height);

    last_update_time = glfwGetTime();
    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {

	// clear the color and depth in the frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // OpenGL Draw commands
	current_time = glfwGetTime();
	if(do_rot) MoveCameraHoz(1) ;



    last_update_time = current_time;
    draw(window,0,0,1,1, 1, 1, 1);
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    glfwTerminate();
    //    exit(EXIT_SUCCESS);
}
