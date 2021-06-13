
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

//#pragma region Shader_Source
//const char* vertexShaderSource = R"(
//#version 110
//uniform mat4 matrixModelView;
//uniform mat4 matrixNormal;
//uniform mat4 matrixModelViewProjection;
//attribute vec3 vertexPosition;
//attribute vec3 vertexNormal;
//attribute vec2 vertexTexCoord;
//varying vec3 esVertex, esNormal;
//varying vec2 texCoord0;
//void main()
//{
//    esVertex = vec3(matrixModelView * vec4(vertexPosition, 1.0));
//    esNormal = vec3(matrixNormal * vec4(vertexNormal, 1.0));
//    texCoord0 = vertexTexCoord;
//    gl_Position = matrixModelViewProjection * vec4(vertexPosition, 1.0);
//}
//)";
//
//const char* fragmentShaderSource = R"(
//#version 110
//uniform vec4 lightPosition;             
//uniform vec4 lightAmbient;              
//uniform vec4 lightDiffuse;              
//uniform vec4 lightSpecular;             
//uniform vec4 materialAmbient;           
//uniform vec4 materialDiffuse;           
//uniform vec4 materialSpecular;          
//uniform float materialShininess;        
//uniform sampler2D map0;                 
//uniform bool textureUsed;               
//varying vec3 esVertex, esNormal;
//varying vec2 texCoord0;
//void main()
//{
//    vec3 normal = normalize(esNormal);
//    vec3 light;
//    if(lightPosition.w == 0.0)
//    {
//        light = normalize(lightPosition.xyz);
//    }
//    else
//    {
//        light = normalize(lightPosition.xyz - esVertex);
//    }
//    vec3 view = normalize(-esVertex);
//    vec3 halfv = normalize(light + view);
//
//    vec3 color = lightAmbient.rgb * materialAmbient.rgb;       
//    float dotNL = max(dot(normal, light), 0.0);
//    color += lightDiffuse.rgb * materialDiffuse.rgb * dotNL;    
//    if(textureUsed)
//        color *= texture2D(map0, texCoord0).rgb;               
//    float dotNH = max(dot(normal, halfv), 0.0);
//    color += pow(dotNH, materialShininess) * lightSpecular.rgb * materialSpecular.rgb; 
//    gl_FragColor = vec4(color, materialDiffuse.a);
//}
//)";
//
//#pragma endregion

#pragma region Property_for_Shader
GLuint shaderProgram = 0;
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
GLuint sunVBO = 0, mercuryVBO = 0, venusVBO = 0, earthVBO = 0, marsVBO = 0, jupiterVBO = 0, saturnVBO = 0, uranusVBO = 0, neptuneVBO = 0;
GLuint sunIBO = 0, mercuryIBO = 0, venusIBO = 0, earthIBO = 0, marsIBO = 0, jupiterIBO = 0, saturnIBO = 0, uranusIBO = 0, neptuneIBO = 0;

GLuint sunTex, mercuryTex, venusTex, earthTex, moonTex, marsTex, jupiterTex, saturnTex, uranusTex, neptuneTex;

Matrix4 matrixModelView;
Matrix4 matrixProjection;

int screenWidth;
int screenHeight;
float cameraX, cameraY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
float timeSpeed;
bool isTimeGoing;
float matAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float matDiff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

const int   SCREEN_WIDTH = 1500;
const int   SCREEN_HEIGHT = 500;
const float CAMERA_DISTANCE = 100.0f;
const float DEFAULT_TIME_SPEED = 1.0f;
#pragma endregion

#pragma region Sphere_Properties
Sphere sun(17.109f, 36, 18);
Sphere mercury(0.6f, 36, 18);
Sphere venus(1.5f, 36, 18);
Sphere earth(1.575f, 36, 18);
Sphere moon(30.073f, 36, 18);
Sphere mars(0.815f, 36, 18);
Sphere jupiter(3.517f, 36, 18);
Sphere saturn(2.965f, 36, 18);
Sphere uranus(1.250f, 36, 18);
Sphere neptune(1.218f, 36, 18);

