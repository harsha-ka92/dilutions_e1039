/// Fun4SimTree.C:  Macro to analyze the simulated tree created by Fun4SimMicroDst.C.
R__LOAD_LIBRARY(libana_sim_dst)

#include <TVector3.h>
#include <ktracker/SRecEvent.h>
#include <ktracker/SRawEvent.h>
#include <UtilAna/UtilDimuon.h>

using namespace std;
const char* TYPE_GMC = "J/#psi E906Gen"; // D-Y, J/#psi, E906Gen, PYTHIA, etc.
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
//"weight" removed from all histogram projections when working with e906 legacy generated MC
void Fun4SimTree(const char* fname="sim_tree_target_no_cham_res.root", const char* tname="tree")
{
	file = new TFile(fname);
	tree = (TTree*)file->Get(tname);
	gSystem->Exec("rm -rf result_jpsi_shift");
	gSystem->mkdir("result_jpsi_shift", true);
	c1 = new TCanvas("c1", "");
	c1->SetGrid();
	gStyle->SetOptStat(1);
	gStyle->SetLegendFont(12);
	gStyle->SetTitleFont(12);
	//c1->SetLogy(true);

	inte_lumi = GetInteLumi();
	cout << "Integrated luminosity = " << inte_lumi << endl;

	tree->Draw("rec_stat", "weight"); // cf. GlobalConsts.h.
	c1->SaveAs("result_jpsi_shift/h1_rec_stat.png");


	/// You can use these functions or add new ones.
	DrawDimTrueKin();
	DrawDimRecoKin();
	//DrawDimTrkTrueKin();
	DrawTrkTrueKin();
	FitCosTheta();
	//AnaEvents();

	exit(0);
}

///
/// Functions below
///
void DrawDimTrueKin()
{
  tree->Draw("n_dim_true");
  c1->SaveAs("result_jpsi_shift/h1_true_n_dim.png");

  const double PI = TMath::Pi();
  DrawTrueVar("dim_true.pdg_id"    , "True dimuon PDG ID", 1000, 0, 0);
  DrawTrueVar("dim_true.mom.X()"   , "True dimuon px (GeV)", 100, -5,   5);
  DrawTrueVar("dim_true.mom.Y()"   , "True dimuon py (GeV)", 100, -5,   5);
  DrawTrueVar("dim_true.mom.Z()"   , "True dimuon pz (GeV)", 100,  0, 100);
  DrawTrueVar("dim_true.mass"   , "True dimuon mass (GeV)", 100, 1.5, 5.5);
  DrawTrueVar("dim_true.mom.Eta()" , "True dimuon #eta", 110, 0, 11);
  DrawTrueVar("dim_true.mom.Phi()" , "True dimuon #phi", 100, -PI, PI);
  DrawTrueVar("dim_true.pos.Z()" , "True dimuon Z", 126, 0., 350.);
  DrawTrueVar("dim_true.x1"        , "True x1", 50, 0, 1);
  DrawTrueVar("dim_true.x2"        , "True x2", 50, 0, 1);
  DrawTrueVar("dim_true.xF"        , "True xF", 50, -1, 1);
  DrawTrueVar("dim_true.costh"     , "True cos#theta", 50, -1, 1);
  DrawTrueVar("dim_true.phi"       , "True #phi"     , 50, -PI, PI);
}

void DrawDimRecoKin()
{
  tree->Draw("n_dim_reco", "weight");
  c1->SaveAs("result_jpsi_shift/h1_reco_n_dim.png");

  tree->Draw("rec_stat", "weight"); // cf. GlobalConsts.h.
  c1->SaveAs("result_jpsi_shift/h1_rec_stat.png");
  
  tree->Draw("trig_bits", "weight * (rec_stat==0)");
  c1->SaveAs("result_jpsi_shift/h1_trig_bits.png");

  tree->Draw("dim_reco.mass", "weight * (rec_stat==0)");
  c1->SaveAs("result_jpsi_shift/h1_dim_reco_mass.png");
 
  tree->Draw("dim_reco.pos.Z()", "weight * (rec_stat==0)");
  c1->SaveAs("result_jpsi_shift/h1_dim_reco_pos_z.png");

  tree->Draw("dim_reco.x1", "weight * (rec_stat==0)");
  c1->SaveAs("result_jpsi_shift/h1_dim_reco_x1.png");

  tree->Draw("dim_reco.x2", "weight * (rec_stat==0)");
  c1->SaveAs("result_jpsi_shift/h1_dim_reco_x2.png");
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
  c1->SaveAs("result_jpsi_shift/h1_trk_true_pz_asym.png");
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
  c1->SaveAs("result_jpsi_shift/h1_trk_true_charge.png");
}

