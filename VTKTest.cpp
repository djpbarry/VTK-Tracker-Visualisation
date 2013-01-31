// VTKTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkElevationFilter.h"

/*int main( int argc, char* argv[])
{
	vtkRenderWindow *renwin = vtkRenderWindow::New();
	vtkRenderer *ren1 = vtkRenderer::New();
	renwin->AddRenderer(ren1);
	vtkConeSource *cone = vtkConeSource::New();
	cone->SetResolution(8);
	vtkPolyDataMapper *map = vtkPolyDataMapper::New();
	//vtkElevationFilter *elev = vtkElevationFilter::New();
	//elev->SetInput(cone->GetOutput());
	//elev->SetLowPoint( 0.,0.,-1.0);
	//elev->SetHighPoint(0.,0.,1.0);
	//map->SetInput(elev->GetPolyDataOutput());
	map->SetInput(cone->GetOutput());
	vtkActor *act = vtkActor::New();
	act->SetMapper(map);
	ren1->AddActor(act);
	//act->GetProperty()->SetOpacity(0.25);
	act->GetProperty()->SetAmbient(0.2);
	act->GetProperty()->SetDiffuse(0.6);
	act->GetProperty()->SetSpecular(0.5);
	act->GetProperty()->SetSpecularPower(25.0);
	for(double red = 1.0; red > 0.0; red -= 0.0001){
		act->GetProperty()->SetColor(red,0.0,0.0);
		renwin->Render();
	}
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renwin);
	iren->Initialize();
	iren->Start();
}
*/