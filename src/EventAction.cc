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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EventAction.hh"
#include "HistoManager.hh"
#include "Run.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event *anEvent) {

  //  Print the Run status
  G4int eventID = anEvent->GetEventID();
  G4Run *run = static_cast<G4Run *>(
      G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  G4int nOfEvents = run->GetNumberOfEventToBeProcessed();
  G4double evperCent = 10.; // status increment in percent

  if (fmod(eventID, double(nOfEvents * evperCent * 0.001)) == 0) {
    time_t my_time = time(NULL);
    tm *ltm = localtime(&my_time);
    G4double status = (100 * (eventID / double(nOfEvents)));
    std::cout << "=> Run " << eventID << " starts (" << status << "%, "
              << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << ")"
              << std::endl;
  }

  fTotalEnergyDeposit = 0.;
  fTotalEnergyFlow = 0.;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddEdep(G4double Edep) { fTotalEnergyDeposit += Edep; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddEflow(G4double Eflow) { fTotalEnergyFlow += Eflow; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event *) {
  Run *run = static_cast<Run *>(
      G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  run->AddEdep(fTotalEnergyDeposit);
  run->AddEflow(fTotalEnergyFlow);

  G4AnalysisManager::Instance()->FillH1(1, fTotalEnergyDeposit);
  G4AnalysisManager::Instance()->FillH1(3, fTotalEnergyFlow);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
