/// Fun4SimTree.C:  Macro to analyze the simulated tree created by Fun4SimMicroDst.C.
R__LOAD_LIBRARY(libana_sim_dst)

#include <TVector3.h>
#include <ktracker/SRecEvent.h>
#include <ktracker/SRawEvent.h>

using namespace std;
const char* TYPE_GMC = "J/#Psi PYTHIA"; // D-Y, J/#psi, E906Gen, PYTHIA, etc.
TFile* file;
TTree* tree;
TCanvas* c1;
double inte_lumi;
void DrawDimTrueKin();
void DrawDimRecoKin();
void DrawDimTrkTrueKin();
void DrawTrkTrueKin();
void DrawTrueVar(const string varname, const string title_x, const int n_x, const double x_lo, const double x_hi);
void FitCosTheta();
void AnaEvents();
double GetInteLumi(const char* fn_lumi="lumi_tot.txt");
int dim_origin(double pos_chisq_t, double pos_chisq_d, double pos_chisq_us, double neg_chisq_t, double neg_chisq_d, double neg_chisq_us, double z1, double z2);

void Fun4SimTree(const char* fname="sim_tree.root", const char* tname="tree")
{
	file = new TFile(fname);
	tree = (TTree*)file->Get(tname);
	gSystem->Exec("rm -rf result_sim");
	gSystem->mkdir("result_sim", true);
	c1 = new TCanvas("c1", "");
	c1->SetGrid();
	gStyle->SetOptStat(1);
	gStyle->SetLegendFont(12);
	gStyle->SetTitleFont(12);
	//c1->SetLogy(true);

	inte_lumi = GetInteLumi();
	cout << "Integrated luminosity = " << inte_lumi << endl;

	tree->Draw("rec_stat", "weight"); // cf. GlobalConsts.h.
	c1->SaveAs("result_sim/h1_rec_stat.png");

	//seperating the reco_z positions based on true_z
	//
	int nEntries, rec_stat;
	double z_true, z_reco;
	double weight;
	nEntries = 0;
	rec_stat = -9999;
	z_true = z_reco = 0;
	DimuonList*  dim_reco=0;
	DimuonList*  dim_true=0;
	tree->SetBranchAddress("dim_reco", &dim_reco);
	tree->SetBranchAddress("dim_true", &dim_true);
	tree->SetBranchAddress("weight", &weight);
	tree->SetBranchAddress("rec_stat", &rec_stat);

	int nbins = 100;
	double x_min = -720;
	TH1* h_ti1_dims = new TH1D("h_ti1_dims", "", nbins, x_min, -1);
	TH1* h_ti2_dims = new TH1D("h_ti2_dims", "", nbins, x_min,-10);
	TH1* h_ti3_dims = new TH1D("h_ti3_dims", "", nbins, x_min, -10);
	TH1* h_al77k1_dims = new TH1D("h_al77k1_dims", "", nbins, x_min, -10);
	TH1* h_al77k2_dims = new TH1D("h_al77k2_dims", "", nbins, x_min, -10);
	TH1* h_al1_dims = new TH1D("h_al1_dims", "", nbins, x_min, -10);
	TH1* h_al2_dims = new TH1D("h_al2_dims", "", nbins, x_min, -10);
	TH1* h_fridge_dims = new TH1D("h_fridge_dims", "", nbins, x_min, -10);
	TH1* h_coli_dims = new TH1D("h_coli_dims", "", nbins, x_min, -10);
	TH1* h_r_all_dims = new TH1D("h_r_all_dims", "", nbins, x_min, -10);
	TH1* h_t_all_dims = new TH1D("h_t_all_dims", "", nbins, x_min, -20);
	double n_all, n_fridge, n_ti1, n_ti2, n_ti3, n_al77k1, n_al77k2, n_al1, n_al2, n_coli;
	n_all = n_fridge = n_ti1 = n_ti2 = n_ti3 = n_al77k1 = n_al77k2 = n_al1 = n_al2 = n_coli = 0.;

	nEntries = tree->GetEntries();

	DimuonData dt;
	DimuonData dr;
	bool target_like = true;
	double Z;
	for(int i = 0; i<nEntries; ++i){

		tree->GetEntry(i);
		for (int j = 0; j < dim_true ->size(); ++j){
			dt = dim_true->at(j);
			dr = dim_reco->at(j);		

			Z = -1000;

			if(target_like){
				if(dim_origin(dr.chisq_target_pos, dr.chisq_dump_pos, dr.chisq_upstream_pos, dr.chisq_target_neg, dr.chisq_dump_neg, dr.chisq_upstream_neg, dr.pos_pos.Z(), dr.pos_neg.Z()) == 0) {Z = dr.pos.Z();}	
			}

			if(!target_like) Z = dr.pos.Z();

			if(Z != -1000){
				if (rec_stat ==0) { h_r_all_dims->Fill(Z); ++n_all;}

				if(dt.pos.Z() > -667. && dt.pos.Z() < -535. && rec_stat ==0) { h_coli_dims->Fill(Z, weight); ++n_coli;}

				else if(dt.pos.Z() > -468. && dt.pos.Z() < -458. && rec_stat ==0) { h_ti1_dims->Fill(Z, weight); ++n_ti1;}
				else if(dt.pos.Z() > -440. && dt.pos.Z() < -430. && rec_stat ==0) { h_ti2_dims->Fill(Z, weight); ++n_ti2;}
				else if(dt.pos.Z() > -195. && dt.pos.Z() < -185. && rec_stat ==0) { h_ti3_dims->Fill(Z, weight); ++n_ti3;}

				else if(dt.pos.Z() > -340. && dt.pos.Z() < -330. && rec_stat ==0) { h_al77k1_dims->Fill(Z, weight); ++n_al77k1;}
				else if(dt.pos.Z() > -280. && dt.pos.Z() < -270. && rec_stat ==0) { h_al77k2_dims->Fill(Z, weight); ++n_al77k2;}

				else if(dt.pos.Z() > -305.263 && dt.pos.Z() < -304.263 && rec_stat ==0) { h_al1_dims->Fill(Z, weight); ++n_al1;}
				else if(dt.pos.Z() > -295.737 && dt.pos.Z() < -294.737 && rec_stat ==0) { h_al2_dims->Fill(Z, weight); ++n_al2;}

				else if(dt.pos.Z() >= -304.263 && dt.pos.Z() <= -295.737 && rec_stat ==0) { h_fridge_dims->Fill(Z, weight); ++n_fridge;}
			}
		}
	}
	if(target_like) cout<< " total number of reconstructed target-like dimuons : "<<n_all<<endl;
	if(!target_like) cout<< " total number of reconstructed dimuons : "<<n_all<<endl;
	cout<< " # of dimuons likely to generated in :" <<endl; 
	//cout<< "\t colimator\t\t	: "<<n_coli<<" ( "<<n_coli/n_all *100 <<"% ) "<<endl;
	// cout<< "\t Ti window at 2\t\t	: "<<n_ti1<<" ( "<<n_ti1/n_all *100<<"% ) "<<endl;
	// cout<< "\t Ti window at 3\t\t	: "<<n_ti2<<" ( "<<n_ti2/n_all *100<<"% ) "<<endl;
	// cout<< "\t Ti window at 9\t\t	: "<<n_ti3<<" ( "<<n_ti3/n_all *100<<"% ) "<<endl;
	// cout<< "\t Al 77K foil at 4\t	: "<<n_al77k1<<" ( "<<n_al77k1/n_all *100<<"% ) "<<endl;
	// cout<< "\t Al 77K foil at 8\t	: "<<n_al77k2<<" ( "<<n_al77k2/n_all *100<<"% ) "<<endl;
	 cout<< "\t Al window at 5\t\t	: "<<n_al1<<" ( "<<n_al1/n_all *100<<"% ) "<<endl;
	 cout<< "\t Al window at 7\t\t	: "<<n_al2<<" ( "<<n_al2/n_all *100<<"% ) "<<endl;
	cout<< "\t fridge \t		: "<<n_fridge<<" ( "<<n_fridge/n_all *100<<"% ) "<<endl;

	THStack hs("hs", "Vertex Z (cm) distribution of reco: dimuons (true [-670.0,-180.0])");

	//hs.Add(h_r_all_dims);
	//h_r_all_dims->SetLineStyle(1);
	/*
	   hs.Add(h_ti1_dims);
	   h_ti1_dims->SetLineColorAlpha(kBlue, 1.);
	   hs.Add(h_ti2_dims);
	   h_ti2_dims->SetLineColorAlpha(kBlue-9, 1.);
	   hs.Add(h_ti3_dims);
	   h_ti3_dims->SetLineColorAlpha(kBlue-1, 1.);
	
	   hs.Add(h_al77k1_dims);
	   h_al77k1_dims->SetLineColorAlpha(kMagenta, 1.);
	   hs.Add(h_al77k2_dims);
	   h_al77k2_dims->SetLineColorAlpha(kMagenta+3, 1.);
	*/
	   hs.Add(h_al1_dims);
	   h_al1_dims->SetLineColorAlpha(kGreen, 1.);
	   hs.Add(h_al2_dims);
	   h_al2_dims->SetLineColorAlpha(kGreen+3, 1.);
	 
	//hs.Add(h_coli_dims);
	//h_coli_dims->SetLineColorAlpha(kRed, 1.);

	hs.Add(h_fridge_dims);
	h_fridge_dims->SetLineColorAlpha(kCyan, 1.);

	c1->SetFillColorAlpha(kYellow-10, 0.25);
	hs.Draw("nostack");

	TLegend* leg = new TLegend(0.15,0.5,0.4,0.9);
	//leg->AddEntry(h_coli_dims,"Colimator @ 1","l");
	//leg->AddEntry(h_ti1_dims,"Ti @ 2","l");
	//leg->AddEntry(h_ti2_dims,"Ti @ 3","l");
	//leg->AddEntry(h_ti3_dims,"Ti @ 9","l");
	//leg->AddEntry(h_al77k1_dims,"AL 77K @ 4","l");
	//leg->AddEntry(h_al77k2_dims,"AL 77K @ 8","l");
	leg->AddEntry(h_al1_dims,"Al @ 5","l");
	leg->AddEntry(h_al2_dims,"Al @ 7","l");
	leg->AddEntry(h_fridge_dims,"Fridge","l");
	//leg->AddEntry(h_r_all_dims,"All Dimuons","l");
	leg->Draw();
	c1->Update();
	c1->SaveAs("result_sim/beamline_sep.png");

	/// You can use these functions or add new ones.
	DrawDimTrueKin();
	DrawDimRecoKin();
	//DrawDimTrkTrueKin();
	DrawTrkTrueKin();
	//FitCosTheta();
	//AnaEvents();

	exit(0);
}

