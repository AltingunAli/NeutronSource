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
/// \file TrackingAction.cc
/// \brief Implementation of the TrackingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TrackingAction.hh"

#include "EventAction.hh"
#include "HistoManager.hh"
#include "Run.hh"

#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4StepStatus.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(EventAction *event) : fEventAction(event) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track *track) {
  // count secondary particles
  if (track->GetTrackID() == 1)
    return;

  Run *run = static_cast<Run *>(
      G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  G4String name = track->GetDefinition()->GetParticleName();
  G4double meanLife = track->GetDefinition()->GetPDGLifeTime();
  G4double energy = track->GetKineticEnergy();
  if (meanLife != 0)
    run->ParticleCount(name, energy, meanLife);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track *track) {
  // keep only outgoing particle
  G4StepStatus status = track->GetStep()->GetPostStepPoint()->GetStepStatus();
  if (status != fWorldBoundary)
    return;

  const G4ParticleDefinition *particle = track->GetParticleDefinition();
  G4String name = particle->GetParticleName();
  G4double energy = track->GetKineticEnergy();

  fEventAction->AddEflow(energy);

  Run *run = static_cast<Run *>(
      G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->ParticleFlux(name, energy);

  // const G4ThreeVector& postStepPosition =
  // track->GetStep()->GetPostStepPoint()->GetPosition();

  // std::cout << "Particle Pos:" << "x = " << postStepPosition.x() << ", y = "
  // << postStepPosition.y()
  //           << ", z = " << postStepPosition.z() << G4endl;

  // histograms: enery flow
  //
  G4AnalysisManager *analysis = G4AnalysisManager::Instance();

  G4int ih = 0;
  G4String type = particle->GetParticleType();
  G4double charge = particle->GetPDGCharge();
  if (charge > 3.)
    ih = 10;
  else if (particle == G4Gamma::Gamma())
    ih = 4;
  else if (particle == G4Electron::Electron())
    ih = 5;
  else if (particle == G4Positron::Positron())
    ih = 5;
  else if (particle == G4Neutron::Neutron())
    ih = 6;
  else if (particle == G4Proton::Proton())
    ih = 7;
  else if (particle == G4Deuteron::Deuteron())
    ih = 8;
  else if (particle == G4Alpha::Alpha())
    ih = 9;
  else if (type == "nucleus")
    ih = 10;
  else if (type == "baryon")
    ih = 11;
  else if (type == "meson")
    ih = 12;
  else if (type == "lepton")
    ih = 13;
  if (ih > 0)
    analysis->FillH1(ih, energy);

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  // Track positions
  G4double posX = track->GetStep()->GetPostStepPoint()->GetPosition().x();
  G4double posY = track->GetStep()->GetPostStepPoint()->GetPosition().y();
  G4double posZ = track->GetStep()->GetPostStepPoint()->GetPosition().z();

  G4double preX = track->GetStep()->GetPreStepPoint()->GetPosition().x();
  G4double preY = track->GetStep()->GetPreStepPoint()->GetPosition().y();
  G4double preZ = track->GetStep()->GetPreStepPoint()->GetPosition().z();

  // std::cout << name << " :" << "x = " << posX << " (" << preX << ") "
  //           << ", y = " << posY << " (" << preY << ") " << ", z = " << posZ
  //           << " (" << preZ << ") " << G4endl;
  const G4int kHisto2D = 6;
  const G4int kPArticleHisto2D = 10;
  ih = -1;
  //&& posX <= 500. && posX >= -500. && posY <= 500. && posY >= -500.

  // for (G4int i = 0; i < kHisto2D; i++) {
  //   for (G4int j = 0; j < kPArticleHisto2D; j++) {
  if (posZ <= -500.) {
    if (particle == G4Gamma::Gamma())
      ih = 0;
    else if (particle == G4Electron::Electron() ||
             particle == G4Positron::Positron())
      ih = 1;
    else if (particle == G4Neutron::Neutron())
      ih = 2;
    else if (particle == G4Proton::Proton())
      ih = 3;
    else if (particle == G4Deuteron::Deuteron())
      ih = 4;
    else if (particle == G4Alpha::Alpha())
      ih = 5;
    else if (type == "nucleus" || charge > 3.)
      ih = 6;
    else if (type == "baryon")
      ih = 7;
    else if (type == "meson")
      ih = 8;
    else if (type == "lepton")
      ih = 9;
    if (ih >= 0)
      analysis->FillH2(ih, posX, posY, 1);
  }
  if (posZ >= 500.) {
    if (particle == G4Gamma::Gamma())
      ih = 0 + kPArticleHisto2D;
    else if (particle == G4Electron::Electron() ||
             particle == G4Positron::Positron())
      ih = 1 + kPArticleHisto2D;
    else if (particle == G4Neutron::Neutron())
      ih = 2 + kPArticleHisto2D;
    else if (particle == G4Proton::Proton())
      ih = 3 + kPArticleHisto2D;
    else if (particle == G4Deuteron::Deuteron())
      ih = 4 + kPArticleHisto2D;
    else if (particle == G4Alpha::Alpha())
      ih = 5 + kPArticleHisto2D;
    else if (type == "nucleus" || charge > 3.)
      ih = 6 + kPArticleHisto2D;
    else if (type == "baryon")
      ih = 7 + kPArticleHisto2D;
    else if (type == "meson")
      ih = 8 + kPArticleHisto2D;
    else if (type == "lepton")
      ih = 9 + kPArticleHisto2D;
    if (ih > 0)
      analysis->FillH2(ih, posX, posY, 1);
  }
  if (posX <= -500.) {
    if (particle == G4Gamma::Gamma())
      ih = 0 + kPArticleHisto2D * 2;
    else if (particle == G4Electron::Electron() ||
             particle == G4Positron::Positron())
      ih = 1 + kPArticleHisto2D * 2;
    else if (particle == G4Neutron::Neutron())
      ih = 2 + kPArticleHisto2D * 2;
    else if (particle == G4Proton::Proton())
      ih = 3 + kPArticleHisto2D * 2;
    else if (particle == G4Deuteron::Deuteron())
      ih = 4 + kPArticleHisto2D * 2;
    else if (particle == G4Alpha::Alpha())
      ih = 5 + kPArticleHisto2D * 2;
    else if (type == "nucleus" || charge > 3.)
      ih = 6 + kPArticleHisto2D * 2;
    else if (type == "baryon")
      ih = 7 + kPArticleHisto2D * 2;
    else if (type == "meson")
      ih = 8 + kPArticleHisto2D * 2;
    else if (type == "lepton")
      ih = 9 + kPArticleHisto2D * 2;
    if (ih > 0)
      analysis->FillH2(ih, posZ, posY, 1);
  }
  if (posX >= 500.) {
    if (particle == G4Gamma::Gamma())
      ih = 0 + kPArticleHisto2D * 3;
    else if (particle == G4Electron::Electron() ||
             particle == G4Positron::Positron())
      ih = 1 + kPArticleHisto2D * 3;
    else if (particle == G4Neutron::Neutron())
      ih = 2 + kPArticleHisto2D * 3;
    else if (particle == G4Proton::Proton())
      ih = 3 + kPArticleHisto2D * 3;
    else if (particle == G4Deuteron::Deuteron())
      ih = 4 + kPArticleHisto2D * 3;
    else if (particle == G4Alpha::Alpha())
      ih = 5 + kPArticleHisto2D * 3;
    else if (type == "nucleus" || charge > 3.)
      ih = 6 + kPArticleHisto2D * 3;
    else if (type == "baryon")
      ih = 7 + kPArticleHisto2D * 3;
    else if (type == "meson")
      ih = 8 + kPArticleHisto2D * 3;
    else if (type == "lepton")
      ih = 9 + kPArticleHisto2D * 3;
    if (ih > 0)
      analysis->FillH2(ih, posZ, posY, 1);

    // std::cout << "ih = " << ih << " posX = " << posX << " posY = " << posY
    //           << " posZ = " << posZ << G4endl;
  }
  if (posY <= -500.) {
    if (particle == G4Gamma::Gamma())
      ih = 0 + kPArticleHisto2D * 4;
    else if (particle == G4Electron::Electron() ||
             particle == G4Positron::Positron())
      ih = 1 + kPArticleHisto2D * 4;
    else if (particle == G4Neutron::Neutron())
      ih = 2 + kPArticleHisto2D * 4;
    else if (particle == G4Proton::Proton())
      ih = 3 + kPArticleHisto2D * 4;
    else if (particle == G4Deuteron::Deuteron())
      ih = 4 + kPArticleHisto2D * 4;
    else if (particle == G4Alpha::Alpha())
      ih = 5 + kPArticleHisto2D * 4;
    else if (type == "nucleus" || charge > 3.)
      ih = 6 + kPArticleHisto2D * 4;
    else if (type == "baryon")
      ih = 7 + kPArticleHisto2D * 4;
    else if (type == "meson")
      ih = 8 + kPArticleHisto2D * 4;
    else if (type == "lepton")
      ih = 9 + kPArticleHisto2D * 4;
    if (ih > 0)
      analysis->FillH2(ih, posZ, posX, 1);
  }
  if (posY >= 500.) {
    if (particle == G4Gamma::Gamma())
      ih = 0 + kPArticleHisto2D * 5;
    else if (particle == G4Electron::Electron() ||
             particle == G4Positron::Positron())
      ih = 1 + kPArticleHisto2D * 5;
    else if (particle == G4Neutron::Neutron())
      ih = 2 + kPArticleHisto2D * 5;
    else if (particle == G4Proton::Proton())
      ih = 3 + kPArticleHisto2D * 5;
    else if (particle == G4Deuteron::Deuteron())
      ih = 4 + kPArticleHisto2D * 5;
    else if (particle == G4Alpha::Alpha())
      ih = 5 + kPArticleHisto2D * 5;
    else if (type == "nucleus" || charge > 3.)
      ih = 6 + kPArticleHisto2D * 5;
    else if (type == "baryon")
      ih = 7 + kPArticleHisto2D * 5;
    else if (type == "meson")
      ih = 8 + kPArticleHisto2D * 5;
    else if (type == "lepton")
      ih = 9 + kPArticleHisto2D * 5;
    if (ih > 0)
      analysis->FillH2(ih, posZ, posX, 1);
  }
  // }
  // }

  // if (posZ < 0) {
  //   analysis->FillH2(0, posX, posY, 1);
  // }
  // if (posZ > 0) {
  //   analysis->FillH2(1, posX, posY, 1);
  // }
  // if (posX < 0) {
  //   analysis->FillH2(2, posZ, posY, 1);
  // }
  // if (posX > 0) {
  //   analysis->FillH2(3, posZ, posY, 1);
  // }
  // if (posY < 0) {
  //   analysis->FillH2(4, posZ, posX, 1);
  // }
  // if (posY > 0) {
  //   analysis->FillH2(5, posZ, posX, 1);
  // }
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
