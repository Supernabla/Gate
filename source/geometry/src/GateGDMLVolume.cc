/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/

#include <G4String.hh>
#include <G4GDMLParser.hh>
#include <G4VisExtent.hh>

#include "GateTools.hh"
#include "GateMessageManager.hh"
#include "GateVVolume.hh"
#include "GateGDMLVolumeMessenger.hh"

#include "GateGDMLVolume.hh"

//----------------------------------------------------------------------------------------

GateGDMLVolume::GateGDMLVolume(const G4String& itsName,
                               G4bool itsFlagAcceptChildren,
                               G4int depth)
  : GateVVolume(itsName, itsFlagAcceptChildren, depth),
    mPathToGDMLFile(""), mLogicalName(""), pTopLevelSolid(nullptr),
    pTopLevelLogical(nullptr), pMessenger(nullptr)
{ 
  // Set default material name. This should have no effect, cause we intend
  // to read the material from the GDML file.
  SetMaterialName("Vacuum");

  pMessenger = new GateGDMLVolumeMessenger(this);
}


GateGDMLVolume::~GateGDMLVolume()
{
  delete pMessenger;
}

//----------------------------------------------------------------------------------------

G4LogicalVolume* GateGDMLVolume::ConstructOwnSolidAndLogicalVolume(G4Material*,
                                                                   G4bool flagUpdateOnly)
{
  if (flagUpdateOnly == false || pTopLevelLogical == nullptr)
  {
    // missing info --> exit with error
    if (mPathToGDMLFile == "")
    {
      GateError("Cannot parse GDML file: path has not been set.");
    }
    else if (mLogicalName == "")
    {
      GateError("Cannot load volume from GDML file: name has not been set.");
    }

    // Get logical volume (+ child volumes), material(s) and solid(s)
    // are defined in the GDML file.
    G4GDMLParser parser;
    parser.Read(mPathToGDMLFile);
    pTopLevelLogical = parser.GetVolume(mLogicalName);
  }
  else
  {
    // TODO: implement "update only". What would that be?
  }

  return pTopLevelLogical;
}


void GateGDMLVolume::DestroyOwnSolidAndLogicalVolume()
{
  if (pTopLevelLogical)
  {
    delete pTopLevelLogical;
    pTopLevelLogical = nullptr;
  }
}

//----------------------------------------------------------------------------------------

G4double GateGDMLVolume::GetHalfDimension(size_t axis)
{
  if (pTopLevelLogical)
  {
    const G4VisExtent extent = pTopLevelLogical->GetSolid()->GetExtent();
    if (axis == 0)
    {
      return 0.5 * (extent.GetXmax() - extent.GetXmin());
    }
    else if (axis == 1)
    {
      return 0.5 * (extent.GetYmax() - extent.GetYmin());
    }
    else if (axis == 2)
    {
      return 0.5 * (extent.GetZmax() - extent.GetZmin());
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------

void GateGDMLVolume::DescribeMyself(size_t level)
{
  G4cout << GateTools::Indent(level) << "From GDML File:                     ";
  G4cout << mPathToGDMLFile << Gateendl;
  G4cout << GateTools::Indent(level) << "Logical Volume Name (in GDML File): ";
  G4cout << mLogicalName << Gateendl;
  G4cout << GateTools::Indent(level) << "Geometry Type:                      ";
  G4cout << pTopLevelLogical->GetSolid()->GetEntityType() << Gateendl;
}
