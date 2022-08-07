/*
 * Copyright (c) 2022 Wojciech Graj
 *
 * Licensed under the MIT license: https://opensource.org/licenses/MIT
 * Permission is granted to use, copy, modify, and redistribute the work.
 * Full license information available in the project LICENSE file.
 **/

#include "satcat_code.h"

#include <stddef.h>

const char *sc_status_str(const char code)
{
	switch(code) {
	case SCSTAT_OPERATIONAL:
		return "Operational";
	case SCSTAT_NONOPERATIONAL:
		return "Nonoperational";
	case SCSTAT_PARTIALLY_OPERATIONAL:
		return "Partially Operational";
	case SCSTAT_BACKUP:
		return "Backup/Standby";
	case SCSTAT_SPARE:
		return "Spare";
	case SCSTAT_EXTENDED_MISSION:
		return "Extended Mission";
	case SCSTAT_DECAYED:
		return "Decayed";
	case SCSTAT_UNKNOWN:
		return "Unknown";
	default:
		return NULL;
	}
}

const char *sc_source_str(const sc_code_t code){
	switch (code) {
	case SCSRC_AB  :
		return "Arab Satellite Communications Organization";
	case SCSRC_ABS :
		return "Asia Broadcast Satellite";
	case SCSRC_AC  :
		return "Asia Satellite Telecommunications Company (ASIASAT)";
	case SCSRC_ALG :
		return "Algeria";
	case SCSRC_ANG :
		return "Angola";
	case SCSRC_ARGN:
		return "Argentina";
	case SCSRC_ASRA:
		return "Austria";
	case SCSRC_AUS :
		return "Australia";
	case SCSRC_AZER:
		return "Azerbaijan";
	case SCSRC_BEL :
		return "Belgium";
	case SCSRC_BELA:
		return "Belarus";
	case SCSRC_BERM:
		return "Bermuda";
	case SCSRC_BGD :
		return "Peoples Republic of Bangladesh";
	case SCSRC_BHUT:
		return "Kingdom of Bhutan";
	case SCSRC_BOL :
		return "Bolivia";
	case SCSRC_BRAZ:
		return "Brazil";
	case SCSRC_BUL:
		return "Bulgaria";
	case SCSRC_CA  :
		return "Canada";
	case SCSRC_CHBZ:
		return "China/Brazil";
	case SCSRC_CHLE:
		return "Chile";
	case SCSRC_CIS :
		return "Commonwealth of Independent States (former USSR)";
	case SCSRC_COL :
		return "Colombia";
	case SCSRC_CRI :
		return "Republic of Costa Rica";
	case SCSRC_CZCH:
		return "Czech Republic (former Czechoslovakia)";
	case SCSRC_DEN :
		return "Denmark";
	case SCSRC_ECU :
		return "Ecuador";
	case SCSRC_EGYP:
		return "Egypt";
	case SCSRC_ESA :
		return "European Space Agency";
	case SCSRC_ESRO:
		return "European Space Research Organization";
	case SCSRC_EST:
		return "Estonia";
	case SCSRC_EUME:
		return "European Organization for the";
	case SCSRC_EUTE:
		return "European Telecommunications Satellite Organization (EUTELSAT)";
	case SCSRC_FGER:
		return "France/Germany";
	case SCSRC_FIN :
		return "Finland";
	case SCSRC_FR  :
		return "France";
	case SCSRC_FRIT:
		return "France/Italy";
	case SCSRC_GER :
		return "Germany";
	case SCSRC_GHA :
		return "Republic of Ghana";
	case SCSRC_GLOB:
		return "Globalstar";
	case SCSRC_GREC:
		return "Greece";
	case SCSRC_GRSA:
		return "Greece/Saudi Arabia";
	case SCSRC_GUAT:
		return "Guatemala";
	case SCSRC_HUN :
		return "Hungary";
	case SCSRC_IM  :
		return "International Mobile Satellite Organization (INMARSAT)";
	case SCSRC_IND :
		return "India";
	case SCSRC_INDO:
		return "Indonesia";
	case SCSRC_IRAN:
		return "Iran";
	case SCSRC_IRAQ:
		return "Iraq";
	case SCSRC_IRID:
		return "Iridium";
	case SCSRC_ISRA:
		return "Israel";
	case SCSRC_ISRO:
		return "Indian Space Research Organisation";
	case SCSRC_ISS :
		return "International Space Station";
	case SCSRC_IT  :
		return "Italy";
	case SCSRC_ITSO:
		return "International Telecommunications Satellite Organization (INTELSAT)";
	case SCSRC_JPN :
		return "Japan";
	case SCSRC_KAZ :
		return "Kazakhstan";
	case SCSRC_KEN :
		return "Republic of Kenya";
	case SCSRC_LAOS:
		return "Laos";
	case SCSRC_LKA:
		return "Democratic Socialist Republic of Sri Lanka";
	case SCSRC_LTU :
		return "Lithuania";
	case SCSRC_LUXE:
		return "Luxembourg";
	case SCSRC_MA  :
		return "Morroco";
	case SCSRC_MALA:
		return "Malaysia";
	case SCSRC_MEX :
		return "Mexico";
	case SCSRC_MMR :
		return "Republic of the Union of Myanmar";
	case SCSRC_MNG :
		return "Mongolia";
	case SCSRC_MUS :
		return "Mauritius";
	case SCSRC_NATO:
		return "North Atlantic Treaty Organization";
	case SCSRC_NETH:
		return "Netherlands";
	case SCSRC_NICO:
		return "New ICO";
	case SCSRC_NIG :
		return "Nigeria";
	case SCSRC_NKOR:
		return "Democratic People\'s Republic of Korea";
	case SCSRC_NOR :
		return "Norway";
	case SCSRC_NPL :
		return "Federal Democratic Republic of Nepal";
	case SCSRC_NZ  :
		return "New Zealand";
	case SCSRC_O3B :
		return "O3b Networks";
	case SCSRC_ORB :
		return "ORBCOMM";
	case SCSRC_PAKI:
		return "Pakistan";
	case SCSRC_PERU:
		return "Peru";
	case SCSRC_POL :
		return "Poland";
	case SCSRC_POR :
		return "Portugal";
	case SCSRC_PRC :
		return "People\'s Republic of China";
	case SCSRC_PRY :
		return "Republic of Paraguay";
	case SCSRC_PRES:
		return "People\'s Republic of China/";
	case SCSRC_QAT :
		return "State of Qatar";
	case SCSRC_RASC:
		return "RascomStar-QAF";
	case SCSRC_ROC :
		return "Taiwan (Republic of China)";
	case SCSRC_ROM :
		return "Romania";
	case SCSRC_RP  :
		return "Philippines (Republic of the Philippines)";
	case SCSRC_RWA :
		return "Republic of Rwanda";
	case SCSRC_SAFR:
		return "South Africa";
	case SCSRC_SAUD:
		return "Saudi Arabia";
	case SCSRC_SDN :
		return "Republic of Sudan";
	case SCSRC_SEAL:
		return "Sea Launch";
	case SCSRC_SES:
		return "SES";
	case SCSRC_SGJP:
		return "Singapore/Japan";
	case SCSRC_SING:
		return "Singapore";
	case SCSRC_SKOR:
		return "Republic of Korea";
	case SCSRC_SPN :
		return "Spain";
	case SCSRC_STCT:
		return "Singapore/Taiwan";
	case SCSRC_SVN:
		return "Slovenia";
	case SCSRC_SWED:
		return "Sweden";
	case SCSRC_SWTZ:
		return "Switzerland";
	case SCSRC_TBD :
		return "To Be Determined";
	case SCSRC_THAI:
		return "Thailand";
	case SCSRC_TMMC:
		return "Turkmenistan/Monaco";
	case SCSRC_TUN :
		return "Republic of Tunisia";
	case SCSRC_TURK:
		return "Turkey";
	case SCSRC_UAE :
		return "United Arab Emirates";
	case SCSRC_UK  :
		return "United Kingdom";
	case SCSRC_UKR :
		return "Ukraine";
	case SCSRC_UNK :
		return "Unknown";
	case SCSRC_URY :
		return "Uruguay";
	case SCSRC_US  :
		return "United States";
	case SCSRC_USBZ:
		return "United States/Brazil";
	case SCSRC_VENZ:
		return "Venezuela";
	case SCSRC_VTNM:
		return "Vietnam";
	default:
		return NULL;
	}
}

