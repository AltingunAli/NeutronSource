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
/// \file DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction *Det)
    : fDetector(Det) {
  fTesthadrDir = new G4UIdirectory("/testhadr/");
  fTesthadrDir->SetGuidance("commands specific to this example");

  G4bool broadcast = false;
  fDetDir = new G4UIdirectory("/testhadr/det/", broadcast);
  fDetDir->SetGuidance("detector construction commands");

  fMaterCmd1 = new G4UIcmdWithAString("/testhadr/det/setAbsorMat", this);
  fMaterCmd1->SetGuidance("Select absorber material");
  fMaterCmd1->SetParameterName("choice", false);
  fMaterCmd1->AvailableForStates(G4State_PreInit, G4State_Idle);

  fMaterCmd2 = new G4UIcmdWithAString("/testhadr/det/setContMat", this);
  fMaterCmd2->SetGuidance("Select container material");
  fMaterCmd2->SetParameterName("choice", false);
  fMaterCmd2->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSizeCmd1 =
      new G4UIcmdWithADoubleAndUnit("/testhadr/det/setAbsorRadius", this);
  fSizeCmd1->SetGuidance("Set absorber radius");
  fSizeCmd1->SetParameterName("Radius", false);
  fSizeCmd1->SetRange("Radius>0.");
  fSizeCmd1->SetUnitCategory("Length");
  fSizeCmd1->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSizeCmd2 =
      new G4UIcmdWithADoubleAndUnit("/testhadr/det/setAbsorLength", this);
  fSizeCmd2->SetGuidance("Set absorber length");
  fSizeCmd2->SetParameterName("Length", false);
  fSizeCmd2->SetRange("Length>0.");
  fSizeCmd2->SetUnitCategory("Length");
  fSizeCmd2->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSizeCmd3 = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setContThick", this);
  fSizeCmd3->SetGuidance("Set container thickness");
  fSizeCmd3->SetParameterName("Thick", false);
  fSizeCmd3->SetRange("Thick>0.");
  fSizeCmd3->SetUnitCategory("Length");
  fSizeCmd3->AvailableForStates(G4State_PreInit, G4State_Idle);

  fIsotopeCmd = new G4UIcommand("/testhadr/det/setIsotopeMat", this);
  fIsotopeCmd->SetGuidance("Build and select a material with single isotope");
  fIsotopeCmd->SetGuidance("  symbol of isotope, Z, A, density of material");

  // ##################################################################################//

  fSiliconSlabs =
      new G4UIcmdWithAnInteger("/testhadr/det/SetSiliconSlabs", this);
  fSiliconSlabs->SetGuidance("Create the silicon slabs around the monitor.");
  fSiliconSlabs->SetParameterName("SiliconSlabs", false);
  fSiliconSlabs->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSiliconSlabs->SetToBeBroadcasted(false);

  fNbAbsorCmd = new G4UIcmdWithAnInteger("/testhadr/det/setNbOfAbsor", this);
  fNbAbsorCmd->SetGuidance("Set number of Absorbers.");
  fNbAbsorCmd->SetParameterName("NbAbsor", false);
  fNbAbsorCmd->SetRange("NbAbsor>0");
  fNbAbsorCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fNbAbsorCmd->SetToBeBroadcasted(false);

  fAbsorCmd = new G4UIcommand("/testhadr/det/setAbsor", this);
  fAbsorCmd->SetGuidance("Set the absor nb, the material, the thickness.");
  fAbsorCmd->SetGuidance("  absor number : from 1 to NbOfAbsor");
  fAbsorCmd->SetGuidance("  material name");
  fAbsorCmd->SetGuidance("  thickness (with unit) : t>0.");
  //
  G4UIparameter *AbsNbPrm = new G4UIparameter("AbsorNb", 'i', false);
  AbsNbPrm->SetGuidance("absor number : from 1 to NbOfAbsor");
  AbsNbPrm->SetParameterRange("AbsorNb>0");
  fAbsorCmd->SetParameter(AbsNbPrm);
  //
  G4UIparameter *MatPrm = new G4UIparameter("material", 's', false);
  MatPrm->SetGuidance("material name");
  fAbsorCmd->SetParameter(MatPrm);
  //
  G4UIparameter *ThickPrm = new G4UIparameter("thickness", 'd', false);
  ThickPrm->SetGuidance("thickness of absorber");
  ThickPrm->SetParameterRange("thickness>0.");
  fAbsorCmd->SetParameter(ThickPrm);
  //
  G4UIparameter *unitPrm = new G4UIparameter("unit", 's', false);
  unitPrm->SetGuidance("unit of thickness");
  G4String unitList = G4UIcommand::UnitsList(G4UIcommand::CategoryOf("mm"));
  unitPrm->SetParameterCandidates(unitList);
  fAbsorCmd->SetParameter(unitPrm);

  fAbsorCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fAbsorCmd->SetToBeBroadcasted(false);

  fSizeYCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setSizeY", this);
  fSizeYCmd->SetGuidance("Set sizeY of the absorber");
  fSizeYCmd->SetParameterName("SizeY", false);
  fSizeYCmd->SetRange("SizeY>0.");
  fSizeYCmd->SetUnitCategory("Length");
  fSizeYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSizeYCmd->SetToBeBroadcasted(false);

  fSizeZCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setSizeZ", this);
  fSizeZCmd->SetGuidance("Set sizeYZ of the absorber");
  fSizeZCmd->SetParameterName("SizeZ", false);
  fSizeZCmd->SetRange("SizeZ>0.");
  fSizeZCmd->SetUnitCategory("Length");
  fSizeZCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSizeZCmd->SetToBeBroadcasted(false);

  // ###################################################################################//
  G4UIparameter *symbPrm = new G4UIparameter("isotope", 's', false);
  symbPrm->SetGuidance("isotope symbol");
  fIsotopeCmd->SetParameter(symbPrm);
  //
  G4UIparameter *ZPrm = new G4UIparameter("Z", 'i', false);
  ZPrm->SetGuidance("Z");
  ZPrm->SetParameterRange("Z>0");
  fIsotopeCmd->SetParameter(ZPrm);
  //
  G4UIparameter *APrm = new G4UIparameter("A", 'i', false);
  APrm->SetGuidance("A");
  APrm->SetParameterRange("A>0");
  fIsotopeCmd->SetParameter(APrm);
  //
  G4UIparameter *densityPrm = new G4UIparameter("density", 'd', false);
  densityPrm->SetGuidance("density of material");
  densityPrm->SetParameterRange("density>0.");
  fIsotopeCmd->SetParameter(densityPrm);
  //
  G4UIparameter *unitPrm_ = new G4UIparameter("unit", 's', false);
  unitPrm_->SetGuidance("unit of density");
  G4String unitList_ = G4UIcommand::UnitsList(G4UIcommand::CategoryOf("g/cm3"));
  unitPrm_->SetParameterCandidates(unitList_);
  fIsotopeCmd->SetParameter(unitPrm_);
  //
  fIsotopeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger() {
  delete fMaterCmd1;
  delete fMaterCmd2;
  delete fSizeCmd1;
  delete fSizeCmd2;
  delete fSizeCmd3;
  delete fIsotopeCmd;
  delete fDetDir;
  delete fTesthadrDir;

  // ###################################################################################//
  delete fNbAbsorCmd;
  delete fSiliconSlabs;
  delete fAbsorCmd;
  delete fSizeYCmd;
  delete fSizeZCmd;
  // ###################################################################################//
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {
  if (command == fMaterCmd1) {
    fDetector->SetAbsorMaterial(newValue);
  }
  if (command == fMaterCmd2) {
    fDetector->SetContainMaterial(newValue);
  }

  if (command == fSizeCmd1) {
    fDetector->SetAbsorRadius(fSizeCmd1->GetNewDoubleValue(newValue));
  }

  if (command == fSizeCmd2) {
    fDetector->SetAbsorLength(fSizeCmd2->GetNewDoubleValue(newValue));
  }

  if (command == fSizeCmd3) {
    fDetector->SetContainThickness(fSizeCmd3->GetNewDoubleValue(newValue));
  }

  if (command == fIsotopeCmd) {
    G4int Z;
    G4int A;
    G4double dens;
    G4String name, unt;
    std::istringstream is(newValue);
    is >> name >> Z >> A >> dens >> unt;
    dens *= G4UIcommand::ValueOf(unt);
    fDetector->MaterialWithSingleIsotope(name, name, dens, Z, A);
    fDetector->SetAbsorMaterial(name);
  }
  // ###################################################################################//
  if (command == fSiliconSlabs) {
    fDetector->SetSiliconSlabs(fSiliconSlabs->GetNewIntValue(newValue));
  }
  if (command == fNbAbsorCmd) {
    fDetector->SetNbOfAbsor_Slab(fNbAbsorCmd->GetNewIntValue(newValue));
  }
  if (command == fAbsorCmd) {
    G4int num;
    G4double tick;
    G4String unt, mat;
    std::istringstream is(newValue);
    is >> num >> mat >> tick >> unt;
    G4String material = mat;
    tick *= G4UIcommand::ValueOf(unt);
    fDetector->SetAbsorMaterial_Slab(num, material);
    fDetector->SetAbsorThickness_Slab(num, tick);
  }

  if (command == fSizeYCmd) {
    fDetector->SetAbsorSizeY_Slab(fSizeYCmd->GetNewDoubleValue(newValue));
  }

  if (command == fSizeZCmd) {
    fDetector->SetAbsorSizeZ_Slab(fSizeZCmd->GetNewDoubleValue(newValue));
  }
  // ###################################################################################//
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
