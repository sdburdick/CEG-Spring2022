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

#include <vtkInteractorStyleTrackballCamera.h>

int size = 50;

//brought to global scope to allow keyboard callback to update
vtkNew<vtkOBJImporter> importer;
vtkNew<vtkRenderWindow> renWin;
vtkNew<vtkSphereSource> sphere;


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
                sphere->SetRadius(sphere->GetRadius()+1);
                std::cout << "The PgUp was pressed." << std::endl;
            }
            if (key == "Next")
            {
                if (sphere->GetRadius() >= 1) {
                    sphere->SetRadius(sphere->GetRadius() - 1);
                }
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

  
  importer->SetFileName(argv[1]);
  importer->SetFileNameMTL(argv[2]);
  importer->SetTexturePath(argv[3]);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
 
  
  vtkNew<vtkRenderWindowInteractor> iren;

  renderer->SetBackground2(colors->GetColor3d("Silver").GetData());
  renderer->SetBackground(.2,.2,.4);
  
  renderer->GradientBackgroundOn();
    
  //read the points?
  vtkSmartPointer<vtkPolyData> polyData;
  

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
  renderer->AddActor(sphereActor);
  //end sphere

  //add the parts to the window for display
  renWin->AddRenderer(renderer);
  renderer->UseHiddenLineRemovalOn();
  renWin->AddRenderer(renderer);
  renWin->SetSize(800, 600);
  renWin->SetWindowName("Homework 1 - points on a globe");
  //2 5.5, -20.6, 57
  iren->SetRenderWindow(renWin);
  vtkNew<KeyPressInteractorStyle> style;
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renWin);
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetCurrentRenderer(renderer);

  importer->SetRenderWindow(renWin);
  importer->Update();

  auto actors = renderer->GetActors();
  actors->InitTraversal();
  std::cout << "There are " << actors->GetNumberOfItems() << " actors"
            << std::endl;

  for (vtkIdType a = 0; a < actors->GetNumberOfItems(); ++a)
  {
    std::cout << importer->GetOutputDescription(a) << std::endl;

    vtkActor* actor = actors->GetNextActor();

    // OBJImporter turns texture interpolation off
    if (actor->GetTexture())
    {
      std::cout << "Has texture\n";
      actor->GetTexture()->InterpolateOn();
    }

    //vtkPolyData* pd =
      //  dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInput());

//    vtkPolyDataMapper* mapper =
  //      dynamic_cast<vtkPolyDataMapper*>(actor->GetMapper());
    //mapper->SetInputData(pd);
  }
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