const char *sc_launch_site_str(const sc_code_t code){
	switch (code) {
	case SCSITE_AFETR:
		return "Air Force Eastern Test Range, Florida, USA";
	case SCSITE_AFWTR:
		return "Air Force Western Test Range, California, USA";
	case SCSITE_CAS  :
		return "Canaries Airspace";
	case SCSITE_DLS  :
		return "Dombarovskiy Launch Site, Russia";
	case SCSITE_ERAS :
		return "Eastern Range Airspace";
	case SCSITE_FRGUI:
		return "Europe\'s Spaceport, Kourou, French Guiana";
	case SCSITE_HGSTR:
		return "Hammaguira Space Track Range, Algeria";
	case SCSITE_JSC  :
		return "Jiuquan Space Center, PRC";
	case SCSITE_KODAK:
		return "Kodiak Launch Complex, Alaska, USA";
	case SCSITE_KSCUT:
		return "Uchinoura Space Center";
	case SCSITE_KWAJ :
		return "US Army Kwajalein Atoll (USAKA)";
	case SCSITE_KYMSC:
		return "Kapustin Yar Missile and Space Complex, Russia";
	case SCSITE_NSC:
		return "Naro Space Complex, Republic of Korea";
	case SCSITE_PLMSC:
		return "Plesetsk Missile and Space Complex, Russia";
	case SCSITE_RLLB :
		return "Rocket Lab Launch Base, Mahia Peninsula, New Zealand";
	case SCSITE_SEAL :
		return "Sea Launch Platform (mobile)";
	case SCSITE_SEMLS:
		return "Semnan Satellite Launch Site, Iran";
	case SCSITE_SMTS:
		return "Shahrud Missile Test Site, Iran";
	case SCSITE_SNMLP:
		return "San Marco Launch Platform, Indian Ocean (Kenya)";
	case SCSITE_SRILR:
		return "Satish Dhawan Space Centre, India";
	case SCSITE_SUBL:
		return "Submarine Launch Platform (mobile)";
	case SCSITE_SVOBO:
		return "Svobodnyy Launch Complex, Russia";
	case SCSITE_TAISC:
		return "Taiyuan Space Center, PRC";
	case SCSITE_TANSC:
		return "Tanegashima Space Center, Japan";
	case SCSITE_TYMSC:
		return "Tyuratam Missile and Space Center, Kazakhstan";
	case SCSITE_UNK:
		return "Unknown";
	case SCSITE_VOSTO:
		return "Vostochny Cosmodrome, Russia";
	case SCSITE_WLPIS:
		return "Wallops Island, Virginia, USA";
	case SCSITE_WOMRA:
		return "Woomera, Australia";
	case SCSITE_WRAS:
		return "Western Range Airspace";
	case SCSITE_WSC:
		return "Wenchang Satellite Launch Site, PRC";
	case SCSITE_XICLF:
		return "Xichang Launch Facility, PRC";
	case SCSITE_YAVNE:
		return "Yavne Launch Facility, Israel";
	case SCSITE_YUN:
		return "Yunsong Launch Site";
	default:
		return NULL;
	}
}

