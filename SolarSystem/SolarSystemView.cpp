
// SolarSystemView.cpp: CSolarSystemView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "SolarSystem.h"
#endif

#include "SolarSystemDoc.h"
#include "SolarSystemView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ToOrtho(GLsizei width, GLsizei height);
void ToPerspective(GLsizei width, GLsizei height);
bool InitGLSL();
bool InitSharedMem();
void ClearSharedMem();
void InitLights();
GLuint LoadTexture(const char* fileName, bool wrap = true);

#pragma region Shader_Source
const char* vertexShaderSource = R"(
#version 110
uniform mat4 matrixModelView;
uniform mat4 matrixNormal;
uniform mat4 matrixModelViewProjection;
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;
varying vec3 esVertex, esNormal;
varying vec2 texCoord0;
void main()
{
    esVertex = vec3(matrixModelView * vec4(vertexPosition, 1.0));
    esNormal = vec3(matrixNormal * vec4(vertexNormal, 1.0));
    texCoord0 = vertexTexCoord;
    gl_Position = matrixModelViewProjection * vec4(vertexPosition, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 110
uniform vec4 lightPosition;             
uniform vec4 lightAmbient;              
uniform vec4 lightDiffuse;              
uniform vec4 lightSpecular;             
uniform vec4 materialAmbient;           
uniform vec4 materialDiffuse;           
uniform vec4 materialSpecular;          
uniform float materialShininess;        
uniform sampler2D map0;                 
uniform bool textureUsed;               
varying vec3 esVertex, esNormal;
varying vec2 texCoord0;
void main()
{
    vec3 normal = normalize(esNormal);
    vec3 light;
    if(lightPosition.w == 0.0)
    {
        light = normalize(lightPosition.xyz);
    }
    else
    {
        light = normalize(lightPosition.xyz - esVertex);
    }
    vec3 view = normalize(-esVertex);
    vec3 halfv = normalize(light + view);

    vec3 color = lightAmbient.rgb * materialAmbient.rgb;       
    float dotNL = max(dot(normal, light), 0.0);
    color += lightDiffuse.rgb * materialDiffuse.rgb * dotNL;    
    if(textureUsed)
        color *= texture2D(map0, texCoord0).rgb;               
    float dotNH = max(dot(normal, halfv), 0.0);
    color += pow(dotNH, materialShininess) * lightSpecular.rgb * materialSpecular.rgb; 
    gl_FragColor = vec4(color, materialDiffuse.a);
}
)";

#pragma endregion

#pragma region Property_for_Shader
GLuint shaderProgram = 0;
bool glslSupported;
GLint uniformMatrixModelView;
GLint uniformMatrixModelViewProjection;
GLint uniformMatrixNormal;
GLint uniformLightPosition;
GLint uniformLightAmbient;
GLint uniformLightDiffuse;
GLint uniformLightSpecular;
GLint uniformMaterialAmbient;
GLint uniformMaterialDiffuse;
GLint uniformMaterialSpecular;
GLint uniformMaterialShininess;
GLint uniformMap0;
GLint uniformTextureUsed;
GLint attribVertexPosition;
GLint attribVertexNormal;
GLint attribVertexTexCoord;

#pragma endregion

#pragma region Global_Variables
int screenWidth;
int screenHeight;
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
int drawMode;
bool vboSupported;
GLuint vboId1 = 0, vboId2 = 0;      
GLuint iboId1 = 0, iboId2 = 0;      
GLuint EarthTex;
int imageWidth;
int imageHeight;
Matrix4 matrixModelView;
Matrix4 matrixProjection;
#pragma endregion

#pragma region Sphere_Properties
Sphere sun(5.0f, 36, 18);
Sphere earth(3.0f, 36, 18);

float sunRot = 0;
float earthRot = 0;
float merRot = 0;
#pragma endregion

// CSolarSystemView

IMPLEMENT_DYNCREATE(CSolarSystemView, CView)

BEGIN_MESSAGE_MAP(CSolarSystemView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSolarSystemView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CSolarSystemView 생성/소멸

CSolarSystemView::CSolarSystemView() noexcept
	:ssTimer(true)
{
	// TODO: 여기에 생성 코드를 추가합니다.
}

CSolarSystemView::~CSolarSystemView()
{

}

BOOL CSolarSystemView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}


// CSolarSystemView 그리기
void CSolarSystemView::OnDraw(CDC* /*pDC*/)
{
	CSolarSystemDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

	DrawGLScene();
}


// CSolarSystemView 인쇄

void CSolarSystemView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSolarSystemView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CSolarSystemView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CSolarSystemView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CSolarSystemView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSolarSystemView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CSolarSystemView 진단

#ifdef _DEBUG
void CSolarSystemView::AssertValid() const
{
	CView::AssertValid();
}

void CSolarSystemView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSolarSystemDoc* CSolarSystemView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSolarSystemDoc)));
	return (CSolarSystemDoc*)m_pDocument;
}
#endif //_DEBUG

