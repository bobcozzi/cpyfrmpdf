

#define __cplusplus__strings__

#include <stdlib.h>
#include <string.h>

#include <ifs.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <qp0ztrc.h>

#include <triml.mih>
#include <cpybytes.mih>

   // --------------------------------------------------------------
#pragma comment(copyright,"(c) Copyright 2010-2023 by R. Cozzi, Jr. All Rights Reserved.")
   // --------------------------------------------------------------


#define STR_VALUE(arg) #arg
#define addOvrParmqt(kwd,p,d,l) \
         if (argc > p) \
         {             \
           memset(d,0x00,sizeof(d)); \
           _CPYBYTES(d, argv[p],l);  \
           d[ triml(d,' ') ] = 0x00; \
           if (strlen(d) > 0)        \
           {                         \
              sprintf(szParm,"%s(\'%s\') ", STR_VALUE(kwd),d); \
              strcat(szOvrPrtf, szParm); \
           } \
         }

#define addOvrParm(kwd,p,d,l) \
         if (argc > p) \
         {             \
           memset(d,0x00,sizeof(d)); \
           _CPYBYTES(d, argv[p],l);  \
           d[ triml(d,' ') ] = 0x00; \
           if (strlen(d) > 0)        \
           {                         \
              sprintf(szParm,"%s(%s) ", STR_VALUE(kwd),d); \
              strcat(szOvrPrtf, szParm); \
           } \
         }

typedef _Packed struct tagVarChar
{
   short int LEN;
   char      DAT[640];
} varchar_t;

