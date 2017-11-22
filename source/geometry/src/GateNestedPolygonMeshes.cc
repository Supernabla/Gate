/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/
#include <memory>
#include <fstream>
#include <sstream>

#include <G4String.hh>
#include <G4Types.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisExtent.hh>
#include <G4VPhysicalVolume.hh>
#include <G4VSolid.hh>
#include <G4Colour.hh>

#include "cadmesh.hh"

#include "GateVVolume.hh"
#include "GateTools.hh"
#include "GateMessageManager.hh"
#include "GateDetectorConstruction.hh"  // <-- contains "theMaterialDatabase"
#include "GateMultiSensitiveDetector.hh"
#include "GateNestedPolygonMeshesMessenger.hh"

#include "GateNestedPolygonMeshes.hh"


namespace {

enum EOrganIDs
{
  OUTLINE = 3,
	SKIN_INNER = 4,
  BRONCHI = 1,
  LUNGE_RECHTS = 2,
  AORTA = 5,

  DELTOID_R = 15,
  BICEPS_BRACHII_R = 14,
  TRICEPS_BRACHII_R = 13,
  INFRASPINATUS_R = 12,
  TERES_MINOR_R = 11,
  TERES_MAJOR_R = 10,
  PECTORALIS_MAJOR_R = 9,
  STERNOCLEIDOMASTOI_R = 8,
  TRAPEZIUS_R = 7,
  RHOMBOID_MAJOR_R = 6,

	CLAVICLE_R = 67,
	STERNUM = 66,
	VERTEBRA_C1 = 65,
	VERTEBRA_C2 = 64,
	VERTEBRA_C3 = 63,
	VERTEBRA_C4 = 62,
	VERTEBRA_C5 = 61,
	VERTEBRA_C6 = 60,
	VERTEBRA_C7 = 59,
	VERTEBRA_T1 = 58,
	VERTEBRA_T2 = 57,
	VERTEBRA_T3 = 56,
	VERTEBRA_T4 = 55,
	VERTEBRA_T5 = 54,
	VERTEBRA_T6 = 53,
	VERTEBRA_T7 = 52,
	VERTEBRA_T8 = 51,
	VERTEBRA_T9 = 50,
	VERTEBRA_T10 = 49,
	VERTEBRA_T11 = 48,
	VERTEBRA_T12 = 47,
	RIB_1_L = 46,
  RIB_2_L = 45,
  RIB_3_L = 44,
  RIB_1_R = 43,
  RIB_2_R = 42,
  RIB_3_R = 41,
  CLAVICLE_L = 40,
  RIB_4_L = 39,
  RIB_5_L = 38,
  RIB_6_L = 37,
  RIB_7_L = 36,
  RIB_8_L = 35,
  RIB_9_L = 34,
  RIB_10_L = 33,
  RIB_11_L = 32,
  RIB_12_L = 31,
  RIB_4_R = 30,
  RIB_5_R = 29,
  RIB_6_R = 28,
  RIB_7_R = 27,
  RIB_8_R = 26,
  RIB_9_R = 25,
  RIB_10_R = 24,
  RIB_11_R = 23,
  RIB_12_R = 22,
  UPPER_TEETH = 69,
  LOWER_TEETH = 68,
  SKULL = 21,
  MANDIBLE = 20,
  HUMERUS_R = 19,
  SCAPULA_R = 18,
  HUMERUS_L = 17,
  SCAPULA_L = 16
};

}  // anonymous namespace


GateNestedPolygonMeshes::GateNestedPolygonMeshes(const G4String& itsName,
                                                 G4bool acceptsChildren,
                                                 G4int depth)
  : GateVVolume(itsName, acceptsChildren, depth),
    mPath(""), mUnitOfLength(mm), mRegionAttributeMapPath(""),
    mRegionAttributeMap(), pTopLevelSolid(nullptr), pTopLevelLogical(nullptr),
    pMessenger(new GateNestedPolygonMeshesMessenger(this))
{  
  // set default material name
  GateVVolume::SetMaterialName("Vacuum");
}

GateNestedPolygonMeshes::~GateNestedPolygonMeshes()
{
}

//----------------------------------------------------------------------------------------

G4LogicalVolume*
GateNestedPolygonMeshes::ConstructOwnSolidAndLogicalVolume(G4Material*,
                                                           G4bool flagUpdateOnly)
{
  // Update: Occurs when clock has changed, to trigger movement. Movement is implemented
  //         on the GateVVolume level, therefore we don't need to do anything.
  if (flagUpdateOnly == true && pTopLevelLogical)
    return pTopLevelLogical;
  
  // upon construction or rebuild: read region attributes from file
  ReadRegionAttributeMap();

  // set verbosity according to GATE geometry verbosity
  G4int verbosity = GateMessageManager::GetInstance()->GetMessageLevel("Geometry");
  cadmesh::SetVerbosityLevel(verbosity);
  
  // set parameters for the file reader
  cadmesh::PolygonMeshFileReader fileReader;
  fileReader.SetUnitOfLength(mUnitOfLength);

  // read meshes from .smesh file
  std::vector<cadmesh::PolygonMesh> meshCollection = fileReader.ReadMeshCollection(mPath);

  // assume human anatomy hierarchy when nesting meshes
  pTopLevelLogical = cadmesh::NestMeshes(meshCollection, mRegionAttributeMap,
                                         HumanAnatomyHierarchy());
  pTopLevelSolid = dynamic_cast<G4TessellatedSolid*>(pTopLevelLogical->GetSolid());
 
  return pTopLevelLogical;
}

//----------------------------------------------------------------------------------------

