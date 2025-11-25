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
/// \file HistoManager.cc
/// \brief Implementation of the HistoManager class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"

#include "G4UnitsTable.hh"
#include <iterator>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager() { Book(); }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book() {
  // Create or get analysis manager
  // The choice of analysis technology is done via selection of a namespace
  // in HistoManager.hh
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");
  analysisManager->SetFileName(fFileName);
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetActivation(true); // enable inactivation of histograms

  // Define histograms start values
  const G4int kMaxHisto = 14;
  const G4String id[] = {"0", "1", "2", "3",  "4",  "5",  "6",
                         "7", "8", "9", "10", "11", "12", "13"};
  const G4String title[] = {
      "dummy",                                                     // 0
      "total energy deposit",                                      // 1
      "dummy",                                                     // 2
      "total kinetic energy flow",                                 // 3
      "energy spectrum of emerging gamma",                         // 4
      "energy spectrum of emerging e+-",                           // 5
      "energy spectrum of emerging neutrons",                      // 6
      "energy spectrum of emerging protons",                       // 7
      "energy spectrum of emerging deuterons",                     // 8
      "energy spectrum of emerging alphas",                        // 9
      "energy spectrum of all others emerging ions",               // 10
      "energy spectrum of all others emerging baryons",            // 11
      "energy spectrum of all others emerging mesons",             // 12
      "energy spectrum of all others emerging leptons (neutrinos)" // 13
  };

  // Default values (to be reset via /analysis/h1/set command)
  G4int nbins = 100;
  G4double vmin = 0.;
  G4double vmax = 100.;

  // Create all histograms as inactivated
  // as we have not yet set nbins, vmin, vmax
  for (G4int k = 0; k < kMaxHisto; k++) {
    G4int ih = analysisManager->CreateH1(id[k], title[k], nbins, vmin, vmax);
    analysisManager->SetH1Activation(ih, false);
  }

  // create 2D histograms for particle flux
  G4int nbins2D = 110;
  G4double vmin2D = -550.;
  G4double vmax2D = 550.;

  const G4int kHisto2D = 6;
  const G4int kPArticleHisto2D = 10;

  const G4String particle_types[] = {
      "gamma",        "e+-",
      "neutrons",     "protons",
      "deuterons",    "alphas",
      "other ions",   "other baryons",
      "other mesons", "other leptons (neutrinos)"};
  const G4String histo_types[] = {
      "XY_-Z Particle Count", "XY_+Z Particle Count", "ZY_-X Particle Count",
      "ZY_+X Particle Count", "ZX_-Y Particle Count", "ZX_+Y Particle Count"};

  G4int hist_id_counter = 1;

  for (G4int i = 0; i < kHisto2D; i++) {
    for (G4int j = 0; j < kPArticleHisto2D; j++) {
      std::ostringstream mystr;
      mystr << hist_id_counter << std::endl;
      G4int ih = analysisManager->CreateH2(
          mystr.str().c_str(), histo_types[i] + " of " + particle_types[j],
          nbins2D, vmin2D, vmax2D, nbins2D, vmin2D, vmax2D);
      analysisManager->SetH2Activation(ih, false);
      // std::cout << "Created 2D histo ID: " << ih << std::endl;
      hist_id_counter++;
    }
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
