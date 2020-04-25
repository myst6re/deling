#include "WorldmapGLWidget.h"

WorldmapGLWidget::WorldmapGLWidget(QWidget *parent,
                                   Qt::WindowFlags f) :
    QOpenGLWidget(parent, f), _map(Q_NULLPTR),
    _distance(-0.714248f), _xRot(-90.0f), _yRot(180.0f), _zRot(180.0f),
    _xTrans(-0.5f), _yTrans(0.5f), _transStep(360.0f), _lastKeyPressed(-1),
    _texture(-1), _segmentGroupId(-1), _segmentId(-1), _blockId(-1),
    _groundType(-1), _polyId(-1), _limits(QRect(0, 0, 32, 24))
{
	/* _xTrans = 0;
	_yTrans = .12f;
	_distance = -0.011123f;
	_xRot = -90;
	_yRot = 180;
	_zRot = 180; */
}

WorldmapGLWidget::~WorldmapGLWidget()
{
	makeCurrent();
	buf.destroy();

	for (const QList<QPair<QOpenGLTexture *, bool>> &texs : qAsConst(_textures)) {
		for (const QPair<QOpenGLTexture *, bool> &tex : qAsConst(texs)) {
			delete tex.first;
		}
	}

	delete _seaTexture;
	delete _roadTexture;
	delete _redTexture;
}

/*
QImage WorldmapGLWidget::toImage(int w, int h)
{
	QSize sz = size();
	if ((w > 0) && (h > 0))
		sz = QSize(w, h);

	QImage pm;
	if (context()->isValid()) {
		context()->makeCurrent();
		QGLFramebufferObject fbo(sz, QGLFramebufferObject::CombinedDepthStencil);
		fbo.bind();
		context()->setInitialized(false);
		uint prevDefaultFbo = d->glcx->d_ptr->default_fbo;
		d->glcx->d_ptr->default_fbo = fbo.handle();
		d->glcx->d_ptr->readback_target_size = sz;
		updateGL();
		fbo.release();
		pm = fbo.toImage();
		d->glcx->d_ptr->default_fbo = prevDefaultFbo;
		d->glcx->setInitialized(false);
		d->glcx->d_ptr->readback_target_size = QSize();
	}

	saveGLState();
	const int nrPics = 360 / DEGREES_BETWEEN_PICTURES;
	for (int i = 0; i < nrPics; i++) {
		catchFbo->bind();
		glColorMask(true, true, true, true);
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_MULTISAMPLE);
		glLoadIdentity();

		GLfloat x = GLfloat(PICTURE_SIZE) / PICTURE_SIZE;
		glFrustum(-x, +x, -1.0, +1.0, 1.0, 1000.0);
		glViewport(0, 0, PICTURE_SIZE, PICTURE_SIZE);

		drawScreenshot(i);
		catchFbo->release();

		QImage catchImage = catchFbo->toImage();
		catchImage.save("object/test" + QString::number(i) + ".png");
	}
	glDisable(GL_MULTISAMPLE);

	restoreGLState();
}
*/
void WorldmapGLWidget::setMap(Map *map)
{
	_map = map;

	_colorRegions[0] = qRgb(128, 0, 0);
	//_colorRegions[1] = qRgb(128, 0, 128);
	//_colorRegions[2] = qRgb(128, 0, 0);
	/* _colorRegions[3] = qRgb(0, 128, 0);
	_colorRegions[4] = qRgb(0, 0, 255);
	_colorRegions[6] = qRgb(128, 128, 128);
	_colorRegions[15] = qRgb(0, 128, 128);
	_colorRegions[19] = qRgb(128, 128, 0); */
	/* _colorRegions[5] = qRgb(255, 0, 255);
	_colorRegions[6] = qRgb(255, 255, 255);
	_colorRegions[7] = qRgb(128, 0, 0);
	_colorRegions[8] = qRgb(0, 128, 0);
	_colorRegions[9] = qRgb(0, 0, 128);
	_colorRegions[10] = qRgb(128, 128, 0);
	_colorRegions[11] = qRgb(0, 128, 128);
	_colorRegions[12] = qRgb(128, 0, 128);
	_colorRegions[13] = qRgb(128, 128, 128); */

	importVertices();

	update();
}

