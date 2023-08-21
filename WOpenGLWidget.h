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
		bufYuv = nullptr;
		textureY = NULL;
		textureU = NULL;
		textureV = NULL;
		videoW = 0;
		videoH = 0;
		frameSize = 0;
	}

	unsigned char*          bufYuv;
	int                     frameSize;

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

public:
	void initBuffer(int width,int height);
	bool isInit();

public slots:
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

	bool m_isInit = false;
};

#endif // WOPENGLWIDGET_H
