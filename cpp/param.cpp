// ***********************************************************************
// Parameters management library implementation
// ***********************************************************************

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "param.h"

//------------------------------------------------------------------------
// Global variables and constants
//------------------------------------------------------------------------
const char delimiters[] = " \t;#=[]\"";
const char blanks[] = " \t#";
int CList::prefixlen = 0;
char* CList::prefixstr = "";
bool CParameter::m_expandmacros = true;

//------------------------------------------------------------------------
// Global class instances
//------------------------------------------------------------------------
CSection par_root, par_macros;

//------------------------------------------------------------------------
// Implementation of member functions
//------------------------------------------------------------------------

// CList class constructor/destructor
CList::CList(char* name)
{
  m_next = m_last = NULL;

  if( name )
    strncpy(m_name, name, MAXPARNAMELEN),
    m_name[MAXPARNAMELEN-1] = 0;
  else m_name[0] = 0;
}

CList::~CList()
{
  Remove();
}

// CParameter class constructor/destructor
CParameter::CParameter(char* name, char* value) : CList(name)
{
  m_value = NULL;
  if( value ) Set(value);
}

CParameter::~CParameter()
{
  free( m_value );
  Remove();
}

// CSection class constructor/destructor
CSection::CSection(char* name) : CList(name) {}

CSection::~CSection()
{
  Clear();
  Remove();
}

// Find an element with given name
CList* CList::Find(char* parname)
{
  CList *p;
  for(p=this; p; p=p->m_next)
    if( strcmp(p->m_name, parname) == 0 ) break;

  return p;
}

CParameter* CParameter::Find(char* parname)
{
  if( *parname ) return (CParameter*) CList::Find(parname);
  else return NULL;
}


// Find a place for a new elemment
// Return: true - element already exists
bool CList::FindPlace(CList*& p)
{
  char *name = p->m_name;
  int res = 1;
  p=this;
  // while( (res = strcmp(name, p->m_name)) > 0 ) // (Sorting version)
  while( (res = strcmp(name, p->m_name)) != 0 )
  {
    if( p->m_next == NULL ) break;
    p=p->m_next;
  }

  // if(res < 0) p = p->m_last; // (Sorting version)
  return !res;
}


// Insert an element into a chain after this
CList* CList::Insert(CList* pnew)
{
  pnew->m_next = m_next;
  pnew->m_last = this;
  if(m_next) m_next->m_last = pnew;
  m_next = pnew;
  return pnew;
}

// Insert/Replace an element at an appropriate position in chain
CParameter* CParameter::Insert(CParameter* pnew)
{
  CParameter *par = pnew;
  if( FindPlace((CList*&)par) )
  {
    if( pnew->m_value ) par->Set(pnew->m_value);
    delete pnew;
    return par;
  }
  return (CParameter*) par->CList::Insert(pnew);
}

// Insert/Replace an element in appropriate place in chain
CSection* CSection::Insert(CSection* snew)
{
  CSection *sec = snew;
  if( FindPlace((CList*&)sec) )
  {
    delete snew;
    return sec;
  }
  return (CSection*) sec->CList::Insert(snew);
}

// Extract the name of a macro from the string given
void CParameter::GetMacroName(char* macro, char* str)
{
  if( *str == '(' )
  {
    str++;
    unsigned mlen = strcspn(str, ")");
    if( mlen > 0 && mlen < strlen(str) )
      strncpy(macro, str, mlen), macro[mlen] = 0;
    else macro[0] = 0;
  }      
  else macro[0] = str[0], macro[1] = 0;
}

