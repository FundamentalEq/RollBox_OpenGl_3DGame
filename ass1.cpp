#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
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

// Screen Extremities
float LeftExtreme = -400.0 ;
float RightExtreme = 400.0 ;
float UpExtreme = 300.0 ;
float DownExtreme = -300.0 ;
float XWidth,YWidth ;
float GamePlayDownExtreme = DownExtreme + 50 ;
float GamePlayUpExtreme = UpExtreme - 100 ;
// Speed
float SpeedY = (UpExtreme - DownExtreme)/100.0 ;
float SpeedX = (RightExtreme - LeftExtreme)/100.0 ;
float SpeedLaser = (SpeedX + SpeedY)/2 ;
int SpeedoMeter = 0 ;
float BlockSpeedChange = 1.2 ;
// Direction
int GoUp = -1 ;
int GoDown = 1 ;

// Global Iterators
int LaserNumber ;
int BlockNumber ;
// Timers
double LastLaserUpdateTime = glfwGetTime();
double LastBlockUpdateTime = glfwGetTime();
double PanTimer = glfwGetTime() ;
double ScoreTimer = glfwGetTime() ;
double RotateMirrorTimer = glfwGetTime() ;
double current_time;
// KillList
vi KillThem ;
// Camera
float x_change = 0; //For the camera pan
float y_change = 0; //For the camera pan
float zoom_camera = 1;
//Mouse keys
bool RightMouseKeyOn ;
bool LeftMouseKeyOn ;
bool LAltOn ;
bool RAltOn ;
bool LCtrlOn ;
bool RCtrlOn ;
bool Pause ;
bool EnableMouseShooting ;
bool CannonSelected ;
bool BucketRedSelected ;
bool BucketGreenSelected ;
bool RotateMirrorAllowed = true ;
bool FreezeGame ;
glm::vec3 SavedMouseCoor ;
// Degree to Radians
double D2R = acos((double)-1) /180.0 ;
//Rotation Matricies
glm::mat3 RotateCloclWise = glm::mat3(glm::vec3(cos(D2R*10),-sin(D2R*10),0),glm::vec3(sin(D2R*10),cos(D2R*10),0),glm::vec3(0,0,1)) ;
glm::mat3 RotateAntiCloclWise = glm::mat3(glm::vec3(cos(D2R*10),sin(D2R*10),0),glm::vec3(-sin(D2R*10),cos(D2R*10),0),glm::vec3(0,0,1)) ;
// Score & Lives
int Score = 0 ;
int EarnedScore = 5 ;
int LostScore = -10 ;
int WinningScore = 500 ;
int LoosingScore = -50 ;
int Lives = 3 ;
int WrongKills = 0 ;
int AllowedWrongKills = 5 ;
struct GameObject
{
    glm::vec3 location,CenterOfRotation , direction , gravity , speed ;
    float height,width,theta,radius ;
    bool fixed,isRotating ;
    int ID,color;
    struct VAO * object ;
    GameObject(void)
    {
        fixed = true , isRotating = false ;
        height = width = theta = radius = 0 ;
        ID = color = 0 ;
        location = CenterOfRotation = direction = gravity = speed = glm::vec3(0,0,0) ;
        object = NULL ;
    }
} ;
// Function Declaration
void MoveCannon(int) ;
void RotateCannon(GLFWwindow*) ;
void CreateLaser(void) ;
void MoveLasers(void) ;
void DetectCollisions(void) ;
void CreateBlocks(void) ;
void MoveBlocks(void) ;
void check_pan(void) ;
void mousescroll(GLFWwindow*, double, double) ;
glm::vec3 GetMouseCoordinates(GLFWwindow*) ;
void MoveBucket(int,int,bool,glm::vec3) ;
void RotateCannonKey(int) ;
void CheckForSelection(GLFWwindow*) ;
void MoveCannonMouse(glm::vec3) ;
void RotateMirrors(void) ;
int RandomNo(int) ;
void ChangeBlockSpeed(int) ;
// Global Variables
bool CursorOnScreen ;
vector< GameObject > Cannon ;
vector< GameObject > Background ;
map<int,GameObject> Lasers ;
vector< GameObject > Mirrors ;
map<int,GameObject > Blocks ;
vector< GameObject > Bucket ;
vector< GameObject > LifeBlocks ;
GLuint programID;


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
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

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
//    exit(EXIT_SUCCESS);
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

/**************************
 * Customizable functions *
 **************************/


