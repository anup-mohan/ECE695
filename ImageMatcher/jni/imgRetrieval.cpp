#include <jni.h>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include "include/OpenCVIncludes.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <android/log.h>

using namespace cv;
using namespace std;

extern "C"{
JNIEXPORT void JNICALL Java_com_example_imagematcher_imgRetrieval_getMatch
(JNIEnv *env, jobject thisObj, jstring imgPath, jstring imgDir, jstring outPath);
};


//These choose which test mode to run in.
enum test_mode {BRUTE_FORCE, FLANN_DEFAULT, FLANN_KMEANS_DEFAULT};
const test_mode MODE = FLANN_DEFAULT;

//This structure contains the image data and its score. It is used to
//help determine the top-scoring image
typedef struct{
	std::string fname;
	int score;
} imgscore;


//This structure groups together the filename and its computed descriptors
typedef struct{
	string fname;
	Mat descriptors;
}descriptor;

//Auxiliary function to sort a list of scores
bool sorting(imgscore first, imgscore second)
{
	if (first.score > second.score)
	return true;
	else
	return false;
}


//This reduces the number of matches based on the Ratio test
int PruneRatioTest(vector<vector<DMatch> > &origMatches, double ratio=0.6)
{
	vector<DMatch> pruned;
	for(unsigned int i=0; i < origMatches.size(); i++)
	{
		//cout << i << ": best two distances = " << origMatches[i][0].distance << " " << origMatches[i][1].distance << endl;
		if((origMatches[i].size()==1)||(origMatches[i][0].distance/origMatches[i][1].distance < ratio))
		pruned.push_back(origMatches[i][0]);
	}
	return pruned.size();
}