float sunRot = 0;

float mercuryRot = 0;
float mercuryRevolve = 4.787f;

float venusRot = 0;
float venusRevolve = 3.502f;

float earthRot = 0;
float earthRevolve = 2.978f;

float moonRot = 0;
float moonRevolve = 1.023f;

float marsRot = 0;
float marsRevolve = 2.413f;

float jupiterRot = 0;
float jupiterRevolve = 1.306f;

float saturnRot = 0;
float saturnRevolve = 0.967f;

float uranusRot = 0;
float uranusRevolve = 0.683f;

float neptuneRot = 0;
float neptuneRevolve = 0.547f;
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
	rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
	posX = 0.0f, posY = 0.0f, posZ = 125.0f;
}

CSolarSystemView::~CSolarSystemView()
{

}

BOOL CSolarSystemView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.cx = 1920;
	cs.cy = 1080;
	ToPerspective(cs.cx, cs.cy);
	return CView::PreCreateWindow(cs);
}

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

	InitMembers();
	GetTextures();

	InitGL();
	//InitGLSL();

	return 0;
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

void CSolarSystemView::DrawGLScene(void)
{
	wglMakeCurrent(m_hDC, m_hglRC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Color Buffer와 Depth Buffer에 존재하는 값을 초기화

	Display();

	// 화면 갱신
	SwapBuffers(m_hDC);
}

void CSolarSystemView::GetTextures() {
	sunTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/sun.bmp", true);
	mercuryTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/mercury.bmp", true);
	venusTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/venus.bmp", true);
	earthTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/earth.bmp", true);
	moonTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/moon.bmp", true);
	marsTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/mars.bmp", true);
	jupiterTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/jupiter.bmp", true);
	saturnTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/saturn.bmp", true);
	uranusTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/uranus.bmp", true);
	neptuneTex = LoadTexture("C:/Users/김지헌/Documents/Github/SolarSystem/Solarsystem/bmp/neptune.bmp", true);
}

void CSolarSystemView::SetCamera() {
	// gluLookAt 구현 파트
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
void CSolarSystemView::OnTimer(UINT_PTR nIDEvent)
{
	if (isTimeGoing) {
		sunRot += 0.8;
		mercuryRot += 0.3;
		venusRot += 0.18;
		earthRot += 0.4651;
		moonRot += 0.04626;
		marsRot += 0.241;
		jupiterRot += 0.8126;
		saturnRot += 0.687;
		uranusRot += 0.259;
		neptuneRot += 0.268;

		mercuryRevolve += 4.787f;
		venusRevolve += 3.502f;
		earthRevolve += 2.978f;
		moonRevolve += 1.023f;
		marsRevolve += 2.413f;
		jupiterRevolve += 1.306f;
		saturnRevolve += 0.967f;
		uranusRevolve += 0.683f;
		neptuneRevolve += 0.547f;
	}
	Invalidate(FALSE);
	CView::OnTimer(nIDEvent);
}

void CSolarSystemView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//ReSizeGLScene(cx, cy);
	ToPerspective(cx, cy);
}

#pragma region //Init_Funcs
void CSolarSystemView::InitGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// Color Buffer 초기값 초기화
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);				// Fragment Shader 수행 전 Depth Buffer 에 값을 저장할 Fragment 를 미리 선별하는 Depth Testing 실행
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Color 와 Texture 의 보간 작업에 대해 GL_NICEST, 품질 우선으로 작업하도록 권고
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	glClearStencil(0);
	glClearDepth(1.f);						// Depth Buffer 초기값 초기화
	glDepthFunc(GL_LEQUAL);					// fragment 의 Depth 값이 Current Depth Buffer와 크기가 같거나 작을 경우 Depth Buffer 에 누적

	InitLights();

	glewExperimental = TRUE;
	GLenum err = glewInit();
}