/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
     // Function is called first on GLFW_PRESS.
    //  cout<<"Pressed key is "<<key<<endl ;
    if (action == GLFW_RELEASE)
    {
        switch (key) {
            case GLFW_KEY_S: MoveCannon(GoUp) ;break;
            case GLFW_KEY_F: MoveCannon(GoDown) ;break;
            case GLFW_KEY_UP : mousescroll(window,0,1) ; break ;
            case GLFW_KEY_DOWN : mousescroll(window,0,-1) ; break ;
            case GLFW_KEY_RIGHT: break;
            case GLFW_KEY_LEFT: break;
            case GLFW_KEY_LEFT_ALT : LAltOn = false ; break ;
            case GLFW_KEY_RIGHT_ALT : RAltOn = false ; break ;
            case GLFW_KEY_LEFT_CONTROL : LCtrlOn = false ; break ;
            case GLFW_KEY_RIGHT_CONTROL : RCtrlOn = false ; break ;
            case GLFW_KEY_SPACE : CreateLaser() ; break ;
            default:
                break;
        }
    }
    else if (action == GLFW_PRESS)
    {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                quit(window);
                break;
            case GLFW_KEY_LEFT_ALT : LAltOn = true ; break ;
            case GLFW_KEY_RIGHT_ALT : RAltOn = true ; break ;
            case GLFW_KEY_LEFT_CONTROL : LCtrlOn = true ; break ;
            case GLFW_KEY_RIGHT_CONTROL : RCtrlOn = true ; break ;
            case GLFW_KEY_P : Pause ^=1 ; break ;
            case GLFW_KEY_A : RotateCannonKey(1) ; break ;
            case GLFW_KEY_D : RotateCannonKey(-1) ; break ;
            case GLFW_KEY_E : EnableMouseShooting ^= 1 ; break ;
            case GLFW_KEY_R : RotateMirrorAllowed ^= 1 ; break ;
            case GLFW_KEY_N : ChangeBlockSpeed(-1) ; break ;
            case GLFW_KEY_M : ChangeBlockSpeed(1) ; break ;
            case GLFW_KEY_RIGHT :
                if(LAltOn || RAltOn) MoveBucket(1,1,0,glm::vec3(0,0,0)) ;
                if(LCtrlOn || RCtrlOn) MoveBucket(0,1,0,glm::vec3(0,0,0)) ;
                break ;
            case GLFW_KEY_LEFT :
                if(LAltOn || RAltOn) MoveBucket(1,-1,0,glm::vec3(0,0,0)) ;
                if(LCtrlOn || RCtrlOn) MoveBucket(0,-1,0,glm::vec3(0,0,0)) ;
                break ;
            default:
                break;
        }
    }
    else if (action == GLFW_REPEAT)
    {
        switch(key)
        {
            case GLFW_KEY_S : MoveCannon(GoUp) ; break ;
            case GLFW_KEY_F : MoveCannon(GoDown) ; break ;
            case GLFW_KEY_A : RotateCannonKey(1) ; break ;
            case GLFW_KEY_D : RotateCannonKey(-1) ; break ;
            case GLFW_KEY_RIGHT :
                if(LAltOn || RAltOn) MoveBucket(1,1,0,glm::vec3(0,0,0)) ;
                if(LCtrlOn || RCtrlOn) MoveBucket(0,1,0,glm::vec3(0,0,0)) ;
                break ;
            case GLFW_KEY_LEFT :
                if(LAltOn || RAltOn) MoveBucket(1,-1,0,glm::vec3(0,0,0)) ;
                if(LCtrlOn || RCtrlOn) MoveBucket(0,-1,0,glm::vec3(0,0,0)) ;
                break ;
            default : break ;
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
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if(action == GLFW_PRESS)
            {
                LeftMouseKeyOn = true ;
                CheckForSelection(window) ;
            }
            if (action == GLFW_RELEASE)
            {
                LeftMouseKeyOn = false ;
                CannonSelected = BucketRedSelected = BucketGreenSelected = false ;
                if(EnableMouseShooting) CreateLaser() ;
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS) RightMouseKeyOn = true ,SavedMouseCoor = GetMouseCoordinates(window) ;
            if (action == GLFW_RELEASE) RightMouseKeyOn = false ;
            break;
        default:
            break;
    }
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
     is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);
    Matrices.projection = glm::ortho(LeftExtreme, RightExtreme, UpExtreme, DownExtreme, 0.1f, 500.0f);
}

void createTriangle ()
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

  /* Define vertex array as used in glBegin (GL_TRIANGLES) */
  static const GLfloat vertex_buffer_data [] = {
    1, -4,1, // vertex 0
    -1,-1,0, // vertex 1
    1,-1,0, // vertex 2
  };

  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 0
    0,1,0, // color 1
    0,0,1, // color 2
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  // triangle = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
}

