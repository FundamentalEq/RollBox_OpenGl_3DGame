#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <deque>
#include <string>
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <ao/ao.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <GL/gl.h>
#include <GL/glu.h>
// #include <FTGL/ftgl.h>
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
float CameraRotateAngle  = 5 * M_PI / (float) 180 ;
float CameraSphereRadius = 10 ;
deque< glm::vec3 > BlockPrevLocation ;
bool FollowCamView = false ;
bool HelliCamView = false ;
glm::vec3 MousePrevPosition ;
bool BlockCamView = false ;

// Tiles
float TileWidth = 2 ;
float TileLength = 2 ;
float TileHeight = 0.2 ;
glm::vec3 WinLoc(0,0,0) ;

// Board
float BoardWidth ;
float BoardLength ;

// Levels
bool GameWon = false ;
int LevelNumber = 1;
double GameWinningTime ;

// Block
float BlockRotateAngleDegree = 5 ;
float BlockRotateAngle = BlockRotateAngleDegree * M_PI / (float) 180 ;
int BlockRotationSteps = (int)( (float)90 / BlockRotateAngleDegree ) ;
int BlockRotationStepsLeft = 0 ;
glm::vec3 BlockRotationFixedPoint ;
glm::vec3 BlockBodyRefPoint ;
glm::vec3 BlockRight ;
glm::vec3 BlockFront ;
bool BlockRotatingUpdateH = false ;
bool BlockRotatingUpdateV = false ;
bool BlockRotatingH = false ;
bool BlockRotatingV = false ;
bool BlockIsFalling = false ;
float BlockFallingSpeed = 0.05 ;
float BlockMoveDir = 0 ;
bool BlockHasMoved = false ;

// GameControls
bool PauseGame = false ;

// Score
int NumberOfSteps = 0 ;

// Screen
int ScreenHeight = 600 ;
int ScreenWidth = 600 ;

// Sound
static const int BUF_SIZE = 4096;
int lost ;
pthread_t Writer_thr[3];

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

struct WavHeader {
    char id[4]; //should contain RIFF
    int32_t totalLength;
    char wavefmt[8];
    int32_t format; // 16 for PCM
    int16_t pcm; // 1 for PCM
    int16_t channels;
    int32_t frequency;
    int32_t bytesPerSecond;
    int16_t bytesByCapture;
    int16_t bitsPerSample;
    char data[4]; // "data"
    int32_t bytesInData;
};

// Global Variables
vector<GameObject> Blocks ;
GameObject Camera ;
vector<GameObject> LiveFloor ;
vector< vector<GameObject> > HiddenFloor ;
vector< pair<int,int> > ButtonList ;
vector<GameObject> Buttons ;
vector<bool> ButtonHasBeenPressed ;
map< string,int > Textures ;
vector<GameObject> SkylineBox;
vector<GameObject> FragileFloor ;

// Function Declarations
void InitCamera(void) ;
void UpdateCamera(void) ;
void MoveCameraVetz(float) ;
void MoveCameraHoz(float) ;
void MoveCameraRadius(float) ;
glm::mat4 RotateBlock(glm::vec3 ,glm::vec3,glm::vec3) ;
void FindAxisOfRotationR(float) ;
void MoveBlockH(float) ;
void MoveBlockV(float) ;
void CheckFall(void) ;
void BlockFall(void) ;
void SetTopView(void) ;
void SetTowerView(void) ;
void SetFollowCamView(void) ;
void SetHeliCam(GLFWwindow*) ;
glm::vec3 GetMouseCoordinates(GLFWwindow*) ;
void SetBlockView(void) ;
float FindCurrentHeight(void) ;
void CheckButtonPress(void) ;
void SetGame(void) ;
glm::vec3 GetBlockScreenCor(void) ;
void MoveBlockMouse(GLFWwindow*) ;

GLuint programID, waterProgramID, fontProgramID, textureProgramID;
double last_update_time, current_time;