void GateNestedPolygonMeshes::DestroyOwnSolidAndLogicalVolume()
{  
  // delete top level logical
  if (pTopLevelSolid)
  {
    delete pTopLevelSolid;
    pTopLevelSolid = nullptr;
  }
  if (pTopLevelLogical)
  {
    delete pTopLevelLogical;
    pTopLevelLogical = nullptr;
  }
}

//----------------------------------------------------------------------------------------

G4double GateNestedPolygonMeshes::GetHalfDimension(size_t axis)
{
  if (pTopLevelSolid)
  {
    const G4VisExtent extent = pTopLevelSolid->GetExtent();
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
    } else
    {
      return 0.0;
    }
  }
  return 0.0;
}

//----------------------------------------------------------------------------------------

void GateNestedPolygonMeshes::PropagateSensitiveDetectorToChild(GateMultiSensitiveDetector* msd)
{
  // set sensitive detector for all daughters of the envelope box, i.e. all tetrahedra
  for (G4int i = 0; i < pTopLevelLogical->GetNoDaughters(); ++i)
  {
    pTopLevelLogical->GetDaughter(i)->GetLogicalVolume()->SetSensitiveDetector(msd);
  }
}

void GateNestedPolygonMeshes::PropagateGlobalSensitiveDetector()
{
  // in case no global SD was assigned to this volume
  if (GateVVolume::m_sensitiveDetector == nullptr)
    return;

  // otherwise check for phantom SD
  GatePhantomSD* phantomSD = \
    GateDetectorConstruction::GetGateDetectorConstruction()->GetPhantomSD();
  if (phantomSD)
  {
    // set for all tetrahedra
    for (G4int i = 0; i < pTopLevelLogical->GetNoDaughters(); ++i)
    {
      pTopLevelLogical->GetDaughter(i)->GetLogicalVolume()->SetSensitiveDetector(phantomSD);
    }
  }
}

void GateNestedPolygonMeshes::DescribeMyself(std::size_t level)
{
  G4cout << GateTools::Indent(level)
         << "From SMESH file: '" << mPath << "'" << Gateendl;
  G4cout << GateTools::Indent(level)
         << "Extent: " << pTopLevelSolid->GetExtent() << Gateendl;
  // TODO: more info
}

void GateNestedPolygonMeshes::ReadRegionAttributeMap()
{
  std::ifstream inputFileStream(mRegionAttributeMapPath);
  if (inputFileStream.is_open() == false)
  {
    GateError("Cannot open material map: '" << mRegionAttributeMapPath << "'");
    return;
  }

  // parse lines
  G4String line;
  while (std::getline(inputFileStream, line))
  {
    // skip comments
    if (line.front() == '#')
      continue;
    
    // columns
    G4int regionMarkerStart;
    G4int regionMarkerEnd;
    G4String materialName;
    G4double r, g, b;
    
    std::istringstream lineStream(line);

    // read columns
    if(lineStream >> regionMarkerStart >> regionMarkerEnd)
      if (lineStream >> materialName)
        lineStream >> r >> g >> b;

    if (lineStream.fail())
      GateError("Failed to read line '" << line << "' in region attribute map.");

    for (G4int rm = regionMarkerStart; rm <= regionMarkerEnd; ++rm)
    {
      cadmesh::RegionAttributes regionAttributes;
      regionAttributes.material = theMaterialDatabase.GetMaterial(materialName);
      regionAttributes.colour = G4Colour(r, g, b);
      mRegionAttributeMap[rm] = regionAttributes;
    }
  }
}

cadmesh::SurfaceNode GateNestedPolygonMeshes::HumanAnatomyHierarchy()
{
  cadmesh::SurfaceNode outline(OUTLINE);
  {
    cadmesh::SurfaceNode skinInner(SKIN_INNER);
    {
      skinInner.children.push_back(cadmesh::SurfaceNode(BRONCHI));
      skinInner.children.push_back(cadmesh::SurfaceNode(LUNGE_RECHTS));
      skinInner.children.push_back(cadmesh::SurfaceNode(AORTA));

      skinInner.children.push_back(cadmesh::SurfaceNode(DELTOID_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(BICEPS_BRACHII_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(TRICEPS_BRACHII_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(INFRASPINATUS_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(TERES_MINOR_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(TERES_MAJOR_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(PECTORALIS_MAJOR_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(STERNOCLEIDOMASTOI_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(TRAPEZIUS_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RHOMBOID_MAJOR_R));

      skinInner.children.push_back(cadmesh::SurfaceNode(CLAVICLE_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(STERNUM));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_C1));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_C2));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_C3));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_C4));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_C5));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_C6));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_C7));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T1));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T2));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T3));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T4));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T5));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T6));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T7));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T8));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T9));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T10));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T11));
      skinInner.children.push_back(cadmesh::SurfaceNode(VERTEBRA_T12));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_1_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_2_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_3_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_1_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_2_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_3_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(CLAVICLE_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_4_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_5_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_6_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_7_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_8_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_9_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_10_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_11_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_12_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_4_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_5_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_6_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_7_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_8_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_9_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_10_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_11_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(RIB_12_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(UPPER_TEETH));
      skinInner.children.push_back(cadmesh::SurfaceNode(LOWER_TEETH));
      skinInner.children.push_back(cadmesh::SurfaceNode(SKULL));
      skinInner.children.push_back(cadmesh::SurfaceNode(MANDIBLE));
      skinInner.children.push_back(cadmesh::SurfaceNode(HUMERUS_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(SCAPULA_R));
      skinInner.children.push_back(cadmesh::SurfaceNode(HUMERUS_L));
      skinInner.children.push_back(cadmesh::SurfaceNode(SCAPULA_L));
    }
    outline.children.push_back(skinInner);
  }

  return outline;
}
