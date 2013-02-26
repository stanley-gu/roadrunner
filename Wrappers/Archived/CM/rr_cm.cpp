///**
// * @file rr_cm.cpp
// * @brief roadRunner C(M) API 2013
// * @author Totte Karlsson & Herbert M Sauro
// *
// * <--------------------------------------------------------------
// * This file is part of cRoadRunner.
// * See http://code.google.com/p/roadrunnerlib for more details.
// *
// * Copyright (C) 2012
// *   University of Washington, Seattle, WA, USA
// *
// * Licensed under the Apache License, Version 2.0 (the "License");
// * you may not use this file except in compliance with the License.
// * You may obtain a copy of the License at
// *
// *     http://www.apache.org/licenses/LICENSE-2.0
// *
// * Unless required by applicable law or agreed to in writing, software
// * distributed under the License is distributed on an "AS IS" BASIS,
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// * See the License for the specific language governing permissions and
// * limitations under the License.
// *
// * In plain english this means:
// *
// * You CAN freely download and use this software, in whole or in part, for personal,
// * company internal, or commercial purposes;
// *
// * You CAN use the software in packages or distributions that you create.
// *
// * You SHOULD include a copy of the license in any redistribution you may make;
// *
// * You are NOT required include the source of software, or of any modifications you may
// * have made to it, in any redistribution you may assemble that includes it.
// *
// * YOU CANNOT:
// *
// * redistribute any piece of this software without proper attribution;
//*/

#pragma hdrstop
#include <string>
#include <sstream>
#include "rrRoadRunner.h"
#include "rrRoadRunnerList.h"
#include "rrLoadModel.h"
#include "rrSimulate.h"
//#include "rrParameter.h"

#include "rrLogger.h"
#include "rrException.h"
#include "rrCGenerator.h"
//#include "rrUtils.h"
//#include "rrStringUtils.h"
//#include "rrCapability.h"
//#include "rrPluginManager.h"
//#include "rrPlugin.h"
//
//#if defined(_MSC_VER)
//	#include <direct.h>
//	#define getcwd _getcwd
//	#define chdir  _chdir
//#elif defined(__BORLANDC__)
  	#include <dir.h>			//getcwd
//#else
//#include <unistd.h>
//#endif
//
#include "rr_cm.h"
#include "rr_cm_support.h"   //Support functions, not exposed as api functions and or data
////---------------------------------------------------------------------------
using namespace std;
using namespace rr;
using namespace rr_cm;

RRHandle rrCallConv createRRHandle()
{
	try
    {
    	string rrInstallFolder(getParentFolder(getRRCAPILocation()));
        string compiler = getCompilerName();
        RRHandle rrHandle = new RoadRunner(JoinPath(rrInstallFolder, "rr_support"), compiler, GetUsersTempDataFolder());
    	return rrHandle;
    }
	catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return NULL;
    }
}

RRInstanceListHandle rrCallConv createRRHandles(int count)
{
	try
    {
    	//string rrInstallFolder(getParentFolder(getRRCAPILocation()));
        //string compiler = getCompilerName();
		RoadRunnerList* listHandle = new RoadRunnerList(count, GetUsersTempDataFolder());

        //Create the C list structure
		RRInstanceListHandle rrList = new RRInstanceList;
        rrList->RRList = (void*) listHandle;
        rrList->Count = count;

        //Create 'count' handles
        rrList->Handle = new RRHandle[count];

        //Populate handles
        for(int i = 0; i < count; i++)
        {
        	rrList->Handle[i] = (*listHandle)[i];
        }
    	return rrList;
    }
	catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return NULL;
    }
}

bool rrCallConv freeRRHandles(RRInstanceListHandle rrList)
{
	try
    {
    	//Delete the C++ list
        RoadRunnerList* listHandle = (RoadRunnerList*) rrList->RRList;

		delete listHandle;

        //Free  C handles
        delete [] rrList->Handle;

        //Free the C list
        delete rrList;
		return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return false;
    }
}

//char* rrCallConv getInstallFolder()
//{
//	if(!gInstallFolder)
//	{
//		gInstallFolder = new char[2048];
//		strcpy(gInstallFolder, "/usr/local");
//	}
//	return gInstallFolder;
//}
//
char* rrCallConv getCompilerName()
{
	string compiler;
    string rrInstallFolder(getParentFolder(getRRCAPILocation()));

        //Get location of shared lib and use that as 'install' folder
	#if defined(_WIN32) || defined(WIN32)
        compiler = (JoinPath(rrInstallFolder,"compilers\\tcc\\tcc.exe"));
	#elif defined(__linux)
        compiler = "gcc";
	#else
        compiler = "gcc";
    #endif

	return createText(compiler);
}
//
//bool rrCallConv  setInstallFolder(const char* folder)
//{
//	try
//    {
//		gInstallFolder = new char[2048];
//	    return strcpy(gInstallFolder, folder) != NULL ? true : false;
//    }
//    catch(Exception& ex)
//    {
//    	stringstream msg;
//    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
//        rr_cm::setError(msg.str());
//  		return false;
//    }
//}

void rrCallConv logMsg(CLogLevel lvl, const char* msg)
{
	try
    {
		Log(lvl)<<msg;
    }
    catch(const Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
    }
}

bool rrCallConv enableLoggingToConsole()
{
	try
    {
	    LogOutput::mLogToConsole = true;
    	return true;
    }
    catch(const Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
  	    return false;
    }
}

bool rrCallConv enableLoggingToFile(RRHandle handle)
{
	try
    {
        char* tempFolder = getTempFolder(handle);
		string logFile = JoinPath(tempFolder, "RoadRunner.log") ;
        freeText(tempFolder);

        gLog.Init("", gLog.GetLogLevel(), unique_ptr<LogFile>(new LogFile(logFile.c_str())));
    	return true;
    }
    catch(const Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
  	    return false;
    }
}

bool rrCallConv setLogLevel(const char* _lvl)
{
	try
    {
        LogLevel lvl = GetLogLevel(_lvl);
		gLog.SetCutOffLogLevel(lvl);
    	return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
  	    return false;
    }
}

char* rrCallConv getLogLevel()
{
	try
    {
        string level = gLog.GetCurrentLogLevel();
        char* lvl = createText(level.c_str());
    	return lvl;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
  	    return NULL;
    }
}

char* rrCallConv getLogFileName()
{
	try
    {
    	return createText(gLog.GetLogFileName().c_str());
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
  	    return NULL;
    }
}

char* rrCallConv getBuildDate()
{
    return rr_cm::createText(__DATE__);
}
//
//char* rrCallConv getBuildTime()
//{
//    return createText(__TIME__);
//}
//
//char* rrCallConv getBuildDateTime()
//{
//    return createText(string(__DATE__) + string(" ") + string(__TIME__));
//}
//
//char* rrCallConv getVersion(RRHandle _rrHandle)
//{
//	try
//    {
//		RoadRunner* handle = (RoadRunner*) _rrHandle;
//        if(!handle)
//        {
//            setError(BAD_HANDLE_ERROR_MSG);
//            return NULL;
//        }
//		return createText(handle->getVersion());
//    }
//    catch(Exception& ex)
//    {
//    	stringstream msg;
//    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
//        setError(msg.str());
//		return NULL;
//    }
//}