void* Writer(void * i)
{
	ao_device* device;
    ao_sample_format format;
    int defaultDriver;
    WavHeader header;

    std::ifstream file;
    file.open("Sea Waves-SoundBible.com-946156036.wav", std::ios::binary | std::ios::in);

    file.read(header.id, sizeof(header.id));
    //assert(!std::memcmp(header.id, "RIFF", 4)); //is it a WAV file?
    file.read((char*)&header.totalLength, sizeof(header.totalLength));
    file.read(header.wavefmt, sizeof(header.wavefmt)); //is it the right format?
    //assert(!std::memcmp(header.wavefmt, "WAVEfmt ", 8));
    file.read((char*)&header.format, sizeof(header.format));
    file.read((char*)&header.pcm, sizeof(header.pcm));
    file.read((char*)&header.channels, sizeof(header.channels));
    file.read((char*)&header.frequency, sizeof(header.frequency));
    file.read((char*)&header.bytesPerSecond, sizeof(header.bytesPerSecond));
    file.read((char*)&header.bytesByCapture, sizeof(header.bytesByCapture));
    file.read((char*)&header.bitsPerSample, sizeof(header.bitsPerSample));
    file.read(header.data, sizeof(header.data));
    file.read((char*)&header.bytesInData, sizeof(header.bytesInData));

    ao_initialize();

    defaultDriver = ao_default_driver_id();

    memset(&format, 0, sizeof(format));
    format.bits = header.format;
    format.channels = header.channels;
    format.rate = header.frequency;
    format.byte_format = AO_FMT_LITTLE;

    device = ao_open_live(defaultDriver, &format, NULL);
    if (device == NULL) {
        std::cout << "Unable to open driver" << std::endl;
        //return;
    }


    char* buffer = (char*)malloc(BUF_SIZE * sizeof(char));

    // determine how many BUF_SIZE chunks are in file
    int fSize = header.bytesInData;
    int bCount = fSize / BUF_SIZE;

    for (int i = 0; i < bCount; ++i) {
        file.read(buffer, BUF_SIZE);
				if(lost==1) {ao_close(device);
		    ao_shutdown();pthread_exit((void *)NULL);}
        ao_play(device, buffer, BUF_SIZE);
    }

    int leftoverBytes = fSize % BUF_SIZE;
  //  std::cout << leftoverBytes;
    file.read(buffer, leftoverBytes);
    memset(buffer + leftoverBytes, 0, BUF_SIZE - leftoverBytes);
		if(lost==1) {ao_close(device);
    ao_shutdown();pthread_exit((void *)NULL);}
    ao_play(device, buffer, BUF_SIZE);

    ao_close(device);
    ao_shutdown();


}

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


/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Function is called first on GLFW_PRESS.
    if(PauseGame)
    {
        if(action == GLFW_PRESS && key == GLFW_KEY_P) PauseGame ^= 1 ;
        return ;
    }
    if (action == GLFW_RELEASE) {
        switch (key) {
            default : break ;
        }
    }
    else if (action == GLFW_PRESS) {
        switch (key) {
	case GLFW_KEY_ESCAPE:
	    quit(window);
	    break;
    case GLFW_KEY_P: PauseGame ^= 1 ; break ;
    case GLFW_KEY_RIGHT : if(!BlockRotatingV && !BlockRotatingH) BlockRotatingH = true ,BlockMoveDir = 1; break ;
    case GLFW_KEY_LEFT : if(!BlockRotatingV && !BlockRotatingH) BlockRotatingH = true ,BlockMoveDir = -1; break ;
    case GLFW_KEY_UP : if(!BlockRotatingV && !BlockRotatingH) BlockRotatingV = true ,BlockMoveDir = 1; break ;
    case GLFW_KEY_DOWN : if(!BlockRotatingV && !BlockRotatingH) BlockRotatingV = true ,BlockMoveDir = -1; break ;
    default:  break;
        }
    }
    else if (action == GLFW_REPEAT)
    {
        switch(key)
        {
            case GLFW_KEY_RIGHT : if(!BlockRotatingV && !BlockRotatingH) BlockRotatingH = true ,BlockMoveDir = 1; break ;
            case GLFW_KEY_LEFT : if(!BlockRotatingV && !BlockRotatingH) BlockRotatingH = true ,BlockMoveDir = -1; break ;
            case GLFW_KEY_UP : if(!BlockRotatingV && !BlockRotatingH) BlockRotatingV = true ,BlockMoveDir = 1; break ;
            case GLFW_KEY_DOWN : if(!BlockRotatingV && !BlockRotatingH) BlockRotatingV = true ,BlockMoveDir = -1; break ;
        }
    }
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
    if(PauseGame) return ;
    switch (key) {
    case 'Q':  case 'q': quit(window); 	break;
    case 'n' : MoveCameraVetz(1) ;   break ;
    case 'm' :  MoveCameraVetz(-1) ; break ;
    case ',' :  MoveCameraHoz(1) ; break ;
    case '.' :  MoveCameraHoz(-1) ; break ;
    case 'j' : MoveCameraRadius(-1) ; break ;
    case 'k' : MoveCameraRadius(1) ; break ;
    case 't' : FollowCamView = false ; BlockCamView = false ; SetTopView() ; break ;
    case 'y' : FollowCamView = false ; BlockCamView = false ; SetTowerView() ; break ;
    case 'f' : BlockCamView = false ; FollowCamView = true ; break ;
    case 'b' : FollowCamView = false ; BlockCamView = true ; break ;
    default:
	break;
    }
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    if(PauseGame) return ;
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
	   if (action == GLFW_PRESS)
       {
           FollowCamView = BlockCamView = false ;
           HelliCamView = true ;
           MousePrevPosition = GetMouseCoordinates(window) ;
	    }
        else if(action == GLFW_RELEASE) HelliCamView = false ;
	    break;
    case GLFW_MOUSE_BUTTON_RIGHT :
        if(action == GLFW_PRESS && !BlockRotatingH && !BlockRotatingV && !BlockIsFalling) MoveBlockMouse(window) ;
        break ;
    default:
	break;
    }
}

