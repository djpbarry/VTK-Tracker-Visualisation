
#include "stdafx.h"
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkCamera.h>
#include "read_trajs.h"
#include <boost/math/special_functions/round.hpp>
#include <boost/random.hpp>

using namespace boost;
int dim=3;
void buildBoxPolyData(vtkSmartPointer<vtkPoints> boxPoints, vtkSmartPointer<vtkPolyData> boxData, int* params);
 
class vtkTimerCallback2 : public vtkCommand
{
  public:
    static vtkTimerCallback2 *New()
    {
      vtkTimerCallback2 *cb = new vtkTimerCallback2;
      cb->TimerCount = 0;
      return cb;
    }

	void Initialise(double* trajectories, int frames, int objects, int dim, vtkRenderer* renderer, vtkCamera* camera)
    {
	  this->trajectories = trajectories;
	  this->frames = frames;
	  this->objects=objects;
	  //this->dim=dim;
	  this->renderer=renderer;
	  this->camera=camera;
      return;
    }
 
    virtual void Execute(vtkObject *caller, unsigned long eventId,
                         void * vtkNotUsed(callData))
    {
		if (vtkCommand::TimerEvent == eventId)
      {
        ++this->TimerCount;
		if(this->TimerCount > this->frames-1){
			this->TimerCount = 1;
		}
      }
		for(int i=0; i<objects; i++){
		  int tOffset = (TimerCount-1) * objects * dim;
		  int oOffset = tOffset + i * dim;
			//int tOffset = (TimerCount-1) * objects;
		 // int oOffset = tOffset + i;
		  double x = trajectories[oOffset];
		  double y = trajectories[oOffset + 1];
		  double multi = trajectories[oOffset + 3];
		  double z = (TimerCount-1);
		  if(x > 0.0 && y > 0.0){
			  actor[i]->VisibilityOn();
			actor[i]->SetPosition(x, y, z);
			if(multi > 0.0){
				actor[i]->GetProperty()->SetColor(0.0, 1.0, 0.0);
			} else {
				actor[i]->GetProperty()->SetColor(1.0, 0.0, 0.0);
			}
		  } else {
			  actor[i]->VisibilityOff();
		  }
		}
		//float scale = 800.0f;
		//float cameraX = scale * sin((float)TimerCount/frames);
		//float cameraY = scale * cos((float)TimerCount/frames);
		//float cameraZ = scale * sin((float)TimerCount/frames);
		//camera->SetPosition(cameraX, cameraY, cameraZ);

      vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
      iren->GetRenderWindow()->Render();

	 // vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
	 // windowToImageFilter->SetInput(iren->GetRenderWindow());
		//windowToImageFilter->SetMagnification(3); //set the resolution of the output image (3 times the current resolution of vtk render window)
		//windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
		//windowToImageFilter->Update();
 
		//std::string savefilename("C:/users/barry05/Desktop/VTKTrackerVis/");
  //      savefilename.append(boost::lexical_cast<std::string > (TimerCount));
  //      savefilename.append(".png");

	 // vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
	 // writer->SetFileName(savefilename.data());
	 // writer->SetInputConnection(windowToImageFilter->GetOutputPort());
	 // writer->Write();
    }
 
  private:
    int TimerCount;
	double* trajectories;
	int frames;
	int objects;
	vtkRenderer* renderer;
	vtkCamera* camera;
  public:
    vtkActor** actor;
};