// Creates the rectangle object used in this sample code
VAO* createRectangle (COLOR colorA, COLOR colorB, COLOR colorC, COLOR colorD, float width, float height)
{
    // GL3 accepts only Triangles. Quads are not supported
    float w=width/2,h=height/2;
    GLfloat vertex_buffer_data [] = {
        -w,-h,0, // vertex 1
        -w,h,0, // vertex 2
        w,h,0, // vertex 3

        w,h,0, // vertex 3
        w,-h,0, // vertex 4
        -w,-h,0  // vertex 1
    };

    GLfloat color_buffer_data [] = {
        colorA.r,colorA.g,colorA.b, // color 1
        colorB.r,colorB.g,colorB.b, // color 2
        colorC.r,colorC.g,colorC.b, // color 3

        colorC.r,colorC.g,colorC.b, // color 4
        colorD.r,colorD.g,colorD.b, // color 5
        colorA.r,colorA.g,colorA.b // color 6
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    return create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
VAO* CreateCircle(COLOR color,float radius,int parts,bool fill)
{
    GLfloat vertex_buffer_data[parts*9];
    GLfloat color_buffer_data[parts*9];
    int i,j;
    float angle=(2*M_PI/parts);
    float current_angle = 0;
    FN(i,parts)
    {
        FN(j,3)
        {
            color_buffer_data[i*9+j*3]=color.r;
            color_buffer_data[i*9+j*3+1]=color.g;
            color_buffer_data[i*9+j*3+2]=color.b;
        }
        vertex_buffer_data[i*9]=0;
        vertex_buffer_data[i*9+1]=0;
        vertex_buffer_data[i*9+2]=0;
        vertex_buffer_data[i*9+3]=radius*cos(current_angle);
        vertex_buffer_data[i*9+4]=radius*sin(current_angle);
        vertex_buffer_data[i*9+5]=0;
        vertex_buffer_data[i*9+6]=radius*cos(current_angle+angle);
        vertex_buffer_data[i*9+7]=radius*sin(current_angle+angle);
        vertex_buffer_data[i*9+8]=0;
        current_angle+=angle;
    }
    if(fill) return create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_FILL);
    else return create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_LINE);
}
/*****************
    ZOOM
*****************/
void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset==-1) zoom_camera /= 1.1; //make it bigger than current size
    else if(yoffset==1)   zoom_camera *= 1.1; //make it bigger than current size
    if (zoom_camera<=1)   zoom_camera = 1;
    if (zoom_camera>=4)   zoom_camera=4;

    if(x_change + LeftExtreme/zoom_camera < LeftExtreme)  x_change=LeftExtreme - LeftExtreme/zoom_camera;
    else if(x_change+RightExtreme/zoom_camera>RightExtreme) x_change=RightExtreme-RightExtreme/zoom_camera;

    if(y_change+ DownExtreme/zoom_camera< DownExtreme) y_change=DownExtreme + DownExtreme/zoom_camera;
    else if(y_change+UpExtreme/zoom_camera > UpExtreme) y_change=UpExtreme - UpExtreme/zoom_camera;
    Matrices.projection = glm::ortho((float)(LeftExtreme/zoom_camera+x_change), (float)(RightExtreme/zoom_camera+x_change), (float)(UpExtreme/zoom_camera+y_change), (float)(DownExtreme/zoom_camera+y_change), 0.1f, 500.0f);
}
void check_pan()
{
    if(x_change-400.0f/zoom_camera<-400)
        x_change=-400+400.0f/zoom_camera;
    else if(x_change+400.0f/zoom_camera>400)
        x_change=400-400.0f/zoom_camera;
    if(y_change-300.0f/zoom_camera<-300)
        y_change=-300+300.0f/zoom_camera;
    else if(y_change+300.0f/zoom_camera>300)
        y_change=300-300.0f/zoom_camera;
    Matrices.projection = glm::ortho((float)(LeftExtreme/zoom_camera+x_change), (float)(RightExtreme/zoom_camera+x_change), (float)(UpExtreme/zoom_camera+y_change), (float)(DownExtreme/zoom_camera+y_change), 0.1f, 500.0f);

}
void DoPan(GLFWwindow* window)
{
    glm::vec3 Mouse = GetMouseCoordinates(window) - SavedMouseCoor ;
    if(Mouse[0]>10) x_change += 10 ;
    else if(Mouse[0]<-10) x_change -= 10 ;
    if(Mouse[1]>10) y_change += 10 ;
    if(Mouse[1]<-10) y_change -= 10 ;
    check_pan() ;
}
/********************
    CURSOR
*********************/
void cursor_enter_callback(GLFWwindow* window, int entered)
{
    if (entered) CursorOnScreen = 1 ;
    else CursorOnScreen = 0 ;
}
// Returns the mouse coordinates translated according to our coordinate system
glm::vec3 GetMouseCoordinates(GLFWwindow* window)
{
    double CursorX,CursorY ;
    glfwGetCursorPos(window, &CursorX, &CursorY) ;
    return glm::vec3(CursorX+LeftExtreme,CursorY+DownExtreme,0) ;
}
// find angle from A to B : assuming both are normalized vectors
float FindAngle(glm::vec3 A,glm::vec3 B)
{
    float theta = acos(dot(A,B)) ;
    if(cross(A,B)[2] < 0 ) theta *= -1 ;
    return theta ;
}
glm::vec3 FindCurrentDirection(glm::vec3 A,glm::vec3 B)
{
    glm::vec3 C = A-B ;
    if(C == glm::vec3(0,0,0)) return glm::vec3(1,0,0) ;
    return normalize(C) ;
}
void draw (GLFWwindow* window)
{
    if(Pause||FreezeGame) return ;
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram (programID);

    Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

    glm::mat4 VP = Matrices.projection * Matrices.view;

    glm::mat4 MVP;	// MVP = Projection * View * Model
    if(EnableMouseShooting) RotateCannon(window) ;
    if(BucketRedSelected) MoveBucket(0, 0, true, GetMouseCoordinates(window)) ;
    if(BucketGreenSelected) MoveBucket(1, 0, true, GetMouseCoordinates(window)) ;
    if(CannonSelected) MoveCannonMouse(GetMouseCoordinates(window)) ;
    DetectCollisions() ;
    if(WrongKills > AllowedWrongKills) --Lives ;
    // Enforce Game Rules
    if(Lives == 0 || Score < LoosingScore)
    {
        cout<<"YOU LOOSE"<<endl ;
        FreezeGame = true ;
    }
    if(Score >= WinningScore)
    {
        cout<<"YOU WIN"<<endl ;
        FreezeGame = true ;
    }
    if(current_time - ScoreTimer >= 0.5)
    {
        ScoreTimer = current_time ;
        cout<<"**************************************"<<endl ;
        cout<<"Score : "<<Score<<endl ;
        cout<<"Lives : "<<Lives<<endl ;
        cout<<"Wrong Kills : "<<WrongKills<<endl ;
        cout<<"Speed : "<<SpeedoMeter<<endl ;
        cout<<"**************************************"<<endl ;
    }

    // Rotate Mirror
    if(current_time - RotateMirrorTimer >= 0.05 && RotateMirrorAllowed)
    {
        RotateMirrorTimer = current_time ;
        RotateMirrors() ;
    }
    if(current_time - LastLaserUpdateTime >=0.005)
    {
        LastLaserUpdateTime = current_time ;
        MoveLasers() ;
        MoveBlocks() ;
    }
    if(current_time - LastBlockUpdateTime >= 0.5)
    {
        LastBlockUpdateTime = current_time ;
        CreateBlocks() ;
    }
    if(RightMouseKeyOn && current_time - PanTimer >= 0.1)
    {
        PanTimer = current_time ;
        DoPan(window) ;
    }
    for(auto it:Cannon)
    {
        Matrices.model = glm::translate (it.location);
        if(it.isRotating)
        {
            Matrices.model = glm::translate (it.CenterOfRotation*(float)-1 ) * Matrices.model ;
            float theta = FindAngle(normalize(it.location - it.CenterOfRotation),it.direction) ;
            Matrices.model = glm::rotate(theta, glm::vec3(0,0,1)) * Matrices.model ;
            Matrices.model = glm::translate (it.CenterOfRotation) * Matrices.model ;
        }
        MVP = VP * Matrices.model; // MVP = p * V * M
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it.object);
    }
    for(auto it:Background)
    {
        Matrices.model = glm::translate (it.location);
        if(it.isRotating)
        {
            Matrices.model = glm::translate (it.CenterOfRotation*(float)-1 ) * Matrices.model ;
            float theta = FindAngle(normalize(it.location - it.CenterOfRotation),it.direction) ;
            Matrices.model = glm::rotate(theta, glm::vec3(0,0,1)) * Matrices.model ;
            Matrices.model = glm::translate (it.CenterOfRotation) * Matrices.model ;
        }
        MVP = VP * Matrices.model; // MVP = p * V * M
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it.object);
    }
    for(auto it2:Lasers)
    {
        auto it = it2.second ;
        Matrices.model = glm::translate (it.location);
        if(it.isRotating)
        {
            Matrices.model = glm::translate (it.CenterOfRotation*(float)-1 ) * Matrices.model ;
            float theta = FindAngle(FindCurrentDirection(it.location,it.CenterOfRotation),it.direction) ;
            Matrices.model = glm::rotate(theta, glm::vec3(0,0,1)) * Matrices.model ;
            Matrices.model = glm::translate (it.CenterOfRotation) * Matrices.model ;
        }
        MVP = VP * Matrices.model; // MVP = p * V * M
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it.object);
    }
    for(auto it:Mirrors)
    {
        Matrices.model = glm::translate (it.location);
        if(it.isRotating)
        {
            Matrices.model = glm::translate (it.CenterOfRotation*(float)-1 ) * Matrices.model ;
            float theta = FindAngle(FindCurrentDirection(it.location,it.CenterOfRotation),it.direction) ;
            Matrices.model = glm::rotate(theta, glm::vec3(0,0,1)) * Matrices.model ;
            Matrices.model = glm::translate (it.CenterOfRotation) * Matrices.model ;
        }
        MVP = VP * Matrices.model; // MVP = p * V * M
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it.object);
    }
    for(auto it2:Blocks)
    {
        auto it = it2.second ;
        Matrices.model = glm::translate (it.location);
        if(it.isRotating)
        {
            Matrices.model = glm::translate (it.CenterOfRotation*(float)-1 ) * Matrices.model ;
            float theta = FindAngle(FindCurrentDirection(it.location,it.CenterOfRotation),it.direction) ;
            Matrices.model = glm::rotate(theta, glm::vec3(0,0,1)) * Matrices.model ;
            Matrices.model = glm::translate (it.CenterOfRotation) * Matrices.model ;
        }
        MVP = VP * Matrices.model; // MVP = p * V * M
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it.object);
    }
    for(auto it:Bucket)
    {
        Matrices.model = glm::translate (it.location);
        if(it.isRotating)
        {
            Matrices.model = glm::translate (it.CenterOfRotation*(float)-1 ) * Matrices.model ;
            float theta = FindAngle(normalize(it.location - it.CenterOfRotation),it.direction) ;
            Matrices.model = glm::rotate(theta, glm::vec3(0,0,1)) * Matrices.model ;
            Matrices.model = glm::translate (it.CenterOfRotation) * Matrices.model ;
        }
        MVP = VP * Matrices.model; // MVP = p * V * M
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it.object);
    }
    FN(i,Lives)
    {
        auto it = LifeBlocks[i] ;
        Matrices.model = glm::translate (it.location);
        if(it.isRotating)
        {
            Matrices.model = glm::translate (it.CenterOfRotation*(float)-1 ) * Matrices.model ;
            float theta = FindAngle(normalize(it.location - it.CenterOfRotation),it.direction) ;
            Matrices.model = glm::rotate(theta, glm::vec3(0,0,1)) * Matrices.model ;
            Matrices.model = glm::translate (it.CenterOfRotation) * Matrices.model ;
        }
        MVP = VP * Matrices.model; // MVP = p * V * M
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it.object);
    }
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
       exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Shoot It Out", NULL, NULL);

    if (!window) {
        glfwTerminate();
//        exit(EXIT_FAILURE);
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
    glfwSetScrollCallback(window, mousescroll);
    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */

/*****************
    CANNON
*****************/
void CreateCannon(void)
{
    GameObject temp ;
    COLOR BaseCannonColor = skyblue ;

    temp.radius = 20 ;
    temp.object = CreateCircle(BaseCannonColor,temp.radius,25,1) ;
    temp.location = glm::vec3(LeftExtreme+20,0,0);
    temp.speed = glm::vec3(0,SpeedY,0) ;
    Cannon.pb(temp) ;

    temp.radius = 10 ;
    temp.object = CreateCircle(blue,temp.radius,25,1) ;
    temp.location = glm::vec3(LeftExtreme+50,0,0);
    temp.speed = glm::vec3(0,SpeedY,0) ;
    temp.CenterOfRotation = glm::vec3(LeftExtreme+20,0,0) ;
    temp.direction = glm::vec3(1,0,0) ;
    temp.isRotating = true ;
    Cannon.pb(temp) ;
}
void MoveCannon(int dir)
{
     GameObject &big = Cannon[0] ;
     glm::vec3 temp = big.location + big.speed*(float)dir ;
     if(temp[1] + big.radius >= GamePlayUpExtreme || temp[1] - big.radius <= GamePlayDownExtreme) return ;
     for(auto &it:Cannon)
     {
         it.location = it.location + it.speed*(float)dir ;
         if(it.isRotating) it.CenterOfRotation += it.speed*(float)dir ;
     }
 }
 void MoveCannonMouse(glm::vec3 Mouse)
 {
     GameObject &big = Cannon[0] ;
     if(Mouse[1] + big.radius >= GamePlayUpExtreme || Mouse[1] - big.radius <= GamePlayDownExtreme) return ;
     for(auto &it:Cannon)
     {
         it.location[1] = Mouse[1] ;
         if(it.isRotating) it.CenterOfRotation[1] = Mouse[1] ;
     }
 }
void RotateCannon(GLFWwindow* window)
{
    if(!CursorOnScreen) return ;
    glm::vec3 Mouse = GetMouseCoordinates(window) ;
    for(auto &it:Cannon) if(it.isRotating) it.direction = normalize(Mouse - it.CenterOfRotation) ;
}
void RotateCannonKey(int Clock)
{
    cout<<"rck called with clock = "<<Clock<<endl ;
    if(Clock == 1) for(auto &it:Cannon) if(it.isRotating)
        it.direction = RotateCloclWise * it.direction ;
    if(Clock == -1) for(auto &it:Cannon) if(it.isRotating)
        it.direction = RotateAntiCloclWise * it.direction ;
}
/******************
    BACKGROUND
*******************/
void CreateBackGround(void)
{
    GameObject temp ;
    COLOR BaseBGColor = white ;

    //Bucket Line
    temp.height = 5 ,temp.width = XWidth ;
    temp.object =  createRectangle(BaseBGColor,BaseBGColor,BaseBGColor,BaseBGColor,XWidth,5);
    temp.location = glm::vec3(0,GamePlayUpExtreme - temp.height/2 ,0) ;
    Background.pb(temp) ;

    // Top Line
    temp.height = 5 ,temp.width = XWidth ;
    temp.object =  createRectangle(BaseBGColor,BaseBGColor,BaseBGColor,BaseBGColor,XWidth,5);
    temp.location = glm::vec3(0,GamePlayDownExtreme - temp.height/2 ,0) ;
    Background.pb(temp) ;
}
/*****************
    LASER
******************/
void CreateLaser(void)
{
    GameObject temp ;
    COLOR BaseBGColor = red ;

    // cout<<"Laser Shot"<<endl ;
    temp.width = 20 , temp.height = 5 ;
    temp.direction = Cannon[1].direction ;
    temp.speed = temp.direction * SpeedLaser ;
    temp.location = Cannon[0].location + Cannon[1].direction*(float)(Cannon[0].radius + 2.0*Cannon[1].radius + temp.width/2.0) ;
    temp.CenterOfRotation = temp.location ;
    temp.ID = LaserNumber++ ;
    temp.isRotating = true ;
    temp.object = createRectangle(BaseBGColor,BaseBGColor,BaseBGColor,BaseBGColor,temp.width,temp.height) ;
    Lasers[temp.ID] = temp ;
}
void MoveLasers(void)
{
    glm::vec3 nose ;
    for(auto &it2:Lasers)
    {
        auto &it = it2.second ;
        it.location = it.CenterOfRotation = it.location + it.speed ;
        nose = it.location + it.direction*it.width ;
        if(nose[1]<=GamePlayDownExtreme || nose[1]>=GamePlayUpExtreme || nose[0]<=LeftExtreme ||nose[0]>=RightExtreme)
            KillThem.pb(it.ID) ;
    }
    for(auto id:KillThem) Lasers.erase(id);//cout<<"Laser Killed"<<endl ;
    KillThem.clear() ;
}
/*******************
    MIRROR
********************/
void CreateMirror(void)
{
    GameObject temp ;
    COLOR BaseMirrorColor = white ;

    temp.height = 5 ,temp.width = 100 ;
    temp.location = glm::vec3(50,50,0) ;
    temp.CenterOfRotation = temp.location ;
    temp.direction = normalize(glm::vec3(1,-1,0)) ;
    temp.isRotating = true ;
    temp.object =  createRectangle(BaseMirrorColor,BaseMirrorColor,BaseMirrorColor,BaseMirrorColor,temp.width,temp.height);
    Mirrors.pb(temp) ;

    temp.height = 5 ,temp.width = 100 ;
    temp.location = glm::vec3(150,-100,0) ;
    temp.CenterOfRotation = temp.location ;
    temp.direction = normalize(glm::vec3(1,-1,0)) ;
    temp.isRotating = true ;
    temp.object =  createRectangle(BaseMirrorColor,BaseMirrorColor,BaseMirrorColor,BaseMirrorColor,temp.width,temp.height);
    Mirrors.pb(temp) ;

    temp.height = 5 ,temp.width = 50 ;
    temp.location = glm::vec3(-100,-100,0) ;
    temp.CenterOfRotation = temp.location ;
    temp.direction = normalize(glm::vec3(1,-1,0)) ;
    temp.isRotating = true ;
    temp.object =  createRectangle(BaseMirrorColor,BaseMirrorColor,BaseMirrorColor,BaseMirrorColor,temp.width,temp.height);
    Mirrors.pb(temp) ;
}
void RotateMirrors(void) { for(auto &mirror:Mirrors)  mirror.direction = RotateCloclWise * mirror.direction ; }
/*******************
    COLLISIONS
********************/
bool CheckRectangleCollision(GameObject &r1,GameObject &r2)
{
    glm::vec3 prependicular = normalize(cross(r1.direction,glm::vec3(0,0,1))) ;
    glm::vec3 Points[4] ;
    Points[0] = r1.location + r1.direction*(r1.width/2) + prependicular*(r1.height/2) ;
    Points[1] = r1.location + r1.direction*(r1.width/2) - prependicular*(r1.height/2) ;
    Points[2] = r1.location - r1.direction*(r1.width/2) + prependicular*(r1.height/2) ;
    Points[3] = r1.location - r1.direction*(r1.width/2) - prependicular*(r1.height/2) ;

    prependicular = normalize(cross(glm::vec3(0,0,1),r2.direction)) ;
    FN(i,4) if(abs(dot(Points[i] - r2.location,prependicular))<= r2.height/2.0 && abs(dot(Points[i] - r2.location,r2.direction)) <= r2.width/2.0 )
            return true ;
    return false ;
}
void reflect(GameObject &r1,glm::vec3 &location,glm::vec3 direction)
{
    glm::vec3 prependicular = normalize(cross(direction,glm::vec3(0,0,1))),num,deno,X ;
    if(dot(prependicular,r1.location - location) < 0 ) prependicular = prependicular * (float)-1 ;
    num = cross(r1.location - location,r1.direction) ;
    deno = cross(direction,r1.direction) ;
    float t = glm::length(num) / glm::length(deno) ;
    if(dot(num,deno) < 0 ) t*=-1 ;
    X = location + direction * t ;
    r1.direction += X ;
    r1.direction = prependicular*abs(dot(r1.direction,prependicular)) + direction*(dot(r1.direction,direction)) ;
    r1.direction -= X ;
    r1.direction = normalize(r1.direction) ;
    r1.location -= X ;
    r1.location = prependicular*abs(dot(r1.location,prependicular)) + direction*(dot(r1.location,direction)) ;
    r1.location += X ;
    r1.CenterOfRotation = r1.location ;
    r1.speed = r1.direction * SpeedLaser ;
}
void DetectCollisions(void)
{
    //Collisions between laser and mirrors
    for(auto &mirror:Mirrors) for(auto &it2:Lasers)
    {
        auto &laser = it2.second ;
        if(CheckRectangleCollision(laser,mirror) ) reflect(laser,mirror.location,mirror.direction) ;
    }

    // Collisions between laser and blocks
    for(auto &it2:Lasers)
    {
        auto &laser = it2.second ;
        for(auto &it:Blocks)
        {
            auto &block = it.second ;
            if(CheckRectangleCollision(laser,block))
            {
                //block is of black color
                if(block.color == 2) Score += EarnedScore ,WrongKills = 0;
                else ++WrongKills ;
                Blocks.erase(block.ID) ;
                KillThem.pb(laser.ID) ;
                break;
            }
        }
    }
    for(auto id:KillThem) Lasers.erase(id) ;
    KillThem.clear() ;

    //Collisions between blocks and mirrors
    for(auto &mirror:Mirrors) for(auto &it2:Blocks)
    {
        auto &block = it2.second ;
        if(CheckRectangleCollision(block,mirror) )
        {
            reflect(block,mirror.location,mirror.direction) ;
            block.speed = block.speed*(float)0.1 ;
        }
    }
    // Collisions between buckets and blocks
    if(!CheckRectangleCollision(Bucket[0],Bucket[1]))
    {
        for(auto &bucket:Bucket) for(auto &it2:Blocks)
        {
            auto &block = it2.second ;
            if(CheckRectangleCollision(block,bucket))
            {
                if(block.color == 2) --Lives ;
                else if(bucket.color == block.color) Score += EarnedScore ;
                KillThem.pb(block.ID) ;
            }
        }
        for(auto id:KillThem) Blocks.erase(id) ;
        KillThem.clear() ;
    }
}
/*******************
    BLOCKS
*******************/
int RandomNo(int limit) { return rand()%limit ;}
COLOR Colors[] = {red,green,black} ;
float FindBlockX(float width)
{
    float X = LeftExtreme + 2*(Cannon[0].radius + Cannon[1].radius) + width/2 ;
    float Y = RightExtreme - width/2 ;
    return X + RandomNo((int)ceil(Y-X+1)) ;
}
void CreateBlocks(void)
{
    GameObject temp ;
    temp.color = RandomNo(3) ;
    COLOR BaseBlockColor = Colors[temp.color] ;
    temp.ID = BlockNumber++ ;
    temp.height = 15 ,temp.width = 15 ;
    temp.location = glm::vec3(FindBlockX(temp.width),GamePlayDownExtreme + temp.height/2,0) ;
    temp.CenterOfRotation = temp.location ;
    temp.isRotating = true ;
    temp.direction = glm::vec3(0,1,0) ;
    temp.speed = glm::vec3(0,SpeedY/10,0) * (float)pow(BlockSpeedChange,(float)SpeedoMeter) ;
    temp.gravity = glm::vec3(0,SpeedY/500.0,0) ;
    temp.object =  createRectangle(BaseBlockColor,BaseBlockColor,BaseBlockColor,BaseBlockColor,temp.width,temp.height);
    Blocks[temp.ID] = temp ;
}
void ChangeBlockSpeed(int change)
{
    SpeedoMeter += change ;
    for(auto &it2:Blocks)
    {
        auto &it = it2.second ;
        it.speed = it.speed * (float)pow(BlockSpeedChange,(float)change) ;
    }
}

void MoveBlocks(void)
{
    for(auto &it2:Blocks)
    {
        auto &it = it2.second ;
        it.location = it.CenterOfRotation = it.location + it.speed ;
        it.speed = it.speed + it.gravity ;
        if(it.location[1]<=DownExtreme || it.location[1]>=UpExtreme || it.location[0]<=LeftExtreme ||it.location[0]>=RightExtreme)
            KillThem.pb(it.ID) ;
    }
    for(auto id:KillThem) Blocks.erase(id);
    KillThem.clear() ;
}
/********************
    BUCKETS
********************/
void CreateBuckets(void)
{
    GameObject temp ;
    COLOR BaseBucketColor = red ;
    temp.color = 0 ;
    temp.height = 90 , temp.width = 120 ; temp.direction = glm::vec3(0,1,0) ;
    temp.CenterOfRotation = temp.location = glm::vec3(LeftExtreme + temp.width/2,UpExtreme - temp.height/2 ,0) ;
    temp.object =  createRectangle(BaseBucketColor,BaseBucketColor,BaseBucketColor,BaseBucketColor,temp.width,temp.height);
    Bucket.pb(temp) ;

    temp.color = 1 ;
    BaseBucketColor = green ;
    temp.height = 90 , temp.width = 120 ;  temp.direction = glm::vec3(0,1,0) ;
    temp.CenterOfRotation=temp.location = glm::vec3(RightExtreme - temp.width/2,UpExtreme - temp.height/2 ,0) ;
    temp.object =  createRectangle(BaseBucketColor,BaseBucketColor,BaseBucketColor,BaseBucketColor,temp.width,temp.height);
    Bucket.pb(temp) ;
}
void MoveBucket(int BucketNumber,int direction,bool FollowMouse,glm::vec3 Mouse)
{
    GameObject &bucket = Bucket[BucketNumber] ;
    if(FollowMouse) bucket.location[0] = Mouse[0] ;
    else bucket.location[0] = bucket.location[0] + direction * SpeedX ;
    if(bucket.location[0] < LeftExtreme + bucket.width/2) bucket.location[0] = LeftExtreme + bucket.width/2 ;
    if(bucket.location[0] > RightExtreme - bucket.width/2) bucket.location[0] = RightExtreme - bucket.width/2 ;
    bucket.CenterOfRotation = bucket.location ;
}
/*************************
    CLICK TO SELECT
**************************/
void CheckForSelection(GLFWwindow* window)
{
    glm::vec3 Mouse = GetMouseCoordinates(window) ;
    // Check click on cannon
    for(auto &it:Cannon) if(glm::length(Mouse - it.location) <= it.radius) CannonSelected = true ,cout<<"Cannon selected"<<endl;
    // Check for click on Buckets
    FN(i,sz(Bucket))
    {
        auto &bucket = Bucket[i] ;
        glm::vec3 prependicular = normalize(cross(glm::vec3(0,0,1),bucket.direction)) ;
        if(abs(dot(Mouse - bucket.location,prependicular))<= bucket.height/2.0 && abs(dot(Mouse - bucket.location,bucket.direction)) <= bucket.width/2.0 )
        {
            if(i == 0) BucketRedSelected = true ,cout<<"Red bucket selected"<<endl;
            else BucketGreenSelected = true,cout<<"Green Bucket selected"<<endl ;
        }
    }
}
/************************
    LIVES
************************/
void CreateLifeBlocks(void)
{
    GameObject temp ;
    COLOR BaseLifeColor = white ;

    temp.height = 25 ,temp.width = 25 ;
    temp.location = glm::vec3(-375,-275,0) ;
    temp.CenterOfRotation = temp.location ;
    temp.direction = normalize(glm::vec3(0,1,0)) ;
    temp.object =  createRectangle(BaseLifeColor,BaseLifeColor,BaseLifeColor,BaseLifeColor,temp.width,temp.height);
    LifeBlocks.pb(temp) ;

    temp.height = 25 ,temp.width = 25 ;
    temp.location = glm::vec3(-340,-275,0) ;
    temp.CenterOfRotation = temp.location ;
    temp.direction = normalize(glm::vec3(0,1,0)) ;
    temp.object =  createRectangle(BaseLifeColor,BaseLifeColor,BaseLifeColor,BaseLifeColor,temp.width,temp.height);
    LifeBlocks.pb(temp) ;

    temp.height = 25 ,temp.width = 25 ;
    temp.location = glm::vec3(-305,-275,0) ;
    temp.CenterOfRotation = temp.location ;
    temp.direction = normalize(glm::vec3(0,1,0)) ;
    temp.object =  createRectangle(BaseLifeColor,BaseLifeColor,BaseLifeColor,BaseLifeColor,temp.width,temp.height);
    LifeBlocks.pb(temp) ;
}
void initGL (GLFWwindow* window, int width, int height)
{
    /* Objects should be created before any other gl function and shaders */
	// Create the models



    CreateCannon() ;
    CreateBackGround() ;
    CreateMirror() ;
    CreateBuckets() ;
    CreateLifeBlocks() ;
    // Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

    // Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	int width = 800; XWidth = width ;
	int height = 600; YWidth = height ;

    // seeding the random genrator

    GLFWwindow* window = initGLFW(width, height);
    glfwSetCursorEnterCallback(window, cursor_enter_callback);
	initGL (window, width, height);

     srand(glfwGetTime()) ;
    /* Draw in loop */
    while (!glfwWindowShouldClose(window))
    {

        // OpenGL Draw commands
        draw(window);

        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
    }

    glfwTerminate();
//    exit(EXIT_SUCCESS);
}