void WorldmapGLWidget::setLimits(const QRect &rect)
{
	_limits = rect;
	update();
}

void WorldmapGLWidget::setXTrans(float trans)
{
	_xTrans = trans;
	update();
}

void WorldmapGLWidget::setYTrans(float trans)
{
	_yTrans = trans;
	update();
}

void WorldmapGLWidget::setZTrans(float trans)
{
	_distance = trans;
	update();
}

void WorldmapGLWidget::setXRot(float rot)
{
	_xRot = rot;
	update();
}

void WorldmapGLWidget::setYRot(float rot)
{
	_yRot = rot;
	update();
}

void WorldmapGLWidget::setZRot(float rot)
{
	_zRot = rot;
	update();
}

void WorldmapGLWidget::setTexture(int texture)
{
	_texture = texture;
	update();
}

void WorldmapGLWidget::setSegmentGroupId(int segmentGroupId)
{
	_segmentGroupId = segmentGroupId;
	update();
}

void WorldmapGLWidget::setSegmentId(int segmentId)
{
	_segmentId = segmentId;
	update();
}

void WorldmapGLWidget::setBlockId(int blockId)
{
	_blockId = blockId;
	update();
}

void WorldmapGLWidget::setGroundType(int groundType)
{
	_groundType = groundType;
	update();
}

void WorldmapGLWidget::setPolyId(int polyId)
{
	_polyId = polyId;
	update();
}

void WorldmapGLWidget::dumpCurrent()
{
	const MapPoly &poly = _map->segments().at(_segmentId).blocks().at(_blockId).polygons().at(_polyId);
	qDebug() << QString::number(poly.flags1(), 16) << QString::number(poly.flags2(), 16)
	         << poly.groundType() << poly.texPage() << poly.clutId();
}

void WorldmapGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	importVertices();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_MULTISAMPLE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vshader->compileSourceFile(":/shaders/vertex.glsl");

	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	fshader->compileSourceFile(":/shaders/fragment.glsl");

	program = new QOpenGLShaderProgram;
	program->addShader(vshader);
	program->addShader(fshader);
	program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	program->link();

	program->bind();
	program->setUniformValue("texture", 0);
	program->setUniformValue("palettes", 1);
}

static quint16 normalizeY(qint16 y)
{
	return quint16(-(y - 128));
}

static QOpenGLTexture *textureFromImage(const QImage &image)
{
	QOpenGLTexture *texture;
	/* if (image.format() == QImage::Format_Indexed8 || image.format() == QImage::Format_Grayscale8) {
		qDebug() << "Indexed";
		texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
		texture->setMinificationFilter(QOpenGLTexture::Nearest);
		texture->setMagnificationFilter(QOpenGLTexture::Nearest);
		texture->setFormat(QOpenGLTexture::R8_UNorm);
		texture->setSize(image.width(), image.height());
		texture->setMipLevels(0);
		texture->allocateStorage();
		// Upload pixel data
		QOpenGLPixelTransferOptions uploadOptions;
		uploadOptions.setAlignment(1);
		texture->setData(0, QOpenGLTexture::Red, QOpenGLTexture::UInt8, image.constBits(), &uploadOptions);
	} else { */
	texture = new QOpenGLTexture(image);
	texture->setMinificationFilter(QOpenGLTexture::NearestMipMapLinear);
	texture->setMagnificationFilter(QOpenGLTexture::Nearest);
	//}

	return texture;
}