int main(int, char* [])
{
	int numParams = 4;
	int* params = (int*) malloc(sizeof(int) * numParams);
	char* paramNames = (char*) malloc(numParams * MAX_LINE * sizeof(char));
	memcpy(&paramNames[0], FRAMES, MAX_LINE);
	memcpy(&paramNames[MAX_LINE], TRAJS, MAX_LINE);
	memcpy(&paramNames[2 * MAX_LINE], WIDTH, MAX_LINE);
	memcpy(&paramNames[3 * MAX_LINE], HEIGHT, MAX_LINE);
	//memcpy(&paramNames[4 * MAX_LINE], DIM, MAX_LINE);
	FILE *file;
	FILE **filePointer = &file;
	fopen_s(filePointer, _trajfilename_, "r");
	for(int i=0; i<numParams; i++){
		loadFloatParam(paramNames, numParams, params, "%s %f", MAX_LINE, 'i', file);
	}
	/*double* trajectories = (double*) malloc(sizeof(double) * params[0] * params[1] * params[4]);*/
	double* trajectories = (double*) malloc(sizeof(double) * params[0] * params[1]*dim);
	//loadTrajectories(file, trajectories, params[0], params[1], MAX_LINE, params[4]);
	loadTrajectories(file, trajectories, params[0], params[1], MAX_LINE, dim);
	fclose(file);

  // Create a sphere
  vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
  //sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(2.0);
  sphereSource->SetPhiResolution(20.0);
  sphereSource->SetThetaResolution(20.0);
  sphereSource->Update();

  //Create trajectory line
  vtkSmartPointer<vtkPoints> trajPoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkPolyData> trajData = vtkSmartPointer<vtkPolyData>::New();
  int* trajCounts = (int*) malloc(sizeof(int) * params[1]);

  for(int o=0; o<params[1]; o++){//Loop over objects
	  for(int i=0, count=0; i<params[0]; i++){//Loop over frames
		  int tOffset = i * params[1] * dim;
		  int oOffset = tOffset + o * dim;
		  //int tOffset = i * params[1];
		  //int oOffset = tOffset + o;
		  double x = trajectories[oOffset];
		  double y = trajectories[oOffset + 1];
		  double z = i;
		  if(x>=0.0 && y>=0.0){
			trajPoints->InsertNextPoint(x, y, z);
			count++;
		  }
		  trajCounts[o] = count;
	  }
  }

  trajData->SetPoints(trajPoints);
	trajData->Allocate();
	for(int o=0, index=0; o<params[1]; o++){//Loop over objects
		for(int j=index; j<index+trajCounts[o]-1; j++){
			int connectivity[] = {j, j+1};
			trajData->InsertNextCell(VTK_LINE, 2, connectivity);
		}
		index += trajCounts[o];
	}
 
  // Setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> trajMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  trajMapper->SetInput(trajData);
   
  vtkSmartPointer<vtkActor> trajActor = vtkSmartPointer<vtkActor>::New();
  trajActor->SetMapper(trajMapper);

  //Create Box
  vtkSmartPointer<vtkPoints> boxPoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkPolyData> boxData = vtkSmartPointer<vtkPolyData>::New();
  buildBoxPolyData(boxPoints, boxData, params);
  vtkSmartPointer<vtkPolyDataMapper> boxMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  boxMapper->SetInput(boxData);
  vtkSmartPointer<vtkActor> boxActor = vtkSmartPointer<vtkActor>::New();
  boxActor->SetMapper(boxMapper);

  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkActor** actor = (vtkActor**) malloc(sizeof(vtkActor) * params[1]);
  
  for(int i=0; i<params[1]; i++){
	  actor[i] = vtkActor::New();
	  actor[i]->GetProperty()->SetColor(1.0, 0.0, 0.0);
	  actor[i]->SetMapper(mapper);
  }

  // Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(321, 164);
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
 
  // Add the actor to the scene
  for(int i=0; i<params[1]; i++){
	renderer->AddActor(actor[i]);
  }
  renderer->AddActor(boxActor);
  renderer->AddActor(trajActor);
  renderer->SetBackground(0.0 ,0.0, 0.0); // Background color white
 
  vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
  camera->SetFocalPoint(params[2]/2, params[3]/2, params[0]/2);

  renderer->SetActiveCamera(camera);

  // Render and interact
  renderWindow->Render();
 
  // Initialize must be called prior to creating timer events.
  renderWindowInteractor->Initialize();
 
  // Sign up to receive TimerEvent
  vtkSmartPointer<vtkTimerCallback2> cb = vtkSmartPointer<vtkTimerCallback2>::New();
  cb->Initialise(trajectories, params[0], params[1], dim, renderer, camera);
  //cb->Initialise(trajectories, params[0], params[1], params[4], renderer);
  cb->actor = actor;
  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);
 
  int timerId = renderWindowInteractor->CreateRepeatingTimer(100);
 
  // Start the interaction and timer
  renderWindowInteractor->Start();
 
  return EXIT_SUCCESS;
}

void buildBoxPolyData(vtkSmartPointer<vtkPoints> boxPoints, vtkSmartPointer<vtkPolyData> boxData, int* params){
  vtkIdType connectivity[2];
	
	boxPoints->InsertNextPoint(0, 0, 0);
  boxPoints->InsertNextPoint(0, 0, params[0]);
  boxPoints->InsertNextPoint(0, params[3], 0);
  boxPoints->InsertNextPoint(0, params[3], params[0]);
  boxPoints->InsertNextPoint(params[2], 0, 0);
  boxPoints->InsertNextPoint(params[2], 0, params[0]);
  boxPoints->InsertNextPoint(params[2], params[3], 0);
  boxPoints->InsertNextPoint(params[2], params[3], params[0]);

  boxData->SetPoints(boxPoints);
  boxData->Allocate();

	connectivity[0] = 0;
	connectivity[1] = 1;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[1] = 2;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[1] = 4;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[0] = 1;
	connectivity[1] = 3;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[1] = 5;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[0] = 2;
	connectivity[1] = 3;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[1] = 6;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[0] = 3;
	connectivity[1] = 7;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[0] = 4;
	connectivity[1] = 5;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[1] = 6;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[0] = 5;
	connectivity[1] = 7;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	connectivity[0] = 6;
	connectivity[1] = 7;
	boxData->InsertNextCell(VTK_LINE,2,connectivity);

	return;
}