//bool CSolarSystemView::InitGLSL() {
//	const int MAX_LENGTH = 2048;
//	char log[MAX_LENGTH];
//	int logLength = 0;
//
//	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	shaderProgram = glCreateProgram();
//
//	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//
//	glCompileShader(vertexShader);
//	glCompileShader(fragmentShader);
//
//	int isVsSuccess, isFsSuccess;
//	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isVsSuccess);
//	if (isVsSuccess == GL_FALSE) {
//		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
//		glGetShaderInfoLog(vertexShader, MAX_LENGTH, &logLength, log);
//		std::cout << "++++++ Vertex Shader Log +++++\n" << log << std::endl;
//	}
//
//	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isFsSuccess);
//	if (isFsSuccess == GL_FALSE) {
//		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
//		glGetShaderInfoLog(fragmentShader, MAX_LENGTH, &logLength, log);
//		std::cout << "++++++ Fragment Shader Log +++++\n" << log << std::endl;
//	}
//
//	glAttachShader(shaderProgram, vertexShader);
//	glAttachShader(shaderProgram, fragmentShader);
//
//	glLinkProgram(shaderProgram);
//
//	glUseProgram(shaderProgram);
//
//	uniformMatrixModelView = glGetUniformLocation(shaderProgram, "matrixModelView");
//	uniformMatrixModelViewProjection = glGetUniformLocation(shaderProgram, "matrixModelViewProjection");
//	uniformMatrixNormal = glGetUniformLocation(shaderProgram, "matrixNormal");
//	uniformLightPosition = glGetUniformLocation(shaderProgram, "lightPosition");
//	uniformLightAmbient = glGetUniformLocation(shaderProgram, "lightAmbient");
//	uniformLightDiffuse = glGetUniformLocation(shaderProgram, "lightDiffuse");
//	uniformLightSpecular = glGetUniformLocation(shaderProgram, "lightSpecular");
//	uniformMaterialAmbient = glGetUniformLocation(shaderProgram, "materialAmbient");
//	uniformMaterialDiffuse = glGetUniformLocation(shaderProgram, "materialDiffuse");
//	uniformMaterialSpecular = glGetUniformLocation(shaderProgram, "materialSpecular");
//	uniformMaterialShininess = glGetUniformLocation(shaderProgram, "materialShininess");
//	uniformMap0 = glGetUniformLocation(shaderProgram, "map0");
//	uniformTextureUsed = glGetUniformLocation(shaderProgram, "textureUsed");
//	attribVertexPosition = glGetAttribLocation(shaderProgram, "vertexPosition");
//	attribVertexNormal = glGetAttribLocation(shaderProgram, "vertexNormal");
//	attribVertexTexCoord = glGetAttribLocation(shaderProgram, "vertexTexCoord");
//
//	float lightPosition[] = { 0, 0, 1, 0 };
//	float lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1 };
//	float lightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1 };
//	float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1 };
//	float materialAmbient[] = { 0.5f, 0.5f, 0.5f, 1 };
//	float materialDiffuse[] = { 0.7f, 0.7f, 0.7f, 1 };
//	float materialSpecular[] = { 0.4f, 0.4f, 0.4f, 1 };
//	float materialShininess = 16;
//	glUniform4fv(uniformLightPosition, 1, lightPosition);
//	glUniform4fv(uniformLightAmbient, 1, lightAmbient);
//	glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
//	glUniform4fv(uniformLightSpecular, 1, lightSpecular);
//	glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
//	glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
//	glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
//	glUniform1f(uniformMaterialShininess, materialShininess);
//	glUniform1i(uniformMap0, 0);
//	glUniform1i(uniformTextureUsed, 1);
//
//	glUseProgram(0);
//
//	int linkStatus;
//	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
//	if (linkStatus == GL_FALSE) {
//		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
//		glGetProgramInfoLog(shaderProgram, MAX_LENGTH, &logLength, log);
//		std::cout << "++++++ GLSL Program Log ++++++\n" << log << std::endl;
//		return false;
//	}
//	else {
//		return true;
//	}
//}

