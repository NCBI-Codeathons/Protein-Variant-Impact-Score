/* ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * File Name:
 *
 * Author:  Jiyao Wang
 *
 * File Description:
 *
 */

#include "pssmCgi.hpp"

#include <internal/structure/tools.hpp>

#include <ncbi_pch.hpp>
#include <corelib/ncbistd.hpp>
#include <corelib/ncbidiag.hpp>
#include <corelib/ncbistr.hpp>
#include <corelib/ncbiexpt.hpp>
#include <connect/ncbi_conn_stream.hpp>
#include <connect/ncbi_core_cxx.hpp>
#include <connect/ncbi_util.h>

#include <corelib/ncbi_system.hpp>

#include <cgi/ncbicgi.hpp>
#include <cgi/cgiapp.hpp>
#include <cgi/cgictx.hpp>
#include <html/html.hpp>
#include <html/page.hpp>
#include <corelib/ncbistre.hpp>
#include <stdlib.h>
#include <string>

#include <time.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
//#include <string>
#include <array>

#define WARNINGMSG(stream) ERR_POST(Warning << stream)
#define ERRORMSG(stream) ERR_POST(Error << stream)

// error exception
#define ERRORTHROW(stream) do { \
        CNcbiOstrstream oss; \
        oss << stream << '\0'; \
        auto_ptr < char > s(oss.str()); \
        NCBI_THROW(CException, eUnknown, s.get()); \
    } while (0)

USING_NCBI_SCOPE;
//using namespace SHProjNS;

//CConstRef<SHProjNS::CShdbInit> log_ini;

CPssmCgi::CPssmCgi()
{
	m_output = "";
	m_errMess = "";
}

CPssmCgi::~CPssmCgi()
{
}

/*
int CPssmCgi::ProcessRequest(CCgiContext& ctx)
{
 CCgiResponse& response = ctx.GetResponse();
 bool headerWritten = false;

 try {
	  const CNcbiRegistry& reg = ctx.GetConfig();
	  const TCgiEntries& entries = ctx.GetRequest().GetEntries();
	  const CCgiRequest& request = ctx.GetRequest();

	  m_ftppath = reg.Get("option", "ftp");

	  TCgiEntriesCI it, it2;
	  int i;

	  CRef<CNCBINode> textNode;

	  //if((it = entries.find("chainpairs")) != entries.end() && it->second != "") {
		  //string mmdb_db_aliase = reg.Get("DATABASE", "PubStruct");
		  //MmdbSrvInitialize(log_ini, mmdb_db_aliase);

		  //string chainpairs = it->second; // e.g., 1HHO_A,4N7N_A|1HHO_B,4N7N_B

		  string gilist = "664806073,6912622";
		  getPssmScores(gilist);

		  //MmdbSrvFinish();
	  //}

	  string callback;
	  if( (it = entries.find("callback")) != entries.end() ) {
		callback = it->second;

		if(IsValidJSONPCallback(callback)) {
		  m_output = callback + "(" + m_output + ");";
		}
	  }

	  textNode = new CHTMLText(m_output);

	  if( (it = entries.find("callback")) != entries.end() ) {
		response.SetContentType("application/json");
	  }

	  response.SetHeaderValue("Cache-Control", "max-age=86400");
	  response.WriteHeader();
	  textNode->Print(response.out(), CNCBINode::eXHTML);

	  //response.Flush();
	  headerWritten = true;
  //response.Flush();
 } catch (CException& e) {
     ERRORMSG("Caught NCBI CException: " << e.GetMsg());
 } catch (exception& e) {
     ERRORMSG("Caught std::exception: " << e.what());
 } catch (...) {
     ERRORMSG("Caught exception of unknown type");
 }

 if (!headerWritten) {
   // HTML (well, really just plain text) response
   response.SetContentType("text/html");
   response.WriteHeader();
   response.out() << "<div>Oops there was a problem!</div>\n";
 }

 response.Flush();

 return 0;
}
*/

