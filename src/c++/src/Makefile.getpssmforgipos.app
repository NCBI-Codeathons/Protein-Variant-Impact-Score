# $Id: Makefile.assayHeatmap.cgi.app,v 1.1 2006/11/08 16:54:52 wangjiy Exp $
#################################

REQUIRES = PLATFORM_DB

APP = getpssmforgipos

SRC = pssmCgi
	
#LOCALDIR = /home/wangjiy/c++/src/internal/structure/PubChem/MmdbSrv_v1

CPPFLAGS = $(ORIG_CPPFLAGS) $(LIBXML_INCLUDE) $(LIBXSLT_INCLUDE) \
  $(PLATFORM_DB_SYMBOLS) \
  $(NCBI_C_INCLUDE) \
  -I$(top_srcdir)/src/internal/structure/shdb \
  -I$(srcdir)

#  -I$(top_srcdir)/src/internal/structure/DBLibs/MmdbSrv \
#  -I$(LOCALDIR) \

CFLAGS = -O

#PRE_LIBS =  $(LOCALDIR)/libpubstruct.a

LIB = shdb mmdb \
  seqdb seqset seq seqcode pubmed pub medline biblio general \
  $(PLATFORM_DB_LIB) \
  entrez2 entrez2cli xconnserv xcgi xhtml xcompress xconnect xconnext xser xutil xncbi \
  $(GENBANK_READER_LDEP) \
  $(LMDB_LIB) \
  $(Z_LIB)

NCBI_C_LIBS = -lncbi
LIB_OBJS = -lm -lc

LIBS =  $(ORIG_LIBS) $(LIBXSLT_LIBS) $(LIBXML_LIBS) \
  $(PLATFORM_DB_LIBS) $(LMDB_LIBS) \
  $(LIB_OBJS) $(NCBI_C_LIBPATH) $(NCBI_C_LIBS) \
  $(Z_LIBS) $(DL_LIBS) $(NETWORK_LIBS) $(BZ2_LIBS) \
  $(LIBXML_STATIC_LIBS)

#-ljsoncpp-static 