void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if(PauseGame) return ;
    if (yoffset==-1) MoveCameraRadius(1) ;
    else if(yoffset==1) MoveCameraRadius(-1) ;
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

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw (GLFWwindow* window, float x, float y, float w, float h, int doM, int doV, int doP)
{
    if(current_time - GameWinningTime > 5 && GameWon)
    {
        BlockIsFalling = false ;
        GameWon = false ;
        ++LevelNumber ;
        SetGame() ;
    }
    if(!BlockRotatingH && !BlockRotatingV && BlockHasMoved) CheckFall() ;

    int fbwidth, fbheight;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);
    glViewport((int)(x*fbwidth), (int)(y*fbheight), (int)(w*fbwidth), (int)(h*fbheight));

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram(programID);

    glm::mat4 VP;
    glm::mat4 MVP;	// MVP = Projection * View * Model

    if(FollowCamView) SetFollowCamView() ;
    if(BlockCamView) SetBlockView() ;
    if(HelliCamView) SetHeliCam(window) ;
    VP = Matrices.projection * Matrices.view;


    // for(auto it:Floor)
    // {
    //     Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
    //     // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
    //     MVP = VP * Matrices.model;
    //     glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    //     draw3DObject(it.object);
    // }

    // Render with texture shaders now
	glUseProgram(textureProgramID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

    // FindAxisOfRotationR(1) ;
    if(BlockIsFalling) BlockFall() ;
    else if(BlockRotatingH) MoveBlockH(BlockMoveDir) ;
    else if(BlockRotatingV) MoveBlockV(BlockMoveDir) ;
    if(!BlockCamView) for(auto &it:Blocks)
    {
        Matrices.model = RotateBlock(it.direction,normalize(cross(it.up,it.direction)),it.up) * glm::scale(it.scale)     ;
        // Matrices.model = RotateBlock(glm::vec3(0,0,-1),glm::vec3(-1,0,0),glm::vec3(0,1,0)) * glm::scale(it.scale)     ;
        Matrices.model = glm::translate(it.location)* Matrices.model ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }

    for(auto &it:LiveFloor)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }
    for(auto &it:SkylineBox)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }
    for(auto &it:Buttons)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }
    FN(i,sz(ButtonHasBeenPressed)) if(ButtonHasBeenPressed[i])
    {
        for(auto &it:HiddenFloor[i])
        {
            Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DTexturedObject(it.object);
        }
    }
    for(auto &it:FragileFloor) if(it.fixed)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }
}
void draw2(GLFWwindow* window, float x, float y, float w, float h, int doM, int doV, int doP)
{
    int fbwidth, fbheight;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);
    glViewport((int)(x*fbwidth), (int)(y*fbheight), (int)(w*fbwidth), (int)(h*fbheight));
    glUseProgram(programID);

    glm::mat4 VP;
    glm::mat4 MVP;	// MVP = Projection * View * Model

    VP = Matrices.projection * Matrices.view;

	glUseProgram(textureProgramID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

    for(auto &it:Blocks)
    {
        Matrices.model = RotateBlock(it.direction,normalize(cross(it.up,it.direction)),it.up) * glm::scale(it.scale)     ;
        // Matrices.model = RotateBlock(glm::vec3(0,0,-1),glm::vec3(-1,0,0),glm::vec3(0,1,0)) * glm::scale(it.scale)     ;
        Matrices.model = glm::translate(it.location)* Matrices.model ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }

    for(auto &it:LiveFloor)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }
    for(auto &it:SkylineBox)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }
    for(auto &it:Buttons)
    {
        Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
        // Matrices.model = glm::rotate((float)(rectangle_rotation*M_PI/180.0f),it.AxisOfRotation) ;
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DTexturedObject(it.object);
    }
    FN(i,sz(ButtonHasBeenPressed)) if(ButtonHasBeenPressed[i])
    {
        for(auto &it:HiddenFloor[i])
        {
            Matrices.model = glm::translate(it.location) * glm::scale(it.scale);
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DTexturedObject(it.object);
        }
    }
    for(auto &it:FragileFloor) if(it.fixed)
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
    Camera.location = glm::rotate(Camera.location,CameraRotateAngle*direction,Camera.up) ;
    UpdateCamera() ;
}
void MoveCameraVetz(float direction)
{
    glm::vec3 normal = normalize(cross(Camera.up,Camera.direction - Camera.location)) ;
    Camera.location = glm::rotate(Camera.location,CameraRotateAngle*direction,normal) ;
    Camera.up = normalize(cross(Camera.direction - Camera.location,normal)) ;
    UpdateCamera() ;
}
void MoveCameraRadius(float direction)
{
    CameraSphereRadius = glm::length(Camera.location - Camera.direction) * (float)(1 + direction*0.05)  ;
    // if(Radius < 5) Radius = 5 ;
    Camera.location = normalize(Camera.location -Camera.direction) * CameraSphereRadius + Camera.direction  ;
    UpdateCamera() ;
}
void SetTopView(void)
{
    Camera.location = glm::vec3(0,0,CameraSphereRadius)  ;
    Camera.up = glm::vec3(0,1,0) ;
    Camera.direction = glm::vec3(0,0,0) ;
    UpdateCamera() ;
}
void SetTowerView(void)
{
    Camera.location = normalize(glm::vec3(0,-1,1)) * CameraSphereRadius ;
    Camera.up = normalize(cross(Camera.location,glm::vec3(1,0,0))) ;
    Camera.direction = glm::vec3(0,0,0) ;
    UpdateCamera() ;
}
void SetFollowCamView(void)
{
    auto &Block = Blocks[0] ;
    glm::vec3 displacement = normalize( BlockPrevLocation.front() - Block.location ) ;
    Camera.location =  displacement * CameraSphereRadius + Block.location ;
    Camera.location.z = abs(Camera.location.z) ;
    displacement = Block.location - Camera.location ;
    Camera.up = normalize(glm::vec3(0,0,1) - (displacement) * dot(glm::vec3(0,0,1),displacement)) ;
    // Camera.up = glm::vec3(0,0,1) ;
    Camera.direction = Block.location ;
    UpdateCamera() ;
}
glm::vec3 GetMouseCoordinates(GLFWwindow* window)
{
    double CursorX,CursorY ;
    glfwGetCursorPos(window, &CursorX, &CursorY) ;
    cout<<"Mouse is "<<CursorX<<" "<<CursorY<<endl ;
    return glm::vec3(CursorX,CursorY,0) ;
}
void SetHeliCam(GLFWwindow* window)
{
    float suppress = 1000 ;
    glm::vec3 Mouse = GetMouseCoordinates(window) ;
    MoveCameraHoz(-(Mouse.x - MousePrevPosition.x)/suppress) ;
    MoveCameraVetz((Mouse.y - MousePrevPosition.y)/suppress) ;
}
void SetBlockView(void)
{
    auto &Block = Blocks[0] ;
    Camera.location = Block.location + glm::vec3(0,0,1) * (FindCurrentHeight()) ;
    Camera.direction = normalize(Block.location -  BlockPrevLocation.front() )   + Block.location ;
    Camera.up = normalize(glm::vec3(0,0,1) - (Camera.direction - Camera.location) * dot(glm::vec3(0,0,1),(Camera.direction - Camera.location))) ;
    UpdateCamera() ;
}
/************************
    BLOCKS
*************************/
int RandomNo(int limit) { return rand()%limit ;}
float CorrectNumbers(float x)
{
    if(abs(x - ceil(x)) > abs(x - floor(x))) return floor(x) ;
    return ceil(x) ;
}
void CreateBlocks(void)
{
    GameObject temp ;
    temp.height = TileLength + TileWidth ;temp.width = TileWidth ;temp.length = TileLength ;
    temp.scale = glm::vec3(temp.width,temp.length,temp.height) ;
    temp.object = createCube(Textures["Box"]) ;
    temp.AxisOfRotation = glm::vec3(0,0,1) ;
    temp.up = glm::vec3(0,0,1) ;
    temp.direction = glm::vec3(-1,0,0) ;
    temp.location = glm::vec3(0,0,0) ;
    Blocks.pb(temp) ;
}
glm::mat4 RotateBlock(glm::vec3 X,glm::vec3 Y,glm::vec3 Z)
{
    // glm::vec4 row1(X.x,Y.x,Z.x,0) ;
    // glm::vec4 row2(X.y,Y.y,Z.y,0) ;
    // glm::vec4 row3(X.z,Y.z,Z.z,0) ;
    // glm::vec4 row4(0,0,0,1) ;
    glm::vec4 row1(X.x,X.y,X.z,0) ;
    glm::vec4 row2(Y.x,Y.y,Y.z,0) ;
    glm::vec4 row3(Z.x,Z.y,Z.z,0) ;
    glm::vec4 row4(0,0,0,1) ;
    return glm::mat4(row1,row2,row3,row4) ;
}
void AlignBlock(vector<vector<int> > &Grid)
{
    vector< pair<int,int> > Available  ;
    FN(i,sz(Grid)) if(Grid[i][0] == 1 || (Grid[i][0]%2 == 0 && Grid[i][0] > 0) ) Available.pb({i,0}) ;
    // there must be atleast one spot available
    assert(sz(Available) > 0 ) ;
    auto it = Available[ RandomNo(sz(Available)) ] ;
    Blocks[0].location.x = (it.first - BoardWidth/2)*TileWidth;
    Blocks[0].location.y = (it.second - BoardLength/2)*TileLength ;
    BlockPrevLocation.pb(Blocks[0].location) ;
}
float FindCurrentHeight(void)
{
    if(abs(dot(glm::vec3(0,0,1),Blocks[0].up))> 0.9) return TileLength + TileWidth ;
    else return TileWidth ;
}
glm::vec3 FindRightOfBlock(void)
{
    glm::vec3 right = normalize(cross(Camera.direction - Camera.location,Camera.up)) ;
    if(abs(dot(right,glm::vec3(1,0,0))) > abs(dot(right,glm::vec3(0,1,0))))
        right = glm::vec3(1,0,0) * dot(right,glm::vec3(1,0,0)) / abs(dot(right,glm::vec3(1,0,0))) ;
    else
        right = glm::vec3(0,1,0) * dot(right,glm::vec3(0,1,0)) / abs(dot(right,glm::vec3(0,1,0))) ;
    return right ;
}
glm::vec3 FindFrontOfBlock(void) { return normalize(cross(glm::vec3(0,0,1),FindRightOfBlock())) ;}
void FindAxisOfRotationH(float dir)
{
    if(BlockRotatingUpdateH)
    {
        if( (BlockRotationStepsLeft--) == 0 ) BlockRotatingH = BlockRotatingUpdateH = false ;
        return  ;
    }
    BlockRotatingUpdateH = true ; BlockRotationStepsLeft = BlockRotationSteps - 1 ;
    auto &Block = Blocks[0] ;
    BlockRight = FindRightOfBlock() ;
    glm::vec3 normal = normalize(cross(Blocks[0].direction,Blocks[0].up)) ;
    if(abs(dot(BlockRight,Block.up)) > 0.9)
    {
        // cout<<"Right is along UP"<<endl ;
        // cout<<"current height is "<<FindCurrentHeight()<<endl ;
        BlockBodyRefPoint = BlockRight * dir * ((TileLength + TileWidth)/2) + glm::vec3(0,0,1) * ((float)-1*FindCurrentHeight()/2) ;
        BlockRotationFixedPoint = BlockBodyRefPoint + Block.location ;
        // cout<<"fixed point vector" ; FN(i,3) cout<<BlockRotationFixedPoint[i]<<" " ; cout<<endl ;
    }
    else
    {
        // cout<<"Right is not along UP"<<endl ;
        // cout<<"current height is "<<FindCurrentHeight()<<endl ;
        BlockBodyRefPoint = BlockRight * dir * (TileWidth/2) + glm::vec3(0,0,1) * ((float)-1*FindCurrentHeight()/2) ;
        BlockRotationFixedPoint = BlockBodyRefPoint + Block.location ;
    }
}
void MoveBlockH(float dir)
{
    auto &Block = Blocks[0] ;
    FindAxisOfRotationH(dir) ;
    Block.up = glm::rotate(Block.up,BlockRotateAngle*dir,normalize(cross(glm::vec3(0,0,1),BlockRight)));
    Block.direction = glm::rotate(Block.direction,BlockRotateAngle*dir,normalize(cross(glm::vec3(0,0,1),BlockRight))) ;
    BlockBodyRefPoint = glm::rotate(BlockBodyRefPoint,BlockRotateAngle*dir,normalize(cross(glm::vec3(0,0,1),BlockRight))) ;
    Block.location = BlockRotationFixedPoint - BlockBodyRefPoint ;
    if(!BlockRotatingUpdateH)
    {
        FN(i,3)
        {
            Block.up[i] = CorrectNumbers(Block.up[i]) ;
            Block.direction[i] = CorrectNumbers(Block.direction[i]) ;
            Block.location[i] = CorrectNumbers(Block.location[i]) ;
        }
        BlockPrevLocation.pb(Block.location) ;
        if(sz(BlockPrevLocation)) BlockPrevLocation.pop_front() ;
        BlockHasMoved = true ;
        ++NumberOfSteps ;
        cout<<"Level : "<<LevelNumber<<endl ;
        cout<<"Steps Taken Till Now : "<<NumberOfSteps<<endl ;
    }
}

