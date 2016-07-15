/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ==========
#include "Directus3D.h"
#include "IO/Log.h"
//=====================

// CONSTRUCTOR/DECONSTRUCTOR =========================
Directus3D::Directus3D(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_MSWindowsUseDirect3D, true);
	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);

    // This will make Qt update this widget as fast as possible.
    // Yes, paintEvent(QPaintEvent*) will be called also.
    // NOTE: I tested this technique and it yields thousands
    // of FPS, so it should do.
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
}

Socket* Directus3D::GetEngineSocket()
{

    return m_socket;
}

void Directus3D::Initialize(HWND hwnd, HINSTANCE hinstance)
{
    // Create and initialize Directus3D
    m_engine = new Engine();

    HINSTANCE hInstance = hinstance;
    HWND mainWindowHandle = hwnd;
    HWND widgetHandle = (HWND)this->winId();
    m_engine->Initialize(hInstance, mainWindowHandle, widgetHandle);

    // Get the socket
    m_socket = m_engine->GetSocket();
}

void Directus3D::Play()
{
    m_timer->start(0);
}

void Directus3D::Stop()
{
    m_timer->stop();
}

Directus3D::~Directus3D()
{
	ShutdownEngine();
}
//====================================================

//= OVERRIDDEN FUNCTIONS =============================
void Directus3D::resizeEvent(QResizeEvent* evt)
{
    int width = this->size().width();
    int height = this->size().height();

    height = width / (16.0f/9.0f);
    setGeometry(QRect(0, 0, width, height));
	Resize(width, height);
    update();
}

void Directus3D::paintEvent(QPaintEvent* evt)
{
    if (!m_socket)
        return;

    m_socket->Update();
    m_socket->Render();
}
//===================================================

//= Engine functions ================================
void Directus3D::ShutdownEngine()
{
	m_engine->Shutdown();
	delete m_engine;
}

void Directus3D::Resize(int width, int height)
{
    if (!m_socket)
        return;

	m_socket->SetViewport(width, height);
}
//===================================================
