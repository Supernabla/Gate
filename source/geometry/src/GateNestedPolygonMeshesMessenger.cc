/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/
#include <G4String.hh>
#include <G4UIcommand.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

#include "GateVVolume.hh"
#include "GateNestedPolygonMeshes.hh"
#include "GateVolumeMessenger.hh"

#include "GateNestedPolygonMeshesMessenger.hh"


GateNestedPolygonMeshesMessenger::GateNestedPolygonMeshesMessenger(GateNestedPolygonMeshes* itsCreator)
  : GateVolumeMessenger(itsCreator)
{
  const G4String& dir = GateMessenger::GetDirectoryName();
  G4String pathCmdName = dir + "reader/setPathToSMESHFile";
  G4String regionAttributeMapCmdName = dir + "setPathToMaterialMap";
  G4String unitOfLengthCmdName = dir + "reader/setUnitOfLength";

  pSetPathToSMESHFileCmd = new G4UIcmdWithAString(pathCmdName, this);
  pSetPathToSMESHFileCmd->SetGuidance("Set path to SMESH file.");
  pSetPathToRegionAttributeMapCmd = new G4UIcmdWithAString(regionAttributeMapCmdName, this);
  pSetPathToRegionAttributeMapCmd->SetGuidance("Set path to material map (ASCII file).");
  pSetUnitOfLengthCmd = new G4UIcmdWithADoubleAndUnit(unitOfLengthCmdName, this);
  pSetUnitOfLengthCmd->SetGuidance("Unit of length to interpret the coordinates.");
}


GateNestedPolygonMeshesMessenger::~GateNestedPolygonMeshesMessenger()
{
  delete pSetPathToSMESHFileCmd;
  delete pSetPathToRegionAttributeMapCmd;
  delete pSetUnitOfLengthCmd;
}


void GateNestedPolygonMeshesMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  GateVVolume* creatorBase = GateVolumeMessenger::GetVolumeCreator();
  GateNestedPolygonMeshes* creator = dynamic_cast<GateNestedPolygonMeshes*>(creatorBase);
  
  if(command == pSetPathToSMESHFileCmd)
  {
    creator->SetPathToSMESHFile(newValue);
  }
  else if (command == pSetPathToRegionAttributeMapCmd)
  {
    creator->SetPathToRegionAttributeMap(newValue);
  }
  else if (command == pSetUnitOfLengthCmd)
  {
    creator->SetUnitOfLength(pSetUnitOfLengthCmd->GetNewDoubleValue(newValue));
  }
  else
  {
    GateVolumeMessenger::SetNewValue(command, newValue);
  }
}