///
/// Functions below
///
void DrawDimTrueKin()
{
  tree->Draw("n_dim_true");
  c1->SaveAs("result_sim/h1_true_n_dim.png");

  const double PI = TMath::Pi();
  DrawTrueVar("dim_true.pdg_id"    , "True dimuon PDG ID", 1000, 0, 0);
  DrawTrueVar("dim_true.mom.X()"   , "True dimuon px (GeV)", 100, -5,   5);
  DrawTrueVar("dim_true.mom.Y()"   , "True dimuon py (GeV)", 100, -5,   5);
  DrawTrueVar("dim_true.mom.Z()"   , "True dimuon pz (GeV)", 100,  0, 100);
  DrawTrueVar("dim_true.mom.M()"   , "True dimuon mass (GeV)", 100, 0, 5);
  DrawTrueVar("dim_true.mom.Eta()" , "True dimuon #eta", 110, 0, 11);
  DrawTrueVar("dim_true.mom.Phi()" , "True dimuon #phi", 100, -PI, PI);
  DrawTrueVar("dim_true.pos.Z()" , "True dimuon Z", 126, -320, -280);
  DrawTrueVar("dim_true.x1"        , "True x1", 50, 0, 1);
  DrawTrueVar("dim_true.x2"        , "True x2", 50, 0, 1);
  DrawTrueVar("dim_true.xF"        , "True xF", 50, -1, 1);
  DrawTrueVar("dim_true.costh"     , "True cos#theta", 50, -1, 1);
  DrawTrueVar("dim_true.phi"       , "True #phi"     , 50, -PI, PI);
}