// Occupy space and store a new value (looking for macros)
void CParameter::Set(char* value)
{
  // Calculate additional size due to macros
  int i, len = strlen(value), addsize = 0;
  char macro[MAXPARNAMELEN];
  bool dollar = false;

  if( m_expandmacros )
  {
    for(i=0; i<len; i++)
      if( value[i] == '$' )
      {
        dollar = true;

        if( value[++i] == '$' ) continue;
        GetMacroName(macro, value+i);
        if( !*macro ) 
        {
          printf("****** Illegal macro usage in\n%s = %s\n", m_name, value);
          exit(1);
        }

        CParameter *par = par_macros.m_plist.Find(macro);
        if( !par ) 
        {
          printf("****** Undefined macro: $(%s) in\n%s = %s\n",
            macro, m_name, value);
          exit(1);
        }
        addsize += strlen( par->m_value );
      }
  }

  if( m_value ) free( m_value );
  m_value = (char*)malloc(strlen(value) + addsize + 1);

  if( dollar )
  {
    char *p = m_value;
    for(i=0; i<=len; i++)
    {
      if( value[i] == '$' )
        if( value[++i] != '$' )
        {     
          GetMacroName(macro, value+i);
          if( macro[1] ) i += strlen(macro) + 1;

          char *mval = par_macros.m_plist.Find(macro)->m_value;
          strcpy(p, mval);
          p += strlen(mval);
          continue;
        }
      *(p++) = value[i];
    }
  }
  else strcpy(m_value, value);
}


// Locate parameters using "section.parameter" name
// (to be called from par_root)
CParameter* CSection::LocatePar(char* name)
{
  if( !name ) return NULL;

  char secname[MAXPARNAMELEN], parname[MAXPARNAMELEN];
  SplitName(name, secname, parname);

  if( !*parname ) strcpy(parname, secname), *secname = 0;

  CSection *sec = Find(secname);
  if( !sec ) return NULL;

  return sec->m_plist.Find(parname);
}


// Create/replace parameter and section using "section.parameter" name
// (to be called from par_root)
CParameter* CSection::CreatePar(char* name)
{
  if( !name ) return NULL;

  char secname[MAXPARNAMELEN], parname[MAXPARNAMELEN];
  SplitName(name, secname, parname);
  if( !*parname ) return NULL;

  CSection *sec = Insert( new CSection(secname) );
  if( !sec ) return NULL;

  return sec->m_plist.Insert( new CParameter(parname) );
}


// Split "section.parameter" name
// (static function)
void CSection::SplitName(char* name, char* secname, char* parname)
{
  parname[0] = secname[0] = 0;
  if( !name || !*name ) return;
  
  // Find section
  unsigned i = strcspn(name, ".");
  if( i >= MAXPARNAMELEN ) i = MAXPARNAMELEN - 1;
  strncpy(secname, name, i);
  secname[i] = 0;
  
  if( i + 1 >= strlen(name) ) return;
  strncpy(parname, name+i+1, MAXPARNAMELEN);
  parname[MAXPARNAMELEN-1] = 0;
}


// Removes an element from chain
void CList::Remove()
{
  if( m_last ) m_last->m_next = m_next;
  if( m_next ) m_next->m_last = m_last;
}


// Remove all parameters from section
void CSection::Clear(void)
{
  while( !isEmpty() ) delete m_plist.m_next;
}


// Save parameter in the buffer given
int CParameter::Save(char*& buf)
{
  bool quotation = ( strcspn(m_value, blanks) < strlen(m_value) &&
                     m_value[0] != '\"' );
  
  int size = strlen(m_name) + strlen(m_value) + (quotation ? 6 : 4)
             + prefixlen;
  
  if(buf)
  {
    sprintf( buf, quotation ? "%s%s = \"%s\"\n" : "%s%s = %s\n",
             prefixstr, m_name, m_value );
    buf += size;
  }

  return size;
}

// Save section name in the buffer given
int CSection::SaveTitle(char*& buf)
{
  int size = strlen(m_name) + 3 + prefixlen;

  if(buf)
  {
    sprintf(buf, "%s[%s]\n", prefixstr, m_name );
    buf += size;
  }

  return size;
}

// Save section name and parameters in the buffer given
int CSection::Save(char*& buf)
{
  int size = SaveTitle(buf);

  for( CParameter *par = m_plist.next(); par; par=par->next() )
    size += par->Save(buf);

  return size;
}

//------------------------------------------------------------------------
// User interface function
//------------------------------------------------------------------------

//
// Enable or disable macros expansion
//
void SetExpandMacros(bool expand_macros)
{
  CParameter::m_expandmacros = expand_macros;
}

