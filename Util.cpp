#include "util.h"

void Util::Error(std::string message){
	cerr << "ERROR: " << message.c_str() << "." << endl;
	cin.sync();
	cin.ignore();
}

void Util::Pause(){
	cout << "Paused ..." << endl;
	cin.sync();
	cin.ignore();
}

string Util::SelectExperimentFolder(){
	//TODO: Allow to pick a folder from the experiments dir.
	QDir dir = QDir::home();
	
	cout << "Finding '"<< UAVEXP_DIR <<"' directory ... ";
	if(!dir.exists(UAVEXP_DIR))
	{
		cout << "[Error]" << endl;
		
		bool done = dir.mkdir(UAVEXP_DIR);
		
		stringstream ss;
		ss << "Could not find '" << UAVEXP_DIR << "': " << (done? "It was created, please populate" : "It could not be created");
		
		Error(ss.str());

		return "";
	}
	cout << "[Done]" << endl;

	dir.cd(UAVEXP_DIR);

	//Find all dirs.
	QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList::iterator it;

	int cnt = 1, sel = -1;
	cout << "Listing of: " << dir.absolutePath().toUtf8().constData() << endl << endl;
	cout << "   0) Back" << endl;
	for(it = list.begin(); it != list.end(); ++it)
	{
		cout << "   " << cnt++ << ") " << (*it).toUtf8().constData() << endl;
	}
	string opt;
	cin >> opt;
	sel = atoi(opt.c_str());
	
	if(sel == 0) return "";
	
	if(sel < 0 || sel > list.size())
	{
		Error("Invalid option");
		return "";
	}

	QString selQstr = list[--sel];
	string ret = (dir.absolutePath() + "/" + selQstr).toUtf8().constData();
	cout << ret << endl;

	return ret;
}

string Util::CreateRecordingDir(){
	
	QDir dir = QDir::home();
	QString dirstr = QDir::toNativeSeparators(dir.absolutePath());
	cout << "Home path: " << dirstr.toUtf8().constData() << endl;
	
	cout << "Finding '"<< UAVREC_DIR <<"' directory ... ";
	if(!dir.exists(UAVREC_DIR)){
		dir.mkdir(UAVREC_DIR);
		cout << "[Created]" << endl;
	}else{
		cout << "[Done]" << endl;
	}

	dir.cd(UAVREC_DIR);
	cout << dir.absolutePath().toUtf8().constData() << endl;

	stringstream ssoutdir;
	QDateTime t = QDateTime::currentDateTime();
	ssoutdir << "Output " << t.toString("yyyy-MM-dd_hh_mm_ss").toUtf8().constData();
	
	int good = 0;

	good += dir.mkdir(ssoutdir.str().c_str());
	dir.cd(ssoutdir.str().c_str());

	good += dir.mkdir("telemetry");
	good += dir.mkdir("photos");

	if(good < 3){
		stringstream msg;
		msg << "Could not create directories under '" << dir.absolutePath().toUtf8().constData() << "'";
		Util::Error(msg.str());
		return "";
	}

	return dir.absolutePath().toUtf8().constData();
}

bool removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void Util::CheckEmptyRecordingDirs(){
	QDir dir = QDir::home();
	dir.cd(UAVREC_DIR);

	//Find all dirs and check whether they're empty.
	QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList::iterator it;

	cout << "Checking for empty recording dirs ..." << endl;
	for(it = list.begin(); it != list.end(); ++it){
		stringstream ss;
		ss << dir.absolutePath().toUtf8().constData() << "/" << (*it).toUtf8().constData();
		QDir subdir(QString(ss.str().c_str()));
		
		int done = 0;
		int totalFiles = 0;

		if(subdir.cd("telemetry")){
			//Check nr of files.
			QFileInfoList entries = subdir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
			totalFiles += entries.size();
			
			//Account for the telemetry file (header)
			QString file(subdir.absolutePath() + "/telemetry_data.txt");
			QFileInfo fileinfo(file);
			
			if(entries.contains(fileinfo)){
				totalFiles --;
			}
			done ++;
			subdir.cdUp();
		}
		
		if(subdir.cd("photos")){
			//Check Nr of files.
			QFileInfoList entries = subdir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
			totalFiles += entries.size();
			
			done++;
			subdir.cdUp();
		}

		QFileInfoList entries = subdir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
		
		//If files still 0, delete the dir.
		if(entries.size() <= done && totalFiles == 0)
		{
			bool done = removeDir(subdir.absolutePath());
			if(done){
				cout << " - Removed '" << subdir.dirName().toUtf8().constData() << "'" << endl;
			}
		}
	}

	cout << endl;
}

Util::TExperiment Util::LoadExperiment(bool withGT)
{
	//Generate an experiment structure
	TExperiment experiment;
	experiment.valid = false;

	string dir = Util::SelectExperimentFolder();
	if(dir.empty()) return experiment;

	experiment.dir = dir;

	std::vector<TSequence> seqs;
	seqs = getSequences(dir + "\\sequences.txt");
	if(seqs.empty()){
		Util::Error("Could not load sequence information: Please check './sequences.txt'");
		return experiment;
	}

	cout << "0) Back" << endl;
	for (size_t i = 1; i <= seqs.size(); i++)
	{
		cout << i << ") " << seqs[i-1].name << endl;
	}

	//Reads user's input
	string opt;
	size_t ch;
	do{
		cout << "Please give a number from the list above: " << endl;
		cin >> opt;
		ch = atoi(opt.c_str());
		if(ch == 0) return experiment;
	}while(ch < 0 || ch > seqs.size());

	ch--;

	experiment.sequence = seqs[ch];

	std::vector<TBBox> bboxes;
	bboxes = getBBoxes(dir + "\\bbox.txt");
	if(bboxes.empty()){
		Util::Error("Could not open ROI (init. seeds) for sequences: please check './bbox.txt'");
		return experiment;
	}

	size_t k;
	for (k = 0; k < bboxes.size(); k++)
	{
		if(bboxes[k].seqName.compare(seqs[ch].name) == 0)
		{
			break;
		}
	}

	if (k == bboxes.size())
	{
		Util::Error("The 'sequence.txt' and 'bbox.txt' you created are not matching");
		return experiment;
	}

	experiment.init_bbox = bboxes[k];

	//TODO: Load gt if asked.
	if(withGT)
	{
		//Open ground truth file and load contents.
		std::vector<pair<int, cv::Rect>> rects;
		string gtfile = dir + "\\gt files\\gt_" + experiment.sequence.name + ".txt";
		rects = getGTValues(gtfile);
		if(rects.empty()){
			Util::Error("Could not retrieve GT information: Please check '" + gtfile + "'");
			return experiment;
		}

		experiment.gt_rois = rects;
	}

	experiment.valid = true;
	return experiment;
}