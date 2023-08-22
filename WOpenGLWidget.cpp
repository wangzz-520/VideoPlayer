#include "WOpenGLWidget.h"


static float vertices[] = {
//     ---- 位置 ----    - 纹理坐标 -
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,   // 右上
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // 右下
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // 左下
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f    // 左上
};

static int indices[] = {
    0, 1, 3,
    1, 2, 3
};

WOpenGLWidget::WOpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
	, m_impl(new OpenGLDisplayImpl)
{
	
}

void WOpenGLWidget::initBuffer(int width, int height)
{
	if (m_impl && m_impl->bufYuv)
	{
		m_impl->bufYuv = new unsigned char[width * height * 1.5];
		memset(m_impl->bufYuv, 0, width * height * 1.5);

		m_impl->frameSize = width * height * 1.5;
		m_isInit = true;
	}	
}

bool WOpenGLWidget::isInit()
{
	return m_isInit;
}

void WOpenGLWidget::slotReceiveVideoData(uint8_t* yuvBuffer, int width, int height)
{
	if (!isInit())
	{
		initBuffer(width, height);
	}
	m_impl->videoW = width;
	m_impl->videoH = height;

	if (m_impl->bufYuv)
	{
		memcpy(m_impl->bufYuv, yuvBuffer, m_impl->frameSize);
		//update();
		update();
	}
}

void WOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/shaders/shapes.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/shaders/shapes.frag");
    m_program->link();

    //==========带纹理的图片使用的VAO VBO================
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);//绑定VAO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//把顶点数据复制到缓冲的内存中GL_STATIC_DRAW ：数据不会或几乎不会改变。

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);//解绑VAO
    //==========带纹理的图片使用的VAO VBO================

	m_impl->textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
	m_impl->textureY->create();

	m_impl->textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	m_impl->textureU->create();

	m_impl->textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
	m_impl->textureV->create();

    m_program->bind();
	m_program->setUniformValue("tex_y", 0);
	m_program->setUniformValue("tex_u", 1);
	m_program->setUniformValue("tex_v", 2);
}

void WOpenGLWidget::paintGL()
{
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);

    m_program->bind();
	m_impl->textureY->bind(0);
	m_impl->textureU->bind(1);
	m_impl->textureV->bind(2);

    //激活纹理单元0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_impl->textureY->textureId());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_impl->videoW, 
		m_impl->videoH, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_impl->bufYuv);
    //设置纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //激活纹理单元1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_impl->textureU->textureId());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_impl->videoW / 2, m_impl->videoH / 2
		, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char*)m_impl->bufYuv + m_impl->videoW * m_impl->videoH);
    //设置纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//激活纹理单元2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_impl->textureV->textureId());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_impl->videoW / 2, m_impl->videoH / 2
		, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char*)m_impl->bufYuv + m_impl->videoW * m_impl->videoH * 5 / 4);
	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindVertexArray(m_VAO);//绑定VAO
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
    glBindTexture(GL_TEXTURE_2D,0);
    glBindVertexArray(0);
}

void WOpenGLWidget::resizeGL(int w, int h)
{
}