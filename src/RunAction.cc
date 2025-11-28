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
/// \file RunAction.cc
/// \brief Implementation of the RunAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "RunAction.hh"

#include "DetectorConstruction.hh"
#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "Run.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"

#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(DetectorConstruction *det, PrimaryGeneratorAction *prim)
    : fDetector(det), fPrimary(prim) {
  // Book predefined histograms
  fHistoManager = new HistoManager();
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);
  // analysisManager->SetNtupleMerging(true); // Merging the ntuples

  // Create ntuple for energy deposition
  analysisManager->CreateNtuple("NeutronCapture_Data", "NeutronCapture_Data");
  analysisManager->CreateNtupleIColumn("fEvent");
  analysisManager->CreateNtupleSColumn("fParticleName");
  analysisManager->CreateNtupleDColumn("fX");
  analysisManager->CreateNtupleDColumn("fY");
  analysisManager->CreateNtupleDColumn("fZ");
  analysisManager->CreateNtupleSColumn("fInteractionType");
  analysisManager->CreateNtupleSColumn("targetIsotope");
  // analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->FinishNtuple(0);

  // Create ntuple for energy deposition
  analysisManager->CreateNtuple("BoronEdep", "BoronEdep");
  analysisManager->CreateNtupleIColumn("fEvent");
  analysisManager->CreateNtupleSColumn("fParticleName");
  analysisManager->CreateNtupleIColumn("fParentID");
  analysisManager->CreateNtupleIColumn("fParticleID");
  analysisManager->CreateNtupleIColumn("fStepNumber");
  analysisManager->CreateNtupleDColumn("fX");
  analysisManager->CreateNtupleDColumn("fY");
  analysisManager->CreateNtupleDColumn("fZ");
  analysisManager->CreateNtupleSColumn("fInteractionType");
  analysisManager->CreateNtupleSColumn("targetIsotope");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleSColumn("fCreatorProcessName");

  analysisManager->FinishNtuple(1);

  // Create ntuple for energy deposition
  analysisManager->CreateNtuple("SiliconEdep_Y_1", "SiliconEdep_Y_1");
  analysisManager->CreateNtupleIColumn("fEvent");
  analysisManager->CreateNtupleSColumn("fParticleName");
  analysisManager->CreateNtupleIColumn("fParentID");
  analysisManager->CreateNtupleIColumn("fParticleID");
  analysisManager->CreateNtupleIColumn("fStepNumber");
  analysisManager->CreateNtupleDColumn("fX");
  analysisManager->CreateNtupleDColumn("fY");
  analysisManager->CreateNtupleDColumn("fZ");
  analysisManager->CreateNtupleSColumn("fInteractionType");
  analysisManager->CreateNtupleSColumn("targetIsotope");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("StopTable");
  analysisManager->CreateNtupleDColumn("StopFull");
  analysisManager->CreateNtupleDColumn("MeandEdx");
  analysisManager->CreateNtupleDColumn("StopPower");
  analysisManager->CreateNtupleSColumn("fCreatorProcessName");
  analysisManager->CreateNtupleSColumn("fPVatVertexname");

  analysisManager->FinishNtuple(2);

  // Create ntuple for energy deposition
  analysisManager->CreateNtuple("SiliconEdep_Y_2", "SiliconEdep_Y_2");
  analysisManager->CreateNtupleIColumn("fEvent");
  analysisManager->CreateNtupleSColumn("fParticleName");
  analysisManager->CreateNtupleIColumn("fParentID");
  analysisManager->CreateNtupleIColumn("fParticleID");
  analysisManager->CreateNtupleIColumn("fStepNumber");
  analysisManager->CreateNtupleDColumn("fX");
  analysisManager->CreateNtupleDColumn("fY");
  analysisManager->CreateNtupleDColumn("fZ");
  analysisManager->CreateNtupleSColumn("fInteractionType");
  analysisManager->CreateNtupleSColumn("targetIsotope");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("StopTable");
  analysisManager->CreateNtupleDColumn("StopFull");
  analysisManager->CreateNtupleDColumn("MeandEdx");
  analysisManager->CreateNtupleDColumn("StopPower");
  analysisManager->CreateNtupleSColumn("fCreatorProcessName");
  analysisManager->CreateNtupleSColumn("fPVatVertexname");

  analysisManager->FinishNtuple(3);

  // Create ntuple for energy deposition
  analysisManager->CreateNtuple("SiliconEdep_Z_1", "SiliconEdep_Z_1");
  analysisManager->CreateNtupleIColumn("fEvent");
  analysisManager->CreateNtupleSColumn("fParticleName");
  analysisManager->CreateNtupleIColumn("fParentID");
  analysisManager->CreateNtupleIColumn("fParticleID");
  analysisManager->CreateNtupleIColumn("fStepNumber");
  analysisManager->CreateNtupleDColumn("fX");
  analysisManager->CreateNtupleDColumn("fY");
  analysisManager->CreateNtupleDColumn("fZ");
  analysisManager->CreateNtupleSColumn("fInteractionType");
  analysisManager->CreateNtupleSColumn("targetIsotope");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("StopTable");
  analysisManager->CreateNtupleDColumn("StopFull");
  analysisManager->CreateNtupleDColumn("MeandEdx");
  analysisManager->CreateNtupleDColumn("StopPower");
  analysisManager->CreateNtupleSColumn("fCreatorProcessName");
  analysisManager->CreateNtupleSColumn("fPVatVertexname");

  analysisManager->FinishNtuple(4);

  // Create ntuple for energy deposition
  analysisManager->CreateNtuple("SiliconEdep_Z_2", "SiliconEdep_Z_2");
  analysisManager->CreateNtupleIColumn("fEvent");
  analysisManager->CreateNtupleSColumn("fParticleName");
  analysisManager->CreateNtupleIColumn("fParentID");
  analysisManager->CreateNtupleIColumn("fParticleID");
  analysisManager->CreateNtupleIColumn("fStepNumber");
  analysisManager->CreateNtupleDColumn("fX");
  analysisManager->CreateNtupleDColumn("fY");
  analysisManager->CreateNtupleDColumn("fZ");
  analysisManager->CreateNtupleSColumn("fInteractionType");
  analysisManager->CreateNtupleSColumn("targetIsotope");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("StopTable");
  analysisManager->CreateNtupleDColumn("StopFull");
  analysisManager->CreateNtupleDColumn("MeandEdx");
  analysisManager->CreateNtupleDColumn("StopPower");
  analysisManager->CreateNtupleSColumn("fCreatorProcessName");
  analysisManager->CreateNtupleSColumn("fPVatVertexname");

  analysisManager->FinishNtuple(5);

  // Create ntuple for particles reaching to the world boundry
  analysisManager->CreateNtuple("Particles_Exit_World", "Particles_Exit_World");
  analysisManager->CreateNtupleIColumn("fEvent");
  analysisManager->CreateNtupleSColumn("fParticleName");
  analysisManager->CreateNtupleIColumn("fParentID");
  analysisManager->CreateNtupleIColumn("fParticleID");
  analysisManager->CreateNtupleIColumn("fStepNumber");
  analysisManager->CreateNtupleDColumn("fX");
  analysisManager->CreateNtupleDColumn("fY");
  analysisManager->CreateNtupleDColumn("fZ");
  analysisManager->CreateNtupleDColumn("fKinEnergy");
  analysisManager->CreateNtupleSColumn("fInteractionType");
  analysisManager->CreateNtupleSColumn("targetIsotope");
  analysisManager->CreateNtupleSColumn("fCreatorProcessName");
  analysisManager->CreateNtupleSColumn("fPVatVertexname");

  analysisManager->FinishNtuple(6);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction() { delete fHistoManager; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run *RunAction::GenerateRun() {
  fRun = new Run(fDetector);
  return fRun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run *) {
  // show Rndm status
  if (isMaster)
    G4Random::showEngineStatus();

  // keep run condition
  if (fPrimary) {
    G4ParticleDefinition *particle =
        fPrimary->GetParticleGun()->GetParticleDefinition();
    G4double energy = fPrimary->GetParticleGun()->GetParticleEnergy();
    fRun->SetPrimary(particle, energy);
  }

  // histograms
  //
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->IsActive()) {
    analysisManager->OpenFile();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run *) {
  if (isMaster)
    fRun->EndOfRun();

  // save histograms
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->IsActive()) {
    analysisManager->Write();
    analysisManager->CloseFile();
  }

  // show Rndm status
  if (isMaster)
    G4Random::showEngineStatus();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
