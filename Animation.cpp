
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
#include <boost/math/constants/constants.hpp>
#include <boost/random.hpp>

using namespace boost;
int dim = 2;
void buildBoxPolyData(vtkSmartPointer<vtkPoints> boxPoints, vtkSmartPointer<vtkPolyData> boxData, int* params);

class vtkTimerCallback2 : public vtkCommand {
public:

    static vtkTimerCallback2 *New() {
        vtkTimerCallback2 *cb = new vtkTimerCallback2;
        cb->TimerCount = 0;
        return cb;
    }

    void Initialise(double* trajectories, int frames, int objects, int dim, vtkRenderer* renderer, vtkCamera* camera, int* counts, float scale) {
        this->trajectories = trajectories;
        this->frames = frames;
        this->objects = objects;
        //this->dim=dim;
        this->renderer = renderer;
        this->camera = camera;
        this->counts = counts;
		this->scale = scale;
        return;
    }

    virtual void Execute(vtkObject *caller, unsigned long eventId,
            void * vtkNotUsed(callData)) {
        if (vtkCommand::TimerEvent == eventId) {
            ++this->TimerCount;
            if (this->TimerCount > this->frames - 1) {
				return;
            }
        }
        //Set virus position

        int tOffset = (TimerCount - 1) * objects * dim;
		float x0 = scale/2.0f;
		float y0=scale/2.0f;
        double x = trajectories[tOffset] / 7;
        double y = trajectories[tOffset + 1] / 7;
        actor[0]->VisibilityOn();
        actor[0]->SetPosition(x, y, 0.0);
        /*printf("\rStep %d of %d...",TimerCount, frames);*/
        for (int j = 0, actorIndex=0; j < TimerCount; j++) {
            for (int i = 1; i < counts[j]; i++) {
                int tOffset = j * objects * dim + 2*i;
                double x = trajectories[tOffset] / 7;
                double y = trajectories[tOffset + 1] / 7;
                //if(x > 0.0 && y > 0.0){
                actor[actorIndex+i]->VisibilityOn();
                actor[actorIndex+i]->SetPosition(x, y, 0.0);
                //} else {
                // actor[i]->VisibilityOff();
                //}
            }
			actorIndex+=counts[j]-1;
        }
		//float theta = 4.0f * math::constants::pi<float>() * TimerCount/frames;
		//float cameraX = x0 + scale * cos(theta);
  //      float cameraY = y0 + scale * sin(theta);
  //      float cameraZ = scale*(0.6f + 0.4f*sin(theta));

		printf("%d: %f, %f\n",TimerCount, x, y);

        vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
        iren->GetRenderWindow()->Render();

         vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
         windowToImageFilter->SetInput(iren->GetRenderWindow());
        windowToImageFilter->SetMagnification(1); //set the resolution of the output image (3 times the current resolution of vtk render window)
        windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
        windowToImageFilter->Update();

        std::string savefilename("C:/users/barry05/Desktop/VTKTrackerVis/");
              savefilename.append(boost::lexical_cast<std::string > (TimerCount));
              savefilename.append(".png");

         vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
         writer->SetFileName(savefilename.data());
         writer->SetInputConnection(windowToImageFilter->GetOutputPort());
         writer->Write();
    }

private:
    int TimerCount;
    double* trajectories;
    int* counts;
    int frames;
    int objects;
    vtkRenderer* renderer;
    vtkCamera* camera;
	float scale;
public:
    vtkActor** actor;
};