bool CPssmCgi::setPssmRanges(const string gi)
{
    string url = "https://www.ncbi.nlm.nih.gov/Structure/cdannots/cdannots.fcgi?fmt&queries=" + gi;

    CConn_HttpStream istr(url);
    if (!istr.good() && !istr.eof()) {
        return false;
    }

    string json_str;
    NcbiStreamToString(&json_str, istr);

    CJson_Document json_doc;

    if (!json_doc.ParseString(json_str)) {
        m_errMess += "Unable to parse the json from cdannots.fcgi...";
        return false;
    }

    CJson_ConstObject::const_iterator element;

	// http://intranet.ncbi.nlm.nih.gov/ieb/ToolBox/CPP_DOC/lxr/source/src/internal/structure/PubChem/doi_api/doi_api.cpp#L275
	// http://intranet.ncbi.nlm.nih.gov/ieb/ToolBox/CPP_DOC/lxr/source/src/internal/structure/PubChem/doi_api/doi_api.cpp#L275
	// http://intranet.ncbi.nlm.nih.gov/ieb/ToolBox/CPP_DOC/lxr/source/include/misc/jsonwrapp/jsonwrapp11.hpp#L267
	element = json_doc.GetObject().find("data");
	if (element != json_doc.GetObject().end() && element->value.IsArray()) {
	  CJson_ConstArray giArray = element->value.GetArray();
	  for (size_t i=0; i < giArray.size(); ++i) {
		// each gi
		set<unsigned> sCoveredResAll; // all previously covered residues

		if (giArray.at(i).IsObject()) {
		  CJson_ConstObject obj = giArray.at(i).GetObject();

		  element = obj.find("doms");
		  if(element != obj.end() && element->value.IsArray()) {
			  CJson_ConstArray domArray = element->value.GetArray();
			  for (size_t j=0; j < domArray.size(); ++j) {
				// each domain
				vector< vector<unsigned> > vFromTo, vFromToOri;
				set<unsigned> sCoveredResCurr; // covered residues for this pssmid

				string pssmid, type;

				if (domArray.at(j).IsObject()) {
				  CJson_ConstObject obj = domArray.at(j).GetObject();

				  element = obj.find("acc");
				  pssmid = (element != obj.end() && element->value.IsValue() && element->value.GetValue().IsString()) ? element->value.GetValue().GetString() : "";

				  element = obj.find("type");
				  type = (element != obj.end() && element->value.IsValue() && element->value.GetValue().IsString()) ? element->value.GetValue().GetString() : "";

		  element = obj.find("locs");
		  if(element != obj.end() && element->value.IsArray()) {
			  CJson_ConstArray locsArray = element->value.GetArray();
			  for (size_t k=0; k < locsArray.size(); ++k) {
				// each repeat of domain
				if (locsArray.at(k).IsObject()) {
				  CJson_ConstObject obj = locsArray.at(k).GetObject();

				  if(type == "superfamily") {
					  element = obj.find("alnacc");
					  if(element != obj.end() && element->value.IsValue() && element->value.GetValue().IsString()) {
						  pssmid = element->value.GetValue().GetString();
					  }
					  else { // missing alnacc
						  continue; // skip
					  }
				  }

				  element = obj.find("realsegs");
				  if(element != obj.end() && element->value.IsArray()) {
					  CJson_ConstArray segsArray = element->value.GetArray();
					  for (size_t l=0; l < segsArray.size(); ++l) {
						// each segs
						if (segsArray.at(l).IsObject()) {
						  CJson_ConstObject obj = segsArray.at(l).GetObject();

						  element = obj.find("from");
						  unsigned from = (element != obj.end() && element->value.IsValue() && element->value.GetValue().IsUint4()) ? element->value.GetValue().GetUint4(): 0;

						  element = obj.find("to");
						  unsigned to = (element != obj.end() && element->value.IsValue() && element->value.GetValue().IsUint4()) ? element->value.GetValue().GetUint4(): 0;

						  element = obj.find("orifrom");
						  unsigned orifrom = (element != obj.end() && element->value.IsValue() && element->value.GetValue().IsUint4()) ? element->value.GetValue().GetUint4(): 0;

						  element = obj.find("orito");
						  unsigned orito = (element != obj.end() && element->value.IsValue() && element->value.GetValue().IsUint4()) ? element->value.GetValue().GetUint4(): 0;

						  vector<unsigned> FromTo;
						  FromTo.push_back(from);
						  FromTo.push_back(to);

						  vFromTo.push_back(FromTo);

						  vector<unsigned> FromToOri;
						  FromToOri.push_back(orifrom);
						  FromToOri.push_back(orito);

						  vFromToOri.push_back(FromToOri);

						  // check whether the region was covered by previous domain
						  for(unsigned m = from; m <= to; ++m) {
							  if(sCoveredResAll.find(m) == sCoveredResAll.end()) { // previously NOT covered
								  sCoveredResCurr.insert(m);
								  sCoveredResAll.insert(m);
							  }
						  }
						}
					  }
				  }
				}
		      } // for
	  	  }
				} // end each domain

				mPssmid2VFromTo.insert(make_pair(pssmid, vFromTo));
				mPssmid2VFromToOri.insert(make_pair(pssmid, vFromToOri));
				mPssmid2SCoveredRes.insert(make_pair(pssmid, sCoveredResCurr));
				sCoveredResCurr.clear();
		      } // for each domain
	  	  }
		} // end each gi
	  } // for each gi
	}

	setSites(json_doc);

    return true;
}