void DrawTrueVar(const string varname, const string title_x, const int n_x, const double x_lo, const double x_hi)
{
  TH1* h1_all = new TH1D("h1_all", "", n_x, x_lo, x_hi);
  TH1* h1_rec = new TH1D("h1_rec", "", n_x, x_lo, x_hi);
  //tree->Project("h1_all", varname.c_str(), "weight");
  tree->Project("h1_rec", varname.c_str(), "weight * (rec_stat==0)");

  //tree->Project("h1_all", varname.c_str(), "");
  //tree->Project("h1_rec", varname.c_str(), "rec_stat==0");

  ostringstream oss;
  oss << TYPE_GMC << title_x << ";"<<varname.c_str()<<";Yield";
  h1_rec->SetTitle(oss.str().c_str());
  //THStack hs("hs", oss.str().c_str());
  //hs.Add(h1_all);
  //hs.Add(h1_rec);
  h1_rec->SetLineColor(kRed);
  //hs.Draw("nostackb");
  h1_rec->Draw("HIST");
  oss.str("");
  oss << "result_jpsi_shift/h1_";
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
  //int weight =1;
  gStyle->SetOptFit(true);
  TH1* mass = new TH1D("mass", "", 100, 1., 5.5);
  TH1* mass_res = new TH1D("mass_res", "", 100,-0.5, 0.5);
  tree->Project("mass", "dim_reco.mom.M()", "weight*(rec_stat==0 && dim_reco.mom.M()>0)");
  tree->Project("mass_res", "(dim_reco.mom.M()-dim_true.mom.M())/dim_true.mom.M()", "weight*(rec_stat==0 && dim_reco.mom.M()>0)");
 
  //tree->Project("mass", "dim_reco.mass", "weight*(rec_stat==0 && dim_reco.mass>0)");
  //tree->Project("mass_res", "(dim_reco.mass-dim_true.mass)/dim_true.mass", "weight*(rec_stat==0 && dim_reco.mass>0)"); 

  //tree->Project("mass", "dim_reco.mom.M()", "weight*(rec_stat==0 && dim_reco.mom.M()>0 && dim_true.pos.Z()> 0)");
  //tree->Project("mass_res", "(dim_reco.mom.M()-dim_true.mom.M())/dim_true.mom.M()", "weight*(rec_stat==0 && dim_reco.mom.M()>0 && dim_true.pos.Z()> 0)");
  //tree->Project("mass", "dim_reco.mass", "weight*(rec_stat==0 && dim_reco.mass>0 && dim_true.pos.Z()> 0)");
  //tree->Project("mass_res", "(dim_reco.mass-dim_true.mass)/dim_true.mass", "weight*(rec_stat==0 && dim_reco.mass>0 && dim_true.pos.Z()> 0)");

  //tree->Project("mass", "dim_reco.mom.M()", "weight*(rec_stat==0 && dim_reco.mom.M()>0 && dim_true.pos.Z()> -304.5 && dim_true.pos.Z()<-295.5)");
  //tree->Project("mass_res", "(dim_reco.mom.M()-dim_true.mom.M())/dim_true.mom.M()", "weight*(rec_stat==0 && dim_reco.mom.M()>0 && dim_true.pos.Z()> -304.5 && dim_true.pos.Z()<-295.5)");
  //tree->Project("mass", "dim_reco.mass", "weight*(rec_stat==0 && dim_reco.mass>0 && dim_true.pos.Z()> -304.5 && dim_true.pos.Z()<-295.5)");
  //tree->Project("mass_res", "(dim_reco.mass-dim_true.mass)/dim_true.mass", "weight*(rec_stat==0 && dim_reco.mass>0 && dim_true.pos.Z()> -304.5 && dim_true.pos.Z()<-295.5)");
  
  
  mass->Scale(1/inte_lumi);
  TF1* fb = new TF1("fb","gaus(0)",1.5,5.5);
  mass->Fit("fb","","", 2, 4.5);
  //fb->SetParameters(h1_costh->Integral()/h1_costh->GetNbinsX(), 1.0);
  mass->Fit(fb, "REM");

  ostringstream oss;
  oss << TYPE_GMC << " ;reco_mass;Yield/Int: Luminosity";
  mass->SetTitle(oss.str().c_str());
  c1->SaveAs("result_jpsi_shift/h1_peak_fit.png");
  delete fb;
  TF1* fb2 = new TF1("fb2","gaus(0)",-0.5, 0.5);
  mass_res->Fit("fb2","","", -0.5, 0.5);
  mass_res->Fit(fb2, "REM");
  c1->Update();
  oss.str("");
  oss << TYPE_GMC << " ;#frac{(M_{rec}-M_{true})}{M_{true}};Yield";
  mass_res->SetTitle(oss.str().c_str());
  c1->SaveAs("result_jpsi_shift/h1_res_peak_fit.png");
  delete fb2;
  delete mass_res;
  delete mass;
}

//void AnaEvents()
//{
//  typedef map<int, int> IntCount_t;
//  IntCount_t id_cnt;
//  DimuonList* list_dim = new DimuonList();
//  tree->SetBranchAddress("dim_true", &list_dim);
//
//  int n_ent = tree->GetEntries();
//  cout << "AnaEvents(): n = " << n_ent << endl;
//  for (int i_ent = 0; i_ent < n_ent; i_ent++) {
//    if ((i_ent+1) % (n_ent/10) == 0) cout << "  " << 100*(i_ent+1)/n_ent << "%" << flush;
//    tree->GetEntry(i_ent);
//    for (DimuonList::iterator it = list_dim->begin(); it != list_dim->end(); it++) {
//      DimuonData* dd = &(*it);
//      int pdg_id = dd->pdg_id;
//      if (id_cnt.find(pdg_id) == id_cnt.end()) id_cnt[pdg_id] = 1;
//      else                                     id_cnt[pdg_id]++;
//    }
//  }
//  cout << endl;
//  for (IntCount_t::iterator it = id_cnt.begin(); it != id_cnt.end(); it++) {
//    cout << setw(10) << it->first << "  " << setw(10) << it->second << endl;
//  }
//}

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
