
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
#include <vtkPolyLine.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include "read_trajs.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/math/special_functions/round.hpp>

using namespace boost;

normal_distribution<float> _dist(0.0f, 0.1f);
mt19937 rng;
variate_generator<mt19937, normal_distribution<float> > var_nor(rng, _dist);

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

	void Initialise(double* trajectories, int frames, int objects)
    {
	  this->trajectories = trajectories;
	  this->frames = frames;
	  this->objects=objects;
      return;
    }
 
    virtual void Execute(vtkObject *caller, unsigned long eventId,
                         void * vtkNotUsed(callData))
    {
		if (vtkCommand::TimerEvent == eventId)
      {
        ++this->TimerCount;
		if(this->TimerCount > 49){
			this->TimerCount = 0;
		}
      }
	  int offset = TimerCount * objects * 3;
	  double x = trajectories[offset];
	  double y = trajectories[offset + 1];
	  double z = TimerCount;
      actor->SetPosition(x, y, z);
      vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
      iren->GetRenderWindow()->Render();
    }
 
  private:
    int TimerCount;
	double* trajectories;
	int frames;
	int objects;
  public:
    vtkActor* actor;
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
	FILE *file;
	FILE **filePointer = &file;
	fopen_s(filePointer, _trajfilename_, "r");
	for(int i=0; i<numParams; i++){
		loadFloatParam(paramNames, numParams, params, "%s %f", MAX_LINE, 'i', file);
	}
	double* trajectories = (double*) malloc(sizeof(double) * params[0] * params[1] * 3);
	loadTrajectories(file, trajectories, params[0], params[1], MAX_LINE, 3);
	fclose(file);

  // Create a sphere
  vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(2.0);
  sphereSource->SetPhiResolution(20.0);
  sphereSource->SetThetaResolution(20.0);
  sphereSource->Update();

  //Create trajectory line
  vtkSmartPointer<vtkPoints> trajPoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkPolyLine> trajLine = vtkSmartPointer<vtkPolyLine>::New();
  trajLine->GetPointIds()->SetNumberOfIds(params[0]);

  for(int i=0; i<params[0]; i++){
	  int offset = i * params[1] * 3;
	  double x = trajectories[offset];
	  double y = trajectories[offset + 1];
	  double z = i;
	  trajPoints->InsertNextPoint(x, y, z);
	  trajLine->GetPointIds()->SetId(i, i);
  }

    // Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> trajCells = vtkSmartPointer<vtkCellArray>::New();
  trajCells->InsertNextCell(trajLine);
 
  // Create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> trajData = vtkSmartPointer<vtkPolyData>::New();
 
  // Add the points to the dataset
  trajData->SetPoints(trajPoints);
 
  // Add the lines to the dataset
  trajData->SetLines(trajCells);
 
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
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  actor->SetMapper(mapper);

  // Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(640, 480);
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
 
  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->AddActor(boxActor);
  renderer->AddActor(trajActor);
  renderer->SetBackground(0.0 ,0.0, 0.0); // Background color white
 
  // Render and interact
  renderWindow->Render();
 
  // Initialize must be called prior to creating timer events.
  renderWindowInteractor->Initialize();
 
  // Sign up to receive TimerEvent
  vtkSmartPointer<vtkTimerCallback2> cb = vtkSmartPointer<vtkTimerCallback2>::New();
  cb->Initialise(trajectories, params[0], params[1]);
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