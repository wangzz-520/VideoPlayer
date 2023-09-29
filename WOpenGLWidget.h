#ifndef WOPENGLWIDGET_H
#define WOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLTexture>
#include <QImage>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QDebug>
#include <QImage>
#include <QMouseEvent>
#include <QSet>
#include <QPainter>

class OpenGLDisplayImpl
{
public:
	OpenGLDisplayImpl()
	{
		textureY = NULL;
		textureU = NULL;
		textureV = NULL;
		videoW = 0;
		videoH = 0;
	}

	unsigned char *buffer[3] = {0};

	QOpenGLTexture*         textureY;
	QOpenGLTexture*         textureU;
	QOpenGLTexture*         textureV;

	GLsizei                 videoW, videoH;
};

class WOpenGLWidget : public QOpenGLWidget,public QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    WOpenGLWidget(QWidget* parent = Q_NULLPTR);
	~WOpenGLWidget();

public:
	void slotReceiveVideoData(uint8_t* yuvBuffer, int width, int height);

protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

private:
    QOpenGLShaderProgram *m_program = nullptr;          //着色器程序

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;

	OpenGLDisplayImpl *m_impl = nullptr;
};

#endif // WOPENGLWIDGET_H
