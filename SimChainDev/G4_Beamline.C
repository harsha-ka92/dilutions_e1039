/** @file
 * @brief Macro to configure the beamline objects.
 *
 * The usage is similar to `G4_SensitiveDetectors.C`.
 */
#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <g4detectors/PHG4CollimatorSubsystem.h>
#include <Geant4/G4Material.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4PhysicalConstants.hh>
#include <g4main/PHG4Detector.h>
#include <Geant4/G4Element.hh>

class SubsysReco;
R__LOAD_LIBRARY(libg4detectors)
#endif

using namespace CLHEP;

void SetupBeamline(
  PHG4Reco* g4Reco,
  const bool toggle_collimator = true,
  const double collimator_pos_z = -602.36,
  const int register_hits = 0) 
{
	if(toggle_collimator) {
		PHG4CollimatorSubsystem* collimator = new PHG4CollimatorSubsystem("Collimator",0);
		collimator->SuperDetector("Collimator");
		collimator->set_double_param("place_z", collimator_pos_z);
		collimator->set_double_param("size_z", 121.92);
		collimator->SetActive(register_hits);
		g4Reco->registerSubsystem(collimator);

	}
	const double inch = 2.54;
	
	G4double z;
	G4double a;
	G4String symbol;
	G4String name;
	G4double density;
	G4int ncomponents;
	G4int natoms;

        G4Element *eTi  = new G4Element(name="Titanium", symbol="Ti" ,  z=22., a = 47.867 *g/mole);
        G4Element *eV = new G4Element(name="Vanadium",   symbol="V" , z=23., a = 50.942*g/mole);
        G4Element *eCr  = new G4Element(name="Chromium", symbol="Cr" ,  z=24., a = 51.996 *g/mole);
        G4Element *eSn  = new G4Element(name="Tin", symbol="Sn" ,  z=50., a = 118.710 *g/mole);
        G4Element *eAl  = new G4Element(name="Aluminum", symbol="Al" ,  z=13., a = 26.982 *g/mole);
	
	G4Material* sTi = new G4Material(name = "G4_sTi",   density = 4.502 * g/cm3, ncomponents = 1);
	sTi->AddElement(eTi, natoms = 1);

	G4Material* sV = new G4Material(name = "G4_sV",   density = 6.11 * g/cm3, ncomponents = 1);
        sV->AddElement(eV, natoms = 1);
	
	G4Material* sCr = new G4Material(name = "G4_sCr",   density = 7.192 * g/cm3, ncomponents = 1);
        sCr->AddElement(eCr, natoms = 1);

	G4Material* sSn =  new G4Material(name = "G4_sSn",   density = 7.28 * g/cm3, ncomponents = 1);
        sSn->AddElement(eSn, natoms = 1);

	G4Material* sAl = new G4Material(name = "G4_sAl",   density = 2.70 * g/cm3, ncomponents = 1);
        sAl->AddElement(eAl, natoms = 1);

        //Ti 15-3 : 76% Ti + 15% V + 3% Cr + 3% Sn + 3% Al
        //rho_ti = 4.502 g/cm3 , rho_va = 6.11 g/cm3 , rho_cr = 7.192 g/cm3 , rho_sn = 7.28 g/cm3 , rho_al = 2.70 g/cm3
        //rho_ri_15_3 = 0.76 * 4.502 + 0.15*6.099 + 0.03*7.192 + 0.003*7.28 + 0.003*2.70 = 4.582 g/cm3
        G4Material* sTi_15_3 = new G4Material(name = "G4_TiAlloy",   density = 4.582*g/cm3, ncomponents = 5);
        sTi_15_3 ->AddMaterial(sTi, 76 * perCent);
        sTi_15_3 ->AddMaterial(sV, 15 * perCent);
        sTi_15_3 ->AddMaterial(sCr, 3 * perCent);
        sTi_15_3 ->AddMaterial(sSn, 3 * perCent);
        sTi_15_3 ->AddMaterial(sAl, 3 * perCent);
	
	//three ti_ beam windows upstream the target
	//
	PHG4BlockSubsystem* ti_window1 = new PHG4BlockSubsystem("ti_window1", 0);
        ti_window1->set_double_param("place_x",   0.0); // Place and size are preliminary.  See DocDB 9732
        ti_window1->set_double_param("place_y", 0.0);
        ti_window1->set_double_param("place_z", -463.345);
        ti_window1->set_double_param("size_x", 4.*inch);
        ti_window1->set_double_param("size_y", 4.*inch);
        ti_window1->set_double_param("size_z",  0.01397*inch);
        ti_window1->set_string_param("material", "G4_TiAlloy");
        g4Reco->registerSubsystem(ti_window1);

	PHG4BlockSubsystem* ti_window2 = new PHG4BlockSubsystem("ti_window2", 0);
        ti_window2->set_double_param("place_x",   0.0); // Place and size are preliminary.  See DocDB 9732
        ti_window2->set_double_param("place_y", 0.0);
        ti_window2->set_double_param("place_z", -435.405);
        ti_window2->set_double_param("size_x", 4.*inch);
        ti_window2->set_double_param("size_y", 4.*inch);
        ti_window2->set_double_param("size_z",  0.01397*inch);
        ti_window2->set_string_param("material", "G4_TiAlloy");
        g4Reco->registerSubsystem(ti_window2);

	PHG4BlockSubsystem* ti_window3 = new PHG4BlockSubsystem("ti_window3", 0);
	ti_window3->set_double_param("place_x",   0.0); // Place and size are preliminary.  See DocDB 9732
        ti_window3->set_double_param("place_y", 0.0);
        ti_window3->set_double_param("place_z", -191.50);
        ti_window3->set_double_param("size_x", 4.*inch);
        ti_window3->set_double_param("size_y", 4.*inch);
        ti_window3->set_double_param("size_z",  0.01397*inch);
        ti_window3->set_string_param("material", "G4_TiAlloy");
        g4Reco->registerSubsystem(ti_window3);


	//two Al 77K foil : thickness 0.08 cm
	//
	PHG4BlockSubsystem* al_foil1 = new PHG4BlockSubsystem("al_foil1", 0);
	al_foil1->set_double_param("place_x", 0.0); // Place and size are preliminary.  See DocDB 9732
        al_foil1->set_double_param("place_y", 0.0);
        al_foil1->set_double_param("place_z", -335.217);
        al_foil1->set_double_param("size_x", 4.*inch);
        al_foil1->set_double_param("size_y", 4.*inch);
        al_foil1->set_double_param("size_z",  0.0315*inch);
        al_foil1->set_string_param("material", "G4_sAl");
        g4Reco->registerSubsystem(al_foil1);

	PHG4BlockSubsystem* al_foil2 = new PHG4BlockSubsystem("al_foil2", 0);
        al_foil2->set_double_param("place_x", 0.0); // Place and size are preliminary.  See DocDB 9732
        al_foil2->set_double_param("place_y", 0.0);
        al_foil2->set_double_param("place_z", -274.783);
        al_foil2->set_double_param("size_x", 4.*inch);
        al_foil2->set_double_param("size_y", 4.*inch);
        al_foil2->set_double_param("size_z",  0.0315*inch);
        al_foil2->set_string_param("material", "G4_sAl");
        g4Reco->registerSubsystem(al_foil2);

	//two Al windows in the fridge
	//
	PHG4BlockSubsystem* al_window1 = new PHG4BlockSubsystem("al_window1", 0);
        al_window1->set_double_param("place_x", 0.0); // Place and size are preliminary.  See DocDB 9732
        al_window1->set_double_param("place_y", 0.0);
        al_window1->set_double_param("place_z", -304.763);
        al_window1->set_double_param("size_x", 4.*inch);
        al_window1->set_double_param("size_y", 4.*inch);
        al_window1->set_double_param("size_z",  0.00984*inch);
        al_window1->set_string_param("material", "G4_sAl");
        g4Reco->registerSubsystem(al_window1);

	PHG4BlockSubsystem* al_window2 = new PHG4BlockSubsystem("al_window2", 0);
        al_window2->set_double_param("place_x", 0.0); // Place and size are preliminary.  See DocDB 9732
        al_window2->set_double_param("place_y", 0.0);
        al_window2->set_double_param("place_z", -295.237);
        al_window2->set_double_param("size_x", 4.*inch);
        al_window2->set_double_param("size_y", 4.*inch);
        al_window2->set_double_param("size_z",  0.00984*inch);
        al_window2->set_string_param("material", "G4_sAl");
        g4Reco->registerSubsystem(al_window2);	
	
	//two Al endcaps of the target
	//
	
	return;
}
