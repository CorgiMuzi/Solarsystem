
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

#pragma region Prop_for_drawing_Spheres
//void DrawTriangle(float* v1, float* v2, float* v3);
//void Normalize(float v[3]);
//void SubDivide(float* v1, float* v2, float* v3, long depth);

//static GLfloat vdata[12][3] = {
//   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
//   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
//   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
//};
//
//static GLint tindices[20][3] = {
//   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
//   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
//   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
//   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
#pragma endregion

bool InitGLSL();
CBitmap* GetImgBitmap(CImage* pImg);
GLuint LoadGLTexture(BITMAP* pBm);
GLuint LoadImgTexture(LPCTSTR fName); 

#pragma region Shader_Source
const char* vertexShaderSource = R"(
#version 330
in vec3 pos;
in vec2 texAttrib;
out vec2 passTexAttrib;

void main(){
gl_position = vec4(pos, 1.0f);
passTexAttrib = texAttrib;
)";

const char* fragmentShaderSource = R"(
#version 330
in vec2 passTexAttrib;
out vec4 fragmentColor;

uniform smapler2D tex;
void main(){
fragmentColor = texture(tex, passTexAttrib);
}
)";
#pragma endregion

#pragma region Property_for_Shader

GLuint shaderProgram = 0;
GLuint VBO = 0, VAO = 0;
GLuint earthTex;
GLuint quadPositionVBO;
GLuint quadTexVBO;


// temp
float position[] = {
	-0.5f, -0.5f,
	0.5f, -0.5f,
	0.5f, 0.5f,
	-0.5f, 0.5f
};

float texCoordinate[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};
#pragma endregion

float lineColor[] = { 5.0f, 0.0f, 0.0f, 1 };
float lineColor2[] = { 0.0f, 5.0f, 0.0f, 1 };

float sunRot = 0;
float earthRot = 0;
float merRot = 0;

Sphere sun(5.0f, 36, 18);
Sphere earth(3.0f, 36, 18);

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
	, m_view_type(VIEW_DEFAULT)
	, m_texNum(0)
{
	// TODO: 여기에 생성 코드를 추가합니다.
}

CSolarSystemView::~CSolarSystemView()
{
	while (m_globTexture.GetSize()) {
		GLuint nt = (UINT)m_globTexture[0];
		glDeleteTextures(1, &nt);
		m_globTexture.RemoveAt(0);
	}
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

	m_globTexture.Add((WORD)LoadImgTexture(_T("bmp/earth.bmp")));
	m_globTexture.Add((WORD)LoadImgTexture(_T("bmp/moon.bmp")));

	return 0;
}

void CSolarSystemView::InitGL(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Color Buffer 초기값 초기화
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.f); // Depth Buffer 초기값 초기화
	glEnable(GL_DEPTH_TEST); // Fragment Shader 수행 전 Depth Buffer 에 값을 저장할 Fragment 를 미리 선별하는 Depth Testing 실행
	glDepthFunc(GL_LEQUAL); // fragment 의 Depth 값이 Current Depth Buffer와 크기가 같거나 작을 경우 Depth Buffer 에 누적
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Color 와 Texture 의 보간 작업에 대해 GL_NICEST, 품질 우선으로 작업하도록 권고
	glEnable(GL_TEXTURE_2D);
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
	ReSizeGLScene(cx, cy);
}

void CSolarSystemView::ReSizeGLScene(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION); // 행렬 연산의 대상이 되는 스택을 Projection Stack 으로 변경
	glLoadIdentity();

	double screenRatio = (double)width / (double)height;

	glFrustum(-screenRatio, screenRatio, -1.0f, 1.0f, 1.0f, 10000.0f); // 원근감을 주기 위해 카메라 시야각 설정


}

