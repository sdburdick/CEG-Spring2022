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

#include <vtkGeoMath.h>
#include <vtkPointData.h>
#include <vtkInformationVector.h>
#include <vtkDelimitedTextReader.h>
#include <vtkTable.h>
#include <vtkCellArray.h>

//extern int main2(int argc, char* argv[]);
//#include "locations.cpp"

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
            
            //importer->Update();
            
            // Output the key that was pressed
            std::cout << "Pressed " << key << std::endl;
            double datal[3];
            sphere->GetCenter(datal);
            std::cout << "Sphere: [ " << datal[0] << ", " << datal[1] << ", " << datal[2] << ", " << sphere->GetRadius() << "]" << std::endl;
            // Handle an arrow key
            if (key == "Up")
            {
                sphere->SetCenter(datal[0] + .1, datal[1], datal[2]);
                std::cout << "The up arrow was pressed." << std::endl;
            }
            if (key == "Down")
            {
                sphere->SetCenter(datal[0] - .1, datal[1], datal[2]);
                std::cout << "The dn arrow was pressed." << std::endl;
            }
            if (key == "Left")
            {
                sphere->SetCenter(datal[0], datal[1]+.1, datal[2]);
                std::cout << "The lt arrow was pressed." << std::endl;
            }
            if (key == "Right")
            {
                sphere->SetCenter(datal[0], datal[1] - .1, datal[2]);
                std::cout << "The rt arrow was pressed." << std::endl;
            }
            if (key == "Prior")
            {
                polyActor->SetScale(*polyActor->GetScale()+1);
                std::cout << "The PgUp was pressed." << std::endl;
                std::cout << "Size is " << *polyActor->GetScale() << std::endl;
            }
            if (key == "Next")
            {
                if (*polyActor->GetScale() >= 1.0) {
                    polyActor->SetScale(*polyActor->GetScale()-1 );
                }
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
            renWin->Render();
            // Forward events
            vtkInteractorStyleTrackballCamera::OnKeyPress();
        }
    };
    vtkStandardNewMacro(KeyPressInteractorStyle);

} // namespace


int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cout
        << "Usage: " << argv[0]
        << " objfile mtlfile texturepath e.g. doorman.obj doorman.mtl doorman"
        << std::endl;
    return EXIT_FAILURE;
  }

  
  globeImporter->SetFileName(argv[1]);
  globeImporter->SetFileNameMTL(argv[2]);
  globeImporter->SetTexturePath(argv[3]);

  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindowInteractor> renWinInteractor;

  
  renderer->GradientBackgroundOn();
 
  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  
  

  //begin point data loadout and manip

  //adapted from https://discourse.vtk.org/t/how-to-speed-up-about-loading-csv-file/1111
  vtkNew<vtkDelimitedTextReader> m_vtkCSVReader;
  polyActor->GetProperty()->SetPointSize(4);

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

  vtkSmartPointer<vtkCellArray> PolyPoint = vtkSmartPointer<vtkCellArray>::New();
  
  for (vtkIdType i = 0; i < rows; i++)
  {
      double tableVals[3] = { table->GetValue(i, 0).ToDouble(), table->GetValue(i, 1).ToDouble(), table->GetValue(i, 2).ToDouble() };
      geo->LongLatAltToRect(tableVals, rect);
          //std::cout << "Point " << i << "| " << j << "= " << table->GetValue(j,i).ToDouble() << std::endl;
     //vtkIdType id = pointsFromFile->InsertNextPoint(rect[0]/6378000, rect[1] / 6378000, rect[2] / 6378000);
      vtkIdType id = pointsFromFile->InsertNextPoint(rect);
      //vtkIdType id = pointsFromFile->InsertNextPoint(i, i, i);
      PolyPoint->InsertNextCell(1);
      PolyPoint->InsertCellPoint(id);
      
      //pointsFromFile->InsertNextPoint(-i, -i, -i);
      
  }
  
  //PolyPoint->InsertNextCell(pointIds);
  //vtkNew<vtkPolyData> polyDataPoints;
  vtkNew<vtkPolyData> polyDataPoints;
  vtkNew<vtkCellArray> dotAppearance;
  //dotAppearance->
  //polyDataPoints->SetVerts(
  
    

  polyDataPoints->SetPoints(pointsFromFile);
  polyDataPoints->SetVerts(PolyPoint);


  mapper->SetInputData(polyDataPoints);
  //end data manipulation of locations.dat
  

  polyActor->SetMapper(mapper);
  polyActor->GetProperty()->SetDiffuseColor(
      colors->GetColor3d("Light_salmon").GetData());
  polyActor->GetProperty()->SetSpecular(0.6);
  polyActor->GetProperty()->SetSpecularPower(30);
  //polyActor->SetScale(117.25);
  polyActor->SetPosition(13.2, 7.4, -21.1);
  

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


  //::main2(argc, argv);

  renWin->Render();
  renWinInteractor->Start();


  
  return EXIT_SUCCESS;
}
