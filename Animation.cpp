
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
#include <vtkAxes.h>
#include "read_trajs.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/math/special_functions/round.hpp>

using namespace boost;

normal_distribution<float> _dist(0.0f, 0.1f);
mt19937 rng;
variate_generator<mt19937, normal_distribution<float> > var_nor(rng, _dist);
 
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
	  double x = trajectories[offset]/10.0;
	  double y = trajectories[offset + 1]/10.0;
	  double z = TimerCount/10.0;
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
	int params[2];
	char paramNames[2 * MAX_LINE];
	memcpy(&paramNames[0], FRAMES, MAX_LINE);
	memcpy(&paramNames[MAX_LINE], TRAJS, MAX_LINE);
	FILE *file;
	FILE **filePointer = &file;
	fopen_s(filePointer, _trajfilename_, "r");
	loadFloatParam(paramNames, 2, params, "%s %f", MAX_LINE, 'i', file);
	loadFloatParam(paramNames, 2, params, "%s %f", MAX_LINE, 'i', file);
	double* trajectories = (double*) malloc(sizeof(double) * params[0] * params[1] * 3);
	loadTrajectories(file, trajectories, params[0], params[1], MAX_LINE, 3);
	fclose(file);

  // Create a sphere
  vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(0.3);
  sphereSource->SetPhiResolution(20.0);
  sphereSource->SetThetaResolution(20.0);
  sphereSource->Update();

  //Create Axes
  vtkSmartPointer<vtkAxes> axes = vtkSmartPointer<vtkAxes>::New();
    
  // Create a mapper and actor for axes
  vtkSmartPointer<vtkPolyDataMapper> axesmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  axesmapper->SetInputConnection(axes->GetOutputPort());
  vtkSmartPointer<vtkActor> axesactor = vtkSmartPointer<vtkActor>::New();
  axesactor->GetProperty()->SetLineWidth(5.0);
  axesactor->SetScale(10.0);
  axesactor->SetMapper(axesmapper);

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
  renderer->AddActor(axesactor);
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