void CSolarSystemView::DrawGLScene(void)
{
	wglMakeCurrent(m_hDC, m_hglRC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Color Buffer와 Depth Buffer에 존재하는 값을 초기화

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


	Display();

	// 화면 갱신
	SwapBuffers(m_hDC);

}

void GetColor(float color[]) {
	static int j = 0;
	j++;
	srand(j);
	for (int i = 0; i < 3; i++) {
		color[i] = (rand() % 256) / 255.0;
	}
}

#pragma region Shader_Part
bool InitGLSL() {
	GLenum err = glewInit();

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

	// Linking Two Shaders into the Shader Program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	// Setting Shader Program
	glUseProgram(shaderProgram);

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

CBitmap* GetImgBitmap(CImage* pImg) {
	if (pImg == NULL)
		return NULL;

	CDC* pDC = new CDC;
	if (!pDC->CreateCompatibleDC(CDC::FromHandle(pImg->GetDC())))
		return NULL;
	CBitmap* pBm = new CBitmap;
	pBm->CreateCompatibleBitmap(CDC::FromHandle(pImg->GetDC()), pImg->GetWidth(), pImg->GetHeight());

	CBitmap* pBmOld = pDC->SelectObject(pBm);
	pImg->BitBlt(pDC->m_hDC, CPoint(0, 0));
	pDC->DeleteDC();

	pImg->ReleaseDC();

	return pBm;
}

GLuint LoadGLTexture(BITMAP* pBm) {
	GLuint texi;
	glGenTextures(1, &texi);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, texi);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return texi;
}

GLuint LoadImgTexture(LPCTSTR fName) {
	CImage img;
	HRESULT hResult = img.Load(fName);
	if (FAILED(hResult)) {
		_TCHAR fmt[1028];
		_stprintf_s((_TCHAR*)fmt, hResult, _T("Error %d\n%s\nin file:\n%s"), _com_error(hResult).ErrorMessage(), fName);
		MessageBox(NULL, fmt, _T("Error:"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	CBitmap* pBm = NULL;
	if (img.GetBPP() != 24) {
		img.ReleaseDC();
		CImage tmg;
		tmg.Create(img.GetWidth(), img.GetHeight(), 24);
		img.BitBlt(tmg.GetDC(), CPoint(0, 0));
		pBm = GetImgBitmap(&tmg);
		tmg.ReleaseDC();
	}
	else {
		pBm = GetImgBitmap(&img);
		img.ReleaseDC();
	}

	BITMAP BMP;
	pBm->GetBitmap(&BMP);

	return LoadGLTexture(&BMP);
}
#pragma endregion

#pragma region 1st_Method_for_Drawing_Sphere
/// <summary>
/// 1st Method for Drawing Sphere
/// </summary>
/// <param name="r">radius</param>
/// <param name="lats">latitudes</param>
/// <param name="longs">longitudes</param>
//void CSolarSystemView::drawSphere(double r, int lats, int longs) {
//
//    int i, j;
//    for (i = 0; i <= lats; i++) {
//        double lat0 = PI * (-0.5 + (double)(i - 1) / lats);
//        double z0 = sin(lat0);
//        double zr0 = cos(lat0);
//
//        double lat1 = PI * (-0.5 + (double)i / lats);
//        double z1 = sin(lat1);
//        double zr1 = cos(lat1);
//
//        
//
//        glBegin(GL_POLYGON);
//        for (j = 0; j <= longs; j++) {
//            double lng = 2 * PI * (double)(j - 1) / longs;
//            double x = cos(lng);
//            double y = sin(lng);
//
//            float color[3];
//            //GetColor(color);
//            glColor3f(1.0f, 0.0f, 0.0f);
//            glNormal3f(x * zr0, y * zr0, z0);
//            glVertex3f(x * zr0, y * zr0, z0);
//            glNormal3f(x * zr1, y * zr1, z1);
//            glVertex3f(x * zr1, y * zr1, z1);
//        }
//
//        glEnd();
//    }
//
//}
#pragma endregion

#pragma region 2nd_Method_for_Drawing_Sphere
//void DrawTriangle(float* v1, float* v2, float* v3) {
//	// 알아보기 편하게 하기 위해 색 지정
//	/*float color[3];
//	GetColor(color);
//	glColor3fv(color);*/
//	glColor3f(1.0f, 0.0f, 1.0f);
//	glBegin(GL_LINE_STRIP);
//	glNormal3fv(v1); glVertex3fv(v1);
//	glNormal3fv(v2); glVertex3fv(v2);
//	glNormal3fv(v3); glVertex3fv(v3);
//	glEnd();
//}
//
//void Normalize(float v[3]) {
//	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
//	if (d == 0.0) {
//		return;
//	}
//	v[0] /= d;
//	v[1] /= d;
//	v[2] /= d;
//}
//
//
//void SubDivide(float* v1, float* v2, float* v3, long depth) {
//	GLfloat v12[3], v23[3], v31[3];
//	if (depth == 0) {
//		DrawTriangle(v1, v2, v3);
//		return;
//	}
//
//	for (int i = 0; i < 3; i++) {
//		v12[i] = (v1[i] + v2[i]) / 2.0;
//		v23[i] = (v2[i] + v3[i]) / 2.0;
//		v31[i] = (v3[i] + v1[i]) / 2.0;
//	}
//
//	Normalize(v12);
//	Normalize(v23);
//	Normalize(v31);
//
//	SubDivide(v1, v12, v31, depth - 1);
//	SubDivide(v2, v23, v12, depth - 1);
//	SubDivide(v3, v31, v23, depth - 1);
//	SubDivide(v12, v23, v31, depth - 1);
//}
//
//void CSolarSystemView::DrawSphere2() {
//	for (int i = 0; i < 20; i++) {
//		SubDivide(&vdata[tindices[i][0]][0],
//			&vdata[tindices[i][1]][0],
//			&vdata[tindices[i][2]][0], 3);
//	}
//}
#pragma endregion

void CSolarSystemView::Display() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, (GLuint)m_globTexture.GetAt(m_texNum));
	glRotatef(sunRot, 0.0f, 1.0f, 0.0f);
	glRotatef(sunRot, 0.5f, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	// Front Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	// Top Face
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	// Bottom Face
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	// Left Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	
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
