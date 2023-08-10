 CPYFRMPDF:  CMD        PROMPT('COZZI - Copy PDF to Printer') MAXPOS(2)

 PDFILE:     PARM       KWD(STMF) TYPE(*PNAME) LEN(640) +
                          EXPR(*YES) VARY(*YES) MIN(1) +
                          PROMPT('PDF location and file name')
 OUTQ:       PARM       KWD(OUTQ) TYPE(OUTQUAL) DFT(*JOB) +
                          SNGVAL((*JOB)) +
                          PROMPT('Output queue')
 OUTQUAL:    QUAL       TYPE(*NAME) LEN(10) EXPR(*YES)
             QUAL       TYPE(*NAME) LEN(10) DFT(*LIBL) +
                          SPCVAL((*LIBL)) EXPR(*YES) +
                          PROMPT('Library')
 ASCIIPRN:   PARM       KWD(TOFILE) TYPE(DEVQUAL) DFT(*AUTO) +
                          SNGVAL((*AUTO)) PROMPT('*USERASCII +
                          Print device file')
 DEVQUAL:    QUAL       TYPE(*NAME) LEN(10) EXPR(*YES)
             QUAL       TYPE(*NAME) LEN(10) DFT(*LIBL) +
                          SPCVAL((*LIBL)) EXPR(*YES) PROMPT('Library')
 SPLFNAME:   PARM       KWD(SPLFNAME) TYPE(*NAME) LEN(10) +
                          DFT(PDFWRITER) EXPR(*YES) PROMPT('SPOOL +
                          File name')
 USRDATA:    PARM       KWD(USRDTA) TYPE(*CHAR) LEN(10) DFT(CPYFRMPDF) +
                          SPCVAL((CPYFRMPDF)) PROMPT('User Data')
 HOLDSPLF:   PARM       KWD(HOLD) TYPE(*CHAR) LEN(10) RSTD(*YES) +
                          DFT(*NO) SPCVAL((*YES) (*NO)) PROMPT('Hold')
 SAVESPLF:   PARM       KWD(SAVE) TYPE(*CHAR) LEN(10) RSTD(*YES) +
                          DFT(*NO) SPCVAL((*YES) (*NO)) +
                          PROMPT('Save after printing')
 FORMS:      PARM       KWD(FORMTYPE) TYPE(*CHAR) LEN(10) DFT(*STD) +
                          SPCVAL((*STD)) PROMPT('Form type')
 COPIES:     PARM       KWD(COPIES) TYPE(*INT2) DFT(1) RANGE(1 255) +
                          EXPR(*YES) PROMPT('Copies')
 OWNER:      PARM       KWD(SPLFOWN) TYPE(*NAME) LEN(10) RSTD(*YES) +
                          DFT(*CURUSRPRF) SPCVAL((*CURUSRPRF) +
                          (*JOB) (*CURGRPPRF) (*JOBGRPPRF)) +
                          EXPR(*YES) PROMPT('SPOOL file owner')
 ADDLOVR:    PARM       KWD(ADDLOVR) TYPE(*CHAR) LEN(640) EXPR(*YES) +
                          PROMPT('Additional OVRPRTF parameters')
 