void WorldmapGLWidget::importVertices()
{
	if (Q_NULLPTR == _map) {
		return;
	}

	QList< QList< QPair<QImage, bool> > > images = _map->textureImages();
	QImage seaImage = _map->seaTextureImage(),
	        roadImage = _map->roadTextureImage();

	// Set HasBlackPixels attribute on polys
	//_map->searchBlackPixels(images, seaImage, roadImage);

	for (const QList< QPair<QImage, bool> > &images : qAsConst(images)) {
		QList< QPair<QOpenGLTexture *, bool> > texs;

		for (const QPair<QImage, bool> &image : qAsConst(images)) {
			texs.append(qMakePair(textureFromImage(image.first), image.second));
		}

		_textures.append(texs);
	}

	_seaTexture = textureFromImage(seaImage);
	_roadTexture = textureFromImage(roadImage);

	QImage redImage(256, 256, QImage::Format_RGB32);
	redImage.fill(Qt::red);
	_redTexture = textureFromImage(redImage);

	/* for (int i = 0; i < 9; ++i) {
		_specialTextures.append(QList<QOpenGLTexture *>()
		                 << textureFromImage(
		                     _map->specialTextureImage(
		                         Map::SpecialTextureName(int(Map::Sea1) + i))));
	} */

	QVector<GLfloat> vertData;

	const int segmentPerLine = 32, blocksPerLine = 4,
	        diffSize = _limits.width() - _limits.height();
	const float scaleVect = 2048.0f, scaleTex = 255.0f, scale = _limits.width() * blocksPerLine;
	const float xShift = -_limits.x() * blocksPerLine + (diffSize < 0 ? -diffSize : 0) * blocksPerLine / 2.0f;
	const float zShift = -_limits.y() * blocksPerLine + (diffSize > 0 ? diffSize : 0) * blocksPerLine / 2.0f;
	int xs = 0, ys = 0;
	QMap<QPair<quint8, quint8>, QList< QPair<quint8, quint8> > > collect4;
	QMap<QPair<quint8, quint8>, QList<quint8> > collect1;
	QMap<quint8, bool> collect2, collect3;
	/* QMap<quint8, quint8> maxWPerGround, maxHPerGround;

	foreach (const MapSegment &segment, _map->segments()) {
		foreach (const MapBlock &block, segment.blocks()) {
			foreach (const MapPoly &poly, block.polygons()) {
				if (maxW.contains(poly.groundType())) {

				} else {
					maxW.insert(poly.groundType(), poly.)
				}


				for (quint8 i = 0; i < 3; ++i) {
					const TexCoord &tc = poly.texCoord(i);

				}
			}
		}
	} */

	QList< QVector<GLfloat> > drawAfter;
	foreach (const MapSegment &segment, _map->segments()) {
		int xb = 0, yb = 0;
		foreach (const MapBlock &block, segment.blocks()) {
			foreach (const MapPoly &poly, block.polygons()) {
				const int x = xs * blocksPerLine + xb, z = ys * blocksPerLine + yb;

				if (poly.vertices().size() != 3) {
					qWarning() << "Wrong vertices size" << poly.vertices().size();
					return;
				}

				QVector<GLfloat> vertDataPoly;

				for (quint8 i = 0; i < 3; ++i) {
					const Vertex &v = poly.vertex(i);
					const TexCoord &tc = poly.texCoord(i);
					vertDataPoly.append((xShift + x + v.x / scaleVect) / scale);
					vertDataPoly.append(normalizeY(v.y) / scaleVect / scale);
					vertDataPoly.append((zShift + z - v.z / scaleVect) / scale);
					if (poly.isRoadTexture()) {
						vertDataPoly.append(tc.x / float(_roadTexture->width() - 1));
						vertDataPoly.append(tc.y / float(_roadTexture->height() - 1));
					} else if (poly.isWaterTexture()) {
						vertDataPoly.append(tc.x / float(_seaTexture->width() - 1));
						vertDataPoly.append(tc.y / float(_seaTexture->height() - 1));
					} else {
						vertDataPoly.append(tc.x / scaleTex);
						vertDataPoly.append(tc.y / scaleTex);
					}
				}

				if (false && poly.isTransparent()) {
					drawAfter.append(vertDataPoly);
				} else {
					vertData.append(vertDataPoly);
				}

				/* for (quint8 i = 0; i < 16; ++i) {
					quint8 val = ((i < 8 ? poly.u1() : poly.u2()) >> (i % 8)) & 1;
					if (val) {
						if (poly.groundType() == 10 || poly.groundType() >= 31) {
							collect2.insert(i, true);
						} else if (poly.groundType() != 10 && poly.groundType() < 25) {
							collect3.insert(i, true);
						}
					}
				} */
				/* if (poly.groundType() == 31) { //poly.groundType() == 10 || poly.groundType() >= 31) {
					collect2.insert(poly.clutId(), true);
				} else if (poly.groundType() != 10 && poly.groundType() < 25) {
					collect3.insert(poly.u1(), true);
				} */

				/* if (poly.groundType() == 10) {
					qDebug() << poly.texCoord(0).x << poly.texCoord(0).y
								 << poly.texCoord(1).x << poly.texCoord(1).y
									 << poly.texCoord(2).x << poly.texCoord(2).y;
				} */
			}

			xb += 1;

			if (xb >= blocksPerLine) {
				xb = 0;
				yb += 1;
			}
		}

		xs += 1;

		if (xs >= segmentPerLine) {
			xs = 0;
			ys += 1;
		}
	}

	foreach (const QVector<GLfloat> &floats, drawAfter) {
		vertData.append(floats);
	}

	QMapIterator<QPair<quint8, quint8>, QList<quint8> > it(collect1);
	while(it.hasNext()) {
		it.next();
		qDebug() << it.key() << it.value();
	}
	/* qDebug() << collect4.keys(); */
	qDebug() << collect2.keys();
	qDebug() << collect3.keys();

	buf.create();
	buf.bind();
	buf.allocate(vertData.constData(), vertData.count() * int(sizeof(GLfloat)));
}

