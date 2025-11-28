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
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"
#include "G4EmCalculator.hh"
#include "SteppingActionMessenger.hh"

#include "EventAction.hh"
#include "HistoManager.hh"
#include "Run.hh"

#include "G4HadronicProcess.hh"
#include "G4RunManager.hh"

#include "G4SystemOfUnits.hh"
#include <any>
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction *event) : fEventAction(event) {

  steppingMessenger = new SteppingActionMessenger(this);
  save_silicon_data = 0;
  save_flux_data = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

SteppingAction::~SteppingAction() { delete steppingMessenger; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step *aStep) {
  // count processes
  //
  const G4StepPoint *endPoint = aStep->GetPostStepPoint();
  const G4VProcess *process = endPoint->GetProcessDefinedStep();
  Run *run = static_cast<Run *>(
      G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->CountProcesses(process);

  // ############################################################################################//
  // #  Accesing Track and Step information #//
  // ############################################################################################//

  // Track object
  G4Track *theTrack = aStep->GetTrack();
  // Objects for pre- and post-positions of the track
  G4StepPoint *thePrePoint = aStep->GetPreStepPoint();
  G4StepPoint *thePostPoint = aStep->GetPostStepPoint();

  // ############################################################################################//
  // #  PARTICLE INFORMATION #//
  // ############################################################################################//
  // Particle Name
  G4ParticleDefinition *particleType = theTrack->GetDefinition();
  // G4String fParticleName = particleType->GetParticleName();
  G4String fParticleName = aStep->GetTrack()
                               ->GetDynamicParticle()
                               ->GetDefinition()
                               ->GetParticleName();

  G4ThreeVector posParticle = thePostPoint->GetPosition();

  G4int StepNumberr = aStep->GetTrack()->GetCurrentStepNumber();
  G4int part_parent_ID = theTrack->GetParentID();
  G4int part_ID = theTrack->GetTrackID();
  // ############################################################################################//
  // #  PARTICLE VOLUMES #//
  // ############################################################################################//
  // Vertex volume of the corresponding particle
  const G4LogicalVolume *PVatVertex = theTrack->GetLogicalVolumeAtVertex();
  G4String PVatVertexname = PVatVertex->GetName();
  // Particle pre and post step volume
  G4VPhysicalVolume *thePostPV = thePostPoint->GetPhysicalVolume();
  G4VPhysicalVolume *thePrePV = thePrePoint->GetPhysicalVolume();

  // thePrePV =  nullptr;
  // G4VPhysicalVolume *thePrePV =  nullptr;
  // thePrePV = thePrePoint->GetPhysicalVolume();
  G4String thePrePVname = "";
  if (thePrePV == nullptr) {
    thePrePVname = "Out_of_World";
  } else {
    // Get the name of the previous physical volume
    thePrePVname = thePrePV->GetName();
  }

  // Current physical volume of the particle
  // G4VPhysicalVolume *thePostPV =  nullptr;
  // thePostPV =  nullptr;
  // thePostPV = thePostPoint->GetPhysicalVolume();
  G4String thePostPVname = "";
  if (thePostPV == nullptr) {
    thePostPVname = "Out_of_World";
  } else {
    // Get the name of the current physical volume
    thePostPVname = thePostPV->GetName();
  }

  // ############################################################################################//
  // #  Particle Interactions #//
  // ############################################################################################//

  // Get the creator process for the current particle
  const G4VProcess *creatorProcess = theTrack->GetCreatorProcess();
  G4String creatorProcessName = "";

  // part_parent_ID > 0

  if (part_parent_ID != 0) {
    // G4cout << "creatorProcess->GetProcessName(): " <<
    // creatorProcess->GetProcessName() << std::endl;
    creatorProcessName = creatorProcess->GetProcessName();
  } else {
    creatorProcessName = "NoCreator";
  }

  // Get the particles previous and current interaction type (process)
  // Process in the previous volume
  G4VProcess *preProcess =
      const_cast<G4VProcess *>(thePrePoint->GetProcessDefinedStep());
  G4String preProcessName = "";

  if (preProcess == 0)
    preProcessName = "No preProcessName";
  else
    preProcessName =
        preProcess->GetProcessName(); // Get the name of the previous process

  // Process in the current volume
  G4VProcess *postProcess =
      const_cast<G4VProcess *>(thePostPoint->GetProcessDefinedStep());
  G4String postProcessName = "";

  if (postProcess == 0)
    postProcessName = "No PostProcess";
  else
    postProcessName =
        postProcess->GetProcessName(); // Get the name of the current process

  G4String interactionType = ""; // Type of the interaction
  G4String targetIsotope = "";   // H1 = 1.0, C12 = 2.0, O16 = 3.0, ...
  // G4String particleName = "";    //  hadronic_particle_name = 1 if the
  // interactiong particle is neutron, 0 otherwise Create a G4HadronicProcess
  // object
  G4HadronicProcess *hproc = dynamic_cast<G4HadronicProcess *>(postProcess);

  if (!postProcess) {
    std::cout << "postProcess is not good" << std::endl;
  }
  // Create a G4Isotope object (G4HadronicProcess::G4Isotope)
  const G4Isotope *target = nullptr;
  // G4Nucleus *TargetNucleus =  nullptr;

  // Name of the target atom
  // G4String targetName = "XXXX";
  if ((hproc && (hproc->GetTargetIsotope()))) {
    // If a hadronic process is underway for the corresponding particle, get the
    // pointer for the target isotope
    target = hproc->GetTargetIsotope();
    // Get the name of the target for the hadronic process
    // targetName = target->GetName();
    // interactionType = "Hadronic";
    interactionType = hproc->GetProcessName();
    targetIsotope = target->GetName();
  } else {
    interactionType = postProcessName;
    targetIsotope = thePostPVname;
    // particleName = particle_name;
  }
  // ############################################################################################//

  // ############################################################################################//
  // #  EVENT ID #//
  // ############################################################################################//
  G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  // ############################################################################################//

  // Create an instance of the analysis manager
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

  // Fill the ntuple only for neutrons created by inelastic scattering
  if (fParticleName == "neutron" && interactionType == "neutronInelastic") {
    // position of the photon created inside the detector
    analysisManager->FillNtupleIColumn(0, 0, evt);
    analysisManager->FillNtupleSColumn(0, 1, fParticleName);
    analysisManager->FillNtupleDColumn(0, 2, posParticle[0] / mm);
    analysisManager->FillNtupleDColumn(0, 3, posParticle[1] / mm);
    analysisManager->FillNtupleDColumn(0, 4, posParticle[2] / mm);
    analysisManager->FillNtupleSColumn(0, 5, interactionType);
    analysisManager->FillNtupleSColumn(0, 6, targetIsotope);
    // analysisManager->FillNtupleDColumn(0, 7, edepStep);
    analysisManager->AddNtupleRow(0);
  }

  // energy deposit
  //
  // G4double edepStep = aStep->GetTotalEnergyDeposit() / CLHEP::keV;
  G4double edepStep = aStep->GetTotalEnergyDeposit() / CLHEP::MeV;

  // if (aStep->GetTrack()->GetNextVolume()) {
  //   // // Stopping Power from input Table.

  //   // Get the material at the pre-step point
  //   G4Material *prematerial = thePrePoint->GetMaterial();
  //   // Get the material at the post-step point
  //   G4Material *postmaterial = thePostPoint->GetMaterial();
  //   //   G4cout << "Next volume is  nullptr!" << G4endl;
  //   //   return;
  //   // }
  //   // You can now use the material object, for example, to get its name
  //   G4String materialName = postmaterial->GetName();
  //   G4double density = postmaterial->GetDensity() / CLHEP::g;

  //   // current step number
  //   G4int StepNumber = aStep->GetTrack()->GetCurrentStepNumber();

  //   // Get step length
  //   G4double stepLength = aStep->GetStepLength() / CLHEP::cm2;
  //   // G4cout << "Step length: " << stepLength << " mm" << G4endl;

  //   // Kinetic energy of the particle after each step
  //   G4double kinEnergy = theTrack->GetKineticEnergy();

  //   G4double postKineticEnergy =
  //   aStep->GetPostStepPoint()->GetKineticEnergy(); G4double preKineticEnergy
  //   = aStep->GetPreStepPoint()->GetKineticEnergy();

  //   G4double EDifference = (postKineticEnergy - preKineticEnergy) /
  //   CLHEP::MeV;

  //   // Get step length
  //   G4double stepLength_ = aStep->GetStepLength() / CLHEP::mm;

  //   G4EmCalculator emCalculator;
  //   G4double dEdxTable = 0., dEdxFull = 0.;

  //   if (particleType->GetPDGCharge() != 0.) {
  //     dEdxTable =
  //         emCalculator.GetDEDX(preKineticEnergy, particleType, postmaterial);
  //     dEdxFull = emCalculator.ComputeTotalDEDX(preKineticEnergy,
  //     particleType,
  //                                              postmaterial);
  //   }

  //   G4double stopTable = dEdxTable / density;
  //   G4double stopFull = dEdxFull / density;

  //   // Stopping Power from simulation.
  //   //
  //   G4double meandEdx = edepStep / stepLength_;
  //   G4double stopPower = meandEdx / density;

  //   // // current step number
  //   // G4int StepNumberr = aStep->GetTrack()->GetCurrentStepNumber();
  //   // if ((thePostPVname == "physiSlab_AlongZ_1" ||
  //   //      thePostPVname == "physiSlab_AlongY_1" ||
  //   //      thePostPVname == "physiSlab_AlongX_1" ||
  //   //      thePostPVname == "physiSlab_AlongY_2" ||
  //   //      thePostPVname == "physiSlab_AlongX_2" ||
  //   //      thePostPVname == "physiSlab_AlongZ_2")) {
  //   //   std::cout << "Event Number: " << evt << std::endl;
  //   //   std::cout << " Particle: " << fParticleName << std::endl;
  //   //   std::cout << "Particle ID: " << part_ID << std::endl;
  //   //   std::cout << "Particle Parent ID:  " << part_parent_ID << std::endl;
  //   //   std::cout << "Step No: " << StepNumberr << std::endl;
  //   //   std::cout << "Interaction Type: " << interactionType << std::endl;
  //   //   std::cout << "Target Isotope: " << targetIsotope << std::endl;
  //   //   std::cout << "Creator Process: " << creatorProcessName << std::endl;
  //   //   std::cout << "Edep: " << edepStep << std::endl;
  //   //   std::cout << "Ekin_post - Ekin_pre: " << EDifference << std::endl;
  //   //   std::cout << "Int. Lenght (mm): " << aStep->GetStepLength() /
  //   CLHEP::mm
  //   //             << std::endl;
  //   //   std::cout << "Prevoius Volume: " << thePrePVname << std::endl;
  //   //   std::cout << "Current Volume:  " << thePostPVname << std::endl;
  //   //   std::cout << std::endl;
  //   // }

  //   if ((thePostPVname == "physiSlab_AlongZ_1" ||
  //        thePostPVname == "physiSlab_AlongY_1" ||
  //        thePostPVname == "physiSlab_AlongX_1" ||
  //        thePostPVname == "physiSlab_AlongY_2" ||
  //        thePostPVname == "physiSlab_AlongX_2" ||
  //        thePostPVname == "physiSlab_AlongZ_2")) {
  //     std::cout << "Event: " << evt << "\nStep: " << StepNumber
  //               << "\ncreatorProcessName: " << creatorProcessName
  //               << "\npreProcessName: " << preProcessName
  //               << "\npostProcessName: " << postProcessName
  //               << "\nParticle: " << fParticleName
  //               << "\nParentID: " << part_parent_ID
  //               << "\nParticleID: " << part_ID << "\nPos: " << posParticle[0]
  //               << "  " << posParticle[1] << " " << posParticle[2]
  //               << "\nIntType: " << interactionType
  //               << "\nTarget: " << targetIsotope
  //               << "\nKinEn (MeV): " << kinEnergy
  //               << "\npreKineticEnergy (MeV): " << preKineticEnergy
  //               << "\npostKineticEnergy (MeV): " << postKineticEnergy
  //               << "\nEdep (MeV): " << aStep->GetTotalEnergyDeposit()
  //               << "\nS_Lenght(mm): " << aStep->GetStepLength() / CLHEP::mm
  //               << "\nPre_vol: " << thePrePVname
  //               << "\nPost_Vol: " << thePostPVname << std::endl;
  //     std::cout << std::endl;

  //     G4cout << "Mean dE/dx = " << meandEdx << " MeV/mm" << "\t(" <<
  //     stopPower
  //            << G4endl;

  //     G4cout << "\nFrom formulas :" << G4endl;
  //     G4cout << "   restricted dEdx = " << dEdxTable << "\t(" << stopTable
  //            << ")" << G4endl;

  //     G4cout << "   Full dEdx = " << dEdxFull << "\t(" << stopFull << ")"
  //            << G4endl;
  //     G4cout << "\n" << G4endl;
  //     G4cout << "\n" << G4endl;
  //   }
  // }
  // if (thePostPVname == "physiSlab_AlongZ_1" ||
  //     thePostPVname == "physiSlab_AlongY_1" ||
  //     thePostPVname == "physiSlab_AlongX_1" ||
  //     thePostPVname == "physiSlab_AlongY_2" ||
  //     thePostPVname == "physiSlab_AlongX_2" ||
  //     thePostPVname == "physiSlab_AlongZ_2") {
  //   std::cout << evt << ": Particle: " << fParticleName << "  "
  //             << part_parent_ID << "  " << interactionType << "  "
  //             << targetIsotope << "  " << "Creator: " << creatorProcessName
  //             << "  " << aStep->GetTotalEnergyDeposit() / CLHEP::MeV << "  "
  //             << (postKineticEnergy - preKineticEnergy) / CLHEP::MeV << "  "
  //             << "Int. Lenght: " << aStep->GetStepLength() / CLHEP::mm << " "
  //             << thePrePVname << " " << thePostPVname << std::endl;
  //   std::cout << std::endl;
  // }

  // If the particle is interacted with the enriched boron slab.
  if (thePostPVname == "B4C_enriched") {
    // position of the photon created inside the detector
    analysisManager->FillNtupleIColumn(1, 0, evt);
    analysisManager->FillNtupleSColumn(1, 1, fParticleName);
    analysisManager->FillNtupleIColumn(1, 2, part_parent_ID);
    analysisManager->FillNtupleIColumn(1, 3, part_ID);
    analysisManager->FillNtupleIColumn(1, 4, StepNumberr);
    analysisManager->FillNtupleDColumn(1, 5, posParticle[0] / mm);
    analysisManager->FillNtupleDColumn(1, 6, posParticle[1] / mm);
    analysisManager->FillNtupleDColumn(1, 7, posParticle[2] / mm);
    analysisManager->FillNtupleSColumn(1, 8, interactionType);
    analysisManager->FillNtupleSColumn(1, 9, targetIsotope);
    analysisManager->FillNtupleDColumn(1, 10, edepStep);
    analysisManager->FillNtupleSColumn(1, 11, creatorProcessName);
    analysisManager->AddNtupleRow(1);
  }

  // if (!step->GetTrack()->GetNextVolume())
  //  Check if the particle is leaving the world volume
  //  Save the information of the particle exiting the world volume
  if (thePrePV != nullptr && thePostPV == nullptr && save_flux_data == 1) {
    // G4cout << "Particle exited the world volume." << G4endl;
    analysisManager->FillNtupleIColumn(6, 0, evt);
    analysisManager->FillNtupleSColumn(6, 1, fParticleName);
    analysisManager->FillNtupleIColumn(6, 2, part_parent_ID);
    analysisManager->FillNtupleIColumn(6, 3, part_ID);
    analysisManager->FillNtupleIColumn(6, 4, StepNumberr);
    analysisManager->FillNtupleDColumn(6, 5, posParticle[0] / mm);
    analysisManager->FillNtupleDColumn(6, 6, posParticle[1] / mm);
    analysisManager->FillNtupleDColumn(6, 7, posParticle[2] / mm);
    analysisManager->FillNtupleDColumn(
        6, 8, aStep->GetPreStepPoint()->GetKineticEnergy() / MeV);
    analysisManager->FillNtupleSColumn(6, 9, interactionType);
    analysisManager->FillNtupleSColumn(6, 10, targetIsotope);
    analysisManager->FillNtupleSColumn(6, 11, creatorProcessName);
    analysisManager->FillNtupleSColumn(6, 12, PVatVertexname);

    analysisManager->AddNtupleRow(6);
  }
  // ###############################################################################################//
  // Print the particles step information
  // #############################################################################################//
  G4double postKineticEnergy = aStep->GetPostStepPoint()->GetKineticEnergy();
  G4double preKineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

  G4double EDifference = (postKineticEnergy - preKineticEnergy) / CLHEP::MeV;

  G4int print_step_info = 0;
  if (print_step_info) {
    std::cout << "Event Number: " << evt << std::endl;
    std::cout << "Particle: " << fParticleName << std::endl;
    std::cout << "Particle ID: " << part_ID << std::endl;
    std::cout << "Particle Parent ID:  " << part_parent_ID << std::endl;
    std::cout << "Step No: " << StepNumberr << std::endl;
    std::cout << "Interaction Type: " << interactionType << std::endl;
    std::cout << "Target Isotope: " << targetIsotope << std::endl;
    std::cout << "Creator Process: " << creatorProcessName << std::endl;
    std::cout << "Edep: " << edepStep << std::endl;
    std::cout << "Ekin_post - Ekin_pre: " << EDifference << std::endl;
    std::cout << "Int. Lenght (mm): " << aStep->GetStepLength() / CLHEP::mm
              << std::endl;
    std::cout << "Prevoius Volume: " << thePrePVname << std::endl;
    std::cout << "Current Volume:  " << thePostPVname << std::endl;
    std::cout << " Vertx Vol:  " << PVatVertexname << std::endl;
    std::cout << std::endl;
  }
  // #############################################################################################//

  //  // If no energy deposit, return1
  if (edepStep <= 0.)
    return;
  fEventAction->AddEdep(edepStep);
  //-------------------------------------------------------------------------//

  // Save the eergy deposition data in the silicon slabs
  if (save_silicon_data == 1 && aStep->GetTrack()->GetNextVolume()) {

    // // Stopping Power from input Table.

    // Get the material at the pre-step point
    G4Material *prematerial = thePrePoint->GetMaterial();
    // Get the material at the post-step point
    G4Material *postmaterial = thePostPoint->GetMaterial();
    //   G4cout << "Next volume is  nullptr!" << G4endl;
    //   return;
    // }
    // You can now use the material object, for example, to get its name
    G4String materialName = postmaterial->GetName();
    G4double density = postmaterial->GetDensity() / (g / cm3);

    // Get step length
    G4double stepLength = aStep->GetStepLength() / cm2;
    // G4cout << "Step length: " << stepLength << " mm" << G4endl;

    // Kinetic energy of the particle after each step
    G4double kinEnergy = theTrack->GetKineticEnergy() * MeV;

    G4double postKineticEnergy =
        aStep->GetPostStepPoint()->GetKineticEnergy() * MeV;
    G4double preKineticEnergy =
        aStep->GetPreStepPoint()->GetKineticEnergy() * MeV;

    // current step number
    G4int StepNumber = aStep->GetTrack()->GetCurrentStepNumber();

    G4EmCalculator emCalculator;
    G4double dEdxTable = 0., dEdxFull = 0.;

    if (particleType->GetPDGCharge() != 0.) {
      dEdxTable =
          emCalculator.GetDEDX(preKineticEnergy, particleType, postmaterial);
      dEdxFull = emCalculator.ComputeTotalDEDX(preKineticEnergy, particleType,
                                               postmaterial);
    }
    G4double stopTable = dEdxTable / density;
    G4double stopFull = dEdxFull / density;

    // Stopping Power from simulation.
    //
    G4double meandEdx = edepStep / stepLength;
    G4double stopPower = meandEdx / density;

    // Deposition in the silicon slabs
    //********************************************************************
    if ((stepLength != 0) && thePostPVname == "physiSlab_AlongY_1") {
      analysisManager->FillNtupleIColumn(2, 0, evt);
      analysisManager->FillNtupleSColumn(2, 1, fParticleName);
      analysisManager->FillNtupleIColumn(2, 2, part_parent_ID);
      analysisManager->FillNtupleIColumn(2, 3, part_ID);
      analysisManager->FillNtupleIColumn(2, 4, StepNumber);
      analysisManager->FillNtupleDColumn(2, 5, posParticle[0] / mm);
      analysisManager->FillNtupleDColumn(2, 6, posParticle[1] / mm);
      analysisManager->FillNtupleDColumn(2, 7, posParticle[2] / mm);
      analysisManager->FillNtupleSColumn(2, 8, interactionType);
      analysisManager->FillNtupleSColumn(2, 9, targetIsotope);
      analysisManager->FillNtupleDColumn(2, 10, edepStep / MeV);
      analysisManager->FillNtupleDColumn(
          2, 11, stopTable / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleDColumn(
          2, 12, stopFull / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleDColumn(2, 13,
                                         meandEdx / (CLHEP::MeV / CLHEP::cm));
      analysisManager->FillNtupleDColumn(
          2, 14, stopPower / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleSColumn(2, 15, creatorProcessName);
      analysisManager->FillNtupleSColumn(2, 16, PVatVertexname);

      analysisManager->AddNtupleRow(2);
    }
    //******************************************************************** */

    if ((stepLength != 0) && thePostPVname == "physiSlab_AlongY_2") {
      analysisManager->FillNtupleIColumn(3, 0, evt);
      analysisManager->FillNtupleSColumn(3, 1, fParticleName);
      analysisManager->FillNtupleIColumn(3, 2, part_parent_ID);
      analysisManager->FillNtupleIColumn(3, 3, part_ID);
      analysisManager->FillNtupleIColumn(3, 4, StepNumber);
      analysisManager->FillNtupleDColumn(3, 5, posParticle[0] / mm);
      analysisManager->FillNtupleDColumn(3, 6, posParticle[1] / mm);
      analysisManager->FillNtupleDColumn(3, 7, posParticle[2] / mm);
      analysisManager->FillNtupleSColumn(3, 8, interactionType);
      analysisManager->FillNtupleSColumn(3, 9, targetIsotope);
      analysisManager->FillNtupleDColumn(3, 10, edepStep / MeV);
      analysisManager->FillNtupleDColumn(
          3, 11, stopTable / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleDColumn(
          3, 12, stopFull / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleDColumn(3, 13,
                                         meandEdx / (CLHEP::MeV / CLHEP::cm));
      analysisManager->FillNtupleDColumn(
          3, 14, stopPower / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleSColumn(3, 15, creatorProcessName);
      analysisManager->FillNtupleSColumn(3, 16, creatorProcessName);

      analysisManager->AddNtupleRow(3);
    }
    //******************************************************************** */

    if ((stepLength != 0) && thePostPVname == "physiSlab_AlongZ_1") {
      analysisManager->FillNtupleIColumn(4, 0, evt);
      analysisManager->FillNtupleSColumn(4, 1, fParticleName);
      analysisManager->FillNtupleIColumn(4, 2, part_parent_ID);
      analysisManager->FillNtupleIColumn(4, 3, part_ID);
      analysisManager->FillNtupleIColumn(4, 4, StepNumber);
      analysisManager->FillNtupleDColumn(4, 5, posParticle[0] / mm);
      analysisManager->FillNtupleDColumn(4, 6, posParticle[1] / mm);
      analysisManager->FillNtupleDColumn(4, 7, posParticle[2] / mm);
      analysisManager->FillNtupleSColumn(4, 8, interactionType);
      analysisManager->FillNtupleSColumn(4, 9, targetIsotope);
      analysisManager->FillNtupleDColumn(4, 10, edepStep / MeV);
      analysisManager->FillNtupleDColumn(
          4, 11, stopTable / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleDColumn(
          4, 12, stopFull / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleDColumn(4, 13,
                                         meandEdx / (CLHEP::MeV / CLHEP::cm));
      analysisManager->FillNtupleDColumn(
          4, 14, stopPower / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleSColumn(4, 15, creatorProcessName);
      analysisManager->FillNtupleSColumn(4, 16, creatorProcessName);

      analysisManager->AddNtupleRow(4);
    }
    //******************************************************************** */

    if ((stepLength != 0) && thePostPVname == "physiSlab_AlongZ_2") {
      analysisManager->FillNtupleIColumn(5, 0, evt);
      analysisManager->FillNtupleSColumn(5, 1, fParticleName);
      analysisManager->FillNtupleIColumn(5, 2, part_parent_ID);
      analysisManager->FillNtupleIColumn(5, 3, part_ID);
      analysisManager->FillNtupleIColumn(5, 4, StepNumber);
      analysisManager->FillNtupleDColumn(5, 5, posParticle[0] / mm);
      analysisManager->FillNtupleDColumn(5, 6, posParticle[1] / mm);
      analysisManager->FillNtupleDColumn(5, 7, posParticle[2] / mm);
      analysisManager->FillNtupleSColumn(5, 8, interactionType);
      analysisManager->FillNtupleSColumn(5, 9, targetIsotope);
      analysisManager->FillNtupleDColumn(5, 10, edepStep / MeV);
      analysisManager->FillNtupleDColumn(
          5, 11, stopTable / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleDColumn(
          5, 12, stopFull / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleDColumn(5, 13,
                                         meandEdx / (CLHEP::MeV / CLHEP::cm));
      analysisManager->FillNtupleDColumn(
          5, 14, stopPower / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g));
      analysisManager->FillNtupleSColumn(5, 15, creatorProcessName);
      analysisManager->FillNtupleSColumn(5, 16, creatorProcessName);

      analysisManager->AddNtupleRow(5);
    }
    // if (meandEdx != 0 && (thePostPVname == "physiSlab_AlongZ_1" ||
    //                       thePostPVname == "physiSlab_AlongY_1" ||
    //                       thePostPVname == "physiSlab_AlongX_1" ||
    //                       thePostPVname == "physiSlab_AlongY_2" ||
    //                       thePostPVname == "physiSlab_AlongX_2" ||
    //                       thePostPVname == "physiSlab_AlongZ_2")) {
    //   std::cout << "Event: " << evt << "\nStep: " << StepNumber
    //             << "\ncreatorProcessName: " << creatorProcessName
    //             << "\npreProcessName: " << preProcessName
    //             << "\npostProcessName: " << postProcessName
    //             << "\nParticle: " << fParticleName
    //             << "\nParentID: " << part_parent_ID
    //             << "\nParticleID: " << part_ID << "\nPos: " <<
    // posParticle[0]
    //             << "  " << posParticle[1] << " " << posParticle[2]
    //             << "\nIntType: " << interactionType
    //             << "\nTarget: " << targetIsotope
    //             << "\nKinEn (MeV): " << kinEnergy
    //             << "\npreKineticEnergy (MeV): " << preKineticEnergy
    //             << "\npostKineticEnergy (MeV): " << postKineticEnergy
    //             << "\nEdep (MeV): " << aStep->GetTotalEnergyDeposit()
    //             << "\nS_Lenght(mm): " << aStep->GetStepLength() /
    // CLHEP::mm
    //             << "\nPre_vol: " << thePrePVname
    //             << "\nPost_Vol: " << thePostPVname << std::endl;
    //   std::cout << std::endl;

    //   G4cout << "Mean dE/dx = " << meandEdx / (CLHEP::MeV / CLHEP::cm)
    //          << " MeV/cm" << "\t("
    //          << stopPower / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g)
    //          << " MeV*cm2/g)" << G4endl;

    //   G4cout << "\n From formulas :" << G4endl;
    //   G4cout << "   restricted dEdx = " << dEdxTable / (CLHEP::MeV /
    //   CLHEP::cm)
    //          << " MeV/cm" << "\t("
    //          << stopTable / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g)
    //          << " MeV*cm2/g)" << G4endl;

    //   G4cout << "   full dEdx       = " << dEdxFull / (CLHEP::MeV /
    //   CLHEP::cm)
    //          << " MeV/cm" << "\t("
    //          << stopFull / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g) << "
    //          MeV*cm2/g)"
    //          << G4endl;
    //   G4cout << "\n" << G4endl;
    //   G4cout << "\n" << G4endl;
    // }
  }
  //-------------------------------------------------------------------------//

  // G4cout << fParticleName << " " << part_parent_ID << "  " << part_ID
  //        << " -----> Mean dE/dx = " << meandEdx / (MeV / cm) << " MeV/cm"
  //        << "\t(" << stopPower / (MeV * cm2 / g) << " MeV*cm2/g)" << G4endl;

  // G4cout << fParticleName << " " << part_parent_ID << "  " << part_ID << "  "
  //        << StepNumber << "  " << materialName
  //        << "\n From formulas :" << G4endl;
  // G4cout << "   restricted dEdx = " << dEdxTable / (CLHEP::MeV / CLHEP::cm)
  //        << " MeV/cm" << "\t("
  //        << stopTable / (CLHEP::MeV * CLHEP::cm2 / CLHEP::g) << " MeV*cm2/g)"
  //        << G4endl;

  // std::cout << evt << ": Particle: " << fParticleName << "  " <<
  // part_parent_ID
  //           << "  " << part_ID << "  " << posParticle[0] << "  "
  //           << posParticle[1] << " " << posParticle[2] << "  "
  //           << interactionType << "  " << targetIsotope << "  "
  //           << aStep->GetTotalEnergyDeposit() / CLHEP::keV << "  "
  //           << "step Lenght: " << aStep->GetStepLength() << "  " <<
  //           thePrePVname << " "
  //           << thePostPVname << std::endl;
  // std::cout << std::endl;
}

//*********************************************************************************//
// Set the
void SteppingAction::SaveSiliconEdepData(G4int val) {
  // change the transverse size
  // if the value is less than zero
  if (val < 0) {
    G4cout << "\n --->warning from Save the silicon data: value should be "
              "greater or equal to zero - Value: "
           << val << " is out of range. Command refused" << G4endl;
    return;
  }
  save_silicon_data = val;
}
void SteppingAction::SaveParticleFluxData(G4int val) {
  // change the transverse size
  // if the value is less than zero
  if (val < 0) {
    G4cout << "\n --->warning from save the flux data: value should be "
              "greater or equal to zero - Value: "
           << val << " is out of range. Command refused" << G4endl;
    return;
  }
  save_flux_data = val;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
