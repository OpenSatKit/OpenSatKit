/*
 * File:
 *   $Id: $ 
 *
 * Purpose: This file contains a unit test cases for the EXPAT XML parser. Currently
 *          these server as examples rather than unit tests.
 * 
 * $Date: $
 * $Revision: $
 * $Log: $
 */

/*
 * Includes
 */

#include "common_types.h"
#include "utassert.h"
#include "uttest.h"

#include <expat.h>

#if defined(__amigaos__) && defined(__USE_INLINE__)
#include <expat.h>
#endif

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define BUFFSIZE        8192



/*
 * "Outline" Test Function Definitions
 */

char Buff[BUFFSIZE];
int Depth = 0;

static void XMLCALL
start(void *data, const char *el, const char **attr)
{
  int i;

  for (i = 0; i < Depth; i++)
    printf("  ");

  printf("%s", el);

  for (i = 0; attr[i]; i += 2) {
    printf(" %s='%s'", attr[i], attr[i + 1]);
  }

  printf("\n");
  Depth++;
}

static void XMLCALL
end(void *data, const char *el)
{
  Depth--;
}



/*
 * Test Function Definitions
 */

/* EXPAT_Test01  - Outline Example */
void EXPAT_Test01(void)
{

   FILE*   fp;
   boolean done;
   int     len;

   printf("EXPAT_Test01(void)\n");

   XML_Parser p = XML_ParserCreate(NULL);
   printf("EXPAT_Test01(void): After XML_ParserCreate()\n");
   if (! p) {
      fprintf(stderr, "Couldn't allocate memory for parser\n");
   }
   else {

      XML_SetElementHandler(p, start, end);
      printf("EXPAT_Test01(void): After XML_SetElementHandler()\n");

      fp = fopen("sch-tbl.xml","r");

      if (fp != NULL) {

         done = FALSE;

         while (!done) {

            len = (int)fread(Buff, 1, BUFFSIZE, fp);
            if (ferror(fp)) {
               fprintf(stderr, "Read error\n");
               done = TRUE;
            }
            else {

               done = feof(fp);

               if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {

                  fprintf(stderr, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
                  XML_GetCurrentLineNumber(p),
                  XML_ErrorString(XML_GetErrorCode(p)));
                  done = TRUE;

               } /* End if valid parse */
            } /* End if valid fread */

         } /* End file read loop */
         fclose(fp);

      } /* End if file opened */

      XML_ParserFree(p);

   } /* end if parser allocated */

} /* End EXPAT_Test01() */




/******************************************************************************
 *
 * EXPAT Management Function
 */
void EXPAT_Parser(char* FileName,
                  XML_StartElementHandler StartElementHandler,
                  XML_EndElementHandler   EndElementHandler)
{

   FILE*   fp;
   boolean done;
   int     len;

   printf("EXPAT_Parser()\n");

   XML_Parser p = XML_ParserCreate(NULL);
   if (! p) {
      fprintf(stderr, "Couldn't allocate memory for parser\n");
   }
   else {

      XML_SetElementHandler(p, StartElementHandler, EndElementHandler);

      fp = fopen(FileName,"r");

      if (fp != NULL) {

         done = FALSE;

         while (!done) {

            len = (int)fread(Buff, 1, BUFFSIZE, fp);
            if (ferror(fp)) {
               fprintf(stderr, "Read error\n");
               done = TRUE;
            }
            else {

               done = feof(fp);

               if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {

                  fprintf(stderr, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
                  XML_GetCurrentLineNumber(p),
                  XML_ErrorString(XML_GetErrorCode(p)));
                  done = TRUE;

               } /* End if valid parse */
            } /* End if valid fread */

         } /* End file read loop */
         fclose(fp);

      } /* End if file opened */

      XML_ParserFree(p);

   } /* end if parser allocated */

} /* End EXPAT_Test02() */

/******************************************************************************
 *
 * Prototype for XML scheduler table
 *
 */
#define SCHTBL_XML_EL_SLOT   "slot"
#define SCHTBL_XML_AT_ID     "id"
#define SCHTBL_XML_AT_ENABLE  "enable"
#define SCHTBL_XML_AT_FREQ   "frequency"
#define SCHTBL_XML_AT_MSG_ID "msg_id"
static void XMLCALL
SCHTBL_StartElement(void *data, const char *el, const char **attr)
{
  int i;

  if (strcmp(el,SCHTBL_XML_EL_SLOT) == 0)
  {
     printf("Slot: ");
     for (i = 0; attr[i]; i += 2) {
        if (strcmp(attr[i],SCHTBL_XML_AT_ID)==0) {
           printf("id=%s, ", attr[i + 1]);
        } else if (strcmp(attr[i],SCHTBL_XML_AT_ENABLE)==0)
        {
           printf("enable=%s, ", attr[i + 1]);
        } else if (strcmp(attr[i],SCHTBL_XML_AT_FREQ)==0)
        {
           printf("frequency=%s, ", attr[i + 1]);
        } else if (strcmp(attr[i],SCHTBL_XML_AT_MSG_ID)==0)
        {
           printf("msg_id=%s, ", attr[i + 1]);
        }

     } /* End attribute loop */
     printf("\n");
  } /* End if SCHTBL_XML_EL_SLOT found */

} /* End SCHTBL_StartElement() */