static GLdouble deg2rad(GLdouble deg)
{
	const GLdouble pi = 3.1415926535897932384626433832795;
	return deg / 360 * pi;
}

static void perspectiveGL(GLdouble fovY, GLdouble aspect,
                          GLdouble zNear, GLdouble zFar)
{
	GLdouble fW, fH;

	fH = tan(deg2rad(fovY)) * zNear;
	fW = fH * aspect;

	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void WorldmapGLWidget::resizeGL(int w, int h)
{
	//glViewport(0, 0, GLint(w), GLint(h));

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	_matrixProj.setToIdentity();
	_matrixProj.perspective(70.0f, GLfloat(w) / h, 0.000001f, 1000.0f);

	//perspectiveGL(70.0, double(w) / double(h), 0.000001, 1000.0);

	//glMatrixMode(GL_MODELVIEW);
}

void WorldmapGLWidget::paintGL()
{
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (Q_NULLPTR == _map) {
		return;
	}

	QMatrix4x4 m;
	//m.ortho(-1.0f, 1.0f, -1.0f, 1.0f, 4.0f, 15.0f);
	m.translate(_xTrans, _yTrans, _distance);
	m.rotate(_xRot, 1.0f, 0.0f, 0.0f);
	m.rotate(_yRot, 0.0f, 1.0f, 0.0f);
	m.rotate(_zRot, 0.0f, 0.0f, 1.0f);

	program->setUniformValue("projMatrix", _matrixProj);
	program->setUniformValue("mvMatrix", m);
	program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

	int bufIndex = 0, alphaLocation = program->uniformLocation("alpha");
	QTime t;
	t.start();

	program->setUniformValue(alphaLocation, 1.0f);
	const QList<quint8> &regions = _map->encounterRegions();

	QList<MapPoly> drawAfter;
	int segmentId = 0;
	foreach (const MapSegment &segment, _map->segments()) {
		bool isInRegion = _texture >= 0 && regions.at(segmentId) == _texture;
		int blockId = 0;
		foreach (const MapBlock &block, segment.blocks()) {
			int polyId = 0;
			foreach (const MapPoly &poly, block.polygons()) {

				if (_texture >= 0) {
					program->setUniformValue(alphaLocation, 0.5f);

					if (poly.isTransparent()) {
						program->setUniformValue(alphaLocation, 0.0f);
					}

					if (isInRegion && poly.groundType() <= 31) {
						program->setUniformValue(alphaLocation, 1.0f);

						if (poly.isTransparent()) {
							program->setUniformValue(alphaLocation, 0.5f);
						}
					}
				}

				if (_segmentGroupId >= 0) {
					if (_segmentGroupId == segment.groupId()) {
						program->setUniformValue(alphaLocation, 1.0f);
					} else {
						program->setUniformValue(alphaLocation, 0.5f);
					}
				}

				if (_segmentId >= 0) {
					if (_segmentId == segmentId) {
						program->setUniformValue(alphaLocation, 1.0f);
					} else {
						program->setUniformValue(alphaLocation, 0.5f);
					}
				}

				if (_blockId >= 0) {
					if (_blockId == blockId) {
						program->setUniformValue(alphaLocation, 1.0f);
					} else {
						program->setUniformValue(alphaLocation, 0.5f);
					}
				}

				if (_groundType >= 0) {
					if (_groundType == poly.groundType()) {
						program->setUniformValue(alphaLocation, 1.0f);
					} else {
						program->setUniformValue(alphaLocation, 0.5f);
					}
				}

				if (_polyId >= 0) {
					if (_polyId == polyId) {
						program->setUniformValue(alphaLocation, 1.0f);
					} else {
						program->setUniformValue(alphaLocation, 0.5f);
					}
				}

				/* if (poly.isTransparent()) {
					program->setUniformValue(alphaLocation, 0.0f);
				} */

				polyId += 1;

				if (poly.isRoadTexture()) {
					_roadTexture->bind();
				} else if (poly.isWaterTexture()) {
					_seaTexture->bind();
				} else if (poly.texPage() < _textures.size() && poly.clutId() < _textures.at(poly.texPage()).size()) {
					QPair<QOpenGLTexture *, bool> p = _textures.at(poly.texPage()).at(poly.clutId());
					p.first->bind();
					/* const OpenGLPalettedTexture &paltex = _textures.at(poly.texPage());
					program->setUniformValue(palIdLocation, (poly.clutId() + 0.5f) / paltex.palettes()->height());
					program->setUniformValue(palMultiplierLocation, paltex.paletteMultiplier());
					glActiveTexture(GL_TEXTURE0);
					paltex.texture()->bind();
					glActiveTexture(GL_TEXTURE1);
					paltex.palettes()->bind(); */
				} else {
					qDebug() << poly.texPage() << poly.clutId();
					//glActiveTexture(GL_TEXTURE0);
					_redTexture->bind();
					/* glActiveTexture(GL_TEXTURE1);
					_redTexture->bind(); */
				}

				glDrawArrays(GL_TRIANGLES, bufIndex * 3, 3);

				bufIndex += 1;
			}
			blockId += 1;
		}
		segmentId += 1;
	}

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//program->setUniformValue(alphaLocation, 0.5f);

	/* foreach (const MapPoly &poly, drawAfter) {
		if (poly.isRoadTexture()) {
			_roadTexture->bind();
		} else if (poly.isWaterTexture()) {
			_seaTexture->bind();
		} else if (poly.texPage() < _textures.size() && poly.clutId() < _textures.at(poly.texPage()).size()) {
			_textures.at(poly.texPage()).at(poly.clutId()).first->bind();
		} else {
			qDebug() << poly.texPage() << poly.clutId();
			_redTexture->bind();
		}

		glDrawArrays(GL_TRIANGLES, bufIndex * 3, 3);

		bufIndex += 1;
	} */

	qDebug() << t.elapsed();
}

void WorldmapGLWidget::wheelEvent(QWheelEvent *event)
{
	setFocus();
	_distance += event->delta() / 4096.0;
	update();
}

void WorldmapGLWidget::mousePressEvent(QMouseEvent *event)
{
	setFocus();
	_moveStart = event->pos();
	if(event->button() == Qt::MidButton)
	{
		_distance = -35;
		update();
	}
}

void WorldmapGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	/* if(event->modifiers() == Qt::CTRL) {
		if(event->button() == Qt::LeftButton)
		{
			_xRot += fmod((event->pos().x() - _moveStart.x()) / 4096.0f, 360.0f);
		}
		else if(event->button() == Qt::RightButton)
		{
			_yRot -= fmod((event->pos().y() - _moveStart.y()) / 4096.0f, 360.0f);
		}
	} else if(event->button() == Qt::LeftButton) {
		_xTrans += (event->pos().x() - _moveStart.x()) / 4096.0;
		_yTrans -= (event->pos().y() - _moveStart.y()) / 4096.0;
	}
	_moveStart = event->pos();
	updateGL(); */
}