char* rrCallConv getRRCAPILocation()
{
#if defined(_WIN32) || defined(WIN32)
	char path[MAX_PATH];
    HINSTANCE handle = NULL;
    const char* dllName = "rr_c_api";
    handle = GetModuleHandle(dllName);
	if(GetModuleFileNameA(handle, path, ARRAYSIZE(path)) != 0)
    {
	    string aPath(ExtractFilePath(path));
		return createText(aPath);
    }
    return NULL;
#else
	return createText(JoinPath(getInstallFolder(),"/lib"));
#endif
}

char* rrCallConv getCopyright(RRHandle handle)
{
	try
    {
   		RoadRunner* rri = castToRRInstance(handle);
        char* text = createText(rri->getCopyright());
        return text;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return NULL;
    }
}

////char* rrCallConv getInfo()
////{
////	try
////    {
////        char* text = NULL;
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////        else
////        {
////            text = createText(rrHandle->getInfo());
////        }
////        return text;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////char* rrCallConv getlibSBMLVersion()
////{
////	try
////    {
////        char* text = NULL;
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////        else
////        {
////            text = createText(rrHandle->getlibSBMLVersion());
////        }
////        return text;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////	}
////}
////
////char* rrCallConv getCurrentSBML()
////{
////	try
////    {
////        char* text = NULL;
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////        else
////        {
////            text = createText(rrHandle->writeSBML());
////        }
////        return text;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////	}
////}
////
//////Flags and Options
////bool rrCallConv setComputeAndAssignConservationLaws(const bool& OnOrOff)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        rrHandle->computeAndAssignConservationLaws(OnOrOff);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return false;
////     }
////}
////

bool rrCallConv setTempFolder(RRHandle handle, const char* folder)
{
	try
    {
    	RoadRunner* rrHandle = castToRRInstance(handle);
	    return rrHandle->setTempFileFolder(folder);
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
  		return false;
    }
}

char* rrCallConv getTempFolder(RRHandle handle)
{
	try
    {
    	RoadRunner* rrHandle = castToRRInstance(handle);
	    return createText(rrHandle->getTempFileFolder());
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return NULL;
    }
}
//
////bool rrCallConv setCompiler(const char* fName)
////{
////	try
////    {
////    	if(!rrHandle)
////    	{
////        	setError(ALLOCATE_API_ERROR_MSG);
////        	return false;
////    	}
////		if(rrHandle->getCompiler())
////		{
////			return rrHandle->getCompiler()->setCompiler(fName);
////		}
////		else
////		{
////			return false;
////		}
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return false;
////    }
////}
////
////bool rrCallConv setCompilerLocation(const char* folder)
////{
////	try
////    {
////    	if(!rrHandle)
////    	{
////        	setError(ALLOCATE_API_ERROR_MSG);
////        	return false;
////    	}
////		if(rrHandle->getCompiler())
////		{
////			return rrHandle->getCompiler()->setCompilerLocation(folder);
////		}
////		else
////		{
////			return false;
////		}
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return false;
////    }
////}
////
////char* rrCallConv getCompilerLocation()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////	    return createText(rrHandle->getCompiler()->getCompilerLocation());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////bool rrCallConv setSupportCodeFolder(const char* folder)
////{
////	try
////    {
////    	if(!rrHandle)
////    	{
////        	setError(ALLOCATE_API_ERROR_MSG);
////        	return false;
////    	}
////		if(rrHandle->getCompiler())
////		{
////			return rrHandle->getCompiler()->setSupportCodeFolder(folder);
////		}
////		else
////		{
////			return false;
////		}
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return false;
////    }
////}
////
////char* rrCallConv getSupportCodeFolder()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////	    return createText(rrHandle->getCompiler()->getSupportCodeFolder());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////char* rrCallConv getWorkingDirectory()
////{
////	try
////    {
////	    return createText(rr::getCWD());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////

bool rrCallConv loadSBMLFromFile(RRHandle _handle, const char* fileName)
{
	try
    {
    	RoadRunner* rri = castToRRInstance(_handle);

        //Check if file exists first
        if(!FileExists(fileName))
        {
            stringstream msg;
            msg<<"The file "<<fileName<<" was not found";
            setError(msg.str());
            return false;
        }

        if(!rri->loadSBMLFromFile(fileName))
        {
            setError("Failed to load SBML semantics");	//There are many ways loading a model can fail, look at logFile to know more
            return false;
        }
        return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return false;
    }
}

TPHandle rrCallConv loadModelFromFileTP(RRInstanceListHandle _handles, const char* fileName, int nrOfThreads)
{
	try
    {
        //Check if file exists first
        if(!FileExists(fileName))
        {
            stringstream msg;
            msg<<"The file "<<fileName<<" do not exist";
            setError(msg.str());
            return NULL;
        }

        RoadRunnerList *rrs = getRRList(_handles);
        LoadModel* tp = new LoadModel(*rrs, fileName, nrOfThreads);

        if(!tp)
        {
            setError("Failed to create a LoadModel Thread Pool");
        }
        return tp;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return false;
    }
}

bool rrCallConv waitForJobs(TPHandle handle)
{
	try
    {
        ThreadPool* aTP = (ThreadPool*) handle;
        if(aTP)
        {
            aTP->waitForAll();
            return true;
        }
		return false;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return false;
    }
}

int rrCallConv getNumberOfRemainingJobs(TPHandle handle)
{
	try
    {
        ThreadPool* aTP = (ThreadPool*) handle;
        if(aTP)
        {
            return aTP->getNumberOfRemainingJobs();
        }
    	return -1;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return -1;
    }
}



//bool rrCallConv loadSBML(const char* sbml)
//{
//	try
//    {
//        if(!rrHandle)
//        {
//            setError(ALLOCATE_API_ERROR_MSG);
//            return false;
//        }
//
//        if(!rrHandle->loadSBML(sbml))
//        {
//            setError("Failed to load SBML semantics");
//            return false;
//        }
//        return true;
//    }
//    catch(Exception& ex)
//    {
//    	stringstream msg;
//    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
//        setError(msg.str());
//	  	return false;
//    }
//}

bool rrCallConv loadSBMLH(RRHandle _handle, const char* sbml)
{
	try
    {
    	RoadRunner* handle = castToRRInstance(_handle);
        if(!handle)
        {
            setError(INVALID_HANDLE_ERROR_MSG);
            return false;
        }

        if(!handle->loadSBML(sbml))
        {
            setError("Failed to load SBML semantics");
            return false;
        }
        return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	  	return false;
    }
}


////bool rrCallConv loadSimulationSettings(const char* fileName)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////        //Check if file exists first
////        if(!FileExists(fileName))
////        {
////            stringstream msg;
////            msg<<"The file "<<fileName<<" was not found";
////            setError(msg.str());
////            return false;
////        }
////
////        if(!rrHandle->loadSimulationSettings(fileName))
////        {
////            setError("Failed to load SBML semantics");	//There are many wasy loading a model can fail, look at logFile to know more
////            return false;
////        }
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
////
////char* rrCallConv getSBML()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        string sbml = rrHandle->getSBML();
////
////        return createText(sbml);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return NULL;
////    }
////}
////
////bool rrCallConv unLoadModel()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        return rrHandle->unLoadModel();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return NULL;
////    }
////}
////
bool rrCallConv setTimeStart(RRHandle handle, const double timeStart)
{
	try
    {
        RoadRunner* rrHandle = castToRRInstance(handle);
        rrHandle->setTimeStart(timeStart);
    	return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return false;
    }
}