void CSolarSystemView::InitLights() {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);

	GLfloat ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	float lightPos[4] = { 0, 0, 1, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void CSolarSystemView::InitMembers()
{
	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;

	cameraAngleX = cameraAngleY = 0.0f;
	cameraDistance = CAMERA_DISTANCE;
	isTimeGoing = false;
	timeSpeed = DEFAULT_TIME_SPEED;
}
#pragma endregion

void CSolarSystemView::ToOrtho(GLsizei width, GLsizei height)
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

void CSolarSystemView::ToPerspective(GLsizei width, GLsizei height)
{
	const float N = 0.1f;
	const float F = 1000.0f;
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

void CSolarSystemView::ReSizeGLScene(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);										// 행렬 연산의 대상이 되는 스택을 Projection Stack 으로 변경
	glLoadIdentity();

	screenWidth = width;
	screenHeight = height;

	double screenRatio = (double)screenWidth / (double)screenHeight;

	glFrustum(-screenRatio, screenRatio, -1.0f, 1.0f, 1.0f, 10000.0f);	// 원근감을 주기 위해 카메라 시야각 설정
}

GLuint CSolarSystemView::LoadTexture(const char* fileName, bool wrap)
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

void CSolarSystemView::CreateShadow() {
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

BOOL CSolarSystemView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam) {
		case VK_F1:
			isTimeGoing = true;
			break;

		case VK_F2:
			isTimeGoing = false;
			break;

		case VK_LEFT:
			cameraX += 0.5f;
			return TRUE;
			break;

		case VK_RIGHT:
			cameraX += -0.5f;
			return TRUE;
			break;

		case VK_UP:
			cameraY += -0.5f;
			return TRUE;
			break;

		case VK_DOWN:
			cameraY += 0.5f;
			return TRUE;
			break;

		case VK_F3:
			cameraAngleY += 0.5f;
			return TRUE;
			break;

		case VK_F4:
			cameraAngleY += -0.5f;
			return TRUE;
			break;

		case VK_F5:
			cameraAngleX += 0.5f;
			return TRUE;
			break;

		case VK_F6:
			cameraAngleX += -0.5f;
			return TRUE;
			break;

		case VK_F7:
			cameraDistance += 0.5f;
			break;

		case VK_F8:
			cameraDistance += -0.5f;
			break;

		case VK_F12:
			mercuryRevolve	+= 1;
			venusRevolve	+= 1;
			earthRevolve	+= 1;
			moonRevolve		+= 1;
			marsRevolve		+= 1;
			jupiterRevolve	+= 1;
			saturnRevolve	+= 1;
			uranusRevolve	+= 1;
			neptuneRevolve	+= 1;
			break;

		default:
			return FALSE;
		}
	}
	return CView::PreTranslateMessage(pMsg);
}

