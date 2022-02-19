#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJImporter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkRenderer.h>
#include <vtkTexture.h>
#include <vtkSphereSource.h>

#include <vtkCylinderSource.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

#include "vtkDataObjectToTable.h"
#include <vtkPolyDataMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkTextMapper.h>

#include <vtkAssembly.h>
#include <vtkGeoMath.h>
#include <vtkPointData.h>
#include <vtkInformationVector.h>
#include <vtkDelimitedTextReader.h>
#include <vtkTable.h>
#include <vtkCellArray.h>

//brought to global scope to allow keyboard callback to update
vtkNew<vtkOBJImporter> globeImporter;
vtkNew<vtkRenderWindow> renWin;
vtkNew<vtkSphereSource> sphere;
vtkNew<vtkActor> polyActor;


namespace {
    vtkSmartPointer<vtkPolyData> ReadPolyData(const char* fileName)
    {
        vtkSmartPointer<vtkPolyData> polyData;
        vtkNew<vtkSphereSource> source;
        source->SetPhiResolution(25);
        source->SetThetaResolution(25);
        source->Update();
        polyData = source->GetOutput();
        return polyData;
    }
} // namespace

//Burdick - this block of code allowed me to interact with the drawn area from the keyboard to better align data points / sizes
namespace {
    // Define interaction style
    class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static KeyPressInteractorStyle* New();
        vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

        virtual void OnKeyPress() override
        {
            // Get the keypress
            vtkRenderWindowInteractor* rwi = this->Interactor;
            std::string key = rwi->GetKeySym();
            // Output the key that was pressed
            std::cout << "Pressed " << key << std::endl;
            double datal[3];
            sphere->GetCenter(datal);
            std::cout << "Sphere: [ " << datal[0] << ", " << datal[1] << ", " << datal[2] << ", " << sphere->GetRadius() << "]" << std::endl;
            int x=0;
            int y=0;
            int z=0;

            if (key == "q")
            {
                std::cout << "The up arrow was pressed. " << polyActor->GetOrientationWXYZ()[3] << std::endl;
            }
            if (key == "a")
            {
                std::cout << "The up arrow was pressed. " << polyActor->GetOrientationWXYZ()[3] << std::endl;
            }
            if (key == "Up")
            {
                x--;
                std::cout << "The up arrow was pressed. " << polyActor->GetOrientationWXYZ()[3] << std::endl;
            }
            if (key == "Down")
            {
                x++;
                std::cout << "The up arrow was pressed. " << polyActor->GetOrientationWXYZ()[3] << std::endl;
            }
            if (key == "Left")
            {
                y--;
                std::cout << "The up arrow was pressed. " << polyActor->GetOrientationWXYZ()[1] << std::endl;
            }
            if (key == "Right")
            {
                y++;
                std::cout << "The up arrow was pressed. " << polyActor->GetOrientationWXYZ()[1] << std::endl;
            }
            if (key == "Prior")
            {
                z++;
                std::cout << "The PgUp was pressed." << std::endl;
                std::cout << "Size is " << *polyActor->GetScale() << std::endl;
            }
            if (key == "Next")
            {
                z--;
                std::cout << "Size is " << *polyActor->GetScale() << std::endl;
                std::cout << "The Pg Down was pressed." << std::endl;
            }
            if (key == "o")
            {
                sphere->SetCenter(datal[0], datal[1], datal[2]+.1);
                std::cout << "The PgUp was pressed." << std::endl;
            }
            if (key == "l")
            {
                
                sphere->SetCenter(datal[0], datal[1], datal[2]-.1);
                std::cout << "The Pg Down was pressed." << std::endl;
            }
            // Handle a "normal" key
            if (key == "a")
            {
                std::cout << "The a key was pressed." << std::endl;
            }
            polyActor->RotateWXYZ(x, 1, 0, 0);
            polyActor->RotateWXYZ(y, 0, 1, 0);
            polyActor->RotateWXYZ(z, 0, 0, 1);
            double polyOr[3];
            polyActor->GetOrientation(polyOr);
            std::cout << polyOr[0] << " " << polyOr[1] << " " << polyOr[2] << std::endl;
            renWin->Render();
            // Forward events
            vtkInteractorStyleTrackballCamera::OnKeyPress();
        }
    };
    vtkStandardNewMacro(KeyPressInteractorStyle);

} // namespace



