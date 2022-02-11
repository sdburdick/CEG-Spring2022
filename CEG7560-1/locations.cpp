/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: Mace.cxx,v $
  Language:  C++
  Date:      $Date: 2002/09/30 20:36:40 $
  Version:   $Revision: 1.8 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkDebugLeaks.h"
#include "vtkGlyph3D.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
//#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"

#include <vtkProperty.h>
#include <vtkGeoMath.h>
#include <vtkPointData.h>
#include <vtkInformationVector.h>
#include <vtkDelimitedTextReader.h>
#include <vtkTable.h>
#include <vtkCellArray.h>

int main2(int argc, char* argv[])
{
    //vtkDebugLeaks::PromptUserOff();
    vtkNew<vtkActor> polyActor2;
    vtkRenderer* renderer = vtkRenderer::New();
    vtkRenderWindow* renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    vtkSphereSource* sphere = vtkSphereSource::New();
    sphere->SetThetaResolution(10); sphere->SetPhiResolution(10);
    vtkPolyDataMapper* sphereMapper = vtkPolyDataMapper::New();
    sphereMapper->SetInputConnection(sphere->GetOutputPort());
    vtkActor* sphereActor = vtkActor::New();
    sphereActor->SetMapper(sphereMapper);

    vtkConeSource* cone = vtkConeSource::New();
    cone->SetResolution(6);


    //begin point data loadout and manip
    //adapted from https://discourse.vtk.org/t/how-to-speed-up-about-loading-csv-file/1111
    vtkNew<vtkDelimitedTextReader> m_vtkCSVReader;
    polyActor2->GetProperty()->SetPointSize(4);

    m_vtkCSVReader->SetFileName("..//locations.dat");
    m_vtkCSVReader->DetectNumericColumnsOn();
    m_vtkCSVReader->SetFieldDelimiterCharacters(" ");
    m_vtkCSVReader->Update();

    vtkSmartPointer<vtkTable> table = m_vtkCSVReader->GetOutput();

    vtkNew<vtkPoints> pointsFromFile;

    int rows = table->GetNumberOfRows();
    int cols = table->GetNumberOfColumns();

    //convert the loaded points into their location on the globe:
    vtkNew<vtkGeoMath> geo;
    double rect[3];
    pointsFromFile->SetNumberOfPoints(rows);

    vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();
    pointIds->SetNumberOfIds(rows);

    for (vtkIdType i = 0; i < rows; ++i)
    {
        //pointIds->SetId(i, i);
    }


    vtkSmartPointer<vtkCellArray> PolyPoint = vtkSmartPointer<vtkCellArray>::New();

    for (vtkIdType i = 0; i < rows; i++)
    {
        double tableVals[3] = { table->GetValue(i, 0).ToDouble(), table->GetValue(i, 1).ToDouble(), table->GetValue(i, 2).ToDouble() };
        geo->LongLatAltToRect(tableVals, rect);
        //std::cout << "Point " << i << "| " << j << "= " << table->GetValue(j,i).ToDouble() << std::endl;
   //pointsFromFile->InsertNextPoint(rect[0]/6378000, rect[1] / 6378000, rect[2] / 6378000);
   // pointsFromFile->InsertNextPoint(rect);
        pointsFromFile->InsertNextPoint(i, i, i);
        pointIds->SetId(i, i);

        //pointsFromFile->InsertNextPoint(-i, -i, -i);

    }

    PolyPoint->InsertNextCell(pointIds);
    //vtkNew<vtkPolyData> polyDataPoints;
    vtkNew<vtkPolyData> polyDataPoints;
    vtkNew<vtkCellArray> dotAppearance;
    //dotAppearance->
    //polyDataPoints->SetVerts(

    polyDataPoints->SetPoints(pointsFromFile);
    polyDataPoints->SetVerts(PolyPoint);
    //end burdick



    vtkGlyph3D* glyph = vtkGlyph3D::New();
    glyph->SetInputConnection(sphere->GetOutputPort());
    //glyph->SetInputConnection(m_vtkCSVReader->GetOutputPort());
    glyph->SetSourceConnection(cone->GetOutputPort());
    glyph->SetVectorModeToUseNormal();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetScaleFactor(0.25);

    vtkPolyDataMapper* spikeMapper = vtkPolyDataMapper::New();
    spikeMapper->SetInputConnection(glyph->GetOutputPort());

    vtkActor* spikeActor = vtkActor::New();
    spikeActor->SetMapper(spikeMapper);

    renderer->AddActor(sphereActor);
    renderer->AddActor(spikeActor);
    renderer->SetBackground(1, 1, 1);
    renWin->SetSize(300, 300);

    // interact with data
    renWin->Render();

    int retVal = 0; //vtkRegressionTestImage( renWin );

    //if ( retVal == vtkRegressionTester::DO_INTERACTOR)
    {
        iren->Start();
    }
    // Clean up
    renderer->Delete();
    renWin->Delete();
    iren->Delete();
    sphere->Delete();
    sphereMapper->Delete();
    sphereActor->Delete();
    cone->Delete();
    glyph->Delete();
    spikeMapper->Delete();
    spikeActor->Delete();

    return !retVal;
}