//
// Make istream object from file and call ParseStream
//
void ParseFile(char* filename)
{
  ifstream instr(filename);
  if( !instr.is_open() )
  {
    printf("****** Error opening file: %s\n", filename);
    exit(1);
  }
  ParseStream(instr);
  instr.close();
}

void ParseBuf(char *buf)
{
  if( !buf || !*buf ) return;
  stringstream instr(buf, ios_base::in);
  ParseStream( instr );
}

//
// Parse text buffer and append parameters and sections to par_root
//
void ParseStream(istream& instr)
{
  int i, j;
  static char line[MAXLINELEN];
  CSection* cursec = &par_root;

  while( !instr.eof() )
  {
    int parsed = 0;
    
    // Copy next line into the buffer
    instr.getline(line, MAXLINELEN);

    // Skip leading spaces and "#" symbols
    char* name = line + strspn(line, blanks);
    
    // Look for section
    if( *name == '[' )
    {
      // Extract section name
      i = strcspn(++name, delimiters);
      if( name[i] == ']' )
      {
        // Change/Insert section
        name[i] = 0;
        cursec = par_root.Insert( new CSection(name) );
        parsed++;
      }
    }
    // Look for parameter 
    if( *name == 0 || *name == ';' ) parsed++;
    else
    {
      // Extract parameter name
      i = strcspn(name, delimiters);
      j = i + strspn(name+i, blanks);
      if( name[j] == '=' && i>0 )
      {
        // Extract value
        char *value = name + j + 1;
        value += strspn(value, blanks);
        if( value[0] == '\"' ) j = strcspn(++value, "\"");
        else j = strcspn(value, delimiters);
        
        if( j>=0 )
        {
          // Change/Insert parameter
          name[i] = value[j] = 0;
          if( name[0] == '$' && CParameter::m_expandmacros )
            par_macros.m_plist.Insert( new CParameter(name+1, value) );
          else
            cursec->m_plist.Insert( new CParameter(name, value) );
          parsed++;
        }
      }
    }

    if( !parsed )
    {
      printf("****** Error parsing parameters in line:\n%s\n", line);
      exit(1);
    }
  }
}

//
// Calculate the size of the buffer required for function SavePar
// (see below)
//
int GetBufSize(char* list, bool prefix)
{
  return SaveBuf(list, NULL, prefix);
}

//
// Store all the parameters into a buffer
// Input:
//   buf=0 - only required size is calculated
//   name  - names of section and parameters that should be saved
//           separated by ";". All is saved if name="".
// Return: used buffer size
//
int SaveBuf(char* list, char* buf, bool prefix)
{
  int size = 0;

  CList::prefixlen = (prefix ? 2 : 0);
  CList::prefixstr = (prefix ? (char*)"# " : (char*)"");

  if( *list )
  {
    // Save selected sections or parameters
    int i, j=0;
    while( list[j] )
    {
      char name[MAXPARNAMELEN];
      char secname[MAXPARNAMELEN], parname[MAXPARNAMELEN];
      i = j;
      j += strcspn(list+j, ";");

      // Extract current parameter and section names
      strncpy(name, list+i, j-i);
      name[j-i] = 0;
      if( list[j] ) j++;

      par_root.SplitName(name, secname, parname);
  
      CSection *sec = par_root.Find(secname);
      if( !sec ) continue;
  
      // Save parameter or section
      if( *parname )
      {
        CParameter* par = sec->m_plist.Find(parname);
        if( par )
          size += sec->SaveTitle(buf),
          size += par->Save(buf);
      }
      else size += sec->Save(buf);
    }
  }
  else
    for(CSection *sec = &par_root; sec; sec=sec->next())
      size += sec->Save(buf);

  if(buf) *buf = 0;

  return size + 1;
}

//
// Remove section or parameter from list
//
void RemovePar(char* name)
{
  char secname[MAXPARNAMELEN], parname[MAXPARNAMELEN];
  par_root.SplitName(name, secname, parname);

  if( !*name )
  {
    while( par_root.next() ) delete par_root.next();
    par_root.Clear();
    return;
  }

  CSection *sec = par_root.Find(secname);
  if( !sec ) return;
  if( *parname )
  {
    CParameter* par = sec->m_plist.Find(parname);
    if( par ) delete par;
    if( !sec->m_plist.next() && sec != &par_root ) delete sec;
  }
  else
  {
    if( sec == &par_root ) sec->Clear();
    else delete sec;
  }
}

