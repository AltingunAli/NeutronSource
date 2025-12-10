//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"

#include "DetectorMessenger.hh"

#include "G4Box.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SolidStore.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4UnitsTable.hh"
#include "G4VisAttributes.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction() {
  fNbOfAbsor = 1;
  fAbsorThickness[0] = 0 * mm; // dummy, for initialization
  fAbsorThickness[1] = 1 * mm;
  fAbsorSizeYZ = 1 * mm;
  fAbsorSizeY = 1 * mm;
  fAbsorSizeZ = 1 * mm;

  fAbsorRadius = 15 * mm;
  fAbsorLength = 60 * mm;
  fContainThickness = 2.4 * mm;
  DefineMaterials();
  SetAbsorMaterial("BeO");
  SetContainMaterial("Stainless-Steel");
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction() { delete fDetectorMessenger; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *DetectorConstruction::Construct() {
  return ConstructVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials() {
  G4int ncomponents, natoms;

  G4Element *Be = new G4Element("Beryllium", "Be", 4., 9.01 * g / mole);
  G4Element *N = new G4Element("Nitrogen", "N", 7., 14.01 * g / mole);
  G4Element *O = new G4Element("Oxygen", "O", 8., 16.00 * g / mole);
  G4Element *Cr = new G4Element("Chromium", "Cr", 24., 51.99 * g / mole);
  G4Element *Fe = new G4Element("Iron", "Fe", 26., 55.84 * g / mole);
  G4Element *Ni = new G4Element("Nickel", "Ni", 28., 58.69 * g / mole);

  G4Material *BeO = new G4Material("BeO", 3.05 * g / cm3, ncomponents = 2);
  BeO->AddElement(Be, natoms = 1);
  BeO->AddElement(O, natoms = 1);

  G4Material *inox =
      new G4Material("Stainless-Steel", 8 * g / cm3, ncomponents = 3);
  inox->AddElement(Fe, 74 * perCent);
  inox->AddElement(Cr, 18 * perCent);
  inox->AddElement(Ni, 8 * perCent);

  G4Material *Air = new G4Material("Air", 1.290 * mg / cm3, ncomponents = 2);
  Air->AddElement(N, 70. * perCent);
  Air->AddElement(O, 30. * perCent);

  fWorldMaterial = Air;

  /********************************************************************************/
  // Added by Altingun
  //********************************************************************************/
  G4NistManager *nistManager = G4NistManager::Instance();

  G4double A;            // atomic mass
  G4double Z;            // atomic number
  G4double d;            // density
  G4double fractionmass; // mass fraction

  // Create the AlMg3 material
  // The composition is roughly 3% Magnesium by weight
  // Average density of aluminum alloys is around 2.7 g/cm³
  A = 26.98 * g / mole;
  G4Element *Al = new G4Element("TS_Aluminium_Metal", "Al", Z = 13., A);
  // G4Element *Al = new G4Element("Aluminum", "Al", Z = 13., A);
  A = 24.305 * g / mole;
  G4Element *Mg = new G4Element("Magnesium", "Mg", Z = 12., A);

  d = 2.67 * g / cm3; // g/cm^3
  G4Material *AlMg3 = new G4Material("AlMg3", d, 2);
  AlMg3->AddElement(Al, fractionmass = 0.97); // 97% Aluminum
  AlMg3->AddElement(Mg, fractionmass = 0.03); // 3% Magnesium

  A = 54.94 * g / mole;
  G4Element *Mn = new G4Element("Manganese", "Mn", Z = 25., A);
  A = 28.09 * g / mole;
  G4Element *Si = new G4Element("Silicon", "Si", Z = 14., A);
  // A = 52.00 * g / mole;
  // G4Element *Cr = new G4Element("Chromium", "Cr", Z = 24., A);
  // A = 58.70 * g / mole;
  // G4Element *Ni = new G4Element("Nickel", "Ni", Z = 28., A);
  A = 55.84 * g / mole;
  G4Element *TS_Fe = new G4Element("TS_Iron_Metal", "Fe", Z = 26., A);

  // Aluminum as material

  // Then, build the material with a given density
  d = 2.70 * g / cm3; // Density of Aluminium in g/cm^3
  G4Material *Alu = new G4Material("Alu", d, 1); // 1 component
  Alu->AddElement(Al, 1.0); // Add the Aluminium element with mass fraction 1.0

  // Stainless steel (Medical Physics, Vol 25, No 10, Oct 1998)
  d = 8.02 * g / cm3;
  G4Material *SSteel = new G4Material("SSteel", d, 5);
  SSteel->AddElement(Mn, fractionmass = 0.02);
  SSteel->AddElement(Si, fractionmass = 0.01);
  SSteel->AddElement(Cr, fractionmass = 0.19);
  SSteel->AddElement(Ni, fractionmass = 0.10);
  SSteel->AddElement(TS_Fe, fractionmass = 0.68);

  // silicon as materials

  matSilicon = nistManager->FindOrBuildMaterial("G4_Si");

  G4int iz, n; // iz=nb of protons  in an isotope;
  // n=nb of nucleons in an isotope;

  G4String name, symbol;
  G4double abundance;

  // Create new isotopes
  G4Isotope *iso_B10 =
      new G4Isotope("Boron-10", iz = 5, n = 10, A = 10.01 * g / mole);
  G4Isotope *iso_B11 =
      new G4Isotope("Boron-11", iz = 5, n = 11, A = 11.009 * g / mole);

  // Create the Boron element with specified enrichment
  G4Element *B = new G4Element("enriched Boron", symbol = "B", natoms = 2);

  // Adjust the enrichment to your needs. For 50% enriched:
  B->AddIsotope(iso_B10, 0.5);
  B->AddIsotope(iso_B11, 0.5);

  // Create the Carbon element (natural abundance)
  G4Isotope *iso_C12 =
      new G4Isotope("C12", iz = 6, n = 12, A = 12.011 * g / mole);
  G4Element *C = new G4Element("Carbon", symbol = "C", natoms = 1);
  C->AddIsotope(iso_C12, 1.0);

  // Create the B4C material
  G4Material *B4C_enriched =
      new G4Material("B4C_enriched", 2.52 * g / cm3, 2); // Bulk density of B4C
  B4C_enriched->AddElement(B, 4);
  B4C_enriched->AddElement(C, 1);

  //********************************************************************************/
  //********************************************************************************/

  /// G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material *DetectorConstruction::MaterialWithSingleIsotope(G4String name,
                                                            G4String symbol,
                                                            G4double density,
                                                            G4int Z, G4int A) {
  // define a material from an isotope
  //
  G4int ncomponents;
  G4double abundance, massfraction;

  G4Isotope *isotope = new G4Isotope(symbol, Z, A);

  G4Element *element = new G4Element(name, symbol, ncomponents = 1);
  element->AddIsotope(isotope, abundance = 100. * perCent);

  G4Material *material = new G4Material(name, density, ncomponents = 1);
  material->AddElement(element, massfraction = 100. * perCent);

  return material;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *DetectorConstruction::ConstructVolumes() {
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  // ################################################################################//
  // World
  // ################################################################################//
  fWorldSizeX = 1. * m;
  fWorldSizeY = 1. * m;
  fWorldSizeZ = 1. * m;

  G4Box *sWorld = new G4Box("World", // name
                            0.5 * fWorldSizeX, 0.5 * fWorldSizeY,
                            0.5 * fWorldSizeZ); // dimensions

  G4LogicalVolume *lWorld = new G4LogicalVolume(sWorld,         // shape
                                                fWorldMaterial, // material
                                                "World");       // name

  fPWorld = new G4PVPlacement(0,               // no rotation
                              G4ThreeVector(), // at (0,0,0)
                              lWorld,          // logical volume
                              "World",         // name
                              0,               // mother volume
                              false,           // no boolean operation
                              0);              // copy number

  lWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  // ################################################################################//
  // Creating the silicon box around the monitor to measure energy deposition
  // This is a hollow box located on the edge of the world.
  // ################################################################################//
  //
  // Silicon Box - This is a silicon to calculate dose. It is a hollow box
  // located on the edge of the world.
  // if (CreateSiliconSlabs) {
  // G4double skin = 1.0 * mm;
  // G4double boxsizeX = 0.5 * fWorldSizeX * .99;
  // G4double boxsizeY = 0.5 * fWorldSizeY * .99;
  // G4double boxsizeZ = 0.5 * fWorldSizeZ * .99;

  // auto SolidExterior = new G4Box("SolidExterior", boxsizeX, boxsizeY,
  // boxsizeZ);

  // auto SolidInterior = new G4Box("SolidInterior", boxsizeX - skin,
  //                                boxsizeY - skin, boxsizeZ - skin);
  // auto SiBox = new G4SubtractionSolid("SiBox", SolidExterior,
  // SolidInterior); auto logSiBox = new G4LogicalVolume(SiBox, matSilicon,
  // "logSiBox"); auto physSiBox =
  //     new G4PVPlacement(0, {0, 0, 0}, logSiBox, "physSiBox", lWorld, false,
  //     0);

  // G4VisAttributes *logicSiBoxAtt = new G4VisAttributes(G4Color(0.5, 0.5,
  // 0.5)); logSiBox->SetVisAttributes(logicSiBoxAtt);
  // }
  // ################################################################################//

  // ################################################################################//
  // Creating the silicon slabs around the monitor to measure energy deposition
  // Here 4 different 2 mm thick slabs are created on 4 sides of the monitor.
  // CreateSiliconSlabs parameter is set from macro file to enable/disable the
  // geometry.
  // ################################################################################//
  if (CreateSiliconSlabs) {
    G4double boxsizeX = 2.0 * mm;
    G4double boxsizeY = 0.5 * fWorldSizeY * .99;
    G4double boxsizeZ = 0.5 * fWorldSizeZ * .99;

    auto SolidSiBox_1 =
        new G4Box("SolidSiBox_1", boxsizeX, boxsizeY, boxsizeZ); // along X

    G4LogicalVolume *logsiSlab_AlongX =
        new G4LogicalVolume(SolidSiBox_1,        // shape
                            matSilicon,          // material
                            "logsiSlab_AlongX"); // name
    auto physiSlab_AlongX = new G4PVPlacement(
        0, {-(fWorldSizeX * .99 / 2. + 1.6 * mm), 0, 0}, logsiSlab_AlongX,
        "physiSlab_AlongX", lWorld, false, 0);

    // ******************************************************************************//
    auto SolidSiBox_2 =
        new G4Box("SolidSiBox_2", boxsizeZ, boxsizeY, boxsizeX); // along Z
    G4LogicalVolume *logsiSlab_AlongZ_1 =
        new G4LogicalVolume(SolidSiBox_2,          // shape
                            matSilicon,            // material
                            "logsiSlab_AlongZ_1"); // name
    auto physiSlab_AlongZ_1 = new G4PVPlacement(
        0, {0., 0, -(fWorldSizeX * .99 / 2. + 1.6 * mm)}, logsiSlab_AlongZ_1,
        "physiSlab_AlongZ_1", lWorld, false, 0);

    G4LogicalVolume *logsiSlab_AlongZ_2 =
        new G4LogicalVolume(SolidSiBox_2,          // shape
                            matSilicon,            // material
                            "logsiSlab_AlongZ_2"); // name
    auto physiSlab_AlongZ_2 = new G4PVPlacement(
        0, {0., 0, (fWorldSizeX * .99 / 2. + 1.6 * mm)}, logsiSlab_AlongZ_2,
        "physiSlab_AlongZ_2", lWorld, false, 0);
    //*******************************************************************************//

    auto SolidSiBox_3 =
        new G4Box("SolidSiBox_3", boxsizeZ, boxsizeX, boxsizeY); // along Y

    new G4Box("SolidSiBox_3", boxsizeZ, boxsizeY, boxsizeX); // along Y
    G4LogicalVolume *logsiSlab_AlongY_1 =
        new G4LogicalVolume(SolidSiBox_3,          // shape
                            matSilicon,            // material
                            "logsiSlab_AlongY_1"); // name
    auto physiSlab_AlongY_1 = new G4PVPlacement(
        0, {0., -(fWorldSizeX * .99 / 2. + 1.6 * mm), 0.}, logsiSlab_AlongY_1,
        "physiSlab_AlongY_1", lWorld, false, 0);

    G4LogicalVolume *logsiSlab_AlongY_2 =
        new G4LogicalVolume(SolidSiBox_3,          // shape
                            matSilicon,            // material
                            "logsiSlab_AlongY_2"); // name
    auto physiSlab_AlongY_2 = new G4PVPlacement(
        0, {0., (fWorldSizeX * .99 / 2. + 1.6 * mm), 0.}, logsiSlab_AlongY_2,
        "physiSlab_AlongY_2", lWorld, false, 0);
  }
  // ################################################################################//
  // Monitor geometry: multiple slabs
  // ################################################################################//
  fXfront[0] = -0.5 * fAbsorSizeX;

  std::cout << "fAbsorSizeX: " << fAbsorSizeX << std::endl;
  std::cout << "fAbsorSizeY: " << fAbsorSizeY << std::endl;
  std::cout << "fAbsorSizeZ: " << fAbsorSizeZ << std::endl;
  //
  for (G4int k = 1; k <= fNbOfAbsor; k++) {
    G4Material *material = fAbsorMaterial_Slab[k];
    G4String matname = material->GetName();

    // G4Box* solidAbsor =
    //   new G4Box(matname, fAbsorThickness[k] / 2, fAbsorSizeYZ / 2,
    //   fAbsorSizeYZ / 2);
    G4Box *solidAbsor = new G4Box(matname, fAbsorThickness[k] / 2,
                                  fAbsorSizeY / 2, fAbsorSizeZ / 2);
    G4LogicalVolume *logicAbsor = new G4LogicalVolume(solidAbsor, // solid
                                                      material,   // material
                                                      matname);   // name

    // fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1];

    //********************************************************************************/
    // -The first slab is AlMg3(Aluminum-Magnesium alloy) as the front window
    // material.

    // -The second slab(s) is (are) 100 micron aluminum plate coated
    // with 1 micron of B4C_enriched (Boron Carbide) as the neutron converter.
    // B4C has good thermal neutron absorption performance and it is coated on
    // the AlMg3 slab.

    // -The third slab is 40 micron thick Stainless Steel as mesh.

    // -The fourth slab is 500 micron thick AlMg3 as anode plate.

    // -Finally, the fifth slab is 200 micron thick AlMg3 as the back window.

    //        *         *         *         *         *
    //        *         *         *         *         *
    //        *         *         *         *         *
    //        *         *         *         *         *
    //        *         *         *         *         *
    //        * <-1mm-> *<-1.7mm->*<-100um->* <-1mm-> *
    //        *         *         *         *         *
    //        *         *         *         *         *
    //        *         *         *         *         *
    //        *         *         *         *         *
    //        *         *         *         *         *

    //      AlMg3     Al-B4C    SSteel     AlMg3      AlMg3
    //     (front)  (converter) (mesh)    (anode)     (back)
    //     (200um)  (100um+1um) (40um)    (500 um)   (200 um)

    // Check the macro file for the material and thickness definitions.
    //********************************************************************************/
    // The distance between the slabs. This is hardcoded for now. In the future,
    // we will implement this in a more flexible way. Or we will use gdml for
    // complex geometries.
    //--------------------------------------------------------------------------------/
    // if (k == 1 || k == 3) {
    //   fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1];
    // } else {
    //   fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1] + 1. * mm;
    // }
    //--------------------------------------------------------------------------------/
    switch (k) {
    case 1:
      fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1];
      break;
    case 2:
      fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1] + 1. * mm;
      break;
    case 3:
      fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1];
      break;
    case 4:
      fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1] + 1.7 * mm;
      break;
    case 5:
      fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1] + 0.1 * mm;
      break;
    case 6:
      fXfront[k] = fXfront[k - 1] + fAbsorThickness[k - 1] + 1. * mm;
      break;
    }

    //********************************************************************************/
    // Create the physical volume for the slabs
    //********************************************************************************/
    G4double xcenter = fXfront[k] + 0.5 * fAbsorThickness[k];
    G4ThreeVector position = G4ThreeVector(xcenter, 0., 0.);

    new G4PVPlacement(0,          // no rotation
                      position,   // position
                      logicAbsor, // logical volume
                      matname,    // name
                      lWorld,     // mother
                      false,      // no boulean operat
                      k);         // copy number

    // Set visualization attributes
    G4VisAttributes *AbsorAtt;
    if (matname == "AlMg3") {
      AbsorAtt = new G4VisAttributes(G4Color(0.5, 0.5, 0.5));
      logicAbsor->SetVisAttributes(AbsorAtt);
    } else if (matname == "B4C_enriched") {
      AbsorAtt = new G4VisAttributes(G4Color(0.0, 0.0, 0.0, 0.4));
      logicAbsor->SetVisAttributes(AbsorAtt);
    } else if (matname == "SSteel") {
      AbsorAtt = new G4VisAttributes(G4Color(0.5, 0.5, 0.6));
      logicAbsor->SetVisAttributes(AbsorAtt);
    }
  }
  // ###############################################################################//
  // PrintParameters();

  // always return the root volume
  //
  return fPWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// void DetectorConstruction::PrintParameters() {