void DrawDimRecoKin()
{
  tree->Draw("n_dim_reco", "weight");
  c1->SaveAs("result_sim/h1_reco_n_dim.png");

  tree->Draw("rec_stat", "weight"); // cf. GlobalConsts.h.
  c1->SaveAs("result_sim/h1_rec_stat.png");
  
  tree->Draw("trig_bits", "weight * (rec_stat==0)");
  c1->SaveAs("result_sim/h1_trig_bits.png");

  tree->Draw("dim_reco.mom.M()", "weight * (rec_stat==0)");
  c1->SaveAs("result_sim/h1_dim_reco_mass.png");
 
  tree->Draw("dim_reco.pos.Z()", "weight * (rec_stat==0)");
  c1->SaveAs("result_sim/h1_dim_reco_pos_z.png");

  tree->Draw("dim_reco.x1", "weight * (rec_stat==0)");
  c1->SaveAs("result_sim/h1_dim_reco_x1.png");

  tree->Draw("dim_reco.x2", "weight * (rec_stat==0)");
  c1->SaveAs("result_sim/h1_dim_reco_x2.png");
}

/// Function to draw tracks of dimuons (not single tracks).
void DrawDimTrkTrueKin()
{
  DrawTrueVar("dim_true.mom_pos.X()", "True px (GeV) of mu+", 100, -5, 5);
  DrawTrueVar("dim_true.mom_pos.Y()", "True py (GeV) of mu+", 100, -5, 5);
  DrawTrueVar("dim_true.mom_pos.Z()", "True pz (GeV) of mu+", 100,  0, 100);
  DrawTrueVar("dim_true.mom_neg.X()", "True px (GeV) of mu-", 100, -5, 5);
  DrawTrueVar("dim_true.mom_neg.Y()", "True py (GeV) of mu-", 100, -5, 5);
  DrawTrueVar("dim_true.mom_neg.Z()", "True pz (GeV) of mu-", 100,  0, 100);

  THStack* hs;
  TH1* h1_all = new TH1D("h1_all", "", 100, -1, 1);
  TH1* h1_rec = new TH1D("h1_rec", "", 100, -1, 1);
  tree->Project("h1_all", "(dim_true.mom_pos.Z() - dim_true.mom_neg.Z())/(dim_true.mom_pos.Z() + dim_true.mom_neg.Z())", "weight");
  tree->Project("h1_rec", "(dim_true.mom_pos.Z() - dim_true.mom_neg.Z())/(dim_true.mom_pos.Z() + dim_true.mom_neg.Z())", "weight * (rec_stat==0)");

  ostringstream oss;
  oss << TYPE_GMC << " GMC;gpz+gpz (GeV) of tracks;N of tracks";
  hs = new THStack("hs", oss.str().c_str());
  hs->Add(h1_all);
  hs->Add(h1_rec);
  h1_rec->SetLineColor(kRed);
  hs->Draw("nostack");
  c1->SaveAs("result_sim/h1_trk_true_pz_asym.png");
}

