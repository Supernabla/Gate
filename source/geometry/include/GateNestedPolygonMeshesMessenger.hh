/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/

#ifndef GATE_NESTED_POLYGON_MESHES__MESSENGER_HH
#define GATE_NESTED_POLYGON_MESHES__MESSENGER_HH

#include <G4String.hh>
#include <G4UIcommand.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

#include "GateVolumeMessenger.hh"

class GateNestedPolygonMeshes;


class GateNestedPolygonMeshesMessenger : public GateVolumeMessenger
{
  public:
    explicit GateNestedPolygonMeshesMessenger(GateNestedPolygonMeshes* itsCreator);
    ~GateNestedPolygonMeshesMessenger() final;

    void SetNewValue(G4UIcommand*, G4String) final;

  private:
    G4UIcmdWithAString* pSetPathToRegionAttributeMapCmd;
    G4UIcmdWithAString* pSetPathToSMESHFileCmd;
    G4UIcmdWithADoubleAndUnit* pSetUnitOfLengthCmd;
};

#endif  // GATE_NESTED_POLYGON_MESHES__MESSENGER_HH
