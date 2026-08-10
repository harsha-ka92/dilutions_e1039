R__LOAD_LIBRARY(ktracker)
R__LOAD_LIBRARY(sqgenfit)

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TH1D.h>
#include <TMultiGraph.h>
#include <ktracker/SRecEvent.h>
#include <ktracker/SRawEvent.h>
#include <TLorentzVector.h>

using namespace std;

TFile *f_file;
TTree *_mixed;
TTree *_sorted;
TTree *_ana;

double cal_phiS(double px, double py);
int dim_origin(SRecTrack* trk_pos, SRecTrack* trk_neg, double z1, double z2);


void plots(const string list_run_ana)
{   
	gSystem->Exec("rm -rf plots");
	gSystem->mkdir("plots", 1);
	gStyle->SetOptStat(0);
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);

	ostringstream oss;	
	oss<<"Dimuon Mass Spectra (For "<<n_runs<<" runs between : "<<run_id_low<<" - "<<run_id_high<<")";
	TH1D* m_sorted = new TH1D("m_sorted",oss.str().c_str(), 90 , 0, 9);
	TH1D* m_mixed = new TH1D("m_mixed","m_mixed", 90 , 0, 9);

	oss.str("");
	oss<<"Target-Like Dimuon Mass Spectra (For "<<n_runs<<" runs between : "<<run_id_low<<" - "<<run_id_high<<")";
	TH1D* m_sorted_target = new TH1D("m_sorted_target",oss.str().c_str(), 90 , 0, 9);
	TH1D* m_mixed_target = new TH1D("m_mixed_target","m_mixed_target", 90 , 0, 9);

	oss.str("");
	oss<<"D0 Occupancy (For "<<n_runs<<" runs between : "<<run_id_low<<" - "<<run_id_high<<")";
	TH1D* h_occuD1 = new TH1D("h_occuD1", oss.str().c_str(), 400, 0, 2000); 

	int bins = 6;
	int max_tm = 6;
	oss.str("");
	oss<<"Positive Track Multiplicity (For "<<n_runs<<" runs between : "<<run_id_low<<" - "<<run_id_high<<")";
	TH1D* h_tm_pl = new TH1D("h_tm_pl", oss.str().c_str(), bins, 0, max_tm);
	TH1D* h_tm_pm = new TH1D("h_tm_pm", oss.str().c_str(), bins, 0, max_tm);
	TH1D* h_tm_ph = new TH1D("h_tm_ph", oss.str().c_str(), bins, 0, max_tm);

	oss.str("");
	oss<<"Negative Track Multiplicity (For "<<n_runs<<" runs between : "<<run_id_low<<" - "<<run_id_high<<")";
	TH1D* h_tm_nl = new TH1D("h_tm_nl", oss.str().c_str(), bins, 0, max_tm);
	TH1D* h_tm_nm = new TH1D("h_tm_nm", oss.str().c_str(), bins, 0, max_tm);
	TH1D* h_tm_nh = new TH1D("h_tm_nh", oss.str().c_str(), bins, 0, max_tm);

	oss.str("");
	oss<<"#Phi_{#vec{q}_{T}} ( For the "<<n_runs<<" runs between : "<<run_id_low<<" - "<<run_id_high<<")";
	int x_lim = TMath::Pi(); 
	TH1D* h_phi_all = new TH1D("h_phi_all", oss.str().c_str(), 20, -x_lim, x_lim);
	TH1D* h_phi_bkg = new TH1D("h_phi_bkg", oss.str().c_str(), 20, -x_lim, x_lim);

	oss.str("");
	oss<<"Signal/BKG ratio for different track separation cuts ( For the "<<n_runs<<" runs between : "<<run_id_low<<" - "<<run_id_high<<")";
	TGraph* g_sn_ratio = new TGraph();
	g_sn_ratio->SetTitle(oss.str().c_str());

	TCanvas* c1 = new TCanvas("c1", "");
	c1->SetGrid();

	for (auto it = list_in.begin(); it != list_in.end(); it++) {
		std::cout << "Input = " << *it << std::endl;
		const string fn_in = *it;	

		f_file = new TFile(fn_in.c_str());
		if(!f_file){
			cout << "Cannot get the DST tree.  Abort." << endl;
			exit(1);
		}

		_sorted = (TTree*) f_file->Get("save_sorted");
		_mixed = (TTree*) f_file->Get("save_mix");	
		_ana = (TTree*) f_file->Get("ana_tree");

		SRecEvent* sorted_event = new SRecEvent();
		SRecEvent* mixed_event = new SRecEvent();
		SRecEvent* sorted_tl_event = new SRecEvent();
		SRecEvent* mixed_tl_event = new SRecEvent();
		int occuD1=0; int occuD0 =0; int occuD2 =0; int occuD3p =0; int occuD3m =0;
		std::vector<SRecTrack> * pos_tracks =0;
		std::vector<SRecTrack> * neg_tracks =0;
		int spill_id =0;
		double pot_run = 0;

		_sorted->SetBranchAddress("recEvent", &sorted_event);
		_sorted->SetBranchAddress("occuD1", &occuD0);
		_sorted->SetBranchAddress("occuD2", &occuD2);
		_sorted->SetBranchAddress("occuD3p", &occuD3p);
		_sorted->SetBranchAddress("occuD3m", &occuD3m);
		_ana->SetBranchAddress("occuD1", &occuD1);
		_ana->SetBranchAddress("spill_ID", &spill_id);
		_ana->SetBranchAddress("pos_tracks", &pos_tracks);
		_ana->SetBranchAddress("neg_tracks", &neg_tracks);
		_mixed->SetBranchAddress("recEvent", &mixed_event);

		int n_sorted = _sorted ->GetEntries();
		int n_mixed = _mixed ->GetEntries();
		int n_ana = _ana ->GetEntries();
		
		for (int k= 0; k < n_ana; k++){

			_ana->GetEntry(k);

			h_occuD1->Fill(occuD1);

			//positive track multiplicity
			if (occuD1 <=150) h_tm_pl->Fill(pos_tracks->size());
			else if (150 < occuD1 && occuD1 <=250) h_tm_pm->Fill(pos_tracks->size());
			else if (occuD1 >250) h_tm_ph->Fill(pos_tracks->size());

			//negative track multuolicity
			if (occuD1 <=150) h_tm_nl->Fill(neg_tracks->size());
			else if (150 < occuD1 && occuD1 <=250) h_tm_nm->Fill(neg_tracks->size());
			else if (occuD1 >250) h_tm_nh->Fill(neg_tracks->size());
		}

		for (int i= 0; i < n_sorted; i++){

			_sorted->GetEntry(i);

			for (int n_dims=0; n_dims <sorted_event->getNDimuons(); n_dims++){

				SRecDimuon s_dim = sorted_event->getDimuon(n_dims);

				SRecTrack* trk_pos = &(sorted_event->getTrack(s_dim.get_track_id_pos()));
				SRecTrack* trk_neg = &(sorted_event->getTrack(s_dim.get_track_id_neg()));

				double z1 = trk_pos->get_pos_vtx().Z();
				double z2 = trk_neg->get_pos_vtx().Z();	
				double mass = s_dim.get_mass();

				//applying the track separation cut to enhance the signal - BKG ratio
				if (fabs(z1-z2)>200) continue;

				m_sorted->Fill(mass);

				//dim_origin: target-like =0, dump-like =1, upstream-like =2
				if (dim_origin(trk_pos, trk_neg, z1, z2) == 0){
					s_dim.calcVariables(1);
					m_sorted_target->Fill(s_dim.get_mass());

					double phi_siv = 0; double px=0; double py=0;
					TLorentzVector mom = s_dim.get_mom();
					py = mom.Py();
					px = mom.Px();
					phi_siv = cal_phiS(px, py);
					//if (phi_siv < 2*TMath::Pi() || phi_siv > -2*TMath::Pi())
					if (phi_siv != -999) h_phi_all->Fill(phi_siv);

				}
			}

		}
		
		for (int j= 0; j < n_mixed; j++){

			_mixed->GetEntry(j);

			for (int n_dims=0; n_dims <mixed_event->getNDimuons(); n_dims++){

				SRecDimuon m_dim = mixed_event->getDimuon(n_dims);

				SRecTrack* m_trk_pos = &(mixed_event->getTrack(m_dim.get_track_id_pos()));
				SRecTrack* m_trk_neg = &(mixed_event->getTrack(m_dim.get_track_id_neg()));

				double z1 = m_trk_pos->get_pos_vtx().Z();
				double z2 = m_trk_neg->get_pos_vtx().Z();

				//applying the track separation cut to enhance the signal - BKG ratio
				if (fabs(z1-z2)>200) continue;

				m_mixed->Fill(m_dim.get_mass());

				//dim_origin: target-like =0, dump-like =1, upstream-like =2
				if (dim_origin(m_trk_pos, m_trk_neg, z1, z2) == 0){
					m_dim.calcVariables(1);
					m_mixed_target->Fill(m_dim.get_mass());

					double phi_siv = 0; double px=0; double py=0;
					TLorentzVector mom = m_dim.get_mom();
					py = mom.Py();
					px = mom.Px();
					phi_siv = cal_phiS(px, py);
					//if (phi_siv < 2*TMath::Pi() || phi_siv > -2*TMath::Pi()) 
					if (phi_siv != -999) h_phi_bkg->Fill(phi_siv);

				}
			}
		}
	}
	//Do you want the mass plots to be normalized to POT??
	bool scale = false;

	double tot_pots = std::accumulate(pots.begin(), pots.end(), 0.0);
	std::cout<<tot_pots<<std::endl;
	oss.str("");
	oss<<"Yield/POT ("<<tot_pots<<")";
	/////
	//plot All dimuon mass spectra
	/////
	c1->Clear();
	m_sorted->SetFillColorAlpha(kBlue-7, 0.35);
	m_sorted->GetYaxis()->SetRangeUser(-20,m_sorted->GetMaximum()+10);
	if(scale) m_sorted->GetYaxis()->SetTitle(oss.str().c_str());
	else m_sorted->GetYaxis()->SetTitle("Yield");
	m_sorted->GetXaxis()->SetTitle("Mass (GeV)");
	if(scale) m_sorted->Scale(1./tot_pots);
	m_sorted->Draw("HIST");

	c1->SaveAs("plots/mass_sorted.png");
	
	if(scale) m_mixed->Scale(1./tot_pots);

	TH1D *m_signal = new TH1D(*m_sorted); 
	m_signal->SetNameTitle("m_signal", "Mass Spectrum for Signal Dimuons;Mass (GeV);Yield");
	if (!(m_signal->GetSumw2N() > 0)) m_signal->Sumw2(kTRUE); // ensure proper error propagation
	m_signal->Add(m_mixed, -1.0);
	m_signal->SetFillColorAlpha(kGreen-9, 0.85);
	m_signal->Draw("SAME HISTE");

	//m_mixed->SetMarkerStyle(kFullCircle);
	//m_mixed->SetMarkerColor(kRed);
	m_mixed->SetFillColorAlpha(kRed-9, 0.75);
	//m_mixed->GetYaxis()->SetRangeUser(0,60);
	m_mixed->Draw("SAME HIST");

	auto legend = new TLegend(0.55,0.6,0.85,0.8);
	//legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
	legend->AddEntry(m_sorted,"m_sorted (Measured)","f");
	legend->AddEntry(m_mixed,"m_mixed (Combinatoric)","f");
	legend->AddEntry(m_signal,"Measured - Combinatoric","f");
	legend->Draw();

	c1->Update();
	c1->SaveAs("plots/dim_mass.png");

	////
	//plot D0 occupancy
	////
	//
	c1->Clear();
	h_occuD1->SetFillColorAlpha(kBlue-7, 0.35);
	h_occuD1->GetYaxis()->SetTitle("Yield");
	h_occuD1->GetXaxis()->SetTitle("D0 Occupancy");
	h_occuD1->Draw("HIST");
	c1->SaveAs("plots/occuD1.png");

	////
	//plot target-like dimuon mass spectra
	////
	c1->Clear();
	m_sorted_target->SetFillColorAlpha(kBlue-7, 0.35);
	m_sorted_target->GetYaxis()->SetRangeUser(-15,m_sorted_target->GetMaximum()+10);
	if(scale) m_sorted_target->GetYaxis()->SetTitle(oss.str().c_str());
	else m_sorted_target->GetYaxis()->SetTitle("Yield");
	m_sorted_target->GetXaxis()->SetTitle("Mass (GeV)");
	if(scale) m_sorted_target->Scale(1./tot_pots);
	m_sorted_target->Draw("HIST");

	if(scale) m_mixed_target->Scale(1./tot_pots);

	TH1D *m_signal_target = new TH1D(*m_sorted_target); // TH1F or TH1D, same as h_hit_hitsperchannel1
	m_signal_target->SetNameTitle("m_signal_target", "Mass Spectrum for Signal Dimuons;Mass (GeV);Yield");
	if (!(m_signal_target->GetSumw2N() > 0)) m_signal_target->Sumw2(kTRUE); // ensure proper error propagation
	m_signal_target->Add(m_mixed_target, -1.0);
	m_signal_target->SetFillColorAlpha(kGreen-9, 0.85);
	m_signal_target->Draw("SAME HISTE");
	
	int jpsi =0;
	for (int i = 1; i <= m_signal_target->GetNbinsX(); i++){
		double x = m_signal_target->GetBinCenter(i);
		if(x>2.7 && x < 3.5){
			jpsi += m_signal_target->GetBinContent(i);	
			cout<<"bin content : "<<m_signal_target->GetBinContent(i)<<endl;
			cout<<"error of bin_"<<i<<"(sqrt(sumw2) of the bin) : "<<m_signal_target->GetBinError(i)<<endl;
		} 
	}
	std::cout<<"# of JPsi : "<<jpsi<<std::endl;

	//m_mixed->SetMarkerStyle(kFullCircle);
	//m_mixed->SetMarkerColor(kRed);
	m_mixed_target->SetFillColorAlpha(kRed-9, 0.75);
	//m_mixed->GetYaxis()->SetRangeUser(0,60);
	m_mixed_target->Draw("SAME HIST");
	
	int bkg =0;
        for (int i = 1; i <= m_mixed_target->GetNbinsX(); i++){
                double x = m_mixed_target->GetBinCenter(i);
                if(x>2.7 && x < 3.5){
                        bkg += m_mixed_target->GetBinContent(i);
                }
        }
        std::cout<<"# of bkg : "<<bkg<<std::endl;

	auto legend_tar = new TLegend(0.55,0.6,0.85,0.8);
	//legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
	legend_tar->AddEntry(m_sorted_target,"m_sorted (Measured)","f");
	legend_tar->AddEntry(m_mixed_target,"m_mixed (Combinatoric)","f");
	legend_tar->AddEntry(m_signal_target,"Measured - Combinatoric","f");
	legend_tar->Draw();

	c1->SaveAs("plots/targetdim_mass.png");

	////
	//plot track muliplicity
	////
	TCanvas* c2 = new TCanvas("c2", "");
	c2->SetGrid();
	c2->SetLogy();

	h_tm_pl->SetLineColor(kBlue-9);
	h_tm_pl->GetYaxis()->SetTitle("Yield");
	h_tm_pl->GetYaxis()->SetRangeUser(0.2,h_tm_ph->GetMaximum()+200000);
	h_tm_pl->GetXaxis()->SetTitle("Number of Tracks / Event");
	h_tm_pl->GetXaxis()->SetRangeUser(0,8);
	h_tm_pl->SetLineWidth(3);
	h_tm_pl->Draw();
	h_tm_pm->SetLineColor(kRed-9);
	h_tm_pm->SetLineWidth(3);
	h_tm_pm->Draw("SAME");
	h_tm_ph->SetLineColor(kGreen-9);
	h_tm_ph->SetLineWidth(3);
	h_tm_ph->Draw("SAME");

	auto legend_p = new TLegend(0.55,0.6,0.85,0.8);
	//legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
	legend_p->AddEntry(h_tm_pl,"0 #leq D0 #leq 150","l");
	legend_p->AddEntry(h_tm_pm,"150 < D0 #leq 250","l");
	legend_p->AddEntry(h_tm_ph,"250 < D0","l");
	legend_p->Draw();

	c2->SaveAs("plots/pos_track_mul.png");

	c2->Clear();
	h_tm_nl->SetLineColor(kBlue-9);
	h_tm_nl->GetYaxis()->SetTitle("Yield");
	h_tm_nl->GetYaxis()->SetRangeUser(0.2,h_tm_nh->GetMaximum()+200000);
	h_tm_nl->GetXaxis()->SetTitle("Number of Tracks / Event");
	h_tm_nl->GetXaxis()->SetRangeUser(0,7);
	h_tm_nl->SetLineWidth(3);
	h_tm_nl->Draw();
	h_tm_nm->SetLineColor(kRed-9);
	h_tm_nm->SetLineWidth(3);
	h_tm_nm->Draw("SAME");
	h_tm_nh->SetLineColor(kGreen-9);
	h_tm_nh->SetLineWidth(3);
	h_tm_nh->Draw("SAME");

	auto legend_n = new TLegend(0.55,0.6,0.85,0.8);
	//legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
	legend_n->AddEntry(h_tm_nl,"0 #leq D0 #leq 150","l");
	legend_n->AddEntry(h_tm_nm,"150 < D0 #leq 250","l");
	legend_n->AddEntry(h_tm_nh,"250 < D0","l");
	legend_n->Draw();
	c2->SaveAs("plots/neg_track_mul.png");		


	/////
	//plot phi distributions
	/////
	TCanvas* c3 = new TCanvas("c3", "");
	gPad->SetBottomMargin(0.20);
	c3->Clear();
	h_phi_all->SetFillColorAlpha(kBlue-7, 0.35);
	h_phi_all->GetYaxis()->SetRangeUser(0,h_phi_all->GetMaximum()+50);
	h_phi_all->GetXaxis()->SetRangeUser(-1.2*TMath::Pi(),1.2*TMath::Pi());
	h_phi_all->GetXaxis()->SetNdivisions(530, kTRUE);
	h_phi_all->GetYaxis()->SetTitle("Yield");
	h_phi_all->GetXaxis()->SetTitle("#Phi_{#vec{q}_{T}}");
	h_phi_all->GetXaxis()->SetTitleFont(43);
	h_phi_all->GetXaxis()->SetTitleSize(25);
	h_phi_all->ClearUnderflowAndOverflow();
	h_phi_all->Draw("HIST");

	c3->SaveAs("plots/phi_sorted.png");

	TH1D *phi_signal = new TH1D(*h_phi_all); // TH1F or TH1D, same as h_hit_hitsperchannel1
	phi_signal->SetNameTitle("phi_signal", "#Phi_{#vec{q}_T} Distributions;#Phi_{#vec{q}_T};Yield");
	if (!(phi_signal->GetSumw2N() > 0)) phi_signal->Sumw2(kTRUE); // ensure proper error propagation
	phi_signal->Add(h_phi_bkg, -1.0);
	phi_signal->SetFillColorAlpha(kGreen-9, 0.85);
	phi_signal->ClearUnderflowAndOverflow();
	phi_signal->Draw("SAME HISTE");

	//m_mixed->SetMarkerStyle(kFullCircle);
	//m_mixed->SetMarkerColor(kRed);
	h_phi_bkg->SetFillColorAlpha(kRed-9, 0.75);
	h_phi_bkg->ClearUnderflowAndOverflow();
	//m_mixed->GetYaxis()->SetRangeUser(0,60);
	h_phi_bkg->Draw("SAME HIST");

	auto legend_phi = new TLegend(0.68,0.67,0.93,0.86);
	//legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
	legend_phi->AddEntry(h_phi_all,"#Phi_{#vec{q}_{T}, sorted}","f");
	legend_phi->AddEntry(m_mixed,"#Phi_{#vec{q}_{T}, mixed}","f");
	legend_phi->AddEntry(m_signal,"#Phi_{#vec{q}_{T}, signal}","f");
	legend_phi->Draw();

	c3->SaveAs("plots/phi_dists.png");

	/////
	//plot s/bkg ratio plot
	/////
	c3->Clear();
	g_sn_ratio->SetMarkerStyle(21);
	g_sn_ratio->SetMarkerColor(kRed);
	g_sn_ratio->SetLineColor(kBlue);
	g_sn_ratio->GetXaxis()->SetTitle("|z_1-z_2|");
	g_sn_ratio->GetYaxis()->SetTitle("singal/BKG");
	g_sn_ratio->Draw("ACP");
	c3->Update();
	c3->SaveAs("plots/sn_ratio.png");
}