int main(int argc, char* argv[])
{
    
  globeImporter->SetFileName("../globe/globe.obj");
  globeImporter->SetFileNameMTL("../globe/globe.mtl");
  globeImporter->SetTexturePath("none");

  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindowInteractor> renWinInteractor;

  
  renderer->GradientBackgroundOn();
 
  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  
  

  //begin point data loadout and manip

  //adapted from https://discourse.vtk.org/t/how-to-speed-up-about-loading-csv-file/1111
  vtkNew<vtkDelimitedTextReader> m_vtkCSVReader;
  polyActor->GetProperty()->SetPointSize(6);

  m_vtkCSVReader->SetFileName("..//locations.dat");
  //m_vtkCSVReader->SetFileName("..//oneloc.dat");
  m_vtkCSVReader->DetectNumericColumnsOn();
  m_vtkCSVReader->SetFieldDelimiterCharacters(" ");
  m_vtkCSVReader->Update();

  vtkSmartPointer<vtkTable> table = m_vtkCSVReader->GetOutput();

  vtkNew<vtkPoints> pointsFromFile;

  int rows = table->GetNumberOfRows();
  int cols = table->GetNumberOfColumns();

  ////////////////////////////////////////
  //convert the loaded points into their location on the globe:
  ////////////////////////////////////////
  vtkNew<vtkGeoMath> geo;
  double rect[3];
  pointsFromFile->SetNumberOfPoints(rows); 
  
  vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();
  pointIds->SetNumberOfIds(rows);

  vtkSmartPointer<vtkCellArray> PolyPoint = vtkSmartPointer<vtkCellArray>::New();
  
  double scalarVal = 7000;
  for (vtkIdType i = 0; i < rows; i++)
  {
      double tableVals[3] = { table->GetValue(i, 0).ToDouble(), table->GetValue(i, 1).ToDouble(), table->GetValue(i, 2).ToDouble() };
      geo->LongLatAltToRect(tableVals, rect);
      //these positions seem to be in long/lat form, with a lot of bad data
      vtkIdType id = pointsFromFile->InsertNextPoint((double)rect[1] / scalarVal, (double)rect[0] / scalarVal, (double)rect[2] / scalarVal);
      PolyPoint->InsertNextCell(1);
      PolyPoint->InsertCellPoint(id);
  }
  vtkNew<vtkPolyData> polyDataPoints;
  vtkNew<vtkCellArray> dotAppearance;
  
  polyDataPoints->SetPoints(pointsFromFile);
  polyDataPoints->SetVerts(PolyPoint);


  mapper->SetInputData(polyDataPoints);
  /////////////////////////////////////
  //end data manipulation of locations.dat
  ////////////////////////////////////////

  polyActor->SetMapper(mapper);
  polyActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Light_salmon").GetData());
  polyActor->GetProperty()->SetSpecular(0.6);
  polyActor->GetProperty()->SetSpecularPower(30);
  polyActor->SetScale(1.0/15.0);
  polyActor->SetPosition(13.2, 7.4, -21.1);
  

  //rotate IN THIS ORDER to match lat/long 0 0 to given earth graphic model
  polyActor->RotateWXYZ(-50, 0, 1, 0);//145
  polyActor->RotateWXYZ(48.39, 1, 0, 0);//-21
  polyActor->RotateWXYZ(104.6, 0, 0, 1);//54

  

  // Setup renderer
  renderer->AddActor(polyActor);
  renderer->SetBackground(colors->GetColor3d("mint").GetData());

  //model the ocean under the wireframe
  sphere->SetRadius(58.4);
  sphere->SetPhiResolution(100);
  sphere->SetThetaResolution(100);
  sphere->SetLatLongTessellation(true);
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->SetOrigin(0, 0, 0);
  sphereActor->SetPosition(13.2, 7.4, -21.1);
  sphereActor->GetProperty()->SetColor(0, 0, 1);
  sphereActor->GetProperty()->SetOpacity(1);
  //use the same renderer
  renderer->AddActor(sphereActor);
  //end sphere

  //add the parts to the window for display
//  renWin->AddRenderer(renderer);
  renderer->UseHiddenLineRemovalOn();
  renWin->AddRenderer(renderer);
  renWin->SetSize(800, 600);
  renWin->SetWindowName("Homework 1 - points on a globe");
  //2 5.5, -20.6, 57
  renWinInteractor->SetRenderWindow(renWin);
  vtkNew<KeyPressInteractorStyle> style;
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renWin);
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetCurrentRenderer(renderer);

  globeImporter->SetRenderWindow(renWin);
  globeImporter->Update();


  //vtkNew<vtkAssembly>assembly;
  //assembly->AddPart(sphereActor);
  //assembly->AddPart(polyActor);

  //::main2(argc, argv);

  renWin->Render();
  renWinInteractor->Start();

  
  
  return EXIT_SUCCESS;
}
