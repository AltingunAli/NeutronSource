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
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4Material;
class DetectorMessenger;
const G4int kMaxAbsor = 10; // 0 + 9

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  ~DetectorConstruction() override;

public:
  G4VPhysicalVolume *Construct() override;

  G4Material *MaterialWithSingleIsotope(G4String, G4String, G4double, G4int,
                                        G4int);

  void SetAbsorRadius(G4double);
  void SetAbsorLength(G4double);
  void SetAbsorMaterial(G4String);

  void SetContainThickness(G4double);
  void SetContainMaterial(G4String);

public:
  G4double GetAbsorRadius() { return fAbsorRadius; };
  G4double GetAbsorLength() { return fAbsorLength; };
  G4Material *GetAbsorMaterial() { return fAbsorMaterial; };

  G4double GetContainThickness() { return fContainThickness; };
  G4Material *GetContainMaterial() { return fContainMaterial; };

  void SetNbOfAbsor_Slab(G4int);
  void SetAbsorMaterial_Slab(G4int, const G4String &);
  void SetAbsorThickness_Slab(G4int, G4double);

  void SetAbsorSizeY_Slab(G4double);
  void SetAbsorSizeZ_Slab(G4double);
  void SetSiliconSlabs(G4int);

  G4int GetNbOfAbsor() { return fNbOfAbsor; }
  G4Material *GetAbsorMaterial_Slab(G4int i) { return fAbsorMaterial_Slab[i]; };
  G4double GetAbsorThickness(G4int i) { return fAbsorThickness[i]; };
  G4double GetXfront(G4int i) { return fXfront[i]; };

  G4double GetAbsorSizeX() { return fAbsorSizeX; };
  G4double GetAbsorSizeYZ() { return fAbsorSizeYZ; };

  G4double GetWorldSizeX() { return fWorldSizeX; };
  G4double GetWorldSizeYZ() { return fWorldSizeYZ; };

  void PrintParameters();

private:
  G4double fAbsorRadius = 0., fAbsorLength = 0.;
  G4Material *fAbsorMaterial = nullptr;
  G4LogicalVolume *fLAbsor = nullptr;

  G4double fContainThickness = 0.;
  G4Material *fContainMaterial = nullptr;
  G4LogicalVolume *fLContain = nullptr;

  G4Material *fWorldMaterial = nullptr;
  G4Material *matSilicon = nullptr;
  G4VPhysicalVolume *fPWorld = nullptr;

  DetectorMessenger *fDetectorMessenger = nullptr;

  G4int fNbOfAbsor = 0;
  G4Material *fAbsorMaterial_Slab[kMaxAbsor];
  G4double fAbsorThickness[kMaxAbsor];
  G4double fXfront[kMaxAbsor];

  G4double fAbsorSizeX = 0.;
  G4double fAbsorSizeYZ = 0.;
  G4double fAbsorSizeY = 0.;
  G4double fAbsorSizeZ = 0.;

  G4double fWorldSizeX = 0.;
  G4double fWorldSizeYZ = 0.;
  G4double fWorldSizeY = 0.;
  G4double fWorldSizeZ = 0.;

  G4int CreateSiliconSlabs = 0;

private:
  void DefineMaterials();
  G4VPhysicalVolume *ConstructVolumes();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
