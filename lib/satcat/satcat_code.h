/*
 * Copyright (c) 2022-2026 Wojciech Graj
 *
 * Licensed under the MIT license: https://opensource.org/licenses/MIT
 * Permission is granted to use, copy, modify, and redistribute the work.
 * Full license information available in the project LICENSE file.
 **/

#ifndef SATCAT_CODE_H
#define SATCAT_CODE_H

#ifdef _ISOC99_SOURCE
#include <stdint.h>
typedef uint64_t sc_code_t;
#else
#include <limits.h>
#if (UINT_MAX >= 0xFFFFFFFFFFUL)
typedef unsigned sc_code_t;
#elif (ULONG_MAX >= 0xFFFFFFFFFFUL)
typedef unsigned long sc_code_t;
#else
typedef unsigned long long sc_code_t;
#endif
#endif

#define SC_STR5_TO_CODE(s)\
	(sc_code_t)((sc_code_t)(s)[0]\
	| (sc_code_t)(s)[1] << 8\
	| (sc_code_t)(s)[2] << 16\
	| (sc_code_t)(s)[3] << 24\
	| (sc_code_t)(s)[4] << 32)

/* Functions */
const char *sc_status_str(char code);
const char *sc_source_str(sc_code_t code);
const char *sc_launch_site_str(sc_code_t code);

/* Status */
#define SCSTAT_OPERATIONAL '+'
#define SCSTAT_NONOPERATIONAL '-'
#define SCSTAT_PARTIALLY_OPERATIONAL 'P'
#define SCSTAT_BACKUP 'B'
#define SCSTAT_SPARE 'S'
#define SCSTAT_EXTENDED_MISSION 'X'
#define SCSTAT_DECAYED 'D'
#define SCSTAT_UNKNOWN '?'