//   G4cout << "\n The Absorber  is a cylinder of " << fAbsorMaterial->GetName()
//          << "  radius = " << G4BestUnit(fAbsorRadius, "Length")
//          << "  length = " << G4BestUnit(fAbsorLength, "Length") << G4endl;
//   G4cout << " The Container is a cylinder of " << fContainMaterial->GetName()
//          << "  thickness = " << G4BestUnit(fContainThickness, "Length")
//          << G4endl;

//   G4cout << "\n" << fAbsorMaterial << G4endl;
//   G4cout << "\n" << fContainMaterial << G4endl;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorMaterial(G4String materialChoice) {
  // search the material by its name
  G4Material *pttoMaterial =
      G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (pttoMaterial) {
    fAbsorMaterial = pttoMaterial;
    if (fLAbsor) {
      fLAbsor->SetMaterial(fAbsorMaterial);
    }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetAbsorMaterial : "
           << materialChoice << " not found" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetContainMaterial(G4String materialChoice) {
  // search the material by its name
  G4Material *pttoMaterial =
      G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (pttoMaterial) {
    fContainMaterial = pttoMaterial;
    if (fLContain) {
      fLContain->SetMaterial(fContainMaterial);
    }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetContainMaterial : "
           << materialChoice << " not found" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorRadius(G4double value) {
  fAbsorRadius = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorLength(G4double value) {
  fAbsorLength = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetContainThickness(G4double value) {
  fContainThickness = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetNbOfAbsor_Slab(G4int ival) {
  // set the number of Absorbers
  //
  if (ival < 1 || ival > (kMaxAbsor - 1)) {
    G4cout << "\n ---> warning from SetfNbOfAbsor: " << ival
           << " must be at least 1 and and most " << kMaxAbsor - 1
           << ". Command refused" << G4endl;
    return;
  }
  fNbOfAbsor = ival;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorMaterial_Slab(G4int iabs,
                                                 const G4String &material) {
  // search the material by its name
  //
  if (iabs > fNbOfAbsor || iabs <= 0) {
    G4cout << "\n --->warning from SetfAbsorMaterial: absor number " << iabs
           << " out of range. Command refused" << G4endl;
    return;
  }

  G4Material *pttoMaterial =
      G4NistManager::Instance()->FindOrBuildMaterial(material);
  if (pttoMaterial) {
    fAbsorMaterial_Slab[iabs] = pttoMaterial;
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorThickness_Slab(G4int iabs, G4double val) {
  // change Absorber thickness
  //
  if (iabs > fNbOfAbsor || iabs <= 0) {
    G4cout << "\n --->warning from SetfAbsorThickness: absor number " << iabs
           << " out of range. Command refused" << G4endl;
    return;
  }
  if (val <= DBL_MIN) {
    G4cout << "\n --->warning from SetfAbsorThickness: thickness " << val
           << " out of range. Command refused" << G4endl;
    return;
  }
  fAbsorThickness[iabs] = val;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//*********************************************************************************//
// Added by Altingun -> setSizeY and setSizeZ
//*********************************************************************************//
void DetectorConstruction::SetAbsorSizeY_Slab(G4double val) {
  // change the transverse size
  //
  if (val <= DBL_MIN) {
    G4cout << "\n --->warning from SetfAbsorSizeY: thickness " << val
           << " out of range. Command refused" << G4endl;
    return;
  }
  fAbsorSizeY = val;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetAbsorSizeZ_Slab(G4double val) {
  // change the transverse size
  //
  if (val <= DBL_MIN) {
    G4cout << "\n --->warning from SetfAbsorSizeZ: thickness " << val
           << " out of range. Command refused" << G4endl;
    return;
  }
  fAbsorSizeZ = val;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetSiliconSlabs(G4int val) {
  // change the transverse size
  //
  if (val < 0) {
    G4cout << "\n --->warning from Silicon Slab Volumes: Value can not be "
              "negative: "
           << val << ". Command refused" << G4endl;
    return;
  }
  CreateSiliconSlabs = val;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
//*********************************************************************************//
//*********************************************************************************//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