// CSolarSystemView 메시지 처리기
// Creating OpenGL DC
BOOL CSolarSystemView::SetPixelformat(HDC hdc) {
	int pixelformat;

	PIXELFORMATDESCRIPTOR pfd = { //Describe the pixel format of a drawing surface
	   sizeof(PIXELFORMATDESCRIPTOR), // To set the size of pfd
	   1, // Specifies the version of data structure, value must to be 1. <= 왜인지는 모르겠음
	   PFD_DRAW_TO_WINDOW | // 윈도우 창에 버퍼가 그릴 수 있다.
	   PFD_SUPPORT_OPENGL | // OpenGL 을 지원하는 버퍼
	   PFD_GENERIC_FORMAT | // 픽셀 포멧을 GDI 소프트웨어 구현에 의해 지원됨..????
	   PFD_DOUBLEBUFFER, // 이중 버퍼
	   PFD_TYPE_RGBA, // Color Index 대신 RGBA 4가지 구성요소를 사용해 픽셀 표현
	   32, // 알파값을 제외한 RGB 컬러 버퍼의 크기를 지정
	   0,0,0,0,0,0, // 컬러 버퍼의 비트 평면 값 및 비트 시프트 값
	   8, // 알파 비트 평면값
	   0, // 알파 비트 시프트값
	   8, // 누적 버퍼(Accumulation Buffer) 값
	   0,0,0,0, // 누적 버퍼 RGBA 값
	   16, // Depth Buffer 값
	   0,  // Stencil Buffer 값
	   0, // Auxiliary Buffer 값 (보조 버퍼)
	   PFD_MAIN_PLANE, // 최근에는 쓰이지 않지만 , iLayerType
	   0, // 0000 0000 8비트로 앞 4비트는 오버레이 평면의 갯수, 뒤 4비트는 언더레이 평면의 갯수를 정한다.
	   0,0,0 }; // dwLayerMask, dwVisibleMask, dwDamageMask 3개 인자로 dwVisibleMask 만 사용한다. 
			  // underlay plane 의 transparent color RGB value 를 설정한다. 

	if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE) { // 위에서 작성한 pfd 조건과 일치하는 Device Context를 Search
		MessageBox(L"ChoosePixelFormat failed", L"Error Code", MB_OK);
		return FALSE;
	}


	if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) { // Search 한 픽셀 포멧이 적용되지 않을 경우 에러
		MessageBox(L"SetPixelFormat failed", L"Error", MB_OK);
	}
	return TRUE;
}

int CSolarSystemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_hDC = GetDC()->m_hDC;
	if (!SetPixelformat(m_hDC))
		return -1;

	m_hglRC = wglCreateContext(m_hDC); // DC 를 바탕으로 OpenGL을 사용할 Rendering Context 생성
	wglMakeCurrent(m_hDC, m_hglRC); // 생성된 Rendering Context를 Current Context로 설정

	if (ssTimer) { // 타이머 시작
		SetTimer(1, 100, NULL);
	}

	InitGL();
	InitGLSL();

	return 0;
}

void CSolarSystemView::InitGL(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Color Buffer 초기값 초기화
	//glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST); // Fragment Shader 수행 전 Depth Buffer 에 값을 저장할 Fragment 를 미리 선별하는 Depth Testing 실행
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Color 와 Texture 의 보간 작업에 대해 GL_NICEST, 품질 우선으로 작업하도록 권고
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	glClearStencil(0);
	glClearDepth(1.f); // Depth Buffer 초기값 초기화
	glDepthFunc(GL_LEQUAL); // fragment 의 Depth 값이 Current Depth Buffer와 크기가 같거나 작을 경우 Depth Buffer 에 누적

	InitLights();

	GLenum err = glewInit();
}

#pragma region Shader_Part
const int   SCREEN_WIDTH = 1500;
const int   SCREEN_HEIGHT = 500;
const float CAMERA_DISTANCE = 4.0f;