/* Source */
#define SCSRC_AB    (sc_code_t)137977938497UL
#define SCSRC_ABS   (sc_code_t)137981280833UL
#define SCSRC_AC    (sc_code_t)137977938753UL
#define SCSRC_ALG   (sc_code_t)137980496961UL
#define SCSRC_ANG   (sc_code_t)137980497473UL
#define SCSRC_ARGN  (sc_code_t)138752250433UL
#define SCSRC_ARM   (sc_code_t)137980891713UL
#define SCSRC_ASRA  (sc_code_t)138534867777UL
#define SCSRC_AUS   (sc_code_t)137981285697UL
#define SCSRC_AZER  (sc_code_t)138819230273UL
#define SCSRC_BEL   (sc_code_t)137980822850UL
#define SCSRC_BELA  (sc_code_t)138534470978UL
#define SCSRC_BERM  (sc_code_t)138736190786UL
#define SCSRC_BGD   (sc_code_t)137980299074UL
#define SCSRC_BHR   (sc_code_t)137981216834UL
#define SCSRC_BHUT  (sc_code_t)138853828674UL
#define SCSRC_BOL   (sc_code_t)137980825410UL
#define SCSRC_BRAZ  (sc_code_t)138953183810UL
#define SCSRC_BUL   (sc_code_t)137980826946UL
#define SCSRC_BWA   (sc_code_t)137980106562UL
#define SCSRC_CA    (sc_code_t)137977938243UL
#define SCSRC_CHBZ  (sc_code_t)138953246787UL
#define SCSRC_CHTU  (sc_code_t)138870540355UL
#define SCSRC_CHLE  (sc_code_t)138601580611UL
#define SCSRC_CIS   (sc_code_t)137981282627UL
#define SCSRC_COL   (sc_code_t)137980825411UL
#define SCSRC_CRI   (sc_code_t)137980629571UL
#define SCSRC_CZCH  (sc_code_t)138651327043UL
#define SCSRC_DEN   (sc_code_t)137980953924UL
#define SCSRC_DJI   (sc_code_t)137980627524UL
#define SCSRC_ECU   (sc_code_t)137981412165UL
#define SCSRC_EGYP  (sc_code_t)138786981701UL
#define SCSRC_ESA   (sc_code_t)137980105541UL
#define SCSRC_ESRO  (sc_code_t)138769748805UL
#define SCSRC_EST   (sc_code_t)137981350725UL
#define SCSRC_ETH   (sc_code_t)137980564549UL
#define SCSRC_EUME  (sc_code_t)138601649477UL
#define SCSRC_EUTE  (sc_code_t)138602108229UL
#define SCSRC_FGER  (sc_code_t)138819225414UL
#define SCSRC_FIN   (sc_code_t)137980954950UL
#define SCSRC_FR    (sc_code_t)137977942598UL
#define SCSRC_FRIT  (sc_code_t)138853044806UL
#define SCSRC_GER   (sc_code_t)137981216071UL
#define SCSRC_GHA   (sc_code_t)137980102727UL
#define SCSRC_GLOB  (sc_code_t)138551446599UL
#define SCSRC_GREC  (sc_code_t)138567569991UL
#define SCSRC_GRSA  (sc_code_t)138534933063UL
#define SCSRC_GUAT  (sc_code_t)138852521287UL
#define SCSRC_HRV   (sc_code_t)137981481544UL
#define SCSRC_HUN   (sc_code_t)137980958024UL
#define SCSRC_IM    (sc_code_t)137977941321UL
#define SCSRC_IND   (sc_code_t)137980300873UL
#define SCSRC_INDO  (sc_code_t)138768830025UL
#define SCSRC_IRAN  (sc_code_t)138751857225UL
#define SCSRC_IRAQ  (sc_code_t)138802188873UL
#define SCSRC_IRID  (sc_code_t)138584609353UL
#define SCSRC_IRL   (sc_code_t)137980826185UL
#define SCSRC_ISRA  (sc_code_t)138534867785UL
#define SCSRC_ISRO  (sc_code_t)138769748809UL
#define SCSRC_ISS   (sc_code_t)137981285193UL
#define SCSRC_IT    (sc_code_t)137977943113UL
#define SCSRC_ITSO  (sc_code_t)138769814601UL
#define SCSRC_JPN   (sc_code_t)137980956746UL
#define SCSRC_KAZ   (sc_code_t)137981739339UL
#define SCSRC_KEN   (sc_code_t)137980953931UL
#define SCSRC_LAOS  (sc_code_t)138836656460UL
#define SCSRC_LKA   (sc_code_t)137980103500UL
#define SCSRC_LTU   (sc_code_t)137981416524UL
#define SCSRC_LUXE  (sc_code_t)138602370380UL
#define SCSRC_MA    (sc_code_t)137977938253UL
#define SCSRC_MALA  (sc_code_t)138534469965UL
#define SCSRC_MCO   (sc_code_t)137981018957UL
#define SCSRC_MDA   (sc_code_t)137980101709UL
#define SCSRC_MEX   (sc_code_t)137981609293UL
#define SCSRC_MMR   (sc_code_t)137981218125UL
#define SCSRC_MNE   (sc_code_t)137980366413UL
#define SCSRC_MNG   (sc_code_t)137980497485UL
#define SCSRC_MUS   (sc_code_t)137981285709UL
#define SCSRC_NATO  (sc_code_t)138769875278UL
#define SCSRC_NETH  (sc_code_t)138652435790UL
#define SCSRC_NICO  (sc_code_t)138768763214UL
#define SCSRC_NIG   (sc_code_t)137980496206UL
#define SCSRC_NKOR  (sc_code_t)138819881806UL
#define SCSRC_NOR   (sc_code_t)137981218638UL
#define SCSRC_NPL   (sc_code_t)137980825678UL
#define SCSRC_NZ    (sc_code_t)137977944654UL
#define SCSRC_O3B   (sc_code_t)137980162895UL
#define SCSRC_ORB   (sc_code_t)137980170831UL
#define SCSRC_PAKI  (sc_code_t)138668622160UL
#define SCSRC_PERU  (sc_code_t)138870408528UL
#define SCSRC_POL   (sc_code_t)137980825424UL
#define SCSRC_POR   (sc_code_t)137981218640UL
#define SCSRC_PRC   (sc_code_t)137980236368UL
#define SCSRC_PRY   (sc_code_t)137981678160UL
#define SCSRC_PRES  (sc_code_t)138836005456UL
#define SCSRC_QAT   (sc_code_t)137981346129UL
#define SCSRC_RASC  (sc_code_t)138568483154UL
#define SCSRC_ROC   (sc_code_t)137980235602UL
#define SCSRC_ROM   (sc_code_t)137980890962UL
#define SCSRC_RP    (sc_code_t)137977942098UL
#define SCSRC_RWA   (sc_code_t)137980106578UL
#define SCSRC_SAFR  (sc_code_t)138819289427UL
#define SCSRC_SAUD  (sc_code_t)138585391443UL
#define SCSRC_SDN   (sc_code_t)137980953683UL
#define SCSRC_SEAL  (sc_code_t)138718299475UL
#define SCSRC_SEN   (sc_code_t)137980953939UL
#define SCSRC_SES   (sc_code_t)137981281619UL
#define SCSRC_SGJP  (sc_code_t)138785998675UL
#define SCSRC_SING  (sc_code_t)138635266387UL
#define SCSRC_SKOR  (sc_code_t)138819881811UL
#define SCSRC_SLB   (sc_code_t)137980169299UL
#define SCSRC_SPN   (sc_code_t)137980956755UL
#define SCSRC_STCT  (sc_code_t)138852652115UL
#define SCSRC_SVN   (sc_code_t)137980958291UL
#define SCSRC_SWED  (sc_code_t)138584348499UL
#define SCSRC_SWTZ  (sc_code_t)138954430291UL
#define SCSRC_TBD   (sc_code_t)137980297812UL
#define SCSRC_THAI  (sc_code_t)138667968596UL
#define SCSRC_TMMC  (sc_code_t)138568093012UL
#define SCSRC_TUN   (sc_code_t)137980958036UL
#define SCSRC_TURK  (sc_code_t)138702640468UL
#define SCSRC_UAE   (sc_code_t)137980363093UL
#define SCSRC_UK    (sc_code_t)137977940821UL
#define SCSRC_UKR   (sc_code_t)137981217621UL
#define SCSRC_UNK   (sc_code_t)137980759637UL
#define SCSRC_URY   (sc_code_t)137981678165UL
#define SCSRC_US    (sc_code_t)137977942869UL
#define SCSRC_USBZ  (sc_code_t)138953249621UL
#define SCSRC_VAT   (sc_code_t)137981346134UL
#define SCSRC_VENZ  (sc_code_t)138954032470UL
#define SCSRC_VTNM  (sc_code_t)138735932502UL
#define SCSRC_ZWE   (sc_code_t)137980368730UL

