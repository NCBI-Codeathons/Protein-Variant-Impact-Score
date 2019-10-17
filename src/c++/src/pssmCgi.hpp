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

#ifndef CPSSM_CGI
#define CPSSM_CGI

#include <misc/jsonwrapp/jsonwrapp.hpp>
#include <serial/objostrjson.hpp>
#include <serial/objistr.hpp>

#include <cgi/cgiapp.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <set>
#include <map>
#include <string>

using namespace std;
USING_NCBI_SCOPE;

/////////////////////////////////////////////////////////////////////////////
//  CPssmCgi::

class CPssmCgi //: public CCgiApplication
//class CPssmCgi
{
public:
  CPssmCgi();
  ~CPssmCgi();

//  virtual int ProcessRequest(CCgiContext& ctx);

  bool setPssmRanges(const string gi);
  bool setSites(CJson_Document & json_doc);
  bool getPssmScores(unsigned resPos);

  string m_output, m_errMess;

private:
  map<string, set<unsigned> > mPssmid2SCoveredRes, mSite2SCoveredRes;
  map<string, vector< vector<unsigned> > > mPssmid2VFromTo, mPssmid2VFromToOri; // ...Ori means the resi in consensus sequence
};

#endif
