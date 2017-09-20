/*----------------------
  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/
#ifndef GATE_GDML_VOLUME_HH
#define GATE_GDML_VOLUME_HH

#include "G4String.hh"
#include "G4Types.hh"

#include "GateVolumeManager.hh"
#include "GateVVolume.hh"

class G4LogicalVolume;
class G4Material;
class G4VSolid;
class GateGDMLVolumeMessenger;

/// \class GateGDMLVolume
/// \brief Loads an arbitrary logical volume (+ child volumes) from a GDML file.
class GateGDMLVolume : public GateVVolume
{
  public:
    GateGDMLVolume(G4String const& itsName, G4bool acceptsChildren = true, G4int depth = 0);
    ~GateGDMLVolume() final;

    FCT_FOR_AUTO_CREATOR_VOLUME(GateGDMLVolume)

    virtual G4LogicalVolume* ConstructOwnSolidAndLogicalVolume(G4Material*, G4bool) final;
    virtual void DestroyOwnSolidAndLogicalVolume() final;
    virtual G4double GetHalfDimension(size_t) final;

    void SetPathToGDMLFile(const G4String& path) { mPathToGDMLFile = path; }
    void SetLogicalName(const G4String& name) { mLogicalName = name; }

  private:
    void DescribeMyself(size_t);

  private:
    G4String mPathToGDMLFile;
    G4String mLogicalName;

    G4VSolid* pTopLevelSolid;
    G4LogicalVolume* pTopLevelLogical;
    GateGDMLVolumeMessenger* pMessenger;
};

MAKE_AUTO_CREATOR_VOLUME(GDMLVolume,GateGDMLVolume)

#endif  // GATE_GDML_VOLUME_HH