bool InitGLSL() {
	const int MAX_LENGTH = 2048;
	char log[MAX_LENGTH];
	int logLength = 0;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgram = glCreateProgram();

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	int isVsSuccess, isFsSuccess;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isVsSuccess);
	if (isVsSuccess == GL_FALSE) {
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
		glGetShaderInfoLog(vertexShader, MAX_LENGTH, &logLength, log);
		std::cout << "++++++ Vertex Shader Log +++++\n" << log << std::endl;
	}

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isFsSuccess);
	if (isFsSuccess == GL_FALSE) {
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
		glGetShaderInfoLog(fragmentShader, MAX_LENGTH, &logLength, log);
		std::cout << "++++++ Fragment Shader Log +++++\n" << log << std::endl;
	}

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	uniformMatrixModelView = glGetUniformLocation(shaderProgram, "matrixModelView");
	uniformMatrixModelViewProjection = glGetUniformLocation(shaderProgram, "matrixModelViewProjection");
	uniformMatrixNormal = glGetUniformLocation(shaderProgram, "matrixNormal");
	uniformLightPosition = glGetUniformLocation(shaderProgram, "lightPosition");
	uniformLightAmbient = glGetUniformLocation(shaderProgram, "lightAmbient");
	uniformLightDiffuse = glGetUniformLocation(shaderProgram, "lightDiffuse");
	uniformLightSpecular = glGetUniformLocation(shaderProgram, "lightSpecular");
	uniformMaterialAmbient = glGetUniformLocation(shaderProgram, "materialAmbient");
	uniformMaterialDiffuse = glGetUniformLocation(shaderProgram, "materialDiffuse");
	uniformMaterialSpecular = glGetUniformLocation(shaderProgram, "materialSpecular");
	uniformMaterialShininess = glGetUniformLocation(shaderProgram, "materialShininess");
	uniformMap0 = glGetUniformLocation(shaderProgram, "map0");
	uniformTextureUsed = glGetUniformLocation(shaderProgram, "textureUsed");
	attribVertexPosition = glGetAttribLocation(shaderProgram, "vertexPosition");
	attribVertexNormal = glGetAttribLocation(shaderProgram, "vertexNormal");
	attribVertexTexCoord = glGetAttribLocation(shaderProgram, "vertexTexCoord");

	float lightPosition[] = { 0, 0, 1, 0 };
	float lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1 };
	float lightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1 };
	float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1 };
	float materialAmbient[] = { 0.5f, 0.5f, 0.5f, 1 };
	float materialDiffuse[] = { 0.7f, 0.7f, 0.7f, 1 };
	float materialSpecular[] = { 0.4f, 0.4f, 0.4f, 1 };
	float materialShininess = 16;
	glUniform4fv(uniformLightPosition, 1, lightPosition);
	glUniform4fv(uniformLightAmbient, 1, lightAmbient);
	glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
	glUniform4fv(uniformLightSpecular, 1, lightSpecular);
	glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
	glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
	glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
	glUniform1f(uniformMaterialShininess, materialShininess);
	glUniform1i(uniformMap0, 0);
	glUniform1i(uniformTextureUsed, 1);

	glUseProgram(0);

	int linkStatus;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		glGetProgramInfoLog(shaderProgram, MAX_LENGTH, &logLength, log);
		std::cout << "++++++ GLSL Program Log ++++++\n" << log << std::endl;
		return false;
	}
	else {
		return true;
	}
}

void InitLights() {
	GLfloat lightKa[] = { .3f, .3f, .3f, 1.0f };  
	GLfloat lightKd[] = { .7f, .7f, .7f, 1.0f };  
	GLfloat lightKs[] = { 1, 1, 1, 1 };           
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	float lightPos[4] = { 0, 0, 1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);
}

bool InitSharedMem()
{
	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;

	mouseLeftDown = mouseRightDown = mouseMiddleDown = false;
	mouseX = mouseY = 0;

	cameraAngleX = cameraAngleY = 0.0f;
	cameraDistance = CAMERA_DISTANCE;

	drawMode = 0; // 0:fill, 1: wireframe, 2:points

	return true;

}

void ClearSharedMem()
{
		glDeleteBuffers(1, &vboId1);
		glDeleteBuffers(1, &iboId1);
		glDeleteBuffers(1, &vboId2);
		glDeleteBuffers(1, &iboId2);
		vboId1 = iboId1 = 0;
		vboId2 = iboId2 = 0;
}

void SetCamera()
{
	// 태양계 그리기

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// World Coordinate Sys 조정
	// 카메라 세팅
	GLfloat rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
	GLfloat posX = 0.0f, posY = 0.0f, posZ = 25.0f;

	// 1번 스케일링
	glScalef(1.0f, 1.0f, 1.0f);

	// 2번 회전
	glRotatef(rotX, 1, 0, 0);
	glRotatef(rotY, 0, 1, 0);
	glRotatef(rotZ, 0, 0, 1);

	// 3번 이동
	glTranslatef(-posX, -posY, -posZ);
}

GLuint LoadTexture(const char* fileName, bool wrap)
{
	Image::Bmp bmp;
	if (!bmp.read(fileName))
		return 0;     

	int width = bmp.getWidth();
	int height = bmp.getHeight();
	const unsigned char* data = bmp.getDataRGB();
	GLenum type = GL_UNSIGNED_BYTE;  

	GLenum format;
	int bpp = bmp.getBitCount();
	if (bpp == 8)
		format = GL_LUMINANCE;
	else if (bpp == 24)
		format = GL_RGB;
	else if (bpp == 32)
		format = GL_RGBA;
	else
		return 0;               

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);

	return texture;
}


