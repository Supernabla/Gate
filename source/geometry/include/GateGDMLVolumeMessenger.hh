/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/

#ifndef GATE_GDML_VOLUME_MESSENGER_HH
#define GATE_GDML_VOLUME_MESSENGER_HH

#include <G4String.hh>

#include "GateVolumeMessenger.hh"

class G4UIcommand;
class G4UIcmdWithAString;
class GateGDMLVolume;

class GateGDMLVolumeMessenger : public GateVolumeMessenger
{
  public:
    GateGDMLVolumeMessenger(GateGDMLVolume* itsCreator);
    ~GateGDMLVolumeMessenger() final;

    void SetNewValue(G4UIcommand*, G4String) final;

  private:
    G4UIcmdWithAString* pSetPathToGDMLFileCmd;
    G4UIcmdWithAString* pSetLogicalNameCmd;    
};

#endif  // GATE_GDML_VOLUME_MESSENGER_HH