//void CSolarSystemView::DrawSun() {
//	glGenBuffers(1, &sunVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
//	glBufferData(GL_ARRAY_BUFFER, sun.getInterleavedVertexSize(), sun.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &sunIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sun.getIndexSize(), sun.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateY(sunRot);
//	ModelMatrix.rotateZ(7.25);
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 SunModel(ModelMatrix);    // right
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, sunTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunIBO);
//
//	int stride = sun.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * SunModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		sun.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}
//
//void CSolarSystemView::DrawMercury() {
//	glGenBuffers(1, &mercuryVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, mercuryVBO);
//	glBufferData(GL_ARRAY_BUFFER, mercury.getInterleavedVertexSize(), mercury.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &mercuryIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mercuryIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mercury.getIndexSize(), mercury.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateZ(0.01); // 자전 기울기
//
//	ModelMatrix.rotateY(mercuryRevolve);
//	ModelMatrix.translate(17.109 + 7.8, 0, 0);
//	ModelMatrix.rotateY(-mercuryRevolve);
//
//	ModelMatrix.rotateY(mercuryRot);
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 MercuryModel(ModelMatrix);
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, mercuryTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, mercuryVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mercuryIBO);
//
//	int stride = mercury.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * MercuryModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		mercury.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}
//
//void CSolarSystemView::DrawVenus() {
//
//	glGenBuffers(1, &venusVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, venusVBO);
//	glBufferData(GL_ARRAY_BUFFER, venus.getInterleavedVertexSize(), venus.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &venusIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, venusIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, venus.getIndexSize(), venus.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateZ(-2.64);
//
//	ModelMatrix.rotateY(venusRevolve);
//	ModelMatrix.translate(17.109 + 14, 0, 0);
//	ModelMatrix.rotateY(-venusRevolve);
//
//	ModelMatrix.rotateY(venusRot);
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 VenusModel(ModelMatrix);    // right
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, venusTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, venusVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, venusIBO);
//
//	int stride = venus.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * VenusModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		venus.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}
//
//void CSolarSystemView::DrawEarth() {
//
//	glGenBuffers(1, &earthVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, earthVBO);
//	glBufferData(GL_ARRAY_BUFFER, earth.getInterleavedVertexSize(), earth.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &earthIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, earthIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, earth.getIndexSize(), earth.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateZ(-23.44);
//
//	ModelMatrix.rotateY(earthRevolve);
//	ModelMatrix.translate(17.109 + 20, 0, 0); // 궤도 장반경
//	ModelMatrix.rotateY(-earthRevolve);
//
//	ModelMatrix.rotateY(earthRot);   // 자전 속도
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 EarthModel(ModelMatrix);
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, earthTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, earthVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, earthIBO);
//
//	int stride = earth.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * EarthModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		earth.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}
//
//void CSolarSystemView::DrawMars() {
//
//	glGenBuffers(1, &marsVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, marsVBO);
//	glBufferData(GL_ARRAY_BUFFER, mars.getInterleavedVertexSize(), mars.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &marsIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, marsIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mars.getIndexSize(), mars.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateZ(-25.19);
//
//	ModelMatrix.rotateY(marsRevolve);
//	ModelMatrix.translate(17.109 + 30.4, 0, 0);
//	ModelMatrix.rotateY(-marsRevolve);
//
//	ModelMatrix.rotateY(marsRot);
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 MarsModel(ModelMatrix);    // right
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, marsTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, marsVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, marsIBO);
//
//	int stride = mars.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * MarsModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		mars.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}
//
//void CSolarSystemView::DrawJupiter() {
//
//	glGenBuffers(1, &jupiterVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, jupiterVBO);
//	glBufferData(GL_ARRAY_BUFFER, jupiter.getInterleavedVertexSize(), jupiter.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &jupiterIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jupiterIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, jupiter.getIndexSize(), jupiter.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateZ(-3.12);
//
//	ModelMatrix.rotateY(jupiterRevolve);
//	ModelMatrix.translate(17.109 + 50, 0, 0);
//	ModelMatrix.rotateY(-jupiterRevolve);
//
//	ModelMatrix.rotateY(jupiterRot);
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 EarthModel(ModelMatrix);    // right
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, jupiterTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, jupiterVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jupiterIBO);
//
//	int stride = jupiter.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * EarthModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		jupiter.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}
//
//void CSolarSystemView::DrawSaturn() {
//
//	glGenBuffers(1, &saturnVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, saturnVBO);
//	glBufferData(GL_ARRAY_BUFFER, saturn.getInterleavedVertexSize(), saturn.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &saturnIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, saturnIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, saturn.getIndexSize(), saturn.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateZ(-26.73);
//
//	ModelMatrix.rotateY(saturnRevolve);
//	ModelMatrix.translate(17.109 + 65, 0, 0);
//	ModelMatrix.rotateY(-saturnRevolve);
//
//	ModelMatrix.rotateY(saturnRot);
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 SaturnModel(ModelMatrix);    // right
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, saturnTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, saturnVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, saturnIBO);
//
//	int stride = saturn.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * SaturnModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		saturn.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}
//
//void CSolarSystemView::DrawUranus() {
//
//	glGenBuffers(1, &uranusVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, uranusVBO);
//	glBufferData(GL_ARRAY_BUFFER, uranus.getInterleavedVertexSize(), uranus.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &uranusIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uranusIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, uranus.getIndexSize(), uranus.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateZ(-23.44);
//
//	ModelMatrix.rotateY(uranusRevolve);
//	ModelMatrix.translate(17.109 + 75, 0, 0);
//	ModelMatrix.rotateY(-uranusRevolve);
//
//	ModelMatrix.rotateY(uranusRot);
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 UranusModel(ModelMatrix);    // right
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, uranusTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, uranusVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uranusIBO);
//
//	int stride = uranus.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * UranusModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		uranus.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}
//
//void CSolarSystemView::DrawNeptune() {
//
//	glGenBuffers(1, &neptuneVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, neptuneVBO);
//	glBufferData(GL_ARRAY_BUFFER, neptune.getInterleavedVertexSize(), neptune.getInterleavedVertices(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glGenBuffers(1, &neptuneIBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, neptuneIBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, neptune.getIndexSize(), neptune.getIndices(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	// transform camera (view)
//	Matrix4 matrixView;
//	matrixView.translate(-posX, -posY, -posZ);
//	matrixView.rotateY(rotY);
//
//	// common model matrix
//	Matrix4 ModelMatrix;
//	ModelMatrix.rotateX(-90);
//	ModelMatrix.rotateZ(-28.33);
//
//	ModelMatrix.rotateY(neptuneRevolve);
//	ModelMatrix.translate(17.109 + 85, 0, 0);
//	ModelMatrix.rotateY(-neptuneRevolve);
//
//	ModelMatrix.rotateY(neptuneRot);
//
//	glPushMatrix();
//	// model matrix for each instance
//	Matrix4 NeptuneModel(ModelMatrix);    // right
//
//	glUseProgram(shaderProgram);
//	glBindTexture(GL_TEXTURE_2D, neptuneTex);
//
//	glEnableVertexAttribArray(attribVertexPosition);
//	glEnableVertexAttribArray(attribVertexNormal);
//	glEnableVertexAttribArray(attribVertexTexCoord);
//
//	glBindBuffer(GL_ARRAY_BUFFER, neptuneVBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, neptuneIBO);
//
//	int stride = neptune.getInterleavedStride();
//	glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
//	glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
//
//	matrixModelView = matrixView * NeptuneModel;
//	Matrix4 matrixModelViewProjection = matrixProjection * matrixModelView;
//	Matrix4 matrixNormal = matrixModelView;
//	matrixNormal.setColumn(3, Vector4(0, 0, 0, 1));
//	glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrixModelView.get());
//	glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
//	glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());
//
//	glDrawElements(GL_TRIANGLES,
//		neptune.getIndexCount(),
//		GL_UNSIGNED_INT,
//		(void*)0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	glDisableVertexAttribArray(attribVertexPosition);
//	glDisableVertexAttribArray(attribVertexNormal);
//	glDisableVertexAttribArray(attribVertexTexCoord);
//
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glUseProgram(0);
//
//	glPopMatrix();
//}

