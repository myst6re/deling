/****************************************************************************
 ** Makou Reactor Final Fantasy VII Field Script Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
 ** Copyright (C) 2020 Julian Xhokaxhiu <https://julianxhokaxhiu.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "Renderer.h"

// Get the size of a vector in bytes
template<typename T>
size_t vectorSizeOf(const typename std::vector<T>& vec)
{
  return sizeof(T) * vec.size();
}

Renderer::Renderer(QOpenGLWidget *_widget) :
    mProgram(_widget), mVertexShader(QOpenGLShader::Vertex, _widget), mFragmentShader(QOpenGLShader::Fragment, _widget),
    mVAO(this), mVertex(QOpenGLBuffer::VertexBuffer), mIndex(QOpenGLBuffer::IndexBuffer),
    mTexture(QOpenGLTexture::Target2D), _hasError(false), _buffersHaveChanged(true)
#ifdef QT_DEBUG
    , mLogger(_widget)
#endif
{
	mWidget = _widget;

	mGL.initializeOpenGLFunctions();

#ifdef QT_DEBUG
	connect(&mLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(messageLogged(QOpenGLDebugMessage)));

	if (mLogger.initialize()) {
		mLogger.startLogging();
	} else {
		qWarning() << "Cannot initialize OpenGL Debug Logger";
		if (!QOpenGLContext::currentContext()->hasExtension(QByteArrayLiteral("GL_KHR_debug"))) {
			qWarning() << "GL_KHR_debug extension is not available in this context";
		}
	}
#endif

	if (!mVertexShader.compileSourceFile(":/shaders/main.vert")) {
		qWarning() << "Cannot compile main.vert";
		_hasError = true;
	}
	if (!mVertexShader.log().isEmpty()) {
		qWarning() << "Warning during main.vert compilation" << mVertexShader.log();
	}
	if (!mFragmentShader.compileSourceFile(":/shaders/main.frag")) {
		qWarning() << "Cannot compile main.frag";
		_hasError = true;
	}
	if (!mFragmentShader.log().isEmpty()) {
		qWarning() << "Warning during main.frag compilation" << mFragmentShader.log();
	}

	if (!mProgram.addShader(&mVertexShader)) {
		qWarning() << "Cannot add the vertex shader";
		_hasError = true;
	}
	if (!mProgram.addShader(&mFragmentShader)) {
		qWarning() << "Cannot add the fragment shader";
		_hasError = true;
	}

	mProgram.bindAttributeLocation("a_position", ShaderProgramAttributes::POSITION);
	mProgram.bindAttributeLocation("a_color", ShaderProgramAttributes::COLOR);
	mProgram.bindAttributeLocation("a_texcoord", ShaderProgramAttributes::TEXCOORD);

	if (!mProgram.link()) {
		qWarning() << "Cannot link the program" << mProgram.log();
		_hasError = true;
	}
	if (!mProgram.bind()) {
		qWarning() << "Cannot bind the program";
		_hasError = true;
	}

	mProgram.setUniformValue("tex", 0);

	mGL.glEnable(GL_PROGRAM_POINT_SIZE);

	mGL.glEnable(GL_DEPTH_TEST);
	mGL.glDepthFunc(GL_LEQUAL);

	mGL.glDisable(GL_CULL_FACE);

	mGL.glDisable(GL_BLEND);
	
	// Vertex Array Object
	if (!mVAO.isCreated() && !mVAO.create()) {
#ifdef QT_DEBUG
		qWarning() << "Cannot create the vertex array object";
#endif
		return;
	}
}

void Renderer::clear()
{
	mGL.glClearColor(0, 0, 0, 0);
	mGL.glClearDepthf(1.0f);
	mGL.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::show()
{
	mWidget->update();
}

void Renderer::reset()
{
	mProjectionMatrix.setToIdentity();
	mViewMatrix.setToIdentity();
	mModelMatrix.setToIdentity();
}

bool Renderer::updateBuffers()
{
	// --- Before Draw ---
	mVAO.bind();
	mProgram.bind();

	// Vertex Buffer
	if (!mVertex.isCreated() && !mVertex.create()) {
#ifdef QT_DEBUG
		qWarning() << "QOpenGLBuffer buffers not supported (mVertex)";
#endif
		return false;
	}

	if (!mVertex.bind()) {
#ifdef QT_DEBUG
		qWarning() << "QOpenGLBuffer bind type not supported (mVertex)" << mVertex.type();
#endif
		return false;
	}

	if (_buffersHaveChanged) {
		mVertex.allocate(mVertexBuffer.data(), int(vectorSizeOf(mVertexBuffer)));
	}

	mProgram.enableAttributeArray(ShaderProgramAttributes::POSITION);
	mProgram.enableAttributeArray(ShaderProgramAttributes::COLOR);
	mProgram.enableAttributeArray(ShaderProgramAttributes::TEXCOORD);
	
	mProgram.setAttributeBuffer(ShaderProgramAttributes::POSITION, GL_FLOAT, 0, 4, sizeof(RendererVertex));
	mProgram.setAttributeBuffer(ShaderProgramAttributes::COLOR, GL_FLOAT, 4 * sizeof(GLfloat), 4, sizeof(RendererVertex));
	mProgram.setAttributeBuffer(ShaderProgramAttributes::TEXCOORD, GL_FLOAT, (4 * sizeof(GLfloat)) + (4 * sizeof(GLfloat)), 2, sizeof(RendererVertex));

	// Index Buffer
	if (mIndexBuffer.empty()) {
		for (uint32_t idx = 0; idx < mVertexBuffer.size(); idx++) {
			mIndexBuffer.push_back(idx);
		}
		_buffersHaveChanged = true;
	}

	if (!mIndex.isCreated() && !mIndex.create()) {
#ifdef QT_DEBUG
		qWarning() << "QOpenGLBuffer buffers not supported (mIndex)";
#endif
		return false;
	}

	if (!mIndex.bind()) {
#ifdef QT_DEBUG
		qWarning() << "QOpenGLBuffer bind type not supported (mIndex)" << mIndex.type();
#endif
		return false;
	}
	if (_buffersHaveChanged) {
		mIndex.allocate(mIndexBuffer.data(), int(vectorSizeOf(mIndexBuffer)));
	}
	
	_buffersHaveChanged = false;
	
	return true;
}

void Renderer::drawStart(float _pointSize)
{
	// --- Before Draw ---
	mVAO.bind();
	mProgram.bind();
	
	// Set Point Size
	mProgram.setUniformValue("pointSize", _pointSize);

	// Bind matrices
	mProgram.setUniformValue("modelMatrix", mModelMatrix);
	mProgram.setUniformValue("projectionMatrix", mProjectionMatrix);
	mProgram.setUniformValue("viewMatrix", mViewMatrix);
}

void Renderer::draw(RendererPrimitiveType _type, float _pointSize, bool clear)
{
	if (!updateBuffers()) {
		return;
	}

	drawStart(_pointSize);

	// --- Draw ---
	mGL.glDrawElements(GLenum(_type), GLsizei(mIndexBuffer.size()), GL_UNSIGNED_INT, nullptr);
#ifdef QT_DEBUG
	GLenum lastError = mGL.glGetError();
	if (lastError != GL_NO_ERROR) {
		qDebug() << "mGL.glDrawElements(_type, mIndexBuffer.size(), GL_UNSIGNED_INT, nullptr)" << lastError << _type << mIndexBuffer.size();
	}
#endif

	drawEnd(clear);
}

void Renderer::drawEnd(bool clear)
{
	// --- After Draw ---
	if (clear) {
		mVertex.release();
		mIndex.release();
		mVertexBuffer.clear();
		mIndexBuffer.clear();
		_buffersHaveChanged = true;
	}
	
	if (mTexture.isCreated() && mTexture.isBound()) {
		mTexture.release();
	}
	mGL.glDisable(GL_BLEND);

	mVAO.release();
	mProgram.release();
}

void Renderer::setViewport(int32_t _x, int32_t _y, int32_t _width, int32_t _height)
{
	mGL.glViewport(_x, _y, _width, _height);
}

void Renderer::bindModelMatrix(QMatrix4x4 _matrix)
{
	mModelMatrix = _matrix;
}

void Renderer::bindProjectionMatrix(QMatrix4x4 _matrix)
{
	mProjectionMatrix = _matrix;
}

void Renderer::bindViewMatrix(QMatrix4x4 _matrix)
{
	mViewMatrix = _matrix;
}

void Renderer::bindVertex(const RendererVertex *_vertex, uint32_t _count)
{
	for (uint32_t idx = 0; idx < _count; idx++) {
		mVertexBuffer.push_back(_vertex[idx]);
	}
	_buffersHaveChanged = _count > 0;
}

void Renderer::bindIndex(uint32_t *_index, uint32_t _count)
{
	for (uint32_t idx = 0; idx < _count; idx++) {
		mIndexBuffer.push_back(_index[idx]);
	}
	_buffersHaveChanged = _count > 0;
}

void Renderer::bindTexture(const QImage &_image, bool generateMipmaps)
{
	if (mTexture.isCreated()) {
		mTexture.destroy();
	}

	mTexture.create();
	mTexture.setMinificationFilter(QOpenGLTexture::NearestMipMapLinear);
	mTexture.setMagnificationFilter(QOpenGLTexture::Nearest);

	mTexture.setData(
		_image,
		generateMipmaps ? QOpenGLTexture::GenerateMipMaps
	                  : QOpenGLTexture::DontGenerateMipMaps
	);

	mTexture.bind();

	mGL.glEnable(GL_BLEND);
	mGL.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::bindTexture(QOpenGLTexture *texture)
{
	texture->bind();
	
	mGL.glEnable(GL_BLEND);
	mGL.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::bufferVertex(QVector3D _position, QRgba64 _color, QVector2D _texcoord)
{
	mVertexBuffer.push_back(
		RendererVertex {
			{ _position.x(), _position.y(), _position.z(), 1.0f},
			{ _color.red8() / float(UINT8_MAX), _color.green8() / float(UINT8_MAX), _color.blue8() / float(UINT8_MAX), _color.alpha8() / float(UINT8_MAX)},
			{ _texcoord.x(), _texcoord.y() }
		}
	);
}
#ifdef QT_DEBUG
void Renderer::messageLogged(const QOpenGLDebugMessage &msg)
{
	QString error;

	// Format based on severity
	switch (msg.severity())
	{
	case QOpenGLDebugMessage::NotificationSeverity:
		error += "NOTIFICATION";
		break;
	case QOpenGLDebugMessage::HighSeverity:
		error += "HIGH";
		break;
	case QOpenGLDebugMessage::MediumSeverity:
		error += "MEDIUM";
		break;
	case QOpenGLDebugMessage::LowSeverity:
		error += "LOW";
		break;
	case QOpenGLDebugMessage::InvalidSeverity:
		error += "INVALID";
		break;
	case QOpenGLDebugMessage::AnySeverity:
		error += "ANY";
		break;
	}

	error += "[";

	// Format based on source
#define CASE(c) case QOpenGLDebugMessage::c: error += #c; break
	switch (msg.source())
	{
	CASE(APISource);
	CASE(WindowSystemSource);
	CASE(ShaderCompilerSource);
	CASE(ThirdPartySource);
	CASE(ApplicationSource);
	CASE(OtherSource);
	CASE(InvalidSource);
	CASE(AnySource);
	}
#undef CASE

	error += "][";

	// Format based on type
#define CASE(c) case QOpenGLDebugMessage::c: error += #c; break
	switch (msg.type())
	{
	CASE(ErrorType);
	CASE(DeprecatedBehaviorType);
	CASE(UndefinedBehaviorType);
	CASE(PortabilityType);
	CASE(PerformanceType);
	CASE(OtherType);
	CASE(MarkerType);
	CASE(GroupPushType);
	CASE(GroupPopType);
	CASE(InvalidType);
	CASE(AnyType);
	}
#undef CASE

	error += "]";

	qDebug() << qPrintable(error) << qPrintable(msg.message());
}
#endif