static void XMLCALL
SCHTBL_EndElement(void *data, const char *el)
{

} /* End SCHTBL_EndElement() */

/* EXPAT_Test02  - Scheduler Table  Example */
void EXPAT_Test02(void)
{

   FILE*   fp;
   boolean done;
   int     len;

   printf("EXPAT_Test02(void)\n");

   XML_Parser p = XML_ParserCreate(NULL);
   if (! p) {
      fprintf(stderr, "Couldn't allocate memory for parser\n");
   }
   else {

      XML_SetElementHandler(p, SCHTBL_StartElement, SCHTBL_EndElement);

      fp = fopen("sch-tbl-lab-def.xml","r");

      if (fp != NULL) {

         done = FALSE;

         while (!done) {

            len = (int)fread(Buff, 1, BUFFSIZE, fp);
            if (ferror(fp)) {
               fprintf(stderr, "Read error\n");
               done = TRUE;
            }
            else {

               done = feof(fp);

               if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {

                  fprintf(stderr, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
                  XML_GetCurrentLineNumber(p),
                  XML_ErrorString(XML_GetErrorCode(p)));
                  done = TRUE;

               } /* End if valid parse */
            } /* End if valid fread */

         } /* End file read loop */
         fclose(fp);

      } /* End if file opened */

      XML_ParserFree(p);

   } /* end if parser allocated */

} /* End EXPAT_Test02() */

/******************************************************************************
 *
 * Prototype for XML message table
 *
 */
#define MSGTBL_XML_EL_ENTRY     "entry"
#define MSGTBL_XML_AT_ID        "id"
#define MSGTBL_XML_AT_STREAM_ID "stream-id"
#define MSGTBL_XML_AT_SEQ_SEG   "seq-seg"
#define MSGTBL_XML_AT_MSG_LEN   "length"
static void XMLCALL
MSGTBL_StartElement(void *data, const char *el, const char **attr)
{
  int i;

  if (strcmp(el,MSGTBL_XML_EL_ENTRY) == 0)
  {
     printf("Entry: ");
     for (i = 0; attr[i]; i += 2) {
        if (strcmp(attr[i],MSGTBL_XML_AT_ID)==0) {
           printf("id=%s, ", attr[i + 1]);
        } else if (strcmp(attr[i],MSGTBL_XML_AT_STREAM_ID)==0)
        {
           printf("stream-id=%s, ", attr[i + 1]);
        } else if (strcmp(attr[i],MSGTBL_XML_AT_SEQ_SEG)==0)
        {
           printf("seq-seg=%s, ", attr[i + 1]);
        } else if (strcmp(attr[i],MSGTBL_XML_AT_MSG_LEN)==0)
        {
           printf("length=%s, ", attr[i + 1]);
        }

     } /* End attribute loop */
     printf("\n");
  } /* End if MSGTBL_XML_EL_ENTRY found */

} /* End MSGTBL_StartElement() */

static void XMLCALL
MSGTBL_EndElement(void *data, const char *el)
{

} /* End MSGTBL_EndElement() */

/* EXPAT_Test03  - Message Table  Example */
void EXPAT_Test03(void)
{

   EXPAT_Parser("msg-tbl-lab-def.xml", MSGTBL_StartElement, MSGTBL_EndElement);

} /* End EXPAT_Test03() */

/* EXPAT_TestNN  - Example Scheduler Table */
void EXPAT_TestNN(void)
{

   printf("EXPAT_TestNN(void)\n");

} /* End EXPAT_TestNN() */

void EXPAT_Setup(void)
{
    /* initialize test environment to default state for every test */
}

void EXPAT_TearDown(void)
{
    /* cleanup test environment */
}

void EXPAT_AddTestCase(void)
{
    UtTest_Add(EXPAT_Test01, EXPAT_Setup, EXPAT_TearDown, "EXPAT_Test01 - Outline Example");
    UtTest_Add(EXPAT_Test02, EXPAT_Setup, EXPAT_TearDown, "EXPAT_Test02 - Scheduler Table Example");
    UtTest_Add(EXPAT_Test03, EXPAT_Setup, EXPAT_TearDown, "EXPAT_Test03 - Message Table Example");
    UtTest_Add(EXPAT_TestNN, EXPAT_Setup, EXPAT_TearDown, "EXPAT_TestNN - TBD Description");
}