/// Function to draw single tracks.
void DrawTrkTrueKin()
{
  DrawTrueVar("trk_true.charge"     , "True charge of single tracks"  ,   3, -1.5, 1.5);
  DrawTrueVar("trk_true.mom_vtx.X()", "True px (GeV) of single tracks", 100, -5, 5);
  DrawTrueVar("trk_true.mom_vtx.Y()", "True py (GeV) of single tracks", 100, -5, 5);
  DrawTrueVar("trk_true.mom_vtx.Z()", "True pz (GeV) of single tracks", 100,  0, 100);

  THStack* hs;
  TH1* h1_all = new TH1D("h1_all", "", 3, -1.5, 1.5);
  TH1* h1_rec = new TH1D("h1_rec", "", 3, -1.5, 1.5);
  tree->Project("h1_all", "trk_true.charge", "weight");
  tree->Project("h1_rec", "trk_true.charge", "weight * (rec_stat==0)");

  ostringstream oss;
  oss << TYPE_GMC << " GMC;True charge of single tracks;N of tracks";
  hs = new THStack("hs", oss.str().c_str());
  hs->Add(h1_all);
  hs->Add(h1_rec);
  h1_rec->SetLineColor(kRed);
  hs->Draw("nostack");
  c1->SaveAs("result_sim/h1_trk_true_charge.png");
}

void DrawTrueVar(const string varname, const string title_x, const int n_x, const double x_lo, const double x_hi)
{
  TH1* h1_all = new TH1D("h1_all", "", n_x, x_lo, x_hi);
  TH1* h1_rec = new TH1D("h1_rec", "", n_x, x_lo, x_hi);
  tree->Project("h1_all", varname.c_str(), "weight");
  tree->Project("h1_rec", varname.c_str(), "weight * (rec_stat==0)");

  ostringstream oss;
  oss << TYPE_GMC << title_x << ";Yield";
  THStack hs("hs", oss.str().c_str());
  hs.Add(h1_all);
  hs.Add(h1_rec);
  h1_rec->SetLineColor(kRed);
  hs.Draw("nostack");

  oss.str("");
  oss << "result_sim/h1_";
  for (string::const_iterator it = varname.begin(); it != varname.end(); it++) {
    switch (*it) { // modify bad chars for file name
    case '.': case '*': case '/': oss << '_'; break;
    case '(': case ')': case ' ': /* omit */ break;
    default: oss << *it;
    }
  }
  oss << ".png";
  c1->SaveAs(oss.str().c_str());

  delete h1_all;
  delete h1_rec;
}