//
// Copy one section or parameter to another
//
void CopyPar(char* dstname, char* srcname)
{
  char srcsecname[MAXPARNAMELEN], srcparname[MAXPARNAMELEN],
       dstsecname[MAXPARNAMELEN], dstparname[MAXPARNAMELEN];

  par_root.SplitName(srcname, srcsecname, srcparname);
  par_root.SplitName(dstname, dstsecname, dstparname);

  // Look for source section
  CSection *srcsec = par_root.Find(srcsecname);
  if( !srcsec ) return;

  // Look for source parameter
  CParameter *dstpar, *srcpar;
  if( *srcparname )
  {
    srcpar = srcsec->m_plist.Find(srcparname);
    if( !srcpar ) return;
  }

  // Ensure that destination section exists
  CSection *dstsec = par_root.Insert( new CSection(dstsecname) );

  if( *srcparname )
  {
    // Copy parameter
    if( *dstparname == 0 ) strcpy(dstparname, srcparname);
    dstpar = new CParameter( dstparname, srcpar->m_value );
    dstsec->m_plist.Insert( dstpar );
  }
  else
  {
    // Copy section
    for( srcpar = srcsec->m_plist.next(); srcpar; srcpar=srcpar->next() )
    {
      dstpar = new CParameter( srcpar->m_name, srcpar->m_value );
      dstsec->m_plist.Insert( dstpar );
    }
  }
}

//
// Change name of section or parameter
//
void MovePar(char* dstname, char* srcname)
{
  CopyPar(dstname, srcname);
  RemovePar(srcname);
}

//
// Check whether section or parameter exists
//
bool ExistsPar(char* name)
{
  char secname[MAXPARNAMELEN], parname[MAXPARNAMELEN];
  par_root.SplitName(name, secname, parname);

  CSection *sec = par_root.Find(secname);
  if( !sec ) return false;
  
  if( *parname ) return !!sec->m_plist.Find(parname);
  return true;
}

//
// Get parameter value as integer
//
int GetIntPar(char* name)
{
  CParameter* par = par_root.LocatePar( name );
  if( par ) return atoi(par->m_value);
  return 0;
}

//
// Get parameter value as double
//
double GetDblPar(char* name)
{
  CParameter* par = par_root.LocatePar( name );
  if( par ) return atof(par->m_value);
  return 0.;
}

//
// Get parameter value as string
//
void GetStrPar(char* name, char* value)
{
  CParameter* par = par_root.LocatePar( name );
  if( par ) strcpy(value, par->m_value);
  else value[0] = 0;
}

//
// Compare parameter value with "yes"
//
bool GetBoolPar(char* name)
{
  CParameter* par = par_root.LocatePar( name );
  if( !par ) return false;
  
  return !strcmp(par->m_value, "yes") || !strcmp(par->m_value, "YES");
}

//
// Check parameter to be equil to the string given
//
bool TestPar(char* name, char* value)
{
  CParameter* par = par_root.LocatePar( name );
  if( !par ) return false;
  
  return !strcmp(par->m_value, value);
}

//
// Set parameter value to the string given
//
void SetStrPar(char* name, char* value)
{
  CParameter* par = par_root.CreatePar( name );
  if( par ) par->Set(value);
}

//
// Set parameter value to the integer given
//
void SetIntPar(char* name, int value)
{
  char str[64];
  sprintf(str, "%d", value);
  SetStrPar(name, str);
}

//
// Set parameter value to the double given
//
void SetDblPar(char* name, double value)
{
  char str[64];
  sprintf(str, "%.8g", value);
  SetStrPar(name, str);
}

//
// Set parameter value to yes/no
//
void SetBoolPar(char* name, bool value)
{
  SetStrPar(name, (char*)(value ? "yes" : "no") );
}
