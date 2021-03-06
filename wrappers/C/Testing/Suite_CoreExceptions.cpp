#include <string>
#include "rrLogger.h"
#include "UnitTest++.h"
#include "rrc_core_api.h"
#include "rrUtils.h"
#include "rrIniFile.h"
#include "TestUtils.h"

//Add using clauses..
using namespace std;
using namespace UnitTest;
using namespace rrc;

using rr::joinPath;
using rr::fileExists;

extern string   gTempFolder;
extern string   gDataOutputFolder;
extern string 	gTestDataFolder;
extern string   gRRInstallFolder;

extern bool 	gDebug;

//This tests is mimicking the Python tests
SUITE(CORE_EXCEPTIONS)
{
string TestDataFileName 	= "TestModel_1.dat";
IniFile iniFile;
string TestModelFileName;
RRHandle gRR;

	TEST(DATA_FILES)
	{
		gTestDataFolder 			= joinPath(gRRInstallFolder, "tests");
		string testDataFileName 	= joinPath(gTestDataFolder, TestDataFileName);

		CHECK(fileExists(testDataFileName));
		CHECK(iniFile.Load(testDataFileName));

		clog<<"Loaded test data from file: "<< testDataFileName;
		if(iniFile.GetSection("SBML_FILES"))
		{
			IniSection* sbml = iniFile.GetSection("SBML_FILES");
			IniKey* fNameKey = sbml->GetKey("FNAME1");
			if(fNameKey)
			{
				TestModelFileName  = joinPath(gTestDataFolder, fNameKey->mValue);
				CHECK(fileExists(TestModelFileName));
			}
		}
        else
        {
        	CHECK(false);
        }
	}

	TEST(LOAD_SBML)
	{
		CHECK(loadSBMLFromFile(gRR, TestModelFileName.c_str()));
	}

	TEST(SET_COMPUTE_AND_ASSIGN_CONSERVATION_LAWS)
	{
		//gRR = createRRInstance();
		CHECK(gRR!=NULL);
		bool res = setComputeAndAssignConservationLaws(gRR, true);
		CHECK(res);
		clog<<"\nConversation laws: "<<res<<endl;
	}

    TEST(GET_UNSCALED_ELASTICITY_COEFFICIENT)
    {
		double test;
		bool val =  getuEE(gRR, "J1", "S1", &test);

       // val = getuEE("J1", "S34", test);

		//val =  getuCC("J1", "S1", test);

    }


}