int findMatches(const string &imgName, const string &descriptorDir, const string &outPath)
{
	//cin.ignore();
	//cout << "Feature Matching: \r\n" << endl;
	//waitKey(0);
	/*
	if (argc < 4)
	{
		cout << "Usage: ./CBIR_server <query image> <descriptor directory> <results file>" << endl;
		//cin.ignore();
		exit(1);
	}
	*/

	initModule_nonfree(); //This is needed to use anything in this module (i.e. SIFT stuff)

	
	//////////////////// Compute Descriptor of the Query Image ////////////////////////////////////////////////////////
	
	Ptr<FeatureDetector> detector = FeatureDetector::create("SURF");
	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create("SURF");

	Mat querydescriptors, image;
	vector<KeyPoint> keypoints;
	int i = 1;

	//cout << "Computing descriptors" << endl;


	//string imgName = string(imgName);
	string fname_out = imgName;
	image = imread(fname_out, CV_LOAD_IMAGE_GRAYSCALE);
	if (image.empty()) //File not a supported image type
	{
		__android_log_print(ANDROID_LOG_ERROR, "QUERY", "Empty Image File");
		exit(1);
	}

	/*
	int ext_idx = imgName.find(".jpg");

	if (ext_idx == string::npos)
	{
		cout << "Not JPG" << endl;
		exit(1);
	}
	//std::cout << "\tImage " << i++ << ":\t"<<fname_out<< std::endl;
	*/
	int ext_idx;
	string path;

	// Resize the image to 320x240
	resize(image, image, Size(640,480));

	//Find descriptors
	detector->detect(image,keypoints);
	extractor->compute(image, keypoints, querydescriptors);

	//Store descriptors
	descriptor queryDescriptor;
	queryDescriptor.descriptors = querydescriptors;
	queryDescriptor.fname = imgName;


	///////////////////// Matching Descriptors /////////////////////////////////////////////////////////////////

	DIR *dir;

	//cout<< "Reading descriptors" << endl;
	struct dirent *imgfile;
	vector<descriptor> descriptors;
	///vector<querydescriptor> queryDescriptors;
	string dirName(descriptorDir);
	dirName += "/";

	string outName = outPath + "/" + "Androidaccuracy.txt";
	//__android_log_print(ANDROID_LOG_INFO, "MATCH", "File is %s", outName.c_str());

	ofstream accFile(outName.c_str());
	if (!accFile)
	{
		__android_log_print(ANDROID_LOG_ERROR, "MATCH", "Could not open accuracy file");
		exit(1);
	}

	if((dir = opendir(dirName.c_str())) == NULL)
	{
		__android_log_print(ANDROID_LOG_ERROR, "MATCH", "Could not open descriptor directory");
		exit(1);
	}
	imgfile = readdir(dir);

	i = 1;

	/* This loop runs through each file in the directory,
	* reading in the descriptors and storing them into a
	* vector
	*/
	while (imgfile != NULL)
	{
		string imgName = string(imgfile->d_name);
		string imgpath = dirName + string(imgfile->d_name);

		//cout << "\tFile "<< i++ << ": "<< imgpath << endl;
		descriptor currentDescriptor;

		ext_idx = imgName.find(".xml");
		if (ext_idx == string::npos)
		{
			//File is not of correct type. Skip it.
			imgfile = readdir(dir);
			continue;
		}
		FileStorage fs(imgpath, FileStorage::READ);
		if (!fs.isOpened()) //Ensures file was opened properly
		{
			imgfile = readdir(dir);
			continue;
		}
		fs["imgDescriptors"] >> currentDescriptor.descriptors;
		fs.release();


		imgName.replace(ext_idx,4,".jpg");
		currentDescriptor.fname = imgName;
		descriptors.push_back(currentDescriptor);
		imgfile = readdir(dir);
	}



	//cout << "Done." << endl;
	//cout << "Opening Output File...";
	//ofstream fout(outPath);
	//cout << "Done." << endl;

	vector<vector<DMatch> > matches;
	Ptr<DescriptorMatcher> matcher = NULL;
	Ptr<flann::IndexParams> indexParams = NULL;
	Ptr<flann::SearchParams> searchParams = NULL;

	//Choose correct matcher based on mode specified
	matcher = new FlannBasedMatcher();

	//cout << "Matching descriptors" << endl;
	i = 1;
	

	list<imgscore> scores;
	//cout << "Query " << i++ << ": " << queryDescriptor.fname << endl;
	int j = 1;
	
	for (std::vector<descriptor>::const_iterator train = descriptors.begin(); train != descriptors.end(); ++train)
	{
		//cout << "\tTrain " << j++ << ": " << train->fname << endl;

		(*matcher).knnMatch(queryDescriptor.descriptors, train->descriptors, matches,2);
		//vector<DMatch> pruned = PruneRatioTest(matches);

		//TODO: Optimizations can most likely be made in storing the top matches
		imgscore score;
		score.fname = train->fname;
		score.score = PruneRatioTest(matches); //BruteForceMatch(query->descriptors, train->descriptors);

		scores.push_front(score);
		scores.sort(sorting);
		if (scores.size() > 4)
		scores.pop_back();

	}

	for (std::list<imgscore>::iterator it=scores.begin(); it != scores.end(); ++it)
	{
		//fout << (*it).fname <<"(" << (*it).score << ")\t";
		accFile << (*it).fname <<"(" << (*it).score << ")\t";
	}
	accFile << endl;


	///closedir(dir);
	//cout << "Done.";

	return 0;
	

	accFile.close();


}




JNIEXPORT void JNICALL Java_com_example_imagematcher_imgRetrieval_getMatch
(JNIEnv *env, jobject thisObj, jstring imgPath, jstring imgDir, jstring outPath)
{
	string image = (string)env->GetStringUTFChars(imgPath,0);
	string descriptorDir = (string)env->GetStringUTFChars(imgDir, 0);
	string outputFileDir = (string)env->GetStringUTFChars(outPath, 0);
	findMatches(image, descriptorDir, outputFileDir);
}
