/*----------------------
  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/
#ifndef GATE_NESTED_POLYGON_MESH_HH
#define GATE_NESTED_POLYGON_MESH_HH

#include <memory>

#include <G4String.hh>
#include <G4Types.hh>
#include <G4TessellatedSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>

#include "cadmesh.hh"

#include "GateVVolume.hh"
#include "GateVolumeManager.hh"

class GateNestedPolygonMeshesMessenger;
class GateMultiSensitiveDetector;


// Creates nested polygon meshes from a SMESH-File
class GateNestedPolygonMeshes : public GateVVolume
{
  public:
    GateNestedPolygonMeshes(const G4String& itsName,
                          G4bool acceptsChildren = false,
                          G4int depth = 0);
    ~GateNestedPolygonMeshes() final;

    FCT_FOR_AUTO_CREATOR_VOLUME(GateNestedPolygonMeshes)
    
    // implementation of GateVVolume's interface
    G4LogicalVolume* ConstructOwnSolidAndLogicalVolume(G4Material*, G4bool) final;
    void DestroyOwnSolidAndLogicalVolume() final;
    G4double GetHalfDimension(std::size_t) final;
    void PropagateSensitiveDetectorToChild(GateMultiSensitiveDetector*) final;
    void PropagateGlobalSensitiveDetector() final;

  public:
    // setters for the messenger
    void SetPathToSMESHFile(const G4String& path) { mPath = path; }
    void SetPathToRegionAttributeMap(const G4String& path) { mRegionAttributeMapPath = path; }
    void SetUnitOfLength(G4double unitOfLength) { mUnitOfLength = unitOfLength; }


  private:
  // implementation specifics
    void DescribeMyself(size_t);
    void ReadRegionAttributeMap();
    cadmesh::SurfaceNode HumanAnatomyHierarchy();

  private:
    G4String mPath;
    G4double mUnitOfLength;
    G4String mRegionAttributeMapPath;
    cadmesh::RegionAttributeMap mRegionAttributeMap;

    // box as mother of assembly imprint
    G4TessellatedSolid* pTopLevelSolid;
    G4LogicalVolume* pTopLevelLogical;

    std::unique_ptr<GateNestedPolygonMeshesMessenger> pMessenger;
};

MAKE_AUTO_CREATOR_VOLUME(NestedPolygonMeshes,GateNestedPolygonMeshes)

#endif  // GATE_NESTED_POLYGON_MESH_HH
