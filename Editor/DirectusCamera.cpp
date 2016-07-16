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

//=========================
#include "DirectusCamera.h"
#include "IO/Log.h"
//=========================

DirectusCamera::DirectusCamera(QWidget *parent) : QWidget(parent)
{
    m_directusCore = nullptr;
}

void DirectusCamera::Initialize(DirectusCore* directusCore)
{
    m_directusCore = directusCore;
    m_gridLayout = new QGridLayout();
    m_validator = new QDoubleValidator(-2147483647, 2147483647, 4);

    //= TITLE =================================================
    m_title = new QLabel("Camera");
    m_title->setStyleSheet(
                "background-image: url(:/Images/camera.png);"
                "background-repeat: no-repeat;"
                "background-position: left;"
                "padding-left: 20px;"
                );
    //=========================================================

    //= BACKGROUND ============================================
    m_backgroundLabel = new QLabel("Background");
    m_background = new QPushButton("ClearColor");
    //=========================================================

    //= PROJECTION ============================================
    m_projectionLabel = new QLabel("Projection");
    m_projectionComboBox = new QComboBox();
    m_projectionComboBox->addItem("Perspective");
    m_projectionComboBox->addItem("Orthographic");
    //=========================================================

    //= FOV ===================================================
    m_fovLabel = new QLabel("Field of view");
    m_fov = new DirectusSliderText();
    m_fov->Initialize(1, 179);
    //=========================================================

    //= CLIPPING PLANES ==========================================================
    m_clippingPlanesLabel = new QLabel("Clipping planes");
    m_clippingNear = CreateQLineEdit();
    m_clippingFar = CreateQLineEdit();
    m_clippingPlanesNearLabel = new DirectusAdjustLabel();
    m_clippingPlanesNearLabel->setText("Near");
    m_clippingPlanesNearLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_clippingPlanesNearLabel->AdjustQLineEdit(m_clippingNear);
    m_clippingPlanesFarLabel = new DirectusAdjustLabel();
    m_clippingPlanesFarLabel->setText("Far");
    m_clippingPlanesFarLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_clippingPlanesFarLabel->AdjustQLineEdit(m_clippingFar);
    //=============================================================================

    //= LINE ======================================
    m_line = new QWidget();
    m_line->setFixedHeight(1);
    m_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_line->setStyleSheet(QString("background-color: #585858;"));
    //=============================================

    // addWidget(widget, row, column, rowspan, colspan)
    //= GRID ======================================================================
    // Row 0
    m_gridLayout->addWidget(m_title, 0, 0, 1, 3);

    // Row 1
    m_gridLayout->addWidget(m_backgroundLabel, 1, 0, 1, 1);
     m_gridLayout->addWidget(m_background, 1, 1, 1, 3);

    // Row 2
    m_gridLayout->addWidget(m_projectionLabel, 2, 0, 1, 1);
    m_gridLayout->addWidget(m_projectionComboBox, 2, 1, 1, 3);

    // Row 3
    m_gridLayout->addWidget(m_fovLabel, 3, 0, 1, 1);
    m_gridLayout->addWidget(m_fov->GetSlider(), 3, 1, 1, 2);
    m_gridLayout->addWidget(m_fov->GetLineEdit(), 3, 3, 1, 1);

    // Row 4 and 5
    m_gridLayout->addWidget(m_clippingPlanesLabel, 4, 0, 1, 1);
    m_gridLayout->addWidget(m_clippingPlanesNearLabel, 4, 1, 1, 1);
    m_gridLayout->addWidget(m_clippingNear, 4, 2, 1, 2);
    m_gridLayout->addWidget(m_clippingPlanesFarLabel, 5, 1, 1, 1);
    m_gridLayout->addWidget(m_clippingFar, 5, 2, 1, 2);

    // Row 6 - LINE
    m_gridLayout->addWidget(m_line, 6, 0, 1, 4);
    //=============================================================================

    // textChanged(QString) -> emits signal when changed through code
    // textEdit(QString) -> doesn't emit signal when changed through code
    connect(m_projectionComboBox, SIGNAL(activated(int)), this, SLOT(MapProjection()));
    connect(m_fov, SIGNAL(valueChanged(float)), this, SLOT(MapFOV()));
    connect(m_clippingNear, SIGNAL(textChanged(QString)), this, SLOT(MapClippingPlanes()));
    connect(m_clippingFar, SIGNAL(textChanged(QString)), this, SLOT(MapClippingPlanes()));

    this->setLayout(m_gridLayout);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    this->hide();
}

void DirectusCamera::Reflect(GameObject* gameobject)
{
    m_inspectedCamera = nullptr;

    // Catch evil case
    if (!gameobject)
    {
        this->hide();
        return;
    }

    // Catch the seed of the evil
    m_inspectedCamera = gameobject->GetComponent<Camera>();
    if (!m_inspectedCamera)
    {
        this->hide();
        return;
    }

    // Do the actual reflection
    SetProjection(m_inspectedCamera->GetProjection());
    SetFOV(m_inspectedCamera->GetFieldOfView());
    SetNearPlane(m_inspectedCamera->GetNearPlane());
    SetFarPlane(m_inspectedCamera->GetFarPlane());

    // Make this widget visible
    this->show();
}

void DirectusCamera::SetProjection(Projection projection)
{
    m_projectionComboBox->setCurrentIndex((int)projection);
}

void DirectusCamera::SetNearPlane(float nearPlane)
{
    m_clippingNear->setText(QString::number(nearPlane));
}

void DirectusCamera::SetFarPlane(float farPlane)
{
    m_clippingFar->setText(QString::number(farPlane));
}

void DirectusCamera::SetFOV(float fov)
{
    m_fov->SetValue(fov);
}

QLineEdit* DirectusCamera::CreateQLineEdit()
{
    QLineEdit* lineEdit = new QLineEdit();
    lineEdit->setValidator(m_validator);

    return lineEdit;
}

void DirectusCamera::MapProjection()
{
    if(!m_inspectedCamera || !m_directusCore)
        return;

    Projection projection = (Projection)(m_projectionComboBox->currentIndex());
    m_inspectedCamera->SetProjection(projection);
    m_directusCore->Update();
}

void DirectusCamera::MapFOV()
{
    if(!m_inspectedCamera || !m_directusCore)
        return;

    float fov = m_fov->GetValue();
    m_inspectedCamera->SetFieldOfView(fov);
    m_directusCore->Update();
}

void DirectusCamera::MapClippingPlanes()
{ 
    if(!m_inspectedCamera || !m_directusCore)
        return;

    float nearPlane = m_clippingNear->text().toFloat();
    float farPlane = m_clippingFar->text().toFloat();

    m_inspectedCamera->SetNearPlane(nearPlane);
    m_inspectedCamera->SetFarPlane(farPlane);
    m_directusCore->Update();
}