void FitCosTheta()
{
  gStyle->SetOptFit(true);
  TH1* h1_costh = new TH1D("h1_costh", "", 100, -1, 1);
  tree->Project("h1_costh", "dim_true.costh", "weight");
  h1_costh->Scale(1/inte_lumi);
  TF1* f1 = new TF1("f1", "[0]*(1 + [1]*pow(x,2))", -0.8, 0.8);
  f1->SetParameters(h1_costh->Integral()/h1_costh->GetNbinsX(), 1.0);
  h1_costh->Fit(f1, "REM");

  ostringstream oss;
  oss << TYPE_GMC << " GMC;True cos#theta;Yield";
  h1_costh->SetTitle(oss.str().c_str());
  c1->SaveAs("result_sim/h1_costh_fit.png");
  delete f1;
  delete h1_costh;
}

void AnaEvents()
{
  typedef map<int, int> IntCount_t;
  IntCount_t id_cnt;
  DimuonList* list_dim = new DimuonList();
  tree->SetBranchAddress("dim_true", &list_dim);

  int n_ent = tree->GetEntries();
  cout << "AnaEvents(): n = " << n_ent << endl;
  for (int i_ent = 0; i_ent < n_ent; i_ent++) {
    if ((i_ent+1) % (n_ent/10) == 0) cout << "  " << 100*(i_ent+1)/n_ent << "%" << flush;
    tree->GetEntry(i_ent);
    for (DimuonList::iterator it = list_dim->begin(); it != list_dim->end(); it++) {
      DimuonData* dd = &(*it);
      int pdg_id = dd->pdg_id;
      if (id_cnt.find(pdg_id) == id_cnt.end()) id_cnt[pdg_id] = 1;
      else                                     id_cnt[pdg_id]++;
    }
  }
  cout << endl;
  for (IntCount_t::iterator it = id_cnt.begin(); it != id_cnt.end(); it++) {
    cout << setw(10) << it->first << "  " << setw(10) << it->second << endl;
  }
}

double GetInteLumi(const char* fn_lumi)
{
  ifstream ifs(fn_lumi);
  if (!ifs.is_open()) {
    cout << "GetInteLumi():  Cannot open '" << fn_lumi << "'.  Just return 1.0." << endl;
    return 1.0;
  }
  double val;
  ifs >> val;
  ifs.close();
  return val;
}

int dim_origin(double pos_chisq_t, double pos_chisq_d, double pos_chisq_us, double neg_chisq_t, double neg_chisq_d, double neg_chisq_us,  double z1, double z2){

        //double pos_chisq_t = trk_pos->getChisqTarget();
        //double pos_chisq_d = trk_pos->getChisqDump();
        //double pos_chisq_us = trk_pos->get_chsiq_upstream();

        //double neg_chisq_t = trk_neg->getChisqTarget();
        //double neg_chisq_d = trk_neg->getChisqDump();
        //double neg_chisq_us = trk_neg->get_chsiq_upstream();

        double pos_chi_td = pos_chisq_t - pos_chisq_d;
        double pos_chi_tus = pos_chisq_t - pos_chisq_us;
        double neg_chi_td = neg_chisq_t - neg_chisq_d;
        double neg_chi_tus = neg_chisq_t - neg_chisq_us;

        //
        //Target_Like:- 0
        //
        bool tgt_pos_ok = pos_chisq_t >=0 && pos_chisq_d >=0 && pos_chisq_us >=0 &&
                pos_chi_td <0 && pos_chi_tus <0;
        bool tgt_neg_ok = neg_chisq_t >=0 && neg_chisq_d >=0 && neg_chisq_us >=0 &&
                neg_chi_td <0 && neg_chi_tus <0;

        if (tgt_pos_ok && tgt_neg_ok && z1 > -690 && z2 > -690) return 0;

        //
        //Dump_Like:- 1
        //
        bool d_pos_ok = pos_chisq_t >=0. && pos_chisq_d >=0. && -(pos_chi_td)<0.;
        bool d_neg_ok = neg_chisq_t >=0. && neg_chisq_d >=0. && -(neg_chi_td)<0.;

        if (d_pos_ok && d_neg_ok) return 1;

        //
        //Upstream_Like:- 2
        //
        bool us_pos_ok = pos_chisq_t >=0. && pos_chisq_us >=0. && -(pos_chi_tus)<0.;
        bool us_neg_ok = neg_chisq_t >=0. && neg_chisq_us >=0. && -(neg_chi_tus)<0.;

        if (us_pos_ok && us_neg_ok) return 2;

        return -999;
}