bool CPssmCgi::setSites(CJson_Document & json_doc)
{
    CJson_ConstObject::const_iterator element;

	// http://intranet.ncbi.nlm.nih.gov/ieb/ToolBox/CPP_DOC/lxr/source/src/internal/structure/PubChem/doi_api/doi_api.cpp#L275
	// http://intranet.ncbi.nlm.nih.gov/ieb/ToolBox/CPP_DOC/lxr/source/src/internal/structure/PubChem/doi_api/doi_api.cpp#L275
	// http://intranet.ncbi.nlm.nih.gov/ieb/ToolBox/CPP_DOC/lxr/source/include/misc/jsonwrapp/jsonwrapp11.hpp#L267
	element = json_doc.GetObject().find("data");
	if (element != json_doc.GetObject().end() && element->value.IsArray()) {
	  CJson_ConstArray giArray = element->value.GetArray();
	  for (size_t i=0; i < giArray.size(); ++i) {
		// each gi
		//set<unsigned> sCoveredResAll; // all previously covered residues

		if (giArray.at(i).IsObject()) {
		  CJson_ConstObject obj = giArray.at(i).GetObject();

		  element = obj.find("sites");
		  if(element != obj.end() && element->value.IsArray()) {
			  CJson_ConstArray domArray = element->value.GetArray();
			  for (size_t j=0; j < domArray.size(); ++j) {
				// each domain
				//vector< vector<unsigned> > vFromTo, vFromToOri;
				set<unsigned> sCoveredResCurr; // covered residues for this pssmid

				string pssmid, title;

				if (domArray.at(j).IsObject()) {
				  CJson_ConstObject obj = domArray.at(j).GetObject();

				  element = obj.find("srcdom");
				  pssmid = (element != obj.end() && element->value.IsValue() && element->value.GetValue().IsString()) ? element->value.GetValue().GetString() : "";

				  element = obj.find("title");
				  title = (element != obj.end() && element->value.IsValue() && element->value.GetValue().IsString()) ? element->value.GetValue().GetString() : "";

		  element = obj.find("locs");
		  if(element != obj.end() && element->value.IsArray()) {
			  CJson_ConstArray locsArray = element->value.GetArray();
			  for (size_t k=0; k < locsArray.size(); ++k) {
				// each repeat of domain
				if (locsArray.at(k).IsObject()) {
				  CJson_ConstObject obj = locsArray.at(k).GetObject();

				  //element = obj.find("alnacc");
				  //if(element != obj.end() && element->value.IsValue() && element->value.GetValue().IsString()) {
				  //	  pssmid = element->value.GetValue().GetString();
				  //}

				  element = obj.find("coords");
				  if(element != obj.end() && element->value.IsArray()) {
					  CJson_ConstArray segsArray = element->value.GetArray();
					  for (size_t l=0; l < segsArray.size(); ++l) {
						// each segs
						if (segsArray.at(l).IsValue() && segsArray.at(l).GetValue().IsUint4()) {
						  unsigned pos = segsArray.at(l).GetValue().GetUint4();

						  sCoveredResCurr.insert(pos);
						}
					  } // for
				  }
				}
		      }
	  	  }
				} // end each domain

				if(mSite2SCoveredRes.find(title) == mSite2SCoveredRes.end()) {
					mSite2SCoveredRes.insert(make_pair(title, sCoveredResCurr));
				}
				else {
					mSite2SCoveredRes[title].insert(sCoveredResCurr.begin(), sCoveredResCurr.end());
				}

				sCoveredResCurr.clear();
		      } // for each domain
	  	  }
		} // end each gi
	  } // for each gi
	}

    return true;
}