void CSolarSystemView::OnDestroy()
{
	CView::OnDestroy();
	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hglRC);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (ssTimer) {
		KillTimer(1);
		ssTimer = false;
	}
}


void CSolarSystemView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//ReSizeGLScene(cx, cy);
	ToPerspective(cx, cy);
}

void CSolarSystemView::ReSizeGLScene(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION); // 행렬 연산의 대상이 되는 스택을 Projection Stack 으로 변경
	glLoadIdentity();

	double screenRatio = (double)width / (double)height;

	glFrustum(-screenRatio, screenRatio, -1.0f, 1.0f, 1.0f, 10000.0f); // 원근감을 주기 위해 카메라 시야각 설정
}

void ToOrtho(GLsizei width, GLsizei height)
{
	const float N = -1.0f;
	const float F = 1.0f;

	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// construct ortho projection matrix
	matrixProjection.identity();
	matrixProjection[0] = 2 / width;
	matrixProjection[5] = 2 / height;
	matrixProjection[10] = -2 / (F - N);
	matrixProjection[14] = -(F + N) / (F - N);

	// set orthographic viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(matrixProjection.get());
	//glLoadIdentity();
	//glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ToPerspective(GLsizei width, GLsizei height)
{
	const float N = 0.1f;
	const float F = 100.0f;
	const float DEG2RAD = 3.141592f / 180;
	const float FOV_Y = 40.0f * DEG2RAD;

	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// construct perspective projection matrix
	float aspectRatio = (float)(width) / height;
	float tangent = tanf(FOV_Y / 2.0f);     // tangent of half fovY
	float h = N * tangent;                  // half height of near plane
	float w = h * aspectRatio;              // half width of near plane
	matrixProjection.identity();
	matrixProjection[0] = N / w;
	matrixProjection[5] = N / h;
	matrixProjection[10] = -(F + N) / (F - N);
	matrixProjection[11] = -1;
	matrixProjection[14] = -(2 * F * N) / (F - N);
	matrixProjection[15] = 0;

	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(matrixProjection.get());

	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void CSolarSystemView::DrawGLScene(void)
{
	wglMakeCurrent(m_hDC, m_hglRC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT); // Color Buffer와 Depth Buffer에 존재하는 값을 초기화

	SetCamera();

	Display();

	// 화면 갱신
	SwapBuffers(m_hDC);
}

float lineColor[] = { 0.2f, 0.2f, 0.2f, 1 };

void CSolarSystemView::Display() {
	glGenBuffers(1, &vboId2);
	glBindBuffer(GL_ARRAY_BUFFER, vboId2);
	glBufferData(GL_ARRAY_BUFFER, sun.getInterleavedVertexSize(), sun.getInterleavedVertices(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &iboId2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sun.getIndexSize(), sun.getIndices(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	EarthTex = LoadTexture("bmp/earth.bmp", true);

	glPushMatrix();
	// transform camera (view)
	Matrix4 matrixView;
	matrixView.translate(0, 0, -50);

	// common model matrix
	Matrix4 matrixModelCommon;
	matrixModelCommon.rotateX(-90);
	matrixModelCommon.rotateY(earthRot);

	glPushMatrix();
	// model matrix for each instance
	Matrix4 EarthModel(matrixModelCommon);    // right

	glUseProgram(shaderProgram);
	glBindTexture(GL_TEXTURE_2D, EarthTex);

	glEnableVertexAttribArray(attribVertexPosition);
	glEnableVertexAttribArray(attribVertexNormal);
	glEnableVertexAttribArray(attribVertexTexCoord);

	glBindBuffer(GL_ARRAY_BUFFER, vboId2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId2);

	int stride = sun.getInterleavedStride();
	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));

	matrixModelView = matrixView * EarthModel;
	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
	Matrix4 matrixNormal = matrixModelView;
	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());

	glDrawElements(GL_TRIANGLES,           
		sun.getIndexCount(), 
		GL_UNSIGNED_INT,     
		(void*)0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(attribVertexPosition);
	glDisableVertexAttribArray(attribVertexNormal);
	glDisableVertexAttribArray(attribVertexTexCoord);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	
	glPopMatrix();
}

void CSolarSystemView::RotateSphere(float rot) {
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
}

void CSolarSystemView::OnTimer(UINT_PTR nIDEvent)
{
	if (true) {
		sunRot += 1;
		earthRot += 5;
		earthRot += 8;
	}
	Invalidate(FALSE);
	CView::OnTimer(nIDEvent);
}