int main(int argc, char* argv[])
{
     int   hPDFStmf;
     FILE* hPDFDev = NULL;

     int  fFlags = O_RDONLY;
     int  dFlags = O_WRONLY;
     int  nFileSize = 0;
     int  count = 0;
     int  bufSize = 0;
     int  byteCount = 0;

     varchar_t*  pPdfStmf;

     // argv[0] = this program's name passed by the C interface automatically
     // argv[1] = stmf     varchar(640)   -- PDF file to be printed
     // argv[2] = outq     char(20) DFT(*JOB)
     // argv[3] = PRTFILE  char(20) DFT(*AUTO)
     // argv[4] = SPLFNAME char(10) DFT(PDFWRITER)
     // argv[5] = USRDTA   char(10) DFT(CPYFRMPDF)
     // argv[6] = HOLD     char(10) DFT(*NO)
     // argv[7] = SAVE     char(10) DFT(*NO)
     // argv[8] = FORMTYPE char(10) DFT(*STD)
     // argv[9] = COPIES   short int DFT(1)
     // argv[10] = Owner   char(10) DFT(*CURRENT)
     // argv[11] = AddlOvr char(640) DFT(*BLANKS)  -- Additional OVRPRTF parameters

         char szBuffer[2048];
         char szOvrPrtf[2048];
         char szParm[640];
         char szPrtf[22];
         char szOutq[22];
         char szHold[11];
         char szSave[11];
         char szForm[11];
         char szOwner[11];
         char szSplf[11];
         char szUsrDta[11];
         char szOvrParms[640];
     unsigned short ccsid;
     struct stat info;

     char szPDF_Stmf[2049];    // PDF stmf name and path on the IFS

     char szFlags[64] = "wb";  // SPOOLED file open flags: "wb" (write/binary)

     unsigned char ioBuffer[4096];
     memset(szFlags,0x00,sizeof(szFlags));
     memset(szPDF_Stmf,0x00,sizeof(szPDF_Stmf));
     memset(ioBuffer,0x00,sizeof(ioBuffer));

     pPdfStmf = (varchar_t*) argv[1];

     memcpy(szPDF_Stmf, pPdfStmf->DAT, pPdfStmf->LEN);

     if ( access(szPDF_Stmf, R_OK) != 0)
     {
         Qp0zLprintf("PDF file not found %s\n",szPDF_Stmf);
     }
     else
     {
       stat(szPDF_Stmf, &info);
       ccsid = info.st_ccsid;

          // Open the IFS stmf (PDF) file for input
       if ((hPDFStmf = open(szPDF_Stmf, fFlags)) < 0)
       {
           Qp0zLprintf("open(%s) Error %d %s\n",szPDF_Stmf,
                        errno, strerror(errno));
       }
       else
       {
         memset(szOvrPrtf,0x00,sizeof(szOvrPrtf));
         memset(szParm,0x00,sizeof(szParm));


         // 23-March-2021 -- Added OVRSCOPE(*JOB) to base OVRPRTF command
         // to match the DLTOVR command at the end of the routine.
         // 18-April-2021 -- Added DEVTYPE(*USERASCII) to base OVRPRTF command
         //                  to enable any printer to receive PDF content.
         char szASCII_PRTF[22] = "COZ_PDFOUT";
         char baseOVR[] = "OVRSCOPE(*JOB) RPLUNPRT(*NO)";

         sprintf(szOvrPrtf,"OVRPRTF %s %s ", szASCII_PRTF, baseOVR);

         _CPYBYTES(szPrtf,argv[3],20);
         if (strnicmp(szPrtf,"*AUTO",5)!=0)
         {
            char prtf[11];
            char lib[11];
            memset(prtf,0x00,sizeof(prtf));
            memset(lib,0x00,sizeof(lib));
            _CPYBYTES(prtf,szPrtf,10);
            _CPYBYTES(lib,szPrtf+10,10);
            prtf[ triml(prtf,' ') ] = 0x00;
            lib[ triml(lib,' ') ] = 0x00;

            if (strlen(lib) == 0)
            {
             sprintf(szParm,"TOFILE(%s) ",prtf);
            }
            else
            {
             sprintf(szParm,"TOFILE(%s/%s) ", lib,prtf);
            }
           strcat(szOvrPrtf,szParm);
         }
         else
         {
              // when a users-supplied printer device file is
              // specified, we do not override teh DEVTYPE parm.
              // However, when they don't pass in a printer file
              // we override the default print file used by
              // the system to UserAscii so that this routine
              // overrides our default ST_PDFOUT device.
           strcat(szOvrPrtf," TOFILE(QSYSPRT) DEVTYPE(*USERASCII) ");
         }

         memset(szOutq,0x00,sizeof(szOutq));
         _CPYBYTES(szOutq,argv[2], 20);
         if (memicmp(szOutq,"*AUTO",5)!=0 && memicmp(szOutq,"*JOB",4)!=0)
         {
            char outq[11];
            char lib[11];
            memset(outq,0x00,sizeof(outq));
            memset(lib,0x00,sizeof(lib));
            _CPYBYTES(outq,szOutq,10);
            _CPYBYTES(lib,szOutq+10,10);
            outq[ triml(outq,' ') ] = 0x00;
            lib[ triml(lib,' ') ] = 0x00;

            if (strlen(lib) == 0)
            {
             sprintf(szParm,"OUTQ(%s)",outq);
            }
            else
            {
             sprintf(szParm,"OUTQ(%s/%s)", lib,outq);
            }
            strcat(szOvrPrtf,szParm);
            strcat(szOvrPrtf," ");
            strcpy(szOutq,szParm);
          }
          else
          {
            szOutq[ triml(szOutq,' ') ] = 0x00;
            sprintf(szParm,"OUTQ(%s)",szOutq);
            strcpy(szOutq,szParm);
          }


         addOvrParm(SPLFNAME,4,szSplf,10);
         addOvrParmqt(USRDTA,5,szUsrDta,10);
         addOvrParm(HOLD,6,szHold,10);
         addOvrParm(SAVE,7,szSave,10);
         addOvrParm(FORMTYPE,8,szForm,10);
         if (argc > 10 && memicmp(argv[10],"*CUR",4)!=0)
         {
           addOvrParm(SPLFOWN,10,szOwner,10);
         }

         if (argc > 9 && *(short*)argv[9] > 0)
         {
            sprintf(szParm,"COPIES(%d) ", *((short*)argv[9]));
            strcat(szOvrPrtf,szParm);
         }

            // RC: 10-AUG-2023 -- Included ADDLPARMS parameter
            //     that allows users to include additional OVRPRTF
            //     parameters more easily.
         if (argc > 11)
         {
           char* pAddlParms = argv[11];
           if (!(*pAddlParms == ' ' || pAddlParms==""))
           {
             sprintf(szParm," %s ", pAddlParms);
             strcat(szOvrPrtf,szParm);
           }
         }


         Qp0zLprintf("%s\n",szOvrPrtf);
         system(szOvrPrtf);

         strcpy(szFlags, "wb");

         // Open/Create the PDF file's target SPOOLED (print) file object.
         if ((hPDFDev = fopen(szASCII_PRTF, szFlags)) == NULL)
         {
            Qp0zLprintf("fopen(\"%s\",\"%s\") Error %d %s\n",
                          szASCII_PRTF,szFlags, errno, strerror(errno));
            close( hPDFStmf );
            fclose( hPDFDev );
         }
         else
         {
                 // If the PDF is < 4k then read the entire file at once.
                 // Otherwise, read it in 4k chunks until it is read
                 // in its entirety.
           if (info.st_size > sizeof(ioBuffer)-1)
           {
              bufSize = sizeof(ioBuffer) - 1;
           }
           else
           {
              bufSize = info.st_size;
           }

                 // Write the raw PDF data to the SPOOLED file
           while ((count = read( hPDFStmf, ioBuffer, bufSize ) ) > 0 )
           {
               fwrite(ioBuffer, 1, count, hPDFDev);
               byteCount += count;
           }
           close( hPDFStmf );
           fflush( hPDFDev );
           fclose( hPDFDev );

           Qp0zLprintf("Copied %d bytes from %s to %s as SPOOLED File %s\n",
                        byteCount,szPDF_Stmf,szOutq,szSplf);
         }
         sprintf(szOvrPrtf, "DLTOVR %s LVL(*JOB)",szASCII_PRTF);
         system(szOvrPrtf);
       }
     }
     return byteCount;
}

 
