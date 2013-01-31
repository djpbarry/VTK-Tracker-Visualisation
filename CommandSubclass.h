
#ifndef _command_subclass_h_
#define _command_subclass_h_

class CommandSubclass : public vtkCommand
{
  public:
  vtkTypeMacro(CommandSubclass, vtkCommand);
  static CommandSubclass *New()
  {
    return new CommandSubclass;
  }
 
  void Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), 
                       void *vtkNotUsed(callData))
  {
    std::cout << "timer callback" << std::endl;
 
    vtkRenderWindowInteractor *iren = static_cast<vtkRenderWindowInteractor*>(caller);
 
    this->ProgrammableFilter->Modified();
 
    iren->Render();
 
  }
 
  vtkSmartPointer<vtkProgrammableFilter> ProgrammableFilter;
 
};

#endif