bool rrCallConv setTimeEnd(RRHandle handle, const double timeEnd)
{
	try
    {
        RoadRunner* rrHandle = castToRRInstance(handle);
        rrHandle->setTimeEnd(timeEnd);
        return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return false;
    }
}

bool rrCallConv setNumPoints(RRHandle handle, const int nrPoints)
{
	try
    {
        RoadRunner* rrHandle = castToRRInstance(handle);

        rrHandle->setNumPoints(nrPoints);
	    return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	  	return false;
    }
}

////bool rrCallConv getTimeStart(double& timeStart)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////		timeStart = rrHandle->getTimeStart();
////		return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    }
////  	return false;
////}
////
////
////bool rrCallConv getTimeEnd(double& timeEnd)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////		timeEnd = rrHandle->getTimeEnd();
////		return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv getNumPoints(int& numPoints)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////		numPoints = rrHandle->getNumPoints();
////		return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////

bool rrCallConv setTimeCourseSelectionList(RRHandle handle, const char* list)
{
	try
    {
    	RoadRunner* rrHandle = castToRRInstance(handle);
        rrHandle->setTimeCourseSelectionList(list);
        return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return false;
    }
}

////bool rrCallConv createTimeCourseSelectionList()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        return rrHandle->createTimeCourseSelectionList() > 0 ? true : false;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return false;
////    }
////}
////
////RRStringArrayHandle rrCallConv getTimeCourseSelectionList()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        StringList sNames = rrHandle->getTimeCourseSelectionList();
////
////        if(!sNames.Count())
////        {
////            return NULL;
////        }
////
////        return createList(sNames);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////
////}
////
////RRResultHandle rrCallConv simulate()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        if(!rrHandle->simulate2())
////        {
////            return NULL;
////        }
////
////        SimulationData result = rrHandle->getSimulationResult();
////
////        //Extract the data and return struct..
////        RRResult* aResult  = new RRResult;
////        aResult->ColumnHeaders = new char*[result.cSize()];
////        for(int i = 0; i < result.cSize(); i++)
////        {
////            aResult->ColumnHeaders[i] = createText(result.getColumnNames()[i]);
////            //new char(32);
////            //strcpy(aResult->ColumnHeaders[i], result.GetColumnNames()[i].c_str());
////        }
////
////        aResult->RSize = result.rSize();
////        aResult->CSize = result.cSize();
////        int size = aResult->RSize*aResult->CSize;
////        aResult->Data = new double[size];
////
////        int index = 0;
////        //The data layout is simple row after row, in one single long row...
////        for(int row = 0; row < aResult->RSize; row++)
////        {
////            for(int col = 0; col < aResult->CSize; col++)
////            {
////                aResult->Data[index++] = result(row, col);
////            }
////        }
////	    return aResult;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////

//TPHandle rrCallConv loadModelFromFileTP(RRInstanceListHandle _handles, const char* fileName, int nrOfThreads)
TPHandle rrCallConv simulateTP(RRInstanceListHandle _handles, int nrOfThreads)
{
	try
    {
        RoadRunnerList *rrs = getRRList(_handles);
        Simulate* tp = new Simulate(*rrs, nrOfThreads);

        if(!tp)
        {
            setError("Failed to create a Simulate Thread Pool");
        }
        return tp;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return NULL;
    }
}



////RRResultHandle rrCallConv simulateEx (const double& timeStart, const double& timeEnd, const int& numberOfPoints)
////{
////	try
////    {
////        setTimeStart(timeStart);
////        setTimeEnd (timeEnd);
////        setNumPoints(numberOfPoints);
////	  	return simulate();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRStringArrayHandle rrCallConv getReactionIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        StringList rNames = rrHandle->getReactionIds();
////
////        if(!rNames.Count())
////        {
////            return NULL;
////        }
////
////
////        return createList(rNames);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////RRVectorHandle rrCallConv getRatesOfChange()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        vector<double> rates = rrHandle->getRatesOfChange();
////
////        if(!rates.size())
////        {
////            return NULL;
////		}
////
////        RRVector* list = new RRVector;
////        list->Count = rates.size();
////        list->Data = new double[list->Count];
////
////        for(int i = 0; i < list->Count; i++)
////        {
////            list->Data[i] = rates[i];
////		}
////		return list;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////		setError(msg.str());
////		return NULL;
////	}
////}
////
////RRStringArrayHandle rrCallConv getRatesOfChangeIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        StringList rNames = rrHandle->getRateOfChangeIds();
////
////        if(!rNames.Count())
////        {
////            return NULL;
////        }
////
////        return createList(rNames);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    }
////	return NULL;
////}
////

bool rrCallConv getValue(RRHandle handle, const char* symbolId, double *value)
{
	try
    {
        RoadRunner* rrHandle = castToRRInstance(handle);
	    *value = rrHandle->getValue(symbolId);
        return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return false;
    }
}

bool rrCallConv setValue(RRHandle handle, const char* symbolId, const double value)
{
	try
    {
    	RoadRunner* rrHandle = castToRRInstance(handle);
    	return rrHandle->setValue(symbolId, value);
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return false;
    }
}

////RRMatrixHandle rrCallConv getUnscaledElasticityMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////		DoubleMatrix tempMat = rrHandle->getUnscaledElasticityMatrix();
////
////        RRMatrixHandle matrix = createMatrix(&tempMat);
////	    return matrix;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getScaledElasticityMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////		DoubleMatrix tempMat = rrHandle->getScaledReorderedElasticityMatrix();
////
////
////        RRMatrixHandle matrix = createMatrix(&tempMat);
////	    return matrix;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////

////RRMatrixHandle rrCallConv getStoichiometryMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        DoubleMatrix tempMat = rrHandle->getStoichiometryMatrix();
////
////        RRMatrixHandle matrix = new RRMatrix;
////        matrix->RSize = tempMat.RSize();
////        matrix->CSize = tempMat.CSize();
////        matrix->Data =  new double[tempMat.RSize()*tempMat.CSize()];
////
////        int index = 0;
////        for(rr::u_int row = 0; row < tempMat.RSize(); row++)
////        {
////            for(rr::u_int col = 0; col < tempMat.CSize(); col++)
////            {
////                matrix->Data[index++] = tempMat(row,col);
////            }
////        }
////	    return matrix;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getConservationMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        DoubleMatrix tempMat = rrHandle->getConservationMatrix();
////
////        RRMatrixHandle matrix = new RRMatrix;
////        matrix->RSize = tempMat.RSize();
////        matrix->CSize = tempMat.CSize();
////        matrix->Data =  new double[tempMat.RSize()*tempMat.CSize()];
////
////        int index = 0;
////        for(rr::u_int row = 0; row < tempMat.RSize(); row++)
////        {
////            for(rr::u_int col = 0; col < tempMat.CSize(); col++)
////            {
////                matrix->Data[index++] = tempMat(row,col);
////            }
////        }
////	    return matrix;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getLinkMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        DoubleMatrix *tempMat = rrHandle->getLinkMatrix();
////
////		return createMatrix(tempMat);
////	}
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getL0Matrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        DoubleMatrix *tempMat = rrHandle->getL0Matrix();
////
////		return createMatrix(tempMat);
////	}
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getNrMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        DoubleMatrix *tempMat = rrHandle->getNrMatrix();
////
////		return createMatrix(tempMat);
////	}
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
C_DECL_SPEC bool rrCallConv hasError()
{
    return (gLastError != NULL) ? true : false;
}

