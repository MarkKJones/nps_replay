void eel108_replay(Int_t RunNumber=0, Int_t MaxEvent=0)
{

  // Get RunNumber and MaxEvent if not provided.
  if(RunNumber == 0) {
    cout << "Enter a Run Number (-1 to exit): ";
    cin >> RunNumber;
    if( RunNumber<=0 ) return;
  }
  if(MaxEvent == 0) {
    cout << "\nNumber of Events to analyze: ";
    cin >> MaxEvent;
    if(MaxEvent == 0) {
      cerr << "...Invalid entry\n";
      exit;
    }
  }

  // Create file name patterns.
  // const char* RunFileNamePattern="NPS_3crate_%d.evio.0";
  const char* RunFileNamePattern="nps_coin_%d.dat.0";
  vector<TString> pathList;
  pathList.push_back(".");
  pathList.push_back("./raw");
  pathList.push_back("./raw/../raw.copiedtotape");
  pathList.push_back("./cache");
  pathList.push_back("/net/cdaq/cdaql1data/coda/data/raw");
  const char* ROOTFileNamePattern = "ROOTfiles/nps_%d.root";
  
  // Add variables to global list.
  gHcParms->Define("gen_run_number", "Run Number", RunNumber); 
  gHcParms->AddString("g_ctp_database_filename", "DBASE/NPS/standard.database");
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);

  //Load param file to include (or not) raw fADC data in ROOTfile 
   
  // Load the Hall C style detector map 
  gHcDetectorMap = new THcDetectorMap();
  gHcDetectorMap->Load("MAPS/NPS/DETEC/pcal_nps_eel108.map");
  //gHcDetectorMap->Load("MAPS/NPS/DETEC/pcal_nps_standard.map");
  
  //Add NPS spectrometer apparatus
  THaApparatus* NPS = new THcNPSApparatus("NPS","NPS");
  gHaApps->Add(NPS);

  //Add NPS Calorimeter to NPS apparatus
  THcNPSCalorimeter* cal = new THcNPSCalorimeter("cal", "Calorimeter");
  NPS->AddDetector(cal);
  
  // Add handler for prestart event 125.
  THcConfigEvtHandler* ev125 = new THcConfigEvtHandler("HC", "Config Event type 125");
  gHaEvtHandlers->Add(ev125);
   // Add handler for EPICS events
  THaEpicsEvtHandler *hcepics = new THaEpicsEvtHandler("epics", "HC EPICS event type 180");
  gHaEvtHandlers->Add(hcepics);
 
  THcAnalyzer* analyzer = new THcAnalyzer;  
  THaEvent* event = new THaEvent;

  //Define the run(s) that we want to analyze.
  THcRun* run = new THcRun( pathList, Form(RunFileNamePattern, RunNumber) );
  
  // Set to read in Hall C run database parameters
  run->SetRunParamClass("THcRunParameters");
  run->SetEventRange(1, MaxEvent);    
  run->SetNscan(1);
  run->SetDataRequired(0x7);
  run->Print();
  
  // Define the analysis parameters
  TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber);

  // Define the analysis parameters
  analyzer->SetEvent( event );
  analyzer->SetCountMode(2);  // 0 = counter is # of physics triggers
                              // 1 = counter is # of all decode reads
                              // 2 = counter is event number
   
  analyzer->SetOutFile(ROOTFileName.Data());
  analyzer->SetCrateMapFileName("MAPS/NPS/CRATE/db_cratemap_eel108.dat");
  analyzer->SetOdefFile("DEF-files/NPS/NPS.def");      
  analyzer->SetCutFile("DEF-files/NPS/NPS_cuts.def");  
  // Set EPICS event type
  analyzer->SetEpicsEvtType(180);
  analyzer->AddEpicsEvtType(181);
  analyzer->AddEpicsEvtType(182);
  
  analyzer->Process(run);     // start the actual analysis
  // Create report file from template.
  analyzer->PrintReport("TEMPLATES/NPS/NPS.template",
			Form("REPORT_OUTPUT/NPS/eel108/eel108_report_%d_%d.report", RunNumber, MaxEvent));
  
}
