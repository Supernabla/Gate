/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/
#include <G4String.hh>
#include <G4UIcommand.hh>
#include <G4UIcmdWithAString.hh>

#include "GateVVolume.hh"
#include "GateGDMLVolume.hh"
#include "GateVolumeMessenger.hh"

#include "GateGDMLVolumeMessenger.hh"

GateGDMLVolumeMessenger::GateGDMLVolumeMessenger(GateGDMLVolume* itsCreator)
  : GateVolumeMessenger(itsCreator)
{
  G4String dir = GetDirectoryName();
  G4String pathCmdName = dir + "setPathToGDMLFile";
  G4String logicalNameCmdName = dir + "setLogicalVolumeName";

  pSetPathToGDMLFileCmd = new G4UIcmdWithAString(pathCmdName, this);
  pSetPathToGDMLFileCmd->SetGuidance("Set path to GDML file");
  pSetLogicalNameCmd = new G4UIcmdWithAString(logicalNameCmdName, this);
  pSetLogicalNameCmd->SetGuidance("Set name of logical volume in GDML file");
}


GateGDMLVolumeMessenger::~GateGDMLVolumeMessenger()
{
  delete pSetPathToGDMLFileCmd;
  delete pSetLogicalNameCmd;
}


void GateGDMLVolumeMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  GateVVolume* creatorBase = GateVolumeMessenger::GetVolumeCreator();
  GateGDMLVolume* creator = dynamic_cast<GateGDMLVolume*>(creatorBase);

  if(command == pSetPathToGDMLFileCmd)
  {
    creator->SetPathToGDMLFile(newValue);
  }
  else if (command == pSetLogicalNameCmd)
  {
    creator->SetLogicalName(newValue);
  }
  else
  {
    GateVolumeMessenger::SetNewValue(command, newValue);
  }
}