char* rrCallConv getLastError()
{
	if(!gLastError)
    {
	    gLastError = createText("No Error");
    }
    return gLastError;
}

////bool rrCallConv reset()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////        rrHandle->reset();
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
////int rrCallConv getNumberOfReactions()
////{
//// 	try
////    {
////        if(!rrHandle)
////        {
////           setError(ALLOCATE_API_ERROR_MSG);
////           return -1;
////        }
////        return rrHandle->getNumberOfReactions();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return -1;
////    }
////}
////
////bool rrCallConv getReactionRate(const int& rateNr, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////        value = rrHandle->getReactionRate(rateNr);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
////RRVectorHandle rrCallConv getReactionRates()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        vector<double> vec =  rrHandle->getReactionRates();
////
////        RRVector* aVec = createVectorFromVector_double(vec);
////        return aVec;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////int rrCallConv getNumberOfBoundarySpecies()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return -1;
////        }
////        return rrHandle->getNumberOfBoundarySpecies();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return -1;
////    }
////}
////
////RRStringArrayHandle rrCallConv getBoundarySpeciesIds()
////{
////	try
////    {
////
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        StringList bNames = rrHandle->getBoundarySpeciesIds();
////
////        if(!bNames.Count())
////        {
////            return NULL;
////        }
////
////        return createList(bNames);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	   	return NULL;
////    }
////}
////
////int rrCallConv getNumberOfFloatingSpecies()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return -1;
////        }
////        return rrHandle->getNumberOfFloatingSpecies();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	   	return -1;
////    }
////}
////
////RRStringArrayHandle rrCallConv getFloatingSpeciesIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        StringList fNames = rrHandle->getFloatingSpeciesIds();
////
////        if(!fNames.Count())
////        {
////            return NULL;
////        }
////
////        return createList(fNames);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////int rrCallConv getNumberOfGlobalParameters()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return -1;
////        }
////        return rrHandle->getNumberOfGlobalParameters();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	   	return -1;
////    }
////}
////
////RRStringArrayHandle rrCallConv getGlobalParameterIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        StringList pNames = rrHandle->getGlobalParameterIds();
////
////        if(!pNames.Count())
////        {
////            return NULL;
////        }
////
////        return createList(pNames);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	   	return NULL;
////    }
////}
////
////RRVectorHandle rrCallConv getFloatingSpeciesConcentrations()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        vector<double> vec =  rrHandle->getFloatingSpeciesConcentrations();
////        RRVector* aVec = createVectorFromVector_double(vec);
////        return aVec;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRVectorHandle rrCallConv getBoundarySpeciesConcentrations()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        vector<double> vec =  rrHandle->getBoundarySpeciesConcentrations();
////        RRVector* aVec = createVectorFromVector_double(vec);
////        return aVec;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////
////RRVectorHandle rrCallConv getFloatingSpeciesInitialConcentrations()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        vector<double> vec =  rrHandle->getFloatingSpeciesInitialConcentrations();
////        RRVector* aVec = createVectorFromVector_double(vec);
////        return aVec;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////bool rrCallConv setFloatingSpeciesByIndex (const int& index, const double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        rrHandle->setFloatingSpeciesByIndex(index, value);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv setBoundarySpeciesByIndex (const int& index, const double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        rrHandle->setBoundarySpeciesByIndex(index, value);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv setGlobalParameterByIndex(const int& index, const double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        rrHandle->setGlobalParameterByIndex(index, value);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv setFloatingSpeciesInitialConcentrations(const RRVector* vec)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////        vector<double> tempVec;
////        copyVector(vec, tempVec);
////        rrHandle->changeInitialConditions(tempVec);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv setFloatingSpeciesConcentrations(const RRVector* vec)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////        vector<double> tempVec;
////        copyVector(vec, tempVec);
////        rrHandle->setFloatingSpeciesConcentrations(tempVec);
////
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv setBoundarySpeciesConcentrations(const RRVector* vec)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////        vector<double> tempVec;
////        copyVector(vec, tempVec);
////        rrHandle->setBoundarySpeciesConcentrations(tempVec);
////
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv oneStep(const double& currentTime, const double& stepSize, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        value = rrHandle->oneStep(currentTime, stepSize);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return false;
////    }
////}
////
////RRVectorHandle rrCallConv getGlobalParameterValues()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        vector<double> vec =  rrHandle->getGlobalParameterValues();
////        RRVector* aVec = createVectorFromVector_double(vec);
////        return aVec;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////RRListHandle rrCallConv getAvailableTimeCourseSymbols()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        NewArrayList slSymbols = rrHandle->getAvailableTimeCourseSymbols();
////		return createList(slSymbols);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////RRListHandle rrCallConv getAvailableSteadyStateSymbols()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        NewArrayList slSymbols = rrHandle->getAvailableSteadyStateSymbols();
////		return createList(slSymbols);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////bool rrCallConv getBoundarySpeciesByIndex (const int& index, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        value = rrHandle->getBoundarySpeciesByIndex(index);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv getFloatingSpeciesByIndex (const int& index, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        value = rrHandle->getFloatingSpeciesByIndex(index);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv getGlobalParameterByIndex (const int& index, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        value = rrHandle->getGlobalParameterByIndex(index);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv getuCC (const char* variable, const char* parameter, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////		}
////
////        value = rrHandle->getuCC(variable, parameter);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return false;
////	}
////}
////
////
////bool rrCallConv getCC (const char* variable, const char* parameter, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        value = rrHandle->getCC(variable, parameter);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv getuEE(const char* name, const char* species, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        value = rrHandle->getuEE(name, species);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  		return false;
////	}
////}
////
////bool rrCallConv getEE(const char* name, const char* species, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        value = rrHandle->getEE(name, species);
////        return true;
////    }
////	catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////int rrCallConv getNumberOfDependentSpecies()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return -1;
////        }
////
////        return rrHandle->getNumberOfDependentSpecies();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return -1;
////    }
////}
////
////int rrCallConv getNumberOfIndependentSpecies()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return -1;
////        }
////
////        return rrHandle->getNumberOfIndependentSpecies();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return -1;
////    }
////}
////
////bool rrCallConv steadyState(double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////	   	value = rrHandle->steadyState();
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	  	return false;
////    }
////}
////
////bool rrCallConv evalModel()
////{
////	try
////	{
////		if(!rrHandle)
////		{
////			setError(ALLOCATE_API_ERROR_MSG);
////		}
////		rrHandle->evalModel();
////        return true;
////	}
////	catch(Exception& ex)
////	{
////		stringstream msg;
////		msg<<"RoadRunner exception: "<<ex.what()<<endl;
////		setError(msg.str());
////	    return false;
////	}
////}
////
////char* rrCallConv getParamPromotedSBML(const char* sArg)
////{
////	try
////	{
////		if(!rrHandle)
////		{
////			setError(ALLOCATE_API_ERROR_MSG);
////			return NULL;
////		}
////
////		string param =  rrHandle->getParamPromotedSBML(sArg);
////
////		char* text = createText(param.c_str());
////		return text;
////	}
////	catch(Exception& ex)
////	{
////		stringstream msg;
////		msg<<"RoadRunner exception: "<<ex.what()<<endl;
////		setError(msg.str());
////		return NULL;
////	}
////}
////
////RRVectorHandle rrCallConv computeSteadyStateValues()
////{
////	try
////	{
////		if(!rrHandle)
////		{
////			setError(ALLOCATE_API_ERROR_MSG);
////			return NULL;
////		}
////		vector<double> vec =  rrHandle->computeSteadyStateValues();
////
////		RRVector* aVec = createVectorFromVector_double(vec);
////		return aVec;
////	}
////	catch(Exception& ex)
////	{
////		stringstream msg;
////		msg<<"RoadRunner exception: "<<ex.what()<<endl;
////		setError(msg.str());
////		return NULL;
////	}
////}
////
////bool rrCallConv setSteadyStateSelectionList(const char* list)
////{
////	try
////	{
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        StringList aList(list, " ,");
////        rrHandle->setSteadyStateSelectionList(aList);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return false;
////    }
////}
////
////RRStringArrayHandle rrCallConv getSteadyStateSelectionList()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        StringList sNames = rrHandle->getSteadyStateSelectionList();
////
////        if(!sNames.Count())
////        {
////            return NULL;
////        }
////
////        return createList(sNames);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getFullJacobian()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        DoubleMatrix tempMat = rrHandle->getFullJacobian();
////        return createMatrix(&tempMat);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getReducedJacobian()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        DoubleMatrix tempMat = rrHandle->getReducedJacobian();
////        return createMatrix(&tempMat);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getEigenvalues()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////		DoubleMatrix tempMat = rrHandle->getEigenvalues();
////        return createMatrix(&tempMat);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getEigenvaluesMatrix (const RRMatrixHandle mat)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////		if (mat == NULL) {
////         	stringstream msg;
////    	    msg<<"RoadRunner exception: "<< "Matrix argument to getEigenvaluesMAtrix is NULL" <<endl;
////            setError(msg.str());
////			return NULL;
////		}
////
////    	// Convert RRMatrixHandle mat to a DoubleMatrix
////		DoubleMatrix dmat (mat->RSize, mat->CSize);
////		double value;
////		for (int i=0; i<mat->RSize; i++)
////			for (int j=0; j<mat->CSize; j++) {
////				getMatrixElement (mat, i, j, value);
////				dmat(i,j) = value;
////			}
////		DoubleMatrix tempMat = rrHandle->getEigenvaluesFromMatrix (dmat);
////        // Convert the DoubleMatrix result to a RRMatrixHandle type
////		return createMatrix(&tempMat);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////char* rrCallConv getCSourceFileName()
////{
////	try
////    {
////    	if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        CGenerator* generator = rrHandle->getCGenerator();
////        if(!generator)
////        {
////            return NULL;
////        }
////
////        string fNameS = generator->getSourceCodeFileName();
////
////        fNameS = ExtractFileNameNoExtension(fNameS);
////		return createText(fNameS);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
RRCCode* rrCallConv getCCode(RRHandle handle)
{
	try
    {
    	RoadRunner* rri = castToRRInstance(handle);

        CGenerator* generator = rri->getCGenerator();
        if(!generator)
        {
            return NULL;
        }

        RRCCode* cCode = new RRCCode;
		cCode->Header = NULL;
		cCode->Source = NULL;
        string header = generator->getHeaderCode();
        string source = generator->getSourceCode();

        if(header.size())
        {
            cCode->Header = createText(header);
        }

        if(source.size())
        {
            cCode->Source = createText(source);
        }
        return cCode;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
		return NULL;
    }
}

////// *******  Not yet implemented  ********
////// codeGenerationMode = 0 if mode is C code generation
////// codeGenerationMode = 1 ig mode is internal math interpreter
////bool rrCallConv setCodeGenerationMode (int _mode)
////{
////	return false;
////}
////
//////NOM forwarded functions
////int rrCallConv getNumberOfRules()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return -1;
////        }
////        if(!rrHandle->getNOM())
////        {
////            Log(lWarning)<<"NOM is not allocated.";
////        	return -1;
////        }
////        int value = rrHandle->getNOM()->getNumRules();
////        return value;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return -1;
////    }
////}
////
////bool rrCallConv getScaledFloatingSpeciesElasticity(const char* reactionId, const char* speciesId, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////        value = rrHandle->getScaledFloatingSpeciesElasticity(reactionId, speciesId);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
////RRStringArrayHandle rrCallConv getFloatingSpeciesInitialConditionIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        StringList aList = rrHandle->getFloatingSpeciesInitialConditionIds();
////		return createList(aList);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////RRVectorHandle rrCallConv getRatesOfChangeEx(const RRVectorHandle vec)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        vector<double> tempList = createVectorFromRRVector(vec);
////        tempList = rrHandle->getRatesOfChangeEx(tempList);
////        return createVectorFromVector_double (tempList);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////RRVectorHandle rrCallConv getReactionRatesEx(const RRVectorHandle vec)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        vector<double> tempList = createVectorFromRRVector(vec);
////        tempList = rrHandle->getReactionRatesEx(tempList);
////        return createVectorFromVector_double(tempList);;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////RRListHandle rrCallConv getElasticityCoefficientIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        NewArrayList aList = rrHandle->getElasticityCoefficientIds();
////        RRListHandle bList = createList(aList);
////		return bList;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////bool rrCallConv setCapabilities(const char* caps)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////
////        if(!caps)
////        {
////            return false;
////        }
////        rrHandle->setCapabilities(caps);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
////char* rrCallConv getCapabilities()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        return createText(rrHandle->getCapabilities());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////RRStringArrayHandle rrCallConv getEigenvalueIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////		StringList aList = rrHandle->getEigenvalueIds();
////		return createList(aList);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////RRListHandle rrCallConv getFluxControlCoefficientIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        return createList(rrHandle->getFluxControlCoefficientIds());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getUnscaledConcentrationControlCoefficientMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////		DoubleMatrix aMat = rrHandle->getUnscaledConcentrationControlCoefficientMatrix();
////        //return createMatrix(&(rrHandle->getUnscaledConcentrationControlCoefficientMatrix()));
////        return createMatrix(&(aMat));
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getScaledConcentrationControlCoefficientMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////		DoubleMatrix aMat = rrHandle->getScaledConcentrationControlCoefficientMatrix();
////        return createMatrix(&(aMat));
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getUnscaledFluxControlCoefficientMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        //return createMatrix(&(rrHandle->getUnscaledFluxControlCoefficientMatrix()));
////		DoubleMatrix aMat = rrHandle->getUnscaledFluxControlCoefficientMatrix();
////        return createMatrix(&(aMat));
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////RRMatrixHandle rrCallConv getScaledFluxControlCoefficientMatrix()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////
////        //return createMatrix(&(rrHandle->getScaledFluxControlCoefficientMatrix()));a
////		DoubleMatrix aMat = rrHandle->getScaledFluxControlCoefficientMatrix();
////        return createMatrix(&(aMat));
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////RRListHandle rrCallConv getUnscaledFluxControlCoefficientIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////		NewArrayList arrList = rrHandle->getUnscaledFluxControlCoefficientIds();
////        return createList(arrList);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////RRList* rrCallConv getConcentrationControlCoefficientIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        NewArrayList list = rrHandle->getConcentrationControlCoefficientIds();
////        return createList(list);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////RRListHandle rrCallConv getUnscaledConcentrationControlCoefficientIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        return createList(rrHandle->getUnscaledConcentrationControlCoefficientIds());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////int rrCallConv getNumberOfCompartments()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return -1;
////        }
////        return rrHandle->getNumberOfCompartments();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return -1;
////    }
////}
////
////bool rrCallConv getCompartmentByIndex(const int& index, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return false;
////        }
////        value = rrHandle->getCompartmentByIndex(index);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return false;
////    }
////}
////
////bool rrCallConv setCompartmentByIndex (const int& index, const double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        rrHandle->setCompartmentByIndex(index, value);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return false;
////    }
////}
////
////RRStringArrayHandle rrCallConv getCompartmentIds()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        return createList(rrHandle->getCompartmentIds());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////bool rrCallConv getRateOfChange(const int& index, double& value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////            return NULL;
////        }
////        value = rrHandle->getRateOfChange(index);
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
////// Utility functions ==========================================================
////
////int rrCallConv getNumberOfStringElements (const RRStringArrayHandle list)
////{
////	if (!list)
////		return (-1);
////	else
////	    return (list->Count);
////}
////
////char* rrCallConv getStringElement (RRStringArrayHandle list, int index)
////{
////	try
////	{
////	  if (list == NULL)
////	  {
////	     return NULL;
////	  }
////
////	  if ((index < 0) || (index >= list->Count))
////	  {
////         setError("Index out of range");
////         return NULL;
////	  }
////
////	  return createText(list->String[index]);
////	}
////	catch(Exception& ex)
////	{
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////char* rrCallConv stringArrayToString (const RRStringArrayHandle list)
////{
////	try
////    {
////        if(!list)
////        {
////            return NULL;
////        }
////
////		stringstream resStr;
////	    for(int i = 0; i < list->Count; i++)
////        {
////        	resStr<<list->String[i];;
////            if(i < list->Count -1)
////            {
////            	resStr <<" ";
////            }
////        }
////
////    	return createText(resStr.str());
////    }
////    catch(Exception& ex)
////    {
////        stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////		return NULL;
////    }
////}
////
////char* rrCallConv resultToString(const RRResultHandle result)
////{
////	try
////    {
////        if(!result)
////        {
////            return NULL;
////        }
////		stringstream resStr;
////		//RRResult is a 2D matrix, and column headers (strings)
////        //First header....
////	    for(int i = 0; i < result->CSize; i++)
////        {
////        	resStr<<result->ColumnHeaders[i];
////            if(i < result->CSize -1)
////            {
////            	resStr <<"\t";
////            }
////        }
////        resStr<<endl;
////
////        //Then the data
////        int index = 0;
////	    for(int j = 0; j < result->RSize; j++)
////   	    {
////		    for(int i = 0; i < result->CSize; i++)
////    	    {
////        		resStr<<result->Data[index++];
////	            if(i < result->CSize -1)
////    	        {
////        	    	resStr <<"\t";
////            	}
////            }
////	    	resStr <<"\n";
////        }
////        return createText(resStr.str());
////    }
////    catch(Exception& ex)
////    {
////        stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return NULL;
////    }
////}
////
////char* rrCallConv matrixToString(const RRMatrixHandle matrixHandle)
////{
////	try
////    {
////        if(!matrixHandle)
////        {
////            return NULL;
////        }
////
////        RRMatrix& mat = *matrixHandle;
////        stringstream ss;
////        //ss<<"\nmatrix dimension: "<<mat.RSize<<"x"<<mat.CSize<<" --\n";
////        ss<<"\n";
////        for(int row = 0; row < mat.RSize; row++)
////        {
////            for(int col = 0; col < mat.CSize; col++)
////            {
////                ss<<mat.Data[row*mat.CSize + col];
////                if(col < mat.CSize + 1)
////                {
////                    ss<<"\t";
////                }
////            }
////            ss<<endl;
////        }
////        return createText(ss.str());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
////char* rrCallConv vectorToString(RRVectorHandle vecHandle)
////{
////	try
////    {
////        if(!vecHandle)
////        {
////            setError("Null vector in vectorToString");
////            return NULL;
////        }
////
////        RRVector& vec = *vecHandle;
////
////        stringstream ss;
////        ss<<"vector dimension: "<<vec.Count<<" \n";
////
////        for(int index = 0; index < vec.Count; index++)
////        {
////            ss<<vec.Data[index];
////            if(index < vec.Count + 1)
////            {
////                ss<<"\t";
////            }
////        }
////        ss<<endl;
////        return createText(ss.str());
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////    	return NULL;
////    }
////}
////
// Free Functions =====================================================
bool rrCallConv freeRRHandle(RRHandle handle)
{
	try
    {
    	RoadRunner* rri = castToRRInstance(handle);
        delete rri;
        return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return false;
    }
}


////bool rrCallConv freeMatrix(RRMatrixHandle matrix)
////{
////	try
////    {
////        if(matrix)
////        {
////            delete [] (matrix->Data);
////            delete matrix;
////        }
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
////bool rrCallConv freeResult(RRResultHandle handle)
////{
////	try
////    {
////        delete handle;
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
bool rrCallConv freeText(char* text)
{
	try
    {
        if(text != ALLOCATE_API_ERROR_MSG)
        {
            delete [] text;
        }
        return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
    	return false;
    }
}

////bool rrCallConv freeStringArray(RRStringArrayHandle sl)
////{
////	try
////    {
////    	delete sl;
////    	return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
////bool rrCallConv freeVector(RRVectorHandle vector)
////{
////	try
////    {
////        if(vector)
////        {
////    	   delete [] vector->Data;
////        }
////        return true;
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////	    return false;
////    }
////}
////
bool rrCallConv freeCCode(RRCCodeHandle code)
{
	try
    {
        if(code)
        {
            delete code->Header;
            delete code->Source;
        }
		return true;
    }
    catch(Exception& ex)
    {
    	stringstream msg;
    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
        setError(msg.str());
	    return false;
    }
}

/////////////////////////////////////////////////////////////////
////void rrCallConv Pause()
////{
////    rr::Pause(true, "Hit any key to continue..\n");
////}
////
////RRVectorHandle rrCallConv createVector (int size)
////{
////   RRVectorHandle list = new RRVector;
////   list->Count = size;
////   list->Data = new double[list->Count];
////   return list;
////}
////
////int rrCallConv getVectorLength (RRVectorHandle vector)
////{
////	if (vector == NULL)
////    {
////		setError ("Vector argument is null in getVectorLength");
////		return -1;
////	}
////	else
////    {
////		return vector->Count;
////    }
////}
////
////bool rrCallConv getVectorElement (RRVectorHandle vector, int index, double& value)
////{
////	if (vector == NULL)
////    {
////		setError ("Vector argument is null in getVectorElement");
////		return false;
////	}
////
////	if ((index < 0) || (index >= vector->Count))
////    {
////		stringstream msg;
////		msg << "Index out range in getVectorElement: " << index;
////        setError(msg.str());
////		return false;
////	}
////
////	value = vector->Data[index];
////	return true;
////}
////
////bool rrCallConv setVectorElement (RRVectorHandle vector, int index, double value)
////{
////	if (vector == NULL)
////    {
////		setError ("Vector argument is null in setVectorElement");
////		return false;
////	}
////
////	if ((index < 0) || (index >= vector->Count))
////    {
////		stringstream msg;
////		msg << "Index out range in setVectorElement: " << index;
////        setError(msg.str());
////		return false;
////	}
////
////	vector->Data[index] = value;
////	return true;
////}
////
////// Matrix Routines
////// ------------------------------------------------------------------------------------
////
////RRMatrixHandle rrCallConv createRRMatrix (int r, int c)
////{
////   	RRMatrixHandle matrix = new RRMatrix;
////   	matrix->RSize = r;
////   	matrix->CSize = c;
////   	int dim =  matrix->RSize * matrix->CSize;
////   	if(dim)
////   	{
////		matrix->Data =  new double[dim];
////		return matrix;
////   	}
////   	else
////	{
////        delete matrix;
////		setError ("Dimensions for new RRMatrix in createRRMatrix are zero");
////        return NULL;
////   }
////}
////
////int rrCallConv getMatrixNumRows (RRMatrixHandle m)
////{
////	if (m == NULL) {
////		setError ("Matrix argument is null in getMatrixNumRows");
////		return -1;
////	}
////	return m->RSize;
////}
////
////int  rrCallConv getMatrixNumCols (RRMatrixHandle m)
////{
////	if (m == NULL) {
////		setError ("Matrix argument is null in getMatrixNumCols");
////		return -1;
////	}
////
////	return m->CSize;
////}
////
////bool rrCallConv getMatrixElement (RRMatrixHandle m, int r, int c, double& value)
////{
////	if (m == NULL) {
////		return false;
////		setError ("Matrix argument is null in getMatrixElement");
////	}
////
////	if ((r < 0) || (c < 0) || (r >= m->RSize) || (c >= m->CSize)) {
////		stringstream msg;
////		msg << "Index out range in getMatrixElement: " << r << ", " << c;
////        setError(msg.str());
////		return false;
////	}
////	value = m->Data[r*m->CSize + c];
////	return true;
////}
////
////bool rrCallConv setMatrixElement (RRMatrixHandle m, int r, int c, double value)
////{
////	if (m == NULL) {
////		setError ("Matrix argument is null in setMatrixElement");
////	    return false;
////	}
////
////	if ((r < 0) || (c < 0) || (r >= m->RSize) || (c >= m->CSize)) {
////		stringstream msg;
////		msg << "Index out range in setMatrixElement: " << r << ", " << c;
////        setError(msg.str());
////		return false;
////	}
////	m->Data[r*m->CSize + c] = value;
////	return true;
////}
////
////int rrCallConv  getResultNumRows (RRResultHandle result)
////{
////	if (result == NULL)
////    {
////       setError ("result argument is null in getResultNumRows");
////       return -1;
////	}
////	return result->RSize;
////}
////
////int  rrCallConv  getResultNumCols (RRResultHandle result)
////{
////	if (result == NULL)
////    {
////       setError ("result argument is null in getResultNumCols");
////       return -1;
////	}
////	return result->CSize;
////}
////
////bool  rrCallConv getResultElement(RRResultHandle result, int r, int c, double& value)
////{
////	if (result == NULL)
////    {
////	   setError ("result argument is null in getResultElement");
////       return false;
////	}
////
////	if ((r < 0) || (c < 0) || (r >= result->RSize) || (c >= result->CSize))
////    {
////		stringstream msg;
////		msg << "Index out range in getResultElement: " << r << ", " << c;
////        setError(msg.str());
////		return false;
////    }
////
////	value = result->Data[r*result->CSize + c];
////	return true;
////}
////
////char*  rrCallConv getResultColumnLabel (RRResultHandle result, int column)
////{
////	if (result == NULL)
////    {
////	   setError ("result argument is null in getResultColumnLabel");
////       return NULL;
////	}
////
////	if ((column < 0) || (column >= result->CSize))
////    {
////		stringstream msg;
////		msg << "Index out range in getResultColumnLabel: " << column;
////        setError(msg.str());
////		return NULL;
////    }
////
////	return result->ColumnHeaders[column];
////}
////
////char* rrCallConv getCCodeHeader(RRCCodeHandle code)
////{
////	if (code == NULL)
////    {
////    	setError ("code argument is null in getCCodeHeader");
////		return NULL;
////    }
////	return code->Header;
////}
////
////char* rrCallConv getCCodeSource(RRCCodeHandle code)
////{
////	if (code == NULL)
////    {
////        setError ("code argument is null in getCCodeSource");
////		return NULL;
////    }
////	return code->Source;
////}
////
////// -------------------------------------------------------------------
////// List Routines
////// -------------------------------------------------------------------
////RRListHandle rrCallConv createRRList()
////{
////	RRListHandle list = new RRList;
////	list->Count = 0;
////	list->Items = NULL;
////	return list;
////}
////
////void rrCallConv freeRRList (RRListHandle theList)
////{
////	if(!theList)
////    {
////    	return;
////    }
////    int itemCount = theList->Count;
////    for(int i = 0; i < itemCount; i++)
////    {
////        if(theList->Items[i]->ItemType == litString)
////        {
////              delete [] theList->Items[i]->data.sValue;
////        }
////        if(theList->Items[i]->ItemType == litList)
////        {
////            freeRRList ((RRList *) theList->Items[i]->data.lValue);
////        }
////        delete theList->Items[i];
////    }
////	delete [] theList->Items;
////    delete theList;
////    theList = NULL;
////}
////
////RRListItemHandle rrCallConv createIntegerItem (int value)
////{
////	RRListItemHandle item =  new RRListItem;
////	item->ItemType = litInteger;
////	item->data.iValue = value;
////	return item;
////}
////
////RRListItemHandle rrCallConv createDoubleItem (double value)
////{
////	RRListItemHandle item = new RRListItem;
////	item->ItemType = litDouble;
////	item->data.dValue = value;
////	return item;
////}
////
////RRListItemHandle rrCallConv createStringItem (char* value)
////{
////	RRListItemHandle item = new RRListItem;
////	item->ItemType = litString;
////	item->data.sValue = createText(value);
////	return item;
////}
////
////RRListItemHandle rrCallConv createListItem (RRList* value)
////{
////	RRListItemHandle item = new RRListItem;
////	item->ItemType = litList;
////	item->data.lValue = value;
////	return item;
////}
////
////// Add an item to a given list, returns the index to
////// the item in the list. Returns -1 if it fails.
////int rrCallConv addItem (RRListHandle list, RRListItemHandle *item)
////{
////	int n = list->Count;
////
////	RRListItemHandle *newItems = new RRListItemHandle [n+1];
////    if(!newItems)
////    {
////    	setError("Failed allocating memory in addItem()");
////    	return -1;
////    }
////
////    for(int i = 0; i < n; i++)
////    {
////    	newItems[i] = list->Items[i];
////    }
////
////    newItems[n] = *item;
////    RRListItemHandle *oldItems = list->Items;
////    list->Items = newItems;
////
////    delete [] oldItems;
////
////	list->Count = n+1;
////	return n;
////}
////
////bool rrCallConv isListItemInteger (RRListItemHandle item)
////{
////	return (item->ItemType == litInteger) ? true : false;
////}
////
////bool rrCallConv isListItemDouble (RRListItemHandle item)
////{
////	return (item->ItemType == litDouble) ? true : false;
////}
////
////bool rrCallConv isListItemString (RRListItemHandle item)
////{
////	return (item->ItemType == litString) ? true : false;
////}
////
////bool rrCallConv isListItemList (RRListItemHandle item)
////{
////	return (item->ItemType == litList) ? true : false;
////}
////
////RRListItemHandle rrCallConv getListItem (RRListHandle list, int index)
////{
////	return (index >= list->Count) ? NULL : list->Items[index];
////}
////
////bool rrCallConv getIntegerListItem (RRListItemHandle item, int &value)
////{
////    if (item->ItemType == litInteger)
////    {
////        value = item->data.iValue;
////        return true;
////    }
////    return false;
////}
////
////bool rrCallConv getDoubleListItem (RRListItemHandle item, double &value)
////{
////    if (item->ItemType == litDouble)
////    {
////    	value = item->data.dValue;
////     	return true;
////    }
////
////    return false;
////}
////
////char* rrCallConv getStringListItem (RRListItemHandle item)
////{
////	return (item->ItemType == litString) ? item->data.sValue : NULL;
////}
////
////RRListHandle rrCallConv getList (RRListItemHandle item)
////{
////	return (item->ItemType == litList) ? item->data.lValue : NULL;
////}
////
////bool rrCallConv isListItem (RRListItemHandle item, ListItemType itemType)
////{
////	return  (item->ItemType == itemType) ? true : false;
////}
////
////int rrCallConv getListLength (RRListHandle myList)
////{
////	return myList->Count;
////}
////
////char* rrCallConv listToString (RRListHandle list)
////{
////	try
////    {
////        if(!list)
////        {
////            return NULL;
////        }
////
////        //Types of list items
////        char*           cVal;
////        int             intVal;
////        double          dVal;
////        RRList*        lVal; 		//list is nested list
////		stringstream resStr;
////        resStr<<"{";
////	    for(int i = 0; i < list->Count; i++)
////        {
////			switch(list->Items[i]->ItemType)
////            {
////                case litString:
////					cVal = list->Items[i]->data.sValue;
////                    resStr<<"\"";
////                    if(cVal)
////                    {
////                    	resStr<<cVal;
////                    }
////                    resStr<<"\"";
////                break;
////
////                case litInteger:
////					intVal = list->Items[i]->data.iValue;
////                    resStr<< (intVal);
////                break;
////
////                case litDouble:
////					dVal =  list->Items[i]->data.dValue;
////                    resStr<< (dVal);
////                break;
////
////                case litList:
////					lVal = list->Items[i]->data.lValue;
////                    if(lVal)
////                    {
////                    	char* text = listToString(lVal);
////                    	resStr<<text;
////                        freeText(text);
////                    }
////                    else
////                    {
////						resStr<<"{}";
////                    }
////                break;
////            }
////
////            if(i < list->Count -1)
////            {
////                resStr<<",";
////            }
////        }
////        resStr<<"}";
////        return createText(resStr.str());
////
////    }
////    catch(Exception& ex)
////    {
////        stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////        return NULL;
////    }
////}
////
////
//////PLUGIN Functions
////bool rrCallConv loadPlugins()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////    	return rrHandle->getPluginManager().load();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return false;
////    }
////}
////
////bool rrCallConv unLoadPlugins()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////    	return rrHandle->getPluginManager().unload();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return false;
////    }
////}
////
////int rrCallConv getNumberOfPlugins()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////    	return rrHandle->getPluginManager().getNumberOfPlugins();
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return -1;
////    }
////}
////
////RRStringArray* rrCallConv getPluginNames()
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////        StringList names = rrHandle->getPluginManager().getPluginNames();
////        return createList(names);
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return NULL;
////    }
////}
////
////RRStringArray* rrCallConv getPluginCapabilities(const char* pluginName)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////        Plugin* aPlugin = rrHandle->getPluginManager().getPlugin(pluginName);
////        if(aPlugin)
////        {
////        	StringList aList;
////            vector<Capability>* caps = aPlugin->getCapabilities();
////            if(!caps)
////            {
////            	return NULL;
////            }
////
////            for(int i = 0; i < caps->size(); i++)
////            {
////            	aList.Add((*caps)[i].getName());
////            }
////        	return createList(aList);
////        }
////        else
////        {
////	        return NULL;
////        }
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return NULL;
////    }
////}
////
////RRStringArray* rrCallConv getPluginParameters(const char* pluginName, const char* capability)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////        Plugin* aPlugin = rrHandle->getPluginManager().getPlugin(pluginName);
////        if(aPlugin)
////        {
////        	StringList aList;
////            Parameters* paras = aPlugin->getParameters(capability);
////            if(!paras)
////            {
////            	return NULL;
////            }
////
////            for(int i = 0; i < paras->size(); i++)
////            {
////            	aList.Add((*paras)[i]->getName());
////            }
////        	return createList(aList);
////        }
////        else
////        {
////	        return NULL;
////        }
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return NULL;
////    }
////}
////
////RRParameter* rrCallConv getPluginParameter(const char* pluginName, const char* parameterName)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////        Plugin* aPlugin = rrHandle->getPluginManager().getPlugin(pluginName);
////        if(aPlugin)
////        {
////
////            rr::BaseParameter *para = aPlugin->getParameter(parameterName);
////        	return createParameter( *(para) );
////        }
////        return NULL;
////
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return NULL;
////    }
////}
////
////bool rrCallConv setPluginParameter(const char* pluginName, const char* parameterName, const char* value)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////        Plugin* aPlugin = rrHandle->getPluginManager().getPlugin(pluginName);
////        if(aPlugin)
////        {
////            return aPlugin->setParameter(parameterName, value);
////        }
////		return false;
////
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return NULL;
////    }
////}
////
////char* rrCallConv getPluginInfo(const char* name)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////        Plugin* aPlugin = rrHandle->getPluginManager().getPlugin(name);
////        if(aPlugin)
////        {
////        	return createText(aPlugin->getInfo());
////        }
////        else
////        {
////	        return createText("No such plugin: " + string(name));
////        }
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return NULL;
////    }
////}
////
////bool rrCallConv executePlugin(const char* name)
////{
////	try
////    {
////        if(!rrHandle)
////        {
////            setError(ALLOCATE_API_ERROR_MSG);
////        }
////
////        Plugin* aPlugin = rrHandle->getPluginManager().getPlugin(name);
////        if(aPlugin)
////        {
////        	return aPlugin->execute();
////        }
////        else
////        {
////			return false;
////        }
////    }
////    catch(Exception& ex)
////    {
////    	stringstream msg;
////    	msg<<"RoadRunner exception: "<<ex.what()<<endl;
////        setError(msg.str());
////  	    return NULL;
////    }
////}
//
////We only need to give the linker the folder where libs are
////using the pragma comment. Works for MSVC and codegear
#if defined(CG_IDE)
//#pragma comment(lib, "roadrunner.lib")
#pragma comment(lib, "roadrunner-static.lib")
#pragma comment(lib, "rr-libstruct-static.lib")
#pragma comment(lib, "pugi-static.lib")
#pragma comment(lib, "libsbml-static.lib")
#pragma comment(lib, "sundials_cvode.lib")
#pragma comment(lib, "sundials_nvecserial.lib")
#pragma comment(lib, "libxml2_xe.lib")
#pragma comment(lib, "blas.lib")
#pragma comment(lib, "lapack.lib")
#pragma comment(lib, "libf2c.lib")
#pragma comment(lib, "poco_foundation-static.lib")
#pragma comment(lib, "nleq-static.lib")
#endif