void WorldmapGLWidget::keyPressEvent(QKeyEvent *event)
{
	if(_lastKeyPressed == event->key()
	        && (event->key() == Qt::Key_Left
	            || event->key() == Qt::Key_Right
	            || event->key() == Qt::Key_Down
	            || event->key() == Qt::Key_Up)) {
		if(_transStep > 100.0f) {
			_transStep *= 0.90f; // accelerator
		}
	} else {
		_transStep = 360.0f;
	}
	_lastKeyPressed = event->key();

	switch(event->key())
	{
	case Qt::Key_Left:
		_xTrans += 1.0f/_transStep;
		update();
		break;
	case Qt::Key_Right:
		_xTrans -= 1.0f/_transStep;
		update();
		break;
	case Qt::Key_Down:
		_yTrans += 1.0f/_transStep;
		update();
		break;
	case Qt::Key_Up:
		_yTrans -= 1.0f/_transStep;
		update();
		break;
	case Qt::Key_7:
		_xRot += 0.1f;
		update();
		break;
	case Qt::Key_1:
		_xRot -= 0.1f;
		update();
		break;
	case Qt::Key_8:
		_yRot += 0.1f;
		update();
		break;
	case Qt::Key_2:
		_yRot -= 0.1f;
		update();
		break;
	case Qt::Key_9:
		_zRot += 0.1f;
		update();
		break;
	case Qt::Key_3:
		_zRot -= 0.1f;
		update();
		break;
	default:
		QWidget::keyPressEvent(event);
		return;
	}
}