int dim_origin(SRecTrack* trk_pos, SRecTrack* trk_neg, double z1, double z2){

	double pos_chisq_t = trk_pos->getChisqTarget();
	double pos_chisq_d = trk_pos->getChisqDump();
	double pos_chisq_us = trk_pos->get_chsiq_upstream();

	double neg_chisq_t = trk_neg->getChisqTarget();
	double neg_chisq_d = trk_neg->getChisqDump();
	double neg_chisq_us = trk_neg->get_chsiq_upstream();
	
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


double cal_phiS(double px, double py){
	//std::cout<<"px = "<<px<<", py = "<<py<<", py/px = "<<py/px<<std::endl;
	
	if (px == 0 ) {
		double phi = 0;
		if (py >0 ){
			std::cout<<"Px = 0, py >0: setting Phi to PiOver2()"<<std::endl;
			phi = TMath::PiOver2();
		}
		else if (py <0 ){
			std::cout<<"Px = 0, py <0: setting Phi to -PiOver2()"<<std::endl;
                        phi = -TMath::PiOver2();
		}
		return phi;
	}

	if (py == 0) {
		std::cout<<"Py = 0, setting Phi to zero"<<std::endl;
                double phi = 0;
		return phi;
	}

	double phi =0;
	if (px>0 && py >0) phi = TMath::ATan(fabs(py/px));
	else if (px<0 && py >0) phi = TMath::Pi() -  TMath::ATan(fabs(py/px));
	else if (px<0 && py <0)	phi = -TMath::Pi() +  TMath::ATan(fabs(py/px));
        else if (px>0 && py <0) phi = - TMath::ATan(fabs(py/px));
	else {
		std::cout<<"couldnot resolve the quadrant. setting phi = -999"<<std::endl;
		std::cout<<"px = "<<px<<", py = "<<py<<", py/px = "<<py/px<<std::endl;
		phi = -999;
		return phi;
	}

	if (phi > TMath::Pi()) phi = phi - TMath::TwoPi();
	if (phi < -TMath::Pi()) phi = phi + TMath::TwoPi();
	if (phi > TMath::Pi() || phi < -TMath::Pi()) std::cout<<"out of range phi, phi = "<<phi<<std::endl;	
	return phi;
}