void FindAxisOfRotationV(float dir)
{
    if(BlockRotatingUpdateV)
    {
        if( (BlockRotationStepsLeft--) == 0 ) BlockRotatingV = BlockRotatingUpdateV = false ;
        return  ;
    }
    BlockRotatingUpdateV = true ; BlockRotationStepsLeft = BlockRotationSteps - 1 ;
    auto &Block = Blocks[0] ;
    BlockFront = FindFrontOfBlock() ;
    glm::vec3 normal = normalize(cross(Blocks[0].direction,Blocks[0].up)) ;
    if(abs(dot(BlockFront,Block.up)) > 0.9)
    {
        BlockBodyRefPoint = BlockFront * dir * ((TileLength + TileWidth)/2) + glm::vec3(0,0,1) * ((float)-1*FindCurrentHeight()/2) ;
        BlockRotationFixedPoint = BlockBodyRefPoint + Block.location ;
    }
    else
    {
        BlockBodyRefPoint = BlockFront * dir * (TileWidth/2) + glm::vec3(0,0,1) * ((float)-1*FindCurrentHeight()/2) ;
        BlockRotationFixedPoint = BlockBodyRefPoint + Block.location ;
    }
}
void MoveBlockV(float dir)
{
    // cout<<"dir is "<<dir<<endl ;
    // cout<<"BlockRotationStepsLeft = "<<BlockRotationStepsLeft<<endl ;
    auto &Block = Blocks[0] ;
    FindAxisOfRotationV(dir) ;
    Block.up = glm::rotate(Block.up,BlockRotateAngle*dir,normalize(cross(glm::vec3(0,0,1),BlockFront)));
    Block.direction = glm::rotate(Block.direction,BlockRotateAngle*dir,normalize(cross(glm::vec3(0,0,1),BlockFront))) ;
    BlockBodyRefPoint = glm::rotate(BlockBodyRefPoint,BlockRotateAngle*dir,normalize(cross(glm::vec3(0,0,1),BlockFront))) ;
    // BlockTranslation = glm::translate(fixedpoint - point) ;
    Block.location = BlockRotationFixedPoint - BlockBodyRefPoint ;
    if(!BlockRotatingUpdateV)
    {
        FN(i,3)
        {
            Block.up[i] = CorrectNumbers(Block.up[i]) ;
            Block.direction[i] = CorrectNumbers(Block.direction[i]) ;
            Block.location[i] = CorrectNumbers(Block.location[i]) ;
        }
        BlockPrevLocation.pb(Block.location) ;
        if(sz(BlockPrevLocation)) BlockPrevLocation.pop_front() ;
        BlockHasMoved = true ;
        ++NumberOfSteps ;
        cout<<"Level : "<<LevelNumber<<endl ;
        cout<<"Steps Taken Till Now : "<<NumberOfSteps<<endl ;
    }
}
void BlockFall(void)
{
    auto &Block = Blocks[0] ;
    Block.location.z -= BlockFallingSpeed ;
    if(abs(Block.location.z) >= 2*CameraSphereRadius) Block.location.z = 2*CameraSphereRadius ;
}
glm::vec3 GetBlockScreenCor(void)
{
    auto &Block = Blocks[0] ;
    glm::mat4 model = RotateBlock(Block.direction,normalize(cross(Block.up,Block.direction)),Block.up) * glm::scale(Block.scale)     ;
    model = glm::translate(Block.location)* model ;
    glm::vec3 point = glm::vec3(model * glm::vec4(Block.location,1.0f)) ;
    point = glm::project(glm::vec3(0,0,0),Matrices.view,Matrices.projection,glm::vec4(0,0,ScreenWidth,ScreenHeight)) ;
    // cout<<"Points " ; FN(i,3) cout<<point[i]<<" " ; cout<<endl ;
    // point.x = ScreenWidth - point.x ;
    return point;
}
void MoveBlockMouse(GLFWwindow *window)
{
    // cout<<"Function Called"<<endl ;
    glm::vec3 Mouse = GetMouseCoordinates(window) ;
    glm::vec3 Block  = GetBlockScreenCor() ;
    glm::vec3 displacement = normalize(Mouse - Block) ;
    // cout<<"Displacement is " ; FN(i,3) cout<<displacement[i]<<" " ; cout<<endl ;
    if(abs(dot(displacement,glm::vec3(1,0,0))) > abs(dot(displacement,glm::vec3(0,1,0))))
    {
        BlockRotatingH = true ;
        if(displacement.x > 0) BlockMoveDir = 1 ;
        if(displacement.x < 0) BlockMoveDir = -1 ;
    }
    else
    {
        BlockRotatingV = true ;
        if(displacement.y > 0) BlockMoveDir = -1 ;
        if(displacement.y < 0) BlockMoveDir = 1 ;
    }
}
/*********************
    FALL DETECTION
**********************/
void CheckFall(void)
{
    BlockHasMoved = false ;
    auto &Block = Blocks[0] ;
    bool fall = true ;
    int BlockBase = 0 ;
    bool BlockAtEdge = false ;
    for(auto &tile:LiveFloor)
        if(abs(Block.location.x - tile.location.x) <= (TileLength/2) && abs(Block.location.y - tile.location.y) <= (TileWidth/2) )
        {
            ++BlockBase ;
            fall = false ;
            // break ;
        }
    FN(i,sz(ButtonHasBeenPressed)) if(ButtonHasBeenPressed[i])
    {
        for(auto &tile:HiddenFloor[i])
            if(abs(Block.location.x - tile.location.x) <= (TileLength/2) && abs(Block.location.y - tile.location.y) <= (TileWidth/2) )
            {
                ++BlockBase ;
                fall = false ;
                // break ;
            }
        if(!fall) break ;
    }
    for(auto &tile:FragileFloor)
        if(abs(Block.location.x - tile.location.x) <= (TileLength/2) && abs(Block.location.y - tile.location.y) <= (TileWidth/2) )
        {
            if(FindCurrentHeight() == (TileLength + TileWidth)) tile.fixed = false ;
            else fall = false , ++BlockBase ;
            // break ;
        }
    if(fall)
    {
        if(abs(Block.location.x - WinLoc.x) <= (TileLength/2) && abs(Block.location.y - WinLoc.y) <= (TileWidth/2) )
        {
            cout<<"Game Won"<<endl ;
            GameWon = true ;
            GameWinningTime = glfwGetTime();
        }
        BlockIsFalling = true ;
    }
    else if(BlockBase < 2 && FindCurrentHeight() < (TileLength + TileWidth))
    {
        // Check If the block is at the edge
        // cout<<"BlockBase "<<BlockBase<<endl ;
        glm::vec3 Center1 = Block.location + Block.up * ((TileLength + TileWidth)/4) ;
        glm::vec3 Center2 = Block.location - Block.up * ((TileLength + TileWidth)/4) ;
        if((Center1.x/TileLength + BoardLength/2) > (BoardLength - 1)
            || (Center1.x/TileLength + BoardLength/2) < 0
            || (Center1.y/TileWidth + BoardWidth/2) > (BoardWidth - 1)
            || (Center1.y/TileWidth + BoardWidth/2) < 0 ) BlockAtEdge = true ;

        if((Center2.x/TileLength + BoardLength/2) > (BoardLength - 1)
            || (Center2.x/TileLength + BoardLength/2) < 0
            || (Center2.y/TileWidth + BoardWidth/2) > (BoardWidth - 1)
            || (Center2.y/TileWidth + BoardWidth/2) < 0) BlockAtEdge = true ;
        // Block at the edge.! Block Must fall
        if(BlockAtEdge)
        {
            // cout<<"Block At Edge"<<endl ;
            glm::vec3 right = FindRightOfBlock() , front = FindFrontOfBlock() ;
            if(abs(dot(Block.up,right)) > 0.9)
            {
                // cout<<"Force Moving along H"<<endl ;
                BlockRotatingH = true ;
                if(dot(right,glm::vec3(0,0,0) - Block.location) > 0 ) BlockMoveDir = -1 ;
                else BlockMoveDir = 1 ;
            }
            else
            {
                // cout<<"Force Moving along V"<<endl ;
                BlockRotatingV = true ;
                if(dot(front,glm::vec3(0,0,0) - Block.location) > 0 ) BlockMoveDir = -1 ;
                else BlockMoveDir = 1 ;
            }
        }
    }
    if(!fall) CheckButtonPress() ;
    // else cout<<"Block will not fall"<<endl ;
}
/**********************
    BUTTONS
**********************/
void CreateButtons(void)
{
    GameObject temp ;
    temp.height = TileHeight/1.5 ;temp.width = TileWidth/3 ;temp.length = TileLength/3 ;
    temp.scale = glm::vec3(temp.width,temp.length,temp.height) ;
    temp.object = createCube(Textures["Button"]) ;
    temp.AxisOfRotation = glm::vec3(0,0,1) ;
    for(auto &it : ButtonList)
    {
        temp.location = glm::vec3((it.first - BoardWidth/2)*TileWidth,(it.second - BoardLength/2)*TileLength,-(TileLength+TileWidth)/2) ;
        Buttons.pb(temp) ;
    }
    ButtonHasBeenPressed.clear() ;
    ButtonHasBeenPressed.resize(sz(Buttons),false) ;
    ButtonList.clear() ;
}
void CheckButtonPress(void)
{
    auto &Block = Blocks[0] ;
    FN(i,sz(Buttons))
        if(abs(Block.location.x - Buttons[i].location.x) <= (TileLength/2) && abs(Block.location.y - Buttons[i].location.y) <= (TileWidth/2))
        {
            // cout<<"Button " << i <<" has been pressed"<<endl ;
            ButtonHasBeenPressed[i] = ButtonHasBeenPressed[i] ^ true ;
        }
}
/**********************
    FLOOR
***********************/
vector< vector<int> > GetGrid(void)
{
    string FileName = "Levels/Level" + to_string(LevelNumber) + ".txt" ;
    ifstream in(FileName.c_str()) ;
    if(!in.is_open())
    {
        cout<<"ConGo U have COmpleted the Game.!!!Winner!!!"<<endl ;
        pthread_join(Writer_thr[1],NULL);
        exit(0) ;
    }
    int rows = 0 , cols = 0 ,temp = 0 ;
    in>>rows>>cols ;
    vector< vector<int> > Grid(rows) ;
    FN(i,rows) FN(j,cols)
    {
        in>>temp ;
        Grid[i].pb(temp) ;
    }
    in.close() ;
    return Grid ;
}
void CreateFloor(void)
{
    LiveFloor.clear() , HiddenFloor.clear() ; FragileFloor.clear() ;
    vector< vector<int> > Grid = GetGrid() ;
    int mx = 0 ;
    for(auto &vec:Grid) for(auto &it:vec) mx = max(mx,it) ;
    HiddenFloor.resize(mx/2) ;

    BoardLength = sz(Grid) , BoardWidth = sz(Grid[0]);
    float delta = 0.2 ;

    GameObject lblock ;
    lblock.height = TileHeight ;lblock.width = TileWidth ;lblock.length = TileLength ;
    lblock.scale = glm::vec3(lblock.width - delta,lblock.length - delta,lblock.height) ;
    lblock.object = createCube(Textures["Brick"]) ;
    lblock.AxisOfRotation = glm::vec3(0,0,1) ;

    GameObject hblock ;
    hblock.height = TileHeight ;hblock.width = TileWidth ;hblock.length = TileLength ;
    hblock.scale = glm::vec3(hblock.width - delta,hblock.length - delta,hblock.height) ;
    hblock.object = createCube(Textures["HiddenBrick"]) ;
    hblock.AxisOfRotation = glm::vec3(0,0,1) ;

    GameObject fblock ;
    fblock.height = TileHeight ;fblock.width = TileWidth ;fblock.length = TileLength ;
    fblock.scale = glm::vec3(fblock.width - delta,fblock.length - delta,fblock.height) ;
    fblock.object = createCube(Textures["FragileBrick"]) ;
    fblock.AxisOfRotation = glm::vec3(0,0,1) ;
    fblock.fixed = true ;

    ButtonList.clear() ; Buttons.clear() ; ButtonList.resize(mx/2) ;
    FN(i,BoardLength) FN(j,BoardWidth)
    {
        if(Grid[i][j] == 1 || (Grid[i][j]%2 == 0 && Grid[i][j] > 0 ) )
        {
            lblock.location = glm::vec3((i - BoardWidth/2)*TileWidth,(j - BoardLength/2)*TileLength, -(TileLength + TileWidth)/2 - TileHeight/2) ;
            LiveFloor.pb(lblock) ;
        }
        else if(Grid[i][j] > 0)
        {
            hblock.location = glm::vec3((i - BoardWidth/2)*TileWidth,(j - BoardLength/2)*TileLength, -(TileLength + TileWidth)/2 - TileHeight/2) ;
            HiddenFloor[Grid[i][j]/2 - 1].pb(hblock) ;
        }
        else if(Grid[i][j] == -1)
        {
            fblock.location = glm::vec3((i - BoardWidth/2)*TileWidth,(j - BoardLength/2)*TileLength, -(TileLength + TileWidth)/2 - TileHeight/2) ;
            FragileFloor.pb(fblock) ;
        }
        else if(Grid[i][j] == -2)
            WinLoc = glm::vec3((i - BoardWidth/2)*TileWidth,(j - BoardLength/2)*TileLength, -(TileLength + TileWidth)/2 - TileHeight/2) ;

        if(Grid[i][j]%2 == 0 && Grid[i][j] > 0) ButtonList[Grid[i][j]/2 - 1] = {i,j} ;
    }
    CreateButtons() ;
    AlignBlock(Grid) ;
}
/***************************
    SKYLINE BOX
****************************/
void CreateSkylineBox(void)
{
    GameObject temp ;
    temp.height = temp.width = temp.length = 10*CameraSphereRadius ;
    temp.scale = glm::vec3(temp.width,temp.length,temp.height) ;
    temp.object = createCube(Textures["SkylineBox"]) ;
    temp.AxisOfRotation = glm::vec3(0,0,1) ;
    temp.up = glm::vec3(0,0,1) ;
    temp.direction = glm::vec3(-1,0,0) ;
    temp.location = glm::vec3(0,0,0) ;
    SkylineBox.pb(temp) ;
}
/***************************
    TEXTURES
****************************/
void LoadTextures(void)
{
    Textures["Box"] = createTexture("Images/res_texture.png");
    Textures["Brick"] = createTexture("Images/Brick.jpg");
    Textures["HiddenBrick"] = createTexture("Images/hiddenbrick.png");
    Textures["Button"] = createTexture("Images/button.jpg");
    Textures["SkylineBox"] = createTexture("Images/sky.png");
    Textures["FragileBrick"] = createTexture("Images/fragilebrick.jpg");
}
/***************************
    GAME
***************************/
void SetGame(void)
{
    Blocks.clear() ;
    BlockPrevLocation.clear() ;
    BlockPrevLocation.pb(glm::vec3(1,0,0)) ;
    InitCamera() ;
    CreateBlocks() ;
    CreateFloor();
    NumberOfSteps = 0 ;
    cout<<"Level : "<<LevelNumber<<endl ;
    cout<<"Steps Taken Till Now : "<<NumberOfSteps<<endl ;
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

	window = glfwCreateWindow(width, height, "Shake The Block", NULL, NULL);

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
    LoadTextures() ;
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    // rectangle = createCube() ;
    // for follow cam view
    SetGame() ;
    CreateSkylineBox() ;
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
    srand(glfwGetTime()) ;
    GLFWwindow* window = initGLFW(ScreenWidth, ScreenHeight);
    // initGLEW();
    initGL (window, ScreenWidth,ScreenHeight);
    pthread_create(&Writer_thr[1],NULL,Writer,(void*) NULL);
    last_update_time = glfwGetTime();
    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {


        // OpenGL Draw commands
	current_time = glfwGetTime();

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    last_update_time = current_time;
    draw(window,0,0,1,1, 1, 1, 1);
    GameObject OldCamera = Camera ;
    SetTopView() ;
    draw2(window,0.75,0.75,0.25,0.25, 1, 1, 1);
    Camera = OldCamera ; UpdateCamera() ;
    glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();
        while(PauseGame) glfwPollEvents();
    }
    pthread_join(Writer_thr[1],NULL);
    glfwTerminate();
    //    exit(EXIT_SUCCESS);
}