void WorldmapGLWidget::focusInEvent(QFocusEvent *event)
{
	grabKeyboard();
	QWidget::focusInEvent(event);
}

void WorldmapGLWidget::focusOutEvent(QFocusEvent *event)
{
	releaseKeyboard();
	QWidget::focusOutEvent(event);
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

QRgb WorldmapGLWidget::groundColor(quint8 groundType, quint8 region,
                                   const QSet<quint8> &grounds)
{
	QRgb c;

	switch(groundType) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		c = qRgb(57, 71, 45);
		break;
	case 6:
	case 15:
	case 24:
		c = qRgb(75, 80, 55);
		break;
	case 7:
		c = qRgb(106, 78, 63);
		break;
	case 8:
		c = qRgb(145, 124, 109);
		break;
	case 9:
		c = qRgb(144, 157, 164);
		break;
	case 10:
		//c = qRgb(122, 143, 158);
		//c = qRgb(0xD0, 0xD0, 0xBA); // yellow
		c = qRgb(0xA1, 0xA1, 0x8C);
		break;
	case 14:
		c = qRgb(116, 100, 90);
		break;
	case 16:
	case 25:
		c = qRgb(103, 85, 72);
		break;
	case 17:
	case 18:
	case 23:
		c = qRgb(57, 60, 53);
		break;
	case 27:
		c = qRgb(112, 97, 86);
		break;
	case 28:
		c = qRgb(69, 65, 64);
		break;
	case 29:
		c = qRgb(133, 108, 91);
		break;
	case 31:
		c = qRgb(53, 74, 75);
		break;
	case 32:
		c = qRgb(56, 88, 99);
		break;
	case 33:
		c = qRgb(40, 65, 81);
		break;
	case 34:
		c = qRgb(35, 60, 75);// 56, 88, 99
		break;
	default:
		const quint8 grey = 55 + groundType * 200 / 34;
		c = qRgb(grey, grey, grey);
		break;
	}

	if (/* grounds.contains(groundType) &&*/groundType >= 29 || !_colorRegions.contains(region)) {
		//QRgb cRegion = _colorRegions.value(region, Qt::transparent);
		QRgb cRegion = qRgb(128, 128, 128);
		/* int r = qRed(cRegion) * .5 + qRed(c),
			g = qGreen(cRegion) * .5 + qGreen(c),
			b = qBlue(cRegion) * .5 + qBlue(c); */
		int r = qRed(c) - qRed(cRegion) * .33,
		    g = qGreen(c) - qGreen(cRegion) * .33,
		    b = qBlue(c) - qBlue(cRegion) * .33;

		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;

		c = qRgb(r, g, b);
	}

	return c;
}