void CSolarSystemView::Display() {
		glPushMatrix();
		glTranslatef(cameraX, cameraY, -cameraDistance);
		glRotatef(cameraAngleX, 1, 0, 0);
		glRotatef(cameraAngleY, 0, 1, 0);

		// 태양
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glTranslatef(-1.0f, 0.0f, 0.0f);

			glRotatef(sunRot, 0, 0, 1);
			

			glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
			glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
			glBindTexture(GL_TEXTURE_2D, sunTex);
			sun.draw();
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();


		// 수성
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glRotatef(mercuryRevolve, 0, 0, 1);
			glTranslatef(21.7f, 0.0f, 0.0f);

			glPushMatrix();
				glRotatef(mercuryRot, 0, 0, 1);
				glBindTexture(GL_TEXTURE_2D, mercuryTex);
				mercury.draw();
				glBindTexture(GL_TEXTURE_2D, 0);
			glPopMatrix();

		glPopMatrix();


		// 금성
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glRotatef(venusRevolve, 0, 0, 1);
			glTranslatef(31, 0.0f, 0.0f);


			glPushMatrix();
				glRotatef(venusRot, 0, 0, 1);
				glBindTexture(GL_TEXTURE_2D, venusTex);
				venus.draw();
				glBindTexture(GL_TEXTURE_2D, 0);
			glPopMatrix();
		glPopMatrix();


		// 지구
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glRotatef(earthRevolve, 0, 0, 1);
			glTranslatef(40, 0.0f, 0.0f);

			glPushMatrix();
				glRotatef(earthRot, 0, 0, 1);
				glBindTexture(GL_TEXTURE_2D, earthTex);
				earth.draw();
				glBindTexture(GL_TEXTURE_2D, 0);

				// 달
	/*			glPushMatrix();
					glRotatef(-90, 0, 1, 0);
					glRotatef(moonRevolve, 0, 1, 0);
					glTranslatef(0.0f, 3.8f, 0.0f);

					glPushMatrix();
						glRotatef(moonRot, 0, 1, 0);
						glBindTexture(GL_TEXTURE_2D, moonTex);
						moon.draw(lineColor);
						glBindTexture(GL_TEXTURE_2D, 0);
					glPopMatrix();
				glPopMatrix();*/
			glPopMatrix();
		glPopMatrix();


		// 화성
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glRotatef(marsRevolve, 0, 0, 1);
			glTranslatef(45.6f, 0.0f, 0.0f);

			glPushMatrix();
				glRotatef(marsRot, 0, 0, 1);
				glBindTexture(GL_TEXTURE_2D, marsTex);
				mars.draw();
				glBindTexture(GL_TEXTURE_2D, 0);
			glPopMatrix();
		glPopMatrix();


		// 목성
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glRotatef(jupiterRevolve, 0, 0, 1);
			glTranslatef(85.0f, 0.0f, 0.0f);

			glPushMatrix();
				glRotatef(jupiterRot, 0, 0, 1);
				glBindTexture(GL_TEXTURE_2D, jupiterTex);
				jupiter.draw();
				glBindTexture(GL_TEXTURE_2D, 0);
			glPopMatrix();
		glPopMatrix();


		// 토성
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glRotatef(saturnRevolve, 0, 0, 1);
			glTranslatef(150.0f, 0.0f, 0.0f);

			glPushMatrix();
				glRotatef(saturnRot, 0, 0, 1);
				glBindTexture(GL_TEXTURE_2D, saturnTex);
				saturn.draw();
				glBindTexture(GL_TEXTURE_2D, 0);
			glPopMatrix();
		glPopMatrix();


		// 천왕성
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glRotatef(uranusRevolve, 0, 0, 1);
			glTranslatef(300.0f, 0.0f, 0.0f);

			glPushMatrix();
				glRotatef(uranusRot, 0, 0, 1);
				glBindTexture(GL_TEXTURE_2D, uranusTex);
				uranus.draw();
				glBindTexture(GL_TEXTURE_2D, 0);
			glPopMatrix();
		glPopMatrix();


		// 해왕성
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glRotatef(neptuneRevolve, 0, 0, 1);
			glTranslatef(450.0f, 0.0f, 0.0f);

			glPushMatrix();
				glRotatef(neptuneRot, 0, 0, 1);
				glBindTexture(GL_TEXTURE_2D, neptuneTex);
				neptune.draw();
				glBindTexture(GL_TEXTURE_2D, 0);
			glPopMatrix();

		glPopMatrix();
	glPopMatrix();
}