int main(int, char* []) {
    int numParams = 3;
    int* params = (int*) malloc(sizeof (int) * numParams);
    char* paramNames = (char*) malloc(numParams * MAX_LINE * sizeof (char));
    memcpy(&paramNames[0], FRAMES, MAX_LINE);
    memcpy(&paramNames[1 * MAX_LINE], WIDTH, MAX_LINE);
    memcpy(&paramNames[2 * MAX_LINE], HEIGHT, MAX_LINE);
    FILE *file;
    FILE **filePointer = &file;
    fopen_s(filePointer, _trajfilename_, "r");
    for (int i = 0; i < numParams; i++) {
        loadFloatParam(paramNames, numParams, params, "%s %f", MAX_LINE, 'i', file);
    }
    double* trajectories = (double*) malloc(sizeof (double) * params[0] * MAX_FILS * dim);
    int* counts = (int*) malloc(sizeof (int) * params[0]);
    loadTrajectories(file, trajectories, params[0], MAX_FILS, MAX_LINE, dim, counts);
    fclose(file);

    int sumcounts = 1;
    for(int i=0;i<params[0];i++){
    	sumcounts+=counts[i]-1;
    }

    // Create a sphere
    vtkSmartPointer<vtkSphereSource> sphereSource1 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource1->SetRadius(35.0);
    sphereSource1->SetPhiResolution(20.0);
    sphereSource1->SetThetaResolution(20.0);
    sphereSource1->Update();

    vtkSmartPointer<vtkSphereSource> sphereSource2 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource2->SetRadius(1.0);
    sphereSource2->SetPhiResolution(20.0);
    sphereSource2->SetThetaResolution(20.0);
    sphereSource2->Update();

    //Create Box
    //vtkSmartPointer<vtkPoints> boxPoints = vtkSmartPointer<vtkPoints>::New();
    //vtkSmartPointer<vtkPolyData> boxData = vtkSmartPointer<vtkPolyData>::New();
    //buildBoxPolyData(boxPoints, boxData, params);
    //vtkSmartPointer<vtkPolyDataMapper> boxMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //boxMapper->SetInput(boxData);
    //vtkSmartPointer<vtkActor> boxActor = vtkSmartPointer<vtkActor>::New();
    //boxActor->SetMapper(boxMapper);

    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper1->SetInputConnection(sphereSource1->GetOutputPort());
    mapper2->SetInputConnection(sphereSource2->GetOutputPort());
    vtkActor** actor = (vtkActor**) malloc(sizeof (vtkActor) * sumcounts);

    actor[0] = vtkActor::New();
    actor[0]->GetProperty()->SetColor(0.0, 1.0, 0.0);
    actor[0]->SetMapper(mapper1);

    for (int i = 1; i < sumcounts; i++) {
        actor[i] = vtkActor::New();
        actor[i]->GetProperty()->SetColor(1.0, 0.0, 0.0);
        actor[i]->SetMapper(mapper2);
    }

    // Create a renderer, render window, and interactor
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(1000, 1000);
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actor to the scene
    for (int i = 0; i < sumcounts; i++) {
        renderer->AddActor(actor[i]);
    }
    //renderer->AddActor(boxActor);
    //renderer->AddActor(trajActor);
    renderer->SetBackground(0.0, 0.0, 0.0); // Background color white

    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	camera->SetPosition(params[1]/2.0f, params[2]/2.0f, 1500.0f);
	camera->SetFocalPoint(params[1]/2.0f, params[2]/2.0f, 0.0);
	//camera->SetViewUp(0.0,0.0,1.0);

    renderer->SetActiveCamera(camera);

    // Render and interact
    renderWindow->Render();

    // Initialize must be called prior to creating timer events.
    renderWindowInteractor->Initialize();

    // Sign up to receive TimerEvent
    vtkSmartPointer<vtkTimerCallback2> cb = vtkSmartPointer<vtkTimerCallback2>::New();
    cb->Initialise(trajectories, params[0], MAX_FILS, dim, renderer, camera, counts, params[1]);
    //cb->Initialise(trajectories, params[0], params[1], params[4], renderer);
    cb->actor = actor;
    renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);

    int timerId = renderWindowInteractor->CreateRepeatingTimer(10);

    // Start the interaction and timer
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

void buildBoxPolyData(vtkSmartPointer<vtkPoints> boxPoints, vtkSmartPointer<vtkPolyData> boxData, int* params) {
    vtkIdType connectivity[2];

    boxPoints->InsertNextPoint(0, 0, 0);
    boxPoints->InsertNextPoint(params[1], 0, 0);
    boxPoints->InsertNextPoint(params[1], params[2], 0);
    boxPoints->InsertNextPoint(0, params[2], 0);

    boxData->SetPoints(boxPoints);
    boxData->Allocate();

    connectivity[0] = 0;
    connectivity[1] = 1;
    boxData->InsertNextCell(VTK_LINE, 2, connectivity);

    connectivity[1] = 3;
    boxData->InsertNextCell(VTK_LINE, 2, connectivity);

    connectivity[0] = 1;
    connectivity[1] = 2;
    boxData->InsertNextCell(VTK_LINE, 2, connectivity);

    connectivity[0] = 2;
    connectivity[1] = 3;
    boxData->InsertNextCell(VTK_LINE, 2, connectivity);

    return;
}