#ifndef __SOFTWARE_CRC_H_
#define __SOFTWARE_CRC_H_

#ifdef __cplusplus
extern "C" {
#endif

//
// CRC checks needed by Safety
void doTrimaFileCrcCheck (void);
void doSafetyFilesCrcCheck (void);
void doMachineFileCrcCheck (void);


//
// Control only CRC checks
//
void doGuiFileCrcCheck (void);
void doProcFileCrcCheck (void);

#ifdef __cplusplus
};
#endif

#endif

/* FORMAT HASH 87562e2c7665f96bf4fd5d6ada19187a */