/* Launch Site */
#define SCSITE_AFETR (sc_code_t)353601144385UL
#define SCSITE_AFWTR (sc_code_t)353602324033UL
#define SCSITE_ANDSP (sc_code_t)344994369089UL
#define SCSITE_ALCLC (sc_code_t)289042287681UL
#define SCSITE_BOS   (sc_code_t)137981284162UL
#define SCSITE_CAS   (sc_code_t)137981280579UL
#define SCSITE_DLS   (sc_code_t)137981283396UL
#define SCSITE_ERAS  (sc_code_t)138835743301UL
#define SCSITE_FRGUI (sc_code_t)314963350086UL
#define SCSITE_HGSTR (sc_code_t)353602062152UL
#define SCSITE_JJSLA (sc_code_t)280453401162UL
#define SCSITE_JSC   (sc_code_t)137980236618UL
#define SCSITE_KODAK (sc_code_t)323217542987UL
#define SCSITE_KSCUT (sc_code_t)362207728459UL
#define SCSITE_KWAJ  (sc_code_t)138684749643UL
#define SCSITE_KYMSC (sc_code_t)289160386891UL
#define SCSITE_NSC   (sc_code_t)137980236622UL
#define SCSITE_PLMSC (sc_code_t)289160383568UL
#define SCSITE_RLLB  (sc_code_t)138551250002UL
#define SCSITE_SCSLA (sc_code_t)280453399379UL
#define SCSITE_SEAL  (sc_code_t)138718299475UL
#define SCSITE_SEMLS (sc_code_t)357762418003UL
#define SCSITE_SMTS  (sc_code_t)138836987219UL
#define SCSITE_SNMLP (sc_code_t)344877518419UL
#define SCSITE_SPKII (sc_code_t)314762285139UL
#define SCSITE_SRILR (sc_code_t)353467191891UL
#define SCSITE_SUBL  (sc_code_t)138718369107UL
#define SCSITE_SVOBO (sc_code_t)340414912083UL
#define SCSITE_TAISC (sc_code_t)289160118612UL
#define SCSITE_TANSC (sc_code_t)289160446292UL
#define SCSITE_TYMSC (sc_code_t)289160386900UL
#define SCSITE_UNK   (sc_code_t)137980759637UL
#define SCSITE_VOSTO (sc_code_t)340717162326UL
#define SCSITE_WLPIS (sc_code_t)357712284759UL
#define SCSITE_WOMRA (sc_code_t)280553672535UL
#define SCSITE_WRAS  (sc_code_t)138835743319UL
#define SCSITE_WSC   (sc_code_t)137980236631UL
#define SCSITE_XICLF (sc_code_t)301927188824UL
#define SCSITE_YAVNE (sc_code_t)297667019097UL
#define SCSITE_YSLA  (sc_code_t)138534474585UL
#define SCSITE_YUN   (sc_code_t)137980958041UL

#endif /* SATCAT_CODE_H */
