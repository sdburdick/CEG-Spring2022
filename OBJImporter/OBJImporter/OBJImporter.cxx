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

  vtkNew<vtkOBJImporter> importer;
  importer->SetFileName(argv[1]);
  importer->SetFileNameMTL(argv[2]);
  importer->SetTexturePath(argv[3]);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
 
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;

  renderer->SetBackground2(colors->GetColor3d("Silver").GetData());
  renderer->SetBackground(colors->GetColor3d("Gold").GetData());
  
  renderer->GradientBackgroundOn();
  renWin->AddRenderer(renderer);
  renderer->UseHiddenLineRemovalOn();
  renWin->AddRenderer(renderer);
  renWin->SetSize(640, 480);
  renWin->SetWindowName("OBJImporter");

  iren->SetRenderWindow(renWin);
  importer->SetRenderWindow(renWin);
  importer->Update();



//add a base sphere that is tied to the size of the globe?
  
  //read the points?
  vtkSmartPointer<vtkPolyData> polyData;
  //auto polyData = ReadPolyData(argv[i]);
  //vtkNew<vtkSphereSource> source;
  //source->Update();
  //polyData = source->GetOutput();

  vtkNew<vtkNamedColors> colors2;

  // Set the background color.
  std::array<unsigned char, 4> bkg{ {26, 51, 102, 255} };
  colors2->SetColor("BkgColor", bkg.data());

  // This creates a polygonal cylinder model with eight circumferential facets
  // (i.e, in practice an octagonal prism).
  vtkNew<vtkCylinderSource> cylinder;
  cylinder->SetResolution(8);

  // The mapper is responsible for pushing the geometry into the graphics
  // library. It may also do color mapping, if scalars or other attributes are
  // defined.
  vtkNew<vtkPolyDataMapper> cylinderMapper;
  cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

  // The actor is a grouping mechanism: besides the geometry (mapper), it
  // also has a property, transformation matrix, and/or texture map.
  // Here we set its color and rotate it around the X and Y axes.
  vtkNew<vtkActor> cylinderActor;
  cylinderActor->SetMapper(cylinderMapper);
  cylinderActor->GetProperty()->SetColor(
      colors2->GetColor4d("Tomato").GetData());
  cylinderActor->RotateX(30.0);
  cylinderActor->RotateY(-45.0);

  // The renderer generates the image
  // which is then displayed on the render window.
  // It can be thought of as a scene to which the actor is added
  vtkNew<vtkRenderer> renderer2;
  renderer2->AddActor(cylinderActor);
  renderer2->SetBackground(colors->GetColor3d("BkgColor").GetData());
  // Zoom in a little by accessing the camera and invoking its "Zoom" method.
  renderer2->ResetCamera();
  renderer2->GetActiveCamera()->Zoom(0.5);

  // The render window is the actual GUI window
  // that appears on the computer screen
  //vtkNew<vtkRenderWindow> renderWindow;
  //renderWindow->SetSize(300, 300);
  //renderWindow->AddRenderer(renderer2);
  //enderWindow->SetWindowName("Cylinder");

  vtkNew<vtkActor> cylinderActor2;
  cylinderActor2->SetMapper(cylinderMapper);
  cylinderActor2->GetProperty()->SetColor(
      colors2->GetColor4d("Green").GetData());
  cylinderActor2->RotateX(30.0);
  cylinderActor2->RotateY(-75.0);
  vtkNew<vtkRenderer> renderer3;
  renderer3->AddActor(cylinderActor2);
  renderer3->SetBackground(colors->GetColor3d("BkgColor").GetData());
  // Zoom in a little by accessing the camera and invoking its "Zoom" method.
  renderer3->ResetCamera();
  renderer3->GetActiveCamera()->Zoom(0.25);

  vtkNew<vtkSphereSource> sphere;
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper);
  sphereActor->SetOrigin(2, 1, 3);
  sphereActor->RotateY(6);
  sphereActor->SetPosition(2.25, 0, 0);
  sphereActor->GetProperty()->SetColor(1, 0, 0);
  renderer2->AddActor(sphereActor);



  renWin->AddRenderer(renderer2);
  //renWin->AddRenderer(renderer3);

  // The render window interactor captures mouse events
  // and will perform appropriate camera or actor manipulation
  // depending on the nature of the events.
  //vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  //renderWindowInteractor->SetRenderWindow(renderWindow);

  // This starts the event loop and as a side effect causes an initial render.
  //renderWindow->Render();
  //renderWindowInteractor->Start();



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

    vtkPolyData* pd =
        dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInput());

    vtkPolyDataMapper* mapper =
        dynamic_cast<vtkPolyDataMapper*>(actor->GetMapper());
    mapper->SetInputData(pd);
  }
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