bool CPssmCgi::getPssmScores(unsigned resPos)
{
	m_output = "{\"sites\": [";

	unsigned cnt = 0, cnt2 = 0;
	string currStr;

	map<string, set<unsigned> >::iterator it;
	cnt = 0;
	for(it = mSite2SCoveredRes.begin(); it != mSite2SCoveredRes.end(); ++it) {
		string title = it->first;

		set<unsigned> sPos;
		sPos = it->second;

		set<unsigned>::iterator itSet;
		cnt2 = 0;
		currStr = "";
		for(itSet = sPos.begin(); itSet != sPos.end(); ++itSet) {
			if(resPos == 0 || resPos == *itSet + 1) {
				currStr += NStr::IntToString(*itSet + 1) + ",";
				++cnt2;
			}
		}
		if(cnt2 > 0) {
			currStr = currStr.substr(0, currStr.size() - 1);
			m_output += "{\"title\": \"" + title + "\", \"resiPos\": [";
			m_output += currStr;
			m_output += "]},";

			++cnt;
		}
	}
	if(cnt > 0) m_output = m_output.substr(0, m_output.size() - 1);

	m_output += "], \"pssmlist\": [";

	map<string, vector< vector< unsigned > > >::iterator itMap, itMapOri;
	cnt = 0;
	for(itMap = mPssmid2VFromTo.begin(); itMap != mPssmid2VFromTo.end(); ++itMap) {
		string pssmid = itMap->first;

	    string filepath = "/panfs/pan1/cdd_add/users/hurwitz/get_pssm_data_from_file/";

	    string filename = filepath + pssmid + ".pssm.txt";

		ifstream inFile;

		inFile.open(filename);
		if (!inFile) {
			m_errMess += "Unable to open file " + filename;
			return false;
		}

		string term;
		unsigned termCnt = 0, termPerLine = 20;
		vector<string> vResName;
		vector< int > vScore;
		vector< vector< int > > vPosScore;
		while (inFile >> term) {
			if(termCnt < termPerLine) { // residue names
				vResName.push_back(term);
			}
			else {
				int score = (int) round(NStr::StringToInt(term) / 100.0);
				vScore.push_back(score);
			}

			++termCnt;

			if(termCnt % termPerLine == 0 && termCnt > termPerLine) {
				vPosScore.push_back(vScore);
				vScore.clear();
			}
		}

		inFile.close();

		vector< vector< unsigned > > vFromTo, vFromToOri;
		vFromTo = itMap->second;

		itMapOri = mPssmid2VFromToOri.find(pssmid);
		vFromToOri = itMapOri->second;

		map<string, set<unsigned> >::iterator itMapSet;
		set<unsigned> sCoveredResCurr;

		itMapSet = mPssmid2SCoveredRes.find(pssmid);
		sCoveredResCurr = itMapSet->second;

		cnt2 = 0;
		currStr = "";
		for(unsigned i = 0; i < vFromTo.size(); ++i) {
			for(unsigned j = vFromTo[i][0]; j <= vFromTo[i][1]; ++j) {
				if(sCoveredResCurr.find(j) != sCoveredResCurr.end()) {
					if(resPos == 0 || resPos == j+1) {
						unsigned linenum = vFromToOri[i][0] + j - vFromTo[i][0];
						// convert from 0-based to 1-based
						currStr += "{\"resi\": " + NStr::IntToString(j+1) + ", \"pline\": " + NStr::IntToString(linenum+1) + ", ";
						currStr += "\"scores\": {";
						//cout<<"line "<<linenum<<", "<<" resi "<<j<<": ";
						for(unsigned k = 0; k < vPosScore[linenum].size(); ++k) {
							//cout<<vPosScore[linenum][k]<<",";
							string resName = vResName[k];
							currStr += "\"" + resName + "\": " + NStr::IntToString(vPosScore[linenum][k]);
							if(k != vPosScore[linenum].size() - 1) currStr += ", ";
						}
						currStr += "}},";
						++cnt2;
					}
				}
			}
		}

		if(cnt2 > 0) {
			currStr = currStr.substr(0, currStr.size() - 1);
			m_output += "{\"pssmid\": \"" + pssmid + "\", \"residues\": [";
			m_output += currStr;
			m_output += "]},";

			++cnt;
		}
	}

	if(cnt > 0) m_output = m_output.substr(0, m_output.size() - 1);

	m_output += "]}";

	return true;
}

int main(int argc, const char* argv[])
{
/*
    SetSplitLogFile(true);
    GetDiagContext().SetOldPostFormat(false);

	if (!SHProjNS::HasValidShdbInitObj())
			log_ini = new CShdbInit(true, "pssm");
	else
			log_ini = CShdbInit::GetShdbInitObject();

    return CPssmCgi().AppMain(argc, argv, 0, eDS_Default, \
        "pssm.cgi.ini", "st-pssm");
*/

  try {
	if (argc == 1) {
		cout<<"Usage: pssm gi residuePosition"<<endl;
		exit(1);
	}

	CPssmCgi* pssmcgi = new CPssmCgi();

	//string gi = "664806073";
	//string gi = "6912622";
	//string gi = "4557757";
	string gi = argv[1];

	if(!pssmcgi->setPssmRanges(gi)) {
		cout<<"Error in setPssmRanges"<<endl;
		exit(1);
	}

	unsigned pos = 0;
	if(argc == 3) pos = NStr::StringToUInt(argv[2]);
	if(!pssmcgi->getPssmScores(pos)) {
		cout<<"Error in getPssmScores"<<endl;
		exit(1);
	}

	string text;

	if(pssmcgi->m_errMess =="") {
		text = "{\"data\": " + pssmcgi->m_output + "}";
	}
	else {
		text = "{\"data\": " + pssmcgi->m_output + ", \"error\": " + pssmcgi->m_errMess + "}";
	}

	cout<<text<<endl;

    return 0;
/*
  } catch(const StructureException & ex) {
    exit(1);
  } catch(const CSHException & ex) {
    exit(1);
  } catch(const exception & ex) {
    exit(1);
*/
  } catch(...) {
    exit(1);
  }
}



